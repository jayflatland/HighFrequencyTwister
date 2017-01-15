//FILE CUBEDEFS.H

#ifndef CUBEDEFS_H_
#define CUBEDEFS_H_


#define Color		Axis

#define NSYM_Oh			48 
#define NSYM_D4h		16
#define NMOVE			12
#define NFLIP			2048
#define NSLICE			495
#define NTWIST			2187
#define NFLIPSLICE		64430
#define NCORN6POS		20160
#define NEDGE6POS		665280
#define NEDGE4POS		11880
#define NGOAL			281816820
#define NCOSETBUF		1219276800
#define NCOSET			9754214400ULL
#define NNODE			40
#define NMBITS			4096

//*****************************************************************************
//*            Declaration of some structures and enumeration types           *
//*****************************************************************************

typedef enum {URF,UFL,ULB,UBR,DFR,DLF,DBL,DRB} Corner;
//The 8 corners of the cube

typedef enum  {UR,UF,UL,UB,DR,DF,DL,DB,FR,FL,BL,BR} Edge;
//the 12 edges of the cube

typedef enum
{	U1,U2,U3,U4,U5,U6,U7,U8,U9,R1,R2,R3,R4,R5,R6,R7,R8,R9,
	F1,F2,F3,F4,F5,F6,F7,F8,F9,D1,D2,D3,D4,D5,D6,D7,D8,D9,
	L1,L2,L3,L4,L5,L6,L7,L8,L9,B1,B2,B3,B4,B5,B6,B7,B8,B9} Facelet;
//the 54 facelets of the cube.
	
typedef enum  {mU1,mU3,mR1,mR3,mF1,mF3,mD1,mD3,mL1,mL3,mB1,mB3} Move;
//the 12 moves in the QTM (quarterturn metric)

typedef enum  {U,R,F,D,L,B} Axis;
//Used both for the 6 facelet colors and the 6 axes of faceturns

typedef enum {S_URF3,S_F2,S_U4,S_LR2} BasicSym ;
//all 48 symmetries of the cube are compositions of these 4 symmetries

typedef struct faceletCube
{	
	Color f[54];
}FaceletCube;
//A cube on the facelet level

struct corner_o
{	Corner c;
	char o;
};
//A corner cubie with an attached orientation

struct edge_o
{	Edge e;
	char o;
};
//An edge cubie with an attached orientation

typedef struct cubieCube
{	
	struct corner_o co[8];
	struct edge_o eo[12];
}CubieCube;
//A cube on the cubie level. co[x],eo[x] gives the corner/edge cubie in position x

typedef struct coordCube
{	
	int symFlipSlice;		// < NFLIPSLICE*NSYM_D4h
							// < 16 in target group
							// < 16 for idCube
							
	int edge6Pos;			// < NEDGE6POS
							// < 20160 in target group
							// 0 for idCube
							
	short int edge4Pos;		// < NEDGE4POS
							// < 24 in target group
							// 0 for idCube
															
	short int twist;		// < NTWIST
							// 0 in target group
							// 0 for idCube
	short int corn6Pos;		// < NCORN6POS
							// < NCORN6POS in target group
							// 0 for idCube
							
	short int parity;		// < 2
							// 0 in target group
							// 0 for idCube
}CoordCube;
//A cube on the coordinate level, see below for the definitions of the
//different coordinates.


typedef struct searchNode
{
	unsigned short int flipSliceU,flipSliceR,flipSliceF;
	unsigned short int symU,symR,symF;
	unsigned short int parity;
	unsigned short int twistU,twistR,twistF;
	unsigned short int corn6Pos;
	unsigned short int edge4Pos;
	int edge6Pos;
	short int movesCloserTargetU,movesCloserTargetR,movesCloserTargetF;
	short int movesAllowed;
	short int move;
	short int distU,distR,distF;
	unsigned long long mSym;	
}SearchNode;

//*****************************************************************************
//*                       global arrays and pointers                          *
//*****************************************************************************

CubieCube symCube[NSYM_Oh];
CubieCube moveCube[NMOVE];
int invSymIdx[NSYM_Oh];
int symIdxMultiply[NSYM_Oh][NSYM_Oh];
int moveConjugate[NMOVE][NSYM_Oh];
int twistConjugate[NTWIST][NSYM_D4h];
int rawFlipSliceRep[NFLIPSLICE];
char nextMove[NMBITS][NMOVE+1];
unsigned short int twistMove[NTWIST][NMOVE];
unsigned short int corn6PosMove[NCORN6POS][NMOVE];
unsigned short int edge4PosMove[NEDGE4POS][NMOVE];
int edge6PosMove[NEDGE6POS][NMOVE];
int symFlipSliceClassMove[NFLIPSLICE][NMOVE];
short *movesCloserToTarget[NTWIST];
short moveBitsConjugate[4096][NSYM_Oh];
unsigned long long ESymmetries[NMOVE],GESymmetries[NMOVE];


char *coset;

char *visitedA;
char *visitedB;

//*****************************************************************************
//*                           cubiecube.c functions                           *
//*****************************************************************************

CubieCube cubeAxMove(CubieCube cc, Axis ax);
//Do a U,R,F,D,L, or B move on the cubieCube level.

void initMoveCubes();
//Initialize the 12 QTM-moves on the cubieCube level.

CubieCube faceletCubeToCubieCube(FaceletCube fc);
//Creates a cubieCube from a faceletCube.

FaceletCube cubieCubeToFaceletCube(CubieCube cc);
//Creates a faceletCube from a cubieCube.

CubieCube stringToCubieCube(char* defString);
//Creates a cubieCube from a string. The string may be a maneuver or a
//description of the cube facelets in Singmaster notation.

void cubieCubeToString(CubieCube cc, char* defString);
//Creates a description string in Singmaster notation from a cubieCube.

void cornerMultiply(const CubieCube *a,const CubieCube *b, CubieCube *ab);
void edgeMultiply(const CubieCube *a,const CubieCube *b, CubieCube *ab);
void multiply(const CubieCube *a,const CubieCube *b, CubieCube *ab);
//Computes the composition  of the permutations a and b including orientations.

CubieCube invCubieCube(CubieCube cc);
//Computes the inverse cc^-1 of cc, so we have cc^-1*cc = idCube.

//*****************************************************************************
//*                           coordcube.c functions                           *
//*****************************************************************************

int Cnk(unsigned char n, unsigned char k);
//Binomial coefficient n choose k.

unsigned short int twist(CubieCube cc);
//Indexes the twist of the 8 corners. 0 <= twist < 3^7.

CubieCube invTwist(unsigned short int twist);
//Inverse of the above

unsigned short int flip(CubieCube cc);
//Indexes the flip of the 12 edges. 0<= flip < 2^11.

CubieCube invFlip(unsigned short int flip);
//Inverse of the above

unsigned short int corn6Pos(CubieCube cc);
//Indexes the ordered positions of the URF,UFL,ULB,UBR,DFR and DLF corner.
// 0 <= corn6Pos < 20160

CubieCube invCorn6Pos(unsigned short int idx);
//Inverse of the above

int edge6Pos(CubieCube cc);
//Indexes the 665280 ordered positions of the UR,UF,UL,UB,DR and DF edges.
//Iff the six edges are all in the U-face and D-face the index is <20160.


CubieCube invEdge6Pos(int idx);
//Inverse of the above

int edge4Pos(CubieCube cc);
//Indexes the 11880 ordered positions of the FR,FL,BL and BR edges.
//Iff the 4 edges are all in the UD-slice the index is <24

CubieCube invEdge4Pos(int idx);
//Inverse of the above

unsigned short int slice(CubieCube cc);
//Indexes the 495 unordered positions of the FR, FL, BL and BR UD-slice edges
// 0 <= slice < (12 choose 4)

CubieCube invSlice(unsigned short int slice);
//Inverse of the above

int symFlipSlice(CubieCube cc);
//indexes the 2048*495 possible configurations of the flip + slice coordinates.
//These are divided into 64430 equivalence classes with respect to the
//16 symmetries of the cube (subgroup Dh4), which preserve the UD-axis.
//The value 16*classIndex+symmmetryIndex is returned.
//2048*495=1013670 while 64430*16=1030880. This shows that some
//symFLipSlice coordinates describe the same configuration. This happens
//because there are equivalence classes where the cubes have some symmetry
//itself.

int cornerParity(CubieCube cc);
//Returns 0 if cube has even corner parity, 1 if it has odd parity. 

int edgeParity(CubieCube cc);
//Returns 0 if cube has even edge parity, 1 if it has odd parity. 

void initTwistMove();
//Initializes the twistMove table which gives the new twist of a cube A
//with twist a after applying move m.
//twist(A*moveCube[m]) = twistMove[a][m]

void initCorn6PosMove();
//similar to initTwistMove().

void initEdge6PosMove();
//similar to initTwistMove().

void initEdge4PosMove();
//similar to initTwistMove().

CoordCube cubieCubeToCoordCube(CubieCube cc);
//creates a coordCube from a cubieCube.

//*****************************************************************************
//*                            symmetry.c functions                           *
//*****************************************************************************

void initSymCubes();
//Initializes the 48 cube symmetries of the cube on the cubieCube level.
//For an index x < 48, symCube[x] gives the corresponding symmetry of the cube. 

void initInvSymIdx();
//Initializes the invSymIdx table. For a symmetry index x, invSymIdx[x] gives
//the index of the inverse symmetry: symCube[x]*symCube[ invSymIdx[x] ] =IdCube

void initSymIdxMultiply();
//Initalizes the symIdxMultiply table. For two symmetry indices x and y,
//we compute the index of the product:
//symCube[ symIdxMultiply[x][y] ] = symCube[x]*symCube[y]

void initRawFLipSliceRep();
//Two cubes A and B are called equivalent, if conjugation of one cube by a
//symmetry s in D4h (16 symmetries which preserve the UD-axis) gives the other
//cube: symCube[s]*A*symCube[s]^-1 = B
//The rawflipSlice coordinate is computed by 2048*slice + flip.
//The symFlipSliceRep table maps the index x < 64430 of a flipSlice
//equivalence class to the smallest rawflipSlice coordinate of all cubes in
//this class. In other words rawFlipSliceRep[x] gives the rawFlipSlice
//coordinate of the representant of this class. 

void initSymFlipSliceClassMove();
//Initializes the symFlipSliceClassMove table. If idx is the index of a equivalence
//class and m a move to apply, symFlipSliceClassMove[idx][m] gives the symFlipSlice
//coordinate of the resulting cube.

int symFlipSliceMove(int symFlipSlice, int m);
//computes the new symFlipSlice coordinate after applying move m.
//Because there are 64430*16 different symFlipSlice coordinates, the value is
//computed by a function and not realized as a table like twistMove.

void initMoveConjugate();
//Initializes the moveConjugate table. If m is a move and x the index of a
//symmetry, moveConjugate[m][x] gives the move m' which is the conjugation of
//move m by the the symmetry x:
//moveCube[m'] = symCube[x]*moveCube[m]*symCube[x]^-1

void initTwistConjugate();
//Initializes the twistConjugate table. If A is a cube with twist t and s is a
//symmetry, twistConjugate[t][s] gives the twist t' of the conjugate cube.
//invTwist(t') = symCube[s]*invTwist(t)*symCube[s]^-1

CubieCube edgeConjugate(CubieCube cc, int symIdx);
//Computes the conjugate cube symCube(idx)*cc*symCube(idx)^-1 for edges only.

CubieCube cornerConjugate(CubieCube cc, int symIdx);
//Computes the conjugate cube symCube(idx)*cc*symCube(idx)^-1 for corners only.

unsigned long long getSymmetries(CubieCube cc);
//Computes the symmetries of the cube cc. This is relevant if we want to reduce
//the maneuver searchspace exploiting the symmetries of the given cube.

int symCount(unsigned long long sym);
//sym is an 48 bit vector where a set bit corresponds to a given symmetry.
//symCount computes the number of symmetries given by sym.

void initGESymmetries();
//Initializes the ESymmetries and GESymmetries table. initESymmetries[m] is a
//48 bit vector where bit s is set if moveConjugate[m][s]=m, in other words m
//is a fixpoint of the symmetry s. GESymmetries is similar to ESymmetries.
//The bit s is set if moveConjugate[m][s]>=m.
//These table are needed if we want to reduce the searchspace by symmetry
//in the case the cube we solve has symmetry itself.


//*****************************************************************************
//*                            pruning.c functions                            *
//*****************************************************************************

void initMovesCloserToTarget();
//Computes the movesCloserToTarget table which acts as a pruning table. The target
//subgroup has the following property: the twist coordinate is 0 (all corner
//twist are ok), the flip cooordinate is 0 (all edge flips are ok), the UD-
//slice edges are in their slice and the edge/corner parity is even.
//Every coset is indexed by its symFlipSlice coordinate (16*classIndex +
//symmetryIndex), its twist coordinate and its parity.
//movesCloserToTarget[twist][2*classIndex+parity] is a short int where bit m with
//0<=m<12 is set when move m carries the cube closer to the target subgroup. 

void initMoveBitsConjugate();
// movesCloserToTarget applies only if symmetryIndex s=0. For s<>0
//the corresponding moves have to be conjugated by symCube(s).

int distanceToTarget(CoordCube co);
//computes the distance of the cube in QTM to the target subgroup.

void solveOptimal(CubieCube cu);
//The most important routine of the program.
//Checks all maneuvers with increasing length which transform the cube to the
//target group. Uses the movesCloserToTarget pruningtable in three different
//orientations for more effective pruning. Keeps also track of the corn6Pos, 
//edge6Pos and edge4Pos coordinates. The cube is solved, if these coordinates
//are 0 when the cube is in the target group.

void initNextMove();
//Computes the nextMove table. Given an 12-bit movesAllowed vector z
//and a move m, nextMove[z][m] returns the next move m' in z with m'>m.
//If no such move exists, -1 is returned.

//*****************************************************************************
//*                            some other stuff                               *
//*****************************************************************************

void  pp();
//print a "." to stdout.

#endif /*CUBEDEFS_H_*/
