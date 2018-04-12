#!/usr/bin/env bash

for num_samples in {256..1200..16}
do
    for cores in 1 2 4 8
    do
        export OMP_NUM_THREADS=$cores
        for trial in 1 2 3
        do
        ./mat_mul_omp $num_samples >> result_omp.txt
        done
    done
done