prog: coordcube.o cubiecube.o optiqtm.o pruning.o symmetry.o
	gcc coordcube.o cubiecube.o optiqtm.o pruning.o symmetry.o -o optiqtm

coordcube.o : coordcube.c
	gcc -O3 -Wall -c coordcube.c

cubiecube.o : cubiecube.c
	gcc -O3 -Wall -c cubiecube.c

optiqtm.o : optiqtm.c
	gcc -O3 -Wall -c optiqtm.c

pruning.o : pruning.c
	gcc -O3 -Wall -c pruning.c

symmetry.o : symmetry.c
	gcc -O3 -Wall -c symmetry.c

