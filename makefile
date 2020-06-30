CC = g++

FLAGS	= -std=c++11 -g -Wall -pedantic
# FLAGS	= -std=c++11 -g -Wall -pedantic -fsanitize=address -fsanitize=leak

DEPTH = -ftemplate-depth=10000

.PHONY: main
main: main.out

main.out: main.cpp
	$(CC) $(FLAGS) main.cpp  -o main.out

clean:
	rm -f *.o *.out
