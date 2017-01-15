//FILE PRUNING.C

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include "cubedefs.h"

extern int subOptLev;
extern int symRed;

//movesDefault[m] gives the moves which are allowed after move m by default.
//Each entry is a 12-bit array with the bit m* set if move m* is allowed
//after move m.
const short int movesDefault[12] =
//U:U'; U':U,U'; R:R'; R':R,R'; F:F'; F':F,F';
//D:D',U,U'; D':D,D',U,U'; L:L',R,R'; L':L,L',R,R'; B:B',F,F'; B':B,B',F,F'
{0xffd,0xffc,0xff7,0xff3,0xfdf,0xfcf,0xf7c,0xf3c,0xdf3,0xcf3,0x7cf,0x3cf};
//L':L,L',R,R' means for example: after a L' move, the moves L,L'R,R' are
//not allowed. moveBitsDefault[mL3]=moveBitsDefault[9]=0xcf3 = 0b110011110011
//has the corresponding four bits cleared.

char *mv[] = {"U","U'","R","R'","F","F'","D","D'","L","L'","B","B'"};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void initMovesCloserToTarget()
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
FILE *tableSave;
char fileName[] = {"pruntable"};
int flipSlice,twist,parity,symFlipSlice1,sym1,flipSlice1,twist1,parity1,i;
unsigned long idx,idx1,count=1;
Move m;

tableSave= fopen(fileName,"r+b");
if (tableSave==NULL) goto MAKETABLE;
fseek(tableSave, 0, SEEK_END);
if (ftell(tableSave)!=NFLIPSLICE*NTWIST*4) {fclose(tableSave);goto MAKETABLE;}
fseek(tableSave, 0, SEEK_SET);
printf("\nloading pruning table (538 MB) from disk...");
for (i=0;i<NTWIST;i++)
{
	if (!(i%200)) pp();
	fread(movesCloserToTarget[i],NFLIPSLICE*4,1,tableSave);
}
fclose(tableSave);
free(visitedA); free(visitedB);
return;

MAKETABLE:
printf("\ngenerating pruning table (538 MB), this may take half an hour.\n");
fflush(stdout);
visitedA[0] = 1;visitedB[0] = 1;
while (count != NGOAL)
{
	for (flipSlice=0;flipSlice<NFLIPSLICE;flipSlice++)//Index of equivalence class
	for (twist=0;twist<NTWIST;twist++)
	for (parity=0;parity<2;parity++)
	{
		idx = ((NTWIST*flipSlice + twist)<<1) + parity;
		if (visitedA[idx>>3] & 1<<(idx&7)) continue;//occupied
		else
		{
			for (m=mU1;m<=mB3;m++)
			{
				parity1 = parity ^ 1;//quarter turn changes parity			
				symFlipSlice1 = symFlipSliceClassMove[flipSlice][m];
				sym1 = symFlipSlice1 & 15;
				flipSlice1 = symFlipSlice1>>4;
				twist1 = twistMove[twist][m];
				twist1 = twistConjugate[twist1][sym1];
				idx1 = ((NTWIST*flipSlice1 + twist1)<<1) + parity1;
				if (visitedA[idx1>>3] & 1<<(idx1&7))//occupied, so closer to goal
				{
					movesCloserToTarget[twist][(flipSlice<<1)+parity] |= 1<<m;//set bit for this move
					if (!(visitedB[idx>>3] & 1<<(idx&7)))
					{visitedB[idx>>3] |= 1<<(idx&7);count++;}
				} 
			}		
		}
	}
	for (i=0;i<NGOAL/8+1;i++) visitedA[i] |= visitedB[i];
	printf("%9lu of %u entries done.\n",count,NGOAL);
	fflush(stdout);
}
printf("saving pruning table (538 MB) to disk...");
fflush(stdout);
tableSave= fopen(fileName,"w+b");
for (i=0;i<NTWIST;i++) fwrite(movesCloserToTarget[i],NFLIPSLICE*4,1,tableSave);
fclose(tableSave);

free(visitedA); free(visitedB);
printf(" done.\n");
printf("on the next run the table will be loaded from disk.\n");
fflush(stdout);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void initMoveBitsConjugate()
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
int m,s,n;

for (n=0;n<NMBITS;n++)
for (s=0;s<NSYM_Oh;s++)
{
	moveBitsConjugate[n][s] = 0;
	for (m=mU1;m<=mB3;m++)
	{
		if (n & 1<<m) moveBitsConjugate[n][s] |= 1<<moveConjugate[m][invSymIdx[s]];
	}
}
}
	
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int distanceToTarget(CoordCube co)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
int symFlipSlice = co.symFlipSlice;
int flipSlice = symFlipSlice>>4;
int sym = symFlipSlice&15;
int twist = co.twist;
int twistConj = twistConjugate[co.twist][sym];
int parity = co.parity;
int dist = 0;
short int move,a;

while (1)
{
	a = movesCloserToTarget[twistConj][(flipSlice<<1)+parity];
	if (a == 0) break;//no more moves which reduce distance, so we are finished
	a = moveBitsConjugate[a][sym];
	for (move=0;move<NMOVE;move++)
	{
		if (a & 1<<move)
		{
			dist++;
			symFlipSlice = symFlipSliceMove(symFlipSlice,move);
			sym = symFlipSlice&15;
			flipSlice = symFlipSlice>>4;
			twist = twistMove[twist][move];
			twistConj = twistConjugate[twist][sym];
			parity ^= 1;
			break;
		}
	} 	
}
return dist;	
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void initNextMove()
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
int i,j,k;
for (i=0;i<NMBITS;i++)
for (j=0;j<=NMOVE;j++)//<=NMOVE is correct!
	nextMove[i][j]=-1; //no next move is default
	
for (i=0;i<NMBITS;i++)
for (j=0;j<NMOVE;j++)
for (k=j;k<NMOVE;k++) 
{
	if (i & 1<<k) { nextMove[i][j] = k; break;}
}
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void solveOptimal(CubieCube cu)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
//------------------------initialisation---------------------------------------
int b,mConjU,mConjR,mConjF,twistConjU,twistConjR,twistConjF;
CoordCube coU,coR,coF;
int r_depth,optimalDist=99;
short int m;
SearchNode sn[NNODE];//36 is enough
SearchNode *snP,*snPNew;
int manLength;
unsigned long long sym,sym1,nodes=0,tests=0;

sym= getSymmetries(cu);
if (sym==1) printf("Cube has no symmetry.\n");
else printf("Cube has %u symmetries.\n",symCount(sym));

if (subOptLev==0)  printf("Computing all optimal solutions ");
else if (subOptLev==1) printf("Computing suboptimal solutions (+%d level) ",subOptLev);
else if (subOptLev>1) printf("Computing suboptimal solutions (+%d levels) ",subOptLev);
if (symRed==1 && subOptLev!=-1) printf("up to symmetry");
if (symRed==0 && subOptLev!=-1) printf("without symmetry reduction");
printf("\n");fflush(stdout);

//rotate the cube along URF-diagonal, do get three different orientations
coU = cubieCubeToCoordCube(cu);
cu = edgeConjugate(cu,16);//conjugation S_URF3*cu*S_URF3^-1
cu = cornerConjugate(cu,16);
coR = cubieCubeToCoordCube(cu);//R-face now shows up
cu = edgeConjugate(cu,16);
cu = cornerConjugate(cu,16);
coF = cubieCubeToCoordCube(cu);//F-face now shows up

snP = sn;
snP->distU = distanceToTarget(coU);
snP->distR = distanceToTarget(coR);
snP->distF = distanceToTarget(coF);	

snP->flipSliceU = coU.symFlipSlice>>4;
snP->flipSliceR = coR.symFlipSlice>>4;
snP->flipSliceF = coF.symFlipSlice>>4;	
	
snP->symU = coU.symFlipSlice&15;
snP->symR = coR.symFlipSlice&15;
snP->symF = coF.symFlipSlice&15;
	
snP->parity = coU.parity;//the same for all three cube directions

snP->twistU = coU.twist;
snP->twistR = coR.twist;
snP->twistF = coF.twist;
	
snP->corn6Pos = coU.corn6Pos;//sufficient to use only U-direction
snP->edge6Pos = coU.edge6Pos;
snP->edge4Pos = coU.edge4Pos;

for (b=0;b<NNODE;b++) sn[b].move = mU1-1;
	
twistConjU=twistConjugate[snP->twistU][snP->symU];
twistConjR=twistConjugate[snP->twistR][snP->symR];
twistConjF=twistConjugate[snP->twistF][snP->symF];
	

snP->movesCloserTargetU= moveBitsConjugate[movesCloserToTarget[twistConjU]
	          [(snP->flipSliceU<<1)+snP->parity]][snP->symU];
//movesCloserTargetU now holds a 12-bit array with m set if move m
//reduces the distance to the target group by 1.
//the same in the other two orientations:
snP->movesCloserTargetR= moveBitsConjugate[movesCloserToTarget[twistConjR]
	          [(snP->flipSliceR<<1)+snP->parity]][symIdxMultiply[snP->symR][16]];
snP->movesCloserTargetF= moveBitsConjugate[movesCloserToTarget[twistConjF]
	          [(snP->flipSliceF<<1)+snP->parity]][symIdxMultiply[snP->symF][32]];
	          	                              
snP->movesAllowed = 0xfff;//all moves are allowed for the first node
snP->mSym = sym;

if (snP->distU==0 && snP->distR==0 && snP->distF==0 ) r_depth = manLength=2;
else
{   //find maximum of the three distances
	r_depth = manLength =
	snP->distU > snP->distR ? (snP->distU > snP->distF ? snP->distU : snP->distF)
	: (snP->distR > snP->distF ? snP->distR : snP->distF);
	

	if (snP->distU==manLength) snP->movesAllowed &= snP->movesCloserTargetU;
	if (snP->distR==manLength) snP->movesAllowed &= snP->movesCloserTargetR;
	if (snP->distF==manLength) snP->movesAllowed &= snP->movesCloserTargetF;
	
}
//------------------------end of initialisation--------------------------------

while( 1 ) {
	snP->move = nextMove[snP->movesAllowed][++(snP->move)];
	if (snP->move ==-1)//no more possible move, go to left node
	{
		if (r_depth==manLength)
		{
			printf("depth %2u completed, %14"PRIu64" nodes",manLength,nodes);
			printf(", %14"PRIu64" tests\n",tests);
			fflush(stdout);
			r_depth +=2;
			manLength = r_depth;
			if (manLength>optimalDist+2*subOptLev) return;
			snP->movesAllowed = 0xfff;
			snP->mSym = sym;
		}
		else {r_depth++; snP--;}
		continue;
	}

	m=snP->move;
	sym1= snP->mSym;
	//check if a symmetry transformation of the maneuver leads to a maneuver
	//which is lexically earlier. Ignore those moves.
	if (symRed==1 && (sym1 & GESymmetries[m]) != sym1) continue;

	if (manLength-r_depth>=2)//Prevent UUU, RRR, FFF...
	if (m==(snP-1)->move && m==(snP-2)->move) continue;

	snPNew = snP + 1;

	//coordinates relevant if cube is in target subgroup (all zero -> Id-cube)
	snPNew->edge6Pos=edge6PosMove[snP->edge6Pos][m];
	snPNew->edge4Pos=edge4PosMove[snP->edge4Pos][m];
	snPNew->corn6Pos=corn6PosMove[snP->corn6Pos][m];

	if (r_depth==1)//maneuver complete
	{
		tests++;
		if (snPNew->corn6Pos==0 && snPNew->edge6Pos==0 && snPNew->edge4Pos==0)
		{
			if (optimalDist>manLength) optimalDist=manLength;
			for (b=0;b<manLength;b++) printf("%s ",mv[sn[b].move]);
			printf(" (%uq*)\n",manLength);
			if (subOptLev==-1 ){printf("\n");return;}
		}
		continue;
	}

	//fill in information for right node

	//distances to target for all three cube orientations
	if (snP->movesCloserTargetU & 1<<m) snPNew->distU = snP->distU-1;
		else snPNew->distU = snP->distU+1;
	if (snP->movesCloserTargetR & 1<<m) snPNew->distR = snP->distR-1;
		else snPNew->distR = snP->distR+1;
	if (snP->movesCloserTargetF & 1<<m) snPNew->distF = snP->distF-1;
		else snPNew->distF = snP->distF+1;

	//new coset coordinates for right node
	snPNew->parity = 1 - snP->parity;
	snPNew->mSym = snP->mSym & ESymmetries[m];

	mConjU = moveConjugate[m][snP->symU];
	b = symFlipSliceClassMove[snP->flipSliceU][mConjU];
	snPNew->flipSliceU = b>>4;
	snPNew->symU = symIdxMultiply[b&15][snP->symU];
	snPNew->twistU = twistMove[snP->twistU][m];

	m=moveConjugate[m][16];
	mConjR = moveConjugate[m][snP->symR];
	b = symFlipSliceClassMove[snP->flipSliceR][mConjR];
	snPNew->flipSliceR = b>>4;
	snPNew->symR = symIdxMultiply[b&15][snP->symR];
	snPNew->twistR = twistMove[snP->twistR][m];

	m=moveConjugate[m][16];
	mConjF = moveConjugate[m][snP->symF];
	b = symFlipSliceClassMove[snP->flipSliceF][mConjF];
	snPNew->flipSliceF = b>>4;
	snPNew->symF = symIdxMultiply[b&15][snP->symF];
	snPNew->twistF = twistMove[snP->twistF][m];

	twistConjU=twistConjugate[snPNew->twistU][snPNew->symU];
	twistConjR=twistConjugate[snPNew->twistR][snPNew->symR];
	twistConjF=twistConjugate[snPNew->twistF][snPNew->symF];

	//12bit vectors hold information which moves carry the cube closer
	//to the target group.
	snPNew->movesCloserTargetU= moveBitsConjugate[movesCloserToTarget[twistConjU]
	                [(snPNew->flipSliceU<<1)+snPNew->parity]][snPNew->symU];
	snPNew->movesCloserTargetR= moveBitsConjugate[movesCloserToTarget[twistConjR]
	                [(snPNew->flipSliceR<<1)+snPNew->parity]][symIdxMultiply[snPNew->symR][16]];
	snPNew->movesCloserTargetF= moveBitsConjugate[movesCloserToTarget[twistConjF]
	                [(snPNew->flipSliceF<<1)+snPNew->parity]][symIdxMultiply[snPNew->symF][32]];

	//compute which moves are allowed for this node
	snPNew->movesAllowed = movesDefault[snP->move];
	if (snPNew->distU== --r_depth) snPNew->movesAllowed &= snPNew->movesCloserTargetU;
	if (snPNew->distR== r_depth) snPNew->movesAllowed &= snPNew->movesCloserTargetR;
	if (snPNew->distF== r_depth) snPNew->movesAllowed &= snPNew->movesCloserTargetF;

	//now go to right move
	nodes++;snP++;
	}

}

