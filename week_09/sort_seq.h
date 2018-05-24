//
// Created by ivan on 24.05.18.
//

#ifndef PARALLELE_SORT_SEQ_H
#define PARALLELE_SORT_SEQ_H

#include <people.h>

void create_histogram(person_t * list, int * out, int entries)
{
    for (int i = 0; i < MAX_AGE; ++i)
        out[i]=0;
    for (int j = 0; j < entries; ++j)
        out[list[j].age]++;
}

void calc_index(int * in_out)
{
    //calc starting index
    int total = 0;
    for (int k = 0; k < MAX_AGE; ++k) {
        int old = in_out[k];
        in_out[k] = total;
        total+=old;
    }
}

void calc_output(person_t * list, int *idx, int entries)
{

    person_t * temp = malloc(sizeof(person_t)*entries);
    memcpy(temp,list, sizeof(person_t)*entries);
    //calculate output
    for (int l = 0; l < entries; ++l) {
        memcpy(&(list[idx[temp[l].age]]),&(temp[l]), sizeof(person_t));
        idx[temp[l].age]++;
    }
    free(temp);
}

#endif //PARALLELE_SORT_SEQ_H
