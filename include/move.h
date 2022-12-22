#pragma once

#include <memory>
#include <vector>

#include "constants.h"
#include "move.h"
#include "square.h"

extern const Offset offs[];

struct Move;    // forward decl
typedef std::shared_ptr<Move> MovePtr;

struct Move {
    MoveAction action;
    MoveResult result;
    Square     org;
    Square     trg;

    Move(MoveAction ma, MoveResult mr, Square org, Square trg );
    static MovePtr create(MoveAction ma, MoveResult mr, Square org, Square trg );
    friend std::ostream& operator<<(std::ostream& os, const Move& mv);
};

typedef std::shared_ptr<Move> MovePtr;
typedef std::vector<MovePtr>  MoveList;
