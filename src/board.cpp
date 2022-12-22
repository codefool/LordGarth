#include <iostream>

#include <map>
#include <memory>
#include <sstream>
#include <utility>
#include <vector>

#include "board.h"

std::vector<std::string> split(std::string target, std::string delimiter) {
    std::vector<std::string> v;

    if (!target.empty()) {
        std::string::size_type start = 0;
        while(true) {
            size_t x = target.find(delimiter, start);
            if (x == std::string::npos)
                break;

            v.push_back(target.substr(start, x-start));
            start = x + delimiter.size();
        }

        v.push_back(target.substr(start));
    }
    return v;
}

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

// Initialize a board from Forsyth-Edwards (FEN) notation string.
//
// 
void Board::from_fen(const std::string& fen)
{
    _pm.clear();

    std::vector<std::string> toks = split(fen, " ");
    // Field 1 - Piece Placement Data
    //
    // - Each rank is described, starting with rank 8 and ending with rank 1,
    //   with a "/" between each one; within each rank, the contents of the 
    //   squares are described in order from the a-file to the h-file.
    // - Each piece is identified by a single letter taken from the standard
    //   English names in algebraic notation (pawn = "P", knight = "N",
    //   bishop = "B", rook = "R", queen = "Q" and king = "K").
    // - White pieces are designated using uppercase letters ("PNBRQK"), while
    //   black pieces use lowercase letters ("pnbrqk").
    // - A set of one or more consecutive empty squares within a rank is denoted
    //   by a digit from "1" to "8", corresponding to the number of squares.
    //
    short rank(R8);
    short file(Fa);
    for (const char *p = toks[0].c_str(); *p; ++p ) {
        if ( std::isalpha(*p) ) {
            // piece type
            PieceType pt;
            Side      side = (std::islower(*p)?SIDE_BLACK:SIDE_WHITE);
            switch(std::toupper(*p)) {
                case 'K': pt = PT_KING;   break;
                case 'Q': pt = PT_QUEEN;  break;
                case 'B': pt = PT_BISHOP; break;
                case 'N': pt = PT_KNIGHT; break;
                case 'R': pt = PT_ROOK;   break;
                case 'P': pt = PT_PAWN;   break;
            }
            set(Square(rank, file), pt, side);
            ++file;
        } else if ( std::isdigit(*p) ) {
            // count of empty squares
            file += short( *p - '0' );
        } else if ( *p == '/' ) {
            // end of rank
            --rank;
            file=Fa;
        } else {
            // error! unknown char
        }
    }

    // Field 2 - Active Color
    // - "w" means that White is to move; "b" means that Black is to move
    //
    _on_move = ( std::tolower(toks[1].at(0)) == 'b' ) ? SIDE_BLACK : SIDE_WHITE;

    // Field 3 - Castling Availability
    // - If neither side has the ability to castle, this field uses the 
    //   character "-". 
    // - Otherwise, this field contains one or more letters: 
    //   - "K" if White can castle kingside, 
    //   - "Q" if White can castle queenside, 
    //   - "k" if Black can castle kingside, and 
    //   - "q" if Black can castle queenside. 
    // - A situation that temporarily prevents castling does not prevent
    //    the use of this notation.
    //
    if ( toks[2] == "-" ) {
        _castle_black_kingside =
        _castle_black_queenside =
        _castle_white_kingside =
        _castle_white_queenside = false;
    } else {
        const char *p = toks[2].c_str();
        while ( *p )
        {
            switch( *p++ ) {
            case 'K': _castle_white_kingside  = true; break;    
            case 'Q': _castle_white_queenside = true; break;    
            case 'k': _castle_black_kingside  = true; break;    
            case 'q': _castle_black_queenside = true; break;    
            }
        }
    }

    // Field 4 - En Passant target square
    // - This is a square over which a pawn has just passed while moving two
    //   squares; 
    // - it is given in algebraic notation.
    // -  If there is no en passant target square, this field uses the character "-".
    // -  This is recorded regardless of whether there is a pawn in position to
    //    capture en passant.
    //
    if ( toks[3] == "-" ) {
        _en_passant = Square::UNBOUNDED;
    } else {
        _en_passant = Square(toks[3]);
    }

    // Field 5 - Halfmove Clock
    // - The number of halfmoves since the last capture or pawn advance, used for 
    //   the fifty-move rule.
    //
    _half_move_clock = std::stoi(toks[4]);

    // Field 6 - Fullmove Count
    // - The number of the full moves. It starts at 1 and is incremented after
    //   Black's move.
    //
    _full_move_cnt = std::stoi(toks[5]);
}

std::string Board::fen()
{
    // Field 1 - Piece Placement Data
    //
    // - Each rank is described, starting with rank 8 and ending with rank 1,
    //   with a "/" between each one; within each rank, the contents of the 
    //   squares are described in order from the a-file to the h-file.
    // - Each piece is identified by a single letter taken from the standard
    //   English names in algebraic notation (pawn = "P", knight = "N",
    //   bishop = "B", rook = "R", queen = "Q" and king = "K").
    // - White pieces are designated using uppercase letters ("PNBRQK"), while
    //   black pieces use lowercase letters ("pnbrqk").
    // - A set of one or more consecutive empty squares within a rank is denoted
    //   by a digit from "1" to "8", corresponding to the number of squares.
    //
    std::string fen;
    std::stringstream ss;
    for ( short rank = R8; rank >= R1; --rank ) {
        short cnt(0);
        for ( short file = Fa; file <= Fh; ++file ) {
            PiecePtr ptr = at((Rank)rank,(File)file);
            if ( ptr->is_empty() ) {
                cnt++;
            } else {
                if ( cnt ) {
                    ss << cnt;
                    cnt = 0;
                }
                ss << ptr->glyph();
            }
        }
        if ( cnt )
            ss << cnt;
        if ( rank > R1 )
            ss << '/';
    }
    ss << ' ';

    // Field 2 - Active Color
    // - "w" means that White is to move; "b" means that Black is to move
    //
    ss << (( _on_move == SIDE_BLACK ) ? 'b' : 'w')
       << ' ';

    // Field 3 - Castling Availability
    // - If neither side has the ability to castle, this field uses the 
    //   character "-". 
    // - Otherwise, this field contains one or more letters: 
    //   - "K" if White can castle kingside, 
    //   - "Q" if White can castle queenside, 
    //   - "k" if Black can castle kingside, and 
    //   - "q" if Black can castle queenside. 
    // - A situation that temporarily prevents castling does not prevent
    //    the use of this notation.
    //
    if ( !_castle_white_kingside && !_castle_white_queenside
      && !_castle_black_kingside && !_castle_black_queenside
    ) {
        ss << '-';
    } else {
        if ( _castle_white_kingside  ) ss << 'K';
        if ( _castle_white_queenside ) ss << 'Q';
        if ( _castle_black_kingside  ) ss << 'k';
        if ( _castle_black_queenside ) ss << 'q';
    }
    ss << ' ' ;

    // Field 4 - En Passant target square
    // - This is a square over which a pawn has just passed while moving two
    //   squares; 
    // - it is given in algebraic notation.
    // -  If there is no en passant target square, this field uses the character "-".
    // -  This is recorded regardless of whether there is a pawn in position to
    //    capture en passant.
    //
    if ( _en_passant == Square::UNBOUNDED ) {
        ss << '-';
    } else {
        ss << _en_passant;
    }
    ss << ' ';

    // Field 5 - Halfmove Clock
    // - The number of halfmoves since the last capture or pawn advance, used for 
    //   the fifty-move rule.
    //
    ss << _half_move_clock << ' ';

    // Field 6 - Fullmove Count
    // - The number of the full moves. It starts at 1 and is incremented after
    //   Black's move.
    //
    ss << _full_move_cnt;

    return ss.str();
}
