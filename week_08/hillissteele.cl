__kernel void hillissteele(
	__global int* input_data,
	__local int* a,
	__local int* b,
	int num_iterations,
	int length
	) {

	int g_id = get_global_id(0);

    if(g_id < length){
        a[g_id] = input_data[g_id];
    }else{
        a[g_id] = 0;
    }

    barrier(CLK_LOCAL_MEM_FENCE);


    int k;
    int temp;

    for(int j=0; j < 3; j++){
        //2 ^ j
        k = (int)pow( (float) 2, (float) j);

        //if global_id < k copy, else add element[global id], element[global_id - k]
        if(g_id < k){
            b[g_id] = a[g_id];
        }else{
            b[g_id] = a[g_id] + a[g_id - k];
        }

        barrier(CLK_LOCAL_MEM_FENCE);

        //Swap buffers TODO: use pointers!
        temp = b[g_id];
        b[g_id] = a[g_id];
        a[g_id] = temp;

        barrier(CLK_LOCAL_MEM_FENCE);
    }

    if(g_id < length){
        input_data[g_id] = a[g_id];
    }
}
