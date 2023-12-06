#!/bin/bash

# Path to the folder
folder_path="./files"

# Loop through each file in the folder
for file in "$folder_path"/*; do
    # Check if it's a file
    if [ -f "$file" ]; then
        echo $file | ./encryption/encrypt password1
    fi
done