#include "coordcube.h"

namespace Kociemba { namespace TwoPhase {

int CoordCube::URtoUL_Move[N_URtoUL][N_MOVE];
int CoordCube::twistMove[N_TWIST][N_MOVE];
int CoordCube::flipMove[N_FLIP][N_MOVE];
int CoordCube::FRtoBR_Move[N_FRtoBR][N_MOVE];
int CoordCube::URFtoDLF_Move[N_URFtoDLF][N_MOVE];
int CoordCube::URtoDF_Move[N_URtoDF][N_MOVE];
int CoordCube::UBtoDF_Move[N_UBtoDF][N_MOVE];
int CoordCube::MergeURtoULandUBtoDF[336][336];
unsigned char CoordCube::Slice_URFtoDLF_Parity_Prun[N_SLICE2 * N_URFtoDLF * N_PARITY / 2];
unsigned char CoordCube::Slice_URtoDF_Parity_Prun[N_SLICE2 * N_URtoDF * N_PARITY / 2];
unsigned char CoordCube::Slice_Twist_Prun[N_SLICE1 * N_TWIST / 2 + 1];
unsigned char CoordCube::Slice_Flip_Prun[N_SLICE1 * N_FLIP / 2];
    
constexpr int CoordCube::parityMove[2][18];
    
}}
