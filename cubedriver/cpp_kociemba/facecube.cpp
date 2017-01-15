#include "facecube.h"
#include "cubiecube.h"


namespace Kociemba { namespace TwoPhase {
    
constexpr Facelet FaceCube::cornerFacelet[8][3];

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Map the edge positions to facelet positions. edgeFacelet[UR][0] e.g. gives the position of the facelet in
// the UR edge position, which defines the orientation.<br>
// edgeFacelet[UR][1] gives the position of the other facelet
constexpr Facelet FaceCube::edgeFacelet[12][2];

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Map the corner positions to facelet colors.
constexpr Color FaceCube::cornerColor[8][3];

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Map the edge positions to facelet colors.
constexpr Color FaceCube::edgeColor[12][2];
    
    
void FaceCube::toCubieCube(CubieCube &ccRet) {
    for (int i = 0; i < 8; i++)
        ccRet.cp[i] = URF;// invalidate corners
    for (int i = 0; i < 12; i++)
        ccRet.ep[i] = UR;// and edges
    Color col1, col2;
    for (Corner i : Corner_values) {
        // get the colors of the cubie at corner i, starting with U/D
        int ori;
        for (ori = 0; ori < 3; ori++)
            if (f[cornerFacelet[i][ori]] == U || f[cornerFacelet[i][ori]] == D)
                break;
        col1 = f[cornerFacelet[i][(ori + 1) % 3]];
        col2 = f[cornerFacelet[i][(ori + 2) % 3]];

        for (Corner j : Corner_values) {
            if (col1 == cornerColor[j][1] && col2 == cornerColor[j][2]) {
                // in cornerposition i we have cornercubie j
                ccRet.cp[i] = j;
                ccRet.co[i] = (unsigned char) (ori % 3);
                break;
            }
        }
    }
    for (Edge i : Edge_values)
        for (Edge j : Edge_values) {
            if (f[edgeFacelet[i][0]] == edgeColor[j][0]
                    && f[edgeFacelet[i][1]] == edgeColor[j][1]) {
                ccRet.ep[i] = j;
                ccRet.eo[i] = 0;
                break;
            }
            if (f[edgeFacelet[i][0]] == edgeColor[j][1]
                    && f[edgeFacelet[i][1]] == edgeColor[j][0]) {
                ccRet.ep[i] = j;
                ccRet.eo[i] = 1;
                break;
            }
        }
}
}}


