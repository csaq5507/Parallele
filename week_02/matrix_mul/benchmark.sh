#!/bin/bash
if [ "$1" != "" ]; then
    for ((number=$2;number < $3;number*=2))
    {
    ./week2_$1 $number
    }
else
    echo "usage: ./benchmark.sh [omp|seq|ocl] start_number end_number"
fi

exit 0