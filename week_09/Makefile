
OCL_HOME=/scratch/c703/c7031057/opencl

CC=gcc
CC_FLAGS=-O3 -std=c11 -I$(OCL_HOME)/include -L$(OCL_HOME)/lib -Werror -pedantic

COMMON_DEPENDENCIES=Makefile utils.h

all: countsort_bench

countsort_bench: $(COMMON_DEPENDENCIES) countsort_bench.c
	@$(CC) $(CC_FLAGS) countsort_bench.c	-lm	-o countsort_bench -lOpenCL

.PHONEY: clean
clean:
	@rm	countsort_bench
