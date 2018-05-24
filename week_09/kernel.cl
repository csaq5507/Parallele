typedef char name_t[32];

typedef struct {
	int age;
	name_t name;
} person_t;

__kernel void create_histogram(
	__global person_t* input_data,
	__global int* output_data,
	__local int* temp,
	__local int* b,
	int n,
	int max_age
	)
{
    int g_id = get_global_id(0);
    int l_id = get_local_id(0);

    if(l_id < max_age)
    {
        temp[l_id] = 0;
    }

    barrier(CLK_LOCAL_MEM_FENCE);

    if(g_id < n)
        atomic_add(&temp[input_data[g_id].age],1);


    barrier(CLK_LOCAL_MEM_FENCE);

    if(l_id < max_age)
        atomic_add(&(output_data[l_id]),temp[l_id]);
}


__kernel void calc_index(
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

    //Downsweep stage 1 --> reduction
    for(int offset=1; offset < length; offset = offset * 2){
        if(((l_id+1)%(offset*2)) == 0){
            a[l_id] = a[l_id] + a[l_id - offset];
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }

    a[length -1] = 0;
    int* temp;

    //Downsweep stage 2
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