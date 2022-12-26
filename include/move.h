#pragma once

#include <memory>
#include <vector>

#include "constants.h"
#include "move.h"
#include "square.h"

extern const Offset offs[];

struct Move {
    MoveAction action;
    MoveResult result;
    Square     org;
    Square     dst;

    Move(MoveAction ma, MoveResult mr, Square org, Square dst );
    static MovePtr create(MoveAction ma, MoveResult mr, Square org, Square dst );
    friend std::ostream& operator<<(std::ostream& os, const Move& mv);
};

