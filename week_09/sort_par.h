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

void create_histogram_ocl(person_t * list, int * out, int entries)
{
    for (int i = 0; i < MAX_AGE; ++i)
        out[i]=0;
    cl_mem devArrA = clCreateBuffer(context, CL_MEM_READ_WRITE, entries * sizeof(person_t), NULL, &err);
    CLU_ERRCHECK(err, "Failed to create Buffer for array A");

    cl_mem devArrB = clCreateBuffer(context, CL_MEM_READ_WRITE, MAX_AGE * sizeof(int), NULL, &err);
    CLU_ERRCHECK(err, "Failed to create Buffer for array B");

    //Part 3: fill Memory Buffers
    err = clEnqueueWriteBuffer(command_queue, devArrA, CL_TRUE, 0, entries * sizeof(person_t), list, 0, NULL, NULL);
    CLU_ERRCHECK(err, "Failed to write Array A to device");

    err = clEnqueueWriteBuffer(command_queue, devArrB, CL_TRUE, 0, MAX_AGE * sizeof(int), out, 0, NULL, NULL);
    CLU_ERRCHECK(err, "Failed to write Array B to device");

    //Part 4: create kernel from source
    cl_kernel kernel = clCreateKernel(program, "create_histogram", &err);
    CLU_ERRCHECK(err, "Failed to create kernel from Program");

    //Part 5: set arguments and execute Kernel
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &devArrA);
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &devArrB);
    err = clSetKernelArg(kernel, 2, sizeof(int) * entries, NULL);
    err = clSetKernelArg(kernel, 3, sizeof(int) * MAX_AGE, NULL);
    err = clSetKernelArg(kernel, 4, sizeof(int), &entries);
    err = clSetKernelArg(kernel, 5, sizeof(int), &a);

    size_t global_size[1] = {entries};
    size_t local_size[1] = {MAX_AGE};

    CLU_ERRCHECK(clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, global_size, local_size, 0, NULL, NULL),
                 "Failed to enqueue kernel.");

    //Part 6: copy back results to host
    err = clEnqueueReadBuffer(command_queue, devArrB, CL_TRUE, 0, MAX_AGE * sizeof(int), out, 0, NULL, NULL);
    CLU_ERRCHECK(err, "Failed reading back results");

    //Part 7: cleanup
    CLU_ERRCHECK(clFlush(command_queue), "Failed to flush command queue");
    CLU_ERRCHECK(clFinish(command_queue), "Failed to wait for command queue completition");
    CLU_ERRCHECK(clReleaseKernel(kernel), "Failed to release kernel");
    CLU_ERRCHECK(clReleaseMemObject(devArrA), "Failed to release Matrix A");
    CLU_ERRCHECK(clReleaseMemObject(devArrB), "Failed to release Matrix A");

}


void calc_index_ocl(int * in_out)
{
    cl_mem devArrC = clCreateBuffer(context, CL_MEM_READ_WRITE, MAX_AGE * sizeof(int), NULL, &err);
    CLU_ERRCHECK(err, "Failed to create Buffer for array C");

    //Part 3: fill Memory Buffers
    err = clEnqueueWriteBuffer(command_queue, devArrC, CL_TRUE, 0, MAX_AGE * sizeof(int), in_out, 0, NULL, NULL);
    CLU_ERRCHECK(err, "Failed to write Array C to device");


    cl_kernel kernel = clCreateKernel(program, "calc_index", &err);
    CLU_ERRCHECK(err, "Failed to create kernel from Program");
    int length = log(MAX_AGE) / log(2);

    //Part 5: set arguments and execute Kernel
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &devArrC);
    err = clSetKernelArg(kernel, 1, sizeof(int) * MAX_AGE, NULL);
    err = clSetKernelArg(kernel, 2, sizeof(int) * MAX_AGE, NULL);
    err = clSetKernelArg(kernel, 3, sizeof(int), &length);
    err = clSetKernelArg(kernel, 4, sizeof(int), &a);

    size_t global_size[1] = {MAX_AGE};
    size_t local_size[1] = {MAX_AGE};

    CLU_ERRCHECK(clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, global_size, local_size, 0, NULL, NULL),
                 "Failed to enqueue kernel.");

    //Part 6: copy back results to host
    err = clEnqueueReadBuffer(command_queue, devArrC, CL_TRUE, 0, MAX_AGE * sizeof(int), in_out, 0, NULL, NULL);
    CLU_ERRCHECK(err, "Failed reading back results");

    CLU_ERRCHECK(clFlush(command_queue), "Failed to flush command queue");
    CLU_ERRCHECK(clFinish(command_queue), "Failed to wait for command queue completition");
    CLU_ERRCHECK(clReleaseKernel(kernel), "Failed to release kernel");
    CLU_ERRCHECK(clReleaseMemObject(devArrC), "Failed to release Matrix A");
}

#endif //PARALLELE_SORT_PAR_H
