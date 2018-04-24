
#define mem(data,i,j) (data)[(i) * N + (j)]
#define mem_loc(data,i,j) (data)[(i) * (local_work+2) + (j)]

typedef float value_t;

__kernel void stenci_loc(
    __global value_t* A,
    __global value_t* B,
    int source_x,
    int source_y,
    int N,
    const int local_work,
   __local value_t* a_loc
   ) {

// obtain position of this 'thread'
    size_t i = get_global_id(0);
    size_t j = get_global_id(1);

	size_t i_loc = get_local_id(0) + 1;
	size_t j_loc = get_local_id(1) + 1;

    if (i<N && j<N && i_loc <= local_work && j_loc <= local_work)
    {
//Read in local mem

        mem_loc(a_loc,i_loc,j_loc) = mem(A,i,j);

        if(i_loc == 1 && i != 0)
            mem_loc(a_loc,0,j_loc) = mem(A,i-1,j);
        else if (i_loc == 1 && i == 0)
            mem_loc(a_loc,0,j_loc) = mem(A,i,j);

        if(j_loc == 1 && j != 0)
            mem_loc(a_loc,i_loc,0) = mem(A,i,j-1);
        else if(j_loc == 1 && j == 0)
            mem_loc(a_loc,i_loc,0) = mem(A,i,j);

        if(i_loc == local_work && i != (N - 1))
            mem_loc(a_loc,i_loc+1,j_loc ) = mem(A,i+1,j);
        else if(i_loc == local_work  && i != (N - 1))
            mem_loc(a_loc,i_loc+1,j_loc ) = mem(A,i,j);

        if(j_loc == local_work && j != (N - 1))
            mem_loc(a_loc,i_loc,j_loc+1) = mem(A,i,j+1);
        else if(j_loc == local_work && j == (N - 1))
            mem_loc(a_loc,i_loc,j_loc+1) = mem(A,i,j);
    }
    barrier(CLK_LOCAL_MEM_FENCE);

    if (i>=N || j>=N || i_loc > local_work || j_loc > local_work)
        return;

//calc point
    if (i == source_x && j == source_y) {
        mem(B,i,j) = mem_loc(a_loc,i_loc,j_loc);
        return;
    }

    // get current temperature at (i,j)
    value_t tc = mem_loc(a_loc,i_loc,j_loc);

    // get temperatures left/right and up/down
    value_t tl = mem_loc(a_loc,i_loc,j_loc-1);
    value_t tr = mem_loc(a_loc,i_loc,j_loc+1);
    value_t tu = mem_loc(a_loc,i_loc-1,j_loc);
    value_t td = mem_loc(a_loc,i_loc+1,j_loc);

    // update temperature at current point
    mem(B,i,j) = tc + 0.2f * (tl + tr + tu + td + (-4.0f*tc));

}
