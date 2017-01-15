#pragma once

#include <array>

namespace Kociemba { namespace TwoPhase {

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Then names of the edge positions of the cube. Edge UR e.g., has an U(p) and R(ight) facelet.
enum Edge {
    UR, UF, UL, UB, DR, DF, DL, DB, FR, FL, BL, BR
};
static const std::array<Edge, 12> Edge_values = { UR, UF, UL, UB, DR, DF, DL, DB, FR, FL, BL, BR };


}}