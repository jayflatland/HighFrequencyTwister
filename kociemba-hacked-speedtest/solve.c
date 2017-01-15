#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "search.h"

double u2s(double u){ return u/1000000.0; }

int times[100] = { 0 };
int moves[100] = { 0 };

int countMoves( char const *sol ){
   int inMove = 0; 
   int moves=0;
   for(;*sol;++sol){
     if(*sol == ' '){
       inMove = 0;
     }
     else{
       if(!inMove){
         inMove = 1;
         ++moves;
       }
     }
   }
   return moves;
}

char line[128];
extern unsigned long lastTime;
int main(int argc, char **argv)
{
    unsigned long minTime=999999999;
    unsigned long maxTime=0;
    double totalTime=0;
    int solves=0;
    
    int lastMoves=0;
    int minMoves=999999;
    int maxMoves=0;
    double totalMoves=0;    
    int idx;
    while(fgets(line,sizeof(line),stdin)){
	line[sizeof(line)-1] = 0;
        line[strlen(line)-1] = 0;
        lastTime = 0;
        char *sol = solution(
            line,
            24,
            1000,
            0
        );
        if (sol == NULL) {
            puts("Unsolvable cube!");
            return 2;
        }
        lastMoves = countMoves(sol);
        puts(sol);
        free(sol);

	++solves;

        totalMoves += lastMoves;
        if(lastMoves > maxMoves) maxMoves = lastMoves;
        if(lastMoves < minMoves) minMoves = lastMoves;
        totalTime += lastTime;
        if(lastTime > maxTime) maxTime = lastTime;
        if(lastTime < minTime) minTime = lastTime;

        idx = lastTime / 50000;
        if( idx >= 100) idx=99;
        ++times[idx];
        idx = lastMoves;
        if( idx >= 100) idx=99;
        ++moves[idx];

	printf("solve %5d: time(min=%.6f, avg=%.3f, max=%.3f), moves(min=%d, avg=%.1f, max=%d)\n",
		solves, 
                u2s(minTime), u2s(totalTime/solves), u2s(maxTime),
                minMoves, totalMoves/solves, maxMoves
        );
    }
    printf("times:\n");
    for(int i=0; i<100; ++i){
      if(times[i]){
        printf("%.3f..%.3f = %5d\n", u2s(i*50000),u2s((i+1)*50000), times[i]);
      }
    }
    printf("moves:\n");
    for(int i=0; i<100; ++i){
      if(moves[i]){
        printf("%2d = %5d\n", i, moves[i]);
      }
    }

    return 0;
}
