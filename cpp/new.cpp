#include "Compiler.hpp"
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>

void usage() { printf("Ara [file_name]\n"); }

int main(int argc, char **argv) {

  if (argc < 2) {
    usage();
    return 0;
  }

  Compiler::Lexer Tokenizer = Compiler::InitLexer(argv[1]);

  Tokenizer.TokList = Tokenizer.ScanAll();

  Compiler::Parser P(Tokenizer); //= Compiler::InitParser("./ara.txt");

  // Tokenizer.PrintTokens();

  P.next();
  Compiler::Ast *n = P.ParseNumExpr(0);

  //  P.lex.ScanAll();

  // printer(n);
  Compiler::printTree(n);

  printf("result = %i\n", Compiler::interpretAst(n));
}
