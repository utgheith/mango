CC_FLAGS = -std=c++23 -O3 -Wall -Werror
CC_FILES = ${wildcard *.cc}

O_FILES = ${patsubst %.cc, %.o, ${CC_FILES}}

all : main

main : ${O_FILES} Makefile
	g++ -o main ${O_FILES}

${O_FILES} : %.o : %.cc Makefile
	g++ -MMD ${CC_FLAGS} -c $*.cc


format:
	clang-format -style=Google -Werror -i *.cc *.h

clean:
	rm -rf *.o *.d main

-include *.d

