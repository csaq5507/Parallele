
__kernel void mat_mul(
    __global float* c,
    __global const float* a,
    __global const float* b,
    int N
) {
    // obtain position of this 'thread'
    size_t i = get_global_id(0);

    // if beyond boundaries => skip this one
    if (i >= N) return;

    // compute C := A * B

    for(int j = 0; j<N; j++){
        float sum = 0;
        for(int k=0; k<N; k++){
            sum += a[i * N + k] * b[k * N + j];
        }
        c[i * N + j] = sum;
    }
}
