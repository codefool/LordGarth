#include "moverule.h"
#include "board.h"

void AxesMoveRule::get_moves( Board* b, PiecePtr pt, MoveList& moves) {
    b->gather_moves(pt, axes_dirs, moves);
}

bool AxesMoveRule::can_attack( Board *b, PiecePtr src, PiecePtr trg ) {
    // to attack on the axes, either the rank or file must be equal.
    Square org(src->square());
    Square dst(trg->square());
    short dr = org.rank() - dst.rank();
    short df = org.file() - dst.file();

    if ( dr != 0 && df != 0 )
        return false;

    // target is orthogonal to source, figure out which side 
    //     dr    df
    // +  down  right
    // -  up    left
    Dir dir = ( dr == 0 ) ? ( df < 0 ) ? LFT : RGT
                          : ( dr < 0 ) ? DN  : UP;
    return b->seek(src, dir, trg).rc == Board::SEEKRC_TARGET_FOUND;
}

void DiagMoveRule::get_moves( Board *b, PiecePtr pt, MoveList& moves) {
    b->gather_moves(pt, diag_dirs, moves);
}

bool DiagMoveRule::can_attack( Board *b, PiecePtr src, PiecePtr trg ) {
    // To attack on the diagnonal, the dr/df of the src/trg must be equal.
    // Square org(src->square());
    // Square dst(src->square());
    // short dr = org.rank() - dst.rank();
    // short df = org.file() - dst.file();
    // if ( std::abs(dr) != std::abs(df) )
    //     return false;

    // trg is on the same diagonal, so now determine if there is anything 
    // between src and trg. Be a little smart, as the signs of dr/df will
    // tell the direction to seek
    //     dr    df
    // +  down  right
    // -  up    left
    const Dir dir = src->square().axes_bearing(trg->square());
    if ( dir == NOWHERE )
        return false;
    // Dir dir = (dr < 0) ? (df < 0) ? DNL : DNR 
    //                    : (df < 0) ? UPL : UPR;         
    return b->seek(src, dir, trg).rc == Board::SEEKRC_TARGET_FOUND;
}

void KnightMoveRule::get_moves( Board *b, PiecePtr pt, MoveList& moves) {
    Square org(pt->square());
    for(Dir dir : knight_moves) {
        auto res = b->seek(pt, dir, Square::UNBOUNDED);
        if ( res.rc == Board::SEEKRC_OUT_OF_BOUNDS )
            continue;
        
        MoveAction ma = MV_NONE;
        if ( res.rc == Board::SEEKRC_FOUND_OPPONENT ) {
            ma = MV_CAPTURE;
        } else if ( res.rc == Board::SEEKRC_NOT_FOUND ) {
            ma = MV_MOVE;
        }
        if ( ma != MV_NONE )
            moves.push_back(Move::create(ma, MR_NONE, res.src, res.path.back()));
    }
}

bool KnightMoveRule::can_attack( Board *b, PiecePtr src, PiecePtr trg ) {
    // Knight attacks are simpler - if any of the eight
    // possible moves are the target square - quit.
    Square org(src->square());
    Square dst(trg->square());
    for(Dir dir : knight_moves) {
        Square here = org + offs[dir];
        if ( dst == here )
            return b->at(here) == trg;
    }
    return false;
}

void PawnMoveRule::get_moves( Board *b, PiecePtr pt, MoveList& moves) {
    // pawns are filthy animals ...
    //
    // 1. Pawns can only move one square forward.
    // 2. Pawns on their home square may move two squares forward.
    // 3. Pawns may capture directly to the UPL or UPR (or DNL/DLR for black pawns.)
    // 4. A pawn on its own fifth rank may capture a neighboring
    //    pawn en passant moving UPL or UPR iif the target pawn
    //    moved forward two squares on its last on-move.
    // 5. A pawn that reaches the eighth rank is promoted.
    //
    // Directions are, of course, side dependent.
    // Case 1: pawns can only move one square forwad.
    bool   isBlack = pt->is_black();
    Square ppos    = pt->square();
    Dir    updn    = (isBlack)?DN:UP;
    Rank   pnhm    = (isBlack)?R7:R2;
    Square pos     = ppos + offs[updn];
    if( pos.in_bounds() && b->is_empty(pos) ) {
        // Case 1: pawn can move forward
        if ( (isBlack && pos.rank() == R1) || (pos.rank() == R8) ) {
            // Case 5. A pawn reaching its eighth rank is promoted
            //
            // As we're collecting all possible moves, record four
            // promotions.
            for( auto action : {MV_PROM_QUEEN, MV_PROM_BISHOP, MV_PROM_KNIGHT, MV_PROM_ROOK})
                moves.push_back(Move::create(action, MR_NONE, ppos, pos));
        } else {
            moves.push_back(Move::create(MV_MOVE, MR_NONE, ppos, pos));
        }
        // Case 2: Pawns on their home square may move two spaces.
        if ( ppos.rank() == pnhm ) {
            pos += offs[updn];
            if( pos.in_bounds() && b->is_empty(pos) )
                moves.push_back(Move::create(MV_MOVE, MR_NONE, ppos, pos));
        }
    }

    // Case 3: Pawns may capture directly to the UPL or UPR.
    // see if an opposing piece is UPL or UPR
    const DirList *dirs = (isBlack) ? &black_pawn_attack : &white_pawn_attack;
    b->gather_moves( pt, *dirs, moves, true );

    // Case 4. A pawn on its own fifth rank may capture a neighboring pawn en passant moving
    // UPL or UPR iif the target pawn moved forward two squares on its last on-move.

    // First check if an en passant pawn exists
    // AND the pawn has not moved off its own rank (is not of type PT_PAWN_OFF)
    // AND pawn is on its fifth rank.
    // AND if target pawn is adjacent to this pawn
    if ( pt->type() == PT_PAWN && b->has_en_passant() ) {
        // an en passant candidate exists
        Rank r_pawn = (isBlack) ? R4 : R5;      // rank where the pawn is
        Rank r_move = (isBlack) ? R3 : R6;      // rank with  the space where our pawn moves
        // the en passant file is the file that contains the subject pawn
        // if our pawn is one square away (left or right) from the en passant file
        // then we *can* capture that pawn.
        if( ppos.rank() == r_pawn && std::abs(ppos.file() - b->get_en_passant().file()) == 1 ) {
            // If so, check if the space above the target pawn is empty.
            // If so, then en passant is possible.
            Square epos( r_move, b->get_en_passant().file() ); // pos of target square
            if ( b->is_empty(epos) )
                moves.push_back( Move::create( MV_EN_PASSANT, MR_NONE, ppos, epos ) );
        }
    }
}

bool PawnMoveRule::can_attack( Board *b, PiecePtr src, PiecePtr trg ) {
    // pawn attacks an opposing:
    // 1. Piece on the immediate left-or-right diagnonal [8F4]
    // 2. PAWN provided that:
    //    a. this pawn is on it's 5th rank (4th for black), and
    //    b. this pawn is on it's original file, and 
    //    c. the opponent's pawn is on the same RANK, and 
    //    d. moved two-squares forward on its first move immediately
    //       prior (en passant) [8F5]
    Square org(src->square());
    Square dst(trg->square());
    bool isBlack = src->is_black();
    const DirList *dirs = (isBlack) ? &black_pawn_attack : &white_pawn_attack;
    for ( auto dir : *dirs )
        if ( dst == org + offs[ dir ] )
            return true;
    // en passant
    // if src and dest are both on R5 (R4 for black)
    return false;
}

void CastleMoveRule::get_moves( Board *b, PiecePtr pt, MoveList& moves) {
    // check if castling can occur.
    // 1. Neither the king nor the rook can have moved
    // 2. The squares between the king and the rook must be empty [8A4b]
    // 3. Theking cannot be in check [8A4a], and 
    // 4. The king cannot move over check [8A4a].
}

std::map<MoveRuleOrd, MoveRulePtr> move_rules{
    { MVRULE_AXES,   std::make_shared<AxesMoveRule>()   },
    { MVRULE_DIAG,   std::make_shared<DiagMoveRule>()   },
    { MVRULE_KNIGHT, std::make_shared<KnightMoveRule>() },
    { MVRULE_PAWN,   std::make_shared<PawnMoveRule>()   },
    { MVRULE_CASTLE, std::make_shared<CastleMoveRule>() }
};

std::map<PieceType, MoveRuleOrdList> piece_move_rules{
    { PT_KING  , MoveRuleOrdList{MVRULE_AXES, MVRULE_DIAG, MVRULE_CASTLE} },
    { PT_QUEEN , MoveRuleOrdList{MVRULE_AXES, MVRULE_DIAG               } },
    { PT_BISHOP, MoveRuleOrdList{             MVRULE_DIAG               } },
    { PT_KNIGHT, MoveRuleOrdList{                          MVRULE_KNIGHT} },
    { PT_ROOK  , MoveRuleOrdList{MVRULE_AXES                            } },
    { PT_PAWN  , MoveRuleOrdList{                          MVRULE_PAWN  } }
};
