#include <iostream>
#include <algorithm>
#include <map>
#include <vector>

#include "garth.h"

int main(int argc, char **argv) {
    // Board b;
    Board b("rnbqkbnr/2pppppp/8/8/4P3/8/PPPP1PPP/RNBQKBN1 b KQkq e3 0 1");
    MoveList ml;
    BoardPacked p = b.pack();
    b.get_moves(ml);
    std::cout << b.diagram() << ' ' << ml.size() << std::endl;
    Board c(false);
    c.unpack(p);
    std::cout << c.diagram() << std::endl;
    std::cout << b.fen() << std::endl;
    std::cout << c.fen() << std::endl;
    std::cout << p << std::endl;
    std::cout << ml.back()->pack() << std::endl;

    // for (auto m : ml)
    //     std::cout << *m << std::endl;

    // b.set(e5,PT_BISHOP,SIDE_BLACK);
    // b.set(b2,PT_PAWN,  SIDE_BLACK);
    // b.set(a1,PT_PAWN,  SIDE_WHITE);

    // std::cout << b.diagram() << std::endl;
    // b.test_for_attack(b.at(a1));

    // DiagMoveRule r;
    // r.can_attack(b, b.at(R5,Fe), Square(R8,Fh));

    // b.set(R4,Fd,PT_ROOK, SIDE_BLACK);
    // b.set(R8,Fd,PT_PAWN, SIDE_BLACK);
    // b.set(R8,Fd,PT_PAWN, SIDE_WHITE);

    // AxesMoveRule r;
    // r.can_attack(b, b.at(R4,Fd), Square(R8,Fd));

    // b.set("d4", PT_KNIGHT, SIDE_BLACK);
    // b.set("b5", PT_PAWN,   SIDE_BLACK);

    // KnightMoveRule r;
    // r.get_moves(b, b.at("d4"), ml);
    // for (auto move : ml )
    //     std::cout << *move << std::endl;

    // Board b("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
    // std::string fen(argv[1]);
    // // while( std::getline(std::cin, fen)) {
    //     std::cout << fen << std::endl;
    //     Board b(fen);
    //     MoveList ml;
    //     b.get_moves(ml);
    //     for ( MovePtr m : ml )
    //         std::cout << *m << ' ' << std::endl;
    //     std::cout << b.fen() << std::endl;
    //     std::cout << b.diagram() << std::endl;
    //     std::cout << b.test_for_check(SIDE_WHITE);
    // }
    return 0;
}