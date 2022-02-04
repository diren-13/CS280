#!/bin/bash
output=gnu64
echo "Clearing old executables..."
rm $output
echo "Building..."
g++ -std=c++14 -Werror -Wall -Wextra -Wconversion -pedantic -Wno-deprecated -g driver-sample.cpp PRNG.cpp -o $output
echo "Running \"default\" Test Cases..."
printf '1\n' | ./$output > ./output/output64.txt
printf '2\n' | ./$output >> ./output/output64.txt
printf '3\n' | ./$output >> ./output/output64.txt
printf '4\n' | ./$output >> ./output/output64.txt