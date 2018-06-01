__kernel void mat_mul(
    __global float* c,
    __global const float* a,
    __global const float* b,
    __local float * loc_a,
    __local float * loc_b,
    int N,
    int loc_size
) {
    // obtain position of this 'thread'
    size_t i = get_global_id(1);
    size_t j = get_global_id(0);

    size_t i_loc = get_global_id(1);
    size_t j_loc = get_global_id(0);

    // if beyond boundaries => skip this one
    if (i < N && j < N)
    {

    }

    // compute C := A * B
    float sum = 0;
    for(int k = 0; k<N; k++) {
        sum += a[i*N+k] * b[k*N+j];
    }
    c[i*N+j] = sum;
}