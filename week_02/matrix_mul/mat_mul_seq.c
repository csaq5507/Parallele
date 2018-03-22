#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

typedef float value_t;

//#define SEQ_OPT
// -- matrix utilities --

#ifdef SEQ_OPT

struct M {
    value_t *data;
    int * indirection;
    int N;
};

struct M * init(int N)
{
    struct M* ret = malloc(sizeof(struct M));
    ret->N=N;
    ret->data = malloc(sizeof(value_t)*N*N);
    ret->indirection = malloc(sizeof(int)*N);
    for(int i=0;i<N;i++)
    {
        ret->indirection[i] = i * N;
    }
    return ret;
}

value_t get(struct M* object,int i,int j){
    return object->data[object->indirection[i] + j];
}

void set(struct M* object,int i,int j, value_t value){
    object->data[object->indirection[i] + j] = value;
}

#else

struct M{
    value_t *data;
    int N;

};

struct M * init(int N)
{
    struct M* ret = malloc(sizeof(struct M));
    ret->N=N;
    ret->data = malloc(sizeof(value_t)*N*N);
    return ret;
}

value_t get(struct M* object,int i,int j){
    return object->data[i * object->N + j];
}

void set(struct M* object,int i,int j, value_t value){
    object->data[i * object->N + j] = value;
}


#endif

typedef struct M Matrix;


// ----------------------


int main(int argc, char** argv) {

    // 'parsing' optional input parameter = problem size
    int N = 1000;
    if (argc > 1) {
        N = atoi(argv[1]);
    }
    printf("Computing matrix-matrix product with N=%d\n", N);

    
    // ---------- setup ----------

    // create two input matrixes (on heap!)
    Matrix* A = init(N);
    Matrix* B = init(N);
    
    // fill matrixes
    for(int i = 0; i<N; i++) {
        for(int j = 0; j<N; j++) {
            set(A,i,j,i*j);             // some matrix - note: flattend indexing!
            set(B,i,j,(i==j) ? 1 : 0);  // identity
        }
    }
    
    // ---------- compute ----------
    
    Matrix* C=init(N);

    timestamp begin = now();
    for(long long i = 0; i<N; i++) {
        for(long long j = 0; j<N; j++) {
            value_t sum = 0;
            for(long long k = 0; k<N; k++) {
                sum += get(A,i,k) * get(B,k,j);
            }
            set(C,i,j,sum);
        }
    }
    timestamp end = now();
    printf("Total time: %.3fms\n", (end-begin)*1000);

    // ---------- check ----------    
    
    bool success = true;
    for(long long i = 0; i<N; i++) {
        for(long long j = 0; j<N; j++) {
            if (get(C,i,j) == i*j) continue;
            success = false;
            break;
        }
    }
    
    printf("Verification: %s\n", (success)?"OK":"FAILED");
    
    // ---------- cleanup ----------

    
    // done
    return (success) ? EXIT_SUCCESS : EXIT_FAILURE;
}

