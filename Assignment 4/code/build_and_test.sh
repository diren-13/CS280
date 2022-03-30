#!/bin/bash
output=gnu
echo "Clearing old executables..."
rm $output
echo "Building..."
g++ -std=c++14 -Werror -Wall -Wextra -Wconversion -pedantic -Wno-deprecated -g driver-sample.cpp ALGraph.cpp -o $output
echo "Running \"default\" Test Cases..."
printf '0\n'    |   ./$output > ./output/myout-all.txt
# printf '1\n'    |   ./$output > ./output/myout-D0-1.txt
# printf '2\n'    |   ./$output > ./output/myout-D1-1.txt
# printf '3\n'    |   ./$output > ./output/myout-D4-1.txt
# printf '4\n'    |   ./$output > ./output/myout-D4a-1.txt
# printf '5\n'    |   ./$output > ./output/myout-D5-1.txt
# printf '6\n'    |   ./$output > ./output/myout-D9-1.txt
# printf '7\n'    |   ./$output > ./output/myout-D10-1.txt
# printf '8\n'    |   ./$output > ./output/myout-D10-14.txt
# printf '9\n'    |   ./$output > ./output/myout-D10-all.txt
# printf '10\n'   |   ./$output > ./output/myout-Big-10x100.txt
# printf '11\n'   |   ./$output > ./output/myout-Big-30x10.txt
# printf '12\n'   |   ./$output > ./output/myout-Big-99x2.txt