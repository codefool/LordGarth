#pragma once

#include <map>
#include <memory>
#include <utility>
#include <vector>

enum Side { SIDE_WHITE=0, SIDE_BLACK };

#define OTHER_SIDE(s) ((s==SIDE_WHITE)?SIDE_BLACK:SIDE_WHITE)

enum Rank { R1=0, R2, R3, R4, R5, R6, R7, R8 };
enum File { Fa=0, Fb, Fc, Fd, Fe, Ff, Fg, Fh };

enum RnF : unsigned short {
    a1 = (R1<<3) | Fa, a2 = (R1<<3) | Fb, a3 = (R1<<3) | Fc, a4 = (R1<<3) | Fd, 
    a5 = (R1<<3) | Fe, a6 = (R1<<3) | Ff, a7 = (R1<<3) | Fg, a8 = (R1<<3) | Fh,
    b1 = (R2<<3) | Fa, b2 = (R2<<3) | Fb, b3 = (R2<<3) | Fc, b4 = (R2<<3) | Fd, 
    b5 = (R2<<3) | Fe, b6 = (R2<<3) | Ff, b7 = (R2<<3) | Fg, b8 = (R2<<3) | Fh,
    c1 = (R3<<3) | Fa, c2 = (R3<<3) | Fb, c3 = (R3<<3) | Fc, c4 = (R3<<3) | Fd, 
    c5 = (R3<<3) | Fe, c6 = (R3<<3) | Ff, c7 = (R3<<3) | Fg, c8 = (R3<<3) | Fh,
    d1 = (R4<<3) | Fa, d2 = (R4<<3) | Fb, d3 = (R4<<3) | Fc, d4 = (R4<<3) | Fd, 
    d5 = (R4<<3) | Fe, d6 = (R4<<3) | Ff, d7 = (R4<<3) | Fg, d8 = (R4<<3) | Fh,
    e1 = (R5<<3) | Fa, e2 = (R5<<3) | Fb, e3 = (R5<<3) | Fc, e4 = (R5<<3) | Fd, 
    e5 = (R5<<3) | Fe, e6 = (R5<<3) | Ff, e7 = (R5<<3) | Fg, e8 = (R5<<3) | Fh,
    f1 = (R6<<3) | Fa, f2 = (R6<<3) | Fb, f3 = (R6<<3) | Fc, f4 = (R6<<3) | Fd, 
    f5 = (R6<<3) | Fe, f6 = (R6<<3) | Ff, f7 = (R6<<3) | Fg, f8 = (R6<<3) | Fh,
    g1 = (R7<<3) | Fa, g2 = (R7<<3) | Fb, g3 = (R7<<3) | Fc, g4 = (R7<<3) | Fd, 
    g5 = (R7<<3) | Fe, g6 = (R7<<3) | Ff, g7 = (R7<<3) | Fg, g8 = (R7<<3) | Fh,
    h1 = (R8<<3) | Fa, h2 = (R8<<3) | Fb, h3 = (R8<<3) | Fc, h4 = (R8<<3) | Fd, 
    h5 = (R8<<3) | Fe, h6 = (R8<<3) | Ff, h7 = (R8<<3) | Fg, h8 = (R8<<3) | Fh
};

#define PAWN_RANK_BLACK R7
#define PAWN_RANK_WHITE R2
#define EN_PASSANT_RANK_WHITE R5
#define EN_PASSANT_RANK_BLACK R4

enum PieceType {
    PT_EMPTY = 0,
    PT_KING,
    PT_QUEEN,
    PT_BISHOP,
    PT_KNIGHT,
    PT_ROOK,
    PT_PAWN,
    PT_PAWN_OFF
};

// the MoveAction is pbcked to 4 bits, so 0..15
enum MoveAction {
	MV_NONE             = 0,
	MV_MOVE             = 1,
	MV_CAPTURE          = 2,
	MV_CASTLE_KINGSIDE  = 3,
	MV_CASTLE_QUEENSIDE = 4,
	MV_EN_PASSANT       = 5,
	// MV_UNUSED = 6,
	// MV_UNUSED = 7,
	MV_PROM_QUEEN       = 8,
	MV_PROM_BISHOP      = 9,
	MV_PROM_KNIGHT      = 10,
	MV_PROM_ROOK        = 11
	// UNUSED = 12
	// UNUSED = 13
	// UNUSED = 14
	// UNUSED = 15
};

enum MoveResult { 
    MR_NONE=0,
    MR_CHECK,
    MR_DOUBLE_CHECK
};

enum MoveRuleOrd {
    MVRULE_AXES   = 0x01,
    MVRULE_DIAG   = 0x02,
    MVRULE_KNIGHT = 0x04,
    MVRULE_PAWN   = 0x08,
    MVRULE_CASTLE = 0x10
};

typedef std::vector<MoveRuleOrd>  MoveRuleOrdList;

enum IllegalMoveReason {
    MOVE_INTO_CHECK,            // king cannot move into check
    KING_IN_CHECK,              // move does not take king out of check
    PUTS_KING_IN_CHECK          // move puts king in check
};

//                rank  file
typedef std::pair<short,short>    Offset;

extern const Offset offs[];

enum Dir { 
    UP,   DN,   LFT,  RGT,  UPR,  UPL,  DNR,  DNL,
    KLUP, KUPL, KUPR, KRUP, KRDN, KDNR, KDNL, KLDN,
    NOWHERE
};

typedef std::vector<Dir> DirList;

extern const DirList white_pawn_attack;
extern const DirList black_pawn_attack;

extern const DirList axes_dirs;
extern const DirList diag_dirs;

extern const DirList knight_moves;  // trying to make some front-page drive-in news

class Board;  // forward

class Square; // forward;
typedef std::vector<Square> SquareList;

class Piece; // forward

typedef std::shared_ptr<Piece>    PiecePtr;
typedef std::vector<PiecePtr>     PieceList;
typedef std::map<Square,PiecePtr> PieceMap;

struct Move; // forward

typedef std::shared_ptr<Move> MovePtr;
typedef std::vector<MovePtr>  MoveList;

class MoveRule; // forward

typedef std::shared_ptr<MoveRule> MoveRulePtr;
typedef std::vector<MoveRulePtr>  MoveRuleList;

class Piece; // forward
typedef std::shared_ptr<Piece>    PiecePtr;
typedef std::vector<PiecePtr>     PieceList;
typedef std::map<Square,PiecePtr> PieceMap;
