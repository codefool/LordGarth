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

Move::Move(MoveAction ma, MoveResult mr, Square org, Square trg )
: action(ma), result(mr), org(org), trg(trg)
{}

MovePtr Move::create(MoveAction ma, MoveResult mr, Square org, Square trg )
{
    return std::make_shared<Move>(ma, mr, org, trg);
}

std::ostream& operator<<(std::ostream& os, const Move& mv) {
    switch (mv.action) {
	case MV_NONE            : os << "MV_NONE"; break;
	case MV_MOVE            : os << "MV_MOVE"; break;
	case MV_CAPTURE         : os << "MV_CAPTURE"; break;
	case MV_CASTLE_KINGSIDE : os << "MV_CASTLE_KINGSIDE"; break;
	case MV_CASTLE_QUEENSIDE: os << "MV_CASTLE_QUEENSIDE"; break;
	case MV_EN_PASSANT      : os << "MV_EN_PASSANT"; break;
	case MV_PROMOTION_QUEEN : os << "MV_PROMOTION_QUEEN"; break;
	case MV_PROMOTION_BISHOP: os << "MV_PROMOTION_BISHOP"; break;
	case MV_PROMOTION_KNIGHT: os << "MV_PROMOTION_KNIGHT"; break;
	case MV_PROMOTION_ROOK  : os << "MV_PROMOTION_ROOK"; break;
    }

    os << ' ';

    switch (mv.result) {
    case MR_NONE        : os << "MR_NONE"; break;
    case MR_CHECK       : os << "MR_CHECK"; break;  
    case MR_DOUBLE_CHECK: os << "MR_DOUBLE_CHECK"; break;  
    }

    os << ' ' << mv.org << "-->" << mv.trg;

    return os;
}
