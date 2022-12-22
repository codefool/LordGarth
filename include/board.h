#pragma once

class Board; // forward

#include "constants.h"
#include "piece.h"
#include "moverule.h"

class Board {
private:
    PieceMap    _pm;
    PiecePtr    _kings[2];

    Side        _on_move;
    bool        _castle_white_queenside;
    bool        _castle_white_kingside;
    bool        _castle_black_queenside;
    bool        _castle_black_kingside;
    Square      _en_passant;
    short       _half_move_clock;
    short       _full_move_cnt;
public:
    Board(bool initial_position = true);
    Board(const char *fen);
    Board(std::string fen);
    int piece_cnt();
    PiecePtr at( Rank r, File f );
    PiecePtr at( Square squ );
    bool is_empty(Rank r, File f);
    bool is_empty(Square squ);
    PiecePtr set( Rank r, File f, PieceType pt, Side s );
    PiecePtr set( Square squ, PieceType pt, Side s );
    PieceList get_side( Side s );
    void set_initial_position();
    MoveList& get_moves(MoveList& moves);
    std::string diagram();
    void gather_moves( PiecePtr pp, DirList dirs, MoveList& moves, bool isPawnCapture = false);
    MovePtr check_square(PiecePtr pp, Square trg, bool isPawnCapture = false);

    enum SeekResultCode {
        SEEKRC_NONE,            
        SEEKRC_OUT_OF_BOUNDS,   // seek walked off the board
        SEEKRC_FOUND_FRIENDLY,  // seek found a friendly piece
        SEEKRC_FOUND_OPPONENT,  // seek found an opposing piece
        SEEKRC_TARGET_FOUND,    // seek reached target unobstructed
        SEEKRC_NOT_FOUND        // target not found
    };

    struct SeekResult {
        Square         src;   // where we started
        Square         trg;   // where we're going
        Dir            dir;   // direction we sought
        short          range; // max number of steps
        PiecePtr       enc;   // piece we found
        SquareList     path;  // steps we took        
        SeekResultCode rc; 
    };

    SeekResult seek( PiecePtr src, Dir dir, PiecePtr trg );
    SeekResult seek( PiecePtr src, Dir dir, Square dst );

    void from_fen(const std::string& fen);
    std::string fen();
public:
    static PiecePtr EMPTY;
    static std::vector<MoveRulePtr> move_rule_list;
    static const char *init_pos_fen;
};
