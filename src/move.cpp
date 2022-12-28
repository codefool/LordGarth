#include "move.h"

Move::Move(MoveAction ma, MoveResult mr, Square org, Square dst )
: action(ma), result(mr), org(org), dst(dst)
{}

MovePtr Move::create(MoveAction ma, MoveResult mr, Square org, Square dst ) {
    return std::make_shared<Move>(ma, mr, org, dst);
}

MovePacked Move::pack() const {
    MovePacked ret;
    ret.i = 0;
    ret.f.action = action;
    ret.f.source = org.rnf();
    ret.f.target = dst.rnf();

    return ret;
}

void Move::unpack(MovePacked pack) {
    action = MoveAction(pack.f.action);
    org    = Square(RnF(pack.f.source));
    dst    = Square(RnF(pack.f.target));
}

std::ostream& operator<<(std::ostream& os, const Move& mv) {
    switch (mv.action) {
        case MV_NONE            : os << "MV_NONE"; break;
        case MV_MOVE            : os << "MV_MOVE"; break;
        case MV_CAPTURE         : os << "MV_CAPTURE"; break;
        case MV_CASTLE_KINGSIDE : os << "MV_CASTLE_KINGSIDE"; break;
        case MV_CASTLE_QUEENSIDE: os << "MV_CASTLE_QUEENSIDE"; break;
        case MV_EN_PASSANT      : os << "MV_EN_PASSANT"; break;
        case MV_PROM_QUEEN      : os << "MV_PROM_QUEEN"; break;
        case MV_PROM_BISHOP     : os << "MV_PROM_BISHOP"; break;
        case MV_PROM_KNIGHT     : os << "MV_PROM_KNIGHT"; break;
        case MV_PROM_ROOK       : os << "MV_PROM_ROOK"; break;
    }

    os << ' ';

    switch (mv.result) {
        case MR_NONE        : os << "MR_NONE"; break;
        case MR_CHECK       : os << "MR_CHECK"; break;  
        case MR_DOUBLE_CHECK: os << "MR_DOUBLE_CHECK"; break;  
    }

    os << ' ' << mv.org << "-->" << mv.dst;

    return os;
}
