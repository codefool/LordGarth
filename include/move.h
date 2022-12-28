#pragma once

#include <memory>
#include <vector>

#include "constants.h"
#include "move.h"
#include "square.h"

struct Move {
    MoveAction action;
    MoveResult result;
    Square     org;
    Square     dst;

    Move(MoveAction ma, MoveResult mr, Square org, Square dst );
    Move(MovePacked pack);
    static MovePtr create(MoveAction ma, MoveResult mr, Square org, Square dst );

    MovePacked pack() const;
    void unpack(MovePacked pack);

    friend std::ostream& operator<<(std::ostream& os, const Move& mv);
};

