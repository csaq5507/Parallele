ivan@acerlaptopivan:~/CLionProjects/Parallele/cmake-build-release/week_02/matrix_mul$ ./benchmark.sh seq 512 3000
Computing matrix-matrix product with N=512
Total time: 201.201ms
Verification: OK
Computing matrix-matrix product with N=1024
Total time: 2386.379ms
Verification: OK
Computing matrix-matrix product with N=2048
Total time: 79473.728ms
Verification: OK

ivan@acerlaptopivan:~/CLionProjects/Parallele/cmake-build-release/week_02/matrix_mul$ ./benchmark.sh omp 512 3000
Computing matrix-matrix product with N=512
Total time: 170.969ms
Verification: OK
Computing matrix-matrix product with N=1024
Total time: 2026.479ms
Verification: OK
Computing matrix-matrix product with N=2048
Total time: 22345.423ms
Verification: OK

ivan@acerlaptopivan:~/CLionProjects/Parallele/cmake-build-release/week_02/matrix_mul$ ./benchmark.sh ocl 512 3000
Computing matrix-matrix product with N=512
Total time: 193.885ms
Verification: OK
Computing matrix-matrix product with N=1024
Total time: 441.153ms
Verification: OK
Computing matrix-matrix product with N=2048
Total time: 2902.402ms
Verification: OK

ivan@acerlaptopivan:~/CLionProjects/Parallele/cmake-build-release/week_02/matrix_mul$ ./benchmark.sh ocl_v2 512 3000
Computing matrix-matrix product with N=512
Total time: 169.659ms
Verification: OK
Computing matrix-matrix product with N=1024
Total time: 248.851ms
Verification: OK
Computing matrix-matrix product with N=2048
Total time: 924.065ms
Verification: OK
ivan@acerlaptopivan:


# Matrix multiplication
| Dimension | 512 | 1024 | 2048 |
|---|---|---|---|
| Seq | 201 ms | 2386 ms | 79473 ms |
| OMP | 170 ms | 2026 ms | 22345 ms |
| OCL | 193 ms | 441 ms | 2902 ms |
| OC2 | 169 ms | 248 ms | 924 ms |
