//FILE SYMMETRY.C

#include <assert.h>
#include "cubedefs.h"

extern CubieCube idCube;

//basic cubie level rotations and reflections
//+++ the orientation of a reflected corner has values from 3 to 5 ++++++++++++
const CubieCube basicSymCube[4] =
{
	{{{URF,1},{DFR,2},{DLF,1},{UFL,2},{UBR,2},{DRB,1},{DBL,2},{ULB,1}},
	{{UF,1},{FR,0},{DF,1},{FL,0},{UB,1},{BR,0},
	{DB,1},{BL,0},{UR,1},{DR,1},{DL,1},{UL,1}}},//S_URF3
	{{{DLF,0},{DFR,0},{DRB,0},{DBL,0},{UFL,0},{URF,0},{UBR,0},{ULB,0}},
	{{DL,0},{DF,0},{DR,0},{DB,0},{UL,0},{UF,0},
	{UR,0},{UB,0},{FL,0},{FR,0},{BR,0},{BL,0}}},//S_F2
	{{{UBR,0},{URF,0},{UFL,0},{ULB,0},{DRB,0},{DFR,0},{DLF,0},{DBL,0}},
	{{UB,0},{UR,0},{UF,0},{UL,0},{DB,0},{DR,0},
	{DF,0},{DL,0},{BR,1},{FR,1},{FL,1},{BL,1}}},//S_U4
	{{{UFL,3},{URF,3},{UBR,3},{ULB,3},{DLF,3},{DFR,3},{DRB,3},{DBL,3}},
	{{UL,0},{UF,0},{UR,0},{UB,0},{DL,0},{DF,0},
	{DR,0},{DB,0},{FL,0},{FR,0},{BR,0},{BL,0}}} //S_LR2
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void initSymCubes()
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
int a,b,c,d,idx=0;
CubieCube cb,ci=idCube;
for (a=0;a<3;a++)					//S_URF3
{
	for (b=0;b<2;b++)				//S_F2
	{
		for (c=0;c<4;c++)			//S_U4
		{
			for (d=0;d<2;d++)		//S_LR2
			{
				symCube[idx++] = ci;
				multiply(&ci,&basicSymCube[S_LR2],&cb);
				ci = cb;
			}
			multiply(&ci,&basicSymCube[S_U4],&cb);
			ci = cb;
		}
		multiply(&ci,&basicSymCube[S_F2],&cb);
		ci = cb;
	}	
	multiply(&ci,&basicSymCube[S_URF3],&cb);
	ci = cb;
}
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void initInvSymIdx()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
CubieCube cc;
int j,k;
for (j=0;j<NSYM_Oh;j++)
for (k=0;k<NSYM_Oh;k++)
{	
	cornerMultiply(&symCube[j],&symCube[k],&cc);
	if (cc.co[URF].c == URF && cc.co[UFL].c == UFL && cc.co[ULB].c == ULB)
	{
		invSymIdx[j] = k; break;
	}
}
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void initSymIdxMultiply()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
int i,j,k;
CubieCube cc;

for (i=0;i<NSYM_Oh;i++)
for (j=0;j<NSYM_Oh;j++)
{
	edgeMultiply(&symCube[i],&symCube[j],&cc);
	for (k=0;k<NSYM_Oh;k++)//find the product cube
	{
		if (symCube[k].eo[UR].e==cc.eo[UR].e && 
		    symCube[k].eo[UF].e==cc.eo[UF].e) {symIdxMultiply[i][j] = k; break;}
	}
}
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void initMoveConjugate()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
CubieCube cc;
int i,j,k,l,match;

for (i=0;i<NMOVE;i++)
for (j=0;j<NSYM_Oh;j++)
{
	cc = edgeConjugate(moveCube[i],j);
	for (k=0;k<NMOVE;k++)//find the conjugate cube cube
	{
		match=1;
		for (l=UR;l<=BR;l++)//test all edges
			if (moveCube[k].eo[l].e!=cc.eo[l].e) {match=0; break;} 
		if (match==1)
		{moveConjugate[i][j] = k; break;}
	}	
}	
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void initTwistConjugate()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
CubieCube cc,ccConj;
int i,j;

for (i=0;i<NTWIST;i++)
{
	cc = invTwist(i);
	for (j=0;j<NSYM_D4h;j++)
	{
		ccConj = cornerConjugate(cc,j);
		twistConjugate[i][j] = twist(ccConj);
	}	
}	
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CubieCube edgeConjugate(CubieCube cc, int symIdx)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
CubieCube ccTmp;
edgeMultiply(&symCube[symIdx],&cc,&ccTmp);
edgeMultiply(&ccTmp,&symCube[invSymIdx[symIdx]],&cc);	
return cc;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CubieCube cornerConjugate(CubieCube cc, int symIdx)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
CubieCube ccTmp;
cornerMultiply(&symCube[symIdx],&cc,&ccTmp);
cornerMultiply(&ccTmp,&symCube[invSymIdx[symIdx]],&cc);	
return cc;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void initRawFLipSliceRep()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
int i,j,k,n,min,idx=0;
CubieCube ccK,ccJ,ccI;
char flag[NSLICE*NFLIP];
for (j=0;j<NSLICE*NFLIP;j++) flag[j]=0;
for (j=0;j<NSLICE;j++)
{
	ccJ = invSlice(j);
	for (k=0;k<NFLIP;k++)
	{
		min = NFLIP*j + k;
		if (flag[min]==1) continue;//no representant
		ccK = invFlip(k);
		for (n=0;n<12;n++) ccK.eo[n].e = ccJ.eo[n].e; //merge ccK and ccJ
		for (i=0;i<NSYM_D4h;i++)
		{
			ccI = edgeConjugate(ccK,i);
			n = NFLIP*slice(ccI) + flip(ccI);
			flag[n]=1;//no representant
		}
		rawFlipSliceRep[idx++] = min;
	}
}
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
unsigned long long getSymmetries(CubieCube cc)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
int hasThisSym,symIdx;
unsigned long long symRet=0;
CubieCube ccTmp1,ccTmp;
Corner i; Edge j;	
for (symIdx=0;symIdx<48;symIdx++)
{
	hasThisSym=1;
	cornerMultiply(&symCube[symIdx],&cc,&ccTmp1);
	cornerMultiply(&ccTmp1,&symCube[invSymIdx[symIdx]],&ccTmp);	
	edgeMultiply(&symCube[symIdx],&cc,&ccTmp1);
	edgeMultiply(&ccTmp1,&symCube[invSymIdx[symIdx]],&ccTmp);
	for (i=URF;i<=DRB;i++)
	{
		if (ccTmp.co[i].c!=cc.co[i].c || ccTmp.co[i].o!=cc.co[i].o)
		{hasThisSym=0; break;}
	}
	if (!hasThisSym) continue;
	for (j=UR;j<=BR;j++)
	{
		if (ccTmp.eo[j].e!=cc.eo[j].e || ccTmp.eo[j].o!=cc.eo[j].o)
		{hasThisSym=0; break;}
	}
	if (hasThisSym) symRet |= 1ULL<<symIdx;
}
return symRet;	
	
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int symCount(unsigned long long sym)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
int i=0;
while (sym!=0) 
{
	if (sym&1) i++;
	sym=sym>>1;
}
return i;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void initGESymmetries()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
Move m;
int i;

for (m=mU1;m<=mB3;m++) {ESymmetries[m]=0;GESymmetries[m]=0;}

for (i=0;i<NSYM_Oh;i++)
for (m=mU1;m<=mB3;m++)
{
	if (moveConjugate[m][i]==m) ESymmetries[m] |= 1ULL<<i;
	if (moveConjugate[m][i]>=m) GESymmetries[m] |= 1ULL<<i;
}


}
