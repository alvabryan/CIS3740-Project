#!/bin/bash

for n in {1..10};
do
    ./compression/encode ./files/file$n.txt | ./encryption/encrypt password1
done