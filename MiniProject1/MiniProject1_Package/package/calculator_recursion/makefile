CC			:= gcc
EXE			:= calculator
H_FILES		:= lex.h parser.h codeGen.h
C_FILES		:= lex.c parser.c codeGen.c main.c
O_FILES		:= lex.o parser.o codeGen.o main.o

.PHONY: all

all: $(O_FILES)
	@$(CC) -o $(EXE) $(O_FILES)

%.o: %.c
	@$(CC) -c $^ -o $@

merge:
	@cat $(H_FILES) $(C_FILES) | sed "s/#include \".*\"//g" > out.c

clean:
	rm -rf $(O_FILES) $(EXE)

cleanO:
	rm -rf $(O_FILES)