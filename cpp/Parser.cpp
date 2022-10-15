#include "Compiler.hpp"
#include <cstdio>
#include <cstdlib>
#include <stdlib.h>
#include <string.h>

namespace Compiler {


const char *AstTypeList[] = {"INT", "ADD", "SUB", "DIV", "MUL", "UnKnown"};

Parser::Parser(Lexer lex) { this->TokList = lex.TokList; }

static int tok_i = 0;

void Parser::next() {
  auto Token = this->TokList[tok_i];
  tok_i++;
  Cur_tok = Token;
}

static void* Allocation_List[1024];
static int Alloc_index=0;
void Add_to_Alloc_list(void* ptr){
  Allocation_List[Alloc_index] = ptr;
  Alloc_index++;
}


Ast *makeTree(Token tok, AstType op, Ast *left, Ast *right, int Intval) {
  Ast *Tree = (Ast *)calloc(1, sizeof(Ast));
  Add_to_Alloc_list(Tree);
  if (Tree == nullptr)
    fprintf(stderr, "[ERROR]: Unable to malloc new node at makeTree().\n");
  Tree->Tok = tok;
  Tree->op = op;
  Tree->left = left;
  Tree->right = right;
  Tree->Intval = Intval;
  return Tree;
};

void freeAst(Ast* T){
  if(T != nullptr){
    free(T);
    freeAst(T->left);
    freeAst(T->right);
  }
}

void freeAllocs(){
  for(int i=0; i < Alloc_index;i++){
    free(Allocation_List[i]);
    Allocation_List[i] = nullptr;
  }
}

Ast *makeleaf(Token tok, AstType op, int Intval) {
  return makeTree(tok, op, nullptr, nullptr, Intval);
}

Ast *makeunaryTree(Token tok, AstType op, Ast *left, int Intval) {
  return makeTree(tok, op, left, nullptr, Intval);
}

AstType ToAstType(TokenType T) {
  AstType AT;
  switch (T) {
  case TokenType::Tok_Plus:
    AT = AstType::ADD;
    break;
  case TokenType::Tok_Minus:
    AT = AstType::SUB;
    break;
  case TokenType::Tok_Slash:
    AT = AstType::DIV;
    break;
  case TokenType::Tok_Star:
    AT = AstType::MUL;
    break;
  default: {
    AT = AstType::UnKnown;
    break;
  }
  }
  return AT;
}

Ast *Parser::ParsePrimaryExpr() {
  Ast *n;
  switch (Cur_tok.Id) {
  case TokenType::Tok_num: {
    n = makeleaf(Cur_tok, AstType::INT, Cur_tok.Intval);
    next();
    return n;
  }
  default: {
    fprintf(stderr, "[ERROR]: Syntax error at Token = (%s, %i)\n",
            Cur_tok.getId(), Cur_tok.Intval);
    exit(1);
  }
  }

}

enum class presTable { ADD = 1, SUB = 1, MUL = 2, DIV = 2 };

int getpresd(TokenType T) {
  if (T == TokenType::Tok_Plus || T == TokenType::Tok_Minus)
    return 1;
  if (T == TokenType::Tok_Star || T == TokenType::Tok_Slash)
    return 2;
  else
    return 0;
}

Ast *Parser::ParseNumExpr(int ptp) {
  Ast *left, *right;

  left = ParsePrimaryExpr();

  Token prevTok = Cur_tok;

  if (prevTok.Id == TokenType::Tok_Eof)
    return left;

  while (getpresd(prevTok.Id) > ptp) {
    next();

    right = ParseNumExpr(getpresd(prevTok.Id));

    left = makeTree(prevTok, ToAstType(prevTok.Id), left, right, 0);

    prevTok = Cur_tok;

    if (prevTok.Id == TokenType::Tok_Eof) {
      return left;
    }
  }
  return left;
}

// 3 + 5 * 4 - 1

/*           +
 *         /   \
 *        3     *
 *             / \
 *            5   4
 *
 * ptok = -  ;
 *  
 * ptp  = 2  ;
 *
 *        +  
 *      /   \
 *   (3)    *
 *        /  \
 *      (5) (4) 
 * 
 *
 *
 *
 *   3 lnode -> isnext =  eof  (+) -> 5 lnode  -> isnext = eof (*) -> 4 lnode ->
 * isnext = eof ->
 *
 *
 *              /
 *    (3) (5) (4)
 *
 *
 * */

Parser InitParser(Lexer lr) {
  Parser np(lr);
  lr.ScanAll();
  np.next();
  return np;
}

void printTree(const std::string &prefix, const Ast *node, bool isLeft) {

  if (node != nullptr) {
    printf("%s", prefix.c_str());

    printf("%s", (isLeft ? "├──" : "└──"));

    // print the value of the node
    if (node->op == Compiler::AstType::INT) {
      printf("(%i)\n", node->Intval);
    } else {
      //     std::cout << "("<< AstTypeList[static_cast<int>(node->op)]  <<
      //     ")"<< std::endl;
      printf("(%s)\n", node->Tok.Text.c_str());
    }
    // enter the next tree level - left and right branch
    printTree(prefix + (isLeft ? "│   " : "    "), node->left, true);
    printTree(prefix + (isLeft ? "│   " : "    "), node->right, false);
  }
}

void printTree(const Ast *node) { printTree("", node, false); }

} // namespace Compiler
