//
// Created by ivan on 24.05.18.
//

#ifndef PARALLELE_SORT_PAR_H
#define PARALLELE_SORT_PAR_H

#include <people.h>
#include "cl_utils.h"

//Part1: ocl init
cl_context context;
cl_command_queue command_queue;
cl_device_id device_id;
cl_int err;
int a;
cl_program program;
cl_kernel kernel;

size_t global_size[1];
size_t local_size[1];
cl_mem histogram;

void pre()
{
    device_id = cluInitDevice(0, &context, &command_queue);
    a = MAX_AGE;
    program = cluBuildProgramFromFile(context, device_id, "kernel.cl", NULL);

}

void post()
{
    CLU_ERRCHECK(clReleaseCommandQueue(command_queue), "Failed to release command queue");
    CLU_ERRCHECK(clReleaseProgram(program), "Failed to release Program");
    CLU_ERRCHECK(clReleaseContext(context), "Failed to release OpenCL context");

}

void create_histogram_ocl(person_t * list, int entries)
{
    int out[MAX_AGE] ={0};

    cl_mem devArrA = clCreateBuffer(context, CL_MEM_READ_WRITE, entries * sizeof(person_t), NULL, &err);
    CLU_ERRCHECK(err, "Failed to create Buffer for array A");

    histogram = clCreateBuffer(context, CL_MEM_READ_WRITE, MAX_AGE * sizeof(int), NULL, &err);
    CLU_ERRCHECK(err, "Failed to create Buffer for array B");

    //Part 3: fill Memory Buffers
    err = clEnqueueWriteBuffer(command_queue, devArrA, CL_TRUE, 0, entries * sizeof(person_t), list, 0, NULL, NULL);
    CLU_ERRCHECK(err, "Failed to write Array A to device");

    err = clEnqueueWriteBuffer(command_queue, histogram, CL_TRUE, 0, MAX_AGE * sizeof(int), out, 0, NULL, NULL);
    CLU_ERRCHECK(err, "Failed to write Array B to device");

    //Part 4: create kernel from source
    cl_kernel kernel = clCreateKernel(program, "create_histogram", &err);
    CLU_ERRCHECK(err, "Failed to create kernel from Program");

    //Part 5: set arguments and execute Kernel
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &devArrA);
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &histogram);
    err = clSetKernelArg(kernel, 2, sizeof(int) * MAX_AGE, NULL);
    err = clSetKernelArg(kernel, 3, sizeof(int), NULL);
    err = clSetKernelArg(kernel, 4, sizeof(int), &entries);
    err = clSetKernelArg(kernel, 5, sizeof(int), &a);

    //get kernel work group size
    size_t max_work_group_size = 0;
    size_t calc_global_size = 0;
    clGetKernelWorkGroupInfo(kernel, device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &max_work_group_size, NULL);
    calc_global_size = (size_t) ((entries/max_work_group_size) + 1) * max_work_group_size;

    size_t global_size[1] = {calc_global_size};
    size_t local_size[1] = {max_work_group_size};

    CLU_ERRCHECK(clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, global_size, local_size, 0, NULL, NULL),
                 "Failed to enqueue kernel.");

    //Part 7: cleanup
    CLU_ERRCHECK(clFlush(command_queue), "Failed to flush command queue");
    CLU_ERRCHECK(clFinish(command_queue), "Failed to wait for command queue completition");
    CLU_ERRCHECK(clReleaseKernel(kernel), "Failed to release kernel");
    CLU_ERRCHECK(clReleaseMemObject(devArrA), "Failed to release Matrix A");

}


void calc_index_ocl(int * out)
{


    cl_kernel kernel = clCreateKernel(program, "calc_index", &err);
    CLU_ERRCHECK(err, "Failed to create kernel from Program");
    int length = log(MAX_AGE) / log(2);

    //Part 5: set arguments and execute Kernel
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &histogram);
    err = clSetKernelArg(kernel, 1, sizeof(int) * MAX_AGE, NULL);
    err = clSetKernelArg(kernel, 2, sizeof(int) * MAX_AGE, NULL);
    err = clSetKernelArg(kernel, 3, sizeof(int), &length);
    err = clSetKernelArg(kernel, 4, sizeof(int), &a);

    size_t global_size[1] = {MAX_AGE};
    size_t local_size[1] = {MAX_AGE};

    CLU_ERRCHECK(clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, global_size, local_size, 0, NULL, NULL),
                 "Failed to enqueue kernel.");

    //Part 6: copy back results to host
    err = clEnqueueReadBuffer(command_queue, histogram, CL_TRUE, 0, MAX_AGE * sizeof(int), out, 0, NULL, NULL);
    CLU_ERRCHECK(err, "Failed reading back results");

    CLU_ERRCHECK(clFlush(command_queue), "Failed to flush command queue");
    CLU_ERRCHECK(clFinish(command_queue), "Failed to wait for command queue completition");
    CLU_ERRCHECK(clReleaseKernel(kernel), "Failed to release kernel");
    CLU_ERRCHECK(clReleaseMemObject(histogram), "Failed to release Matrix A");
}

#endif //PARALLELE_SORT_PAR_H
