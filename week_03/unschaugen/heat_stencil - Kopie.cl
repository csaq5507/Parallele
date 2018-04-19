
typedef float value_t;

__kernel void stencil(
    __global const value_t* A, 
    __global value_t* B,
    int source_x,
    int source_y,
    int N,
	__local value_t* tmp
) {

	
    // obtain position of this 'thread'
    size_t i = get_global_id(1);
    size_t j = get_global_id(0);
	size_t k = get_local_id(1);
	size_t l = get_local_id(0);
	
	tmp[k*N+l]=A[i*N+j];
	barrier(CLK_LOCAL_MEM_FENCE);
    // center stays constant (the heat is still on)
    if (i == source_x && j == source_y) {
        B[i*N+j] = tmp[k*N+l];
        return;
    }

    // get current temperature at (i,j)
    value_t tc = tmp[i*N+j];

    // get temperatures left/right and up/down
    value_t tl = ( j !=  0  ) ? tmp[k*N+(l-1)] : tc;
    value_t tr = ( j != N-1 ) ? tmp[i*N+(l+1)] : tc;
    value_t tu = ( i !=  0  ) ? tmp[(k-1)*N+l] : tc;
    value_t td = ( i != N-1 ) ? tmp[(k+1)*N+l] : tc;

    // update temperature at current point
    B[i*N+j] = tc + 0.2f * (tl + tr + tu + td + (-4.0f*tc));

}
