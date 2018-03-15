
__kernel void mat_mul(
    __global float* c,
    __global const float* a,
    __global const float* b,
    int N
) {
    // obtain position of this 'thread'
    size_t i = get_global_id(0);

    // if beyond boundaries => skip this one
    if (i >= N * N) return;

    // compute C := A * B

    int column = i % N;
    int row = i / N;

    float sum = 0;
    for(int k=0; k<N; k++){
        sum += a[row * N + k] * b[k * N + column];
    }
    c[i] = sum;
}
