#include "Compiler.hpp"
#include <stdio.h>
#include <stdlib.h>

namespace Compiler {
  void ThrowError(const char *msg){
       fprintf(stderr,"[Error]:%s\n",msg);
       exit(1);
  }
}
