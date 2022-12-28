#include <iostream>
#include <algorithm>
#include <map>
#include <vector>

#include "garth.h"

enum OptionOrd {
    FEN_DIAGRAM,
    DIAGRAM,
    INTERACTIVE
};

struct OptionInfo {
    OptionOrd   ord;
    short       expected;
};

std::map<std::string,OptionInfo> opts{{"-fen",{FEN_DIAGRAM,1}},{"-d",{DIAGRAM,0}},{"-i",{INTERACTIVE,0}}};
struct {
    std::string fen;

    bool        diagram;
    bool        imode;

} options;


int main(int argc, char **argv) {
    for( int argi(1); argi < argc; ++argi ) {
        char *arg = argv[argi];
        auto found = opts.find(arg);
        if ( found != opts.end() ) {
            if ( argc - argi <= found->second.expected ) {
                std::cerr << arg << " requires " << found->second.expected << " additonal arg(s) - not found" << std::endl;
            } else {
                switch(found->second.ord) {
                    case FEN_DIAGRAM: options.fen = argv[++argi]; break;
                    case DIAGRAM:     options.diagram = true; break;
                    case INTERACTIVE: options.imode   = true; break;
                }
            }
        } else {
            std::cerr << "Unknown option " << arg << std::endl;
        }
    }

    Board b;
    MoveList ml;
    Packed p = b.pack();
    // b.get_moves(ml);
    std::cout << b.diagram() << ' ' << ml.size() << std::endl;
    Board c(false);
    c.unpack(p);
    std::cout << c.diagram() << std::endl;

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