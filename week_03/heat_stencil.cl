__kernel void heat_stencil(
    __global float* A,
    __global float* B,
    __global float* C,
    int N
) {
    // obtain position of this 'thread'
    size_t i = get_global_id(0);
    size_t j = get_global_id(1);

    // if beyond boundaries => skip this one
    if (i >= N || j >= N) return;


            if (i == N/4 && j == N/4) {
                 C[i*N+j] = A[i*N+j];
                return;
            }




            // get current temperature at (i,j)
            float tc = A[i*N+j];

            // get temperatures left/right and up/down
            float tl = ( j !=  0  ) ? A[i*N+(j-1)] : tc;
            float tr = ( j != N-1 ) ? A[i*N+(j+1)] : tc;
            float tu = ( i !=  0  ) ? A[(i-1)*N+j] : tc;
            float td = ( i != N-1 ) ? A[(i+1)*N+j] : tc;

            // update temperature at current point
            C[i*N+j] = tc + 0.2 * (tl + tr + tu + td + (-4*tc));

    for(int t=0;t<500;t++){

            // get current temperature at (i,j)
            float tc = C[i*N+j];

            // get temperatures left/right and up/down
            float tl = ( j !=  0  ) ? C[i*N+(j-1)] : tc;
            float tr = ( j != N-1 ) ? C[i*N+(j+1)] : tc;
            float tu = ( i !=  0  ) ? C[(i-1)*N+j] : tc;
            float td = ( i != N-1 ) ? C[(i+1)*N+j] : tc;

            // update temperature at current point
            B[i*N+j] = tc + 0.2 * (tl + tr + tu + td + (-4*tc));


            barrier(CLK_GLOBAL_MEM_FENCE);


            // get current temperature at (i,j)
            float tc1 = B[i*N+j];

            // get temperatures left/right and up/down
            float tl1 = ( j !=  0  ) ? B[i*N+(j-1)] : tc;
            float tr1 = ( j != N-1 ) ? B[i*N+(j+1)] : tc;
            float tu1 = ( i !=  0  ) ? B[(i-1)*N+j] : tc;
            float td1 = ( i != N-1 ) ? B[(i+1)*N+j] : tc;

            // update temperature at current point
            C[i*N+j] = tc1 + 0.2 * (tl1 + tr1 + tu1 + td1 + (-4*tc1));
    }
}
