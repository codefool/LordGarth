#pragma once

class Board; // forward

#include "constants.h"
#include "piece.h"
#include "moverule.h"
#include "util.h"

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
    void clear_square(Square squ);
    PiecePtr set( Rank r, File f, PieceType pt, Side s );
    PiecePtr set( Square squ, PieceType pt, Side s );
    void place( PiecePtr p, Square squ);
    Side get_on_move() const;
    void set_on_move(Side s);
    void toggle_on_move();
    bool get_castle_white_queenside() const;
    void set_castle_white_queenside(bool state);
    bool get_castle_white_kingside() const;
    void set_castle_white_kingside(bool state);
    bool get_castle_black_queenside() const;
    void set_castle_black_queenside(bool state);
    bool get_castle_black_kingside() const;
    void set_castle_black_kingside(bool state);
    bool has_en_passant() const;
    void clear_en_passant();
    Square get_en_passant() const;
    void set_en_passant(Square eps);
    short get_half_move_clock() const;
    void set_half_move_clock(short cnt);
    short inc_half_move_clock();
    void reset_half_move_clock();
    short get_full_move_cnt() const;
    void set_full_move_cnt(short cnt);
    short inc_full_move_cnt();
    void clear_full_move_cnt();


    PieceList get_side_pieces( Side s );
    void set_initial_position();
    MoveList& get_moves(MoveList& moves);
    void apply_move(Move& mov, Board& cpy);
    void move_piece(PiecePtr ptr, Square dst);
    std::string diagram();
    void gather_moves( PiecePtr pp, DirList dirs, MoveList& moves, bool isPawnCapture = false);
    MovePtr check_square(PiecePtr pp, Square trg, bool isPawnCapture = false);

    short test_for_check(Side s);

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
    static const char *init_pos_fen;
};
