#pragma once

#include <map>
#include <memory>
#include <vector>

#include "square.h"

class Piece {
private:
    PieceType _t;
    Side      _s;
    char      _g;
    Square    _q;

public:
    Piece(PieceType t, Side s);
    const PieceType type() const;
    const Side      side() const;
    const char      glyph() const;
    const short     range() const;
    const Square    square() const;
    const Rank      rank() const;
    const File      file() const;
    const bool      is_black() const;
    const bool      is_empty() const;
    const bool      moves_diag() const;
    const bool      moves_axes() const;
    const bool      moves_pawn() const;
    const bool      moves_knight() const;

    void place( Square& squ );
    void promote(PieceType pt);
    void set_glyph();

    bool operator==(const Piece& rhs);
private:
    static const char *glyphs;
    static const short ranges[];
};

