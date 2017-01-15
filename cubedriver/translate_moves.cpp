#include <string>
#include <stdexcept>

namespace{
    int const U_ = 0;
    int const R_ = 1;
    int const F_ = 2;
    int const D_ = 3;
    int const L_ = 4;
    int const B_ = 5;
}

// move ( or pair of opposite face moves )
struct Move{
    int  face1  = -1; // one of U_, L_ or F_ ( or -1 if none )
    int  times1 =  0; // 0=None, 1=CW, 2=180, 3=CCW 
    int  face2  = -1; // one of R_, D_, or B_ ( or -1 if none )
    int  times2 =  0; // 0=None, 1=CW, 2=180, 3=CCW 
    
    bool empty() const { return times1 <= 0 && times2 <= 0; }
    bool clear() { face1 = -1; times1 = 0; face2 = -1; times2 = 0; }
    
    Move() {}
    Move( int face1, int times1, int face2, int times2 )
        : face1(face1), times1(times1), face2(face2), times2(times2)
    {
    }
    Move( int face, int times ){
        switch(face){
        case U_: case L_: case F_:
            face1  = face;
            times1 = times;
            break;
        case R_: case D_: case B_:
            face2  = face;
            times2 = times;
            break;
        }        
    }
};

inline bool operator==(Move const &l, Move const &r){
    return // allow match based on times==0 regardless of face
        ( l.times1 == r.times1 && ( l.times1 == 0 || l.face1  == r.face1 ) ) &&
        ( l.times2 == r.times2 && ( l.times2 == 0 || l.face2  == r.face2 ) ) ;
}

// combines r into l if possible, and returns true
//  returns false ( and leaves both r and l unchanged ) if r can't be combined into l
inline bool combineMoves(Move &l, Move const &r){
    // l.face1 not compatible with r.face1 ( both defined but not equal )
    if( l.times1 > 0 && r.times1 > 0 && l.face1 != r.face1 ) return false;
    // l.face1 not compatible with r.face1 ( both defined but not equal )
    if( l.times2 > 0 && r.times2 > 0 && l.face2 != r.face2 ) return false;
    // l.face1 not compatible with r.face2 ( both defined but not opposite )
    if( l.times1 > 0 && r.times2 > 0 && ( l.face1 % 3 ) != ( r.face2 % 3 ) ) return false;
    // l.face2 not compatible with r.face1 ( both defined but not opposite )
    if( l.times2 > 0 && r.times1 > 0 && ( l.face2 % 3 ) != ( r.face1 % 3 ) ) return false;

    // combine r.face1 into l.face1
    if( l.times1 <= 0 ){  // l.face1 not defined:
        // just take r.face1 ( which may also be undefined )
        l.face1  = r.face1;
        l.times1 = r.times1;
    }
    else if( r.times1 > 0 ){ // l.face1 and r.face1 both defined
        // add rotations
        l.times1 = (l.times1 + r.times1) % 4;
        if( l.times1 == 0 ){
            l.face1 = -1; // canonical form of no rotation has no face
        }
    }
    //else: r.face1 not defined, leave l.face1 alone
    
    if( l.times2 <= 0 ){  // l.face2 not defined:
        // just take r.face2 ( which may also be undefined )
        l.face2  = r.face2;
        l.times2 = r.times2;
    }
    else if( r.times2 > 0 ){ // l.face2 and r.face2 both defined
        // add rotations
        l.times2 = (l.times2 + r.times2) % 4;
        if( l.times2 == 0 ){
            l.face2 = -1; // canonical form of no rotation has no face
        }
    }
    //else: r.face2 not defined, leave l.face2 alone

    return true;
}


// table to translate a Move ( including double move ) into a single byte command for the machine
std::pair<Move,char> const moveAndCmd[] = {
    { { F_,  1, -1,  0 }, 'a' },
    { { F_,  2, -1,  0 }, 'b' },
    { { F_,  3, -1,  0 }, 'c' },
    { { F_,  1, B_,  1 }, 'd' },
    { { F_,  2, B_,  1 }, 'e' },
    { { F_,  3, B_,  1 }, 'f' },
    { { -1,  0, B_,  1 }, 'g' },
    { { F_,  1, B_,  2 }, 'h' },
    { { F_,  2, B_,  2 }, 'i' },
    { { F_,  3, B_,  2 }, 'j' },
    { { -1,  0, B_,  2 }, 'k' },
    { { F_,  1, B_,  3 }, 'l' },
    { { F_,  2, B_,  3 }, 'm' },
    { { F_,  3, B_,  3 }, 'n' },
    { { -1,  0, B_,  3 }, 'o' },
    { { L_,  1, -1,  0 }, 'p' },
    { { L_,  2, -1,  0 }, 'q' },
    { { L_,  3, -1,  0 }, 'r' },
    { { L_,  1, R_,  1 }, 's' },
    { { L_,  2, R_,  1 }, 't' },
    { { L_,  3, R_,  1 }, 'u' },
    { { -1,  0, R_,  1 }, 'v' },
    { { L_,  1, R_,  2 }, 'w' },
    { { L_,  2, R_,  2 }, 'x' },
    { { L_,  3, R_,  2 }, 'y' },
    { { -1,  0, R_,  2 }, 'z' },
    { { L_,  1, R_,  3 }, 'A' },
    { { L_,  2, R_,  3 }, 'B' },
    { { L_,  3, R_,  3 }, 'C' },
    { { -1,  0, R_,  3 }, 'D' },
    { { U_,  1, -1,  0 }, 'E' },
    { { U_,  2, -1,  0 }, 'F' },
    { { U_,  3, -1,  0 }, 'G' },
    { { U_,  1, D_,  1 }, 'H' },
    { { U_,  2, D_,  1 }, 'I' },
    { { U_,  3, D_,  1 }, 'J' },
    { { -1,  0, D_,  1 }, 'K' },
    { { U_,  1, D_,  2 }, 'L' },
    { { U_,  2, D_,  2 }, 'M' },
    { { U_,  3, D_,  2 }, 'N' },
    { { -1,  0, D_,  2 }, 'O' },
    { { U_,  1, D_,  3 }, 'P' },
    { { U_,  2, D_,  3 }, 'Q' },
    { { U_,  3, D_,  3 }, 'R' },
    { { -1,  0, D_,  3 }, 'S' },
};
size_t const moveAndCmdSize = sizeof(moveAndCmd)/sizeof(moveAndCmd[0]);

int faceFromChar(char c){
    switch(c){
    case 'U': return U_;
    case 'R': return R_;
    case 'F': return F_;
    case 'D': return D_;
    case 'L': return L_;
    case 'B': return B_;
    }
    return -1; // no face
}

// Parse space separated move in common notation(s)
//  U U1 U+    -- Up face 1 quarter turn CW  
//  u U3 U- U' -- Up face 1 quarter turn CCW
//    U2       -- Up face 1 half turn
// Same for R, F, D, L, and B ( right, front, down, left, and back )
// Return pair<int face, int times> or throw std::runtime_error
//  advances str to start of next move ( or to end of string )
std::pair<int,int> parseMove(char const *&str){
    while(::isspace(*str)){
        ++str;
    }
    char c = *str++;
    int face = faceFromChar(::toupper(c));
    if(face < 0){
        throw std::runtime_error("parseMove: invalid face character");
    }
    int times = 0;
    if(::islower(c)){
        times = 3; // CCW
    }
    else{
        // look for suffix
        switch(*str){
        case '1':
        case '+':
            times = 1;
            ++str;
            break;
        case '2':
            times = 2;
            ++str;
            break;
        case '\'':
        case '3':
        case '-':
            times = 3;
            ++str;
            break;
        default:
            times = 1;
            // no suffix: don't consume character
            break;
        }
    }
    while(::isspace(*str)){
        ++str;
    }
    return std::pair<int,int>(face, times);
}


// Parse space separated move in common notation(s)
//  U U1 U+    -- Up face 1 quarter turn CW  
//  u U3 U- U' -- Up face 1 quarter turn CCW
//    U2       -- Up face 1 half turn
// Same for R, F, D, L, and B ( right, front, down, left, and back )
// Return valid Move or throw std::runtime_error
Move parseMove2(char const *&str){
    auto const m =  parseMove(str);
    return Move(m.first, m.second);
}

// lookup machine command character given move
char lookupMoveCommand(Move const &move){
    bool found = false;
    for( auto &&mc : moveAndCmd ){
        if( mc.first == move ){
            return mc.second;
        }
    }
    throw std::runtime_error("lookupMoveCommand: move not found");
}

// translate standard notation moves string into 
//  command string for machine, combining adjacent
//  pairs when possible
std::string translateMoves(std::string const &moves){
    std::string out;
    Move accum;
    char const *str = moves.c_str();
    while(*str){
        Move move = parseMove2(str);
        if(!combineMoves(accum, move)){
            // accum can't be empty if combine failed
            out += lookupMoveCommand(accum);
            accum = move;
        }
    }
    if(!accum.empty()){
        out += lookupMoveCommand(accum);
    }
    return out;
}

