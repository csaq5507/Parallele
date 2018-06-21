#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "cl_utils.h"

typedef cl_float value_t;

#define BLOCK_SIZE 32

// -- matrix utilities --

typedef value_t* Matrix;

Matrix createMatrix(int N, int M);

void releaseMatrix(Matrix m);

// ----------------------

typedef struct _cl_mm_environment {
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel_d_a_c;
    cl_kernel kernel_default;
} cl_mm_environment;

cl_mm_environment createMMEnvironment();

void destroyMMEnvironment(cl_mm_environment);

int roundUpToMultiple(int N, int B) {
    if ((N % B) == 0) return N;
    N = N + (B - (N%B));
    return N;
}

// ----------------------

int SIZES[] = { 500, 734, 1024, 1493, 2345, 4001};
int NUM_SIZES = 6;
int NUM_REPETITION = 3;
const long GLOBAL_MEM_SIZE = 2049728;
const long LOCAL_MEM_SIZE = 49152;
const long GLOBAL_CACHE = 32768;
const long CACHE_LINE = 128;
const long WORK_GROUP_SIZE = 1024;

// ----------------------

int main(int argc, char** argv) {



    // ---------- setup ----------

    cl_mm_environment env = createMMEnvironment();

    // ------ define Parameters -------
    int minSize = 10;
    int maxSize = 20;

    int N = 2000;

    if (argc > 1){
        N = atoi(argv[1]);
    }

    int S = N+1;

    // ---------- compute list --------


    srand(0);
    int* l = (int*)malloc(sizeof(int)*S);
    for(int i = 0; i < S; i++) {
        l[i] = ((rand() / (float) RAND_MAX) * (maxSize - minSize)) + minSize;
    }

    // ---------- init cost mat --------
    int *C = (int*)malloc(sizeof(int)*N*N);

    // ---------- init costs for single matrix --------

    for(int i = 0; i < N; i++) {
        C[i * N + i] = 0;
    }


    // ---------- create buffers --------


    cl_int err;
    cl_mem devMatA = clCreateBuffer(env.context, CL_MEM_READ_ONLY , S * sizeof(int), NULL, &err);
    CLU_ERRCHECK(err, "Failed to create buffer for matrix A");
    cl_mem devMatC = clCreateBuffer(env.context, CL_MEM_WRITE_ONLY , sizeof(int), NULL, &err);

    // ---------- fill buffers --------
    err = clEnqueueWriteBuffer(env.queue, devMatA, CL_TRUE, 0, S * sizeof(int), l, 0, NULL, NULL);
    CLU_ERRCHECK(err, "Failed to write matrix A to device");

    // ---------- create kernel --------
    cl_kernel * k;

    k = &(env.kernel_default);

    int blocksize = 10 * 10;

    // ---------- set arguments --------
    clSetKernelArg(*k, 0, sizeof(cl_mem), (void *)&devMatA);
    clSetKernelArg(*k, 1, sizeof(cl_mem), (void *)&devMatC);

    // submit kernel
    cl_event event;
    CLU_ERRCHECK(clEnqueueNDRangeKernel(env.queue, *k, 1, NULL, blocksize, NULL, 0, NULL, &event), "Failed to enqueue 2D kernel");


    // test whether kernel finished successfully
    cl_int status;
    clGetEventInfo(event, CL_EVENT_COMMAND_EXECUTION_STATUS, sizeof(cl_int), &status, NULL);
    if (status < 0) {
        CLU_ERRCHECK(-status, "Kernel failed to execute succesfully.");
        exit(1);
    }


    // copy results back to host
    err = clEnqueueReadBuffer(env.queue, devMatC, CL_TRUE, 0, N * N * sizeof(value_t), C, 0, NULL, NULL);
    CLU_ERRCHECK(err, "Failed reading back result");



    // free device memory
    CLU_ERRCHECK(clReleaseMemObject(devMatA), "Failed to release Matrix A");
    CLU_ERRCHECK(clReleaseMemObject(devMatC), "Failed to release Matrix C");

    // --- cleanup ---


    // free host memory


    // cleanup

    destroyMMEnvironment(env);

    // finally: report overall result
    printf("\n");
    printf("-------------------------------------------------\n");


    // done
    return EXIT_SUCCESS;
}


Matrix createMatrix(int N, int M) {
    // create data and index vector
    return aligned_alloc(1024,sizeof(value_t)*N*M);
}

void releaseMatrix(Matrix m) {
    free(m);
}

cl_mm_environment createMMEnvironment() {

    cl_mm_environment res;

    // ocl initialization
    cl_device_id device_id = cluInitDeviceWithProperties(0, &res.context, &res.queue, CL_QUEUE_PROFILING_ENABLE);

    // create kernel from source
    cl_int err;
    res.program = cluBuildProgramFromFile(res.context, device_id, "mat_mul.cl", NULL);
    //res.kernel_d_a_c = clCreateKernel(res.program, "matrix_multiplication_divide_and_conquer", &err);
    res.kernel_default = clCreateKernel(res.program, "wave", &err);
    CLU_ERRCHECK(err, "Failed to create wave from program");

    // done
    return res;
}

void destroyMMEnvironment(cl_mm_environment env) {

    // wait for completed operations (there should be none)
    CLU_ERRCHECK(clFlush(env.queue),            "Failed to flush command queue");
    CLU_ERRCHECK(clFinish(env.queue),           "Failed to wait for command queue completion");
    CLU_ERRCHECK(clReleaseKernel(env.kernel_d_a_c),   "Failed to release kernel");
    CLU_ERRCHECK(clReleaseKernel(env.kernel_default),   "Failed to release kernel");
    CLU_ERRCHECK(clReleaseProgram(env.program), "Failed to release program");

    // free management resources
    CLU_ERRCHECK(clReleaseCommandQueue(env.queue), "Failed to release command queue");
    CLU_ERRCHECK(clReleaseContext(env.context),    "Failed to release OpenCL context");
}
