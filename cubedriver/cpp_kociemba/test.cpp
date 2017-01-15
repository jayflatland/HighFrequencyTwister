#include <iostream>

#include "search.h"
#include "tools.h"


int main(){
    Kociemba::TwoPhase::Search search; // ctor will calc pruning tables, may take a full second

    /*
    std::string const rnd = Kociemba::TwoPhase::Tools::randomCube();
    std::string const sol = search.solution( rnd, 24, 1000, false );
    fprintf(stderr, "RND=%s\n", rnd.c_str());
    fprintf(stderr, "SOL=%s\n", sol.c_str());
    */

    
    std::string str;
    while(std::getline(std::cin, str)){
        std::string const err = Kociemba::TwoPhase::Tools::verify(str);
        if(!err.empty()){
            std::cerr << "Errr: '" << err << "'" << std::endl;
            return 1;
        }
        
        std::string const sol = search.solution( str, 24, 1000, false );
        
        puts(sol.c_str());
    }
    
    return 0;
}