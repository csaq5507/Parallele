
OCL_HOME=/scratch/c703/c7031057/opencl

CC=gcc
CC_FLAGS=-O3 -std=c11 -I$(OCL_HOME)/include -L$(OCL_HOME)/lib -Werror -pedantic

COMMON_DEPENDENCIES=Makefile utils.h

all: heat_stencil

heat_stencil: $(COMMON_DEPENDENCIES) heat_stencil.c cl_utils.h
	@$(CC) $(CC_FLAGS) heat_stencil.c -o heat_stencil -lOpenCL

.PHONEY: clean
clean:
	@rm heat_stencil


