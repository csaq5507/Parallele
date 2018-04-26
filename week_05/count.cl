__kernel void count_one(
    __global char* array,
    __local char* scratch,
    __global char* result,
    long length
) {
    int global_index = get_global_id(0);
    int local_index = get_local_id(0);

    // Load data into local memory
    if(global_index < length)
        scratch[local_index] = array[global_index];
    else {
        scratch[local_index] = 0;
    }
    barrier(CLK_LOCAL_MEM_FENCE);

    for(int i = 1; i < get_local_size(0); i = i*2)
    {
        if( (local_index % (i*2)) == 0 )
            scratch[local_index] += scratch[local_index + i];
        barrier(CLK_LOCAL_MEM_FENCE);
    }

    if(local_index == 0) {
        result[get_group_id(0)] = scratch[0];
    }
}