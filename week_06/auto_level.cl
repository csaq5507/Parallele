#define mem(c,x,y) (data)[c + (x) * comp + (y * comp * w)]
#define mem_loc(c,x,y) (loc_data)[c + (x) * comp + (y * comp * (w_loc*2))]
#define mem_sum(c,x,y) (sum)[c + (x) * comp + (y * comp * w_loc)]

__kernel void compute_min(
    __global char* data,    //data
    __local char* loc_data,  //scratch
    int w,
    int h,
    int w_loc,
    int h_loc,
    int comp
) {
    int g_x = get_global_id(0);
    int g_y = get_global_id(1);
    int l_x = get_local_id(0);
    int l_y = get_local_id(1);


    for(int i=0; i<comp; i++)
    {
        mem_loc(i,l_x,l_y) = mem(i,g_x,g_y);
        mem_loc(i,l_x + w_loc,l_y) = (g_x + (w / 2) < w)?mem(i,g_x + (w / 2),g_y):255;
        mem_loc(i,l_x,l_y + h_loc) = (g_y + (h / 2) < h)?mem(i,g_x,g_y + (h / 2)):255;
        mem_loc(i,l_x + w_loc,l_y + h_loc) = (g_x + (w / 2) < w)?
                                             (g_y + (h / 2) < h)? mem(i,g_x + (w / 2),g_y + (h / 2)): 255
                                             :255;
    }

    barrier(CLK_LOCAL_MEM_FENCE);

    for(int i=0; i<comp; i++)
    {
        char a = mem_loc(i, l_x, l_y);
        char b = mem_loc(i, l_x + w_loc, g_y);
        char c = mem_loc(i,l_x,l_y + h_loc);
        char d = mem_loc(i,l_x + w_loc,l_y + h_loc);

        if(b < a)
            a = b;
        if(d < c)
            c = d;
        if(c < a)
            a = c;
        mem(i,g_x,g_y) = a;
    }

}



__kernel void compute_max(
    __global char* data,    //data
    __local char* loc_data,  //scratch
    int w,
    int h,
    int w_loc,
    int h_loc,
    int comp
) {
    int g_x = get_global_id(0);
    int g_y = get_global_id(1);
    int l_x = get_local_id(0);
    int l_y = get_local_id(1);
    int gid = get_group_id(0);


    for(int i=0; i<comp; i++)
    {
        mem_loc(i,l_x,l_y) = mem(i,g_x,g_y);
        mem_loc(i,l_x + w_loc,l_y) = (g_x + (w / 2) < w)?mem(i,g_x + (w / 2),g_y):0;
        mem_loc(i,l_x,l_y + h_loc) = (g_y + (h / 2) < h)?mem(i,g_x,g_y + (h / 2)):0;
        mem_loc(i,l_x + w_loc,l_y + h_loc) = (g_x + (w / 2) < w)?
                                             (g_y + (h / 2) < h)? mem(i,g_x + (w / 2),g_y + (h / 2)): 0
                                             :0;
    }

    barrier(CLK_LOCAL_MEM_FENCE);

    for(int i=0; i<comp; i++)
    {
        char a = mem_loc(i, l_x, l_y);
        char b = mem_loc(i, l_x + w_loc, g_y);
        char c = mem_loc(i,l_x,l_y + h_loc);
        char d = mem_loc(i,l_x + w_loc,l_y + h_loc);

        if(b > a)
            a = b;
        if(d > c)
            c = d;
        if(c > a)
            a = c;
        mem(i,g_x,g_y) = a;
    }

}


__kernel void compute_sum(
    __global char* data,    //data
    __local char* loc_data,  //scratch
    int w,
    int h,
    int w_loc,
    int h_loc,
    int comp
) {
    int g_x = get_global_id(0);
    int g_y = get_global_id(1);
    int l_x = get_local_id(0);
    int l_y = get_local_id(1);
    int gid = get_group_id(0);


    for(int i=0; i<comp; i++)
    {
        mem_loc(i,l_x,l_y) = mem(i,g_x,g_y);
        mem_loc(i,l_x + w_loc,l_y) = (g_x + (w / 2) < w)?mem(i,g_x + (w / 2),g_y):0;
        mem_loc(i,l_x,l_y + h_loc) = (g_y + (h / 2) < h)?mem(i,g_x,g_y + (h / 2)):0;
        mem_loc(i,l_x + w_loc,l_y + h_loc) = (g_x + (w / 2) < w)?
                                             (g_y + (h / 2) < h)? mem(i,g_x + (w / 2),g_y + (h / 2)): 0
                                             :0;
    }

    barrier(CLK_LOCAL_MEM_FENCE);

    for(int i=0; i<comp; i++)
    {
        char a = mem_loc(i, l_x, l_y);
        char b = mem_loc(i, l_x + w_loc, g_y);
        char c = mem_loc(i,l_x,l_y + h_loc);
        char d = mem_loc(i,l_x + w_loc,l_y + h_loc);

        mem(i,g_x,g_y) = a + b + c + d;
    }

}