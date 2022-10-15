#include "Compiler.hpp"
#include <cstdlib>
#include <stdio.h>

namespace Compiler{

int interpretAst(Ast* Tree){
        int left, right;
        
        if(Tree->left != nullptr) left = interpretAst(Tree->left);
        if(Tree->right != nullptr) right = interpretAst(Tree->right);


        switch (static_cast<int >(Tree->op)) {
          case static_cast<int>(AstType::ADD): return (left + right);
          case static_cast<int>(AstType::SUB): return (left - right);
          case static_cast<int>(AstType::MUL): return (left * right);
          case static_cast<int>(AstType::DIV): return (left / right);
          case static_cast<int>(AstType::INT): return (Tree->Intval);
          default:{
                    fprintf(stderr,"[ERROR]: Unknown Ast Type (%s)\n", Tree->Tok.Text.c_str());
                    exit(1);
                  }
        }
}




}
