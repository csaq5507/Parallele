#include <stdio.h>
#include <stdlib.h>

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <CL/cl.h>

#include "utils.h"

typedef float value_t;


// -- kernel code utils --

typedef struct kernel_code {
    const char* code;
    size_t size;
} kernel_code;

kernel_code loadCode(const char* filename);

void releaseCode(kernel_code code);

// -- matrix utilities --

typedef value_t* Matrix;

Matrix createMatrix(int N, int M);

void releaseMatrix(Matrix m);

// ----------------------


int main(int argc, char** argv) {

    // 'parsing' optional input parameter = problem size
    int N = 1000;
    if (argc > 1) {
        N = atoi(argv[1]);
    }
    printf("Computing matrix-matrix product with N=%d\n", N);

    int size = N * N;

    // ---------- setup ----------

    // create two input matrixes (on heap!)
    Matrix A = createMatrix(N,N);
    Matrix B = createMatrix(N,N);
    
    // fill matrixes
    for(int i = 0; i<N; i++) {
        for(int j = 0; j<N; j++) {
            A[i*N+j] = i*j;             // some matrix - note: flattend indexing!
            B[i*N+j] = (i==j) ? 1 : 0;  // identity
        }
    }
    
    // ---------- compute ----------
    
    Matrix C = createMatrix(N,N);

    timestamp begin = now();
    
    
    // -- BEGIN ASSIGNMENT --
    



    {
        // OpenCL reference pages:
        // https://www.khronos.org/registry/OpenCL/sdk/1.2/docs/man/xhtml/

        // some local state variables
        cl_platform_id platform_id = NULL;
        cl_device_id device_id = NULL;
        cl_context context = NULL;
        cl_command_queue command_queue = NULL;
        cl_program program = NULL;
        cl_kernel kernel = NULL;
        cl_uint ret_num_devices;
        cl_uint ret_num_platforms;
        cl_int ret;

        // TODO: all return codes should be checked!

        // Part A - resource management

        // 1) get platform
        ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);

        // 2) get device
        ret = clGetDeviceIDs( platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);

        // 3) create context
        context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);

        // 4) create command queue
        command_queue = clCreateCommandQueue(context, device_id, 0, &ret);



        // Part B - data management

        // 5) create memory buffers on device
        size_t mat_size = sizeof(value_t) * N * N;
        cl_mem devMatA = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY, mat_size, NULL, &ret);
        cl_mem devMatB = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY, mat_size, NULL, &ret);
        cl_mem devMatC = clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, mat_size, NULL, &ret);

        // 6) transfer input data from host to device (synchroniously)
        ret = clEnqueueWriteBuffer(command_queue, devMatA, CL_TRUE, 0, mat_size, &A[0], 0, NULL, NULL);
        ret = clEnqueueWriteBuffer(command_queue, devMatB, CL_TRUE, 0, mat_size, &B[0], 0, NULL, NULL);



        // Part C - computation

        // 6) load kernel code from file
        kernel_code code = loadCode("mat_mul.cl");

        // 7) compile kernel program from source
        program = clCreateProgramWithSource(context, 1, &code.code,
				                      (const size_t *)&code.size, &ret);

        // 8) build program (compile + link for device architecture)
        ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

        // report kernel build errors
        if (ret != CL_SUCCESS) {

            // create a temporary buffer for the message
            size_t size = 1<<20;    // 1MB
            char* msg = malloc(size);
            size_t msg_size;

            // retrieve the error message
            clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, size, msg, &msg_size);

            // print the error message
            printf("Build Error:\n%s",msg);
            exit(1);
        }

        // 9) create OpenCL kernel
        kernel = clCreateKernel(program, "mat_mul", &ret);

        // 10) set arguments
        ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), &devMatC);
        ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), &devMatA);
        ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), &devMatB);
        ret = clSetKernelArg(kernel, 3, sizeof(int), &N);

        // 11) schedule kernel
        size_t global_work_offset = 0;
        size_t global_work_size = N;
        ret = clEnqueueNDRangeKernel(command_queue, kernel,
                    1, &global_work_offset, &global_work_size, NULL,
                    0, NULL, NULL
        );

        // 12) transfere data back to host
        ret = clEnqueueReadBuffer(command_queue, devMatC, CL_TRUE, 0, mat_size, &C[0], 0, NULL, NULL);

        // Part D - cleanup

        // wait for completed operations (there should be none)
        ret = clFlush(command_queue);
        ret = clFinish(command_queue);
        ret = clReleaseKernel(kernel);
        ret = clReleaseProgram(program);

        // free device memory
        ret = clReleaseMemObject(devMatA);
        ret = clReleaseMemObject(devMatB);
        ret = clReleaseMemObject(devMatC);

        // free management resources
        ret = clReleaseCommandQueue(command_queue);
        ret = clReleaseContext(context);

    }
    
    // -- END ASSIGNMENT --

    timestamp end = now();
    printf("Total time: %.3fms\n", (end-begin)*1000);

    // ---------- check ----------    
    
    bool success = true;
    for(long long i = 0; i<N; i++) {
        for(long long j = 0; j<N; j++) {
            if (C[i*N+j] == i*j) continue;
            success = false;
            break;
        }
    }
    
    printf("Verification: %s\n", (success)?"OK":"FAILED");
    
    // ---------- cleanup ----------
    
    releaseMatrix(A);
    releaseMatrix(B);
    releaseMatrix(C);
    
    // done
    return (success) ? EXIT_SUCCESS : EXIT_FAILURE;
}


Matrix createMatrix(int N, int M) {
    // create data and index vector
    return malloc(sizeof(value_t)*N*M);
}

void releaseMatrix(Matrix m) {
    free(m);
}

kernel_code loadCode(const char* filename) {
    size_t MAX_SOURCE_SIZE = 0x100000;

    FILE* fp;

    /* Load the source code containing the kernel*/
    fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Failed to load kernel from file %s\n", filename);
        exit(1);
    }

    kernel_code res;
    res.code = (char*)malloc(MAX_SOURCE_SIZE);
    res.size = fread( (char*)res.code, 1, MAX_SOURCE_SIZE, fp);
    fclose( fp );

    return res;
}

void releaseCode(kernel_code code) {
    free((char*)code.code);
}

