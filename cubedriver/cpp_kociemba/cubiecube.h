#pragma once
#include <string>
#include <array>

#include "corner.h"
#include "edge.h"

namespace Kociemba { namespace TwoPhase {

class FaceCube; // fwd
    
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Cube on the cubie level
class CubieCube {
public:
    // initialize to Id-Cube

    // corner permutation
    std::array<Corner, 8> cp = { { URF, UFL, ULB, UBR, DFR, DLF, DBL, DRB } };

    // corner orientation
    std::array<unsigned char, 8> co = { { 0, 0, 0, 0, 0, 0, 0, 0 } };

    // edge permutation
    std::array<Edge, 12> ep = { { UR, UF, UL, UB, DR, DF, DL, DB, FR, FL, BL, BR } };

    // edge orientation
    std::array<unsigned char, 12> eo = { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };

        
    // ************************************** Moves on the cubie level ***************************************************
private:
    static constexpr std::array<Corner, 8> cpU = { { UBR, URF, UFL, ULB, DFR, DLF, DBL, DRB } };
    static constexpr std::array<unsigned char, 8> const coU = { { 0, 0, 0, 0, 0, 0, 0, 0 } };
    static constexpr std::array<Edge, 12> const epU = { { UB, UR, UF, UL, DR, DF, DL, DB, FR, FL, BL, BR } };
    static constexpr std::array<unsigned char, 12> const eoU = { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };

    static constexpr std::array<Corner, 8> const cpR = { { DFR, UFL, ULB, URF, DRB, DLF, DBL, UBR } };
    static constexpr std::array<unsigned char, 8> const coR = { { 2, 0, 0, 1, 1, 0, 0, 2 } };
    static constexpr std::array<Edge, 12> const epR = { { FR, UF, UL, UB, BR, DF, DL, DB, DR, FL, BL, UR } };
    static constexpr std::array<unsigned char, 12> const eoR = { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };

    static constexpr std::array<Corner, 8> const cpF = { { UFL, DLF, ULB, UBR, URF, DFR, DBL, DRB } };
    static constexpr std::array<unsigned char, 8> const coF = { { 1, 2, 0, 0, 2, 1, 0, 0 } };
    static constexpr std::array<Edge, 12> const epF = { { UR, FL, UL, UB, DR, FR, DL, DB, UF, DF, BL, BR } };
    static constexpr std::array<unsigned char, 12> const eoF = { { 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0 } };

    static constexpr std::array<Corner, 8> const cpD = { { URF, UFL, ULB, UBR, DLF, DBL, DRB, DFR } };
    static constexpr std::array<unsigned char, 8> const coD = { { 0, 0, 0, 0, 0, 0, 0, 0 } };
    static constexpr std::array<Edge, 12> const epD = { { UR, UF, UL, UB, DF, DL, DB, DR, FR, FL, BL, BR } };
    static constexpr std::array<unsigned char, 12> const eoD = { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };

    static constexpr std::array<Corner, 8> const cpL = { { URF, ULB, DBL, UBR, DFR, UFL, DLF, DRB } };
    static constexpr std::array<unsigned char, 8> const coL = { { 0, 1, 2, 0, 0, 2, 1, 0 } };
    static constexpr std::array<Edge, 12> const epL = { { UR, UF, BL, UB, DR, DF, FL, DB, FR, UL, DL, BR } };
    static constexpr std::array<unsigned char, 12> const eoL = { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };

    static constexpr std::array<Corner, 8> const cpB = { { URF, UFL, UBR, DRB, DFR, DLF, ULB, DBL } };
    static constexpr std::array<unsigned char, 8> const coB = { { 0, 0, 1, 2, 0, 0, 2, 1 } };
    static constexpr std::array<Edge, 12> const epB = { { UR, UF, UL, BR, DR, DF, DL, BL, FR, FL, UB, DB } };
    static constexpr std::array<unsigned char, 12> const eoB = { { 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1 } };

    // this CubieCube array represents the 6 basic cube moves
public:       
    static std::array<CubieCube, 6> moveCube;
    static void initMoveCube(){
        moveCube[0].cp = cpU;
        moveCube[0].co = coU;
        moveCube[0].ep = epU;
        moveCube[0].eo = eoU;

        moveCube[1].cp = cpR;
        moveCube[1].co = coR;
        moveCube[1].ep = epR;
        moveCube[1].eo = eoR;

        moveCube[2].cp = cpF;
        moveCube[2].co = coF;
        moveCube[2].ep = epF;
        moveCube[2].eo = eoF;

        moveCube[3].cp = cpD;
        moveCube[3].co = coD;
        moveCube[3].ep = epD;
        moveCube[3].eo = eoD;

        moveCube[4].cp = cpL;
        moveCube[4].co = coL;
        moveCube[4].ep = epL;
        moveCube[4].eo = eoL;

        moveCube[5].cp = cpB;
        moveCube[5].co = coB;
        moveCube[5].ep = epB;
        moveCube[5].eo = eoB;
    }
    
    static void initStatics(){
        initMoveCube();
    }

    CubieCube() {

    };

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    CubieCube(Corner cp[], unsigned char co[], Edge ep[], unsigned char eo[]) {
        for (int i = 0; i < 8; i++) {
            this->cp[i] = cp[i];
            this->co[i] = co[i];
        }
        for (int i = 0; i < 12; i++) {
            this->ep[i] = ep[i];
            this->eo[i] = eo[i];
        }
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // n choose k
    static int Cnk(int n, int k) {
        int i, j, s;
        if (n < k)
            return 0;
        if (k > n / 2)
            k = n - k;
        for (s = 1, i = n, j = 1; i != n - k; i--, j++) {
            s *= i;
            s /= j;
        }
        return s;
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    static void rotateLeft(Corner arr[], int l, int r)
    // Left rotation of all array elements between l and r
    {
        Corner temp = arr[l];
        for (int i = l; i < r; i++)
            arr[i] = arr[i + 1];
        arr[r] = temp;
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    static void rotateRight(Corner arr[], int l, int r)
    // Right rotation of all array elements between l and r
    {
        Corner temp = arr[r];
        for (int i = r; i > l; i--)
            arr[i] = arr[i - 1];
        arr[l] = temp;
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    static void rotateLeft(Edge arr[], int l, int r)
    // Left rotation of all array elements between l and r
    {
        Edge temp = arr[l];
        for (int i = l; i < r; i++)
            arr[i] = arr[i + 1];
        arr[r] = temp;
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    static void rotateRight(Edge arr[], int l, int r)
    // Right rotation of all array elements between l and r
    {
        Edge temp = arr[r];
        for (int i = r; i > l; i--)
            arr[i] = arr[i - 1];
        arr[l] = temp;
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // return cube in facelet representation
        void toFaceCube(FaceCube &fcRet);

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Multiply this CubieCube with another cubiecube b, restricted to the corners.<br>
    // Because we also describe reflections of the whole cube by permutations, we get a complication with the corners. The
    // orientations of mirrored corners are described by the numbers 3, 4 and 5. The composition of the orientations
    // cannot
    // be computed by addition modulo three in the cyclic group C3 any more. Instead the rules below give an addition in
    // the dihedral group D3 with 6 elements.<br>
    //	 
    // NOTE: Because we do not use symmetry reductions and hence no mirrored cubes in this simple implementation of the
    // Two-Phase-Algorithm, some code is not necessary here.
    //	
    void cornerMultiply(CubieCube const &b) {
        Corner cPerm[8];
        unsigned char cOri[8];
        for (Corner corn : Corner_values) {
            cPerm[corn] = cp[b.cp[corn]];

            unsigned char oriA = co[b.cp[corn]];
            unsigned char oriB = b.co[corn];
            unsigned char ori = 0;
            ;
            if (oriA < 3 && oriB < 3) // if both cubes are regular cubes...
            {
                ori = (unsigned char) (oriA + oriB); // just do an addition modulo 3 here
                if (ori >= 3)
                    ori -= 3; // the composition is a regular cube

                // +++++++++++++++++++++not used in this implementation +++++++++++++++++++++++++++++++++++
            } else if (oriA < 3 && oriB >= 3) // if cube b is in a mirrored
            // state...
            {
                ori = (unsigned char) (oriA + oriB);
                if (ori >= 6)
                    ori -= 3; // the composition is a mirrored cube
            } else if (oriA >= 3 && oriB < 3) // if cube a is an a mirrored
            // state...
            {
                ori = (unsigned char) (oriA - oriB);
                if (ori < 3)
                    ori += 3; // the composition is a mirrored cube
            } else if (oriA >= 3 && oriB >= 3) // if both cubes are in mirrored
            // states...
            {
                ori = (unsigned char) (oriA - oriB);
                if (ori < 0)
                    ori += 3; // the composition is a regular cube
                // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            }
            cOri[corn] = ori;
        }
        for (Corner c : Corner_values) {
            cp[c] = cPerm[c];
            co[c] = cOri[c];
        }
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Multiply this CubieCube with another cubiecube b, restricted to the edges.
    void edgeMultiply(CubieCube const &b) {
        Edge ePerm[12];
        unsigned char eOri[12];
        for (Edge edge : Edge_values) {
            ePerm[edge] = ep[b.ep[edge]];
            eOri[edge] = (unsigned char) ((b.eo[edge] + eo[b.ep[edge]]) % 2);
        }
        for (Edge e : Edge_values) {
            ep[e] = ePerm[e];
            eo[e] = eOri[e];
        }
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Multiply this CubieCube with another CubieCube b.
    void multiply(CubieCube b) {
        cornerMultiply(b);
        // edgeMultiply(b);
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Compute the inverse CubieCube
    void invCubieCube(CubieCube c) {
        for (Edge edge : Edge_values)
            c.ep[ep[edge]] = edge;
        for (Edge edge : Edge_values)
            c.eo[edge] = eo[c.ep[edge]];
        for (Corner corn : Corner_values)
            c.cp[cp[corn]] = corn;
        for (Corner corn : Corner_values) {
            unsigned char ori = co[c.cp[corn]];
            if (ori >= 3)// Just for completeness. We do not invert mirrored
                // cubes in the program.
                c.co[corn] = ori;
            else {// the standard case
                c.co[corn] = (unsigned char) -ori;
                if (c.co[corn] < 0)
                    c.co[corn] += 3;
            }
        }
    }

    // ********************************************* Get and set coordinates *********************************************

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // return the twist of the 8 corners. 0 <= twist < 3^7
    int getTwist()const {
        int ret = 0;
        for (int i = URF; i < DRB; i++)
            ret = (int) (3 * ret + co[i]);
        return ret;
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    void setTwist(int twist) {
        int twistParity = 0;
        for (int i = DRB - 1; i >= URF; i--) {
            twistParity += co[i] = (unsigned char) (twist % 3);
            twist /= 3;
        }
        co[DRB] = (unsigned char) ((3 - twistParity % 3) % 3);
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // return the flip of the 12 edges. 0<= flip < 2^11
    int getFlip()const {
        int ret = 0;
        for (int i = UR; i < BR; i++)
            ret = (int) (2 * ret + eo[i]);
        return ret;
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    void setFlip(int flip) {
        int flipParity = 0;
        for (int i = BR - 1; i >= UR; i--) {
            flipParity += eo[i] = (unsigned char) (flip % 2);
            flip /= 2;
        }
        eo[BR] = (unsigned char) ((2 - flipParity % 2) % 2);
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Parity of the corner permutation
    int cornerParity()const {
        int s = 0;
        for (int i = DRB; i >= URF + 1; i--)
            for (int j = i - 1; j >= URF; j--)
                if (cp[j] > cp[i])
                    s++;
        return (int) (s % 2);
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Parity of the edges permutation. Parity of corners and edges are the same if the cube is solvable.
    int edgeParity()const {
        int s = 0;
        for (int i = BR; i >= UR + 1; i--)
            for (int j = i - 1; j >= UR; j--)
                if (ep[j] > ep[i])
                    s++;
        return (int) (s % 2);
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // permutation of the UD-slice edges FR,FL,BL and BR
    int getFRtoBR()const {
        int a = 0, x = 0;
        Edge edge4[4];
        // compute the index a < (12 choose 4) and the permutation array perm.
        for (int j = BR; j >= UR; j--)
            if (FR <= ep[j] && ep[j] <= BR) {
                a += Cnk(11 - j, x + 1);
                edge4[3 - x++] = ep[j];
            }

        int b = 0;
        for (int j = 3; j > 0; j--)// compute the index b < 4! for the
        // permutation in perm
        {
            int k = 0;
            while (edge4[j] != j + 8) {
                rotateLeft(edge4, 0, j);
                k++;
            }
            b = (j + 1) * b + k;
        }
        return (int) (24 * a + b);
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    void setFRtoBR(int idx) {
        int x;
        Edge sliceEdge[] = { FR, FL, BL, BR };
        Edge otherEdge[] = { UR, UF, UL, UB, DR, DF, DL, DB };
        int b = idx % 24; // Permutation
        int a = idx / 24; // Combination
        for (Edge e : Edge_values)
            ep[e] = DB;// Use UR to invalidate all edges

        for (int j = 1, k; j < 4; j++)// generate permutation from index b
        {
            k = b % (j + 1);
            b /= j + 1;
            while (k-- > 0)
                rotateRight(sliceEdge, 0, j);
        }

        x = 3;// generate combination and set slice edges
        for (int j = UR; j <= BR; j++)
            if (a - Cnk(11 - j, x + 1) >= 0) {
                ep[j] = sliceEdge[3 - x];
                a -= Cnk(11 - j, x-- + 1);
            }
        x = 0; // set the remaining edges UR..DB
        for (int j = UR; j <= BR; j++)
            if (ep[j] == DB)
                ep[j] = otherEdge[x++];

    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Permutation of all corners except DBL and DRB
    int getURFtoDLF() const{
        int a = 0, x = 0;
        Corner corner6[6];
        // compute the index a < (8 choose 6) and the corner permutation.
        for (int j = URF; j <= DRB; j++)
            if (cp[j] <= DLF) {
                a += Cnk(j, x + 1);
                corner6[x++] = cp[j];
            }

        int b = 0;
        for (int j = 5; j > 0; j--)// compute the index b < 6! for the
        // permutation in corner6
        {
            int k = 0;
            while (corner6[j] != j) {
                rotateLeft(corner6, 0, j);
                k++;
            }
            b = (j + 1) * b + k;
        }
        return (int) (720 * a + b);
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    void setURFtoDLF(int idx) {
        int x;
        Corner corner6[] = { URF, UFL, ULB, UBR, DFR, DLF };
        Corner otherCorner[] = { DBL, DRB };
        int b = idx % 720; // Permutation
        int a = idx / 720; // Combination
        for (Corner c : Corner_values)
            cp[c] = DRB;// Use DRB to invalidate all corners

        for (int j = 1, k; j < 6; j++)// generate permutation from index b
        {
            k = b % (j + 1);
            b /= j + 1;
            while (k-- > 0)
                rotateRight(corner6, 0, j);
        }
        x = 5;// generate combination and set corners
        for (int j = DRB; j >= 0; j--)
            if (a - Cnk(j, x + 1) >= 0) {
                cp[j] = corner6[x];
                a -= Cnk(j, x-- + 1);
            }
        x = 0;
        for (int j = URF; j <= DRB; j++)
            if (cp[j] == DRB)
                cp[j] = otherCorner[x++];
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Permutation of the six edges UR,UF,UL,UB,DR,DF.
    int getURtoDF() const {
        int a = 0, x = 0;
        Edge edge6[6];
        // compute the index a < (12 choose 6) and the edge permutation.
        for (int j = UR; j <= BR; j++)
            if (ep[j] <= DF) {
                a += Cnk(j, x + 1);
                edge6[x++] = ep[j];
            }

        int b = 0;
        for (int j = 5; j > 0; j--)// compute the index b < 6! for the
        // permutation in edge6
        {
            int k = 0;
            while (edge6[j] != j) {
                rotateLeft(edge6, 0, j);
                k++;
            }
            b = (j + 1) * b + k;
        }
        return 720 * a + b;
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    void setURtoDF(int idx) {
        int x;
        Edge edge6[] = { UR, UF, UL, UB, DR, DF };
        Edge otherEdge[] = { DL, DB, FR, FL, BL, BR };
        int b = idx % 720; // Permutation
        int a = idx / 720; // Combination
        for (Edge e : Edge_values)
            ep[e] = BR;// Use BR to invalidate all edges

        for (int j = 1, k; j < 6; j++)// generate permutation from index b
        {
            k = b % (j + 1);
            b /= j + 1;
            while (k-- > 0)
                rotateRight(edge6, 0, j);
        }
        x = 5;// generate combination and set edges
        for (int j = BR; j >= 0; j--)
            if (a - Cnk(j, x + 1) >= 0) {
                ep[j] = edge6[x];
                a -= Cnk(j, x-- + 1);
            }
        x = 0; // set the remaining edges DL..BR
        for (int j = UR; j <= BR; j++)
            if (ep[j] == BR)
                ep[j] = otherEdge[x++];
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Permutation of the six edges UR,UF,UL,UB,DR,DF
    static int getURtoDF(int idx1, int idx2) {
        CubieCube a;
        CubieCube b;
        a.setURtoUL(idx1);
        b.setUBtoDF(idx2);
        for (int i = 0; i < 8; i++) {
            if (a.ep[i] != BR)
                if (b.ep[i] != BR)// collision
                    return -1;
                else
                    b.ep[i] = a.ep[i];
        }
        return b.getURtoDF();
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Permutation of the three edges UR,UF,UL
    int getURtoUL() const{
        int a = 0, x = 0;
        Edge edge3[3];
        // compute the index a < (12 choose 3) and the edge permutation.
        for (int j = UR; j <= BR; j++)
            if (ep[j] <= UL) {
                a += Cnk(j, x + 1);
                edge3[x++] = ep[j];
            }

        int b = 0;
        for (int j = 2; j > 0; j--)// compute the index b < 3! for the
        // permutation in edge3
        {
            int k = 0;
            while (edge3[j] != j) {
                rotateLeft(edge3, 0, j);
                k++;
            }
            b = (j + 1) * b + k;
        }
        return (int) (6 * a + b);
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    void setURtoUL(int idx) {
        int x;
        Edge edge3[] = { UR, UF, UL };
        int b = idx % 6; // Permutation
        int a = idx / 6; // Combination
        for (Edge e : Edge_values)
            ep[e] = BR;// Use BR to invalidate all edges

        for (int j = 1, k; j < 3; j++)// generate permutation from index b
        {
            k = b % (j + 1);
            b /= j + 1;
            while (k-- > 0)
                rotateRight(edge3, 0, j);
        }
        x = 2;// generate combination and set edges
        for (int j = BR; j >= 0; j--)
            if (a - Cnk(j, x + 1) >= 0) {
                ep[j] = edge3[x];
                a -= Cnk(j, x-- + 1);
            }
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Permutation of the three edges UB,DR,DF
    int getUBtoDF() const{
        int a = 0, x = 0;
        Edge edge3[3];
        // compute the index a < (12 choose 3) and the edge permutation.
        for (int j = UR; j <= BR; j++)
            if (UB <= ep[j] && ep[j] <= DF) {
                a += Cnk(j, x + 1);
                edge3[x++] = ep[j];
            }

        int b = 0;
        for (int j = 2; j > 0; j--)// compute the index b < 3! for the
        // permutation in edge3
        {
            int k = 0;
            while (edge3[j] != UB + j) {
                rotateLeft(edge3, 0, j);
                k++;
            }
            b = (j + 1) * b + k;
        }
        return (int) (6 * a + b);
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    void setUBtoDF(int idx) {
        int x;
        Edge edge3[3] = { UB, DR, DF };
        int b = idx % 6; // Permutation
        int a = idx / 6; // Combination
        for (Edge e : Edge_values)
            ep[e] = BR;// Use BR to invalidate all edges

        for (int j = 1, k; j < 3; j++)// generate permutation from index b
        {
            k = b % (j + 1);
            b /= j + 1;
            while (k-- > 0)
                rotateRight(edge3, 0, j);
        }
        x = 2;// generate combination and set edges
        for (int j = BR; j >= 0; j--)
            if (a - Cnk(j, x + 1) >= 0) {
                ep[j] = edge3[x];
                a -= Cnk(j, x-- + 1);
            }
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    int getURFtoDLB() const {
        Corner perm[6];
        int b = 0;
        for (int i = 0; i < 8; i++)
            perm[i] = cp[i];
        for (int j = 7; j > 0; j--)// compute the index b < 8! for the permutation in perm
        {
            int k = 0;
            while (perm[j] != j) {
                rotateLeft(perm, 0, j);
                k++;
            }
            b = (j + 1) * b + k;
        }
        return b;
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    void setURFtoDLB(int idx) {
        Corner perm[] = { URF, UFL, ULB, UBR, DFR, DLF, DBL, DRB };
        int k;
        for (int j = 1; j < 8; j++) {
            k = idx % (j + 1);
            idx /= j + 1;
            while (k-- > 0)
                rotateRight(perm, 0, j);
        }
        int x = 7;// set corners
        for (int j = 7; j >= 0; j--)
            cp[j] = perm[x--];
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    int getURtoBR() const{
        Edge perm[12];
        int b = 0;
        for (int i = 0; i < 12; i++)
            perm[i] = ep[i];
        for (int j = 11; j > 0; j--)// compute the index b < 12! for the permutation in perm
        {
            int k = 0;
            while (perm[j] != j) {
                rotateLeft(perm, 0, j);
                k++;
            }
            b = (j + 1) * b + k;
        }
        return b;
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    void setURtoBR(int idx) {
        Edge perm[] = { UR, UF, UL, UB, DR, DF, DL, DB, FR, FL, BL, BR };
        int k;
        for (int j = 1; j < 12; j++) {
            k = idx % (j + 1);
            idx /= j + 1;
            while (k-- > 0)
                rotateRight(perm, 0, j);
        }
        int x = 11;// set edges
        for (int j = 11; j >= 0; j--)
            ep[j] = perm[x--];
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Check a cubiecube for solvability. Return the error string.
    // nullptr: Cube is solvable
    char const *verify() const{
        int sum = 0;
        int edgeCount[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        for (Edge e : Edge_values)
            edgeCount[ep[e]]++;
        for (int i = 0; i < 12; i++)
            if (edgeCount[i] != 1)
                return "wrong edge count"; // -2: Not all 12 edges exist exactly once

        for (int i = 0; i < 12; i++)
            sum += eo[i];
        if (sum % 2 != 0)
            return "edge flipped"; // -3: Flip error: One edge has to be flipped

        int cornerCount[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
        for (Corner c : Corner_values)
            cornerCount[cp[c]]++;
        for (int i = 0; i < 8; i++)
            if (cornerCount[i] != 1)
                return "wrong corner count";// -4: Not all corners exist exactly once

        sum = 0;
        for (int i = 0; i < 8; i++)
            sum += co[i];
        if (sum % 3 != 0)
            return "corner twisted";// -5: Twist error: One corner has to be twisted

        if ((edgeParity() ^ cornerParity()) != 0)
            return "parity";// -6: Parity error: Two corners ore two edges have to be exchanged

        return "";// cube ok
    }
};
}}

