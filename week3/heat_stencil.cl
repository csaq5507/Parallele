__kernel void heat_stencil(
    __global float* c,
    __global const float* a,
    __global const float* b,
    int N
) {
    // obtain position of this 'thread'
    size_t i = get_global_id(0);
    size_t j = get_global_id(1);

    // if beyond boundaries => skip this one
    if (i >= N || j >= N) return;


 // center stays constant (the heat is still on)
            if (i == source_x && j == source_y) {
                B[i*N+j] = A[i*N+j];
                continue;
            }

            // get current temperature at (i,j)
            value_t tc = A[i*N+j];

            // get temperatures left/right and up/down
            float tl = ( j !=  0  ) ? A[i*N+(j-1)] : tc;
            float tr = ( j != N-1 ) ? A[i*N+(j+1)] : tc;
            float tu = ( i !=  0  ) ? A[(i-1)*N+j] : tc;
            float td = ( i != N-1 ) ? A[(i+1)*N+j] : tc;

            // update temperature at current point
            B[i*N+j] = tc + 0.2 * (tl + tr + tu + td + (-4*tc));
}
