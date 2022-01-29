#!/bin/bash
output=gnu64
output2=gnu32
echo "Clearing old executables..."
rm $output
rm $output2
echo "Building..."
g++ -o $output driver-sample.cpp ObjectAllocator.cpp PRNG.cpp -O -Werror -Wall -Wextra -Wconversion -std=c++14 -pedantic -g
g++ -o $output2 driver-sample.cpp ObjectAllocator.cpp PRNG.cpp -O -Werror -Wall -Wextra -Wconversion -std=c++14 -pedantic -g -m32
echo "Running \"default\" Test Cases..."
printf '0\n' | ./$output > ./output/output64.txt
printf '0\n' | ./$output2 > ./output/output32.txt
