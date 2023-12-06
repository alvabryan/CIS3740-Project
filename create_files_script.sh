#!/bin/bash

# Define the range of file sizes (number of lines) you want
# For example: 50000 to 500000 lines
min_size=50000
max_size=500000

for n in {1..10};
do 
    # Generate a random size within the specified range
    size=$((RANDOM % (max_size - min_size + 1) + min_size))
    
    # Generate a file of that size
    tr -dc "A-Za-z 0-9" < /dev/urandom | fold -w100 | head -n $size > ./files/file$n.txt
done
