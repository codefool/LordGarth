#include <iostream>

#include <map>
#include <memory>
#include <sstream>
#include <utility>
#include <vector>

#include "constants.h"
#include "move.h"
#include "board.h"


Board::Board(bool initial_position) {
    if ( initial_position )
        set_initial_position();
}

Board::Board(const char *fen) 
: Board(std::string(fen)) 
{}
    
Board::Board(std::string fen) {
    from_fen(fen);
}

Board::Board(BoardPacked pack) {
    unpack(pack);
}

int Board::piece_cnt() { return _pm.size(); }

PiecePtr Board::at( Rank r, File f ) const {
    return at( Square(r,f) );
}

PiecePtr Board::at( Square squ ) const {
    auto itr = _pm.find( squ );
    if ( itr != _pm.end() )
        return itr->second;       
    return EMPTY;
}

bool Board::is_empty(Rank r, File f) const {
    return is_empty(Square(r,f));
}

bool Board::is_empty(Square squ) const {
    return at(squ) == EMPTY;
}

void Board::clear_square(Square squ) { 
    _pm.erase(squ);
}

PiecePtr Board::set( Rank r, File f, PieceType pt, Side s ) {
    return set( Square(r,f), pt, s );
}

PiecePtr Board::set( RnF rnf, PieceType pt, Side s ) {
    return set( Square(rnf), pt, s );
}

PiecePtr Board::set( Square squ, PieceType pt, Side s ) {
    PiecePtr pp = std::make_shared<Piece>(pt, s);
    place( pp, squ );
    return pp;
}

void Board::place( PiecePtr pp, Square squ) {
    pp->place(squ);
    _pm[squ] = pp;
    // remember where the kings are
    if ( pp->is_king() )
        _kings[ pp->side() ] = pp;
}

Side Board::get_on_move() const {
    return _on_move;
}

void Board::set_on_move(Side s) {
    _on_move = s;
}

void Board::toggle_on_move() {
    _on_move = (_on_move == SIDE_BLACK)
             ? SIDE_WHITE
             : SIDE_BLACK;
}

bool Board::has_en_passant() const {
    return _en_passant != Square::UNBOUNDED;
}

void Board::clear_en_passant() {
    _en_passant = Square::UNBOUNDED;
}

Square Board::get_en_passant() const {
    return _en_passant;
}

void Board::set_en_passant(Square eps) {
    _en_passant = eps;
}

short Board::get_half_move_clock() const {
    return _half_move_clock;
}

void Board::set_half_move_clock(short cnt) {
    _half_move_clock = cnt;
}

short Board::inc_half_move_clock() {
    return ++_half_move_clock;
}

void Board::reset_half_move_clock() {
    set_half_move_clock(0);
}

short Board::get_full_move_cnt() const {
    return _full_move_cnt;
}

void Board::set_full_move_cnt(short cnt) {
   _full_move_cnt = cnt; 
}

short Board::inc_full_move_cnt() {
    return ++_full_move_cnt;
}

void Board::clear_full_move_cnt() {
    set_full_move_cnt(0);
}

bool Board::get_castle_white_queenside() const {
    return _castle_white_queenside;
}

void Board::set_castle_white_queenside(bool state) {
    _castle_white_queenside = state;
}

bool Board::get_castle_white_kingside() const {
    return _castle_white_kingside;
}

void Board::set_castle_white_kingside(bool state) {
    _castle_white_kingside = state;
}

bool Board::get_castle_black_queenside() const {
    return _castle_black_queenside;
}

void Board::set_castle_black_queenside(bool state) {
    _castle_black_queenside = state;
}

bool Board::get_castle_black_kingside() const {
    return _castle_black_kingside;
}

void Board::set_castle_black_kingside(bool state) {
    _castle_black_kingside = state;
}


PieceList Board::get_side_pieces( Side s ) const {
    PieceList ret;
    for ( auto pp : _pm ) {
        if ( pp.second->side() == s )
            ret.push_back(pp.second);
    }
    return ret;
}

void Board::set_initial_position() {
    from_fen(init_pos_fen);
}

MoveList& Board::get_moves(MoveList& moves) const {
    for ( auto pp : _pm ) {
        PiecePtr ptr( pp.second );
        if ( ptr->moves_knight() ) {
            Square org(ptr->square());
            for(Dir dir : knight_moves) {
                auto res = seek(ptr, dir, Square::UNBOUNDED);
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
            continue;
        }
        if ( ptr->moves_pawn() ) {
            get_pawn_moves( ptr, moves );
            continue;
        }
        if ( ptr->moves_axes() ) {
            gather_moves(ptr, axes_dirs, moves);
        }
        if ( ptr->moves_diag() ) {
            gather_moves(ptr, diag_dirs, moves);
        }
    }
    return moves;
}

void Board::get_pawn_moves( PiecePtr ptr, MoveList& moves ) const {
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
    bool   isBlack = ptr->is_black();
    Square ppos    = ptr->square();
    Dir    updn    = (isBlack)?DN:UP;
    Rank   pnhm    = (isBlack)?R7:R2;
    Square pos     = ppos + offs[updn];
    if( pos.in_bounds() && is_empty(pos) ) {
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
            if( pos.in_bounds() && is_empty(pos) )
                moves.push_back(Move::create(MV_MOVE, MR_NONE, ppos, pos));
        }
    }

    // Case 3: Pawns may capture directly to the UPL or UPR.
    // see if an opposing piece is UPL or UPR
    const DirList *dirs = (isBlack) ? &black_pawn_attack : &white_pawn_attack;
    gather_moves( ptr, *dirs, moves, true );

    // Case 4. A pawn on its own fifth rank may capture a neighboring pawn en passant moving
    // UPL or UPR iif the target pawn moved forward two squares on its last on-move.

    // First check if an en passant pawn exists
    // AND the pawn has not moved off its own rank (is not of type PT_PAWN_OFF)
    // AND pawn is on its fifth rank.
    // AND if target pawn is adjacent to this pawn
    if ( ptr->is_pawn() && has_en_passant() ) {
        // an en passant candidate exists
        Rank r_pawn = (isBlack) ? R4 : R5;      // rank where the pawn is
        Rank r_move = (isBlack) ? R3 : R6;      // rank with  the space where our pawn moves
        File r_file = get_en_passant().file();
        // the en passant file is the file that contains the subject pawn
        // if our pawn is one square away (left or right) from the en passant file
        // then we *can* capture that pawn.
        if( ppos.rank() == r_pawn && std::abs(ppos.file() - r_file) == 1 ) {
            // If so, check if the space above the target pawn is empty.
            // If so, then en passant is possible.
            Square epos( r_move, r_file ); // pos of target square
            if ( is_empty(epos) )
                moves.push_back( Move::create( MV_EN_PASSANT, MR_NONE, ppos, epos ) );
        }
    }
}

std::string Board::diagram() const {
    std::stringstream ss;
    for ( short r = R8; r >= R1; --r ) {
        ss << char('a' + r) << ": ";
        for( short f = Fa; f <= Fh; ++f ) {
            PiecePtr pt = at((Rank)r,(File)f);
            ss << pt->glyph() << ' ';
        }
        ss << std::endl;
    }
    ss << "   1 2 3 4 5 6 7 8" << std::endl;
    return ss.str();
}

void Board::gather_moves( PiecePtr pp, DirList dirs, MoveList& moves, bool isPawnCapture ) const {
    for (auto d : dirs) {
        Square pos = pp->square();
        Offset o = offs[d];
        int r = pp->range();
        while ( r-- ) {
            pos += o;
            if( !pos.in_bounds() )
                break; // walked off the edge of the board.
            MovePtr mov = check_square(pp, pos, isPawnCapture);
            if (mov == nullptr)
                break; // encountered a friendly piece - walk is over
            moves.push_back(mov);
            if (mov->action == MV_CAPTURE)
                break; // captured an enemy piece - walk is over.
        }
    }
}

MovePtr Board::check_square(PiecePtr pp, Square dst, bool isPawnCapture ) const {
    PiecePtr trg = at(dst);
    Square   org = pp->square();

    if ( trg->is_empty() ) {
        // empty square so record move and continue
        // for isPawnCapture, the move is only valid if the space
        // is occupied by an opposing piece. So, if it's empty
        // return nullptr. Otherwise return the move.
        return (isPawnCapture) ? nullptr
                               : Move::create(MV_MOVE, MR_NONE, org, dst);
    }

    if( trg->side() == pp->side()) {
        // If friendly piece, do not record move and leave.
        return nullptr;
    }

    return Move::create(MV_CAPTURE, MR_NONE, org, dst);
}

Board::SeekResult Board::seek( PiecePtr src, Dir dir, PiecePtr trg ) const {
    SeekResult res = seek(src, dir, trg->square() );
    if ( res.rc == SEEKRC_FOUND_FRIENDLY || res.rc == SEEKRC_FOUND_OPPONENT )
        if ( res.enc == trg )
            res.rc = SEEKRC_TARGET_FOUND;
    return res;
}

Board::SeekResult Board::seek( PiecePtr src, Dir dir, Square dst ) const {
    SeekResult res;
    res.src   = src->square();
    res.trg   = dst;
    res.dir   = dir;
    res.range = src->range();
    res.rc    = SEEKRC_NONE;

    Square here  = res.src;
    short  range = res.range;
    while ( range-- ) {
        Square test = here + offs[dir];
        if ( !test.in_bounds() ) {
            res.rc = SEEKRC_OUT_OF_BOUNDS;
            break;
        }
        here = test;
        res.path.push_back(here);
        res.enc = at(here);
        if ( ! res.enc->is_empty() ) {
            res.rc = ( src->is_black() == res.enc->is_black() ) 
                    ? SEEKRC_FOUND_FRIENDLY 
                    : SEEKRC_FOUND_OPPONENT;
            break;
        }
    }
    // if at this point we exhausted the loop with no other resolution,
    // we simply ran out of steps to walk.
    if ( res.rc == SEEKRC_NONE )
        res.rc = SEEKRC_NOT_FOUND;

    return res;
}

PiecePtr Board::EMPTY = std::make_shared<Piece>(PT_EMPTY, SIDE_WHITE);

// initial position FEN notation
const char *Board::init_pos_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0";


short Board::test_for_attack(PiecePtr trg) const {
    Square     dst(trg->square());
    PieceList  attackers(get_side_pieces(OTHER_SIDE(trg->side())));
    short      cnt(0);
    SeekResult res;
    Dir        dir;
    for ( PiecePtr org : attackers ) {
        Square src = org->square();
        if ( org->moves_knight() ) {
            for ( Dir dir : knight_moves ) {
                if ( org->square() + offs[dir] == dst ) {
                    cnt++;
                    break;
                }
            }
            // since knight cannot move like anything else, we're done for this cycle
            continue;   
        }
        if ( org->moves_pawn() ) {
            const DirList *dirs = ( trg->side() == SIDE_WHITE ) ? &black_pawn_attack : &white_pawn_attack;
            for ( Dir dir : *dirs ) {
                if ( org->square() + offs[dir] == dst ) {
                    cnt++;
                    break;
                }
            }
            // since pawn cannot move like anything else, we're done for this cycle
            continue;
        }
        if ( org->moves_diag() && (dir = src.diag_bearing(dst)) != NOWHERE ) { 
            res = seek( org, dir, trg );
            if ( res.enc == trg )
                cnt++;                     
        }
        if ( org->moves_axes() && ( dir = src.axes_bearing(dst) ) != NOWHERE ) {
            res = seek( org, dir, trg );
            if ( res.enc == trg )
                cnt++;                     
        }
    }

    return cnt;
}

short Board::test_for_check(Side s) const {
    return test_for_attack(_kings[s]);
}

