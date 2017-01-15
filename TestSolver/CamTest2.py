#!/usr/bin/env python

import cv2
import numpy as np
import os.path
import serial

def devIdFromPath( path ):
    realpath = os.path.realpath( path )
    if not "/dev/video" in realpath:
        raise "Not a video device"
    devnum = int(realpath[-1])
    return devnum


vidcaps = [ 
   cv2.VideoCapture( devIdFromPath( '/dev/v4l/by-id/usb-046d_081b_20648CC0-video-index0') ),
   cv2.VideoCapture( devIdFromPath( '/dev/v4l/by-id/usb-046d_081b_B9548CC0-video-index0') ),
   cv2.VideoCapture( devIdFromPath( '/dev/v4l/by-id/usb-046d_081b_B6A2CCC0-video-index0') ),
   cv2.VideoCapture( devIdFromPath( '/dev/v4l/by-id/usb-046d_081b_866188E0-video-index0') ),
]

serialDev = "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AD02CTU2-if00-port0"



for vidcap in vidcaps:
    vidcap.set(3,160)
    vidcap.set(4,120)
    #vidcap.set(21, 0 )
    #for e in dir(cv2.cv
    #vidcap.set(cv2.cv.CV_CAP_PROP_BRIGHTNESS,       0.20 )#0.50 default
    #vidcap.set(cv2.cv.CV_CAP_PROP_CONTRAST,         0.15 )#0.15 default
    #vidcap.set(cv2.cv.CV_CAP_PROP_SATURATION,       0.10 )#0.15 default
    #vidcap.set(cv2.cv.CV_CAP_PROP_GAIN,             0.15 )#0.15 default
    #vidcap.set(cv2.cv.CV_CAP_PROP_EXPOSURE, 0.5 ) #not supported by C310
    #vidcap.set(21, 0.0)#CV_CAP_PROP_AUTO_EXPOSURE #not supported by C310

# * The names of the facelet positions of the cube
# *             |************|
# *             |*U1**U2**U3*|
# *             |************|
# *             |*U4**U5**U6*|
# *             |************|
# *             |*U7**U8**U9*|
# *             |************|
# * ************|************|************|************|
# * *L1**L2**L3*|*F1**F2**F3*|*R1**R2**F3*|*B1**B2**B3*|
# * ************|************|************|************|
# * *L4**L5**L6*|*F4**F5**F6*|*R4**R5**R6*|*B4**B5**B6*|
# * ************|************|************|************|
# * *L7**L8**L9*|*F7**F8**F9*|*R7**R8**R9*|*B7**B8**B9*|
# * ************|************|************|************|
# *             |************|
# *             |*D1**D2**D3*|
# *             |************|
# *             |*D4**D5**D6*|
# *             |************|
# *             |*D7**D8**D9*|
# *             |************|
# * </pre>


sampleCoords = [(403, 40), (391, 32), (379, 26), (419, 32), (405, 26), (85, 42), (45, 34), (55, 41), (68, 47), (76, 59), (90, 55), (232, 39), (77, 76), (89, 69), (233, 49), (76, 91), (88, 82), (233, 63), (38, 47), (49, 52), (62, 60), (552, 40), (50, 68), (63, 75), (552, 51), (564, 59), (62, 90), (545, 62), (558, 71), (218, 83), (533, 69), (546, 77), (226, 77), (523, 76), (249, 78), (236, 72), (412, 51), (425, 45), (541, 26), (411, 66), (423, 60), (540, 38), (411, 81), (527, 58), (538, 54), (244, 37), (383, 43), (395, 50), (243, 50), (385, 58), (397, 67), (243, 62), (385, 71), (397, 81)]


names = [ "U1", "U2", "U3", "U4", "U5", "U6", "U7", "U8", "U9", "R1", "R2", "R3", "R4", "R5", "R6", "R7", "R8", "R9", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "D1", "D2", "D3", "D4", "D5", "D6", "D7", "D8", "D9", "L1", "L2", "L3", "L4", "L5", "L6", "L7", "L8", "L9", "B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8", "B9" ]
def draw_circle(event,x,y,flags,param):
    if event == cv2.EVENT_LBUTTONUP:
        if len( sampleCoords ) < len( names ):
            sampleCoords.append( ( x/2, y/2 ) )#scale down
            print sampleCoords
            print "Next %s" % names[ len( sampleCoords ) ]
cv2.namedWindow('CamTest')

cv2.setMouseCallback('CamTest',draw_circle)

colorCenters = (
    ( np.array([ 147.,  147.,  147.]),  30., "W" ),
    ( np.array([ 231.,   95.,   30.]),  60., "B" ),
    ( np.array([   0.,    0.,  255.]), 110., "R" ),
    ( np.array([   0.,  133.,  217.]),  30., "O" ),
    ( np.array([  53.,  176.,  176.]),  30., "Y" ),
    ( np.array([ 160.,  198. ,   0.]),  30., "G" ),
)

codeToColor = {
    "O" : (   0, 128, 255 ),
    "W" : ( 255, 255, 255 ),
    "Y" : (   0, 255, 255 ),
    "B" : ( 255,   0,   0 ),
    "G" : (   0, 255,   0 ),
    "R" : (   0,   0, 255 ),
    "X" : (   0,   0,   0 ),
}

def guessColor( v ):
    for v2, radius, code in colorCenters:
        d = np.linalg.norm( v - v2 )
        if d < radius:
            return code
    return "X"

class CubeMover:
    def __init__( self ):
        self.port = serial.Serial(serialDev, baudrate=115200, timeout=0.0)
        print "Waiting for power on..."
        #while True:
        #    c = self.port.read(1)
        #    if c == '+':#powered up
        #        break
        print "Powered on!"
    def __del__( self ):
        self.port.write( chr(27) )#shutdown

    def cmd( self, cmd ):
        self.port.write(cmd)

cm = CubeMover()

while True:
    k = chr( cv2.waitKey(1) & 0xFF )
    if k == 'q':    break

    if k == '1':    cm.cmd( '1' )
    if k == '2':    cm.cmd( '2' )
    if k == '3':    cm.cmd( '3' )
    if k == '4':    cm.cmd( '4' )
    if k == '[':    cm.cmd( '[' )
    if k == ']':    cm.cmd( ']' )
    if k == 'F':    cm.cmd( 'a' )
    if k == 'f':    cm.cmd( 'c' )
    if k == 'B':    cm.cmd( 'g' )
    if k == 'b':    cm.cmd( 'o' )
    if k == 'L':    cm.cmd( 'p' )
    if k == 'l':    cm.cmd( 'r' )
    if k == 'R':    cm.cmd( 'v' )
    if k == 'r':    cm.cmd( 'D' )
    if k == 'U':    cm.cmd( 'E' )
    if k == 'u':    cm.cmd( 'G' )
    if k == 'D':    cm.cmd( 'K' )
    if k == 'd':    cm.cmd( 'S' )
    if k == 'z':    sampleCoords = []

    camFrames = []
    for i, vidcap in enumerate( vidcaps ):
        ret, frame = vidcap.read()
        if frame != None:
            camFrames.append( frame )

    if len( camFrames ) == 0:
        print "no frames, skipping"
        continue

    #print "%d frames" % len( camFrames )
    camFrame = np.hstack( camFrames )
    #grayFrame = cv2.cvtColor(camFrame, cv2.COLOR_BGR2GRAY)

    #colors = []
    #for y, x in sampleCoords:
    #    colors.append( camFrame[ x, y ] )
    #print colors

    thinkFrame = camFrame.copy()
    #thinkFrame = cv2.cvtColor(thinkFrame,cv2.COLOR_BGR2HSV)
    #thinkFrame[...,2] -= 127.0
    #thinkFrame[...,2] *= 2.0
    #thinkFrame[...,2] += 127.0
    #thinkFrame[...,2] = cv2.max( thinkFrame[...,2],   0. )
    #thinkFrame[...,2] = cv2.min( thinkFrame[...,2], 255. )
    #thinkFrame = cv2.cvtColor(thinkFrame,cv2.COLOR_HSV2BGR)
    graphFrame = np.zeros( thinkFrame.shape, 'uint8' )
    skipPoints = range(4,54,9)

    for i, coord in enumerate( sampleCoords ):
        if i in skipPoints:
            continue
        x, y = coord

        x1, y1 = x - 2, y - 2
        x2, y2 = x + 2, y + 2

        block = camFrame[y1:y2,x1:x2]
        block_r = block[:,:, 0 ]
        block_g = block[:,:, 1 ]
        block_b = block[:,:, 2 ]
        clr = np.array( ( np.median( block_r ), np.median( block_g ), np.median( block_b ) ) )
        #clr /= np.linalg.norm( clr ) / 255.0

        #code = guessColor(clr)
        #guessClr = codeToColor[code]
        w = 2
        cv2.rectangle(thinkFrame,  ( x1, y1 ), ( x2, y2 ), clr, thickness=-1 )

        x = 0+int(clr[0]/3.)
        y = 0+int(clr[1]/3.)
        w = 1
        cv2.rectangle(graphFrame,  ( x, y ), ( x + w, y + w ), clr, thickness=-1 )

        x = 200+int(clr[0]/3.)
        y = 0+int(clr[2]/3.)
        w = 1
        cv2.rectangle(graphFrame,  ( x, y ), ( x + w, y + w ), clr, thickness=-1 )


    showFrame = np.vstack( ( camFrame, thinkFrame, graphFrame ) )


    showFrame = cv2.resize( showFrame, (0,0), fx=2.0, fy=2.0 )
    cv2.imshow('CamTest', showFrame)

