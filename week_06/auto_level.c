#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "cl_utils.h"
#include "stb/image.h"
#include "stb/image_write.h"

#define DEBUG 0

double auto_level_original(char * in, char * out)
{

    // load input file
    if(DEBUG)
        printf("Loading input file %s ..\n", in);
    int width, height, components;
    unsigned char *data = stbi_load(in, &width, &height, &components, 0);
    if(DEBUG)
        printf("Loaded image of size %dx%d with %d components.\n", width,height,components);

    // start the timer
    double start_time = now();

    // ------ Analyse Image ------

    // compute min/max/avg of each component
    unsigned char min_val[components];
    unsigned char max_val[components];
    unsigned char avg_val[components];

    // an auxilary array for computing the average
    unsigned long long sum[components];

    // initialize
    for(int c = 0; c<components; c++) {
        min_val[c] = 255;
        max_val[c] = 0;
        sum[c] = 0;
    }

    // compute min/max/sub

    for(int x=0; x<width; ++x) {
        for(int y=0; y<height; ++y) {
            for(int c=0; c<components; ++c) {
                unsigned char val = data[c + x*components + y*width*components];
                if (val < min_val[c]) min_val[c] = val;
                if (val > max_val[c]) max_val[c] = val;
                sum[c] += val;
            }
        }
    }
    sum[0]+=sum[0];
    // compute average and multiplicative factors
    float min_fac[components];
    float max_fac[components];
    for(int c=0; c<components; ++c) {
        avg_val[c] = sum[c]/((unsigned long long)width*height);
        min_fac[c] = (float)avg_val[c] / (float)(avg_val[c] - min_val[c]);
        max_fac[c] = (255.0f-(float)avg_val[c]) / (float)(max_val[c] - avg_val[c]);
        if(DEBUG)
            printf("\tComponent %1u: %3u / %3u / %3u * %3.2f / %3.2f\n", c, min_val[c], avg_val[c], max_val[c], min_fac[c], max_fac[c]);
    }

    // ------ Adjust Image ------

    for(int x=0; x<width; ++x) {
        for(int y=0; y<height; ++y) {
            for(int c=0; c<components; ++c) {
                int index = c + x*components + y*width*components;
                unsigned char val = data[index];
                float v = (float)(val - avg_val[c]);
                v *= (val < avg_val[c]) ? min_fac[c] : max_fac[c];
                data[index] = (unsigned char)(v + avg_val[c]);
            }
        }
    }

    if(DEBUG)
        printf("Done, took %.1fms\n", (now() - start_time)*1000.0);
    double time = (now() - start_time)*1000.0;
    // ------ Store Image ------

    if(DEBUG)
        printf("Writing output image %s ...\n", out);

    char temp[80];
    strcpy(temp,out);
    strcat(temp,".original.png");
    stbi_write_png(temp,width,height,components,data,width*components);
    stbi_image_free(data);

    if(DEBUG)
        printf("Done!\n");
    return time;
}

double auto_level_seq(char * in, char * out)
{

    // load input file
    if(DEBUG)
        printf("Loading input file %s ..\n", in);
    int width, height, components;
    unsigned char *data = stbi_load(in, &width, &height, &components, 0);
    if(DEBUG)
        printf("Loaded image of size %dx%d with %d components.\n", width,height,components);

    // start the timer
    double start_time = now();

    // ------ Analyse Image ------

    // compute min/max/avg of each component
    unsigned char min_val[components];
    unsigned char max_val[components];
    unsigned char avg_val[components];

    // an auxilary array for computing the average
    unsigned long long sum[components];

    // initialize
    for(int c = 0; c<components; c++) {
        min_val[c] = 255;
        max_val[c] = 0;
        sum[c] = 0;
    }

    // compute min/max/sub

//*******************************************************
    // Sequentially optimized by loop order ( cache misses )
    for(int y=0; y<height; ++y) {
        for(int x=0; x<width; ++x) {
            for(int c=0; c<components; ++c) {
                unsigned char val = data[c + x*components + y*width*components];
                if (val < min_val[c]) min_val[c] = val;
                if (val > max_val[c]) max_val[c] = val;
                sum[c] += val;
            }
        }
    }
    sum[0]+=sum[0];
    // compute average and multiplicative factors
    float min_fac[components];
    float max_fac[components];
    for(int c=0; c<components; ++c) {
        avg_val[c] = sum[c]/((unsigned long long)width*height);
        min_fac[c] = (float)avg_val[c] / (float)(avg_val[c] - min_val[c]);
        max_fac[c] = (255.0f-(float)avg_val[c]) / (float)(max_val[c] - avg_val[c]);
        if(DEBUG)
            printf("\tComponent %1u: %3u / %3u / %3u * %3.2f / %3.2f\n", c, min_val[c], avg_val[c], max_val[c], min_fac[c], max_fac[c]);
    }

    // ------ Adjust Image ------

//*******************************************************
    // Sequentially optimized by loop order ( cache misses )
    for(int y=0; y<height; ++y) {
        for(int x=0; x<width; ++x) {
            for(int c=0; c<components; ++c) {
                int index = c + x*components + y*width*components;
                unsigned char val = data[index];
                float v = (float)(val - avg_val[c]);
                v *= (val < avg_val[c]) ? min_fac[c] : max_fac[c];
                data[index] = (unsigned char)(v + avg_val[c]);
            }
        }
    }

    if(DEBUG)
        printf("Done, took %.1fms\n", (now() - start_time)*1000.0);
    double time = (now() - start_time)*1000.0;

    // ------ Store Image ------

    if(DEBUG)
        printf("Writing output image %s ...\n", out);

    char temp[80];
    strcpy(temp,out);
    strcat(temp,".seq_opt.png");
    stbi_write_png(temp,width,height,components,data,width*components);
    stbi_image_free(data);

    if(DEBUG)
        printf("Done!\n");
    return time;
}

double auto_level_omp(char * in, char * out)
{

    // load input file
    if(DEBUG)
        printf("Loading input file %s ..\n", in);
    int width, height, components;
    unsigned char *data = stbi_load(in, &width, &height, &components, 0);
    if(DEBUG)
        printf("Loaded image of size %dx%d with %d components.\n", width,height,components);

    // start the timer
    double start_time = now();

    // ------ Analyse Image ------

    // compute min/max/avg of each component
    unsigned char min_val[components];
    unsigned char max_val[components];
    unsigned char avg_val[components];

    // an auxilary array for computing the average
    unsigned long long sum[components];

    // initialize
    for(int c = 0; c<components; c++) {
        min_val[c] = 255;
        max_val[c] = 0;
        sum[c] = 0;
    }

    // compute min/max/sub

    for(int c=0; c<components; ++c) {
#pragma omp parallel for reduction(+:sum[c]) reduction(max:max_val[c]) reduction(min:min_val[c])
        for(int y=0; y<height; ++y) {
            for(int x=0; x<width; ++x) {
                unsigned char val = data[c + x*components + y*width*components];
                if (val < min_val[c])
                    min_val[c] = val;
                if (val > max_val[c])
                    max_val[c] = val;
                sum[c] += val;
            }
        }
    }
    sum[0]+=sum[0];
    // compute average and multiplicative factors
    float min_fac[components];
    float max_fac[components];
    for(int c=0; c<components; ++c) {
        avg_val[c] = sum[c]/((unsigned long long)width*height);
        min_fac[c] = (float)avg_val[c] / (float)(avg_val[c] - min_val[c]);
        max_fac[c] = (255.0f-(float)avg_val[c]) / (float)(max_val[c] - avg_val[c]);
        if(DEBUG)
            printf("\tComponent %1u: %3u / %3u / %3u * %3.2f / %3.2f\n", c, min_val[c], avg_val[c], max_val[c], min_fac[c], max_fac[c]);
    }

    // ------ Adjust Image ------

#pragma omp parallel for schedule(static,128)
    for(int y=0; y<height; ++y) {
        for(int x=0; x<width; ++x) {
            for(int c=0; c<components; ++c) {
                int index = c + x*components + y*width*components;
                unsigned char val = data[index];
                float v = (float)(val - avg_val[c]);
                v *= (val < avg_val[c]) ? min_fac[c] : max_fac[c];
                data[index] = (unsigned char)(v + avg_val[c]);
            }
        }
    }

    if(DEBUG)
        printf("Done, took %.1fms\n", (now() - start_time)*1000.0);

    double time = (now() - start_time)*1000.0;
    // ------ Store Image ------

    if(DEBUG)
        printf("Writing output image %s ...\n", out);
    char temp[80];
    strcpy(temp,out);
    strcat(temp,".omp.png");
    stbi_write_png(temp,width,height,components,data,width*components);
    stbi_image_free(data);

    if(DEBUG)
        printf("Done!\n");
    return time;
}

double kernel_nanoseconds;

double auto_level_ocl(char * in, char * out)
{
    // load input file
    if(DEBUG)
        printf("Loading input file %s ..\n", in);
    int width, height, components;
    unsigned char *data = stbi_load(in, &width, &height, &components, 0);
    if(DEBUG)
        printf("Loaded image of size %dx%d with %d components.\n", width,height,components);

    // start the timer
    double start_time = now();

    // ------ Analyse Image ------

    // compute min/max/avg of each component
    unsigned char min_val[components];
    unsigned char max_val[components];
    unsigned char avg_val[components];

    // an auxilary array for computing the average
    unsigned long sum[components];

    cl_event kernel_execution_event;

    const int local_x_size = 128;
    const int local_y_size = 128;

    int local_groups = ( % local_work_size != 0)? (N / local_work_size) + 1 : N / local_work_size;
    int global_work_size = local_groups * local_work_size;

    cl_context context;
    cl_command_queue command_queue;
    cl_device_id device_id = cluInitDevice(0, &context, &command_queue);

    cl_int err;
    cl_mem dev_array = clCreateBuffer(context, CL_MEM_READ_WRITE , N * sizeof(int), NULL, &err);
    CLU_ERRCHECK(err, "Failed to create buffer for bytearray");

    // Part 3: fill memory buffers (transfering A is enough, B can be anything)
    err = clEnqueueWriteBuffer(command_queue, dev_array, CL_TRUE, 0, N  * sizeof(int), array, 0, NULL,  NULL);
    CLU_ERRCHECK(err, "Failed to fill buffer");

    // Part 4: create kernel from source
    cl_program program = cluBuildProgramFromFile(context, device_id, "count.cl", NULL);
    cl_kernel kernel = clCreateKernel(program, "count_one", &err);
    CLU_ERRCHECK(err, "Failed to create kernel from program");

    // Part 5: set arguments in kernel (those which are constant)
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &dev_array);
    clSetKernelArg(kernel, 1, sizeof(int) * local_work_size, NULL);

    // enqeue a kernel call for the current time step
    size_t local_size[1] = {local_work_size}; // two dimensional range
    kernel_nanoseconds = 0.0;
    while(true) {


        clSetKernelArg(kernel, 2, sizeof(long), &N);
        clSetKernelArg(kernel, 3, sizeof(long), &local_groups);
        size_t size[1] = {global_work_size}; // two dimensional range

        CLU_ERRCHECK(clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, size, local_size, 0, NULL,
                                            &kernel_execution_event), "Failed to enqueue 1D kernel");

        if(local_groups < 2)
            break;
        N = local_groups;
        local_groups = ((local_groups/2) % local_work_size != 0)? ((local_groups/2) / local_work_size) + 1 : (local_groups/2) / local_work_size;
        global_work_size = local_groups * local_work_size;
        // evaluate events
        cl_ulong time_start;
        cl_ulong time_end;

        // kernel
        clWaitForEvents(1, &kernel_execution_event);

        clGetEventProfilingInfo(kernel_execution_event,
                                CL_PROFILING_COMMAND_START, sizeof(time_start),
                                &time_start, NULL);
        clGetEventProfilingInfo(kernel_execution_event,
                                CL_PROFILING_COMMAND_END, sizeof(time_end),
                                &time_end, NULL);

        kernel_nanoseconds+=(time_end - time_start);
    }

    err = clEnqueueReadBuffer(command_queue, dev_array, CL_TRUE, 0, sizeof(int), array, 0, NULL, NULL);
    CLU_ERRCHECK(err, "Failed to read counter from device");
    // Part 7: cleanup
    // wait for completed operations
    CLU_ERRCHECK(clFlush(command_queue),    "Failed to flush command queue");
    CLU_ERRCHECK(clFinish(command_queue),   "Failed to wait for command queue completion");
    CLU_ERRCHECK(clReleaseKernel(kernel),   "Failed to release kernel");
    CLU_ERRCHECK(clReleaseProgram(program), "Failed to release program");

    // free device memory
    CLU_ERRCHECK(clReleaseMemObject(dev_array), "Failed to release Matrix A");

    // free management resources
    CLU_ERRCHECK(clReleaseCommandQueue(command_queue), "Failed to release command queue");
    CLU_ERRCHECK(clReleaseContext(context),            "Failed to release OpenCL context");


    // compute min/max/sub

//*******************************************************
    // Sequentially optimized by loop order ( cache misses )
    for(int y=0; y<height; ++y) {
        for(int x=0; x<width; ++x) {
            for(int c=0; c<components; ++c) {
                unsigned char val = data[c + x*components + y*width*components];
                if (val < min_val[c]) min_val[c] = val;
                if (val > max_val[c]) max_val[c] = val;
                sum[c] += val;
            }
        }
    }

    // compute average and multiplicative factors
    float min_fac[components];
    float max_fac[components];
    for(int c=0; c<components; ++c) {
        avg_val[c] = sum[c]/((unsigned long long)width*height);
        min_fac[c] = (float)avg_val[c] / (float)(avg_val[c] - min_val[c]);
        max_fac[c] = (255.0f-(float)avg_val[c]) / (float)(max_val[c] - avg_val[c]);
        if(DEBUG)
            printf("\tComponent %1u: %3u / %3u / %3u * %3.2f / %3.2f\n", c, min_val[c], avg_val[c], max_val[c], min_fac[c], max_fac[c]);
    }

    // ------ Adjust Image ------

//*******************************************************
    // Sequentially optimized by loop order ( cache misses )
    for(int y=0; y<height; ++y) {
        for(int x=0; x<width; ++x) {
            for(int c=0; c<components; ++c) {
                int index = c + x*components + y*width*components;
                unsigned char val = data[index];
                float v = (float)(val - avg_val[c]);
                v *= (val < avg_val[c]) ? min_fac[c] : max_fac[c];
                data[index] = (unsigned char)(v + avg_val[c]);
            }
        }
    }

    if(DEBUG)
        printf("Done, took %.1fms\n", (now() - start_time)*1000.0);
    double time = (now() - start_time)*1000.0;

    // ------ Store Image ------

    if(DEBUG)
        printf("Writing output image %s ...\n", out);

    char temp[80];
    strcpy(temp,out);
    strcat(temp,".ocl.png");
    stbi_write_png(temp,width,height,components,data,width*components);
    stbi_image_free(data);

    if(DEBUG)
        printf("Done!\n");
    return time;
}

int main(int argc, char** argv) {

    // parse input parameters
    if(argc != 3) {
        printf("Usage: auto_levels [inputfile] [outputfile]\nExample: %s test.png test_out\n", argv[0]);
        return EXIT_FAILURE;
    }

    char* input_file_name = argv[1];
    char* output_file_name = argv[2];

    double originalTime = auto_level_original(input_file_name,output_file_name);
    double seqTime = auto_level_seq(input_file_name,output_file_name);
    double ompTime = auto_level_omp(input_file_name,output_file_name);
    double oclTime = auto_level_ocl(input_file_name,output_file_name);

    printf("%.1f \t %.1f \t %.1f \t %.1f \n",originalTime,seqTime,ompTime,oclTime);

    // done
    return EXIT_SUCCESS;
}
