// 
// capv4l2.h - Paul Rose - Nov 2015
// 
// simple wrapper for v4l2 memory mapped webcam capture
//
// hard coded to capture YUYV8888 and convert to RGBA8888
//
#pragma once
#include <vector>

#include "util.h"

class CapV4L2{
public:
    CapV4L2(std::string const &dev, int xres, int yres, int numBuffers=2);
    ~CapV4L2();

    std::string const &dev() const { return dev_; }

    struct ControlRange{
        ControlRange() : minimum(0), maximum(0), step(0), defaultVal(0) {}
        int minimum, maximum, step, defaultVal;
    };
    enum Control{
        Sharpness=0,
        Brightness,     // black level
        Contrast,       // luma gain
        Saturation,     // chroma gain
        Hue,            // color balance
        Gamma,
        Gain,
        Exposure,
        BacklightCompensation,  // 0=No, 1=Yes
        AutoWhiteBalance,       // 0=No, 1=Yes
        WhiteBalanceTemperature,// only when AutoWhiteBalance=No
        ExposureAuto,           // 1=Manual, 3=Aperture Priority
        ExposureAbsolute,       // only when ExposureAuto=Manual
        
        NumControls
    };
    char const *controlName(Control c);
    ControlRange getControlRange(Control c)const; // will throw std::runtime_error if not supported by device
    int getControlValue(Control c)const;          // will throw std::runtime_error if not supported by device
    void setControlValue(Control c, int value);   // will throw std::runtime_error if not supported by device
    
    void captureOn();
    void captureOff();

    int frames() const { return capturedFrames_; }
    double elapsed() const { return (captureLastTime_ - captureStartTime_) / 1000000.0; }
    double fps() const { return capturedFrames_ ? capturedFrames_/elapsed() : 0.0; }

    double lastFPS() const {
        if( lastFrameTimeDelta_ == 0 ) { return 0.0; }
        return 1.0e6 / lastFrameTimeDelta_;
    }
    
    // these may be different than requested by ctor if requested resolution is not supported by device
    int xres() const { return xres_; }
    int yres() const { return yres_; }

    // capture next frame, available afterward as rgbaFrame()
    void nextFrameBlocking();
    // for select/poll/epoll prior to nextFrameBlocking()
    int fd() { return fd_; }
    
    // rgbaFrameSize() bytes 
    // 4 bytes per pixel: R, G, B, A
    // arranged as yres() rows of xres() pixels each
    unsigned char const *rgbaFrame() const { return rgba_.get(); }
    unsigned char *rgbaFrame() { return rgba_.get(); }
    size_t rgbaFrameSize() const { return rgbaSize_; }
    void getRGBA(int x, int y, unsigned char rgba[4])const{
        ::memcpy(rgba, rgbaFrame()+(y*xres_+x)*4, 4);
    }
   
private:
    void clearRGBAFrame();
private:
    std::string const dev_;
    FD          const fd_;

    int xres_;
    int yres_;
    int stride_;
    int numBuffers_;

    typedef std::vector<MMappedBuffer> Buffers;
    Buffers buffers_;

    bool          captureOn_;
    int           capturedFrames_;
    unsigned long captureStartTime_;
    unsigned long captureLastTime_;

    unsigned long lastCaptureTime_;
    long lastFrameTimeDelta_;

    size_t rgbaSize_;
    std::unique_ptr< unsigned char [] > rgba_;

};
