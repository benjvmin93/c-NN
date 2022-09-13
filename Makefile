CC=gcc
CFLAGS=-std=c99 -pedantic -Werror -Wall -Wextra -fsanitize=address
LDFLAGS=-lcriterion

all: src/main.c src/NeuralNetwork/neuralNet.c 
	$(CC) $(CFLAGS) src/main.c src/NeuralNetwork/neuralNet.c -g -o recognitionAI

check: tests/tests.c
	$(CC) $(CFLAGS) $(LDFLAGS) test -o tests/tests.c
	./test

clean:
	rm -rf recognitionAI test src/*.o tests/*.o
