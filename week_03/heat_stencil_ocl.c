#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "cl_utils.h"

typedef float value_t;


// -- matrix utilities --

typedef value_t* Matrix;

Matrix createMatrix(int N, int M);

void releaseMatrix(Matrix m);

void printTemperature(Matrix m, int N, int M);

// ----------------------


int main(int argc, char** argv) {

    // 'parsing' optional input parameter = problem size
    int N = 500;
    if (argc > 1) {
        N = atoi(argv[1]);
    }
    int T = N*100;
    printf("Computing heat-distribution for room size N=%d for T=%d timesteps\n", N, T);

    
    // ---------- setup ----------

    // create a buffer for storing temperature fields
    Matrix A = createMatrix(N,N);
    Matrix C = createMatrix(N,N);

    // set up initial conditions in A
    for(int i = 0; i<N; i++) {
        for(int j = 0; j<N; j++) {
            A[i*N+j] = 273;             // temperature is 0°C everywhere (273K)
        }
    }

    // and there is a heat source in one corner
    int source_x = N/4;
    int source_y = N/4;
    A[source_x*N+source_y] = 273 + 60;

    printf("Initial:\n");
    printTemperature(A,N,N);
    
    // ---------- compute ----------

    // create a second buffer for the computation    
    Matrix B = createMatrix(N,N);

    timestamp begin = now();
    // -- BEGIN ASSIGNMENT --
    {
        // -- solution with CL utils --

        // Part 1: ocl initialization
        cl_context context;
        cl_command_queue command_queue;
        cl_device_id device_id = cluInitDevice(0, &context, &command_queue);

        // Part 2: create memory buffers
        cl_int err;
        cl_mem devMatA = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY, N * N * sizeof(value_t), NULL, &err);
        CLU_ERRCHECK(err, "Failed to create buffer for matrix A");
        cl_mem devMatB = clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, N * N * sizeof(value_t), NULL, &err);
        CLU_ERRCHECK(err, "Failed to create buffer for matrix B");
        cl_mem devMatC = clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, N * N * sizeof(value_t), NULL, &err);
        CLU_ERRCHECK(err, "Failed to create buffer for matrix C");


        // Part 4: create kernel from source
        cl_program program = cluBuildProgramFromFile(context, device_id, "heat_stencil.cl", NULL);
        cl_kernel kernel = clCreateKernel(program, "heat_stencil", &err);
        CLU_ERRCHECK(err, "Failed to create heat_stencil kernel from program");


        for(int t=0; t<T; t+=1000) {

        	  // Part 3: fill memory buffers
        	        err = clEnqueueWriteBuffer(command_queue, devMatA, CL_FALSE, 0, N * N * sizeof(value_t), A, 0, NULL, NULL);
        	        CLU_ERRCHECK(err, "Failed to write matrix A to device");
                    err = clEnqueueWriteBuffer(command_queue, devMatC, CL_FALSE, 0, N * N * sizeof(value_t), A, 0, NULL, NULL);
                    CLU_ERRCHECK(err, "Failed to write matrix C to device");

        	        // Part 5: set arguments and execute kernel
        	        size_t size[2] = {N, N}; // two dimensional range
        	        cluSetKernelArguments(kernel, 3,
        	            sizeof(cl_mem), (void *)&devMatA,
        	            sizeof(cl_mem), (void *)&devMatB,
        	            sizeof(cl_mem), (void *)&devMatC,
        	            sizeof(int), &N
        	        );
        	        CLU_ERRCHECK(clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, size, NULL, 0, NULL, NULL), "Failed to enqueue 2D kernel");

        	        // Part 6: copy results back to host
        	        err = clEnqueueReadBuffer(command_queue, devMatB, CL_TRUE, 0, N * N * sizeof(value_t), B, 0, NULL, NULL);
        	        CLU_ERRCHECK(err, "Failed reading back result");


               // swap matrixes (just pointers, not content)
               Matrix H = A;
               A = B;
               B = H;

               // show intermediate step
                   printf("Step t=%d:\n", t);
                   printTemperature(A,N,N);
           }

        // Part 7: cleanup
        // wait for completed operations (there should be none)
        CLU_ERRCHECK(clFlush(command_queue),    "Failed to flush command queue");
        CLU_ERRCHECK(clFinish(command_queue),   "Failed to wait for command queue completion");
        CLU_ERRCHECK(clReleaseKernel(kernel),   "Failed to release kernel");
        CLU_ERRCHECK(clReleaseProgram(program), "Failed to release program");

        // free device memory
        CLU_ERRCHECK(clReleaseMemObject(devMatA), "Failed to release Matrix A");
        CLU_ERRCHECK(clReleaseMemObject(devMatB), "Failed to release Matrix B");

        // free management resources
        CLU_ERRCHECK(clReleaseCommandQueue(command_queue), "Failed to release command queue");
        CLU_ERRCHECK(clReleaseContext(context),            "Failed to release OpenCL context");
    }

    // -- END ASSIGNMENT --
    

    timestamp end = now();
    printf("Total time: %.3fms\n", (end-begin)*1000);

    releaseMatrix(B);


    // ---------- check ----------    

    printf("Final:\n");
    printTemperature(A,N,N);
    
    bool success = true;
    for(long long i = 0; i<N; i++) {
        for(long long j = 0; j<N; j++) {
            value_t temp = A[i*N+j];
            if (273 <= temp && temp <= 273+60) continue;
            success = false;
            break;
        }
    }
    
    printf("Verification: %s\n", (success)?"OK":"FAILED");
    
    // ---------- cleanup ----------
    
    releaseMatrix(A);
    
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

void printTemperature(Matrix m, int N, int M) {
    const char* colors = " .-:=+*#%@";
    const int numColors = 10;

    // boundaries for temperature (for simplicity hard-coded)
    const value_t max = 273 + 30;
    const value_t min = 273 + 0;

    // set the 'render' resolution
    int H = 30;
    int W = 50;

    // step size in each dimension
    int sH = N/H;
    int sW = M/W;


    // upper wall
    for(int i=0; i<W+2; i++) {
        printf("X");
    }
    printf("\n");

    // room
    for(int i=0; i<H; i++) {
        // left wall
        printf("X");
        // actual room
        for(int j=0; j<W; j++) {

            // get max temperature in this tile
            value_t max_t = 0;
            for(int x=sH*i; x<sH*i+sH; x++) {
                for(int y=sW*j; y<sW*j+sW; y++) {
                    max_t = (max_t < m[x*N+y]) ? m[x*N+y] : max_t;
                }
            }
            value_t temp = max_t;

            // pick the 'color'
            int c = ((temp - min) / (max - min)) * numColors;
            c = (c >= numColors) ? numColors-1 : ((c < 0) ? 0 : c);

            // print the average temperature
            printf("%c",colors[c]);
        }
        // right wall
        printf("X\n");
    }

    // lower wall
    for(int i=0; i<W+2; i++) {
        printf("X");
    }
    printf("\n");

}

