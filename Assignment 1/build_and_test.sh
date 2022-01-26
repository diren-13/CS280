#!/bin/bash
output=gnu
echo "Clearing old executable..."
rm $output
echo "Building..."
g++ -o $output driver-sample.cpp ObjectAllocator.cpp PRNG.cpp -O -Werror -Wall -Wextra -Wconversion -std=c++14 -pedantic -g
echo "Running \"default\" Test Cases..."
printf '0\n' | ./$output > ../output/output-zeros-ec-LP64.txt
