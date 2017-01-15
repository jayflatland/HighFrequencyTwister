QTM-Optimal Solver


-------------------------------------------------------------------------------
1. The purpose of the QTM-Optimal Solver
-------------------------------------------------------------------------------

The solver finds the shortes solution(s) for a scrambled Rubik's in the quarter turn metric, where a 90 degree faceturn counts as one move and a 180 degree move counts as two moves. The program uses about 600 MB of RAM, mainly for a big pruning table. This pruning table holds information about the moves which transform a given cube closer to a certain subgroup H of the cube (see sourcecode for details).


-------------------------------------------------------------------------------
2. Compiling the solver
-------------------------------------------------------------------------------

It should be no problem to compile the program under LINUX with the supplied Makefile. Just change to the directory which contains the sourcecode on the command-line and invoke "make".

Under Windows, the program also will compile with no problem with the supplied Makefile if you use MinGw (http://www.mingw.org/) and if you comment out the line 

#define _LINUX_

in the file optiqtm.c.

The only difference in the program behaviour between LINUX and Windows is that you may skip a current computation under LINUX with ctrl-c and proceed with the next cube. Under Windows, ctrl-c will terminate the program.

-------------------------------------------------------------------------------
3. Running the solver
-------------------------------------------------------------------------------

Change to the directory which contains the program-file and start the program with "optiqtm" from the command-line. If you start the program with "optiqtm +" or "optiqtm +0", not one but all optimal solutions are computed (only up to symmetry, if the cube has some symmetries). "optiqtm +1" also computes suboptimal solutions, "optiqtm +2" also subsuboptimal solutions etc...
If you add the flag -s, no symmetry reduction is performed (example: "optiqtm +1 -s" computes all optimal and suboptimal solutions without symmetry reduction.

On the first run the big pruning table will be created, which will take some time to complete. This 538 MB table will be saved to disk and the program will start much faster on the next run.

You may enter the cubes in Singmaster notation[1] like

UF UR RD RB LU LF DB DL FR UB DF BL UFR RFD RDB RBU LFU LUB DLB LDF 

or in maneuver notation like

U2 F2 R2 U' L2 D B R' B R' B R' D' L2 U'


If you have defined a file which contains the cubes, for example "maneuvers", you also may use input redirection.
The input format is compatible to the Cube Explorer [2] file format.

Examples for running under LINUX:

optiqtm
find one optimal solution, enter maneuvers during program run.

optiqtm +
find all optimal solutions, enter maneuvers during program run.

optiqtm + <maneuvers
find all optimal solutions for the cubes in the file "maneuvers".

optiqtm <maneuvers |tee results
find one optimal solution for the cubes in the file "maneuvers", redirect output to the file "results".

optiqtm +1 -s <maneuvers |tee results
find all optimal and suboptimal solutions for the cubes in the file "maneuvers", do not use symmetry rduction and redirect output to the file "results".


-------------------------------------------------------------------------------
4. Performance
-------------------------------------------------------------------------------

The time to solve a cube optimally greatly depends on the given situation. But usually it takes between a few minutes and an hour to find an optimal solution. Of course your PC should have enough RAM, 1 GB should be enough.


-------------------------------------------------------------------------------
5. Notes
-------------------------------------------------------------------------------

[1] Singmaster Notation:
A solved cube is represented as

UF UR UB UL DF DR DB DL FR FL BR BL UFR URB UBL ULF DRF DFL DLB DBR

To input a scrambled cube, first give the cubie that is in the UF location,
then the cubie in the UR location, and so forth, according to the list
above. For example, "cube in a cube" would be

UF UR RD RB LU LF DB DL FR UB DF BL UFR RFD RDB RBU LFU LUB DLB LDF


[2] Cube Explorer http://kociemba.org/cube.htm

