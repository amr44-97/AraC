CC = g++

CFLAGS = -Wall -Wextra -g

SRC = Main.cpp

#LINK = ./Include/LibTypes.o ./Include/file_handle.o  Tokenizer.cpp Parser.cpp 


LINK  = Parser.cpp Tokenizer.cpp interpret.cpp Error.cpp Asm_Gen.cpp
#OBJ  = Parser.o Tokenizer.o interpret.o Error.o Asm_Gen.o

OBJ =  ./Include/LibTypes.o ./Include/file_handle.o  Parser.o Tokenizer.o interpret.o Error.o Asm_Gen.o

EXE = Ara

#All = all

all:
	$(CC) $(CFLAGS) -c $(LINK) 
	$(CC) $(CFLAGS) $(OBJ) -o $(EXE) $(SRC)



clean:
	rm $(EXE)
	rm Parser.o Tokenizer.o
# DO NOT DELETE
