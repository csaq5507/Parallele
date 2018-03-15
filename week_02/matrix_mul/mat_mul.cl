
__kernel void vec_add(
    __global float* C, 
    __global const float* A, 
    __global const float* B,
    int N
) {
    // obtain position of this 'thread'
    size_t i = get_global_id(0);
    
    int j = i%N;
    i = i - j * N;
    

    // if beyond boundaries => skip this one
    if (i >= N) return;
    
    // compute C 
    value_t sum = 0;
    for(long long k = 0; k<N; k++) {
            sum += A[i*N+k] * B[k*N+j];
            }
    C[i*N+j] = sum;
}

    
