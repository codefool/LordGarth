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

    inline const uint8_t byte() const { 
        uint8_t by(_t);
        if (is_black())
            by |= 0x08;
        return by;
    }

    inline const bool    is_king()     const { return _t == PT_KING;     }
    inline const bool    is_queen()    const { return _t == PT_QUEEN;    }
    inline const bool    is_bishop()   const { return _t == PT_BISHOP;   }
    inline const bool    is_knight()   const { return _t == PT_KNIGHT;   }
    inline const bool    is_rook()     const { return _t == PT_ROOK;     }
    inline const bool    is_pawn()     const { return _t == PT_PAWN;     }
    inline const bool    is_pawn_off() const { return _t == PT_PAWN_OFF; }

    void place( Square& squ );
    void promote(PieceType pt);
    void set_glyph();

    bool operator==(const Piece& rhs);
private:
    static const char *glyphs;
    static const short ranges[];
};

