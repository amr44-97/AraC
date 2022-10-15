#pragma once
#include "Compiler.hpp"
#include <cstdio>
#include <stdlib.h>



namespace Compiler {


  enum class AstType{INT, ADD , SUB, DIV , MUL, UnKnown };
  
  struct Ast{
    AstType op;
    Ast* left ;
    Ast* right;
    int Intval;
    // Functions 
  };

  struct Parser{
  
    Lexer lex;


        AstType ToAstType(TokenType T);
        Ast* makeTree(AstType op, Ast* left, Ast* right,int Intval);
        Ast* makeleaf(AstType op, int Intval);
        Ast* makeunaryTree(AstType op, Ast* left,int Intval);



        Ast* ParsePrimaryExpr();
        Ast* ParseNumExpr(int ptp);
        

  };

  void printTree(Ast* Tree, int indent);
  Parser InitParser(const char *file_name);

}
