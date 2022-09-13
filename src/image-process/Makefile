CC=gcc
CFLAGS=-std=c99 -pedantic -Werror -Wall -Wextra
DEBUG=-g -fsanitize=address
LDFLAGS=-lcriterion

sdl: SDL.c
	${CC} ${CFLAGS} SDL.c -I/usr/local/include -L/usr/local/lib -lSDL2 -lSDL2_image -o sdl

dbg:
	${CC} ${CFLAGS} ${DEBUG} SDL.c -I/usr/local/include -L/usr/local/lib -lSDL2 -lSDL2_image -o sdl

filter:
	${CC} ${CFLAGS} filter.c -o filter