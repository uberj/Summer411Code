#!/bin/bash

NUMBERS="./numbers"
num_mult=2
num_add=7

#run_test <flag> <bn1> <bn2> <expect> <actual>

# Addition Test
for i in {1..7}
do
    num_1=$NUMBERS/"ascii_big_num"$i"aA.txt"
    num_2=$NUMBERS/"ascii_big_num"$i"bA.txt"
    expected=$NUMBERS/"ascii_big_num"$i"eA.txt"
    actual=$NUMBERS/"ascii_big_num"$i"oA.txt"

    ../run -A $num_1 $num_2 $expected $actual
    #../run -A ./numbers/ascii_big_num$iaA.txt ./numbers/ascii_big_num1bA.txt ./numbers/ascii_big_num1oA.txt ./numbers/ascii_big_num1cA.txt
    x=$(diff $expected $actual)
    if [ "$x" != "" ]; then
        echo "Addition test failed"
        echo "Numbers:"
        cat $num_1
        cat $num_2
        echo "Result:"
        cat $actual
        echo "Expected"
        cat $expected
    else
        echo "Addition test passed"
        echo "$(cat $num_1) + $(cat $num_2) = $(cat $actual)"
        echo "+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+="
        rm $actual
    fi
done

# mult Test
type="M"

for i in {1..2}
do
    num_1=$NUMBERS/"ascii_big_num"$i"a$type.txt"
    num_2=$NUMBERS/"ascii_big_num"$i"b$type.txt"
    expected=$NUMBERS/"ascii_big_num"$i"e$type.txt"
    actual=$NUMBERS/"ascii_big_num"$i"o$type.txt"
    
    # RUN ACTUAL TEST
    ../run -$type $num_1 $num_2 $expected $actual

    # PARSE AND ANALYS THE RESULTS
    x=$(diff $expected $actual)
    if [ "$x" != "" ]; then
        echo "Multiplication test failed"
        echo "Numbers:"
        cat $num_1
        cat $num_2
        echo "Result:"
        cat $actual
        echo "Expected"
        cat $expected
    else
        echo "Multiplication test passed"
        echo "$(cat $num_1) * $(cat $num_2) = $(cat $actual)"
        echo "+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+="
    #    rm $actual
    fi
done
