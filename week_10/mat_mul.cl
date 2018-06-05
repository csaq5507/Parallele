__kernel void mat_mul(
    __global const float* a,
    __global const float* b,
    __global float* c,
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
        __global const float* A,
        __global const float* B,
        __global float* C,
        unsigned int N)
{

    __local float A_loc[BLOCK_SIZE*BLOCK_SIZE]__attribute__ ((aligned(BLOCK_SIZE)));
    __local float B_loc[BLOCK_SIZE*BLOCK_SIZE]__attribute__ ((aligned(BLOCK_SIZE)));

    __local float A_loc1[BLOCK_SIZE*BLOCK_SIZE]__attribute__ ((aligned(BLOCK_SIZE)));
    __local float B_loc1[BLOCK_SIZE*BLOCK_SIZE]__attribute__ ((aligned(BLOCK_SIZE)));

    __local float A_loc2[BLOCK_SIZE*BLOCK_SIZE]__attribute__ ((aligned(BLOCK_SIZE)));
    __local float B_loc2[BLOCK_SIZE*BLOCK_SIZE]__attribute__ ((aligned(BLOCK_SIZE)));

    __local float A_loc3[BLOCK_SIZE*BLOCK_SIZE]__attribute__ ((aligned(BLOCK_SIZE)));
    __local float B_loc3[BLOCK_SIZE*BLOCK_SIZE]__attribute__ ((aligned(BLOCK_SIZE)));

    // obtain position of this 'thread'
    size_t x = get_global_id(0);
    size_t y = get_global_id(1);

    // obtain position of this 'thread' local
    size_t x_loc = get_local_id(0);
    size_t y_loc = get_local_id(1);

    // obtain group index of this 'thread'
    size_t x_group = get_group_id(0);
    size_t y_group = get_group_id(1);

    size_t loc_idx = y_loc * BLOCK_SIZE + x_loc;

	size_t glob_idx_a = (N * BLOCK_SIZE * y_group) + (y_loc * N) + x_loc;
	size_t glob_idx_b = (BLOCK_SIZE * x_group) + (y_loc * N) + x_loc;
	
    float result = 0;

    for (int i=0; i < N; i += (BLOCK_SIZE*2)) {

        if((i + x_loc) >= N)
            A_loc[loc_idx] = 0;
        else
            A_loc[loc_idx] = A[glob_idx_a + i];

        if((i + y_loc) >= N)
            B_loc[loc_idx] = 0;
        else
            B_loc[loc_idx] = B[glob_idx_b + (i*N)];

        barrier(CLK_LOCAL_MEM_FENCE);
        
        int a_idx = y_loc*BLOCK_SIZE;
        #pragma unroll
        for (int j = 0; j < BLOCK_SIZE; ++j) {
            result += A_loc[a_idx + j] * B_loc[j * BLOCK_SIZE + x_loc];
        }
        barrier(CLK_LOCAL_MEM_FENCE);

    }

    if(x < N && y < N)
        C[y * N + x] = result;
}


__kernel void matrix_multiplication_divide_and_conquer_already_filled(
        __global float* A,
        __global float* B,
        __global float* C,
        unsigned int N)
{

    __local float A_loc[BLOCK_SIZE*BLOCK_SIZE]__attribute__ ((aligned(BLOCK_SIZE)));
    __local float B_loc[BLOCK_SIZE*BLOCK_SIZE]__attribute__ ((aligned(BLOCK_SIZE)));

    __local float A_loc1[BLOCK_SIZE*BLOCK_SIZE]__attribute__ ((aligned(BLOCK_SIZE)));
    __local float B_loc1[BLOCK_SIZE*BLOCK_SIZE]__attribute__ ((aligned(BLOCK_SIZE)));



    // obtain position of this 'thread'
    size_t x = get_global_id(0);
    size_t y = get_global_id(1);

    // obtain position of this 'thread' local
    size_t x_loc = get_local_id(0);
    size_t y_loc = get_local_id(1);

    // obtain group index of this 'thread'
    size_t x_group = get_group_id(0);
    size_t y_group = get_group_id(1);

    size_t loc_idx = y_loc * BLOCK_SIZE + x_loc;

	size_t glob_idx_a = (N * BLOCK_SIZE * y_group) + (y_loc * N) + x_loc;
	size_t glob_idx_b = (BLOCK_SIZE * x_group) + (y_loc * N) + x_loc;
	
    float result = 0.0;
    int i;
    for (i=0; i < N - BLOCK_SIZE; i += (BLOCK_SIZE*2)) {

        A_loc[loc_idx] = A[glob_idx_a + i];

        B_loc[loc_idx] = B[glob_idx_b + (i*N)];

        A_loc1[loc_idx] = A[glob_idx_a + i + BLOCK_SIZE];

        B_loc1[loc_idx] = B[glob_idx_b + ((i+BLOCK_SIZE)*N)];

        barrier(CLK_LOCAL_MEM_FENCE);

        int a_idx = y_loc*BLOCK_SIZE;

        #pragma unroll
        for (int j = 0; j < BLOCK_SIZE; ++j) {
            result += A_loc[a_idx + j] * B_loc[j * BLOCK_SIZE + x_loc];
        }

        #pragma unroll
        for (int j = 0; j < BLOCK_SIZE; ++j) {
            result += A_loc1[a_idx + j] * B_loc1[j * BLOCK_SIZE + x_loc];
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }
    if (i<N) {
        A_loc[loc_idx] = A[glob_idx_a + i];

        B_loc[loc_idx] = B[glob_idx_b + (i*N)];
        barrier(CLK_LOCAL_MEM_FENCE);

        int a_idx = y_loc*BLOCK_SIZE;

        #pragma unroll
        for (int j = 0; j < BLOCK_SIZE; ++j) {
            result += A_loc[a_idx + j] * B_loc[j * BLOCK_SIZE + x_loc];
        }
    }

    C[y * N + x] = result;
}
