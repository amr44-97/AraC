#include "Compiler.hpp"
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

using AstType = Compiler::AstType;
using string = std::string;

namespace Asm{


#define L_VAR_S 1024*1024
static const char* List_vars[L_VAR_S];
static int var_count = 0; 

static char *rand_string(char *str, size_t size)
{
    const char charset[] = "_abcdefghij__klmnopqrstuvwxyzABCDEFGHIJK";
    const char Digit[]   = "01234567890123456789";
    if (size) {
        --size;
        for (size_t n = 0; n < size; n+=2) {
            int key = rand() % (int) (sizeof charset - 1);
            int key_d = rand() % (int) (sizeof Digit - 1);
            str[n] = charset[key];
            str[n+1] = Digit[key_d];
        }
        str[size] = '\0';
    }
        return str;
}

char* new_var(size_t size)
{
     char *s =(char*) malloc(size + 1);
     if (s) {
         rand_string(s, size);
     }
    List_vars[var_count] = s;
    var_count++;

     return s;
}

const char* new_var(){

    const char Alpha_Cap[]  = "ABCDEFGHIGKLMNOPQRSTUVWXYZ";
    const char Alpha_smal[] = "abcdefghigklmnopqrstuvwxyz";
    const char digits[]     = "01234567890123456789012345";
    const char sympols[]    = "_";
    
    const char* ll[] = { Alpha_Cap, Alpha_smal, digits }; 

    int Ai  = rand() % sizeof(Alpha_Cap);
    int ai  = rand() % sizeof(Alpha_smal);
    int di  = rand() % sizeof(digits);
    
     string *s = new string("_")  ;//+=  Alpha_Cap[Ai] + Alpha_smal[ai] + digits[di];
     for(int i = rand()%2 ; i < 3; i++){
     s +=  Alpha_Cap[Ai] + Alpha_smal[ai] + digits[di];
     s += ll[i][Ai];
     }

     if(var_count == 0){
       List_vars[0] = s->c_str();
     }
     if (var_count > 0) {
       for(int i =0 ; i < var_count ;i++){
         if(s->length() == strlen(List_vars[i])){
           s += Alpha_Cap[rand()%28] + Alpha_Cap[rand()%28];
           return s->c_str();
         }
       }
     }

     var_count+=1;
     printf("Working rand var !!\n");
  return s->c_str();
}

static const char *reglist[4]= { "%r8", "%r9", "%r10", "%r11" };
static int freereg[4];

void freeall_registers(void)
{
  freereg[0]= freereg[1]= freereg[2]= freereg[3]= 1;
}


static void free_register(int reg)
{
  if (freereg[reg] != 0) {
    fprintf(stderr, "Error trying to free register %d\n", reg);
    exit(1);
  }
  freereg[reg]= 1;
}


static int alloc_register(void)
{
  for (int i=0; i<4; i++) {
    if (freereg[i]) {
      freereg[i]= 0;
      return(i);
    }
  }
  fprintf(stderr, "Out of registers!\n");
  exit(1);
}


FILE* Outfile;

void Init_Ostream(FILE* file){
  Outfile = file;
}

const char* cgload(int value){
  fprintf(Outfile, "\tint %s = %d ;\n",new_var(23),value);
    return List_vars[var_count -1];
}

int cgadd(const char* r1, const char* r2) {
  fprintf(Outfile, "\tint %s =\t%s + %s ;\n",new_var(23),r1,r2);
  return var_count ;
}

int cgsub(const char* r1, const char* r2) {
  fprintf(Outfile, "\tint %s  =\t%s - %s ;\n",new_var(23), r1,r2);
  return var_count ;
}


int cgmul(const char* r1, const char* r2) {
  fprintf(Outfile, "\tint %s  =\t%s * %s ;\n",new_var(23), r1,r2);
  return var_count;
}

// Divide the first register by the second and
// return the number of the register with the result
const char* cgdiv( const char* r1, const char* r2) {
  fprintf(Outfile, "\tint %s  =\t%s + %s ;\n", new_var(23),r1,r2);
  return List_vars[var_count -1];
}

static const char* genAst(Compiler::Ast* T){
        const char *leftReg, *rightReg;

        if (T->left) leftReg = genAst(T->left);
        if (T->right) rightReg = genAst(T->right);
        // 5 + 3 * 4 - 1
        switch (T->op) {
          case AstType::ADD : return (List_vars[cgadd(leftReg,rightReg) -1]);
          case AstType::SUB : return (List_vars[cgsub(leftReg,rightReg) -1]);
          case AstType::MUL : return (List_vars[cgmul(leftReg,rightReg) -1]);
          case AstType::DIV : return (cgdiv(leftReg,rightReg));
          case AstType::INT : return (cgload(T->Intval));
          default: {fprintf(stderr,"[ERROR]:Unknown Ast OP !!\n"); exit(1);}

   }
 };


void cgprintint() {
  fprintf(Outfile,
      "\nprintf(\"%%i \",%s);",List_vars[var_count-1]);
}


// Print out the assembly preamble
void cgpreamble()
{
  fprintf(Outfile,"#include <stdio.h>\n\n"   "int main(){\n\n" );
}

// Print out the assembly postamble
void cgpostamble()
{
  fputs(
	"\n}\n",
  Outfile);
}

void gen_asm(Compiler::Ast* n) {

  cgpreamble();
  printf("preaanle Woriking \n");
  genAst(n);
  cgprintint();      // Print the register with the result as an int
  cgpostamble();
  printf("post Woriking \n");
}


}



