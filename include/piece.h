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

    void place( Square& squ );

    bool operator==(const Piece& rhs);
private:
    static const char *glyphs;
    static const short ranges[];
};

typedef std::shared_ptr<Piece>    PiecePtr;
typedef std::vector<PiecePtr>     PieceList;
typedef std::map<Square,PiecePtr> PieceMap;
