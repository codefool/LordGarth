#include "move.h"

const Offset offs[] = {
	{+1,+0},  // UP
	{-1,+0},  // DN
	{+0,-1},  // LFT
	{+0,+1},  // RGT
	{+1,+1},  // UPR
	{+1,-1},  // UPL
	{-1,+1},  // DNR
	{-1,-1},  // DNL
    // knight offsets
    {+1,-2},  // KLUP
    {+2,-1},  // KUPL
    {+2,+1},  // KUPR       
    {+1,+2},  // KRUP
    {-1,+2},  // KRDN
    {-2,+1},  // KDNR
    {-2,-1},  // KDNL
    {-1,-2}   // KLDN
};

Move::Move(MoveAction ma, MoveResult mr, Square org, Square dst )
: action(ma), result(mr), org(org), dst(dst)
{}

MovePtr Move::create(MoveAction ma, MoveResult mr, Square org, Square dst ) {
    return std::make_shared<Move>(ma, mr, org, dst);
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
