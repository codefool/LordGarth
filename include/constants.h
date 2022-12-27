#pragma once

#include <map>
#include <memory>
#include <utility>
#include <vector>

enum Side { SIDE_WHITE=0, SIDE_BLACK };

#define OTHER_SIDE(s) ((s==SIDE_WHITE)?SIDE_BLACK:SIDE_WHITE)

enum Rank { R1=0, R2, R3, R4, R5, R6, R7, R8 };
enum File { Fa=0, Fb, Fc, Fd, Fe, Ff, Fg, Fh };

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

// the MoveAction is packed to 4 bits, so 0..15
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
