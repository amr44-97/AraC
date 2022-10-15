
using  System;
using  System.IO;

namespace AraCompiler{
  class Program {
    static void Main(string[] args){
    
      var Line = File.ReadAllText("./test.ara");
        
      if(string.IsNullOrWhiteSpace(Line))
          return;

      var Lexer  = new Tokenizer(Line);

        while(true)
        {
          var token = Lexer.NextToken();
          if(token.Type == TokenType.Eof)
            break;
          Console.Write($"{token.Type}: '{token.Text}'");
          if(token.Value != null)
            Console.WriteLine($" {token.Value}");
          Console.WriteLine();

        }
  }
}

  enum TokenType{
    Eof,
    Number,
    WhiteSpace,
    Plus,
    Minus,
    Slash,
    Star,
    OpenParenthesis,
    CloseParenthesis,
    BadToken,
  }

  class Token{
    public Token(TokenType type, int position, string text, object valu){
      Type = type;
      Position = position;
      Text = text;
      Value = valu;
    }

    public TokenType Type{get;}
    public int Position{get;}
    public string Text{get;}
    public object Value{get;}
  }
     
  class Tokenizer{
      private readonly string _text;
      private int _position;

      public Tokenizer(string txt){
        _text = txt;
      }

      private char Current {
        get{
          if(_position >= _text.Length)
            return '\0';
          return _text[_position];
      }
    }

    
      private void Next(){ _position++; }

      public Token NextToken()
      {
        if(_position >= _text.Length)
          return new Token(TokenType.Eof,_position, "\0",null);

        if(char.IsDigit(Current))
        {
          
          var start = _position;
          
          while(char.IsDigit(Current))
            Next();
          
          var length = _position - start;
          var text  = _text.Substring(start, length);
          int.TryParse(text, out var value);
          return new Token(TokenType.Number, start, text , value);
        }
     
        if(char.IsWhiteSpace(Current))
        {
          
          var start = _position;
          
          while(char.IsWhiteSpace(Current))
            Next();
          
          var length = _position - start;
          var text  = _text.Substring(start, length);
          return new Token(TokenType.WhiteSpace, start, text , null);
        }

        if(Current == '+')
          return new Token(TokenType.Plus, _position++, "+",null);
        else if(Current == '-')
          return new Token(TokenType.Minus, _position++, "-",null);
        else if(Current == '/')
          return new Token(TokenType.Slash, _position++, "/",null);
        else if(Current == '*')
          return new Token(TokenType.Star, _position++, "*",null);
        else if(Current == '(')
          return new Token(TokenType.OpenParenthesis, _position++, "(",null);
        else if(Current == ')')
          return new Token(TokenType.CloseParenthesis, _position++, ")",null);

          
        return new Token(TokenType.BadToken , _position++, _text.Substring(_position -1 , 1), null);


      }

}
}



