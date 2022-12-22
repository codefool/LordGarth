#include <sstream>

#include "square.h"

Square::Square()
: pair()
{}

Square::Square( Rank r, File f )
: pair(r, f)
{}

Square::Square(short r, short f) 
: pair(r, f)
{}

Square::Square(std::string fr)
: Square(fr.c_str())
{}

Square::Square(const char *fr) {
    char f = std::tolower(fr[0]);
    char r = fr[1];
    // ranks are 1 origin
    first  = (Rank)( r - '1' );
    second = (File)( f - 'a' );
}

Rank Square::rank() const { return (Rank)first; }
File Square::file() const { return (File)second; }

std::string Square::to_string() const {
    std::stringstream ss;
    ss << char('a' + second) << char('1' + first);
    return ss.str();
}

Square Square::operator+(const Offset& off) const {
    return Square(first + off.first, second + off.second);
}

Square Square::operator+=(const Offset& off) {
    first  += off.first;
    second += off.second;
    return *this;
}

bool Square::operator==(const Square& rhs) {
    return first == rhs.first
        && second == rhs.second;
}

bool Square::in_bounds() {
    return R1 <= first && first <= R8 && Fa <= second && second <= Fh;
}

Square Square::UNBOUNDED(-1,-1);

std::ostream& operator<<(std::ostream& os, const Square& squ) {
    os << squ.to_string();
    return os;
}
