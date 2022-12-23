#pragma once

#include <iostream>
#include <utility>

#include "constants.h"


struct Square : public std::pair<short,short>
{
    Square();
    Square( Rank r, File f );
    Square(short r, short f);
    Square(std::string fr);
    Square(const char *fr);
    Rank rank() const;
    File file() const;

    std::string to_string() const;
    Square operator+(const Offset& off) const;
    Square operator+=(const Offset& off);
    bool operator==(const Square& rhs);
    bool in_bounds();

    friend std::ostream& operator<<(std::ostream& os, const Square& squ);
    static Square UNBOUNDED;
};

typedef std::vector<Square> SquareList;