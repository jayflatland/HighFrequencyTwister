//FILE CUBIECUBE.C

#include "cubedefs.h"
#include <string.h>
#include <ctype.h>

/*++++++++++++++++The layout of the facelets on the cube+++++++++++++++++++++++

             |************|
             |*U1**U2**U3*|
             |************|
             |*U4**U5**U6*|
             |************|
             |*U7**U8**U9*|
             |************|
|************|************|************|************|
|*L1**L2**L3*|*F1**F2**F3*|*R1**R2**F3*|*B1**B2**B3*|
|************|************|************|************|
|*L4**L5**L6*|*F4**F5**F6*|*R4**R5**R6*|*B4**B5**B6*|
|************|************|************|************|
|*L7**L8**L9*|*F7**F8**F9*|*R7**R8**R9*|*B7**B8**B9*|
|************|************|************|************|
             |************|
             |*D1**D2**D3*|
             |************|
             |*D4**D5**D6*|
             |************|
             |*D7**D8**D9*|
             |************|
*/

//+++++++Mapping corners {URF,UFL,ULB,UBR,DFR,DLF,DBL,DRB} to facelets+++++++++
const Facelet cornerFacelet[8][3] =
 {	{U9,R1,F3},{U7,F1,L3},{U1,L1,B3},{U3,B1,R3},
	{D3,F9,R7},{D1,L9,F7},{D7,B9,L7},{D9,R9,B7}};
	
//+++++++Mapping edges {UR,UF,UL,UB,DR,DF,DL,DB,FR,FL,BL,BR} to facelets+++++++
const Facelet edgeFacelet[12][2] =
{	{U6,R2},{U8,F2},{U4,L2},{U2,B2},{D6,R8},{D2,F8},{D4,L8},
	{D8,B8},{F6,R4},{F4,L6},{B6,L4},{B4,R6}};


//++++++++++++Mapping this definition string to facelets+++++++++++++++++++++++
//The definiton string of the Identity cube in Singmaster notation is
//++++ UF UR UB UL DF DR DB DL FR FL BR BL UFR URB UBL ULF DRF DFL DLB DBR ++++
const Facelet SingmasterToFacelet[48] =
{	U8,F2,U6,R2,U2,B2,U4,L2,D2,F8,D6,R8,D8,B8,D4,L8,F6,R4,F4,L6,B4,R6,B6,L4,
	U9,F3,R1,U3,R3,B1,U1,B3,L1,U7,L3,F1,D3,R7,F9,D1,F7,L9,D7,L7,B9,D9,B7,R9};	

//++++++++++++Mapping the facelets {U1.....B9} to the faceletColor+++++++++++++
const Color faceletToColor[54] =
{	U,U,U,U,U,U,U,U,U,R,R,R,R,R,R,R,R,R,F,F,F,F,F,F,F,F,F,
	D,D,D,D,D,D,D,D,D,L,L,L,L,L,L,L,L,L,B,B,B,B,B,B,B,B,B};	
  
const char ColorToChar[] = {"URFDLB"};  
  	

//++++++++cubie level faceturns in the "is replaced by" representation+++++++++
const CubieCube basicCubeMove[6] =
{ 
	{{{UBR,0},{URF,0},{UFL,0},{ULB,0},{DFR,0},{DLF,0},{DBL,0},{DRB,0}},
	{{UB,0},{UR,0},{UF,0},{UL,0},{DR,0},{DF,0},
	{DL,0},{DB,0},{FR,0},{FL,0},{BL,0},{BR,0}}},//U
	{{{DFR,2},{UFL,0},{ULB,0},{URF,1},{DRB,1},{DLF,0},{DBL,0},{UBR,2}},
	{{FR,0},{UF,0},{UL,0},{UB,0},{BR,0},{DF,0},
	{DL,0},{DB,0},{DR,0},{FL,0},{BL,0},{UR,0}}},//R
	{{{UFL,1},{DLF,2},{ULB,0},{UBR,0},{URF,2},{DFR,1},{DBL,0},{DRB,0}},
	{{UR,0},{FL,1},{UL,0},{UB,0},{DR,0},{FR,1},
	{DL,0},{DB,0},{UF,1},{DF,1},{BL,0},{BR,0}}},//F
	{{{URF,0},{UFL,0},{ULB,0},{UBR,0},{DLF,0},{DBL,0},{DRB,0},{DFR,0}},
	{{UR,0},{UF,0},{UL,0},{UB,0},{DF,0},{DL,0},
	{DB,0},{DR,0},{FR,0},{FL,0},{BL,0},{BR,0}}},//D
	{{{URF,0},{ULB,1},{DBL,2},{UBR,0},{DFR,0},{UFL,2},{DLF,1},{DRB,0}},
	{{UR,0},{UF,0},{BL,0},{UB,0},{DR,0},{DF,0},
	{FL,0},{DB,0},{FR,0},{UL,0},{DL,0},{BR,0}}},//L
	{{{URF,0},{UFL,0},{UBR,1},{DRB,2},{DFR,0},{DLF,0},{ULB,2},{DBL,1}},
	{{UR,0},{UF,0},{UL,0},{BR,1},{DR,0},{DF,0},
	{DL,0},{BL,1},{FR,0},{FL,0},{UB,1},{DB,1}}} //B
};


//+++++++++++++++++++The Identitiy Cube++++++++++++++++++++++++++++++++++++++++
const CubieCube idCube =
{
	{{URF,0},{UFL,0},{ULB,0},{UBR,0},{DFR,0},{DLF,0},{DBL,0},{DRB,0}},
	{{UR,0},{UF,0},{UL,0},{UB,0},{DR,0},{DF,0},
	{DL,0},{DB,0},{FR,0},{FL,0},{BL,0},{BR,0}}
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CubieCube cubeAxMove(CubieCube cc, Axis ax)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
CubieCube ccRet;
cornerMultiply(&cc,&basicCubeMove[ax],&ccRet);
edgeMultiply(&cc,&basicCubeMove[ax],&ccRet);
return ccRet;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void initMoveCubes()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
Axis i;
CubieCube cc;
for (i=U;i<=B;i++)
{
	cc = moveCube[2*i] = basicCubeMove[i];//ax
	cc = cubeAxMove(cc,i);//ax^2
	moveCube[2*i+1] = cubeAxMove(cc,i);//ax^3
}
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
FaceletCube cubieCubeToFaceletCube(CubieCube cc)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
Corner i,j; Edge k,m; int n;
unsigned char ori;
FaceletCube fcRet;
for (i=URF;i<=DRB;i++)
{
	j = cc.co[i].c;//cornercubie j is in position i
	ori = cc.co[i].o;//Orientation of this cubie
	for (n=0;n<3;n++)
	fcRet.f[cornerFacelet[i][(n+ori)%3 ]] = faceletToColor[cornerFacelet[j][n]];
}
for (k=UR;k<=BR;k++)
{
	m = cc.eo[k].e;//edgecubie m is in position k
	ori = cc.eo[k].o;//Orientation of this cubie
	for (n=0;n<2;n++)
	fcRet.f[edgeFacelet[k][(n+ori)%2 ]] = faceletToColor[edgeFacelet[m][n]];
}
//set center colors
fcRet.f[U5]=U;fcRet.f[R5]=R;fcRet.f[F5]=F;
fcRet.f[D5]=D;fcRet.f[L5]=L;fcRet.f[B5]=B;
return fcRet;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CubieCube faceletCubeToCubieCube(FaceletCube fc)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
Corner i,j; Edge k,m; int ori;
CubieCube ccRet; Color col1,col2;
for (i=URF;i<=DRB;i++)
{
	//get the colors of the cubie at corner i, starting with the U/D color
	for (ori=0;ori<3;ori++)
	if (fc.f[cornerFacelet[i][ori]]==U || fc.f[cornerFacelet[i][ori]]==D)
	break;
	col1 = fc.f[cornerFacelet[i][(ori+1)%3]];
	col2 = fc.f[cornerFacelet[i][(ori+2)%3]];
	
	for (j=URF;j<=DRB;j++)
	{
		if (col1==faceletToColor[cornerFacelet[j][1]]
		&& col2==faceletToColor[cornerFacelet[j][2]])
		{
			// in cornerposition i we have cornercubie j
			ccRet.co[i].c = j;
			ccRet.co[i].o = ori;
			break;
		}
	}	
}
for (k=UR;k<=BR;k++)
for (m=UR;m<=BR;m++)
{
	if (fc.f[edgeFacelet[k][0]]==faceletToColor[edgeFacelet[m][0]] &&
		fc.f[edgeFacelet[k][1]]==faceletToColor[edgeFacelet[m][1]])
	{
		ccRet.eo[k].e = m;
		ccRet.eo[k].o = 0;
		break;	
	}
	if (fc.f[edgeFacelet[k][0]]==faceletToColor[edgeFacelet[m][1]] &&
		fc.f[edgeFacelet[k][1]]==faceletToColor[edgeFacelet[m][0]])
	{
		ccRet.eo[k].e = m;
		ccRet.eo[k].o = 1;
		break;	
	}
}
return ccRet;
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void cubieCubeToString(CubieCube cc, char* defString)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
FaceletCube fc = cubieCubeToFaceletCube(cc);	
int i,j=0;
for (i=0;i<48;i++)
{
	defString[i+j] = ColorToChar[fc.f[SingmasterToFacelet[i]]];
	//add spaces if necessary
	if ((i<24 && i&1) || (i>24 && (i+1)%3==0)) defString[i + ++j] = ' ';	
}
defString[i+j-1]= 0;	
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CubieCube stringToCubieCube(char* defString)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
	char *sp;
	int i, j, n,pow, fCount[7] = {0,0,0,0,0,0,0};
	CubieCube ccRet = idCube;
	FaceletCube fc;
	Axis ax;
	
	sp= strstr(defString,"(");
	if (sp!=NULL) *sp=0;//remove maneuver length if present	
	sp= strstr(defString,"//");
	if (sp!=NULL) *sp=0;//remove comment if present
	n = strlen(defString);
	for (i=0;i<n;i++)
	{
		switch (toupper(defString[i]))//check if definition string has Singmaster notation
		{
			case 'U': fCount[0]++; break;
			case 'R': fCount[1]++; break;
			case 'F': fCount[2]++; break;
			case 'D': fCount[3]++; break;
			case 'L': fCount[4]++; break;
			case 'B': fCount[5]++; break;
			case '\'':case '2': fCount[6]++; break;
		}
	}
	if (fCount[0]==8 && fCount[1]==8 && fCount[2]==8 && fCount[3]==8 && 
		fCount[4]==8 && fCount[5]==8 && fCount[6]==0)
	{//Singmaster notation
		j = 0;
		for (i=0;i<n;i++)	
		switch (toupper(defString[i]))
		{
			case 'U': fc.f[SingmasterToFacelet[j++]] = U; break;
			case 'R': fc.f[SingmasterToFacelet[j++]] = R; break;
			case 'F': fc.f[SingmasterToFacelet[j++]] = F; break;
			case 'D': fc.f[SingmasterToFacelet[j++]] = D; break;
			case 'L': fc.f[SingmasterToFacelet[j++]] = L; break;
			case 'B': fc.f[SingmasterToFacelet[j++]] = B; break;
		}
		ccRet = faceletCubeToCubieCube(fc);
	}
	else
	{//maneuver notation
		ax = U;
		for (i=0;i<n;i++)
		{
			switch (toupper(defString[i]))
			{
				case 'U': ax = U; break;
				case 'R': ax = R; break;
				case 'F': ax = F; break;
				case 'D': ax = D; break;
				case 'L': ax = L; break;
				case 'B': ax = B; break;
				default: continue;	
			}
			if (i==n) {ccRet = cubeAxMove(ccRet,ax); break;}
			switch (defString[i+1])
			{
				case '3':case'\'':case'-': pow = 3; break;
				case '2': pow = 2; break;
				default: pow = 1;
			}	
			for (j=0;j<pow;j++)
			ccRet = cubeAxMove(ccRet,ax);		
		}		
	}
	return ccRet;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void cornerMultiply(const CubieCube *a,const CubieCube *b, CubieCube *ab)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//The result of the multiplication is the composition of the the permutations
//a and b. Because we also describe reflections of the whole cube by
//permutations, we get a complication with the corners. The orientation of
//mirrored corners is described by the numbers 3, 4 and 5. The composition of
//the orientations cannot be computed by addition modulo three in the cyclic
//group C3 any more. Instead the rules below give an addition in the dihedral
//group D3 with 6 elements.
{
char ori,oriA,oriB;
Corner crn;
ori=0;
for (crn=URF;crn<=DRB;crn++)
{
	ab->co[crn].c =  a->co[b->co[crn].c].c;
	oriA = a->co[b->co[crn].c].o;
	oriB = b->co[crn].o;
	if (oriA<3 && oriB<3)       //if both cubes are regular cubes... 
	{
		ori = oriA + oriB;     //just do an addition modulo 3 here  
		if (ori>=3) ori-=3;    //the composition is a regular cube 
	}
	else if (oriA<3 && oriB>=3) //if cube b is in a mirrored state...
	{
		ori = oriA + oriB;
		if (ori>=6) ori-=3;	    //the composition is a mirrored cube
	}
	else if (oriA>=3 && oriB<3) //if cube a is an a mirrored state...
	{
		ori = oriA - oriB;
		if (ori<3) ori+=3;		//the composition is a mirrored cube	
	}
	else if (oriA>=3 && oriB>=3) //if both cubes are in mirrored states...
	{
		ori = oriA - oriB;
		if (ori<0) ori+=3;		//the composition is a regular cube
	}
	ab->co[crn].o = ori;
};
}	
 
 //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void edgeMultiply(const CubieCube *a,const CubieCube *b, CubieCube *ab)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//With edges we have no complications with mirror images, because the mirror
//image of an edgecubie looks the same as the edgecubie itself. So we only have
//the orientations 0 and 1 and do an addition modulo 2 in all cases.
{
Edge edg;
for (edg=UR;edg<=BR;edg++)
{
	ab->eo[edg].e =  a->eo[b->eo[edg].e].e;
	ab->eo[edg].o = (b->eo[edg].o + a->eo[b->eo[edg].e].o)%2;
};
}

 //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void multiply(const CubieCube *a,const CubieCube *b, CubieCube *ab)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
cornerMultiply(a,b,ab);
edgeMultiply(a,b,ab);	
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CubieCube invCubieCube(CubieCube cc)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
CubieCube ccInv;
Edge edg; Corner crn;
int ori;

for (edg=UR;edg<=BR;edg++) ccInv.eo[cc.eo[edg].e].e = edg;
for (edg=UR;edg<=BR;edg++) ccInv.eo[edg].o = cc.eo[ccInv.eo[edg].e].o;

for (crn=URF;crn<=DRB;crn++) ccInv.co[cc.co[crn].c].c = crn;
for (crn=URF;crn<=DRB;crn++)
{
	ori = cc.co[ccInv.co[crn].c].o;
	if (ori >=3) //just for completeness, no need to invert mirrored cubes here
		ccInv.co[crn].o = ori;
	else //the usual case
	{
		ccInv.co[crn].o = -ori;
		if (ccInv.co[crn].o < 0) ccInv.co[crn].o += 3;
	}
}	
 return ccInv;
}
