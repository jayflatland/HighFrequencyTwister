#pragma once
#include "util.h"
#include "capv4l2.h"
#include "serial.h"

#include "threaded_solver2.h"

typedef std::unique_ptr<CapV4L2> CapV4L2Ptr;

struct PerCam{
    CapV4L2Ptr           cap;
    SDL_Rect             srec; 
    SDL_Rect             drec; // where to render this in the window
    boost::optional<int> exposureAbsolute;
    boost::optional<int> whiteBalanceTemperature;
    boost::optional<int> gain;
    PerCam( std::string const &dev, int xres, int yres, boost::optional<int> exposureAbsolute, boost::optional<int> whiteBalanceTemperature, boost::optional<int> gain );
};

typedef std::unique_ptr<SerialNonBlocking> SerialPtr;

struct Inst{
    enum Op{
        Send,       // send STR to serial port
        Expect,     // wait until STR received from serial port
        WaitMicros, // wait for N microseconds before next instruction
        SnapCal,    // snap calibrationColors using calibrationStates[N]
        ValidateCal,// validate state against calibrationStates[N]
        SaveCal,    // save calibrationColors to disk
        Solved, 
    };
    Op          op;   // operator
    std::string str;  // string operand
    int         n;    // integer operand
};

struct App{
    ThreadedSolver        threadedSolver;
    
    SDLInitAndQuit        sdlinitAndQuit;
    TTFInitAndQuit        ttfInitAndQuit;

    SerialPtr             ser;
    std::string           serRcv;

    char                  solveSpeed = '3';
    char                  calSpeed   = '3';
    unsigned long         targetMS = 980;
    
    std::vector<Inst>     program;
    size_t                programCounter=0;
    std::string           programRcvExpected;
    unsigned long         programTimeExpected=0;
    
    void continueProgram(bool running);
    bool isRunning()const{ return programCounter<program.size(); }

    void runProgram(std::vector<Inst> const &pgm){
        if(!isRunning()){
            program = pgm;
            programCounter = 0;
            continueProgram(true);
        }
    }
    
    unsigned long     autoSolveArmTime=0;
    unsigned long     elapsedSolveTime=0;
    unsigned long     movesStartedTime=0;
    
    std::vector<Inst> getCalibrationProgram()const;
    std::vector<Inst> getCalibrationTestProgram()const;
    std::vector<Inst> getMovesProgram(std::string const &moves)const;
    std::vector<Inst> getURFProgram()const;
    std::vector<Inst> getUFLProgram()const;
    std::vector<Inst> getScrambleProgram()const;
    std::vector<Inst> getSolveProgram()const;
    std::vector<Inst> getAutoSolveProgram()const;
    
    std::string           lastSolution;
    
    void serWrite( std::string const &str ){
        if(ser){
            ser->write(str);
        }
    }
    
    // 6 colors for each position ( centers are represented but ignored )
    SDL_Color             calibrationColors[54][6];
    bool                  validateCalMismatch[54][6];
    
    std::vector< PerCam > cams;

    int                   threads      = 2;
    int const             border       = 8;
    int                   xres         = 160;
    int                   yres         = 120;
    int                   previewScale = 2;
    int                   cursorRadius = 4; // square radius in camera pixels ( radius 2 = 5x5 )
    
    void camPointToPreviewPoint(SDL_Rect const &drec, int &x, int &y){
        x = x*previewScale + drec.x;
        y = y*previewScale + drec.y;
    }
    
    SDL_WindowPtr         win;
    SDL_RendererPtr       ren;
    
    SDL_TexturePtr        texPreview;
    
    TTF_FontPtr           fonMono;
    TTF_FontPtr           fonTimer;
    
    struct Text{
        TTF_Font       *fon = nullptr;
        SDL_Color       clr;
        SDL_TexturePtr  tex;
        SDL_Rect        rec = {0,0,0,0};
        std::string     text;
        bool            changed=false;
    };
    std::vector<Text>     texts;
    
    SDL_Rect              faceletMapEdit[54];
    SDL_Rect              faceletMapEditBoundary;

    SDL_Rect              faceletMapMatched[54];
    SDL_Rect              faceletMapMatchedBoundary;

    
    int                   selectedFacelet = -1;
    int                   selectedCursor  = -1;
    int                   selectedExposureControl = -1;
    
    struct Cursor{
        int facelet=-1;
        int camera=-1;
        int x=-1;
        int y=-1;
        int radius = -1;
        boost::optional<int> associatedCursor;  // only for center facelets, defines which cursor's calibration column to use
    };
    bool haveCenterCursors = false;
    bool suspendCenters = false;
    bool const useCenters()const{ return haveCenterCursors && !suspendCenters; }
    
    typedef std::vector<Cursor> Cursors;
    Cursors               cursors;
    
    SDL_Color faceletColors[54]; // average of 9x9 pixels cetered under cursor
    int faceletStates[54];       // -1 for unknown ( if cursor not defined or calibration not run )
    int prevFaceletStates[54];
    
    void assignAssociatedCursorsForCenters();
    
    // find index of closest match to faceletColor in calibrationColors[facelet][]
    //   return -1 if none of the colors calibrationColors[facelet][] have been assigned yet
    template <class DISTFUNC>
    int findClosestCalibrationColor(int facelet, SDL_Color const &faceletColor,DISTFUNC distfunc);
    void analyzeColors(); 

    std::string configFilename;
    
    App(std::string const &configFilename);
    void load(std::string const &filename);
    void save(std::string const &filename);
    void loadCal(std::string const &filename);
    void saveCal(std::string const &filename);
    
    void setupFaceletMap(SDL_Point where, unsigned scale, unsigned gap, /*out*/SDL_Rect faceletMap[54], /*out*/SDL_Rect &boundary);
    
    // 0-53, -1 for no hit
    int hitTestFaceletMap(SDL_Point where);
    
    // 0-cams.size()-1, -1 for no hit
    //   outCamPoint set to location  within camera
    int hitTestPreview(SDL_Point where, SDL_Point &outCamPoint);
    
    void moveCursor(int dx, int dy);
    
    void run();
    void loop();    
    void renderAll();
    
    void renderPreview( PerCam &cam );
    void renderFaceletMapEdit();
    void renderFaceletMapMatched();
    void renderCalibrationColors();

    void renderCursor( Cursor const &cur, SDL_Color const &clr, bool faceletSelected, bool cursorSelected );
    
    SDL_Color cursorColor( Cursor const &cur );
    
    void clearText(unsigned row);
    void printText(unsigned row, char const *fmt, ...);
    
    void makeText(unsigned row);

    void onKeyDown(SDL_Keycode sym, Uint16 mod);
    void onKeyUp(SDL_Keycode sym, Uint16 mod);
    void onLButtonDown(int mX, int mY);
    void onLButtonUp(int mX, int mY);
    void onRButtonDown(int mX, int mY);
    void onRButtonUp(int mX, int mY);
};

