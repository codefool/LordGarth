// MoveRule
// Abstract base class for handling piece movement behavior.

#pragma once

#include "move.h"
#include "piece.h"

class Board;

class MoveRule {
public:
    // return true if this rule applies to piece of type PieceType
    virtual bool applies(PieceType pt) { return false; }
    // determine all valid moves for Piece pt and add to moves.
    virtual void get_moves( Board *b, PiecePtr pt, MoveList& moves) = 0;
    // return true if piece src can attack piece trg.
    virtual bool can_attack( Board *b, PiecePtr src, PiecePtr trg ) { return false; }
};

typedef std::shared_ptr<MoveRule> MoveRulePtr;

// movement rule for pieces that can move on axes
class AxesMoveRule : public MoveRule {
private:
    static DirList dirs;
public:
    virtual bool applies(PieceType pt);
    virtual void get_moves( Board* b, PiecePtr pt, MoveList& moves);
    virtual bool can_attack( Board *b, PiecePtr src, PiecePtr trg );
};

// movement rule for pieces that can on diagonals
class DiagMoveRule : public MoveRule {
private:
    static DirList dirs;
public:
    virtual bool applies(PieceType pt);
    virtual void get_moves( Board *b, PiecePtr pt, MoveList& moves);
    virtual bool can_attack( Board *b, PiecePtr src, PiecePtr trg );
};

// movement rule for knights
class KnightMoveRule : public MoveRule {
private:
    static DirList dirs;
public:    
    virtual bool applies(PieceType pt);
    virtual void get_moves( Board *b, PiecePtr pt, MoveList& moves);
    virtual bool can_attack( Board *b, PiecePtr src, PiecePtr trg );
};

// movement rule for pawns
class PawnMoveRule : public MoveRule {
private:    
    static DirList on_black;
    static DirList on_white;

public:
    virtual bool applies(PieceType pt);
    virtual void get_moves( Board *b, PiecePtr pt, MoveList& moves);
    virtual bool can_attack( Board *b, PiecePtr src, PiecePtr trg );
};


// movement rule for castle
class CastleMoveRule : public MoveRule {
public:
    virtual bool applies(PieceType pt);
    virtual void get_moves( Board *b, PiecePtr pt, MoveList& moves);
};
