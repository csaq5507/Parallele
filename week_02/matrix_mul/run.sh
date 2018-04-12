#!/usr/bin/env bash


for num_samples in {256..1200..16}
do
    for trials in 1 2 3
    do
    ./mat_mul_seq $num_samples >> result.txt
    done
done

