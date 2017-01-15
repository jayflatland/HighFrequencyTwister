//FILE OPTIQTM.C

#define _LINUX_
//Comment this line out if you compile with MinGW under Windows.
//This disables the SIGNAL handling.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <assert.h>
#include  <signal.h>
#include <ctype.h>
#include "cubedefs.h"

static char manString[256];
int subOptLev;
int symRed;

#ifdef _LINUX_
static sigjmp_buf jump_buf;
#endif

CubieCube cc_a;


#ifdef _LINUX_ 
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void  user_break(int  n)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
printf("-- skipping cube --\n");
fflush(stdout);
siglongjmp(jump_buf, 1);
return;
}
#endif

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void  pp()
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
printf(".");
fflush(stdout);
return;
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int main(int argc, char * argv[])
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
int i,l;
subOptLev=-1;
symRed=1;
for (i=1;i<argc;i++)
{
	if (argv[i][0]=='+')//all optimal solutions/suboptimal solutions
	{
		subOptLev=0;
		if  (argv[i][1]>'0' && argv[i][1]<='9') subOptLev= argv[i][1]-48;
	}
	if (argv[i][0]=='-')
	{
		if (argv[i][1]=='s') symRed=0;
	}
}

printf("initializing memory.\n");
visitedA = (char *)calloc(NGOAL/8+1,1);//initialized to 0 by default
visitedB = (char *)calloc(NGOAL/8+1,1);
for (l=0;l<NTWIST;l++)
movesCloserToTarget[l] = (short *)calloc(NFLIPSLICE*2,2);

printf("initializing tables");fflush(stdout);
initSymCubes();
initMoveCubes();
initInvSymIdx();
initSymIdxMultiply();
initMoveConjugate();
initMoveBitsConjugate();
initGESymmetries();
initTwistConjugate();pp();
initRawFLipSliceRep();pp();
initTwistMove();pp();
initCorn6PosMove();pp();
initEdge4PosMove();pp();
initEdge6PosMove();pp();
initSymFlipSliceClassMove();pp();
initMovesCloserToTarget();pp();
initNextMove();pp();
printf("\r\n");


while (1)
{
	FaceletCube fc;
	printf("enter cube (x to exit): ");fflush(stdout);
	if (fgets(manString,sizeof(manString),stdin)==NULL) break;
	if (manString[0]=='x') exit(EXIT_SUCCESS);
	l=strlen(manString);
	if (manString[l-1]=='\n') manString[l-1]=0;//remove LF
	if (l>1 && manString[l-2]=='\r') manString[l-2]=0;//remove CR if present
	if (strlen(manString)==0) continue;//ignore empty lines
	printf("\nsolving optimal: %s\n",manString);fflush(stdout);
	//cc_a = stringToCubieCube(manString);
	//take kociemba format
	for (i=0;i<54;i++)	
	switch (toupper(manString[i]))
	{
		case 'U': fc.f[i] = U; break;
		case 'R': fc.f[i] = R; break;
		case 'F': fc.f[i] = F; break;
		case 'D': fc.f[i] = D; break;
		case 'L': fc.f[i] = L; break;
		case 'B': fc.f[i] = B; break;
	}
	//ccRet = faceletCubeToCubieCube(fc);
	cc_a = faceletCubeToCubieCube(fc);

	solveOptimal(cc_a);
}
exit(EXIT_SUCCESS);
return 0;
}
