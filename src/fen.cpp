
#include <sstream>

#include "board.h"

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
            set(Square(rank, file++), pt, side);
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
    ss << (IS_BLACK( _on_move ) ? 'b' : 'w')
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
