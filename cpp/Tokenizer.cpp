#include "Compiler.hpp"
#include "Include/file_handle.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <ctype.h>


using namespace Compiler;

static int to_int(char const *s)
{
  if ( s == NULL || *s == '\0' ){
    ThrowError("null or empty string argument\n" );
  }

     bool negate = (s[0] == '-');
     if ( *s == '+' || *s == '-' ) 
         ++s;

     if ( *s == '\0'){
        ThrowError("sign character only.\n" );
     }

     int result = 0;
     while(*s)
     {
       if ( *s < '0' || *s > '9' ){
          ThrowError("invalid input string\n" );
       }
          result = result * 10  - (*s - '0');  //assume negative number
          ++s;
     }
     return negate ? result : -result; //-result is positive!
}

namespace Compiler {
  
static const char* TokTypeList[] = {
 
    "Tok_Eof",

    "Tok_Plus",
    "Tok_Minus",

    "Tok_Slash",
    "Tok_Star",

    "Tok_num",
    "Tok_newline",
    "Tok_WhiteSpace",
    "Tok_OpenParenthesis",
    "Tok_CloseParenthesis",
    "Tok_BadTok",
    };

  const char* Token::getId() {
    return TokTypeList[ 
     static_cast<int>(this->Id)
    ];
  }



  Token maketok(TokenType id, std::string txt, int intval, int position, bool isnum ){
          Token toke;
          toke.Id     = id;
          toke.Text   = txt;
          toke.Intval = intval;
          toke.Pos    = position;
          toke.Isnum  = isnum;
    return toke;
  }


std::vector<Token> Lexer::ScanAll(){
        File fp;
        fp.open(this->file_name,"r+");
        this->buffer = fp.read_to_string().str;
        fp.buf.destroy();
        fp.close();
        std::vector<Token> List; 


    for(size_t x =0; x < buffer.length() ;x++){
      std::string ts = ""; 
      switch (buffer[x]) {
          case '+':{  
                     ts+=buffer[x];
                     List.push_back(maketok( TokenType::Tok_Plus, ts,0, x));
                     ts ="";
                     break; 
                   }
          case '-': {
                     ts += buffer[x]; 
                     List.push_back(maketok(TokenType::Tok_Minus, ts,0, x));
                     ts ="";
                     break; 
                    }

          case '*': {  
                      ts += buffer[x];  
                      List.push_back(maketok(TokenType::Tok_Star, ts,0, x) );
                     ts ="";
                      break; }

          case '/': {  
                      ts += buffer[x];  
                      List.push_back(maketok(TokenType::Tok_Slash,ts,0,x));
                      ts ="";
                      break; 
                    }
          case ' ': {  
                     // ts += buffer[x];  
                     // List.push_back(maketok(TokenType::Tok_WhiteSpace, ts,0,x));
                     ts ="";
                      break; 
                    }
         case '\n': {  
             //         ts += buffer[x];  
             //          List.push_back(maketok(TokenType::Tok_newline, ts,0,x));
                      ts ="";
                      break; 
                    }

          case '(': {  
                     ts += buffer[x];  
                     List.push_back(maketok(TokenType::Tok_OpenParenthesis, ts,0,x));
                     ts ="";
                      break; 
                    }
          
          case ')': {  
                      ts += buffer[x];  
                      List.push_back(maketok(TokenType::Tok_CloseParenthesis, ts,0,x));
                      ts ="";
                      break; 
                    }



          case '0':case '1':case '2':case '3':case '4':
          case '5':case '6':case '7':case '8':case '9': 
          {
            int start = x;
            int end   = 0;
            while(isdigit(buffer[x])){
                end++;
                x++;
            }
            auto txt = buffer.substr(start,end);
            int tmp  = to_int(txt.c_str());
            List.push_back(maketok(TokenType::Tok_num ,txt,tmp,start + x -1,true));
            x--;
            ts ="";
           break;                                               
          }
          default:{ ThrowError("Unknown Token\n"); }
        }
       }
            List.push_back(maketok(TokenType::Tok_Eof ,"\0",0, buffer.length()));
        return List;
}



void Lexer::nextToken()  {
        
       //Token token;

       char  c = buffer[buf_index];
       std::string ts ="";
      
       if(buf_index >= (int) buffer.length()){
         CurrentToken = maketok(TokenType::Tok_Eof, ts,0, buf_index);
         //return CurrentToken;
       }
        switch (c) {
          case '+':{  
                     ts += c;
                     CurrentToken =  maketok(TokenType::Tok_Plus, ts,0, buf_index);
                     break; 
                   }
          case '-': {
                     ts += c; 
                     CurrentToken =  maketok(TokenType::Tok_Minus, ts,0, buf_index);
                     break; 
                    }

          case '*': {  
                      ts += c;  
                      CurrentToken = maketok(TokenType::Tok_Star, ts,0, buf_index);  
                      break; }
          case '/': {  
                      ts += c;  
                      CurrentToken =  maketok(TokenType::Tok_Slash, ts,0, buf_index); 
                      break; 
                    }

          case '(': {  
                      ts += c;  
                      CurrentToken =  maketok(TokenType::Tok_OpenParenthesis, ts,0, buf_index); 
                      break; 
                    }
          
          case ')': {  
                      ts += c;  
                      CurrentToken =  maketok(TokenType::Tok_CloseParenthesis, ts,0, buf_index); 
                      break; 
                    }

          case ' ': {  
                    //  ts += c;  
                    // token =  maketok(TokenType::Tok_WhiteSpace, ts,0, buf_index); 
                      break; 
                    }
          case '\n': {  
                      ts += c;  
                      CurrentToken =  maketok(TokenType::Tok_newline, ts,0, buf_index); 
                      break; 
                    }


          case '0':case '1':case '2':case '3':case '4':
          case '5':case '6':case '7':case '8':case '9': 
          {
           
            int start = buf_index;
            int end   = 0;
            while(isdigit(c)){
                end++;
                buf_index++;
                c = buffer[buf_index];
            }
            auto txt = buffer.substr(start,end);
            int tmp  = to_int(txt.c_str());
            CurrentToken = maketok(TokenType::Tok_num ,txt,tmp,start + buf_index -1,true);
            buf_index--;
            break;                                               
          }
          default:{ ts+=c; printf("[Error]: Unknown Token -> (%s)\n",ts.c_str());  }
        }
       buf_index++;

 //       return CurrentToken;
}




Lexer InitLexer(const char *file_name){
  Lexer lex;
  lex.file_name = file_name;
  lex.fd.open(file_name, "r+");
  lex.buffer = lex.fd.read_to_string().str;
  return lex;
}

void Lexer::deinit(){
    this->fd.buf.destroy();
    this->fd.close();
    
}


void Lexer::PrintTokens(){
  std::vector<Token> tl = this->TokList;

  for(auto i : tl){
    if(i.Isnum){
      printf("ID = { %s},  Text = {%s}, pos = [%i],   intval = %i\n" , i.getId(), i.Text.c_str(), i.Pos , i.Intval);
    }
    else {
      printf("ID = { %s},  Text = {%s}, pos = [%i]\n" , i.getId(), i.Text.c_str(), i.Pos );
    }
  
    }

}




}
