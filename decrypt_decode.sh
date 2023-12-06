#!/bin/bash

# Loop through each file in the folder
for n in {1..10}; 
do
    ./encryption/decrypt ./files/file$n.txt.hzip password1 | ./compression/decode
done