#pragma once

#include <map>
#include <memory>
#include <utility>
#include <vector>

enum Side { SIDE_WHITE=0, SIDE_BLACK };

#define OTHER_SIDE(s) ((s==SIDE_WHITE)?SIDE_BLACK:SIDE_WHITE)

enum Rank : uint8_t { R1=0, R2, R3, R4, R5, R6, R7, R8 };
enum File : uint8_t { Fa=0, Fb, Fc, Fd, Fe, Ff, Fg, Fh };

#define RNF(r,f) ((r<<3)|f)

// convenience references so that squares can be refered to by their
// canonical names.
enum RnF : uint8_t {
    a1 = RNF(R1,Fa), a2 = RNF(R1,Fb), a3 = RNF(R1,Fc), a4 = RNF(R1,Fd),
    a5 = RNF(R1,Fe), a6 = RNF(R1,Ff), a7 = RNF(R1,Fg), a8 = RNF(R1,Fh),
    b1 = RNF(R2,Fa), b2 = RNF(R2,Fb), b3 = RNF(R2,Fc), b4 = RNF(R2,Fd),
    b5 = RNF(R2,Fe), b6 = RNF(R2,Ff), b7 = RNF(R2,Fg), b8 = RNF(R2,Fh),
    c1 = RNF(R3,Fa), c2 = RNF(R3,Fb), c3 = RNF(R3,Fc), c4 = RNF(R3,Fd),
    c5 = RNF(R3,Fe), c6 = RNF(R3,Ff), c7 = RNF(R3,Fg), c8 = RNF(R3,Fh),
    d1 = RNF(R4,Fa), d2 = RNF(R4,Fb), d3 = RNF(R4,Fc), d4 = RNF(R4,Fd),
    d5 = RNF(R4,Fe), d6 = RNF(R4,Ff), d7 = RNF(R4,Fg), d8 = RNF(R4,Fh),
    e1 = RNF(R5,Fa), e2 = RNF(R5,Fb), e3 = RNF(R5,Fc), e4 = RNF(R5,Fd),
    e5 = RNF(R5,Fe), e6 = RNF(R5,Ff), e7 = RNF(R5,Fg), e8 = RNF(R5,Fh),
    f1 = RNF(R6,Fa), f2 = RNF(R6,Fb), f3 = RNF(R6,Fc), f4 = RNF(R6,Fd),
    f5 = RNF(R6,Fe), f6 = RNF(R6,Ff), f7 = RNF(R6,Fg), f8 = RNF(R6,Fh),
    g1 = RNF(R7,Fa), g2 = RNF(R7,Fb), g3 = RNF(R7,Fc), g4 = RNF(R7,Fd),
    g5 = RNF(R7,Fe), g6 = RNF(R7,Ff), g7 = RNF(R7,Fg), g8 = RNF(R7,Fh),
    h1 = RNF(R8,Fa), h2 = RNF(R8,Fb), h3 = RNF(R8,Fc), h4 = RNF(R8,Fd),
    h5 = RNF(R8,Fe), h6 = RNF(R8,Ff), h7 = RNF(R8,Fg), h8 = RNF(R8,Fh)
};

#define PAWN_RANK_BLACK R7
#define PAWN_RANK_WHITE R2
#define EN_PASSANT_RANK_WHITE R5
#define EN_PASSANT_RANK_BLACK R4

enum PieceType : uint8_t {
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
enum MoveAction : uint8_t {
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

enum MoveResult : uint8_t { 
    MR_NONE=0,
    MR_CHECK,
    MR_DOUBLE_CHECK
};

enum MoveRuleOrd : uint8_t {
    MVRULE_AXES   = 0x01,
    MVRULE_DIAG   = 0x02,
    MVRULE_KNIGHT = 0x04,
    MVRULE_PAWN   = 0x08,
    MVRULE_CASTLE = 0x10
};

typedef std::vector<MoveRuleOrd>  MoveRuleOrdList;

enum IllegalMoveReason : uint8_t {
    IMR_12E_MOVE_INTO_CHECK,     // king cannot move into check
    IMR_12C_KING_STILL_IN_CHECK, // move does not take king out of check
    IMR_12E_PUTS_KING_IN_CHECK,  // move puts king in check
    IMR_8A4_CASTLE_OUT_OF_CHECK, // king cannot castle out of check
    IMR_12E_SQAURE_UNDER_ATTACK  // king moves over or into square under attack
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

// Packed structures
//
// Game Information
//
// xxxx xxxx .... .... .... .... .... .... = number of active pieces on the board (0..31)
// .... .... x... .... .... .... .... .... = white castle kingside enabled  (WK or WKR has not moved)
// .... .... .x.. .... .... .... .... .... = white castle queenside enabled (WK or WQR has not moved)
// .... .... ..x. .... .... .... .... .... = black castle kingside enabled  (BK or BKR has not moved)
// .... .... ...x .... .... .... .... .... = black castle queenside enabled (BK or BQR has not moved)
// .... .... .... x... .... .... .... .... = side on-move: 0-white, 1-black
// .... .... .... .xxx xxx. .... .... .... = 
// .... .... .... .... .... .... .... .... = rank/file of en passant pawn

// It is imperitive that all unused bits - or bits that are out of scope - be set to 0


#pragma pack(1)
union GameInformation {
    uint64_t i;
    struct {
        uint64_t unused                :28;
        uint64_t castle_white_queenside:1;
        uint64_t castle_white_kingside :1;
        uint64_t castle_black_queenside:1;
        uint64_t castle_black_kingside :1;
        uint64_t on_move               :1;
        uint64_t en_passant            :7;
        uint64_t half_move_clock       :8;
        uint64_t full_move_cnt         :8;
        uint64_t piece_cnt             :8;
    } f;
    GameInformation() 
    : i(0) 
    {}
};

union BoardPacked {
    uint8_t b[32];
    struct {
        uint64_t gi;    // game information
        uint64_t pop;   // population bitmap
        uint64_t lo;    // lo 64-bits population info
        uint64_t hi;    // hi 64-bits population info
    } f;

    BoardPacked() {
        f.gi = f.pop = f.lo = f.hi = 0;
    }
    friend std::ostream& operator<<(std::ostream& os, const BoardPacked& p);
};

union MovePacked {
    uint32_t i;
    struct {
        uint32_t action:4;
        uint32_t result:4;
        uint32_t source:6;
        uint32_t target:6;
        uint32_t unused:12;
    } f;
    MovePacked() 
    : i(0) 
    {}
    friend std::ostream& operator<<(std::ostream& os, const MovePacked& p);
};
#pragma pack()

typedef std::vector<BoardPacked> BoardPackedList;
typedef std::vector<MovePacked>  MovePackedList;