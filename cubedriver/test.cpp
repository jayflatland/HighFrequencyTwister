#include <iostream>
#include <boost/format.hpp>
#include <thread>
#include <chrono>
#include <set>
#include <map>
#include <vector>
#include <atomic>

#include "gtod.h"
#include "search.h"
#include "tools.h"

#include "threaded_solver2.h"
#include "translate_moves.h"
#include "time_for_move_sequence.h"


void test(){
    Kociemba::TwoPhase::initStatics();

    ThreadedSolver threadedSolver;

    threadedSolver.start( 8 );
    char solveSpeed = '5';
    unsigned long targetMS = 887;
    unsigned long goalMS = 887;

    Kociemba::TwoPhase::Search searcher;

    std::default_random_engine rng;

    //
    int runCnt = 0;
    int failCnt = 0;
    int accumSolveTime = 0;
    int accumMovesTime = 0;
    int minMoveTime = 99999999;
    std::map<int,int> movesTimeHistogram;
    std::map<int,int> underGoalHistogram;
    std::map<int,int> solutionIdxHistogram;
    for( int i = 0; i < 1000; i++ ) {
        //std::default_random_engine rng;//repeat same test
        Kociemba::TwoPhase::Tools::randomCube(rng);
        std::string rnd = Kociemba::TwoPhase::Tools::randomCube(rng);
        //rnd = "UUUUUUUUURRRRRRRRRFFFFFFFFFDDDDDDDDDLLLLLLLLLBBBBBBBBB";
        //rnd = "UFUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU";

        //std::cout << std::endl;
        //std::cout << "Loop: " << i << std::endl;
        unsigned long startT = gtod();
        std::string sol;
        try {
            sol = threadedSolver.solve( rnd, solveSpeed, targetMS );
            //sol = searcher.solution( rnd, 24, 1000, false );
            
        } catch( std::exception &ex ) {
            std::cout << "rnd=" << rnd << " solution FAILED!! " << ex.what() << std::endl;
            failCnt++;
            continue;
        }
        
        if(!validateSolution( rnd, sol )){
            std::cout << "rnd=" << rnd << " validateSolution FAILED!!" << std::endl;
            failCnt++;
            continue;
        }               
        
        std::string const moves = translateMoves( sol );
        unsigned long dt = gtod() - startT;
        int movesTime = timeForMoveSequence( moves, solveSpeed );

        runCnt++;
        accumSolveTime += dt;
        accumMovesTime += movesTime;
        minMoveTime = std::min( minMoveTime, movesTime );

        //int moveCnt = std::count( sol.begin(), sol.end(), ' ') + 1;

        std::cout << "thinkTime=" << boost::format("%7.3fms")%( (double)dt*1.0e-3 );
        std::cout << " movesTime=" << boost::format("%9.3fms")%(1.0e-3*movesTime);
        std::cout << " moves=" << moves;

        int wCamTime = dt + movesTime + 66000;
        movesTimeHistogram[ movesTime / 100000 * 100 ]++;
        if( wCamTime < goalMS*1000 ) {
            underGoalHistogram[ 0 ]++;
        } else {
            underGoalHistogram[ 1000 ]++;
        }
        solutionIdxHistogram[ threadedSolver.lastSolutionIndex_ ]++;
        //std::cout << " RND=" << rnd;
        //std::cout << " SOL=" << sol;
        std::cout << std::endl;
    }

    //
    double avgSolveTime = (double)accumSolveTime / (double)runCnt;
    double avgMovesTime = (double)accumMovesTime / (double)runCnt;

    std::cout << "Runs: " << runCnt << std::endl;
    std::cout << "Fails: " << failCnt << std::endl;
    std::cout << "avgSolveTime: " << boost::format("%9.3fms")%( avgSolveTime * 1.0e-3 ) << std::endl;
    std::cout << "avgMovesTime: " << boost::format("%9.3fms")%( avgMovesTime * 1.0e-3 ) << std::endl;
    std::cout << "avgTotalTime: " << boost::format("%9.3fms")%( (avgSolveTime+avgMovesTime) * 1.0e-3 ) << std::endl;
    std::cout << "WithCamTime:  " << boost::format("%9.3fms")%( (avgSolveTime+avgMovesTime+60000) * 1.0e-3 ) << std::endl;

    int sum = 0;
    for( auto & r : movesTimeHistogram ) {
        sum += r.second;
    }
    std::cout << "movesTimeHistogram:" << std::endl;
    for( auto & r : movesTimeHistogram ) {
        std::cout << r.first << "," << r.second << "," << boost::format("%.0f%%")%(100.0 * r.second / sum ) << std::endl;
    }

    sum = 0;
    for( auto & r : underGoalHistogram ) {
        sum += r.second;
    }
    std::cout << "underGoalHistogram: " << goalMS << "ms" << std::endl;
    for( auto & r : underGoalHistogram ) {
        std::cout << r.first << "," << r.second << "," << boost::format("%.0f%%")%(100.0 * r.second / sum ) << std::endl;
    }

    sum = 0;
    for( auto & r : solutionIdxHistogram ) {
        sum += r.second;
    }
    std::cout << "solutionIdxHistogram:" << std::endl;
    for( auto & r : solutionIdxHistogram ) {
        std::cout << r.first << "," << r.second << "," << boost::format("%.0f%%")%(100.0 * r.second / sum ) << std::endl;
    }

}


int main() {
    test();
    return 0;
}