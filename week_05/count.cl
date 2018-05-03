
__kernel void count_one(
    __global int* array,
    __local int* scratch,
    long length,
    long group_size
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

    for(int offset = get_local_size(0) / 2; offset > 0; offset = offset / 2) {
        if (local_index < offset) {
            scratch[local_index] += scratch[local_index + offset];
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }

    if(local_index == 0) {
        array[get_group_id(0)] = scratch[0];
    }


}