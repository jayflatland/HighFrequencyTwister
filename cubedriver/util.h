// 
// util.h - Paul Rose - Nov 2015
// 
// random utilities for 
//  error reporting 
//  RAII style resource cleanup
//
#pragma once
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <fstream>
#include <algorithm>
#include <memory>
#include <stdexcept>
#include <string>
#include <map>

#include <poll.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"

#include "gtod.h"

inline int ioctl_retry(int fd, unsigned long int req, void *arg){
    for(int i=0; i<10; ++i){
        int const ret = ::ioctl(fd, req, arg);
        if( ret != -1 || errno != EINTR) return ret;
    }
    return -1;
}
inline int poll_retry(struct pollfd *fds, nfds_t nfds, int timeout){
    for(int i=0; i<10; ++i){
        int const ret = ::poll(fds, nfds, timeout);
        if( ret != -1 || errno != EINTR) return ret;
    }
    return -1;
}

inline void throwErrno(std::string const &prefix){
    std::string const msg = prefix + ": " + strerror(errno);
    throw std::runtime_error(msg);
}
inline void throwSDL(std::string const &prefix){
    std::string const msg = prefix + ": " + SDL_GetError();
    throw std::runtime_error(msg);
}


inline void perrorExit(char const *s){
    perror(s);
    exit(EXIT_FAILURE);
}
inline void sdlExit(char const *s){
    fprintf(stderr, "%s: %s\n", s, SDL_GetError()); 
    exit(EXIT_FAILURE);
}

// holds/closes file handle
//  no copy ctor/assign
//  move ctor/assign OK 
class FD{
public:
    FD() : fd_(-1) {}
    
    FD(int fd) : fd_(fd) {}

    FD(FD &&rhs) : fd_(rhs.fd_) { 
        rhs.fd_=-1; 
    }

    FD &operator=(FD &&rhs) { 
        close(); 
        fd_=rhs.fd_; 
        rhs.fd_ = -1; 
        return *this;
    } 
        
    ~FD(){ 
        close(); 
    }
    
    operator int() const { return fd_; }
   
    void open(int fd){ close(); fd_ = fd; }

    void close(){ if( fd_ >= 0 ) ::close(fd_); }
private:    
    int fd_;
    FD(FD const &);
    FD &operator=(FD const &);
};

// holds/releases mmap region
//  no copy ctor/assign
//  move ctor/assign OK 
struct MMappedBuffer{
    unsigned char *addr;
    size_t         len;
    
    MMappedBuffer() : addr(nullptr), len(0) {}
    MMappedBuffer( unsigned char *addr, size_t len ) : addr(addr), len(len) {}
    ~MMappedBuffer(){
        if(addr && addr != MAP_FAILED){
            ::munmap(addr, len);
        }
    }
    MMappedBuffer(MMappedBuffer &&rhs) : addr(rhs.addr), len(rhs.len) {
        rhs.addr = 0;
        rhs.len  = 0;
    }
    MMappedBuffer &operator=(MMappedBuffer &&rhs){
        if(addr && addr != MAP_FAILED){
            ::munmap(addr, len);
        }
        addr = rhs.addr;
        len  = rhs.len;
        rhs.addr = 0;
        rhs.len  = 0;
    }
    
private:
    MMappedBuffer(MMappedBuffer const &);
    MMappedBuffer &operator=(MMappedBuffer const &);
};            

// SDL_Init/SDL_Quit
class SDLInitAndQuit{
public:
    SDLInitAndQuit(unsigned flags){
        if( SDL_Init( flags ) == -1 ){
            sdlExit("SDL_Init");    
        }    
    }
    ~SDLInitAndQuit(){
        SDL_Quit();
    }
private:
    SDLInitAndQuit(SDLInitAndQuit const &);
    SDLInitAndQuit &operator=(SDLInitAndQuit const &);
};
// SDL_Init/SDL_Quit
class TTFInitAndQuit{
public:
    TTFInitAndQuit(){
        if( TTF_Init()  == -1 ){
            sdlExit("TTF_Init");    
        }    
    }
    ~TTFInitAndQuit(){
        TTF_Quit();
    }
private:
    TTFInitAndQuit(TTFInitAndQuit const &);
    TTFInitAndQuit &operator=(TTFInitAndQuit const &);
};


struct SDL_Destroyer{
    void operator()(SDL_Window   *win){ if(win){ SDL_DestroyWindow(win);   } }
    void operator()(SDL_Renderer *ren){ if(ren){ SDL_DestroyRenderer(ren); } }
    void operator()(SDL_Surface  *sur){ if(sur){ SDL_FreeSurface(sur);     } }
    void operator()(SDL_Texture  *tex){ if(tex){ SDL_DestroyTexture(tex);  } }
    void operator()(TTF_Font     *fnt){ if(fnt){ TTF_CloseFont(fnt);      } }
};
// unique_ptr typedefs for SDL resources using proper cleanup API
typedef std::unique_ptr<SDL_Window,   SDL_Destroyer> SDL_WindowPtr;
typedef std::unique_ptr<SDL_Renderer, SDL_Destroyer> SDL_RendererPtr;
typedef std::unique_ptr<SDL_Surface,  SDL_Destroyer> SDL_SurfacePtr;
typedef std::unique_ptr<SDL_Texture,  SDL_Destroyer> SDL_TexturePtr;
typedef std::unique_ptr<TTF_Font,     SDL_Destroyer> TTF_FontPtr;

struct FILECloser{
    void operator()(FILE *f){
        if(f){
            ::fclose(f);
        }
    }
};
typedef std::unique_ptr<FILE, FILECloser> FILEPtr;


