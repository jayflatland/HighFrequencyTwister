#include <string>
#include <vector>

#include "cpp_kociemba/search.h"

class ThreadedSolver{
public:
    ThreadedSolver();
    ~ThreadedSolver();
    
    void start( int numThreads );
    void stop();
    std::string solve(std::string const &state, char solveSpeed, unsigned long targetMS );

    void waitUntilAllDone();

    int lastSolutionIndex_= -1;

private:
    struct PerThread;
    std::vector< std::unique_ptr<PerThread> > perThread_;

    std::vector<bool> doneThreads_;
};


// validate that solution solves state
bool validateSolution( std::string const &state, std::string const &solution);