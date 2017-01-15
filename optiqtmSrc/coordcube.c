//FILE COORDCUBE.C

#include "cubedefs.h"

extern CubieCube basicCubeMove[6];
extern CubieCube idCube;

//***************************** helper functions*****************************
//***************************************************************************

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int Cnk(unsigned char n, unsigned char k)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
int s;
unsigned char i,j;
if (n<k)   return 0;
if (k>n/2)  k=n-k;
for (s=1,i=n,j=1; i!=n-k; i--,j++) {s *= i; s /= j;}
return s;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void rotateLeft(int arr[], int l, int r)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Left rotation of all array elements between l and r
{
int i, temp = arr[l];
for (i=l;i<r;i++)
 arr[i] = arr[i+1];
 arr[r] = temp;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void rotateRight(int arr[], int l, int r)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Right rotation of all array elements between l and r
{
int i, temp = arr[r];
for (i=r;i>l;i--) arr[i] = arr[i-1];
arr[l] = temp;
}

//**************************end helper functions*******************************
//*****************************************************************************


//********indexing functions and move tables without symmetry reduction********
//*****************************************************************************

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
unsigned short int twist(CubieCube cc)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
short int ret=0;
Corner c;
for (c=URF;c<DRB;c++) ret = 3*ret + cc.co[c].o;
return ret;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CubieCube invTwist(unsigned short int twist)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
int c;
CubieCube ccRet=idCube;
int twistParity=0;
for (c= DRB-1;c>=URF;c--) {twistParity += ccRet.co[c].o = twist%3; twist /=3;}
ccRet.co[DRB].o = (3 - twistParity%3)%3;
return ccRet;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
unsigned short int flip(CubieCube cc)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
short int ret=0;
Edge e;
for (e=UR;e<BR;e++) ret = 2*ret + cc.eo[e].o;
return ret;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CubieCube invFlip(unsigned short int flip)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
int e;
CubieCube ccRet=idCube;;
int flipParity=0;
for (e= BR-1;e>=UR;e--){flipParity += ccRet.eo[e].o = flip%2; flip /=2;}
ccRet.eo[BR].o = (2 - flipParity%2)%2;
return ccRet;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
unsigned short int slice(CubieCube cc)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
int a=0,x=0,j;
//compute the index a for combination (< 12 choose 4).
for (j=BR;j>=UR;j--) if (FR<=cc.eo[j].e && cc.eo[j].e<=BR ) a += Cnk(11-j,x++ +1);
return a;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CubieCube invSlice(unsigned short int slice)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
int a=slice,j,x,perm[4]={8,9,10,11};;
CubieCube ccRet=idCube;
for (j=UR;j<=BR;j++) ccRet.eo[j].e =255;//Invalidate all edges

x = 3;//generate combination and set edges
for (j=UR;j<=BR;j++)
if (a - Cnk(11-j,x+1)>=0) {ccRet.eo[j].e = perm[x]; a -=Cnk(11-j,x-- +1);}
for (j=UR,x=0;j<=BR;j++)//set the remaining edges 0..7
if (ccRet.eo[j].e == 255) ccRet.eo[j].e = x++;
return ccRet;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
unsigned short int corn6Pos(CubieCube cc)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

{
int a=0,b,x=0,j,k,perm[6];
//compute the index a < (8 choose 6) and the permutation array perm.
for (j=URF;j<=DRB;j++)
if (cc.co[j].c<=DLF) {a += Cnk(j,x+1);perm[x++] = cc.co[j].c;}

for (j=5,b=0;j>0;j--)//compute the index b < 6! for the permutation in perm
{
	k = 0;
	while (perm[j] != j) {rotateLeft(perm,0,j);k++;}
	b = (j+1)*b + k; 
}
return 720*a + b;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CubieCube invCorn6Pos(unsigned short int idx)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
int j, k, x, perm[6]={0,1,2,3,4,5};
int b = idx%720;	//Permutation 
int a = idx/720;	//Combination
CubieCube ccRet=idCube;
for (j=URF;j<=DRB;j++) ccRet.co[j].c =255;//Invalidate all corners

for (j=1;j<6;j++)//generate permutation from index b
{
	k = b%(j+1); b /= j+1;
	while (k-- >0) rotateRight(perm,0,j);
}
x = 5;//generate combination and set corners
for (j=DRB;j>=0;j--)
if (a-Cnk(j,x+1)>=0) {ccRet.co[j].c = perm[x]; a -=Cnk(j,x-- +1);}
return ccRet;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int edge6Pos(CubieCube cc)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

{
int a=0,b,x=0,j,k,perm[6];
//compute the index a < (12 choose 6) and the permutation array perm.
for (j=UR;j<=BR;j++)
if (cc.eo[j].e<=DF) {a += Cnk(j,x+1);perm[x++] = cc.eo[j].e;}

for (j=5,b=0;j>0;j--)//compute the index b < 6! for the permutation in perm
{
	k = 0;
	while (perm[j] != j) {rotateLeft(perm,0,j);k++;}
	b = (j+1)*b + k; 
}
return 720*a + b;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CubieCube invEdge6Pos(int idx)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
int j, k, x, perm[6]={0,1,2,3,4,5};
int b = idx%720;	//Permutation 
int a = idx/720;	//Combination
CubieCube ccRet=idCube;
for (j=UR;j<=BR;j++) ccRet.eo[j].e =255;//Invalidate all edges

for (j=1;j<6;j++)//generate permutation from index b
{
	k = b%(j+1); b /= j+1;
	while (k-- >0) rotateRight(perm,0,j);
}
x = 5;//generate combination and set edges
for (j=BR;j>=0;j--)
if (a-Cnk(j,x+1)>=0) {ccRet.eo[j].e = perm[x]; a -=Cnk(j,x-- +1);}
return ccRet;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int edge4Pos(CubieCube cc)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

{
int a=0,b,x=0,j,k,perm[4];
//compute the index a < (12 choose 4) and the permutation array perm.
//for (j=UR;j<=BR;j++)
//if (cc.eo[j].e<=DF) {a += Cnk(j,x+1);perm[x++] = cc.eo[j].e;}
for (j=BR;j>=UR;j--)
  if (FR<=cc.eo[j].e && cc.eo[j].e<=BR )
  {a += Cnk(11-j,x+1); perm[3-  x++] = cc.eo[j].e;}

for (j=  3,b=0;j>0;j--)//compute the index b < 4! for the permutation in perm
{
	k = 0;
	while (perm[j] != j  +8) {rotateLeft(perm,0,j);k++;}
	b = (j+1)*b + k; 
}
return 24*a + b;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CubieCube invEdge4Pos(int idx)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
int j, k, x, perm[4]={8,9,10,11};
int b = idx%24;	//Permutation 
int a = idx/24;	//Combination
CubieCube ccRet=idCube;
for (j=UR;j<=BR;j++) ccRet.eo[j].e =255;//Invalidate all edges

for (j=1;j<4;j++)//generate permutation from index b
{
	k = b%(j+1); b /= j+1;
	while (k-- >0) rotateRight(perm,0,j);
}

x = 3;//generate combination and set edges
for (j=UR;j<=BR;j++)
if (a - Cnk(11-j,x+1)>=0) {ccRet.eo[j].e = perm[3 -x]; a -=Cnk(11-j,x-- +1);}

for (j=UR,x=0;j<=BR;j++)//set the remaining edges 0..7
if (ccRet.eo[j].e == 255) ccRet.eo[j].e = x++;
return ccRet;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int cornerParity(CubieCube cc)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
int i,j,s=0;	

for (i=DRB;i>=URF+1;i--)
for (j=i-1;j>=URF;j--)
	if (cc.co[j].c>cc.co[i].c) s++;
return s%2;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int edgeParity(CubieCube cc)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
int i,j,s=0;	

for (i=BR;i>=UR+1;i--)
for (j=i-1;j>=UR;j--)
	if (cc.eo[j].e>cc.eo[i].e) s++;
return s%2;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void initTwistMove()
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
CubieCube a,b,c,d;
int i,j,m;
for (i=0;i<NTWIST;i++)
{
	a = invTwist(i);
	for (j=mU1;j<=mB3;j+=2)
	{
		m = j>>1;//extract the move axis
		cornerMultiply(&a,&basicCubeMove[m],&b);
		twistMove[i][j] = twist(b);
		cornerMultiply(&b,&basicCubeMove[m],&c);
		cornerMultiply(&c,&basicCubeMove[m],&d);
		twistMove[i][j+1] = twist(d);
		cornerMultiply(&d,&basicCubeMove[m],&a);
	}
}
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void initCorn6PosMove()
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
CubieCube a,b,c,d;
int i,j,m;
for (i=0;i<NCORN6POS;i++)
{
	a = invCorn6Pos(i);
	for (j=mU1;j<=mB3;j+=2)
	{
		m = j>>1;//extract the move axis
		cornerMultiply(&a,&basicCubeMove[m],&b);
		corn6PosMove[i][j] = corn6Pos(b);
		cornerMultiply(&b,&basicCubeMove[m],&c);
		cornerMultiply(&c,&basicCubeMove[m],&d);
		corn6PosMove[i][j+1] = corn6Pos(d);
		cornerMultiply(&d,&basicCubeMove[m],&a);
	}
}
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void initEdge6PosMove()
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
CubieCube a,b,c,d;
int i,j,m;
for (i=0;i<NEDGE6POS;i++)
{
	if (!(i%40000)) pp();
	a = invEdge6Pos(i);
	for (j=mU1;j<=mB3;j+=2)
	{
		m = j>>1;//extract the move axis
		edgeMultiply(&a,&basicCubeMove[m],&b);
		edge6PosMove[i][j] = edge6Pos(b);
		edgeMultiply(&b,&basicCubeMove[m],&c);
		edgeMultiply(&c,&basicCubeMove[m],&d);
		edge6PosMove[i][j+1] = edge6Pos(d);
		edgeMultiply(&d,&basicCubeMove[m],&a);
	}
}
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void initEdge4PosMove()
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
CubieCube a,b,c,d;
int i,j,m;
for (i=0;i<NEDGE4POS;i++)
{
	a = invEdge4Pos(i);
	for (j=mU1;j<=mB3;j+=2)
	{
		m = j>>1;//extract the move axis
		edgeMultiply(&a,&basicCubeMove[m],&b);
		edge4PosMove[i][j] = edge4Pos(b);
		edgeMultiply(&b,&basicCubeMove[m],&c);
		edgeMultiply(&c,&basicCubeMove[m],&d);
		edge4PosMove[i][j+1] = edge4Pos(d);
		edgeMultiply(&d,&basicCubeMove[m],&a);
	}
}
}

//******end indexing functions and move tables without symmetry reduction******
//*****************************************************************************


//*********indexing functions and move tables with symmetry reduction**********
//*****************************************************************************

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int symFlipSlice(CubieCube cc)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
int symIdx=0, classIdx, rawFlipSlice, rep=NFLIP*NSLICE, rBound, lBound, i;
CubieCube ccRep;
for (i=0;i<NSYM_D4h;i++)
{
	ccRep = edgeConjugate(cc,i);
	rawFlipSlice = NFLIP*slice(ccRep) + flip(ccRep);
	if (rawFlipSlice<rep) {rep = rawFlipSlice; symIdx = i;}
}	
//rep not holds the rawFlipSlice coordinate of the representant ccRep
//in the equivalence class of cube cc.
//We have cc = symCube[idx]^-1*ccRep*symCube[idx]
//Get the index of the ccRep-equivalence class from table rawFlipSliceRep now:
rBound = NFLIPSLICE;
lBound = 0;
classIdx = -1;
do
{
	classIdx = (lBound + rBound)/2;
	if (rep<rawFlipSliceRep[classIdx]) rBound = classIdx;
	else if (rep>rawFlipSliceRep[classIdx]) lBound = classIdx;
}
while (rep!=rawFlipSliceRep[classIdx]);
return (classIdx<<4) + symIdx;	
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void initSymFlipSliceClassMove()
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
int i,j,rep,flip,slice,m,n;
CubieCube ccFlip,ccSlice,b,c,d;
for(i=0;i<NFLIPSLICE;i++)
{
	if (!(i%2000)) pp();
	rep = rawFlipSliceRep[i];
	flip = rep%NFLIP;
	slice =rep/NFLIP;
	ccSlice = invSlice(slice);
	ccFlip = invFlip(flip);
	for (n=0;n<12;n++) ccFlip.eo[n].e = ccSlice.eo[n].e;//merge cubes
	for (j=mU1;j<=mB3;j+=2)
	{
		m = j>>1;//extract move axis
		edgeMultiply(&ccFlip,&basicCubeMove[m],&b);
		symFlipSliceClassMove[i][j] = symFlipSlice(b);
		edgeMultiply(&b,&basicCubeMove[m],&c);
		edgeMultiply(&c,&basicCubeMove[m],&d);
		symFlipSliceClassMove[i][j+1] = symFlipSlice(d);
		edgeMultiply(&d,&basicCubeMove[m],&ccFlip);
	}
}	
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int symFlipSliceMove(int a, int m)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
int aSym,aClass,bSym,bClass,mConj,b;
//With the abbreviations A = invSymFlipSlice(a),
//M = moveCube[m] and ASYM = symCube[aSym]
//we have A = ASYM^-1*ACLASS*ASYM with some representant ACLASS
//so A*M = ASYM^-1*ACLASS*ASYM*M =  ASYM^-1*ACLASS*(ASYM *M*ASYM^-1)*ASYM
//Defining MCONJ = ASYM *M*ASYM^-1
//we have A*M = ASYM^-1*ACLASS*MCONJ*ASYM
//Define B = ACLASS*MCONJ then we have A*M = ASYM^-1*B*ASYM
//With the abbreviation BSYM = symCube[bSym]
//We have B = BSYM^-1*BCLASS*BSYM with some representant BCLASS
//So A*M = ASYM^-1* BSYM^-1*BCLASS*BSYM*ASYM 
//= (BSYM*ASYM)^-1*BCLASS*(BSYM*ASYM)
aClass = a>>4;aSym = a&15;
mConj = moveConjugate[m][aSym];
b = symFlipSliceClassMove[aClass][mConj];
bClass = b>>4; bSym = b&15;
return (bClass<<4) + symIdxMultiply[bSym][aSym];
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CoordCube cubieCubeToCoordCube(CubieCube cc)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
CoordCube co;
co.corn6Pos = corn6Pos(cc);
co.edge6Pos = edge6Pos(cc);
co.edge4Pos = edge4Pos(cc);
co.parity = cornerParity(cc);
co.symFlipSlice = symFlipSlice(cc);
co.twist = twist(cc);
return co;
}

//*******end indexing functions and move tables with symmetry reduction********
//*****************************************************************************
