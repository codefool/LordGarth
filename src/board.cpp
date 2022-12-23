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

// set("a1", PT_KING, SIDE_WHITE);
// PiecePtr set( std::string fr, PieceType pt, Side s)
// {
//     return set(Square(fr), pt, s);
// }

PiecePtr Board::set( Rank r, File f, PieceType pt, Side s ) {
    return set( Square(r,f), pt, s );
}

PiecePtr Board::set( Square squ, PieceType pt, Side s ) {
    PiecePtr pp = std::make_shared<Piece>(pt, s);
    pp->place(squ);
    _pm[squ] = pp;
    // remember where the kings are
    if (pt == PT_KING)
        _kings[s] = pp;
    return pp;
}

PieceList Board::get_side( Side s ) {
    PieceList ret;
    for ( auto pp : _pm ) {
        if ( pp.second->side() == s )
            ret.push_back(pp.second);
    }
    return ret;
}

void Board::set_initial_position() {
    from_fen(init_pos_fen);

    // static PieceType court[] = {
    //     PT_ROOK, PT_KNIGHT, PT_BISHOP, PT_QUEEN,
    //     PT_KING, PT_BISHOP, PT_KNIGHT, PT_ROOK
    // };

    // short f = Fa;
    // for( PieceType pt : court ) {
    //     File fi = static_cast<File>(f++);
    //     set( R1, fi, pt,      SIDE_WHITE );    // white court piece
    //     set( R2, fi, PT_PAWN, SIDE_WHITE );    // white pawn
    //     set( R7, fi, PT_PAWN, SIDE_BLACK );    // black pawn
    //     set( R8, fi, pt,      SIDE_BLACK );    // black court piece
    // }
}

MoveList& Board::get_moves(MoveList& moves) {
    for ( auto pp : _pm ) {
        for ( auto rule : move_rule_list )
            if ( rule->applies( pp.second->type() ) )
                rule->get_moves( this, pp.second, moves );
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

std::vector<MoveRulePtr> Board::move_rule_list{
    std::make_shared<AxesMoveRule>(),
    std::make_shared<DiagMoveRule>(),
    std::make_shared<KnightMoveRule>(),
    std::make_shared<PawnMoveRule>(),
    std::make_shared<CastleMoveRule>()
};

// initial position FEN notation
const char *Board::init_pos_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0";
