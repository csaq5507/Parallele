/*
 * hillissteele.c
 * 
 * Copyright 2018 Jutti <jutti@jutti-550P5C-550P7C>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "utils.h"
#include "cl_utils.h"

typedef float value_t;

void printarray(int*input,int n){
	for(int i=0;i<n;i++){
		printf("%d ",input[i]);
	}
	printf("\n");
}

int* hillissteele_seq(int * input, int n) {
	int length= ceil(log(n)/log(2));
	int* res=malloc(sizeof(int)*n);
	int* tmp=malloc(sizeof(int)*n);
	
	for(int i=0;i<length;i++){
		for(int j=0;j<n;j++){
			if(j<i+1) res[j]=input[j];
			else{
				res[j]=input[j]+input[j-(int)pow(2,i)];

			}
		}
	tmp=input;
	input=res;
	res=tmp;
	
	}
	return input;
	
}
int* hillissteele_ocl(int* input, int n){
	int length= log(n)/log(2);
    printf("Num of iterations: %d \n", length);
	
	//Part1: ocl init
	cl_context context;
	cl_command_queue command_queue;
	cl_device_id device_id= cluInitDevice(0, &context, &command_queue);
	
	//Part 2: create memory buffers
	cl_int err;
	cl_mem devArrA= clCreateBuffer(context, CL_MEM_READ_WRITE , n * sizeof(int),NULL,&err);
	CLU_ERRCHECK(err,"Failed to create Buffer for array A");
		
	
	//Part 3: fill Memory Buffers
	err= clEnqueueWriteBuffer(command_queue, devArrA, CL_TRUE, 0, n * sizeof(int), input,0,NULL,NULL);
	CLU_ERRCHECK(err,"Failed to write Array A to device");

	//Part 4: create kernel from source
	cl_program program= cluBuildProgramFromFile(context, device_id, "hillissteele.cl",NULL);

	cl_kernel kernel= clCreateKernel(program, "hillissteele", &err);
	CLU_ERRCHECK(err,"Failed to create kernel from Program");
	
	//Part 5: set arguments and execute Kernel
	err= clSetKernelArg(kernel,0,sizeof(cl_mem),&devArrA);
	err= clSetKernelArg(kernel,1,sizeof(int) * n,NULL);
	err= clSetKernelArg(kernel,2,sizeof(int) * n,NULL);
	err= clSetKernelArg(kernel,3,sizeof(int),&length);
    err= clSetKernelArg(kernel,4,sizeof(int),&n);

    size_t size[1] = {n};

	CLU_ERRCHECK(clEnqueueNDRangeKernel(command_queue,kernel,1,NULL,size,size,0,NULL,NULL),"Failed to enqueue kernel.");

	//Part 6: copy back results to host
	err= clEnqueueReadBuffer(command_queue,devArrA,CL_TRUE,0,n*sizeof(int),input,0,NULL,NULL);
	CLU_ERRCHECK(err,"Failed reading back results");

	//Part 7: cleanup
	CLU_ERRCHECK(clFlush(command_queue), "Failed to flush command queue");
	CLU_ERRCHECK(clFinish(command_queue), "Failed to wait for command queue completition");
	CLU_ERRCHECK(clReleaseKernel(kernel), "Failed to release kernel");
	CLU_ERRCHECK(clReleaseProgram(program), "Failed to release Program");	
	// free device memory
    CLU_ERRCHECK(clReleaseMemObject(devArrA), "Failed to release Matrix A");

    // free management resources
    CLU_ERRCHECK(clReleaseCommandQueue(command_queue), "Failed to release command queue");
	CLU_ERRCHECK(clReleaseContext(context), "Failed to release OpenCL context");		
	
	return input; 
}
int main(int argc, char **argv)
{
	int n=8;
	int*input=malloc(sizeof(int)*n);
	for (int i=0;i<n;i++){
		input[i]=i+1;
	}
	printarray(input,n);
	printf("Sequenziell:\n");
	int* result=malloc(sizeof(int)*n);
	result=hillissteele_seq(input,n);
	printarray(result,n);
	printf("OpenCl:\n");
		for (int i=0;i<n;i++){
		input[i]=i+1;
	}
	result=hillissteele_ocl(input,n);
	printarray(result,n);
	
	
	return 0;
}


