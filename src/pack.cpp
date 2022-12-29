// pack board into binary
#include "constants.h"
#include "board.h"

#pragma pack(1)
union jig_dw2b {
    uint64_t dw[2];
    uint8_t   b[16];
    jig_dw2b() {
        dw[0] = dw[1] = 0;
    }
};

union jig_n2b {
    uint8_t b;
    struct {
        uint8_t hi:4;
        uint8_t lo:4;
    } n;
    jig_n2b() {
        b = 0;
    }
};
#pragma pack()

BoardPacked Board::pack() const {
    BoardPacked ret;

    GameInformation gi;
    gi.f.piece_cnt = _pm.size();
    gi.f.castle_white_queenside = (_castle_white_queenside)?1:0;
    gi.f.castle_white_kingside  = (_castle_white_kingside) ?1:0;
    gi.f.castle_black_queenside = (_castle_black_queenside)?1:0;
    gi.f.castle_black_kingside  = (_castle_black_kingside) ?1:0;
    gi.f.on_move                = (_on_move == SIDE_WHITE) ?0:1;
    gi.f.en_passant             = _en_passant.rnf();
    gi.f.unused                 = 0;
    gi.f.half_move_clock        = _half_move_clock;
    gi.f.full_move_cnt          = _full_move_cnt;

    ret.f.gi = gi.i;
    ret.f.pop = ret.f.hi = ret.f.lo = 0;

    int      bit(63);
    jig_dw2b pieces;
    jig_n2b  n2b;
    bool     hilo(true);
    uint8_t *bytes = pieces.b;
    for ( short rank(R8); rank >= R1; --rank ) {
        for ( short file(Fa); file <= Fh; ++file, --bit ) {
            Square sq(rank,file);
            auto itr = _pm.find(sq);
            if ( itr != _pm.end() ) {
                ret.f.pop |= (1ULL << bit);
                if (hilo) {
                    n2b.n.hi = itr->second->byte();
                } else {
                    n2b.n.lo = itr->second->byte();
                    *bytes++ = n2b.b;
                    n2b.b = 0;
                }
                hilo = !hilo;
            }
        }       
    }
    if ( n2b.b ) {
        *bytes = n2b.b;
    }

    ret.f.lo = pieces.dw[0];
    ret.f.hi = pieces.dw[1];

    return ret;
}

void Board::unpack(BoardPacked pack) {
    jig_dw2b pieces;

    GameInformation gi;
    gi.i = pack.f.gi;
 
    _castle_white_queenside = gi.f.castle_white_queenside == 1;
    _castle_white_kingside  = gi.f.castle_white_kingside  == 1;
    _castle_black_queenside = gi.f.castle_black_queenside == 1;
    _castle_black_kingside  = gi.f.castle_black_kingside  == 1;
    _on_move                = ( gi.f.on_move == 1 ) ? SIDE_BLACK : SIDE_WHITE;
    _en_passant             = Square(RnF(gi.f.en_passant));
    _half_move_clock        = gi.f.half_move_clock;
    _full_move_cnt          = gi.f.full_move_cnt;

    _pm.clear();
    pieces.dw[0] = pack.f.lo;
    pieces.dw[1] = pack.f.hi;
    bool hilo(true);

    uint64_t map = 1ULL<<63;
    jig_n2b  n2b;
    uint8_t *bytes = pieces.b;
    uint8_t  by;

    n2b.b = *bytes++;
    for ( short rank(R8); rank >= R1; --rank ) {
        for ( short file(Fa); file <= Fh; ++file, map >>= 1ULL ) {
            if ( (pack.f.pop & map) != 0 ) {
                if ( hilo ) {
                    by = n2b.n.hi;
                } else {
                    by = n2b.n.lo;
                    n2b.b = *bytes++;
                }
                hilo = !hilo;
                PieceType pt = PieceType(by & 0x07);
                Side      s   = Side((by & 0x08)?SIDE_BLACK:SIDE_WHITE);
                set( Square(rank, file), pt, s );
            }
        }
    }
}

std::ostream& operator<<(std::ostream& os, const BoardPacked& p) {
    auto oflags = os.flags(std::ios::hex);
    auto ofill  = os.fill('0');
    auto owid   = os.width(16);

    os << p.f.gi << ','
       << p.f.pop << ','
       << p.f.hi << ','
       << p.f.lo;
    
    os.flags(oflags);
    os.fill(ofill);
    os.width(owid);

    return os;
}
