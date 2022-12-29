#include "constants.h"
#include "move.h"
#include "board.h"

void Board::apply_move(Move& mov, Board& cpy) {
    // first, create a scratch copy of this board to modify
    cpy = *this;

    // now, all operations are done on cpy
    PiecePtr src = cpy.at(mov.org);
    PiecePtr trg = cpy.at(mov.dst);

    switch( mov.action )
    {
    case MV_CASTLE_KINGSIDE:
        // mov.getSource() is the location of the king,
        // mov.getTarget() is the location of the rook
        // Move king to Fg, rook to Ff
        move_piece( src, Square(mov.org.rank(), Fg));
        move_piece( trg, Square(mov.dst.rank(), Ff));
        break;
    case MV_CASTLE_QUEENSIDE:
        // mov.getSource() is the location of the king,
        // mov.getTarget() is the location of the rook
        // Move king to Fc, rook to Fd
        move_piece( src, Square(mov.org.rank(), Fc));
        move_piece( trg, Square(mov.dst.rank(), Fd));
        break;
    case MV_PROM_QUEEN:  
        move_piece( src, mov.dst );
        src->promote(PT_QUEEN);  
        break; 
    case MV_PROM_BISHOP: 
        move_piece( src, mov.dst );
        src->promote(PT_BISHOP); 
        break;
    case MV_PROM_KNIGHT: 
        move_piece( src, mov.dst );
        src->promote(PT_KNIGHT); 
        break;
    case MV_PROM_ROOK:   
        move_piece( src, mov.dst );
        src->promote(PT_ROOK);   
        break;
    case MV_MOVE:
    case MV_CAPTURE:
        move_piece( src, mov.dst );
        break;
    case MV_EN_PASSANT: {
        // move the piece, but remove the pawn "passed by"
        //    a  b  c         a  b  c
        //   +--+--+--+      +--+--+--+
        // 6 |  | P|  |    4 | p|xP|  |
        //   +--/--+--+      +--\--+--+
        // 5 | P|xp|  |    3 |  | p|  |
        //   +--+--+--+      +--+--+--+
        //
        move_piece( src, mov.dst );
        // the pawn 'passed by' will be one square toward on-move
        Dir d = (src->side() == SIDE_BLACK) ? UP : DN;
        Square s = mov.dst + offs[d];
        // remove the piece from the board, but prob. need to record this somewhere.
        clear_square( s );
        }
        break;
    }
    // at this point check if either king is in check, and mark the move
    // accordingly.
}

void Board::move_piece(PiecePtr ptr, Square dst) {
    static Square a1("A1");
    static Square a8("A8");
    static Square h1("H1");
    static Square h8("H8");
    // if the dst square is not empty, then we're capturing
    if ( !is_empty(dst) ) {
        clear_square(dst);
    }

    // set the square of the space to dst
    Square org = ptr->square();
    place(ptr, dst);
    // increment the half-move clock for 50-move rule
    _half_move_clock++;

    // check for key piece moves
    switch( ptr->type() ) {
        case PT_KING:
            // king moved - castling no longer available for both sides
            if ( ptr->side() == SIDE_WHITE ) {
                set_castle_white_kingside( false );
                set_castle_white_queenside( false );
            } else {
                set_castle_black_kingside( false );
                set_castle_black_queenside( false );
            }
            break;

        case PT_ROOK:
            // rook moved - castling no longer available for that side
            if ( ptr->side() == SIDE_WHITE ) {
                     if ( org == a1 ) set_castle_white_queenside( false );
                else if ( org == h1 ) set_castle_white_kingside( false );
            } else {
                     if ( org == a8 ) set_castle_black_queenside( false );
                else if ( org == h8 ) set_castle_black_kingside( false );
            }
            break;

        case PT_PAWN:
            // if the file of the org square and dst square differ
            // then pawn moved off its file.
            if ( org.file() != dst.file() ) {
                // pawn has moved off it's original file
                ptr->promote( PT_PAWN_OFF );    
            } else if ( org.rank() == ( ( ptr->side() ) ? R7 : R2 ) &&
                        dst.rank() == ( ( ptr->side() ) ? R5 : R4 )
            ) {
                set_en_passant( org );
            }
            // pawn move resets the half-move-clock
            _half_move_clock = 0;
            break;
    }
}