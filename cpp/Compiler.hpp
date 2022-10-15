#pragma once
#include <stdio.h>
#include <string>
#include <vector>
#include "Include/file_handle.hpp"

namespace Compiler {


  void ThrowError(const char* msg);


enum class TokenType {
  Tok_Eof=0,

  Tok_Plus,
  Tok_Minus,

  Tok_Slash,
  Tok_Star,

  Tok_num,
  Tok_newline,
  Tok_WhiteSpace,
  Tok_OpenParenthesis,
  Tok_CloseParenthesis,

  Tok_BadTok,
};

class Token {
public:
  TokenType Id;
  std::string Text;
  int Intval;
  int Pos;
  bool Isnum;

  const char *getId();
};

Token maketok(TokenType id, std::string txt, int intval, int position,
              bool isnum = false);

class Lexer {
  int buf_index = 0;

public:
  const char *file_name;
  std::string buffer;
  std::vector<Token> TokList;
  File fd;
  Token CurrentToken;

  std::vector<Token> ScanAll();
  void nextToken();
  void PrintTokens();

  void deinit();

};


Lexer InitLexer(const char *file_name);

enum class AstType { INT=0, ADD, SUB, DIV, MUL, UnKnown };

struct Ast {
  Token Tok;
  AstType op;
  Ast *left;
  Ast *right;
  int Intval;
  // Functions
};

struct Parser {
  Parser();
  Parser(Lexer lex);
  Lexer lex;
  Ast *ParsePrimaryExpr();
  Ast *ParseNumExpr(int ptp);

  Token Cur_tok;
  void next();
  std::vector<Token> TokList;
};

AstType ToAstType(TokenType T);
Ast *makeTree(Token tok, AstType op, Ast *left, Ast *right, int Intval);
Ast *makeleaf(Token Tok, AstType op, int Intval);
Ast *makeunaryTree(Token Tok, AstType op, Ast *left, int Intval);

void printTree(const Ast *node);
Parser InitParser(const char *file_name);
AstType ToAstType(TokenType T);

int interpretAst(Ast *Tree);

void freeAst(Ast* T);
void freeAllocs() ;







} // namespace Compiler
  //


namespace Asm {
  void Init_Ostream(FILE* file);
  void gen_asm(Compiler::Ast* n);

}

