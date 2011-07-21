#!/bin/bash

echo $1 $2 $3 $4 $5
if [ $# -ne 5 ];then
    echo "./make_test <test_number> <type> <number_1> <number_2> <expected result>"
    exit 0
fi
echo $1 $2 $3 $4 $5

NUMBERS="./numbers"

# $1 should be which test number it is
# $2 should be what type of test
# $3 should be input number 1
# $4 should be input number 2
# $5 should be exepcted result
num_1="ascii_big_num$1a$2.txt"
num_2="ascii_big_num$1b$2.txt"
num_expected="ascii_big_num$1e$2.txt"

echo $3 > $NUMBERS/$num_1
echo $4 > $NUMBERS/$num_2
echo $5 > $NUMBERS/$num_expected

