__kernel void downsweep(
	__global int* input_data,
	__local int* a,
	__local int* b,
	int num_iterations,
	int length
	) {

	int l_id = get_local_id(0);

    //Load data into local memory
    if(l_id < length){
        a[l_id] = input_data[l_id];
    }else{
        a[l_id] = 0;
    }

    barrier(CLK_LOCAL_MEM_FENCE);

    for(int offset=1; offset < length; offset = offset * 2){
        if(((l_id+1)%(offset*2)) == 0){
            a[l_id] = a[l_id] + a[l_id - offset];
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }

    a[length -1] = 0;
    int* temp;

    for(int offset=length; offset>1; offset = offset/2){
        if(((l_id+1)%(offset)) == 0){
            temp = a[l_id];
            a[l_id] = a[l_id] + a[l_id - offset/2];
            a[l_id - offset/2] = temp;
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }

    if(l_id < length){
        input_data[l_id] = a[l_id];
    }
}
