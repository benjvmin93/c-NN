CC=gcc
CFLAGS=-std=c99 -lm -pedantic -Werror -Wall -Wextra -fsanitize=address
OBJ=src/main.c src/image-process/SDL.c src/NeuralNetwork/filter.c src/NeuralNetwork/neuralNet.c src/utils/xmalloc.c src/utils/matrix.c src/utils/files.c
LIB=-I/usr/local/include -L/usr/local/lib -lm -lSDL2 -lSDL2_image

all:
	${CC} ${CFLAGS} ${OBJ} ${LIB} -o CNN

dbg:
	${CC} ${CFLAGS} -g ${OBJ} ${LIB} -o CNN

test:
	./tests/run-test.sh

test-cnn:
	./tests/run-test.sh neuralnet

test-matrix:
	./tests/run-test.sh matrix

clean:
	rm -f ./tests/*.c ./tests/compilation.log CNN
