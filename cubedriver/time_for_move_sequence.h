#pragma once

#include <string>

inline int timeForMoveSequence( const std::string &cmd, char solveSpeed ) {
    static std::set<char> const halfTurnMovesMoves = { 'b', 'e', 'h', 'i', 'j', 'k', 'm', 'q', 't', 'w', 'x', 'y', 'z', 'B', 'F', 'I', 'L', 'M', 'N', 'O', 'Q' };
    int t = 0;
    int t90 = 39, t180 = 65;
    switch( solveSpeed ) {
        case '1':   t90 = 48; t180 = 75; break;
        case '2':   t90 = 39; t180 = 65; break;
        case '3':   t90 = 37; t180 = 62; break;
        case '4':   t90 = 37; t180 = 58; break;
        case '5':   t90 = 35; t180 = 56; break;
    }
    for( auto & c : cmd ) {
        if( halfTurnMovesMoves .count( c ) > 0 ) { t += 1000 * ( t180 + 2 ); }
        else                                     { t += 1000 * ( t90  + 2 ); }
    }
    return t;
}

