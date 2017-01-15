// 
// capv4l2.h - Paul Rose - Nov 2015
// 
// simple wrapper for v4l2 memory mapped webcam capture
//
// hard coded to capture YUYV8888 and convert to RGBA8888
//
#include <fcntl.h>

#include <linux/videodev2.h>

#include "capv4l2.h"
#include "util.h"

static inline unsigned char clip(int color) { return (color > 0xFF) ? 0xFF : ((color < 0) ? 0 : color); }

// src:
//   YUYV = 4 bytes for 2 pixels: pix1.Y, shared.U, pix2.Y, shared.V
// dst:
//   RGBA = 4 bytes per pixel: pix1.R, pix1.G, pix1.B, pix1.A
// 
// bit-math taken from:
//      libv4l2 (in v4l-utils)
//      (C) 2008 Hans de Goede <hdegoede@redhat.com>
//      Released under LGPL

static void yuyv_to_rgba(const unsigned char *yuyv, unsigned char *rgba, int xres, int yres, int bytes_per_line){
    int const extra_bytes_per_row = bytes_per_line - 2*xres;
    
    while (--yres >= 0) {
        for (int j = 0; j < xres; j += 2) {
            int const y0 = *yuyv++;
            int const u  = *yuyv++;
            int const y1 = *yuyv++;
            int const v  = *yuyv++;
            
            int const u1 = (((u - 128) << 7) +  (u - 128)) >> 6;
            int const rg = (((u - 128) << 1) +  (u - 128) + ((v - 128) << 2) + ((v - 128) << 1)) >> 3;
            int const v1 = (((v - 128) << 1) +  (v - 128)) >> 1;

            // RGBA 0
            *rgba++ = clip(y0 + v1);
            *rgba++ = clip(y0 - rg);
            *rgba++ = clip(y0 + u1);
            *rgba++ = 255;
            
            // RGBA 1 
            *rgba++ = clip(y1 + v1);
            *rgba++ = clip(y1 - rg);
            *rgba++ = clip(y1 + u1);
            *rgba++ = 255;
        }
        yuyv += extra_bytes_per_row;
    }
}



CapV4L2::CapV4L2(std::string const &dev, int xres, int yres, int numBuffers)
    : dev_(dev)
    , fd_( open( dev_.c_str(), O_RDWR ) )
    , xres_(xres)
    , yres_(yres)
    , stride_(0)
    , numBuffers_(numBuffers)
    , captureOn_(false)
    , capturedFrames_(0)
    , captureStartTime_(0)
    , captureLastTime_(0)
    , lastCaptureTime_(0)
    , lastFrameTimeDelta_(0)
    , rgbaSize_(0)
{
    if( fd_ < 0 ) throwErrno(dev_ + ": open"); 
    
    // check capabilities
    v4l2_capability cap;
    if(ioctl_retry(fd_, VIDIOC_QUERYCAP, &cap) < 0){
        throwErrno(dev_ + ": ioctl VIDIOC_QUERYCAP");
    }    
    if(!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)){
        throw std::runtime_error(dev_ + ": device does not handle single-planar video capture [V4L2_CAP_VIDEO_CAPTURE]");
    } 
    if(!(cap.capabilities & V4L2_CAP_STREAMING)){
        throw std::runtime_error(dev_ + ": device does not handle frame streaming [V4L2_CAP_STREAMING]");
    }
    
    // format YUYV 4:2:2    
    v4l2_format format;
    memset(&format, 0, sizeof(format));
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    format.fmt.pix.width  = xres_;
    format.fmt.pix.height = yres_;
    format.fmt.pix.field  = V4L2_FIELD_INTERLACED;
    if(ioctl_retry(fd_, VIDIOC_S_FMT, &format) < 0){
        throwErrno(dev_ + ": ioctl VIDIOC_S_FMT {V4L2_BUF_TYPE_VIDEO_CAPTURE,V4L2_PIX_FMT_YUYV,160,120}");
    }
    // Note VIDIOC_S_FMT may change width and height
    xres_   = format.fmt.pix.width;
    yres_   = format.fmt.pix.height;
    stride_ = format.fmt.pix.bytesperline;    
    
    // will be using numBuffers mmap buffers
    v4l2_requestbuffers bufrequest;
    memset(&bufrequest, 0, sizeof(bufrequest));
    bufrequest.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    bufrequest.memory = V4L2_MEMORY_MMAP;
    bufrequest.count = numBuffers_;
    if(ioctl_retry(fd_, VIDIOC_REQBUFS, &bufrequest) < 0){
        throwErrno(dev_ + ": ioctl VIDIOC_REQBUFS {V4L2_BUF_TYPE_VIDEO_CAPTURE,V4L2_MEMORY_MMAP,1}");
    }

    buffers_.reserve(numBuffers_);
    for(int idx=0; idx<numBuffers_; ++idx){
        v4l2_buffer bufferinfo;
        memset(&bufferinfo, 0, sizeof(bufferinfo));
         
        bufferinfo.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        bufferinfo.memory = V4L2_MEMORY_MMAP;
        bufferinfo.index = idx;
         
        if(ioctl_retry(fd_, VIDIOC_QUERYBUF, &bufferinfo) < 0){
            throwErrno(dev_ + ": VIDIOC_QUERYBUF {V4L2_BUF_TYPE_VIDEO_CAPTURE,V4L2_MEMORY_MMAP,idx}");
        }     
        
        void *buffer_start = mmap(
            nullptr,
            bufferinfo.length,
            PROT_READ | PROT_WRITE,
            MAP_SHARED,
            fd_,
            bufferinfo.m.offset
        );
        if(buffer_start == MAP_FAILED){
            throwErrno(dev_ + ": mmap");
        }
         
        memset(buffer_start, 0, bufferinfo.length);
        buffers_.push_back( MMappedBuffer( static_cast<unsigned char *>(buffer_start), bufferinfo.length ) ); 
    }
    
    rgbaSize_ = xres_ * yres_ * 4;
    rgba_.reset( new unsigned char [rgbaSize_] );

    clearRGBAFrame(); 
}

CapV4L2::~CapV4L2(){
printf("~CapV4L2()\n");    
    captureOff();
    // unmap will be handled by buffers_ dtor
    // close will be handled by fd_ dtor
}

char const *CapV4L2::controlName(Control c){
    switch(c){
    case Sharpness:                     return "Sharpness";
    case Brightness:                    return "Brightness";
    case Contrast:                      return "Contrast";
    case Saturation:                    return "Saturation";
    case Hue:                           return "Hue";
    case Gamma:                         return "Gamma";
    case Gain:                          return "Gain";
    case Exposure:                      return "Exposure";        
    case BacklightCompensation:         return "BacklightCompensation";        
    case AutoWhiteBalance:              return "AutoWhiteBalance";	
    case WhiteBalanceTemperature:       return "WhiteBalanceTemperature";	
    case ExposureAuto:                  return "ExposureAuto";	 // V4L2_EXPOSURE_MANUAL V4L2_EXPOSURE_APERTURE_PRIORITY
    case ExposureAbsolute:              return "ExposureAbsolute";	
    }
    return nullptr;
}
static int controlId(CapV4L2::Control c){
    switch(c){
    case CapV4L2::Sharpness:              return V4L2_CID_SHARPNESS;
    case CapV4L2::Brightness:             return V4L2_CID_BRIGHTNESS;
    case CapV4L2::Contrast:               return V4L2_CID_CONTRAST;
    case CapV4L2::Saturation:             return V4L2_CID_SATURATION;
    case CapV4L2::Hue:                    return V4L2_CID_HUE;
    case CapV4L2::Gamma:                  return V4L2_CID_GAMMA;
    case CapV4L2::Gain:                   return V4L2_CID_GAIN;
    case CapV4L2::Exposure:               return V4L2_CID_EXPOSURE;
    case CapV4L2::BacklightCompensation:  return V4L2_CID_BACKLIGHT_COMPENSATION;	
    case CapV4L2::AutoWhiteBalance:       return V4L2_CID_AUTO_WHITE_BALANCE;	
    case CapV4L2::WhiteBalanceTemperature:return V4L2_CID_WHITE_BALANCE_TEMPERATURE;	
    case CapV4L2::ExposureAuto:           return V4L2_CID_EXPOSURE_AUTO;	 // V4L2_EXPOSURE_MANUAL V4L2_EXPOSURE_APERTURE_PRIORITY
    case CapV4L2::ExposureAbsolute:       return V4L2_CID_EXPOSURE_ABSOLUTE;	
    }
    return -1;
}

CapV4L2::ControlRange CapV4L2::getControlRange(Control c)const{
    v4l2_queryctrl queryctrl;
    memset(&queryctrl, 0, sizeof(queryctrl));
    queryctrl.id = controlId(c);
    if(ioctl_retry(fd_, VIDIOC_QUERYCTRL, &queryctrl) < 0){
        throwErrno(dev_ + ": VIDIOC_QUERYCTRL");
    }
    ControlRange r;
    r.minimum     = queryctrl.minimum;
    r.maximum     = queryctrl.maximum;
    r.step        = queryctrl.step;
    r.defaultVal  = queryctrl.default_value;
    return r;
}
int CapV4L2::getControlValue(Control c)const{
    v4l2_control ctrl;
    memset(&ctrl, 0, sizeof(ctrl));
    ctrl.id = controlId(c);
    if(ioctl_retry(fd_,  VIDIOC_G_CTRL, &ctrl) < 0){
        throwErrno(dev_ + ": VIDIOC_G_CTRL");
    }
    return ctrl.value;
}
void CapV4L2::setControlValue(Control c, int value){
    v4l2_control ctrl;
    memset(&ctrl, 0, sizeof(ctrl));
    ctrl.id = controlId(c);
    ctrl.value = value;
    if(ioctl_retry(fd_,  VIDIOC_S_CTRL, &ctrl) < 0){
        throwErrno(dev_ + ": VIDIOC_S_CTRL");
    }
}



void CapV4L2::captureOn(){
    captureOff();
    
    //post buffers
    for(int idx=0; idx<numBuffers_; ++idx){
        v4l2_buffer bufferinfo;
         
        memset(&bufferinfo, 0, sizeof(bufferinfo));
        bufferinfo.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        bufferinfo.memory = V4L2_MEMORY_MMAP;
        bufferinfo.index = idx; 
        
        // Put the buffer in the incoming queue.
        if(ioctl_retry(fd_, VIDIOC_QBUF, &bufferinfo) < 0){
            throwErrno(dev_ + ": VIDIOC_QBUF {V4L2_BUF_TYPE_VIDEO_CAPTURE,V4L2_MEMORY_MMAP,idx}");
        }        
    }
    
    // Activate streaming
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(ioctl_retry(fd_, VIDIOC_STREAMON, &type) < 0){
        throwErrno(dev_ + ": VIDIOC_STREAMON {V4L2_BUF_TYPE_VIDEO_CAPTURE}");
    }
    
    captureOn_ = true;
    capturedFrames_ = 0;
    captureStartTime_ = 0;
    captureLastTime_ = 0;

    lastFrameTimeDelta_ = gtod();
}
void CapV4L2::captureOff(){
    if( captureOn_){
        // Deactivate streaming
        int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if(ioctl_retry(fd_, VIDIOC_STREAMOFF, &type) < 0){
            // ignore error on turn-off
        }
        captureOn_ = false;
        clearRGBAFrame();
    }
}

void CapV4L2::clearRGBAFrame(){
    // "clear" to a recognizable gradient pattern 
    unsigned char *p = rgba_.get();    
    for( int y=0; y<yres_; ++y){
        for( int x=0; x<xres_; ++x ){
            *p++ = (256*x)/xres_; // R
            *p++ = (256*y)/yres_; // G
            *p++ = 100;           // B
            *p++ = 255;           // A
        }
    }  
}

// capture next frame, available afterward as rgbaFrame()
void CapV4L2::nextFrameBlocking(){
    if(captureOn_){
        v4l2_buffer bufferinfo;

        // de-queue a buffer
        memset(&bufferinfo, 0, sizeof(bufferinfo));
        bufferinfo.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        bufferinfo.memory = V4L2_MEMORY_MMAP;
        if(ioctl_retry(fd_, VIDIOC_DQBUF, &bufferinfo) < 0){
            throwErrno(dev_ + ": VIDIOC_DQBUF");
        }
        int const index = bufferinfo.index;
        const unsigned long now = gtod();
        captureLastTime_ = now;
        if(captureStartTime_ == 0){ 
            captureStartTime_ = captureLastTime_;
            capturedFrames_ = 0;
        }
        else{
            ++capturedFrames_;
        }
        
        // process
        yuyv_to_rgba(buffers_[index].addr, rgba_.get(), xres_, yres_, stride_);

        // re-post
        memset(&bufferinfo, 0, sizeof(bufferinfo));
        bufferinfo.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        bufferinfo.memory = V4L2_MEMORY_MMAP;
        bufferinfo.index = index; 
        if(ioctl_retry(fd_, VIDIOC_QBUF, &bufferinfo) < 0){
            throwErrno(dev_ + ": VIDIOC_QBUF");
        }

        lastFrameTimeDelta_ = now - lastCaptureTime_;
        lastCaptureTime_ = now;
    }
}

//#define TEST_MAIN
#ifdef TEST_MAIN
int main(int argc, char *argv[]){
    try{
        char const *dev = "/dev/video0";
        if( argc > 1 ){
            dev = argv[1];
        }

        CapV4L2 cap( dev, 160, 120, 2 );
        
        cap.captureOn();
        
        while(cap.frames() < 100){
            cap.nextFrameBlocking();
        }   
        
        printf("%d frames in %.2f seconds (%.2f FPS)\n", cap.frames(), cap.elapsed(), cap.fps() ); 
    }
    catch(std::exception &ex){
        fprintf(stderr, "caught: %s\n", ex.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
#endif
