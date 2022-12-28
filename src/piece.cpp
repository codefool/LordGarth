#include "piece.h"

Piece::Piece(PieceType t, Side s)
: _t(t), _s(s)
{
    set_glyph();
}

const PieceType Piece::type() const { return _t; }
const Side      Piece::side() const { return _s; }
const char      Piece::glyph() const { return _g; }
const short     Piece::range() const { return ranges[_t]; }
const Square    Piece::square() const { return _q; }
const Rank      Piece::rank() const { return (Rank)_q.first; }
const File      Piece::file() const { return (File)_q.second; }
const bool      Piece::is_black() const { return _s == SIDE_BLACK; }
const bool      Piece::is_empty() const { return _t == PT_EMPTY; }

const bool Piece::moves_diag() const {
    return is_king() || is_queen() || is_bishop();
}
const bool Piece::moves_axes() const {
    return is_king() || is_queen() || is_rook();
}
const bool Piece::moves_pawn() const {
    return is_pawn() || is_pawn_off();
}
const bool Piece::moves_knight() const {
    return is_knight();
}

void Piece::place( Square& squ ) { _q = squ; }

void Piece::promote( PieceType pt ) { 
    _t = pt; 
    set_glyph();
}

void Piece::set_glyph() {
    _g = glyphs[_t];
    if ( is_black() ) 
        _g = std::tolower(_g);
}

bool Piece::operator==(const Piece& rhs) {
    return _t == rhs._t
        && _s == rhs._s
        && _g == rhs._g
        && _q == rhs._q;
}

// piece constants, indexed by piece type ordinal
const char *Piece::glyphs = ".KQBNRPP";
// piece ranges  -1 indicates custom range
const short Piece::ranges[] = { -1, 1, 7, 7, 1, 7, 1 };
