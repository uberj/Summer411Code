#!/bin/bash

sed 's/unsigned long int/uint32_t/g' $1 >> temp
rm -f $1
mv temp $1

sed 's/long unsigned int/uint32_t/g' $1 >> temp
rm -f $1
mv temp $1
