#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include "Compiler.hpp"
#include "./Include/file_handle.hpp"

void usage(){
  printf("Ara [file_name]\n");
}


int main(int argc , char** argv){
  
  if(argc < 2){
    usage();
    return 0;
  }

  Compiler::Lexer Tokenizer = Compiler::InitLexer(argv[1]);

  Tokenizer.TokList =  Tokenizer.ScanAll();
 
  Compiler::Parser P(Tokenizer);//= Compiler::InitParser("./ara.txt");

 // Tokenizer.PrintTokens();

  P.next();
  Compiler::Ast* n = P.ParseNumExpr(0);
  
//  P.lex.ScanAll();


 // printer(n);
  Compiler::printTree(n);
 
  FILE *fp = fopen("Out.c","w+");
  const char* p  = "#include <stdio.h>\n\n"
                   "int main(){\n\n"
                   "printf(\"Hello World!!!!!!\\n\");\n\n"
                   "return 0;\n"
                   "}"
;
  fwrite(p,1,strlen(p),fp); 
  File fps;
  fps.open("c_out.c", "wr+");

  FILE *s  = fopen("Out_asm.c","wb");
  if(s == NULL){
    fprintf(stderr, "Error\n");
    exit(1);
  }
  const char* ps = "#include <stdio.h>\n\n" "int main(){\n";
  fwrite(ps,1,strlen(ps),s);
  

  Asm::Init_Ostream(fps.ptr);
  Asm::gen_asm(n);


 // printf("result = %i\n", Compiler::interpretAst(n));

  Compiler::freeAlloc();
  Tokenizer.deinit();

  fps.close();
}
