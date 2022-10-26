#!/bin/sh

RED='\033[0;31m'
YELLOW='\033[0;33m'
GREEN='\033[0;32m' 
CYAN='\033[0;36m'
NC='\033[0m'

BBlue='\033[1;34m'
UBlack='\033[4;30m'

CC=gcc
CFLAGS="-std=c99 -pedantic -Werror -Wall -Wextra -fsanitize=address"

MATRIX_FILES="src/utils/matrix.c src/utils/xmalloc.c"
NN_FILES="src/NeuralNetwork/neuralNet.c src/image-process/SDL.c src/NeuralNetwork/filter.c src/utils/files.c"

TESTS_DONE=0
TESTS_SUCCEED=0
TESTS_FAILED=0

run_all_tests() {
    run_test matrix
    run_test neuralnet
}

run_test() {
    printf "\n${BBlue}=======================RUNNING $1 TESTS=======================${NC}\n\n";

    for file in `ls tests/$1`; do
        output=$(echo $file | sed -e 's/\.[^.]*$/\./')out
        if [ "$1" = "matrix" ]; then
            ${CC} ${CFLAGS} -g "tests/$1/$file" ${MATRIX_FILES} -o tests/$output >tests/compilation.log 2>&1

        elif [ $1 = "neuralnet" ]; then
            ${CC} ${CFLAGS} -g "tests/$1/$file" ${MATRIX_FILES} ${NN_FILES} -lSDL2 -lSDL2_image -o tests/$output -lm >tests/compilation.log 2>&1
        else
            echo "Bad arg: $1"
            exit 1
        fi
        COMPILE=$?

        printf "######################################\n\n"

        if [ "$COMPILE" -eq 1 ]; then
            printf "Compilation failed: ./tests/${output}\n\n"
            printf "tests/$1/$output: ${RED}Failed${NC}:\n"
            cat tests/compilation.log
            echo
            TESTS_FAILED=$(($TESTS_FAILED + 1))
            continue;
        fi;

        printf "Running ./tests/${output}\n\n"
        ./tests/$output
        
        if [ "$?" -eq 0 ]; then
            printf "tests/$1/$output: ${CYAN}Succeed${NC}\n\n"
            TESTS_SUCCEED=$(($TESTS_SUCCEED + 1))
        else
            printf "tests/$1/$output: ${RED}Failed${NC}\n\n"
            TESTS_FAILED=$(($TESTS_FAILED + 1))
        fi
        TESTS_DONE=$(($TESTS_DONE + 1))
    done

    printf "######################################\n\n"
    printf "\n${BBlue}========================END OF $1 TESTS========================${NC}\n\n"
}

if [ $# -eq 0 ]; then
    run_all_tests
else
    run_test $1
fi

if [ "$DEBUG" == "0" ]; then 
    rm tests/*.c
fi

printf "${UBlack}Results:${NC}\n"
echo "Tests done: $TESTS_DONE"
echo "Tests success: $TESTS_SUCCEED"
echo "Tests failed: $TESTS_FAILED"