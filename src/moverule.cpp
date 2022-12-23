#include "moverule.h"
#include "board.h"

bool AxesMoveRule::applies(PieceType pt) { 
    return pt == PT_KING || pt == PT_QUEEN || pt == PT_ROOK; 
}

void AxesMoveRule::get_moves( Board* b, PiecePtr pt, MoveList& moves) {
    b->gather_moves(pt, dirs, moves);
}

DirList AxesMoveRule::dirs{UP, DN, LFT, RGT};

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


bool DiagMoveRule::applies(PieceType pt) { 
    return pt == PT_KING || pt == PT_QUEEN || pt == PT_BISHOP; 
}

void DiagMoveRule::get_moves( Board *b, PiecePtr pt, MoveList& moves) {
    b->gather_moves(pt, dirs, moves);
}

bool DiagMoveRule::can_attack( Board *b, PiecePtr src, PiecePtr trg ) {
    // To attack on the diagnonal, the dr/df of the src/trg must be equal.
    Square org(src->square());
    Square dst(src->square());
    short dr = dst.rank() - dst.rank();
    short df = dst.file() - dst.file();
    if ( std::abs(dr) != std::abs(df) )
        return false;

    // trg is on the same diagonal, so now determine if there is anything 
    // between src and trg. Be a little smart, as the signs of dr/df will
    // tell the direction to seek
    //     dr    df
    // +  down  right
    // -  up    left
    Dir dir = (dr < 0) ? (df < 0) ? DNL : DNR 
                        : (df < 0) ? UPL : UPR;         
    return b->seek(src, dir, trg).rc == Board::SEEKRC_TARGET_FOUND;
}

DirList DiagMoveRule::dirs{UPL, UPR, DNL, DNR};



bool KnightMoveRule::applies(PieceType pt) { 
    return pt == PT_KNIGHT; 
}

void KnightMoveRule::get_moves( Board *b, PiecePtr pt, MoveList& moves) {
    Square org(pt->square());
    for(Dir dir : dirs) {
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
    for(Dir dir : dirs) {
        Square here = org + offs[dir];
        if ( dst == here )
            return b->at(here) == trg;
    }
    return false;
}

DirList KnightMoveRule::dirs{ KLUP, KUPL, KUPR, KRUP, KRDN, KDNR, KDNL, KLDN };



bool PawnMoveRule::applies(PieceType pt) { 
    return pt == PT_PAWN || pt == PT_PAWN_OFF; 
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
        // pawn can move forward
        if ( (isBlack && pos.rank() == R1) || (pos.rank() == R8) ) {
            // Case 5. A pawn reaching its eighth rank is promoted
            //
            // As we're collecting all possible moves, record four
            // promotions.
            for( auto action : {MV_PROMOTION_QUEEN, MV_PROMOTION_BISHOP, MV_PROMOTION_KNIGHT, MV_PROMOTION_ROOK})
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
    DirList *dirs = (isBlack) ? &on_black : &on_white;
    b->gather_moves( pt, *dirs, moves, true );
    // Case 4. A pawn on its own fifth rank may capture a neighboring pawn en passant moving
    // UPL or UPR iif the target pawn moved forward two squares on its last on-move.

    // First check if an en passant pawn exists
    // AND the pawn has not moved off its own rank (is not of type PT_PAWN_OFF)
    // AND pawn is on its fifth rank.
    // AND if target pawn is adjacent to this pawn
    // if ( pt->type() == PT_PAWN && _p.gi().hasEnPassant() ) {
    //     // an en passant candidate exists
    //     Rank r_pawn = (isBlack) ? R4 : R5;      // rank where the pawn is
    //     Rank r_move = (isBlack) ? R3 : R6;      // rank with  the space where our pawn moves
    //     // the en passant file is the file that contains the subject pawn
    //     // if our pawn is one square away (left or right) from the en passant file
    //     // then we *can* capture that pawn.
    //     if( ppos.rank() == r_pawn && abs( ppos.f() - _p.gi().getEnPassantFile()) == 1 ) {
    //         // If so, check if the space above the target pawn is empty.
    //         // If so, then en passant is possible.
    //         Square epos( r_move, _p.gi().getEnPassantFile() ); // pos of target square
    //         if ( _p.is_square_empty(epos) )
    //             moves.push_back( Move::create( MV_EN_PASSANT, MR_NONE, ppos, epos ) );
    //     }
    // }
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
    DirList *dirs = (isBlack) ? &on_black : &on_white;
    for ( auto dir : *dirs )
        if ( dst == org + offs[ dir ] )
            return true;
    // en passant
    // if src and dest are both on R5 (R4 for black)
    return false;
}

DirList PawnMoveRule::on_black{DNL,DNR};
DirList PawnMoveRule::on_white{UPL,UPR};


bool CastleMoveRule::applies(PieceType pt) { 
    return pt == PT_KING;
}

void CastleMoveRule::get_moves( Board *b, PiecePtr pt, MoveList& moves) {
}

