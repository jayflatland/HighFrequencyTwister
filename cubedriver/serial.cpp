// 
// serial.h - Paul Rose - Nov 2015
// 
// simple wrapper for serial access
//
// hard coded to 115200 baud, raw
//
#include <fcntl.h>
#include <termios.h>

#include "serial.h"

Serial::Serial(std::string const &dev)
    : dev_(dev)
    , fd_( open( dev_.c_str(), O_RDWR | O_NOCTTY ) )
{
    if( fd_ < 0 ) throwErrno(dev_ + ": open"); 

    struct termios tio;
    memset( &tio, 0, sizeof(tio) );
    cfmakeraw(&tio); // don't wait for newline, etc.
    cfsetispeed(&tio,B115200);
    cfsetospeed(&tio,B115200);
    if( tcsetattr(fd_, TCSAFLUSH, &tio) < 0 ){
        throwErrno(dev_ + ": tcsetattr RAW 115200");
    }
}

Serial::~Serial(){
    // close will be handled by fd_ dtor
}

int Serial::read(){
    unsigned char b;
    if( ::read( fd_, &b, 1 ) < 0 ){
        if( errno == EWOULDBLOCK ){
            return -1;
        }
        throwErrno(dev_ + ": read");
    }
    return b;
}

void Serial::write(char c){
    if( ::write( fd_, &c, 1 ) < 0 ){
        throwErrno(dev_ + ": write");
    }
}

int Serial::command(char c){
    write(c);
    return read();
}

SerialNonBlocking::SerialNonBlocking(std::string const &dev)
    : dev_(dev)
    , fd_( open( dev_.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK ) )
{
    if( fd_ < 0 ) throwErrno(dev_ + ": open"); 

    struct termios tio;
    memset( &tio, 0, sizeof(tio) );
    cfmakeraw(&tio); // don't wait for newline, etc.
    cfsetispeed(&tio,B115200);
    cfsetospeed(&tio,B115200);
    if( tcsetattr(fd_, TCSAFLUSH, &tio) < 0 ){
        throwErrno(dev_ + ": tcsetattr RAW 115200");
    }
}

SerialNonBlocking::~SerialNonBlocking(){
    // close will be handled by fd_ dtor
}


std::string SerialNonBlocking::read(){
    std::string ret;
    char buff[128];
    
    int n;
    while( (n = ::read( fd_, buff, sizeof(buff) )) > 0){
        ret.append( buff, n );
    }
    if( n < 0 && errno != EWOULDBLOCK ){
        throwErrno(dev_ + ": read");
    }
    return ret;
}

void SerialNonBlocking::write(std::string const &str){
    int n = 0;
    if( writeBuffer_.empty() ){
        n = ::write( fd_, str.c_str(), str.size() );
        if( n < 0 ){
            if( errno != EWOULDBLOCK ){
                throwErrno(dev_ + ": read");
            }
            n = 0;
        }
    }
    if( n < str.size() ){
        writeBuffer_.insert( writeBuffer_.end(), str.begin() + n, str.end() );
    }
}
    
bool SerialNonBlocking::writesBuffered()const{
    return !writeBuffer_.empty();
}
        
void SerialNonBlocking::onReadyToWrite(){
    if(!writeBuffer_.empty()){
        int n = ::write( fd_, &writeBuffer_[0], writeBuffer_.size() );
        if( n < 0 ){
            if( errno != EWOULDBLOCK ){
                throwErrno(dev_ + ": read");
            }
            n = 0;
        }
        if( n > 0 ){
            writeBuffer_.erase( writeBuffer_.begin(), writeBuffer_.begin()+n );
        }
    }
}

#ifdef TEST_MAIN
int main(int argc, char **argv) {
    Serial ser(argv[1]);

    printf("waiting for Hello from Arduino\n");
    
    int c;
    while((c=ser.read()) != 'H'){
       fprintf(stderr, "unexpected char '%c' while waiting for 'H'\n", c);
    }

    printf("Hello received\n");

    ser.command('o');
     
    printf("press enter");
    char buf[128];
    if(!fgets(buf, sizeof(buf), stdin)){
    }

    char const *commands = "444OUuDdUuFfBbLlRro";

    for(char const *p=commands; *p; ++p) {
        printf("sending %c\n", *p);
        int r = ser.command(*p);
        printf("received %c\n", r);
    }
    printf("Done\n");
}
#endif
