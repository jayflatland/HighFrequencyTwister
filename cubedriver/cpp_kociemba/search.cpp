#include <sys/time.h>

#include "search.h"



namespace Kociemba { namespace TwoPhase {
    
void initStatics(){
    static bool initialized=false;
    if(!initialized){
        initialized = true;
        CubieCube::initStatics();
        CoordCube::initStatics();
    }
}    
    
    
}}