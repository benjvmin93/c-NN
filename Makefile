CC=gcc
CFLAGS=-std=c99 -pedantic -Werror -Wall -Wextra -fsanitize=address
LDFLAGS=-lcriterion

all: src/main.c src/NeuralNetwork/neuralNet.c 
	${CC} ${CFLAGS} src/main.c src/image-process/SDL.c src/NeuralNetwork/neuralNet.c src/utils/matrix.c -I/usr/local/include -L/usr/local/lib -lSDL2 -lSDL2_image -o CNN

dbg: src/main.c src/NeuralNetwork/neuralNet.c 
	${CC} ${CFLAGS} -g src/main.c src/image-process/SDL.c src/NeuralNetwork/neuralNet.c src/utils/matrix.c -I/usr/local/include -L/usr/local/lib -lSDL2 -lSDL2_image -o CNN

clean:
	rm -rf CNN
