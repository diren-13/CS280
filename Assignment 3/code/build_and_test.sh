#!/bin/bash
output=gnu
echo "Clearing old executables..."
rm $output
echo "Building..."
g++ -std=c++14 -Werror -Wall -Wextra -Wconversion -pedantic -Wno-deprecated -g driver-sample.cpp PRNG.cpp -o $output
echo "Running \"default\" Test Cases..."
printf '1\n' | ./$output > ../output/nobf.txt