#!/usr/bin/env python

import cv2
import sys
import numpy as np
import threading
import time
import serial
import subprocess
import random
import os

if os.name == 'nt':
    serialDev = "COM3"
    capDevNum = 0
else:
    serialDev = "/dev/ttyACM0"
    capDevNum = 0


"""
#./solutionfinder LF FU DR DB RU FR UB UL DF BR LD BL DRF BUR BDL BLU BRD DFL FUL FRU
#D2U2L3B1R1U3B2F2L2D3L1B2L2D1L2B2D2L2D1D2L2B2L2U2L2D2L2
#Orientation: FU UB FD BL BD LF LU BR RU DR LD RF FRU DRF URB LUB DLB ULF RDB FLD
#Cmdline: ./solutionfinder FU UB FD BL BD LF LU BR RU DR LD RF FRU DRF URB LUB DLB ULF RDB FLD
#Output: ddbLfddulddRDuurbbdrrDlldllbbuddllffrrfflluurruu
#Solving time: 1.960

#Cmdline: ./solutionfinder DL RD BR UF BU FL BD FR BL UL RU DF FUL RUF FLD FDR LUB RDB DLB RBU
#Output: hmrbuhuRglUrjdgDhjDjgdmhjkmjhjgjh
#Solving time: 1.855

#Orientation: RU BR LB LD BU LU DR FD FU BD RF LF FUL BUR FLD FDR DLB FRU RDB LUB
#Cmdline: ./solutionfinder RU BR LB LD BU LU DR FD FU BD RF LF FUL BUR FLD FDR DLB FRU RDB LUB
#Output: bDLfubdUldRUlDjkhjdjgumhj;gmk;k
#Solve Time:          1.738
#Scan and Solve Time: 5.780

 1=O  2=O  3=Y  4=R  5=B  6=B  7=G  8=O
 9=W 10=R 11=W 12=Y 13=Y 14=G 15=W 16=W
17=R 18=Y 19=R 20=G 21=R 22=B 23=O 24=G
25=G 26=W 27=W 28=B 29=Y 30=G 31=B 32=W
33=B 34=B 35=R 36=R 37=O 38=O 39=B 40=W
41=O 42=Y 43=G 44=O 45=G 46=Y 47=Y 48=R


example
    U      
BBURUDBFUFFFRRFUUFLULUFUDLRRDBBDBDBLUDDFLLRRBRLLLBRDDF - example
RRBRUFDLFFFLULULDLBUUULLUFDBRBUBFFRRUBDRFLFBDRDBLDRDDB

RUBRLBLDBULUDRFDFUBDRFLFFULBURFLDFDRDLBFRURDBLUB
#dduuLbrUbbffllDlbblldllbbddlldddllbblluullddll



"""

#colorCenters = [
#    [ np.array( [  95, 217, 237.] ), "Y" ],
#    [ np.array( [ 239, 239, 239.] ), "W" ],
#    [ np.array( [ 142, 162, 0.  ] ), "G" ],
#    [ np.array( [ 151,  75, 9.  ] ), "B" ],
#    [ np.array( [   0,   9, 255.] ), "R" ],
#    [ np.array( [   0, 169, 254.] ), "O" ],
#]

colorCenters = (
    ( np.array([   0.        ,  133.36772884,  217.34315932]),           "O" ),
    ( np.array([ 147.22431864,  147.22431864,  147.22431864]),           "W" ),
    ( np.array([  53.36270246,  176.31991094,  176.31991094]),           "Y" ),
    ( np.array([  2.38390351e+02,   9.05263999e+01,   1.07089590e-01]),  "B" ),
    ( np.array([ 160.08046326,  198.4924313 ,    0.        ]),           "G" ),
    ( np.array([   0.,    0.,  255.]),                                   "R" ),
)


def guessColor( v ):
    bestCode = None
    bestDist = None
    for v2, code in colorCenters:
        d = np.linalg.norm( v - v2 )
        if bestDist == None or d < bestDist:
            bestDist = d
            bestCode = code
    if bestDist != None and bestDist < 120.0:
        return bestCode
    return "X"


rects = [
    # 0 3
    # 1 4
    # 2 5
    #[ 250, 115, 20, 20 ],
    #[ 250, 220, 20, 20 ],
    #[ 245, 335, 20, 20 ],
    #[ 330, 115, 20, 20 ],
    #[ 330, 220, 20, 20 ],
    #[ 325, 335, 20, 20 ],


    [ 240, 105, 40, 40 ],
    [ 240, 210, 40, 40 ],
    [ 235, 325, 40, 40 ],
    [ 320, 105, 40, 40 ],
    [ 320, 210, 40, 40 ],
    [ 315, 325, 40, 40 ],
]

ids = []

runScanner = False
runSolver = False
cubeState = None
scrambleCnt = 0
engage = False
disengage = False
cmdSpeed = 1
colorSamples = None

statusText = "testing1\ntesting2"

def watcherThread():
    global ids
    global colorSamples
    global runScanner
    global runSolver
    global cubeState
    global scrambleCnt
    global engage
    global disengage
    global cmdSpeed
    global capDevNum
    global statusText
    vidcap = cv2.VideoCapture(capDevNum)

    vidcap.set(cv2.cv.CV_CAP_PROP_BRIGHTNESS,       0.50 )#0.50 default
    vidcap.set(cv2.cv.CV_CAP_PROP_CONTRAST,         0.15 )#0.15 default
    vidcap.set(cv2.cv.CV_CAP_PROP_SATURATION,       0.15 )#0.15 default
    vidcap.set(cv2.cv.CV_CAP_PROP_GAIN,             0.15 )#0.15 default
    #vidcap.set(cv2.cv.CV_CAP_PROP_FPS,            60.0  ) #no change
    #vidcap.set(cv2.cv.CV_CAP_PROP_HUE,              0.50 )#not supported
    #vidcap.set(cv2.cv.CV_CAP_PROP_EXPOSURE,         0.1 )#not supported
    while True:
        ret, frame = vidcap.read()
        if frame == None:
            print "no frame, skipping"
            continue

        colorSamples = []
        for rect in rects:
            x1, y1, w, h = rect
            x2, y2 = x1 + w, y1 + h

            block = frame[y1:y2,x1:x2]
            block_b = block[:,:, 0 ]
            block_g = block[:,:, 1 ]
            block_r = block[:,:, 2 ]
            #print block_r
            clr = np.array( ( np.median( block_b ), np.median( block_g ), np.median( block_r ) ) )
            clr /= np.linalg.norm( clr ) / 255.0

            colorSamples.append( clr )
            #print clr

            #clr = (255,0,0)
            code = guessColor(clr)

            cv2.rectangle(frame,  ( x1-1, y1-1 ), ( x2+1, y2+1 ), clr, thickness=-1 )
            cv2.rectangle(frame,  ( x1-1, y1-1 ), ( x2+1, y2+1 ), (128,128,128) )
            #cv2.putText( frame, code, (x1+2,y2-2), cv2.FONT_HERSHEY_PLAIN, 1.0, (0,0,0) )
            tclr = (255,255,255)
            if code in "YW":
                tclr = (0,0,0)
            cv2.putText( frame, code, (x1+3,y2-3), cv2.FONT_HERSHEY_PLAIN, 1.0, tclr )


            cv2.rectangle(frame,  ( 400, 100 ), ( 400+255/3, 100+255/3 ), (255,255,255) )
            cv2.rectangle(frame,  ( 400, 300 ), ( 400+255/3, 300+255/3 ), (255,255,255) )

            x = 400+int(clr[0]/3.)
            y = 100+int(clr[1]/3.)
            w = 5
            cv2.rectangle(frame,  ( x, y ), ( x + w, y + w ), clr, thickness=-1 )

            x = 400+int(clr[0]/3.)
            y = 300+int(clr[2]/3.)
            w = 5
            cv2.rectangle(frame,  ( x, y ), ( x + w, y + w ), clr, thickness=-1 )
        #print colorSamples
        ids = map( guessColor, colorSamples )
        #print "thread:", ids

        y = 15
        for line in statusText.split('\n'):
            cv2.putText( frame, line, ( 5, y ), cv2.FONT_HERSHEY_PLAIN, 1.0, (255,255,255) )
            y += 15

        cv2.imshow('Video', frame)

        k = cv2.waitKey(1)
        #if k >= 0:
        #    print k & 0xff
        if k == 1113938:#up
            rects[0][1] -= 5
            print rects
        if k == 1113940:#down
            rects[0][1] += 5
            print rects
        if k == 1113937:#left
            rects[0][0] -= 5
            print rects
        if k == 1113939:#right
            rects[0][0] += 5
            print rects
        #if k & 0xff == ord('i'):
        #    for i, s in enumerate( colorSamples ):
        #        print i, s
        #        guessColor( s )
        if k & 0xFF == ord('1'): cmdSpeed = 1
        if k & 0xFF == ord('2'): cmdSpeed = 2
        if k & 0xFF == ord('3'): cmdSpeed = 3
        if k & 0xFF == ord('4'): cmdSpeed = 4
        if k & 0xFF == ord('5'): cmdSpeed = 5
        if k & 0xFF == ord('6'): cmdSpeed = 6
        if k & 0xFF == ord('7'): cmdSpeed = 7
        if k & 0xFF == ord('8'): cmdSpeed = 8
        if k & 0xFF == ord('9'): cmdSpeed = 9

        if k & 0xFF == ord('p'): engage = True
        if k & 0xFF == ord('O'): engage = True
        if k & 0xFF == ord('o'): disengage = True
        if k & 0xFF == ord('q'):
            break
        if k & 0xFF == ord('r'): scrambleCnt = 50
        if k & 0xFF == ord('t'): scrambleCnt = 5
        if k & 0xFF == ord('s'): runScanner = True
        if k & 0xFF == ord('g'): runSolver = True

    # When everything is done, release the capture
    print "Watcher thread exiting"
    vidcap.release()
    cv2.destroyAllWindows()

class CubeMover:
    def __init__( self ):
        self.port = serial.Serial(serialDev, baudrate=115200, timeout=0.0)
        time.sleep( 2 )#wait for arduino to wake up
        self.port.write( "o" )#disengage
        #self.port.write( "2" )#slower
    def __del__( self ):
        self.port.write( "o" )#disengage
        self.port.write( chr(27) )#shutdown
        #time.sleep( 2 )
    def drain( self ):
        start = time.time()
        global statusText
        statusText = ""
        while time.time() - start < 0.5:
            statusText += self.port.read(1)

    def engage( self, b ):
        if b:
            self.port.write( "O" )
        else:
            self.port.write( "o" )
        self.drain()

    def speed( self, spd ):
        spd = str(spd)
        if spd in "123456789":
            self.port.write( spd )
        self.drain()
    def cmd( self, cmd ):
        if cmd == "": return
        self.port.write(cmd)
        for i in range(len(cmd)):
            while True:
                c = self.port.read(1)
                if c == ".":
                    break

def isLocked():
    if 'X' in ids:
        return False
    if len(set(ids)) == 1: #all blanks
        return False
    return True

def main():
    global ids
    global colorSamples
    global runScanner
    global runSolver
    global cubeState
    global scrambleCnt
    global engage
    global disengage
    global cmdSpeed
    cm = CubeMover()
    t = threading.Thread(target=watcherThread)
    t.start()

    speed = None

    scannerMoves = [
        ( ( 11, 12, 13, 17, 24, 23 ), "R"    , 0.50 ), #0
        ( (  3,  4,  5, 19, 18, 17 ), "R"    , 0.50 ), #1
        ( ( 31, 32, 25, 21, 20, 19 ), "R"    , 0.50 ), #2
        ( ( 45, 46, 47, 23, 22, 21 ), "R"    , 0.50 ), #3     HOME
        ( ( 11, 12, 13, 17, 24, 23 ), "f"    , 0.50 ), #4
        ( (  9, 10, 11,  7,  6,  5 ), "f"    , 0.50 ), #5
        ( ( 15, 16,  9, 37, 36, 35 ), "f"    , 0.50 ), #6
        ( ( 13, 14, 15, 45, 44, 43 ), "f"    , 0.50 ), #7     HOME
        ( ( 11, 12, 13, 17, 24, 23 ), "uudd" , 0.50 ), #8     FLIP
        ( ( 27, 12, 29, 33, 24, 39 ), "R"    , 0.50 ), #9
        ( (  7,  8,  1, 35, 34, 33 ), "rr"   , 0.50 ), #10
        ( ( 41, 42, 43, 39, 38, 37 ), "R"    , 0.50 ), #11    FLIP
        ( ( 27, 12, 29, 33, 24, 39 ), "f"    , 0.50 ), #12
        ( ( 25, 26, 27,  3,  2,  1 ), "ff"   , 0.50 ), #13
        ( ( 29, 30, 31, 41, 48, 47 ), "f"    , 0.50 ), #14    FLIP
        ( ( 27, 12, 29, 33, 24, 39 ), "uudd" , 0.50 ), #15    HOME
        ( ( 11, 12, 13, 17, 24, 23 ), "llff" , 0.50 ), #16
        ( ( 27, 28, 29, 33, 40, 39 ), "ffll" , 0.50 ), #17    HOME
    ]

    jay2stdMap = {
         1 : 30,  2 :  4,  3 : 27,  4 :  2,  5 : 24,  6 :  0,  7 : 33,  8 :  6,
         9 : 35, 10 :  1, 11 : 25, 12 : 16, 13 : 38, 14 :  9, 15 : 40, 16 : 18,
        17 : 26, 18 :  3, 19 : 28, 20 : 21, 21 : 47, 22 : 11, 23 : 37, 24 : 17,
        25 : 29, 26 :  5, 27 : 31, 28 : 22, 29 : 44, 30 : 13, 31 : 46, 32 : 20,
        33 : 32, 34 :  7, 35 : 34, 36 : 19, 37 : 41, 38 : 15, 39 : 43, 40 : 23,
        41 : 42, 42 : 14, 43 : 39, 44 :  8, 45 : 36, 46 : 10, 47 : 45, 48 : 12,
    }

    U1=0;  U2=1;  U3=2;  U4=3;  U5=4;  U6=5;  U7=6;  U8=7;  U9=8;
    R1=9;  R2=10; R3=11; R4=12; R5=13; R6=14; R7=15; R8=16; R9=17;
    F1=18; F2=19; F3=20; F4=21; F5=22; F6=23; F7=24; F8=25; F9=26;
    D1=27; D2=28; D3=29; D4=30; D5=31; D6=32; D7=33; D8=34; D9=35;
    L1=36; L2=37; L3=38; L4=39; L5=40; L6=41; L7=42; L8=43; L9=44;
    B1=45; B2=46; B3=47; B4=48; B5=49; B6=50; B7=51; B8=52; B9=53;

    jay2KociembaMap = {
         1  : U1,  2  : U2,  3  : U3,  8  : U4, "U" : U5,  4  : U6,  7  : U7,  6  : U8,  5  : U9,
        17  : R1, 18  : R2, 19  : R3, 24  : R4, "R" : R5, 20  : R6, 23  : R7, 22  : R8, 21  : R9,
         9  : F1, 10  : F2, 11  : F3, 16  : F4, "F" : F5, 12  : F6, 15  : F7, 14  : F8, 13  : F9,
        43  : D1, 44  : D2, 45  : D3, 42  : D4, "D" : D5, 46  : D6, 41  : D7, 48  : D8, 47  : D9,
        33  : L1, 34  : L2, 35  : L3, 40  : L4, "L" : L5, 36  : L6, 39  : L7, 38  : L8, 37  : L9,
        25  : B1, 26  : B2, 27  : B3, 32  : B4, "B" : B5, 28  : B6, 31  : B7, 30  : B8, 29  : B9,
    }

    colorMap = [ "X" ] * 48
    colorSampleMap = [ (0,0,0) ] * 48

    directionMap = {
        "W" : "U",
        "Y" : "D",
        "B" : "F",
        "G" : "B",
        "R" : "L",
        "O" : "R",
    }

    runCnt = 0
    cm.engage(True)
    prevRunScanner = False
    while t.isAlive():
        if speed != cmdSpeed:
            speed = cmdSpeed
            print "Going to speed %s" % speed
            cm.speed( speed )
        time.sleep(0.0)
        if engage:
            cm.engage(True)
            engage = False
        if disengage:
            cm.engage(False)
            disengage = False
        if scrambleCnt >0:
            random.seed( time.time() )
            for i in range( scrambleCnt ):
                cm.cmd( random.choice( "udlrfbUDLRFB" ) )
            scrambleCnt = 0
            cubeState = None
        if runScanner and not prevRunScanner:#edge detect
            scanStartTime = time.time()
            colorMap = [ "X" ] * 48
            colorSampleMap = [ (0,0,0) ] * 48
        prevRunScanner = runScanner
        if runScanner:
            if isLocked():
                assignments, cmd, delay = scannerMoves[runCnt]
                for i, idx in enumerate( assignments ):
                    #print i, idx
                    #print "[%d] %d<=%s" % ( runCnt, idx, ids[i] )
                    if colorMap[ idx - 1 ] != "X" and colorMap[ idx - 1 ] != ids[ i ]:
                        print "ERROR!! move %d, idx %d going from %s to %s" % ( runCnt, idx, colorMap[ idx - 1 ], ids[ i ] )
                    colorMap[ idx - 1 ] = ids[ i ]
                    colorSampleMap[ idx - 1 ] = colorSamples[ i ]
                cm.cmd( cmd )
                time.sleep(delay)
                runCnt += 1
                if runCnt >= len(scannerMoves):
                    runCnt = 0
                    runScanner = False
                    #colorMap = map( jay2stdMap.get, colorMap )
                    stdMap = {}
                    cubeState = ["X"]*54
                    for i, clr in enumerate( colorMap ):
                        idx = i + 1
                        stdMap[ jay2stdMap[ idx ] ] = clr
                        print "%d=%s" % ( idx, clr ),
                        #print "m[%d]=%d <= %s" % ( idx, jay2KociembaMap[ idx ], directionMap[clr] )
                        cubeState[ jay2KociembaMap[ idx ] ] = directionMap[clr]
                    print
                    print "colorSampleMap:", colorSampleMap
                    cubeState[ U5 ] = "U"
                    cubeState[ L5 ] = "L"
                    cubeState[ F5 ] = "F"
                    cubeState[ R5 ] = "R"
                    cubeState[ B5 ] = "B"
                    cubeState[ D5 ] = "D"
                    cubeState = ''.join(cubeState)
                    print "cubeState:", cubeState
                    now = time.time()
                    scanTime = now - scanStartTime
                    print "Scan Time: %.3f" % scanTime

        if runSolver and cubeState != None:
            solveStartTime = time.time()
            runSolver = False
            cmds = [ './solve', cubeState ]
            print "Cmdline:", " ".join(cmds)
            proc = subprocess.Popen(cmds, stdout=subprocess.PIPE)
            output = proc.communicate()[0]
            solveMoves = ""
            if not "Unsolvable" in output:
                movesMap = { 
                    "U"  : "u",  "D"  : "d",  "L"  : "l",  "R"  : "r",  "F"  : "f",  "B"  : "b",
                    "U'" : "U",  "D'" : "D",  "L'" : "L",  "R'" : "R",  "F'" : "F",  "B'" : "B",
                    "U2" : ";",  "D2" : "m",  "L2" : "j",  "R2" : "k",  "F2" : "g",  "B2" : "h",
                }
                for o in output.split():
                    solveMoves += movesMap[ o ]
            solutionFoundTime = time.time()
            cm.cmd( solveMoves )
            now = time.time()
            thinkTime = solutionFoundTime - solveStartTime
            movesTime = now - solutionFoundTime
            moveCnt = len(solveMoves)
            solveTime = now - solveStartTime
            print "Output:", output
            if moveCnt > 0:
                print "Moves:", solveMoves
                print "Think Time: %.3f" % thinkTime
                print "Moves Time: %.3f (%d moves, %dms per move)" % ( movesTime, moveCnt, movesTime * 1e3 / moveCnt )
                print "Solve Time: %.3f" % solveTime
if __name__ == "__main__":
    main()












