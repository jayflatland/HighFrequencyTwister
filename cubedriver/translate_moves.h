#pragma once
#include <string>

std::string translateMoves(std::string const &moves);

// Parse space separated move in common notation(s)
//  U U1 U+    -- Up face 1 quarter turn CW  
//  u U3 U- U' -- Up face 1 quarter turn CCW
//    U2       -- Up face 1 half turn
// Same for R, F, D, L, and B ( right, front, down, left, and back )
// Return pair<int face, int times> or throw std::runtime_error
//  face is 0-5 for URFDLB
//  times is 1-3 for CW, 180, CCW
//  advances str to start of next move ( or to end of string )
std::pair<int /*face*/, int /*times*/> parseMove(char const *&str);
