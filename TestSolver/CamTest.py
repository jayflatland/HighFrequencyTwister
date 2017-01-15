#!/usr/bin/env python

import cv2
import numpy as np
import os.path

def devIdFromPath( path ):
    realpath = os.path.realpath( path )
    if not "/dev/video" in realpath:
        raise "Not a video device"
    devnum = int(realpath[-1])
    return devnum


vidcaps = [ 
   cv2.VideoCapture( devIdFromPath( '/dev/v4l/by-id/usb-046d_081b_B6A2CCC0-video-index0') ),
   cv2.VideoCapture( devIdFromPath( '/dev/v4l/by-id/usb-046d_081b_866188E0-video-index0') ),
   cv2.VideoCapture( devIdFromPath( '/dev/v4l/by-id/usb-046d_081b_20648CC0-video-index0') ),
   cv2.VideoCapture( devIdFromPath( '/dev/v4l/by-id/usb-046d_081b_B9548CC0-video-index0') ),
]

for vidcap in vidcaps:
    vidcap.set(3,160)
    vidcap.set(4,120)
    #vidcap.set(21, 0 )
    #for e in dir(cv2.cv
    #vidcap.set(cv2.cv.CV_CAP_PROP_BRIGHTNESS,       0.50 )#0.50 default
    #vidcap.set(cv2.cv.CV_CAP_PROP_CONTRAST,         0.15 )#0.15 default
    #vidcap.set(cv2.cv.CV_CAP_PROP_SATURATION,       0.15 )#0.15 default
    #vidcap.set(cv2.cv.CV_CAP_PROP_GAIN,             0.15 )#0.15 default
    #vidcap.set(cv2.cv.CV_CAP_PROP_EXPOSURE, 0.5 ) #not supported by C310
    #vidcap.set(21, 0.0)#CV_CAP_PROP_AUTO_EXPOSURE #not supported by C310

baseline = cv2.imread('baseline.jpg')
accumulate = False
while True:
    k = chr( cv2.waitKey(1) & 0xFF )
    if k == 'q':    break

    rebaseline = False
    if k == 'r':
        rebaseline = True
        accumulate = True
        print "Rebaselining and starting accumulation"
    if k == 'a':
        accumulate = not accumulate
        if accumulate:  print "Starting accumulation"
        else:           print "Stopping accumulation"

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

    if rebaseline:
        print "Rebaselining!"
        baseline = camFrame
        cv2.imwrite( 'baseline.jpg', baseline )
    elif accumulate:
        baseline = cv2.max( baseline, camFrame )
        cv2.imwrite( 'baseline.jpg', baseline )

    deltaFrame = cv2.addWeighted( camFrame, 1.0, baseline, -1.0, 192.0 )

    showFrame = np.vstack( ( camFrame, baseline, deltaFrame ) )
    showFrame = cv2.resize( showFrame, (0,0), fx=2.0, fy=2.0 )
    cv2.imshow('CamTest', showFrame)

