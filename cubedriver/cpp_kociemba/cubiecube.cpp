#include "cubiecube.h"
#include "facecube.h"

namespace Kociemba { namespace TwoPhase {
    
std::array<CubieCube, 6> CubieCube::moveCube;    
    
void CubieCube::toFaceCube(FaceCube &fcRet){
    for (Corner c : Corner_values) {
        int i = c;
        int j = cp[i];// cornercubie with index j is at
        // cornerposition with index i
        unsigned char ori = co[i];// Orientation of this cubie
        for (int n = 0; n < 3; n++)
            fcRet.f[FaceCube::cornerFacelet[i][(n + ori) % 3]] = FaceCube::cornerColor[j][n];
    }
    for (Edge e : Edge_values) {
        int i = e;
        int j = ep[i];// edgecubie with index j is at edgeposition
        // with index i
        unsigned char ori = eo[i];// Orientation of this cubie
        for (int n = 0; n < 2; n++)
            fcRet.f[FaceCube::edgeFacelet[i][(n + ori) % 2]] = FaceCube::edgeColor[j][n];
    }
}

}}

