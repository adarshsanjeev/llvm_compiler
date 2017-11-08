OBJ = bcc
CXX = g++ -Wall
LEX = flex 
YACC = bison -d --debug --verbose --report=all --warnings=all

$(OBJ):	parser.tab.c lex.yy.c
	$(CXX) parser.tab.c -lfl -o $(OBJ) -flto `llvm-config --ldflags --system-libs --libs core native`

parser.tab.c: parser.y lex.yy.c ast.hpp visitor.hpp llvmv.hpp
	$(YACC) parser.y 
lex.yy.c: scanner.l
	$(LEX) scanner.l

.PHONY: clean 
clean:
	-@rm -rf parser.tab.c parser.tab.h lex.yy.c $(OBJ) parser.output 2>/dev/null || true
