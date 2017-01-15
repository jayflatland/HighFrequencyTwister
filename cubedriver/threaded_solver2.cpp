#include <iostream>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <set>
#include <map>
#include <boost/range.hpp>

#include "cpp_kociemba/search.h"

#include "threaded_solver2.h"
#include "translate_moves.h"
#include "time_for_move_sequence.h"
#include "gtod.h"

namespace{
    enum FF{
        U1, U2, U3, U4, U5, U6, U7, U8, U9, 
        R1, R2, R3, R4, R5, R6, R7, R8, R9, 
        F1, F2, F3, F4, F5, F6, F7, F8, F9, 
        D1, D2, D3, D4, D5, D6, D7, D8, D9, 
        L1, L2, L3, L4, L5, L6, L7, L8, L9, 
        B1, B2, B3, B4, B5, B6, B7, B8, B9    
    };
    enum {
        U_ = 0,
        R_ = 1,
        F_ = 2,
        D_ = 3,
        L_ = 4,
        B_ = 5,
    };

    int const U1_[54] = {
        U7, U4, U1, U8, U5, U2, U9, U6, U3, 
        B1, B2, B3, R4, R5, R6, R7, R8, R9, 
        R1, R2, R3, F4, F5, F6, F7, F8, F9, 
        D1, D2, D3, D4, D5, D6, D7, D8, D9, 
        F1, F2, F3, L4, L5, L6, L7, L8, L9, 
        L1, L2, L3, B4, B5, B6, B7, B8, B9
    };
    int const U2_[54] = {
        U9, U8, U7, U6, U5, U4, U3, U2, U1, 
        L1, L2, L3, R4, R5, R6, R7, R8, R9, 
        B1, B2, B3, F4, F5, F6, F7, F8, F9, 
        D1, D2, D3, D4, D5, D6, D7, D8, D9, 
        R1, R2, R3, L4, L5, L6, L7, L8, L9, 
        F1, F2, F3, B4, B5, B6, B7, B8, B9
    };
    int const U3_[54] = {
        U3, U6, U9, U2, U5, U8, U1, U4, U7, 
        F1, F2, F3, R4, R5, R6, R7, R8, R9, 
        L1, L2, L3, F4, F5, F6, F7, F8, F9, 
        D1, D2, D3, D4, D5, D6, D7, D8, D9, 
        B1, B2, B3, L4, L5, L6, L7, L8, L9, 
        R1, R2, R3, B4, B5, B6, B7, B8, B9
    };
    int const R1_[54] = {
        U1, U2, F3, U4, U5, F6, U7, U8, F9, 
        R7, R4, R1, R8, R5, R2, R9, R6, R3, 
        F1, F2, D3, F4, F5, D6, F7, F8, D9, 
        D1, D2, B7, D4, D5, B4, D7, D8, B1, 
        L1, L2, L3, L4, L5, L6, L7, L8, L9, 
        U9, B2, B3, U6, B5, B6, U3, B8, B9
    };
    int const R2_[54] = {
        U1, U2, D3, U4, U5, D6, U7, U8, D9, 
        R9, R8, R7, R6, R5, R4, R3, R2, R1, 
        F1, F2, B7, F4, F5, B4, F7, F8, B1, 
        D1, D2, U3, D4, D5, U6, D7, D8, U9, 
        L1, L2, L3, L4, L5, L6, L7, L8, L9, 
        F9, B2, B3, F6, B5, B6, F3, B8, B9
    };
    int const R3_[54] = {
        U1, U2, B7, U4, U5, B4, U7, U8, B1, 
        R3, R6, R9, R2, R5, R8, R1, R4, R7, 
        F1, F2, U3, F4, F5, U6, F7, F8, U9, 
        D1, D2, F3, D4, D5, F6, D7, D8, F9, 
        L1, L2, L3, L4, L5, L6, L7, L8, L9, 
        D9, B2, B3, D6, B5, B6, D3, B8, B9
    };
    int const F1_[54] = {
        U1, U2, U3, U4, U5, U6, L9, L6, L3, 
        U7, R2, R3, U8, R5, R6, U9, R8, R9, 
        F7, F4, F1, F8, F5, F2, F9, F6, F3, 
        R7, R4, R1, D4, D5, D6, D7, D8, D9, 
        L1, L2, D1, L4, L5, D2, L7, L8, D3, 
        B1, B2, B3, B4, B5, B6, B7, B8, B9
    };
    int const F2_[54] = {
        U1, U2, U3, U4, U5, U6, D3, D2, D1, 
        L9, R2, R3, L6, R5, R6, L3, R8, R9, 
        F9, F8, F7, F6, F5, F4, F3, F2, F1, 
        U9, U8, U7, D4, D5, D6, D7, D8, D9, 
        L1, L2, R7, L4, L5, R4, L7, L8, R1, 
        B1, B2, B3, B4, B5, B6, B7, B8, B9
    };
    int const F3_[54] = {
        U1, U2, U3, U4, U5, U6, R1, R4, R7, 
        D3, R2, R3, D2, R5, R6, D1, R8, R9, 
        F3, F6, F9, F2, F5, F8, F1, F4, F7, 
        L3, L6, L9, D4, D5, D6, D7, D8, D9, 
        L1, L2, U9, L4, L5, U8, L7, L8, U7, 
        B1, B2, B3, B4, B5, B6, B7, B8, B9
    };
    int const D1_[54] = {
        U1, U2, U3, U4, U5, U6, U7, U8, U9, 
        R1, R2, R3, R4, R5, R6, F7, F8, F9, 
        F1, F2, F3, F4, F5, F6, L7, L8, L9, 
        D7, D4, D1, D8, D5, D2, D9, D6, D3, 
        L1, L2, L3, L4, L5, L6, B7, B8, B9, 
        B1, B2, B3, B4, B5, B6, R7, R8, R9
    };
    int const D2_[54] = {
        U1, U2, U3, U4, U5, U6, U7, U8, U9, 
        R1, R2, R3, R4, R5, R6, L7, L8, L9, 
        F1, F2, F3, F4, F5, F6, B7, B8, B9, 
        D9, D8, D7, D6, D5, D4, D3, D2, D1, 
        L1, L2, L3, L4, L5, L6, R7, R8, R9, 
        B1, B2, B3, B4, B5, B6, F7, F8, F9
    };
    int const D3_[54] = {
        U1, U2, U3, U4, U5, U6, U7, U8, U9, 
        R1, R2, R3, R4, R5, R6, B7, B8, B9, 
        F1, F2, F3, F4, F5, F6, R7, R8, R9, 
        D3, D6, D9, D2, D5, D8, D1, D4, D7, 
        L1, L2, L3, L4, L5, L6, F7, F8, F9, 
        B1, B2, B3, B4, B5, B6, L7, L8, L9
    };
    int const L1_[54] = {
        B9, U2, U3, B6, U5, U6, B3, U8, U9, 
        R1, R2, R3, R4, R5, R6, R7, R8, R9, 
        U1, F2, F3, U4, F5, F6, U7, F8, F9, 
        F1, D2, D3, F4, D5, D6, F7, D8, D9, 
        L7, L4, L1, L8, L5, L2, L9, L6, L3, 
        B1, B2, D7, B4, B5, D4, B7, B8, D1
    };
    int const L2_[54] = {
        D1, U2, U3, D4, U5, U6, D7, U8, U9, 
        R1, R2, R3, R4, R5, R6, R7, R8, R9, 
        B9, F2, F3, B6, F5, F6, B3, F8, F9, 
        U1, D2, D3, U4, D5, D6, U7, D8, D9, 
        L9, L8, L7, L6, L5, L4, L3, L2, L1, 
        B1, B2, F7, B4, B5, F4, B7, B8, F1
    };
    int const L3_[54] = {
        F1, U2, U3, F4, U5, U6, F7, U8, U9, 
        R1, R2, R3, R4, R5, R6, R7, R8, R9, 
        D1, F2, F3, D4, F5, F6, D7, F8, F9, 
        B9, D2, D3, B6, D5, D6, B3, D8, D9, 
        L3, L6, L9, L2, L5, L8, L1, L4, L7, 
        B1, B2, U7, B4, B5, U4, B7, B8, U1
    };
    int const B1_[54] = {
        R3, R6, R9, U4, U5, U6, U7, U8, U9, 
        R1, R2, D9, R4, R5, D8, R7, R8, D7, 
        F1, F2, F3, F4, F5, F6, F7, F8, F9, 
        D1, D2, D3, D4, D5, D6, L1, L4, L7, 
        U3, L2, L3, U2, L5, L6, U1, L8, L9, 
        B7, B4, B1, B8, B5, B2, B9, B6, B3
    };
    int const B2_[54] = {
        D9, D8, D7, U4, U5, U6, U7, U8, U9, 
        R1, R2, L7, R4, R5, L4, R7, R8, L1, 
        F1, F2, F3, F4, F5, F6, F7, F8, F9, 
        D1, D2, D3, D4, D5, D6, U3, U2, U1, 
        R9, L2, L3, R6, L5, L6, R3, L8, L9, 
        B9, B8, B7, B6, B5, B4, B3, B2, B1
    };
    int const B3_[54] = {
        L7, L4, L1, U4, U5, U6, U7, U8, U9, 
        R1, R2, U1, R4, R5, U2, R7, R8, U3, 
        F1, F2, F3, F4, F5, F6, F7, F8, F9, 
        D1, D2, D3, D4, D5, D6, R9, R6, R3, 
        D7, L2, L3, D8, L5, L6, D9, L8, L9, 
        B3, B6, B9, B2, B5, B8, B1, B4, B7
    };

    int const LR_90_[54] = {
        F1, F2, F3, F4, F5, F6, F7, F8, F9, 
        R7, R4, R1, R8, R5, R2, R9, R6, R3, 
        D1, D2, D3, D4, D5, D6, D7, D8, D9, 
        B9, B8, B7, B6, B5, B4, B3, B2, B1,
        L3, L6, L9, L2, L5, L8, L1, L4, L7, 
        U9, U8, U7, U6, U5, U4, U3, U2, U1,
    };
    int const LR_90_transform_fwd[6] = { B_, R_, U_, F_, L_, D_ };
    int const LR_90_transform_rev[6] = { F_, R_, D_, B_, L_, U_ };
    

    int const FB_90_[54] = {
        L7, L4, L1, L8, L5, L2, L9, L6, L3, 
        U7, U4, U1, U8, U5, U2, U9, U6, U3, 
        F7, F4, F1, F8, F5, F2, F9, F6, F3, 
        R7, R4, R1, R8, R5, R2, R9, R6, R3, 
        D7, D4, D1, D8, D5, D2, D9, D6, D3, 
        B3, B6, B9, B2, B5, B8, B1, B4, B7,   

    };
    int const FB_90_transform_fwd[6] = { R_, D_, F_, L_, U_, B_ };
    int const FB_90_transform_rev[6] = { L_, U_, F_, R_, D_, B_ };

    int const URF_[54] = {
        F7, F4, F1, F8, F5, F2, F9, F6, F3,
        U9, U8, U7, U6, U5, U4, U3, U2, U1,
        R7, R4, R1, R8, R5, R2, R9, R6, R3,
        B7, B4, B1, B8, B5, B2, B9, B6, B3,
        D1, D2, D3, D4, D5, D6, D7, D8, D9,
        L3, L6, L9, L2, L5, L8, L1, L4, L7,
    };
    int const URF_transform_fwd[6] = { R_, F_, U_, L_, B_, D_ };
    int const URF_transform_rev[6] = { F_, U_, R_, B_, D_, L_ };

    inline int charToFace(char c){
        switch(c){
        case 'U': return 0;
        case 'R': return 1;
        case 'F': return 2;
        case 'D': return 3;
        case 'L': return 4;
        case 'B': return 5;
        }
        return -1;
    }
    inline char faceToChar(int f){
        return "URFDLB"[f];
    }

    struct ThreadConfig {
        const char *movesPrefix_;
        int const  *stateMap_;
        int const  *transformFwd_;
        int const  *transformRev_;
        ThreadConfig( const char *movesPrefix, int const *stateMap, int const *transformFwd, int const  *transformRev )
            : movesPrefix_( movesPrefix ), stateMap_( stateMap ), transformFwd_( transformFwd ), transformRev_( transformRev )
        {}
    };

    // moves to prefix for threads 1-19
    //  start with all CW moves, then CCW, and only use 180 if necessary
    const ThreadConfig allMoves[] = {
          ThreadConfig( 0,     0,      0,                   0                   ),//[ 0],207,21%
          ThreadConfig( 0,     LR_90_, LR_90_transform_fwd, LR_90_transform_rev ),//[ 1],171,17%
          ThreadConfig( 0,     FB_90_, FB_90_transform_fwd, FB_90_transform_rev ),//[ 2],161,16%
          ThreadConfig( "D ",  D1_,    0,                   0                   ),//[ 3], 63, 6%
          ThreadConfig( "L ",  L1_,    0,                   0                   ),//[ 5], 47, 5%
          ThreadConfig( "R ",  R1_,    0,                   0                   ),//[ 4], 36, 4%
          ThreadConfig( "L' ", L3_,    0,                   0                   ),//[10], 36, 4%
          ThreadConfig( "B' ", B3_,    0,                   0                   ),//[ 7], 43, 4%
          ThreadConfig( "B ",  B1_,    0,                   0                   ),//[ 8], 36, 4%
          ThreadConfig( "F ",  F1_,    0,                   0                   ),//[11], 29, 3%
          ThreadConfig( "D' ", D3_,    0,                   0                   ),//[ 6], 33, 3%
          ThreadConfig( "U' ", U3_,    0,                   0                   ),//[13], 15, 2%
          ThreadConfig( "U ",  U1_,    0,                   0                   ),//[ 9], 20, 2%
          ThreadConfig( "R' ", R3_,    0,                   0                   ),//[14], 16, 2%
          ThreadConfig( "F' ", F3_,    0,                   0                   ),//[12], 22, 2%
          ThreadConfig( "F2 ", F2_,    0,                   0                   ),//[18], 15, 2%
          ThreadConfig( "D2 ", D2_,    0,                   0                   ),//[16], 18, 2%
          ThreadConfig( "R2 ", R2_,    0,                   0                   ),//[17],  6, 1%
          ThreadConfig( "L2 ", L2_,    0,                   0                   ),//[15], 14, 1%
          ThreadConfig( "B2 ", B2_,    0,                   0                   ),//[19], 10, 1%
          ThreadConfig( "U2 ", U2_,    0,                   0                   ),//[20],  2, 0%
    };

    // index with movesTable[face][times]
    //  rotates face CW by time*90 degrees
    //  times==0 has null pointer
    int const * const movesTable[6][4]{
        { nullptr, U1_, U2_, U3_ },
        { nullptr, R1_, R2_, R3_ },
        { nullptr, F1_, F2_, F3_ },
        { nullptr, D1_, D2_, D3_ },
        { nullptr, L1_, L2_, L3_ },
        { nullptr, B1_, B2_, B3_ },
    };


    std::string remapState( std::string const &s, int const *transformFwd ) {
        std::string r = s;
        for(auto &&c : r){
            int const face = charToFace( c );
            if( face >= 0 ){
                c = faceToChar( transformFwd[ face ] );
            }
            //else: this is a non-face character ( should not happen for state )
        }
        return r;
    }
    
    std::string remapSolution( std::string const &sol, int const *transformRev ) {
        std::string r = sol;
        for(auto &&c : r){
            int const face = charToFace( c );
            if( face >= 0 ){
                c = faceToChar( transformRev[ face ] );
            }
            //else: this is a non-face character ( move modifier or delimiter), leave it alone
        }
        return r;
    }

    std::string makeMove( std::string const &s, int const *m ) {
        std::string r(54, ' ');
        for( int i = 0; i < 54; i++ ) {
            r[i] = s[ m[i] ];
        }
        return r;
    }
    std::string makeMove( std::string const &s, std::pair<int,int> m ) {
        return makeMove( s, movesTable[m.first][m.second] );
    }
    std::string makeMoves( std::string s, std::string const &moves ){
        char const *str = moves.c_str();
        while(*str){
            auto const move = parseMove(str);
            s = makeMove(s, move);
        }
        return s;
    }
    std::string const solved="UUUUUUUUURRRRRRRRRFFFFFFFFFDDDDDDDDDLLLLLLLLLBBBBBBBBB";
}

bool validateSolution( std::string const &state, std::string const &solution){
    return makeMoves( state, solution ) == solved;
}


struct ThreadedSolver::PerThread{
    std::thread t;
    
    std::mutex m;
    std::condition_variable cv;
    
    // input
    bool inputReady = false;
    bool die = false;
    std::string cubeState;
    
    // process
    Kociemba::TwoPhase::Search search;
    
    // output
    bool outputReady = false;
    std::string solution;
    std::string error;
    long thinkTime = 0;
    
    void tfunc();
};

void ThreadedSolver::PerThread::tfunc(){
    while( !die ){
        // Wait until we have input
        std::unique_lock<std::mutex> lk(m);
        // wait is called with lock held
        cv.wait(lk, [this]{return inputReady;});
        inputReady = false; // ready for next wait
        
        if(die){
            break;
        }
        
        error.clear();
        solution.clear();
        long const startTime = gtod();
        try{
            solution = search.solution(cubeState, 24, 1000, false);
            //if(!validateSolution( cubeState, solution )){
            //    fprintf(stderr, "FAIL! %s %s\n", cubeState.c_str(), solution.c_str() );
            //}
        }
        catch(std::exception &ex){
            error = ex.what();
        }
        thinkTime = gtod()-startTime;
        
        outputReady = true;
        
        // Manual unlocking is done before notifying, to avoid waking up
        // the waiting thread only to block again 
        lk.unlock();
        cv.notify_one();
    }
}


ThreadedSolver::ThreadedSolver(){
    Kociemba::TwoPhase::initStatics();
}
ThreadedSolver::~ThreadedSolver(){
    stop();
}

void ThreadedSolver::start( int numThreads ){
    std::vector< std::unique_ptr<PerThread> > perThread;
    if( numThreads > boost::size(allMoves) ) {
        numThreads = boost::size(allMoves); // limit to number of unique starting moves
    }
    if(!perThread_.empty()){
        throw std::runtime_error("already started");
    }
    doneThreads_.clear();
    
    for(int i=0; i<numThreads; ++i){
        perThread.push_back( std::unique_ptr<PerThread>(new PerThread) );
    }
    for(int i=0; i<numThreads; ++i){
        perThread[i]->t = std::thread(
            std::function<void(void)>(
                std::bind(&PerThread::tfunc, perThread[i].get() )
            )
        );
    }
    perThread_.swap(perThread);
}
void ThreadedSolver::stop(){
    for(int i=0; i<perThread_.size(); ++i){
        {
            std::lock_guard<std::mutex> lk(perThread_[i]->m);
            perThread_[i]->die = true;
            perThread_[i]->inputReady = true;
        }
        perThread_[i]->cv.notify_one();
    }
    for(int i=0; i<perThread_.size(); ++i){
        perThread_[i]->t.join();
    }
    perThread_.clear();
    doneThreads_.clear();
}


std::string ThreadedSolver::solve(std::string const &state, char solveSpeed, unsigned long targetMS){
    if(!doneThreads_.empty()){
        waitUntilAllDone();
    }
    
    long const startThinkTime = gtod();
    long const startSolveTime = startThinkTime - 66000;
    if(perThread_.empty()){
        throw std::runtime_error("not started");
    }
    for(int i=0; i<perThread_.size(); ++i){
        {
            std::lock_guard<std::mutex> lk(perThread_[i]->m);
            perThread_[i]->cubeState = state;

            if( allMoves[i].stateMap_ ) {
                //printf( "statemap:  before %s\n", perThread_[i]->cubeState.c_str() );
                perThread_[i]->cubeState = makeMove(perThread_[i]->cubeState, allMoves[i].stateMap_);
                //printf( "statemap:  after  %s\n", perThread_[i]->cubeState.c_str() );
            }
            if( allMoves[i].transformFwd_  ) {
                //printf( "charremap: before %s\n", perThread_[i]->cubeState.c_str() );
                perThread_[i]->cubeState = remapState(perThread_[i]->cubeState, allMoves[i].transformFwd_);
                //printf( "charremap: after  %s\n", perThread_[i]->cubeState.c_str() );
            }

            perThread_[i]->inputReady = true;
            perThread_[i]->outputReady = false;
        }
        perThread_[i]->cv.notify_one();
    }
    // wait for workers
    bool allDone = false;

    doneThreads_.resize(perThread_.size(), false);
    while( !allDone ) {
        allDone = true;
        for(int i=0; i<perThread_.size(); ++i){
            //don't re-check threads
            if( doneThreads_[ i ] ) {
                continue;
            }
            std::unique_lock<std::mutex> lk(perThread_[i]->m);
            PerThread *pt = perThread_[i].get();
            //perThread_[i]->cv.wait(lk, [pt]{ return pt->outputReady;});
            if( perThread_[i]->cv.wait_for(lk, std::chrono::microseconds(1), [pt]{ return pt->outputReady;}) ) {
                doneThreads_[ i ] = true;

                //<duplicated code>
                std::string const solRaw = perThread_[i]->solution;
                std::string sol = solRaw;
                if( allMoves[i].movesPrefix_ ) {
                    sol = allMoves[i].movesPrefix_ + solRaw;
                }
                if( allMoves[i].transformRev_ ){
                    sol = remapSolution( sol, allMoves[i].transformRev_ );
                }
                
                std::string const cmd = translateMoves( sol );
                int const movesTime = timeForMoveSequence( cmd, solveSpeed );
                //<duplicated code>

                long const now = gtod();
                long const totalThinkTime = now - startThinkTime;
                long const totalEstimatedTime = now - startSolveTime + movesTime;
                if( totalEstimatedTime <  targetMS*1000 ) {
                    lastSolutionIndex_ = i;
                    printf("Early picking solution %d think=%.3fms movesTime=%.3fms totalTime=%.3f, cmd=%s\n", i, totalThinkTime/1000.0, movesTime/1000.0, totalEstimatedTime/1000.0, translateMoves(sol).c_str());
                    return sol;
                }

            } else {
                allDone = false;
            }
        }
    }
    int shortestSolutionTime=-1;
    int shortestSolutionIdx=-1;
    std::string shortestSolution;
    std::string error;
    for(int i=0; i<perThread_.size(); ++i){
        std::string const solRaw = perThread_[i]->solution;
        std::string sol = solRaw;
        if( allMoves[i].movesPrefix_ ) {
            sol = allMoves[i].movesPrefix_ + solRaw;
        }
        if( allMoves[i].transformRev_ ){
            sol = remapSolution( sol, allMoves[i].transformRev_ );
        }
        
        std::string const cmd = translateMoves( sol );
        int const movesTime = timeForMoveSequence( cmd, solveSpeed );
        long const thinkTime = perThread_[i]->thinkTime;
       
        printf("Solution %d time=%.3fms think=%.3fms cmd=%s\n", i, movesTime/1000.0, thinkTime/1000.0, cmd.c_str());
        
        if(shortestSolutionTime < 0 || movesTime < shortestSolutionTime ){
            shortestSolutionTime = movesTime;
            shortestSolution = sol;
            shortestSolutionIdx = i;
        }
        if(error.empty() && !perThread_[i]->error.empty()){
            error = perThread_[i]->error;
        }
    }

    unsigned long finishThinkTime = gtod();
    unsigned long totalThinkTime = finishThinkTime - startThinkTime;
    unsigned long totalEstimatedTime = finishThinkTime - startSolveTime + shortestSolutionTime;

    printf("Picking solution %d think=%.3fms movesTime=%.3fms totalTime=%.3f, cmd=%s\n", shortestSolutionIdx, totalThinkTime/1000.0, shortestSolutionTime/1000.0, totalEstimatedTime/1000.0, translateMoves(shortestSolution).c_str());
    lastSolutionIndex_ = shortestSolutionIdx;

    if(shortestSolution.empty()){
        throw std::runtime_error( error );
    }
    return shortestSolution;
}

void ThreadedSolver::waitUntilAllDone(){
    if(!doneThreads_.empty()){
        for(int i=0; i<perThread_.size(); ++i){
            //don't re-check threads
            if( doneThreads_[ i ] ) {
                continue;
            }
            std::unique_lock<std::mutex> lk(perThread_[i]->m);
            PerThread *pt = perThread_[i].get();
            perThread_[i]->cv.wait(lk, [pt]{ return pt->outputReady;});
        }
    }
    doneThreads_.clear();
}

