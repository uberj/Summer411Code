#!/bin/bash

NUMBERS="./numbers"
num_mult=7
num_add=7
num_div=6

#run_test <flag> <bn1> <bn2> <expect> <actual>

# Addition Test
for (( i=1; i <= $num_add; i++ ))
do
    num_1=$NUMBERS/"ascii_big_num"$i"aA.txt"
    num_2=$NUMBERS/"ascii_big_num"$i"bA.txt"
    expected=$NUMBERS/"ascii_big_num"$i"eA.txt"
    actual=$NUMBERS/"ascii_big_num"$i"oA.txt"

    ../run -A $num_1 $num_2 $expected $actual
    #../run -A ./numbers/ascii_big_num$iaA.txt ./numbers/ascii_big_num1bA.txt ./numbers/ascii_big_num1oA.txt ./numbers/ascii_big_num1cA.txt
    x=$(diff $expected $actual)
    if [ "$x" != "" ]; then
        echo "Addition test $i failed"
        echo "Numbers:"
        cat $num_1
        cat $num_2
        echo "Result:"
        cat $actual
        echo "Expected"
        cat $expected
    else
        echo "Addition test $i passed"
        echo "$(cat $num_1) + $(cat $num_2) = $(cat $actual)"
        echo "+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+="
        rm $actual
    fi
done

# mult Test
type="M"

for (( i=1; i <= $num_mult; i++ ))
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
        echo "Multiplication test $i failed"
        echo "Numbers:"
        cat $num_1
        cat $num_2
        echo "Result:"
        cat $actual
        echo "Expected"
        cat $expected
    else
        echo "Multiplication test $i passed"
        echo "$(cat $num_1) * $(cat $num_2) = $(cat $actual)"
        echo "+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+="
        rm $actual
    fi
done

# Fast division Test
type="D"

for (( i=1; i <= $num_div; i++ ))
do
    num_1=$NUMBERS/"ascii_big_num"$i"a$type.txt"
    num_2=$NUMBERS/"ascii_big_num"$i"b$type.txt"
    expected_Q=$NUMBERS/"ascii_big_num"$i"eq$type.txt"
    expected_R=$NUMBERS/"ascii_big_num"$i"er$type.txt"
    actual=$NUMBERS/"ascii_big_num"$i"q$type.txt"
    remain=$NUMBERS/"ascii_big_num"$i"r$type.txt"

    # RUN ACTUAL TEST
    ../run -$type $num_1 $num_2 $expected_Q $expected_R $actual $remain
   # echo "-$type $num_1 $num_2 $expected_Q $expected_R $actual $remain"

    # PARSE AND ANALYS THE RESULTS
    x=$(diff $expected_Q $actual)
    x=$(diff $expected_R $remain)
    if [ "$x" != "" ]; then
        echo "Division test $i failed"
        echo "Numbers:"
        cat $num_1
        cat $num_2
        echo "Result:"
        cat $actual
        cat $remain
        echo "Expected"
        cat $expected_Q
        cat $expected_R
    else
        echo "Division test $i passed"
        echo "$(cat $num_1) / $(cat $num_2) = $(cat $actual), r: $(cat $remain)"
        echo "+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+="
        rm $actual
    fi
done
