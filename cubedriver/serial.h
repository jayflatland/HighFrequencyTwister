// 
// serial.h - Paul Rose - Nov 2015
// 
// simple wrapper for serial access
//
// hard coded to 115200 baud, raw
//
#pragma once

#include "util.h"

class Serial{
public:
    Serial(std::string const &dev);
    ~Serial();

    std::string const &dev() const { return dev_; }

    int read(); // reads 1 character
    void write( char c ); // writes 1 character
    int command(char c); // writes 1 character command, waits for 1 character result

    // for select/poll/epoll prior to read()
    int fd() { return fd_; }

private:
    std::string const dev_;
    FD          const fd_;
};

class SerialNonBlocking{
public:
    SerialNonBlocking(std::string const &dev);
    ~SerialNonBlocking();

    std::string const &dev() const { return dev_; }

    // for select/poll/epoll prior to read()
    int fd() { return fd_; }

    // write str ( or buffer for writing )
    void write(std::string const &str);
    // read str ( or return blank if nothing ready )
    std::string read();
    
    bool writesBuffered()const;
    void onReadyToWrite();
    
private:
    std::string const dev_;
    FD          const fd_;

    std::vector<char> writeBuffer_;
};
