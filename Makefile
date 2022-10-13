CC=gcc
CFLAGS=-std=c99 -lm -pedantic -Werror -Wall -Wextra -fsanitize=address
LDFLAGS=-lcriterion

all: src/main.c src/NeuralNetwork/neuralNet.c 
	${CC} ${CFLAGS} src/main.c src/image-process/SDL.c src/NeuralNetwork/filter.c src/NeuralNetwork/neuralNet.c src/utils/matrix.c -I/usr/local/include -L/usr/local/lib -lmath -lSDL2 -lSDL2_image -o CNN

dbg: src/main.c src/NeuralNetwork/neuralNet.c 
	${CC} ${CFLAGS} -g src/main.c src/image-process/SDL.c src/NeuralNetwork/neuralNet.c src/NeuralNetwork/filter.c src/utils/matrix.c -I/usr/local/include -L/usr/local/lib -lSDL2 -lSDL2_image -o CNN

test:
	./tests/run-test.sh

test-cnn:
	./tests/run-test.sh neuralnet

test-matrix:
	./tests/run-test.sh matrix

clean:
	rm -f ./tests/*.c  ./tests/compilation.log CNN