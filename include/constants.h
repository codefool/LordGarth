#pragma once

#include <utility>
#include <vector>

enum Side { SIDE_WHITE=0, SIDE_BLACK };
enum Rank { R1=0, R2, R3, R4, R5, R6, R7, R8 };
enum File { Fa=0, Fb, Fc, Fd, Fe, Ff, Fg, Fh };
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
	MV_PROMOTION_QUEEN  = 8,
	MV_PROMOTION_BISHOP = 9,
	MV_PROMOTION_KNIGHT = 10,
	MV_PROMOTION_ROOK   = 11
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

//                rank  file
typedef std::pair<short,short>    Offset;

enum Dir { 
    UP,   DN,   LFT,  RGT,  UPR,  UPL,  DNR,  DNL,
    KLUP, KUPL, KUPR, KRUP, KRDN, KDNR, KDNL, KLDN
};

typedef std::vector<Dir> DirList;
