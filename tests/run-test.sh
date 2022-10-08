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
MATRIX_FILES="src/utils/matrix.c"
NN_FILES="src/NeuralNetwork/neuralNet.c"

TESTS_DONE=0
TESTS_SUCCEED=0
TESTS_FAILED=0

run_all_tests() {
    echo 'RUNNING ALL TESTS.';
    run_test matrix
    run_test neuralnet
}

run_test() {
    printf "\n${BBlue}=======================RUNNING $1 TESTS=======================${NC}\n\n";

    for file in `ls tests/$1`; do
        if [ "$1" = "matrix" ]; then
            ${CC} ${CFLAGS} -g "tests/$1/$file" ${MATRIX_FILES} -o $file
        elif [ $1 = "neuralnet" ]; then
            ${CC} ${CFLAGS} -g "tests/$1/$file" ${MATRIX_FILES} ${NN_FILES} -o $file
        else
            echo "Bad arg: $1"
            exit 1
        fi

        ./$file
        if [ "$?" -eq 0 ]; then
            printf "tests/$1/$file: ${CYAN}Succeed${NC}\n"
            TESTS_SUCCEED=$(($TESTS_SUCCEED + 1))
        else
            printf "tests/$1/$file: ${RED}Failed${NC}\n"
            TESTS_FAILED=$(($TESTS_FAILED + 1))
        fi
        TESTS_DONE=$(($TESTS_DONE + 1))
        # rm test
    done
    printf "\n${BBlue}========================END OF $1 TESTS========================${NC}\n\n"
}

if [ $# -eq 0 ]; then
    run_all_tests
else
    run_test $1
fi

printf "${UBlack}Results:${NC}\n"
echo "Tests done: $TESTS_DONE"
echo "Tests success: $TESTS_SUCCEED"
echo "Tests failed: $TESTS_FAILED"