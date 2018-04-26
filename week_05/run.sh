#!/usr/bin/env bash


for num_samples in 8 16 32 64 128 256
do
    for trials in 1 2 3
    do
    ./count_ones $num_samples"m" >> result.txt
    done
done

