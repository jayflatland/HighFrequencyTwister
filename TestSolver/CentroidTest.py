#!/usr/bin/env python

import cv2
import sys
import numpy as np


vidcap = cv2.VideoCapture(1)
while True:
    ret, frame = vidcap.read()
    if frame == None:
        print "no frame, skipping"
        continue

    gray = cv2.cvtColor(frame, cv2.cv.CV_RGB2GRAY)
    #ret,gray = cv2.threshold(gray,128,255,0)
    gray = cv2.adaptiveThreshold(gray,255,cv2.ADAPTIVE_THRESH_GAUSSIAN_C,cv2.THRESH_BINARY,63,2)
    cv2.imshow('Gray Tresh', gray)
    contours,hierarchy = cv2.findContours(gray, cv2.cv.CV_RETR_LIST, cv2.cv.CV_CHAIN_APPROX_SIMPLE)
    r = []
    for i, c in enumerate(contours):
        c = cv2.convexHull(c)
        a = cv2.contourArea( c )
        p = cv2.arcLength( c, True )
        squareSideLength = a**0.5
        squarePerimeter = 4.0 * squareSideLength
        if a <  1000.0: continue
        if a > 10000.0: continue
        err = p - squarePerimeter
        err *= err
        if err > 1000.0: continue
        print i, a, p, squarePerimeter, err
        r.append( c )
        #print map( cv2.contourArea, contours )
    contours = r

    cv2.drawContours(frame,contours, -1, (255,255,255) )
    #
    #cnt = contours[0]
    #M = cv2.moments(cnt)
    #print M

    cv2.imshow('Video', frame)

    k = cv2.waitKey(1)
    if k & 0xFF == ord('q'):
        break
