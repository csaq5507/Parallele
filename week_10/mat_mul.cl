__kernel void mat_mul(
    __global float* c,
    __global const float* a,
    __global const float* b,
    int N
) {
    // obtain position of this 'thread'
    size_t i = get_global_id(1);
    size_t j = get_global_id(0);

    // if beyond boundaries => skip this one
    if (i >= N || j >= N) return;

    // compute C := A * B
    float sum = 0;
    for(int k = 0; k<N; k++) {
        sum += a[i*N+k] * b[k*N+j];
    }
    c[i*N+j] = sum;
}



#define BLOCK_SIZE 32

__kernel void matrix_multiplication_divide_and_conquer(
        __global float* A,
        __global float* B,
        __global float* C,
        unsigned int N,
        unsigned int S)
{
    // Block index
    int bx = get_group_id(0);
    int by = get_group_id(1);

    // Thread index
    int tx = get_local_id(0);
    int ty = get_local_id(1);

    // Global index
    int gx = get_global_id(0);
    int gy = get_global_id(1);

    int aBegin = N * BLOCK_SIZE * by;

    int aEnd = aBegin + S - 1;

    int aStep  = BLOCK_SIZE;

    int bBegin = BLOCK_SIZE * bx;

    int bStep  = BLOCK_SIZE * N;

    int y_idx = 0;

    float Csub = 0;

    for (int a = aBegin, b = bBegin; a <= aEnd; y_idx += BLOCK_SIZE, a += aStep, b += bStep) {

        __local float As[BLOCK_SIZE][BLOCK_SIZE];
        __local float Bs[BLOCK_SIZE][BLOCK_SIZE];

        if((a - aBegin) > N || (y_idx > N))
            break;

        if((a - aBegin) + tx > N)
            As[ty][tx] = 0;
        else
            As[ty][tx] = A[a + N * ty + tx];

        if(y_idx + ty > N)
            Bs[ty][tx] = 0;
        else
            Bs[ty][tx] = B[b + N * ty + tx];

        barrier(CLK_LOCAL_MEM_FENCE);

        for (int k = 0; k < BLOCK_SIZE; ++k) {
            Csub += As[ty][k] * Bs[k][tx];
        }
        barrier(CLK_LOCAL_MEM_FENCE);

    }

    int c = N * BLOCK_SIZE * by + BLOCK_SIZE * bx;
    if(gx < N && gy < N)
        C[c + N * ty + tx] = Csub;
}