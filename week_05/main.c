#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <memory.h>
#include "utils.h"
#include "cl_utils.h"


void print_usage(char** argv)
{
    printf("Usage:\n\t%s <N>\n",argv[0]);
}

void fill_array(int * array, int N)
{
    for(long i=0; i<N;i++)
        array[i]=rand()%2;
    printf("Array filled.\n");
}

//Actual Task, parallelize this function

double kernel_nanoseconds = 0;

//OCL Version
int count_one_ocl(int * array, long N)
{
    cl_event kernel_execution_event;

    const int local_work_size = 128;
    int local_groups = (N % local_work_size != 0)? (N / local_work_size) + 1 : N / local_work_size;
    int local_groups1 = local_groups;
    N = local_groups * local_work_size;

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
        size_t size[1] = {N}; // two dimensional range

        CLU_ERRCHECK(clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, size, local_size, 0, NULL,
                                            &kernel_execution_event), "Failed to enqueue 1D kernel");

        if(local_groups < 2)
            break;
        local_groups = (local_groups % local_work_size != 0)? (local_groups / local_work_size) + 1 : local_groups / local_work_size;
        N = local_groups * local_work_size;
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

    err = clEnqueueReadBuffer(command_queue, dev_array, CL_TRUE, 0, sizeof(int) * local_groups1, array, 0, NULL, NULL);
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


    return array[0];
}


//OMP Version
int count_one_omp(int * array, long N)
{
    int counter=0;
#pragma omp parallel for reduction(+:counter)
    for (int i = 0; i < N; ++i)
        counter+=array[i];
    return counter;
}


//SEQ Version
int count_one_seq(int * array, long N)
{
    int counter=0;
    for (int i = 0; i < N; ++i)
        counter+=array[i];
    return counter;
}

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

    int* bytearray = malloc(sizeof(int)*N);
    fill_array(bytearray,N);

    timestamp begin = now();
    long count_seq = count_one_seq(bytearray,N);
    timestamp end = now();
    printf("Count SEQ: %ld ones in %.3fms\n",count_seq, (end-begin)*1000);
    begin = now();
    long count_omp = count_one_omp(bytearray,N);
    end = now();
    printf("Count OMP: %ld ones in %.3fms\n",count_omp, (end-begin)*1000);
    begin = now();
    long count_ocl = count_one_ocl(bytearray,N);
    end = now();

    printf("Count OCL: %ld ones in %.3fms; kernel_execution_time: %.3fms\n",count_ocl, (end-begin)*1000,kernel_nanoseconds / 1000000);

    printf("Verification: %s\n %ld / %ld = %f ~ 0.5\n",(count_ocl==count_omp)?(count_ocl==count_seq)?"All Same -> OK":"Not OK":"Not OK",count_seq,N,((double)count_seq)/((double)N));

    free(bytearray);
    return(EXIT_SUCCESS);
}
