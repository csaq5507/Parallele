
#define mem(data,i,j) (data)[(i) * N + (j)]

typedef float value_t;

__kernel void stenci_loc(
    __global const value_t* A,
    __global value_t* B,
    int source_x,
    int source_y,
    int N,
    int local_work
) {


    // obtain position of this 'thread'
    size_t i = get_global_id(0);
    size_t j = get_global_id(1);

	size_t i_loc = get_local_id(0) + 1;
	size_t j_loc = get_local_id(1) + 1;


    __local value_t a_loc[(local_work + 2) * (local_work + 2)];
    __local value_t b_loc[(local_work + 2) * (local_work + 2)];

    mem(a_loc,i_loc,j_loc) = mem(A,i,j);
    mem(b_loc,i_loc,j_loc) = mem(B,i,j);

    if(i_loc == 0 && i != 0)
    {
        mem(a_loc,i_loc-1,j_loc) = mem(A,i-1,j);
        mem(b_loc,i_loc-1,j_loc) = mem(B,i-1,j);
    }
    if(j_loc == 0 && j != 0)
    {
        mem(a_loc,i_loc,j_loc-1) = mem(A,i,j-1);
        mem(b_loc,i_loc,j_loc-1) = mem(B,i,j-1);
    }

    if(i_loc == (local_work - 1) && i != (N - 1))
    {
        mem(a_loc,i_loc+1,j_loc) = mem(A,i+1,j);
        mem(b_loc,i_loc+1,j_loc) = mem(B,i+1,j);
    }
    if(j_loc == (local_work - 1) && j != (N - 1))
    {
        mem(a_loc,i_loc,j_loc+1) = mem(A,i,j+1);
        mem(b_loc,i_loc,j_loc+1) = mem(B,i,j+1);
    }
    barrier(CLK_LOCAL_MEM_FENCE);

    if (i == source_x && j == source_y) {
        mem(b_loc,i_loc,j_loc) = mem(a_loc,i_loc,j_loc);
        return;
    }

    // get current temperature at (i,j)
    value_t tc = mem(a_loc,i_loc,j_loc);

    // get temperatures left/right and up/down
    value_t tl = ( j !=  0  ) ? mem(a_loc,i_loc,j_loc-1) : tc;
    value_t tr = ( j != N-1 ) ? mem(a_loc,i_loc-1,j_loc) : tc;
    value_t tu = ( i !=  0  ) ? mem(a_loc,i_loc,j_loc+1) : tc;
    value_t td = ( i != N-1 ) ? mem(a_loc,i_loc+1,j_loc) : tc;

    // update temperature at current point
    mem(b_loc,i_loc,j_loc) = tc + 0.2f * (tl + tr + tu + td + (-4.0f*tc));


}
