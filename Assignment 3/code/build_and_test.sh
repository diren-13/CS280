#!/bin/bash
output=gnu
echo "Clearing old executables..."
rm $output
echo "Building..."
g++ -std=c++14 -Werror -Wall -Wextra -Wconversion -pedantic -Wno-deprecated -g driver-sample.cpp ObjectAllocator.cpp PRNG.cpp -o $output
echo "Running \"default\" Test Cases..."
printf '0\n' | ./$output > ../output/myout_bf_counts.txt