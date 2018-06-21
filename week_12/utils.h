#pragma once

#include <time.h>

typedef float value_t;


// add a pseudo-bool type
typedef int bool;
#define true  (0==0)
#define false (0!=0)


// a small wrapper for convenient time measurements

typedef double timestamp;

timestamp now() {
    struct timespec spec;
    timespec_get(&spec, TIME_UTC);
    return spec.tv_sec + spec.tv_nsec / (1e9);
}


struct vector{
    size_t size;
    value_t * row;
};

struct vector_t{
    size_t size;
    value_t * column;
};

struct matrix{
    size_t size;
    struct vector * column;
};

struct matrix_t{
    size_t size;
    struct vector_t * row;
};

struct matrix * generate(int size)
{
    struct matrix * mat = malloc(sizeof(struct matrix));
    mat->size=size;
    mat->column = malloc(sizeof(struct vector) * size);
    for (int i = 0; i < size; ++i) {
        mat->column[i].size = size;
        mat->column[i].row = malloc(sizeof(value_t) * size);
        for (int j = 0; j < size; ++j) {
            mat->column[i].row[j] = 0;
        }
    }
}

float get(struct matrix * mat, int row, int column)
{
    return mat->column[column].row[row];
}

float get_t(struct matrix_t * mat, int row, int column)
{
    return mat->row[row].column[column];
}

void set(struct matrix * mat, int row, int column, value_t value)
{
    mat->column[column].row[row] = value;
}

void set_t(struct matrix_t * mat, int row, int column, value_t value)
{
    mat->row[row].column[column] = value;
}