#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <memory.h>
#include "utils.h"
#include "cl_utils.h"

//#define OMP

void print_usage(char** argv)
{
    printf("Usage:\n\t%s <N>\n",argv[0]);
}

void fill_array(char * array, int N)
{
    for(long i=0; i<N;i++)
        array[i]=rand()%2;
    printf("Array filled.\n");
}

//Actual Task, parallelize this function
#if defined(OCL)

double kernel_milliseconds = 0;

//OCL Version
int count_one(char * array, long N)
{
    cl_event kernel_execution_event;

    const int local_work_size = 16;
    int local_groups = (N % local_work_size != 0)? (N / local_work_size) + 1 : N / local_work_size;

    char counter[local_groups];

    cl_context context;
    cl_command_queue command_queue;
    cl_device_id device_id = cluInitDevice(0, &context, &command_queue);

    cl_int err;
    cl_mem dev_array = clCreateBuffer(context, CL_MEM_READ_WRITE , N * sizeof(char), NULL, &err);
    CLU_ERRCHECK(err, "Failed to create buffer for bytearray");
    cl_mem dev_counter = clCreateBuffer(context,CL_MEM_READ_WRITE, sizeof(char) * local_groups,NULL,&err);
    CLU_ERRCHECK(err, "Failed to create buffer for counter");


    // Part 3: fill memory buffers (transfering A is enough, B can be anything)
    err = clEnqueueWriteBuffer(command_queue, dev_array, CL_TRUE, 0, N  * sizeof(char), array, 0, NULL,  NULL);
    CLU_ERRCHECK(err, "Failed to fill buffer");

    err = clEnqueueWriteBuffer(command_queue, dev_counter, CL_TRUE, 0, sizeof(char) * local_groups, counter, 0, NULL,  NULL);
    CLU_ERRCHECK(err, "Failed to fill buffer");

    // Part 4: create kernel from source
    cl_program program = cluBuildProgramFromFile(context, device_id, "count.cl", NULL);
    cl_kernel kernel = clCreateKernel(program, "count_one", &err);
    CLU_ERRCHECK(err, "Failed to create kernel from program");

    // Part 5: set arguments in kernel (those which are constant)
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &dev_array);
    clSetKernelArg(kernel, 1, sizeof(char) * local_work_size, NULL);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), &dev_counter);
    clSetKernelArg(kernel, 3, sizeof(long), &N);

    // enqeue a kernel call for the current time step
    size_t size[1] = {N}; // two dimensional range
    size_t local_size[1] = {local_work_size}; // two dimensional range
    CLU_ERRCHECK(clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, size, local_size, 0, NULL, &kernel_execution_event), "Failed to enqueue 1D kernel");


    // download state of A to host
    err = clEnqueueReadBuffer(command_queue, dev_counter, CL_TRUE, 0, sizeof(char) * local_groups, counter, 0, NULL, NULL);
    CLU_ERRCHECK(err, "Failed to read counter from device");

    // Part 7: cleanup
    // wait for completed operations
    clWaitForEvents(1, &kernel_execution_event);
    CLU_ERRCHECK(clFlush(command_queue),    "Failed to flush command queue");
    CLU_ERRCHECK(clFinish(command_queue),   "Failed to wait for command queue completion");
    CLU_ERRCHECK(clReleaseKernel(kernel),   "Failed to release kernel");
    CLU_ERRCHECK(clReleaseProgram(program), "Failed to release program");

    // free device memory
    CLU_ERRCHECK(clReleaseMemObject(dev_array), "Failed to release Matrix A");
    CLU_ERRCHECK(clReleaseMemObject(dev_counter), "Failed to release Matrix B");

    // free management resources
    CLU_ERRCHECK(clReleaseCommandQueue(command_queue), "Failed to release command queue");
    CLU_ERRCHECK(clReleaseContext(context),            "Failed to release OpenCL context");


    // evaluate events
    cl_ulong time_start;
    cl_ulong time_end;

    // kernel
    clGetEventProfilingInfo(kernel_execution_event,
                            CL_PROFILING_COMMAND_START, sizeof(time_start),
                            &time_start, NULL);
    clGetEventProfilingInfo(kernel_execution_event,
                            CL_PROFILING_COMMAND_END, sizeof(time_end),
                            &time_end, NULL);

    kernel_milliseconds = (time_end - time_start) / 1000000;
    int result = 0;
    for(int i=0; i < local_groups; i++)
        result += counter[i];

    return result;
}

#elif defined(OMP)

//OMP Version
int count_one(char * array, long N)
{
    int counter=0;
#pragma omp parallel for reduction(+:counter)
    for (int i = 0; i < N; ++i)
        if(array[i]==1) counter++;
    return counter;
}

#else

//SEQ Version
int count_one(char * array, long N)
{
    int counter=0;
    for (int i = 0; i < N; ++i)
        if(array[i]==1) counter++;
    return counter;
}

#endif

int main(int argc, char** argv) {

	long N;
    time_t t;

    if (argc > 1) {
        if(argv[1][strlen(argv[1])-1] == 'k'){
            argv[1][strlen(argv[1])-1] = '\0';
            N = atoi(argv[1]) * 1000;
        } else if(argv[1][strlen(argv[1])-1] == 'm') {
            argv[1][strlen(argv[1])-1] = '\0';
            N = atoi(argv[1]) * 1000000;
        }
        else
            N = atoi(argv[1]);
        if(N==0){
            print_usage(argv);
            return EXIT_FAILURE;
        }

    } else
    {
        print_usage(argv);
        return EXIT_FAILURE;
    }
    srand((unsigned) time(&t));

    printf("Your input is: %ld\n", N);

    char* bytearray = malloc(sizeof(char)*N);
    fill_array(bytearray,N);

    timestamp begin = now();
    long count = count_one(bytearray,N);
    timestamp end = now();
#ifdef OCL
    printf("Count %ld ones in %.3fms\n",count, kernel_milliseconds);

#else
    printf("Count %ld ones in %.3fms\n",count, (end-begin)*1000);
#endif
    printf("Verification: %ld / %ld = %f ~ 0.5\n",count,N,((double)count)/((double)N));

    free(bytearray);
    return(EXIT_SUCCESS);
}
