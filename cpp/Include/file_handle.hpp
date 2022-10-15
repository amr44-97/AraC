#ifndef F_HANDLER
#define F_HANDLER 
#include "LibTypes.hpp"
#include<stdio.h>


struct File{
    const char *name;
    size_t size = 0 ;
    FILE *ptr   = nullptr;
    String buf; // replaced with string from  libstr.h
    
    
 //   File(const char *file_name,const char *flag);
    void open(const char *file_name,const char *flag);
    String read_to_string();
    //String read_to_string(const char * file_name);
    void copy_to(const char *dest);
    void close();
   // ~File();
    private: 
      int opened=0;
   

};


#endif

