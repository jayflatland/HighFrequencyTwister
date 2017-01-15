#include <signal.h>
#include <stdarg.h>
#include <limits.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>

#include "cubedriver.h"
#include "config.h"
#include "translate_moves.h"

//#include "threaded_solver.h"

#include "cpp_kociemba/cubiecube.h"
#include "cpp_kociemba/search.h"
#include "cpp_kociemba/tools.h"

using boost::property_tree::ptree;


volatile int quit = 0;
void interrupted(int){
    quit = 1;
}



int const U_ = 0;
int const R_ = 1;
int const F_ = 2;
int const D_ = 3;
int const L_ = 4;
int const B_ = 5;


struct HSV{
    unsigned int  h;  // 0 .. 359  
    unsigned int  s;  // 0 .. 255 
    unsigned int  v;  // 0 .. 255 
};

void rgb2hsv(SDL_Color const &rgb, HSV &hsv){
    int const min = std::min(rgb.r, std::min(rgb.g, rgb.b));
    int const max = std::max(rgb.r, std::max(rgb.g, rgb.b));
    
    hsv.v = max;
    
    int const delta = max-min;
    if(delta > 0){
        // max will not be zero because max-min > 0 and max>=min 
        hsv.s = (255*delta) / max;

        int h;
        if( rgb.r == max ){
            h =       ((rgb.g - rgb.b)*60)/delta; // between yellow & magenta
        }
        else if( rgb.g == max ){
            h = 120 + ((rgb.b - rgb.r)*60)/delta; // between cyan & yellow
        }
        else{
            h = 240 + ((rgb.r - rgb.g)*60)/delta; // between magenta & cyan
        }
        if( h < 0 ){
            h += 360;
        }
        hsv.h = h;
    }
    else{
        hsv.s = 0; // grayscale
        hsv.h = 0; // undefined
    }
}


// pair of face rotations
std::string const URF_ = translateMoves("UD' RL' FB' UD'");  // command = PulP
std::string const UFL_ = translateMoves("UD' FB' R'L UD'");  // command = PlAP

// calibrationProgram phase N will transition cube from calibrationStates[N] to calibrationStates[N+1] ( wraps around to 0 after 8 )
int const calibrationStates[9][6] = {
    { U_, R_, F_, D_, L_, B_ },
    { F_, U_, R_, B_, D_, L_ },
    { R_, F_, U_, L_, B_, D_ },
    { B_, D_, R_, F_, U_, L_ },
    { R_, B_, D_, L_, F_, U_ },
    { D_, R_, B_, U_, L_, F_ },
    { L_, F_, D_, R_, B_, U_ },
    { D_, L_, F_, U_, R_, B_ },
    { F_, D_, L_, B_, U_, R_ }
};

// the 24 possible orientations of the centers
int const orientations[24][6] = {
    { U_, R_, F_, D_, L_, B_ }, 
    { U_, B_, R_, D_, F_, L_ },
    { U_, L_, B_, D_, R_, F_ },
    { U_, F_, L_, D_, B_, R_ },
    { R_, F_, U_, L_, B_, D_ },
    { R_, D_, F_, L_, U_, B_ },
    { R_, B_, D_, L_, F_, U_ },
    { R_, U_, B_, L_, D_, F_ },
    { F_, U_, R_, B_, D_, L_ },
    { F_, L_, U_, B_, R_, D_ },
    { F_, D_, L_, B_, U_, R_ },
    { F_, R_, D_, B_, L_, U_ },
    { D_, R_, B_, U_, L_, F_ },
    { D_, F_, R_, U_, B_, L_ },
    { D_, L_, F_, U_, R_, B_ },
    { D_, B_, L_, U_, F_, R_ },
    { L_, F_, D_, R_, B_, U_ },
    { L_, U_, F_, R_, D_, B_ },
    { L_, B_, U_, R_, F_, D_ },
    { L_, D_, B_, R_, U_, F_ },
    { B_, D_, R_, F_, U_, L_ },
    { B_, L_, D_, F_, R_, U_ },
    { B_, U_, L_, F_, D_, R_ },
    { B_, R_, U_, F_, L_, D_ }
};


std::vector<Inst> App::getCalibrationProgram() const {
    char const onStr[] = { calSpeed, calSpeed, calSpeed, '[', 0 };
    std::vector<Inst> const calibrationProgram = {
        // start up
        { Inst::Send,      onStr,       0 },
        { Inst::Expect,    onStr,       0 },
        { Inst::WaitMicros,"",     500000 }, // time enable and for camera to auto-adjust before first snap
        // state 0
        { Inst::SnapCal,   "",          0 },
        { Inst::Send,      URF_,        0 },
        { Inst::Expect,    URF_,        0 },
        { Inst::WaitMicros,"",     500000 },
        // state 1
        { Inst::SnapCal,   "",          1 },
        { Inst::Send,      URF_,        0 },
        { Inst::Expect,    URF_,        0 },
        { Inst::WaitMicros,"",     500000 },
        // state 2`
        { Inst::SnapCal,   "",          2 },
        { Inst::Send,      UFL_,        0 },
        { Inst::Expect,    UFL_,        0 },
        { Inst::WaitMicros,"",     500000 },
        // state 3
        { Inst::SnapCal,   "",          3 },
        { Inst::Send,      URF_,        0 },
        { Inst::Expect,    URF_,        0 },
        { Inst::WaitMicros,"",     500000 },
        // state 4
        { Inst::SnapCal,   "",          4 },
        { Inst::Send,      URF_,        0 },
        { Inst::Expect,    URF_,        0 },
        { Inst::WaitMicros,"",     500000 },
        // state 5
        { Inst::SnapCal,   "",          5 },
        { Inst::Send,      UFL_,        0 },
        { Inst::Expect,    UFL_,        0 },
        { Inst::WaitMicros,"",     500000 },
        // state 6
        { Inst::SnapCal,   "",          6 },
        { Inst::Send,      URF_,        0 },
        { Inst::Expect,    URF_,        0 },
        { Inst::WaitMicros,"",     500000 },
        // state 7
        { Inst::SnapCal,   "",          7 },
        { Inst::Send,      URF_,        0 },
        { Inst::Expect,    URF_,        0 },
        { Inst::WaitMicros,"",     500000 },
        // state 8
        { Inst::SnapCal,   "",          8 },
        { Inst::Send,      UFL_,        0 },
        { Inst::Expect,    UFL_,        0 },
        { Inst::WaitMicros,"",     500000 },
        // shut down
        { Inst::SaveCal,   "",          0 },
        { Inst::Send,      "]]]]",      0 },
        { Inst::Expect,    "]]]]",      0 },
    };
    return calibrationProgram;
}

std::vector<Inst> App::getCalibrationTestProgram() const {
    char const onStr[] = { calSpeed, calSpeed, calSpeed, '[', 0 };
    std::vector<Inst> const calibrationTestProgram = {
        // start up
        { Inst::Send,           onStr,       0 },
        { Inst::Expect,         onStr,       0 },
        { Inst::WaitMicros,     "",     500000 }, // time enable and for camera to auto-adjust before first snap
        // state 0     
        { Inst::ValidateCal,    "",          0 },
        { Inst::Send,           URF_,        0 },
        { Inst::Expect,         URF_,        0 },
        { Inst::WaitMicros,     "",     500000 },
        // state 1     
        { Inst::ValidateCal,    "",          1 },
        { Inst::Send,           URF_,        0 },
        { Inst::Expect,         URF_,        0 },
        { Inst::WaitMicros,     "",     500000 },
        // state 2`     
        { Inst::ValidateCal,    "",          2 },
        { Inst::Send,           UFL_,        0 },
        { Inst::Expect,         UFL_,        0 },
        { Inst::WaitMicros,     "",     500000 },
        // state 3     
        { Inst::ValidateCal,    "",          3 },
        { Inst::Send,           URF_,        0 },
        { Inst::Expect,         URF_,        0 },
        { Inst::WaitMicros,     "",     500000 },
        // state 4     
        { Inst::ValidateCal,    "",          4 },
        { Inst::Send,           URF_,        0 },
        { Inst::Expect,         URF_,        0 },
        { Inst::WaitMicros,     "",     500000 },
        // state 5     
        { Inst::ValidateCal,    "",          5 },
        { Inst::Send,           UFL_,        0 },
        { Inst::Expect,         UFL_,        0 },
        { Inst::WaitMicros,     "",     500000 },
        // state 6     
        { Inst::ValidateCal,    "",          6 },
        { Inst::Send,           URF_,        0 },
        { Inst::Expect,         URF_,        0 },
        { Inst::WaitMicros,     "",     500000 },
        // state 7     
        { Inst::ValidateCal,    "",          7 },
        { Inst::Send,           URF_,        0 },
        { Inst::Expect,         URF_,        0 },
        { Inst::WaitMicros,     "",     500000 },
        // state 8     
        { Inst::ValidateCal,    "",          8 },
        { Inst::Send,           UFL_,        0 },
        { Inst::Expect,         UFL_,        0 },
        { Inst::WaitMicros,     "",     500000 },
        // shut down     
        { Inst::SaveCal,        "",          0 },
        { Inst::Send,           "]]]]",      0 },
        { Inst::Expect,         "]]]]",      0 },
    };
    return calibrationTestProgram;
}


std::vector<Inst> App::getMovesProgram(std::string const &moves) const {
    std::string const commands = translateMoves(moves);
    
    char const onStr[] = { calSpeed, calSpeed, calSpeed, '[', 0 };
    std::vector<Inst> const urfProgram = {
        // start up
        { Inst::Send,      onStr,       0 },
        { Inst::Expect,    onStr,       0 },
        { Inst::WaitMicros,"",     100000 },
        { Inst::Send,      commands,    0 },
        { Inst::Expect,    commands,    0 },
        { Inst::WaitMicros,"",     100000 },
        { Inst::Send,      "]]]]",      0 },
        { Inst::Expect,    "]]]]",      0 },
    };
    return urfProgram;
}



std::vector<Inst> App::getURFProgram() const {
    char const onStr[] = { calSpeed, calSpeed, calSpeed, '[', 0 };
    std::vector<Inst> const urfProgram = {
        // start up
        { Inst::Send,      onStr,       0 },
        { Inst::Expect,    onStr,       0 },
        { Inst::WaitMicros,"",     100000 },
        { Inst::Send,      URF_,        0 },
        { Inst::Expect,    URF_,        0 },
        { Inst::WaitMicros,"",     100000 },
        { Inst::Send,      "]]]]",      0 },
        { Inst::Expect,    "]]]]",      0 },
    };
    return urfProgram;
}

std::vector<Inst> App::getUFLProgram() const {
    char const onStr[] = { calSpeed, calSpeed, calSpeed, '[', 0 };
    std::vector<Inst> const uflProgram = {
        // start up
        { Inst::Send,      onStr,       0 },
        { Inst::Expect,    onStr,       0 },
        { Inst::WaitMicros,"",     100000 },
        { Inst::Send,      UFL_,        0 },
        { Inst::Expect,    UFL_,        0 },
        { Inst::WaitMicros,"",     100000 },
        { Inst::Send,      "]]]]",      0 },
        { Inst::Expect,    "]]]]",      0 },
    };
    return uflProgram;
}

std::vector<Inst> App::getScrambleProgram() const {
    std::string scramble;
    for(int i=0; i<20; ++i){
        scramble += "URFDLB"[rand()%6];
    }
    std::string scrambleCmd;
    try{
        scrambleCmd = translateMoves(scramble);
    }
    catch(std::exception &ex){
        return std::vector<Inst>();
    }
    char const onStr[] = { calSpeed, calSpeed, calSpeed, '[', 0 };
    std::vector<Inst> const scrambleProgram = {
        { Inst::Send,      onStr,           0 },
        { Inst::Expect,    onStr,           0 },
        { Inst::WaitMicros,"",         100000 },
        { Inst::Send,      scrambleCmd,     0 },
        { Inst::Expect,    scrambleCmd,     0 },
        { Inst::WaitMicros,"",         100000 },
        { Inst::Send,      "]]]]",          0 },
        { Inst::Expect,    "]]]]",          0 },
    };
    return scrambleProgram;
}

std::vector<Inst> App::getSolveProgram() const {
    if(lastSolution.empty()){
        return std::vector<Inst>();
    }
    char const onStr[] = { calSpeed, calSpeed, calSpeed, '[', 0 };
    std::vector<Inst> const solveProgram = { 
        { Inst::Send,      onStr,           0 },
        { Inst::Expect,    onStr,           0 },
        { Inst::WaitMicros,"",         100000 },
        { Inst::Send,      lastSolution,    0 },
        { Inst::Expect,    lastSolution,    0 },
        { Inst::WaitMicros,"",         100000 },
        { Inst::Send,      "]]]]",          0 },
        { Inst::Expect,    "]]]]",          0 },
    };
    return solveProgram;
}

std::vector<Inst> App::getAutoSolveProgram() const {
    if(lastSolution.empty()){
        return std::vector<Inst>();
    }
    char const onStr[] = { solveSpeed, solveSpeed, solveSpeed, '[', 0 };
    std::vector<Inst> const solveProgram = { 
        { Inst::Send,      onStr,           0 },
        { Inst::Expect,    onStr,           0 },
        { Inst::Send,      lastSolution,    0 },
        { Inst::Expect,    lastSolution,    0 },
        { Inst::Solved,    "",              0 },
        { Inst::WaitMicros,"",         100000 },
        { Inst::Send,      "]]]]",          0 },
        { Inst::Expect,    "]]]]",          0 },
    };
    return solveProgram;
}


// chart of cube facelet locations
//  
//     0  1  2  3  4  5  6  7  8  9 10 11 
//             Up
//            +--+--+--+
// 0          | 0| 1| 2|
//            +--+--+--+
// 1          | 3| 4| 5|
//            +--+--+--+
// 2  Left    | 6| 7| 8| Right  | Back
//   +--+--+--+--+--+--+--+--+--+--+--+--+
// 3 |36|37|38|18|19|20| 9|10|11|45|46|47|
//   +--+--+--+--+--+--+--+--+--+--+--+--+
// 4 |39|40|41|21|22|23|12|13|14|48|49|50|
//   +--+--+--+--+--+--+--+--+--+--+--+--+
// 5 |42|43|44|24|25|26|15|16|17|51|52|53|
//   +--+--+--+--+--+--+--+--+--+--+--+--+
// 6          |27|28|29|
//            +--+--+--+
// 7          |30|31|32|
//            +--+--+--+
// 8          |33|34|35|
//            +--+--+--+
//             Down


// colors for SDL user interface
//                             R   G   B   A
SDL_Color const clrNull     = {  0,  0,  0,  0}; // using alpha=0 as "no color" flag
SDL_Color const clrBlack    = {  0,  0,  0,255};
SDL_Color const clrGray     = {128,128,128,255};
SDL_Color const clrDarkRed  = { 64,  0,  0,255};
SDL_Color const clrWhite    = {255,255,255,255};
SDL_Color const clrOrange   = {255,165,  0,255};
SDL_Color const clrBlue     = {  0,128,255,255};
SDL_Color const clrYellow   = {255,255,  0,255};
SDL_Color const clrRed      = {255,  0,  0,255};
SDL_Color const clrDkGreen  = {  0,128,  0,255};
SDL_Color const clrGreen    = {  0,255,  0,255};


// return square of pythagorean distance in R,G,B space
inline int clrDistSquared_RGB(SDL_Color a, SDL_Color b){
    int dr = (a.r-b.r);
    int dg = (a.g-b.g);
    int db = (a.b-b.b);
    return dr*dr + dg*dg + db*db;
}

// angular difference ( e.g. 359 is close to 1 )
inline int angleDiff(int a, int b){
    return (((a-b)+540)%360) - 180;
}

inline int clrDistSquared_HS(SDL_Color a, SDL_Color b){
    
    HSV hsv1;
    HSV hsv2;
    rgb2hsv(a, hsv1);
    rgb2hsv(b, hsv2);
    // HSV distance, ignore V,  
    int const dh = angleDiff(hsv2.h,hsv1.h);
    int const ds = hsv2.s - hsv1.s;
    return dh*dh + ds*ds;
}


// colors used to draw cube state in UI
SDL_Color const cubeColors[6]{
    clrWhite  , // up
    clrOrange , // right
    clrBlue   , // front
    clrYellow , // down
    clrRed    , // left
    clrDkGreen, // back
};

// translate facelet number ( 0 .. 53 ) into face number ( 0 .. 5 )
inline int faceletToFace(int facelet){
    return facelet / 9;
}
inline bool isCenterFacelet(int facelet){
    return ( facelet % 9 ) == 4;
}

// 2D map location in 12x9 grid of 54 facelets in standard order
SDL_Point const faceletMapLogical[54] = {
    // Up
    { 3, 0}, { 4, 0}, { 5, 0}, 
    { 3, 1}, { 4, 1}, { 5, 1}, 
    { 3, 2}, { 4, 2}, { 5, 2},     
    // Right
    { 6, 3}, { 7, 3}, { 8, 3}, 
    { 6, 4}, { 7 ,4}, { 8, 4}, 
    { 6, 5}, { 7 ,5}, { 8, 5}, 
    // Front
    { 3, 3}, { 4, 3}, { 5, 3}, 
    { 3, 4}, { 4, 4}, { 5, 4}, 
    { 3, 5}, { 4, 5}, { 5, 5}, 
    //Down
    { 3, 6}, { 4, 6}, { 5, 6},
    { 3, 7}, { 4, 7}, { 5, 7},
    { 3, 8}, { 4, 8}, { 5, 8},
    //Left
    { 0, 3}, { 1, 3}, { 2, 3},
    { 0, 4}, { 1, 4}, { 2, 4},
    { 0, 5}, { 1, 5}, { 2, 5},
    //Back
    { 9, 3}, {10, 3}, {11, 3},
    { 9, 4}, {10, 4}, {11, 4},
    { 9, 5}, {10, 5}, {11, 5},
};

PerCam::PerCam( std::string const &dev, int xres, int yres, boost::optional<int> exposureAbsolute, boost::optional<int> whiteBalanceTemperature, boost::optional<int> gain )
    : cap( new CapV4L2( dev, xres, yres, 2 ) )
    , srec( { 0, 0, cap->xres(), cap->yres() } ) // xres(),yres() can be adjusted by device
    , drec( { 0, 0, 0, 0 } )
    , exposureAbsolute(exposureAbsolute)
    , whiteBalanceTemperature(whiteBalanceTemperature)
    , gain(gain)
{
}

App::App(std::string const &configFilename) 
    : sdlinitAndQuit(SDL_INIT_VIDEO) 
    , ttfInitAndQuit()
    , configFilename(configFilename)
{
    for(int f=0; f<54;  ++f){
        for(int c=0; c<6; ++c){
            calibrationColors[f][c] = clrNull;
            validateCalMismatch[f][c] = false;
        }
        faceletColors[f] = clrNull;
        faceletStates[f] = -1;
        prevFaceletStates[f] = -1;
    }
    
}

void App::setupFaceletMap(SDL_Point where, unsigned scale, unsigned gap, /*out*/SDL_Rect faceletMap[54], /*out*/SDL_Rect &boundary){
    for(int i=0; i<54; ++i){
        SDL_Point const &log = faceletMapLogical[i];
        SDL_Rect        &phy = faceletMap[i];
        
        phy.x = where.x + log.x * (scale+gap);
        phy.y = where.y + log.y * (scale+gap);
        phy.w = scale;
        phy.h = scale;
        
        if( i == 0 ){
            boundary = phy;
        }
        else{
            SDL_UnionRect(&boundary, &phy, &boundary);
        }
    }
}

inline bool pointInRect(SDL_Point const &p, SDL_Rect const &r){
    return p.x >= r.x && p.y >= r.y && p.x < (r.x+r.w) && p.y < (r.y+r.h);
}

// 0-53, -1 for no hit
int App::hitTestFaceletMap(SDL_Point where){
    for(int i=0; i<54; ++i){
        if(pointInRect(where, faceletMapEdit[i])){
            return i;
        }
        if(pointInRect(where, faceletMapMatched[i])){
            return i;
        }
    }
    return -1;
}


// 0-cams.size()-1, -1 for no hit
//   outCamPoint set to location  within camera
int App::hitTestPreview(SDL_Point where, SDL_Point &outCamPoint){
    for(int i=0; i<cams.size(); ++i){
        if(pointInRect(where, cams[i].drec)){
            outCamPoint.x = ((where.x - cams[i].drec.x)/previewScale);
            outCamPoint.y = ((where.y - cams[i].drec.y)/previewScale);
            return i;
        }
    }
    return -1;
}
    

void App::renderPreview( PerCam &cam ){
    void *pixels;
    int pitch;
    if(SDL_LockTexture(texPreview.get(), nullptr, &pixels, &pitch) < 0) {
        throwSDL("SDL_LockTexture");
    }
    unsigned char *dst = static_cast<unsigned char *>(pixels);
    unsigned char *src = cam.cap->rgbaFrame();
    int const yres=cam.cap->yres();
    int const rowBytes = cam.cap->xres()*4;
    for(int y=0; y<yres; ++y){
        memcpy(dst, src, rowBytes);
        src += rowBytes;
        dst += pitch;
    }
    SDL_UnlockTexture(texPreview.get());

    SDL_RenderCopy(ren.get(), texPreview.get(), NULL, &cam.drec);
}
void App::makeText(unsigned row){
    if(row < texts.size()){
        Text &t = texts[row];
        
        t.tex.reset();
        if(!t.text.empty()){
            SDL_SurfacePtr surTxt(
                TTF_RenderText_Solid(t.fon, t.text.c_str(), t.clr)
            );
            if(!surTxt){
                throwSDL("TTF_RenderText_Solid");
            }
            t.tex.reset(
                SDL_CreateTextureFromSurface(ren.get(), surTxt.get())
            );
            if(!t.tex){
                throwSDL("SDL_CreateTextureFromSurface");
            }
            t.rec.w = surTxt->w;
            t.rec.h = surTxt->h;
        }
    }
}

void App::clearText(unsigned row){
    if(row < texts.size()){
        texts[row].text.clear();
        texts[row].changed = true;
    }
}
void App::printText(unsigned row, char const *fmt, ...){
    if(row < texts.size()){
        char buff[128];
        va_list ap;

        va_start(ap, fmt);
        vsnprintf(buff, sizeof(buff), fmt, ap);
        va_end(ap);
        
        texts[row].text += buff;
        texts[row].changed = true;
    }
}

void App::renderFaceletMapEdit(){
    for(int i=0; i<54; ++i){
        SDL_Color const fc = faceletColors[i];
        if(fc.a > 0){
            SDL_SetRenderDrawColor(ren.get(), fc.r, fc.g, fc.b, 255);
            SDL_RenderFillRect(ren.get(), &faceletMapEdit[i]);
        }
        else{
            SDL_Color const c = cubeColors[i/9];
            SDL_SetRenderDrawColor(ren.get(), c.r, c.g, c.b, 255);
            SDL_RenderDrawRect(ren.get(), &faceletMapEdit[i]);
        }
    }
    if( selectedFacelet >= 0 ){
        SDL_Rect sel = faceletMapEdit[selectedFacelet];
        sel.x -= 2;
        sel.y -= 2;
        sel.w += 4;
        sel.h += 4;
        SDL_SetRenderDrawColor(ren.get(), clrGreen.r, clrGreen.g, clrGreen.b, 255);
        SDL_RenderDrawRect(ren.get(), &sel);
    }
}
void App::renderFaceletMapMatched(){
    for(int i=0; i<54; ++i){
        int const state = faceletStates[i];
        if( state >= 0 ){
            SDL_Color const c = cubeColors[state];
            SDL_SetRenderDrawColor(ren.get(), c.r, c.g, c.b, 255);
            SDL_RenderFillRect(ren.get(), &faceletMapMatched[i]);
        }
        else{
          SDL_SetRenderDrawColor(ren.get(), 255, 255, 255, 255);
          SDL_RenderDrawRect(ren.get(), &faceletMapMatched[i]);
        }
    }
    if( selectedFacelet >= 0 ){
        SDL_Rect sel = faceletMapMatched[selectedFacelet];
        sel.x -= 2;
        sel.y -= 2;
        sel.w += 4;
        sel.h += 4;
        SDL_SetRenderDrawColor(ren.get(), clrGreen.r, clrGreen.g, clrGreen.b, 255);
        SDL_RenderDrawRect(ren.get(), &sel);
    }
}

void App::renderCalibrationColors(){
    int const x = faceletMapMatchedBoundary.x + faceletMapMatchedBoundary.w + border;
    int const y = faceletMapMatchedBoundary.y;
    
    for(int f=0; f<54; ++f){
        if( isCenterFacelet(f) ){
            continue;
        }
        
        for(int c=0; c<6; ++c){
            SDL_Rect r;
            r.x = x + f*12;
            r.y = y + c*12;
            r.w = 8;
            r.h = 8;
            SDL_Color clr = calibrationColors[f][c];
            if(clr.a <= 0 ){
                clr = clrGray;
            }
            SDL_SetRenderDrawColor(ren.get(), clr.r, clr.g, clr.b, 255);
            SDL_RenderFillRect(ren.get(), &r);

            if( validateCalMismatch[f][c] ){
                r.x -= 2;
                r.y -= 2;
                r.w += 4;
                r.h += 4;
                SDL_SetRenderDrawColor(ren.get(), clrGreen.r, clrGreen.g, clrGreen.b, 255);
                SDL_RenderDrawRect(ren.get(), &r);
            }
        }
        
        if( f == selectedFacelet ){
            // underline selected column
            SDL_Rect r;
            r.x = x + f*12;
            r.y = y + 6*12;
            r.w = 8;
            r.h = 1;
            SDL_SetRenderDrawColor(ren.get(), clrGreen.r, clrGreen.g, clrGreen.b, 255);
            SDL_RenderDrawRect(ren.get(), &r);
        }

    }
}


SDL_Color App::cursorColor( Cursor const &cur ){
    int r=0, g=0, b=0;
    int n = 0;
    
    int const radius  = cur.radius;
    int const rr = radius*radius;
    
    auto const &cam = cams[cur.camera];        
    unsigned char rgba[4];
    for(int dx=-radius; dx<radius; ++dx){
        for(int dy=-radius; dy<=radius; ++dy){
            if( dx*dx + dy*dy <= rr ){
                SDL_Point p;
                p.x = cur.x + dx;
                p.y = cur.y + dy;
                if(pointInRect(p, cam.srec)){
                    cam.cap->getRGBA(p.x, p.y, rgba);
                    r += rgba[0];
                    g += rgba[1];
                    b += rgba[2];
                    ++n;
                }
            }
        }
    }
    SDL_Color ret = clrNull;
    if( n > 0 ){
        ret.r = ( r + r + n ) / (n+n);
        ret.g = ( g + g + n ) / (n+n);
        ret.b = ( b + b + n ) / (n+n);
        ret.a = 255;
    }
    return ret;
}

void drawFilledCircle( SDL_Renderer *ren, SDL_Rect const &clip, int cx, int cy, int r ){
    int const rr = r*r;
    for(int dx=-r; dx<r; ++dx){
        for(int dy=-r; dy<=r; ++dy){
            if( dx*dx + dy*dy <= rr ){
                SDL_Point p;
                p.x = cx + dx;
                p.y = cy + dy;
                if(pointInRect(p, clip)){
                    SDL_RenderDrawPoint(ren, p.x, p.y);
                }
            }
        }
    }
}


void App::renderCursor( Cursor const &cur, SDL_Color const &clr, bool faceletSelected, bool cursorSelected ){
    auto const &cam = cams[cur.camera];        
    
    int cx = cur.x;
    int cy = cur.y;
    camPointToPreviewPoint( cam.drec, cx, cy );
    
    int const x0 = cam.drec.x;
    int const x1 = cam.drec.x + cam.drec.w;
    int const y0 = cam.drec.y;
    int const y1 = cam.drec.y + cam.drec.h;
    
    int const dr = cur.radius*previewScale;
    
    SDL_Color const clrBack = !faceletSelected ? clrBlack : clr;
    SDL_Color const clrFore =  faceletSelected ? clrBlack : clr;
    
    SDL_SetRenderDrawColor(ren.get(), clrBack.r, clrBack.g, clrBack.b, 255);

    drawFilledCircle(ren.get(), cam.drec, cx, cy, dr);

    if(cursorSelected){
        // cross hairs
        if( x0 < cx ) SDL_RenderDrawLine(ren.get(), x0, cy, cx, cy );
        if( cx < x1 ) SDL_RenderDrawLine(ren.get(), cx, cy, x1, cy );
        if( y0 < cy ) SDL_RenderDrawLine(ren.get(), cx, y0, cx, cy );
        if( cy < y1 ) SDL_RenderDrawLine(ren.get(), cx, cy, cx, y1 );
    }
    
    if( cur.associatedCursor ){
        // connect cursors with a line
        Cursor &ac = cursors[*cur.associatedCursor];
        int cx2 = ac.x;
        int cy2 = ac.y;
        camPointToPreviewPoint( cams[ac.camera].drec, cx2, cy2 );

        SDL_SetRenderDrawColor(ren.get(), clrWhite.r, clrWhite.g, clrWhite.b, 255 );
        
        SDL_RenderDrawLine(ren.get(), cx, cy, cx2, cy2 );
    }
    
    
    SDL_SetRenderDrawColor(ren.get(), clrFore.r, clrFore.g, clrFore.b, 255);
    drawFilledCircle(ren.get(), cam.drec, cx, cy, dr-1);
}

void App::renderAll(){
    SDL_Color const bg = (autoSolveArmTime > 0) ? clrDarkRed : clrBlack;
    SDL_SetRenderDrawColor(ren.get(), bg.r, bg.g, bg.b, 255);
    
    SDL_RenderClear(ren.get());
    for( auto &&cam : cams ){
        renderPreview( cam );        
    }
    
    for( int i=0; i<cursors.size(); ++i){
        auto const &cursor = cursors[i];
     
        bool const faceletSelected = (cursor.facelet == selectedFacelet);
        int const face = faceletToFace(cursor.facelet);
        
        bool const cursorSelected  = faceletSelected && (i == selectedCursor);

        renderCursor( cursor, cubeColors[face], faceletSelected, cursorSelected );
    }
    
    for( auto &&cam : cams ){
        SDL_Rect r = cam.drec;
        r.y -= 6;
        r.h = 4;
        
        if( cam.exposureAbsolute ){
            int const exp = *cam.exposureAbsolute;
            r.w = std::min( (20*exp)/33, r.w );
            SDL_SetRenderDrawColor(ren.get(), clrGreen.r, clrGreen.g, clrGreen.b, 255);
        }
        else{
            SDL_SetRenderDrawColor(ren.get(), clrYellow.r, clrYellow.g, clrYellow.b, 255);
        }
        SDL_RenderFillRect(ren.get(), &r);
        
        r = cam.drec;
        r.y += r.h + 2;
        r.h = 4;
        
        if( cam.whiteBalanceTemperature ){
            int const wb = *cam.whiteBalanceTemperature;
            r.w = std::min( (320*wb)/10000, r.w );
            SDL_SetRenderDrawColor(ren.get(), clrGreen.r, clrGreen.g, clrGreen.b, 255);
        }
        else{
            SDL_SetRenderDrawColor(ren.get(), clrYellow.r, clrYellow.g, clrYellow.b, 255);
        }
        SDL_RenderFillRect(ren.get(), &r);
    }
    
    if( selectedCursor >= 0 ){
        int const selectedPreview = cursors[selectedCursor].camera;
        if( selectedPreview >= 0 ){
            SDL_Rect sel = cams[selectedPreview].drec;
            sel.x -= 2;
            sel.y -= 2;
            sel.w += 4;
            sel.h += 4;
            SDL_SetRenderDrawColor(ren.get(), clrGreen.r, clrGreen.g, clrGreen.b, 255);
            SDL_RenderDrawRect(ren.get(), &sel);
        }
    }
    
    renderFaceletMapEdit();
    renderFaceletMapMatched();
    renderCalibrationColors();
    for(int i=0; i<texts.size(); ++i){
        if(texts[i].changed){
            makeText(i);
            texts[i].changed=false;
        }
        if(texts[i].tex){
            SDL_RenderCopy(ren.get(), texts[i].tex.get(), nullptr, &texts[i].rec); 
        }
    }
    SDL_RenderPresent(ren.get());
}

// return square of pythagorean distance in R,G,B space
inline int distSquared(int x1, int x2, int y1, int y2){
    int dx = (x2-x1);
    int dy = (y2-y1);
    return dx*dx + dy*dy;
}
void App::assignAssociatedCursorsForCenters(){
    haveCenterCursors = false;
    for(auto &&cc : cursors){
        cc.associatedCursor = boost::none; 
        if(isCenterFacelet(cc.facelet)){ // only centers have associated cursors
            boost::optional<int> bestCursor;
            int bestDist; 
            for( int i=0; i<cursors.size(); ++i){
                Cursor const &ac = cursors[i];
                if(!isCenterFacelet(ac.facelet)){ // only non-centes are valid associates
                    if(faceletToFace(ac.facelet) == faceletToFace(cc.facelet)){ // only cursors on the same face are valid associates
                        if(ac.camera == cc.camera){ // only cursors on the same camera are valid associates
                            int const dist = distSquared( ac.x, cc.x, ac.y, cc.y );
                            if( !bestCursor || dist < bestDist ){ // closer cursors are better candidates
                                bestCursor = i;
                                bestDist = dist;
                                haveCenterCursors = true;
                            }
                        }
                    }
                }
            }
            cc.associatedCursor = bestCursor;
        }
    }
}

namespace{
    // nine each U,R,F,D,L,B
    char const solvedState[55] = "UUUUUUUUURRRRRRRRRFFFFFFFFFDDDDDDDDDLLLLLLLLLBBBBBBBBB";
}    

template <class DISTFUNC>
int App::findClosestCalibrationColor(int facelet, SDL_Color const &faceletColor,DISTFUNC distfunc){
    int bestIdx  = -1;
    int bestDist;
    for( int c=0; c<6; ++c){
        SDL_Color const cc = calibrationColors[facelet][c];
        if( cc.a > 0 ){
            int const dist = distfunc( faceletColor, cc );
            if( bestIdx < 0 || dist < bestDist ){
                bestIdx  = c;
                bestDist = dist;
            }
        }
    }
    return bestIdx;
}

void App::analyzeColors(){
    for(int f=0; f<54;  ++f){
        faceletColors[f] = clrNull;
        faceletStates[f] = -1;
    }
    
    int centerHits[6][6]; // centerHits[face][matched color] = hits
    if(useCenters()){
        for(int f=0; f<6; ++f){
            for(int c=0; c<6; ++c){
                centerHits[f][c] = 0;
            }
        }
    }
    
    for( auto &&cur : cursors ){
        SDL_Color const cc = cursorColor( cur );
        // if a non-center facelet has multiple cursors, the only last one encountered is honored.
        //  should fix UI to only allow multiple cursors for centers
        faceletColors[cur.facelet] = cc;
        
        if( useCenters() && isCenterFacelet(cur.facelet) && cur.associatedCursor ){
            Cursor const &ac = cursors[*cur.associatedCursor];
            if( cc.a > 0 ){
                int const idx = findClosestCalibrationColor(ac.facelet, cc, clrDistSquared_HS);
                if(idx >= 0){
                    ++centerHits[ faceletToFace( cur.facelet ) ][idx];
                }
            }
        }
    }
    
    
    //printf("   U  R  F  D  L  B\n");
    //for(int f=0; f<6; ++f){
    //    printf("%c ", "URFDLB"[f]);
    //    for(int c=0; c<6; ++c){
    //        int const h = centerHits[f][c];
    //        printf("%2d ", h);
    //    }
    //    printf("\n");
    //}
    
    clearText(3);
    int maxMatchOrientation = -1; 
    if(useCenters()){
        // for each center face, pick the matched color with the most hits
        int centerStates[6];
        char centerStatesStr[7];    
        for(int f=0; f<6; ++f){
            int maxHits = -1;
            int maxHitsColor = - 1;
            for(int c=0; c<6; ++c){
                int const hits = centerHits[f][c];
                if(hits > 0 && (maxHits < 0 || hits > maxHits)){
                    maxHits = hits;
                    maxHitsColor = c;
                }
            }
            centerStates[f] = maxHitsColor;
            centerStatesStr[f] = ("?URFDLB"+1)[maxHitsColor];
        }
        centerStatesStr[6] = 0;    
        printText(3, "centerStatesStr=%s", centerStatesStr); 
        
        // of all 24 possible center orientations, pick the one that is the most matches with centerStates
        int maxMatches=-1;
        for(int o=0; o<24; ++o){
            int matches = 0;
            for(int c=0; c<6; ++c){
                if(orientations[o][c] == centerStates[c]){
                    ++matches;
                }
            }
            if(maxMatches < 0 || matches > maxMatches){
                maxMatches = matches;
                maxMatchOrientation = o;
            }
        }
    }
    
    int transform[6];
    if( useCenters() ){
        // build transform to re-orient into URFDLB expected by solver
        for(int i=0; i<6; ++i){
            transform[ orientations[maxMatchOrientation][i] ] = i;
        }
    }
    else{
        // centers assumed to be URFDLB, build null transform
        for(int i=0; i<6; ++i){
            transform[i] = i;
        }
    }
    
    for(int f=0; f<54;  ++f){
        int state = -1;
        if( !isCenterFacelet(f) ){
            SDL_Color const fc = faceletColors[f];
            if( fc.a > 0 ){
                state = findClosestCalibrationColor(f, fc, clrDistSquared_RGB);
            }
        }
        else if(useCenters()){
            // fill in the centers from the chosen center orientation
            state = orientations[maxMatchOrientation][faceletToFace(f)];
        }else{
            // center is "known"
            state = f/9;            
        }
        if(state >= 0){
            faceletStates[f] = transform[state];
        }
    }

    // ckociemba is very fast, but still slow enough to limit frame rate for non-trivial solutions
    //  so only find new solution when recognized state changes
    bool const faceletStateChanged = memcmp(faceletStates, prevFaceletStates, sizeof(faceletStates)) != 0;
    if( faceletStateChanged ){
        memcpy(prevFaceletStates, faceletStates, sizeof(prevFaceletStates) );

        // generate state string in format expected by ckociemba/solution()
        char stateStr[55];
        for(int i=0; i<54; ++i){
            if( faceletStates[ i ] >= 0 ){
                stateStr[i] = "URFDLB"[faceletStates[ i ]];
            }
            else{
                stateStr[i] = '.';
            }
        }
        stateStr[54]=0;
        
        lastSolution.clear();
        
        clearText(0);
        if(memcmp(stateStr, solvedState, 54) == 0){
            // ckociemba has a quirk that causes it to generate a (perfectly valid) solution to an already solved
            //  cube instead of just returning an empty solution.
            // so will avoid asking for a solution in that case
            printText(0, "State: %s, SOLVED", stateStr );
        }
        else{
            std::string const err = Kociemba::TwoPhase::Tools::verify(stateStr);
            if(err.empty()){
                //Kociemba::TwoPhase::Search search;
                
                try{
                    //std::string const sol = search.solution(stateStr, 24, 1000, false);
                    //std::string const sol = threadedSolver.solution( stateStr );
                    std::string const sol = threadedSolver.solve( stateStr, solveSpeed, targetMS );
                    std::string cmd;
                    try{
                        cmd = translateMoves(sol);
                        fprintf(stderr, "solution: %s (%d moves)\n", cmd.c_str(), (int)cmd.length() );
                        lastSolution = cmd;
                        printText(0, "State: %s, Solution Command: %s", stateStr, lastSolution.c_str());
                    }
                    catch(std::exception &ex){
                        // this can only happen if there is a bug in translateMoves() ( or possibly in solution() )
                        printText(0, "State: %s, ERROR: translate fail. Sol: %s", stateStr, sol.c_str());
                    }
                }
                catch(std::exception &ex){
                    // because we pre-validated the cube state, this should only happen
                    //  in timeout ( which should never happen becuase we allowed 100 seconds! )
                    printText(0, "State: %s, ERROR: solution fail.", ex.what());
                }
            }
            else{
                printText(0, "State: %s, ERROR: %s", stateStr, err.c_str() );
            }
        }
    }
    
    if(autoSolveArmTime > 0){
       if(!isRunning()){
            if(!lastSolution.empty()){
                movesStartedTime = gtod();
                runProgram(getAutoSolveProgram());
            }
        }
    }
    
    if(autoSolveArmTime > 0){
        // safety disable ( move this to watchdog on microcontroller )
        if(gtod() - autoSolveArmTime > 10000000){
fprintf(stderr, "abort auto solve!\n");          
            autoSolveArmTime = 0;
            // abort any running program
            program.clear();
            programCounter = 0; 
            programRcvExpected.clear();
            programTimeExpected = 0;
            serRcv.clear();
            serWrite("]]]]");
        }
    }
    
}


void App::load(std::string const &fileName){
    Config::Root config = Config::load(fileName);
    
    try{
        ser.reset( new SerialNonBlocking( config.serDev ) );
    }
    catch(std::exception &ex){
        fprintf(stderr, "Error opening serial device '%s', caught: '%s'\n", config.serDev.c_str(), ex.what());
        fprintf(stderr, "Running without serial support\n");
    }
    threads      = config.threads;
    solveSpeed   = config.solveSpeed;
    calSpeed     = config.calSpeed;
    xres         = config.xres;
    yres         = config.yres;
    previewScale = config.previewScale;
    targetMS     = config.targetMS;
    
    
    if(config.cameras.empty()){
        throw std::runtime_error("No cameras in cube.info");
    }
    for( int i=0; i<config.cameras.size(); ++i){
        auto &camera = config.cameras[i];
        cams.push_back( PerCam(camera.dev, xres, yres, camera.exposureAbsolute, camera.whiteBalanceTemperature, camera.gain ) );
        PerCam &cam = cams.back();
    }

    for( int i=0; i<config.cursors.size(); ++i){
        auto &cursor = config.cursors[i];
        if(cursor.facelet < 0 || cursor.facelet >= 54){
            throw std::runtime_error("cube.info: invalid cursor.facelet");
        }
        if(cursor.camera < 0 || cursor.camera >=cams.size()){
            throw std::runtime_error("cube.info: invalid cursor.camera");
        }
        auto &cam = cams[cursor.camera];
        if(cursor.x < 0 || cursor.x >= cam.cap->xres()){
            throw std::runtime_error("cube.info: invalid cursor.x");
        }
        if(cursor.y < 0 || cursor.y >= cam.cap->yres()){
            throw std::runtime_error("cube.info: invalid cursor.y");
        }
        Cursor cur;
        cur.facelet = cursor.facelet;
        cur.camera  = cursor.camera;
        cur.x       = cursor.x;
        cur.y       = cursor.y;
        cur.radius  = cursor.radius;
        cursors.push_back( cur );
    }
    assignAssociatedCursorsForCenters();
}

void App::save(std::string const &filename){
    Config::Root config;
    if(ser){
        config.serDev = ser->dev();
    }
    config.threads       = threads;
    config.solveSpeed    = solveSpeed;
    config.calSpeed      = calSpeed;
    config.xres          = xres;
    config.yres          = yres;
    config.previewScale  = previewScale;
    config.targetMS		 = targetMS;
    
    for( auto &&cam : cams ){
        Config::Camera camera;
        camera.dev = cam.cap->dev();
        camera.exposureAbsolute = cam.exposureAbsolute;
        camera.whiteBalanceTemperature = cam.whiteBalanceTemperature;
        camera.gain = cam.gain;
        config.cameras.push_back(camera);
    }
    for( auto &&cur : cursors ){
        Config::Cursor cursor;
        cursor.facelet = cur.facelet;
        cursor.camera  = cur.camera;
        cursor.x       = cur.x;
        cursor.y       = cur.y;
        cursor.radius  = cur.radius;
        config.cursors.push_back(cursor);
    }
    
    Config::save(filename, config);
} 

void App::loadCal(std::string const &filename){
    FILE *fcal = fopen(filename.c_str(), "r");
    if(fcal){
        SDL_Color tempCalibrationColors[54][6];
        if( ::fread(tempCalibrationColors, sizeof(SDL_Color), 54*6, fcal) == 54*6 ){
            memcpy( calibrationColors, tempCalibrationColors, sizeof(calibrationColors) );
        }
        ::fclose(fcal);
    }
}
void App::saveCal(std::string const &filename){
    FILE *fcal = fopen(filename.c_str(), "w");
    if(fcal){
        ::fwrite(calibrationColors, sizeof(SDL_Color), 54*6, fcal);
        ::fclose(fcal);
    }
}


void App::run(){    
    threadedSolver.start(threads);

    
    // arrange preview N wide by 1 high
    SDL_Rect previewRect     = { 0, 0, 0, 0 };
    SDL_Rect biggestCamSRect = { 0, 0, 0, 0 };
    
    previewRect.w = border;
    for( auto &&cam : cams ){
        biggestCamSRect.w = std::max( biggestCamSRect.w, cam.srec.w );
        biggestCamSRect.h = std::max( biggestCamSRect.h, cam.srec.h );
       
        cam.drec.x = previewRect.w;
        cam.drec.y = border;
        cam.drec.w = (cam.srec.w*previewScale);
        cam.drec.h = (cam.srec.h*previewScale);

        previewRect.h = std::max( previewRect.h, border + cam.drec.h + border );
        
        previewRect.w += cam.drec.w + border;
    }
    
    SDL_Point const faceletMapEditLoc = { border, previewRect.h };
    setupFaceletMap(faceletMapEditLoc, 16, 4, faceletMapEdit, faceletMapEditBoundary);
    faceletMapEditBoundary.w += border;
    faceletMapEditBoundary.h += border;

    SDL_Point const faceletMapMatchedLoc = { faceletMapEditBoundary.x+faceletMapEditBoundary.w+16, previewRect.h };
    setupFaceletMap(faceletMapMatchedLoc, 16, 4, faceletMapMatched, faceletMapMatchedBoundary);
    faceletMapMatchedBoundary.w += border;
    faceletMapMatchedBoundary.h += border;

    SDL_Rect windowRect = {0,0,0,0};
    SDL_UnionRect(&previewRect, &faceletMapEditBoundary, &windowRect);
    SDL_UnionRect(&windowRect, &faceletMapMatchedBoundary, &windowRect);

    fonMono.reset(
        TTF_OpenFont("fonts/FreeMono.ttf", 18)
    );
    if(!fonMono){
        throwSDL("TTF_OpenFont");
    }
    fonTimer.reset(
        TTF_OpenFont("fonts/DIGITALDREAM.ttf", 72)
    );
    if(!fonTimer){
        throwSDL("TTF_OpenFont");
    }
    
    texts.resize(4);
    texts[0].fon   = fonMono.get();
    texts[0].clr   = clrGreen;
    texts[0].rec.x = border;
    texts[0].rec.y = windowRect.h;
    texts[0].rec.w = 0;
    texts[0].rec.h = TTF_FontHeight(texts[0].fon);

    texts[1].fon   = fonTimer.get();
    texts[1].clr   = clrRed;
    texts[1].rec.x = border;
    texts[1].rec.y = texts[0].rec.y + texts[0].rec.h;
    texts[1].rec.w = 0;
    texts[1].rec.h = TTF_FontHeight(texts[1].fon);

    texts[2].fon   = fonMono.get();
    texts[2].clr   = clrWhite;
    texts[2].rec.x = border;
    texts[2].rec.y = texts[1].rec.y + texts[1].rec.h;
    texts[2].rec.w = 0;
    texts[2].rec.h = TTF_FontHeight(texts[2].fon);

    texts[3].fon   = fonMono.get();
    texts[3].clr   = clrYellow;
    texts[3].rec.x = border;
    texts[3].rec.y = texts[2].rec.y + texts[2].rec.h;
    texts[3].rec.w = 0;
    texts[3].rec.h = TTF_FontHeight(texts[3].fon);

    windowRect.h = texts[3].rec.y + texts[3].rec.h + border;
    
    win.reset( 
        SDL_CreateWindow(
            "CubeDriver", 
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
            windowRect.w, windowRect.h, 
            SDL_WINDOW_SHOWN
        ) 
    );
    if(!win){
        throwSDL("SDL_CreateWindow");
    }
    
    //SDL_SetWindowFullscreen(win.get(), SDL_WINDOW_FULLSCREEN);// SDL_WINDOW_FULLSCREEN_DESKTOP, 0
    
    
    ren.reset(
        SDL_CreateRenderer(
            win.get(), 
            -1, 
            0
        ) 
    );    
    if(!ren){
        throwSDL("SDL_CreateRenderer");
    }
    

    texPreview.reset( 
        SDL_CreateTexture(
            ren.get(),
            // SDL preferred SDL_PIXELFORMAT_ARGB8888
            SDL_PIXELFORMAT_ABGR8888,
            SDL_TEXTUREACCESS_STREAMING,
            biggestCamSRect.w, biggestCamSRect.h
        )
    );
    if(!texPreview){
        throwSDL("SDL_CreateTexture");
    }
    
    renderAll();
    
    for( auto &&cam : cams ){
        try{
            cam.cap->setControlValue(CapV4L2::BacklightCompensation, 1);
        }
        catch(std::exception &ex){
            fprintf(stderr, "Warning: could not set control 'CapV4L2::BacklightCompensation' to '1' [%s]\n", ex.what());
        }
        try{
            if(cam.whiteBalanceTemperature){
                //fprintf(stderr, "Setting WhiteBalanceTemperature to %d\n", *cam.whiteBalanceTemperature);
                cam.cap->setControlValue(CapV4L2::AutoWhiteBalance, 0);
                cam.cap->setControlValue(CapV4L2::WhiteBalanceTemperature, *cam.whiteBalanceTemperature);
            } 
            else{
                //fprintf(stderr, "Setting WhiteBalanceTemperature to auto\n" );
                cam.cap->setControlValue(CapV4L2::AutoWhiteBalance, 1);
            }
        }
        catch(std::exception &ex){
            fprintf(stderr, "Warning: could not set control 'CapV4L2::AutoWhiteBalance' to '0' [%s]\n", ex.what());
        }
        try{
            if(cam.gain){
                cam.cap->setControlValue(CapV4L2::Gain, *cam.gain);
            }
        }
        catch(std::exception &ex){
            fprintf(stderr, "Warning: could not set control 'CapV4L2::Gain' to '0' [%s]\n", ex.what());
        }
        char const *ctlId=nullptr;   // for error reporting
        int   ctlValue=-1;
        try{
            if(cam.exposureAbsolute){
                ctlId = "ExposureAuto";
                ctlValue = 1;
                cam.cap->setControlValue(CapV4L2::ExposureAuto, 1); // 1=Manual Mode
                ctlId = "ExposureAbsolute";
                ctlValue = *cam.exposureAbsolute;
                cam.cap->setControlValue(CapV4L2::ExposureAbsolute, *cam.exposureAbsolute);
            }
            else{
                ctlId = "ExposureAuto";
                ctlValue = 3;
                cam.cap->setControlValue(CapV4L2::ExposureAuto, 3); // 3=Aperture Priority Mode
            }
        }
        catch(std::exception &ex){
            fprintf(stderr, "Warning: could not set control '%s' to '%d' [%s]\n", ctlId, ctlValue, ex.what());
        }
        cam.cap->captureOn();
    }
    
    loop();
   
    for( auto &&cam : cams ){
        cam.cap->captureOff();
    }
    int i=0;
    for( auto &&cam : cams ){
        printf("%d: %d frames in %.2f seconds (%.2f FPS)\n", i++, cam.cap->frames(), cam.cap->elapsed(), cam.cap->fps() ); 
    }
    
    threadedSolver.stop();
}

void App::moveCursor(int dx, int dy){
    if(selectedCursor >= 0 && selectedCursor < cursors.size()){
        auto &cursor = cursors[selectedCursor];
        auto &srec   = cams[cursor.camera].srec;
        cursor.x += dx;
        cursor.y += dy;
        if(cursor.x < 0) cursor.x = 0;
        if(cursor.x >= srec.w) cursor.x = srec.w-1;
        if(cursor.y < 0) cursor.y = 0;
        if(cursor.y >= srec.h) cursor.y = srec.h-1;
        
        assignAssociatedCursorsForCenters();
    }
}


inline int roundExposure(int exposure){
    return std::floor( std::floor((exposure + 16.66)/33.33) * 33.33 + 0.1 );
}

void App::onKeyDown(SDL_Keycode sym, Uint16 mod){
    switch(sym){
    case SDLK_PAGEUP:
        selectedFacelet = -1;
        if(selectedCursor >= 0){
            if(++selectedCursor>=cursors.size()){
                selectedCursor = 0;
            }
        }
        else if(!cursors.empty()){
            selectedCursor  = 0;
        }
        
        if(selectedCursor >= 0){
            selectedFacelet = cursors[selectedCursor].facelet;
        }
        break;
    case SDLK_PAGEDOWN:
        selectedFacelet = -1;
        if(selectedCursor >= 0){
            if(--selectedCursor<0){
                selectedCursor = cursors.size()-1;
            }
            selectedFacelet = cursors[selectedCursor].facelet;
        }
        else if(!cursors.empty()){
            selectedCursor  = cursors.size()-1;
        }
        
        if(selectedCursor >= 0){
            selectedFacelet = cursors[selectedCursor].facelet;
        }
        break;
    case SDLK_HOME:
        selectedFacelet = -1;
        if(!cursors.empty()){
            selectedCursor  = 0;
            selectedFacelet = cursors[selectedCursor].facelet;
        }
        else{
            selectedCursor  = -1;
        }
        break;
    case SDLK_END:
        selectedFacelet = -1;
        if(!cursors.empty()){
            selectedCursor  = cursors.size()-1;
            selectedFacelet = cursors[selectedCursor].facelet;
        }
        else{
            selectedCursor  = -1;
        }
        break;
    case SDLK_DELETE:
        selectedFacelet = -1;
        if(selectedCursor >= 0){
            cursors.erase(cursors.begin() + selectedCursor);
            if(cursors.empty()){
                selectedCursor  = -1;
            }
            else if(selectedCursor == cursors.size()){
               selectedCursor = cursors.size()-1;
            }
            assignAssociatedCursorsForCenters();
        }
        if(selectedCursor >= 0){
            selectedFacelet = cursors[selectedCursor].facelet;
        }
        break;
    case SDLK_LEFT:
        moveCursor(-1, 0);
        break;
    case SDLK_RIGHT:
        moveCursor(1, 0);
        break;
    case SDLK_UP:
        moveCursor(0, -1);
        break;
    case SDLK_DOWN:
        moveCursor(0, 1);
        break;
    case SDLK_F1: // validate calibration
        {
        	suspendCenters = true;
            for(int f=0; f<54;  ++f){
                for( int c=0; c<6; ++c){
                    validateCalMismatch[f][c] = false;
                }
            }
            runProgram(getCalibrationTestProgram());
        }
        break;
    case SDLK_F2: // calibrate
        {
            // zero out old calibration ( not necessary, but fun to watch fill in )
            suspendCenters = true;
            for(int f=0; f<54;  ++f){
                for(int c=0; c<6; ++c){
                    calibrationColors[f][c] = clrNull;
                    validateCalMismatch[f][c] = false;
                }
            }
            runProgram(getCalibrationProgram());
        }
        break;
    case SDLK_F3: // U->R->F face rotation
        runProgram(getURFProgram());
        break;
    case SDLK_F4: // U->F->L face rotation
        runProgram(getUFLProgram());
        break;
    case SDLK_F5: //Superflip
        runProgram(getMovesProgram("U R2 F B R B2 R U2 L B2 R U' D' R2 F R' L B2 U2 F2"));
        break;
    case SDLK_F6: // "hard" checkerboard
        runProgram(getMovesProgram("F B2 R' D2 B R U D' R L' D' F' R2 D F2 B'"));
        break;
    case SDLK_F7: // solve
        runProgram(getSolveProgram());
        break;
    case SDLK_F8: // "easy" checkerboard
        runProgram(getMovesProgram("U2 D2 F2 B2 L2 R2"));
        break;
    case SDLK_F9: // spiral
        runProgram(getMovesProgram("L' B' D U R U' R' D2 R2 D L D' L' R' F U"));
        break;
    case SDLK_SPACE:
    case SDLK_F10: // enable auto solve ( one shot )
        if(!isRunning()){
            if(autoSolveArmTime == 0){
                unsigned long const now = gtod();
                autoSolveArmTime = now;
                elapsedSolveTime = 0;
            }
            else{
                autoSolveArmTime = 0;
                elapsedSolveTime = 0;
            }
        }
        break;
    case SDLK_F11: // scramble
        runProgram(getScrambleProgram());
        break;
    case SDLK_F12: // cube in a cube in a cube
        runProgram(getMovesProgram("U' L' U' F' R2 B' R F U B2 U B' L U' F U R F'"));
        break;
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case 'a':
    case 'b':
    case 'c':
        if(mod & KMOD_CTRL){
            // advance exposure by 33 for camera N
            int const n = sym-'1';
            if( n < cams.size() ){
                if( cams[n].exposureAbsolute ){
                    cams[n].exposureAbsolute = roundExposure( std::min( *cams[n].exposureAbsolute + 33, 500 ) );
                    try{
                        cams[n].cap->setControlValue(CapV4L2::ExposureAbsolute, *cams[n].exposureAbsolute);
                        printf("cam %c exposure set to %d\n", sym, *cams[n].exposureAbsolute);
                    }
                    catch(std::exception &ex){
                        fprintf(stderr, "ex: %s\n", ex.what());
                    }
                }
            }
        }
        else if(mod & KMOD_SHIFT){
            // reduce exposure by 33 for camera N
            int const n = sym-'1';
            if( n < cams.size() ){
                if( cams[n].exposureAbsolute ){
                    cams[n].exposureAbsolute = roundExposure( std::max( *cams[n].exposureAbsolute - 33, 33 ) );
                    try{
                        cams[n].cap->setControlValue(CapV4L2::ExposureAbsolute, *cams[n].exposureAbsolute);
                        printf("cam %c exposure set to %d\n", sym, *cams[n].exposureAbsolute);
                    }
                    catch(std::exception &ex){
                        fprintf(stderr, "ex: %s\n", ex.what());
                    }
                }
            }
        }
        else if(mod & KMOD_ALT){
            // toggle auto exposure for camera N
            int const n = sym-'1';
            if( n < cams.size() ){
                if( cams[n].exposureAbsolute ){
                    try{
                        cams[n].cap->setControlValue(CapV4L2::ExposureAuto, 3); // 3=Aperture Priority Mode
                        cams[n].exposureAbsolute = boost::none;
                        printf("cam %c exposure set to auto\n", sym);
                    }
                    catch( std::exception &ex){
                        fprintf(stderr, "ex: %s\n", ex.what());
                    }
                }
                else{
                    try{
                        int const ea = 166; // default value
                        cams[n].cap->setControlValue(CapV4L2::ExposureAuto, 1); // 1=Manual Mode
                        cams[n].cap->setControlValue(CapV4L2::ExposureAbsolute, ea);
                        cams[n].exposureAbsolute = ea;
                        printf("cam %c exposure set to %d\n", sym, *cams[n].exposureAbsolute);
                    }
                    catch( std::exception &ex){
                        fprintf(stderr, "ex: %s\n", ex.what());
                    }
                }
            }
        }
        else{
            if(::isdigit(sym)){
                cursorRadius = sym-'0';
            }
            else if(::isalpha(sym)){
                cursorRadius = (sym-'a')+10;
            }
            
            if(selectedCursor >= 0){
                cursors[selectedCursor].radius = cursorRadius;
            }
        }
        break;
    case '[':
    case ']':
        if(ser){
            ser->write(std::string(1,static_cast<char>(sym)));
        }
        break;
    }
    
}
void App::onKeyUp(SDL_Keycode sym, Uint16 mod){
}
void App::onLButtonDown(int mX, int mY){
    SDL_Point const where={mX,mY};
    
    
    int const f = hitTestFaceletMap(where);
    if(f >= 0 /* && !isCenterFacelet(f) */ ){
        if(selectedFacelet == f){
            selectedFacelet = -1;
        }
        else{
            selectedFacelet = f;
        }
        // deselct cursor and preview, they will be chosen below
        selectedCursor  = -1;
    }
    
    SDL_Point camPoint = {-1,-1};
    int const p = hitTestPreview(where, camPoint);
    if(p>=0 && selectedFacelet >= 0){
        Cursor cur;
        cur.camera  = p;
        cur.facelet = selectedFacelet;
        cur.x       = camPoint.x;
        cur.y       = camPoint.y;
        cur.radius  = cursorRadius;
        cursors.push_back(cur);
        // deselct cursor, it will be chosen below
        selectedCursor  = cursors.size()-1;
        
        assignAssociatedCursorsForCenters();
    }
    
    if(selectedFacelet >= 0){
        if(selectedCursor < 0){
            for(int i=0; i<cursors.size(); ++i){
                auto &cursor = cursors[i];
                if(cursor.facelet == selectedFacelet){
                    selectedCursor = i;
                    break;
                }
            }
        }
        if(selectedCursor >= 0){
            selectedFacelet = cursors[selectedCursor].facelet;
        }
    }
}
void App::onLButtonUp(int mX, int mY){
}
void App::onRButtonDown(int mX, int mY){
}
void App::onRButtonUp(int mX, int mY){
}



void App::continueProgram(bool running){
    while( programCounter < program.size() ){
        auto const &inst = program[programCounter];
        switch(inst.op){
        case Inst::Send:
            serRcv.clear();
            serWrite(inst.str);
            break;
        case Inst::Expect:
            if(running){ 
                programRcvExpected = inst.str;
                return; // resume on next loop()
            }
            if( serRcv.length() < programRcvExpected.length() ){
                return; // wait more
            }
            if( serRcv != programRcvExpected ){
                //error
                fprintf(stderr, "ERROR: received '%s' while expecting '%s'\n", serRcv.c_str(), programRcvExpected.c_str() );
                programCounter = program.size();
                return;
            }
            programRcvExpected.clear();
            running=true;
            break;
        case Inst::WaitMicros:
            if(running){
                programTimeExpected = gtod() + inst.n;
                return; // resume on next loop()
            }
            if( gtod() < programTimeExpected ){
                return; // wait more
            }
            programTimeExpected = 0;
            running=true;
            break;
        case Inst::SnapCal:
            for( int facelet=0; facelet<54; ++facelet ){
                int const face = faceletToFace(facelet); 
                int const faceColor = calibrationStates[inst.n][face];  // "color" at face
                calibrationColors[facelet][faceColor] = faceletColors[facelet];
            }
            suspendCenters = false;
            break;
        case Inst::ValidateCal:
            for( int facelet=0; facelet<54; ++facelet ){
                if(isCenterFacelet(facelet)){
                    continue;
                }
                int const face = faceletToFace(facelet);
                int const expected = calibrationStates[inst.n][face];  // "color" at face
                int const observed = faceletStates[facelet];
                if( expected != observed ){
                    validateCalMismatch[facelet][expected] = true;
                }
            }
            suspendCenters = false;
            break;
        case Inst::SaveCal:
            saveCal(configFilename + ".cal");
            break;
        case Inst::Solved:
            if( autoSolveArmTime > 0 ){
            	unsigned long now = gtod();
                elapsedSolveTime = now - autoSolveArmTime;
                unsigned long thinkTime = movesStartedTime - autoSolveArmTime;
                unsigned long movesTime = now - movesStartedTime;
                fprintf(stderr, "Solve Time: %.0fms  Think Time: %.0fms  Moves Time: %.0fms\n", 1.0e-3*elapsedSolveTime, 1.0e-3*thinkTime, 1.0e-3*movesTime );
                autoSolveArmTime = 0;

                // wait for threaded solvers to finish now that we have captured the elapsedSolveTime
                threadedSolver.waitUntilAllDone();
            }
            break;
        }
        ++programCounter;
    }
}

void App::loop(){
    unsigned long const startTime = gtod();

    
    //capture loop
    std::vector<pollfd> pfds;
    pfds.reserve(cams.size());
    while(!quit){
        pfds.clear();
        for( auto &&cam : cams ){
            pollfd pfd;
            pfd.fd = cam.cap->fd();
            pfd.events = POLLIN|POLLRDHUP|POLLERR;
            pfd.revents = 0;
            pfds.push_back(pfd);
        }
        if(ser){
            pollfd pfd;
            pfd.fd = ser->fd();
            pfd.events = POLLIN|POLLRDHUP|POLLERR;
            if(ser->writesBuffered()){
                pfd.events |= POLLOUT;
            }
            pfd.revents = 0;
            pfds.push_back(pfd);
        }
       
        int timeout = autoSolveArmTime>0 ? 0 : 1;
        
        int const n = poll_retry(&pfds[0], pfds.size(), 1);
        if(n < 0){
            perrorExit("poll");
        }
        else{ // n > 0
            unsigned long const us = elapsedSolveTime;
            unsigned long const ms = us / 1000;
            unsigned long const s  = ms / 1000;
            unsigned long const m  = s  /   60;
            clearText(1);
            printText(1, "%02u:%02u.%03u", m, s%60, ms%1000);
            // get frames that changed
            clearText(2);
            for( int i=0; i<cams.size(); ++i){
                if(pfds[i].revents){
                    cams[i].cap->nextFrameBlocking();
                }
                printText(2, "fps[%d]=%.2f ", i, cams[i].cap->lastFPS());
            }
            
            if(ser){
                if( pfds.back().revents & POLLOUT ){
                    ser->onReadyToWrite();
                }
                if( pfds.back().revents & ~POLLOUT ){
                    serRcv += ser->read();
                }
            }
            //clearText(2);
            //printText(2, "serRcv: %s", serRcv.c_str());

            //clearText(3);
            //printText(3, "calibration: %d match, %d mismatch", validateCalMatchCount, validateCalMismatchCount );
            
            analyzeColors();
        }
        
        // periodic
        SDL_Event e;
        if(SDL_PollEvent(&e)){
            switch(e.type) {
            case SDL_QUIT: 
                quit= true;
                break;
            case SDL_KEYDOWN:
                onKeyDown(e.key.keysym.sym,e.key.keysym.mod);
                break;
            case SDL_KEYUP: 
                onKeyUp(e.key.keysym.sym,e.key.keysym.mod);
                break;
            case SDL_MOUSEBUTTONDOWN: 
                switch(e.button.button) {
                case SDL_BUTTON_LEFT: 
                    onLButtonDown(e.button.x,e.button.y);
                    break;
                case SDL_BUTTON_RIGHT: 
                    onRButtonDown(e.button.x,e.button.y);
                    break;
                }
                break;
            case SDL_MOUSEBUTTONUP: 
                switch(e.button.button) {
                case SDL_BUTTON_LEFT: 
                    onLButtonUp(e.button.x,e.button.y);
                    break;
                case SDL_BUTTON_RIGHT: 
                    onRButtonUp(e.button.x,e.button.y);
                    break;
                }
                break;
            default:
                break;
            }                
        }                
            
        // periodic
        continueProgram(false); // wake up running program

        if( autoSolveArmTime > 0 ){
            elapsedSolveTime = gtod() - autoSolveArmTime;
        }
        
        renderAll();
    }
}

int main(int argc, char *argv[]){
    Kociemba::TwoPhase::initStatics();
    
    std::string configFilename = "cube";
    if( argc == 2 ) {
        configFilename = argv[ 1 ];
    }
    signal(SIGINT, interrupted);
    try{
        App app(configFilename);
        app.load(configFilename+".info");
        app.loadCal(configFilename+".cal");
        app.run();
        app.saveCal(configFilename+".cal");
        app.save(configFilename+".info");
    }
    catch(std::exception &ex){
        fprintf(stderr, "caught: %s\n", ex.what());
    }
    return EXIT_FAILURE;
}
