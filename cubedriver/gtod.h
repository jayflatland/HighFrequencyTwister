#pragma once

#include <sys/time.h>

inline unsigned long gtod(){
   struct timeval tv;
   gettimeofday(&tv,NULL);
   return tv.tv_sec * 1000000 + tv.tv_usec;
}

