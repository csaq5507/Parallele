OCL_HOME="C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v9.1"

CC=gcc
CC_FLAGS=-O3 -std=c11 -I$(OCL_HOME)/include -L$(OCL_HOME)/lib -pedantic

COMMON_DEPENDENCIES=makefile cl_utils.h
all: main_seq main_omp main_ocl

main_seq: $(COMMON_DEPENDENCIES) main_seq.c 
	@$(CC) $(CC_FLAGS) main_seq.c -o main_seq

main_omp: $(COMMON_DEPENDENCIES) main_omp.c 
	@$(CC) $(CC_FLAGS) main_omp.c -o main_omp -fopenmp

main_ocl: $(COMMON_DEPENDENCIES) main_ocl.c 
	@$(CC) $(CC_FLAGS) main_ocl.c -o main_ocl -lOpenCL

.PHONEY: clean
clean:
	@rm -f main_seq main_omp main_ocl
	
run: all
	@echo "Sequential:"
	@./main_seq
	@echo
	@echo "OpenMP:"
	@./main_omp
	@echo
	@echo "OpenCL:"
	@./main_ocl
