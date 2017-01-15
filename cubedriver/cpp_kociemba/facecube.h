#pragma once

#include "color.h"
#include "corner.h"
#include "edge.h"
#include "facelet.h"

namespace Kociemba { namespace TwoPhase {

class CubieCube; //fwd

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Cube on the facelet level
class FaceCube {
public:    
    std::array<Color, 54> f = { { 
        U, U, U, U, U, U, U, U, U,
        R, R, R, R, R, R, R, R, R,
        F, F, F, F, F, F, F, F, F,
        D, D, D, D, D, D, D, D, D,
        L, L, L, L, L, L, L, L, L,
        B, B, B, B, B, B, B, B, B
    } };

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Map the corner positions to facelet positions. cornerFacelet[URF][0] e.g. gives the position of the
    // facelet in the URF corner position, which defines the orientation.<br>
    // cornerFacelet[URF][1] and cornerFacelet[URF][2] give the position of the other two facelets
    // of the URF corner (clockwise).
        static constexpr Facelet cornerFacelet[8][3] = { { U9, R1, F3 }, { U7, F1, L3 }, { U1, L1, B3 }, { U3, B1, R3 },
                                                         { D3, F9, R7 }, { D1, L9, F7 }, { D7, B9, L7 }, { D9, R9, B7 } };

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Map the edge positions to facelet positions. edgeFacelet[UR][0] e.g. gives the position of the facelet in
    // the UR edge position, which defines the orientation.<br>
    // edgeFacelet[UR][1] gives the position of the other facelet
    static constexpr Facelet edgeFacelet[12][2] = { { U6, R2 }, { U8, F2 }, { U4, L2 }, { U2, B2 }, { D6, R8 }, { D2, F8 },
                                                    { D4, L8 }, { D8, B8 }, { F6, R4 }, { F4, L6 }, { B6, L4 }, { B4, R6 } };

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Map the corner positions to facelet colors.
    static constexpr Color cornerColor[8][3]  = { { U, R, F }, { U, F, L }, { U, L, B }, { U, B, R },
                                                  { D, F, R }, { D, L, F }, { D, B, L }, { D, R, B } };

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Map the edge positions to facelet colors.
    static constexpr Color edgeColor[12][2] = { { U, R }, { U, F }, { U, L }, { U, B }, 
                                                { D, R }, { D, F }, { D, L }, { D, B },
                                                { F, R }, { F, L }, { B, L }, { B, R } };

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    FaceCube() {
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Construct a facelet cube from a string
    FaceCube(std::string cubeString){
        for (int i = 0; i < cubeString.length(); i++){
            if( i < 54 ){
                switch(cubeString[i]){
                case 'U': f[i] = U; break;
                case 'R': f[i] = R; break;
                case 'F': f[i] = F; break;
                case 'D': f[i] = D; break;
                case 'L': f[i] = L; break;
                case 'B': f[i] = B; break;
                }
            }
        }
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Gives string representation of a facelet cube
    std::string to_String() {
        std::string s;
        for (int i = 0; i < 54; i++)
            s += "URFDLB"[ f[i] ];
        return s;
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Gives CubieCube representation of a faceletcube
    void toCubieCube(CubieCube &ccRet);
};

}}

