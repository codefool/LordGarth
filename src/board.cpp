#include <iostream>

#include <map>
#include <memory>
#include <sstream>
#include <utility>
#include <vector>

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

int Board::piece_cnt() { return _pm.size(); }

PiecePtr Board::at( Rank r, File f ) {
    return at( Square(r,f) );
}

PiecePtr Board::at( Square squ ) {
    auto itr = _pm.find( squ );
    if ( itr != _pm.end() )
        return itr->second;       
    return EMPTY;
}

bool Board::is_empty(Rank r, File f) {
    return is_empty(Square(r,f));
}

bool Board::is_empty(Square squ) {
    return at(squ) == EMPTY;
}

void Board::clear_square(Square squ) { 
    _pm.erase(squ);
}

PiecePtr Board::set( Rank r, File f, PieceType pt, Side s ) {
    return set( Square(r,f), pt, s );
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
    if ( pp->type() == PT_KING )
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


PieceList Board::get_side_pieces( Side s ) {
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

MoveList& Board::get_moves(MoveList& moves) {
    for ( auto pp : _pm ) {
        for ( MoveRuleOrd ord : piece_move_rules[pp.second->type()] ) {
            move_rules[ord]->get_moves( this, pp.second, moves );
        }
    }
    return moves;
}

std::string Board::diagram() {
    std::stringstream ss;
    for ( short r = R8; r >= R1; --r ) {
        for( short f = Fa; f <= Fh; ++f ) {
            PiecePtr pt = at((Rank)r,(File)f);
            ss << pt->glyph() << ' ';
        }
        ss << std::endl;
    }
    return ss.str();
}

void Board::gather_moves( PiecePtr pp, DirList dirs, MoveList& moves, bool isPawnCapture ) {
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

MovePtr Board::check_square(PiecePtr pp, Square trg, bool isPawnCapture ) {
    PiecePtr other = at(trg);
    Square   org = pp->square();

    if ( other->is_empty() ) {
        // empty square so record move and continue
        // for isPawnCapture, the move is only valid if the space
        // is occupied by an opposing piece. So, if it's empty
        // return nullptr. Otherwise return the move.
        return (isPawnCapture) ? nullptr
                                : Move::create(MV_MOVE, MR_NONE, org, trg);
    }

    if( other->side() == pp->side()) {
        // If friendly piece, do not record move and leave.
        return nullptr;
    }

    return Move::create(MV_CAPTURE, MR_NONE, org, trg);
}

Board::SeekResult Board::seek( PiecePtr src, Dir dir, PiecePtr trg ) {
    SeekResult res = seek(src, dir, trg->square() );
    if ( res.rc == SEEKRC_FOUND_FRIENDLY || res.rc == SEEKRC_FOUND_OPPONENT )
        if ( res.enc == trg )
            res.rc = SEEKRC_TARGET_FOUND;
    return res;
}

Board::SeekResult Board::seek( PiecePtr src, Dir dir, Square dst ) {
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


short Board::test_for_check(Side s) {
    PiecePtr   king(_kings[s]);
    Square     trg(king->square());
    PieceList  att(get_side_pieces(OTHER_SIDE(s)));
    short      cnt(0);
    SeekResult res;
    Dir        dir;
    for ( PiecePtr attacker : att ) {
        Square src = attacker->square();
        if ( attacker->moves_diag() && (dir = src.diag_bearing(trg)) != NOWHERE ) { 
            res = seek( attacker, dir, trg );
            if ( res.enc == king )
                cnt++;                     
        }
        if ( attacker->moves_axes() && ( dir = src.axes_bearing(trg) ) != NOWHERE ) {
            res = seek( attacker, dir, trg );
            if ( res.enc == king )
                cnt++;                     
        }
        if ( attacker->moves_knight() ) {
            for ( Dir dir : knight_moves ) {
                if ( attacker->square() + offs[dir] == trg ) {
                    cnt++;
                    break;
                }
            }
        }
        if ( attacker->moves_pawn() ) {
            const DirList *dirs = ( s == SIDE_WHITE ) ? &black_pawn_attack : &white_pawn_attack;
            for ( Dir dir : *dirs ) {
                if ( attacker->square() + offs[dir] == trg ) {
                    cnt++;
                    break;
                }
            }
        }
    }

    return cnt;
}
