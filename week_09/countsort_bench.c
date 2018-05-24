//
// Created by ivan on 22.05.18.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <CL/cl.h>

#include "people.h"
#include "utils.h"

#include "sort_par.h"
#include "sort_seq.h"


void generate_list(person_t * list, int entries)
{
    for (int i = 0; i < entries; ++i) {
        list[i].age=rand() % MAX_AGE;
        gen_name(list[i].name);
    }
}


void print_list(person_t * list, int entries)
{
    for (int i = 0; i < entries; ++i) {
        printf("%d years: %s\n",list[i].age,list[i].name);
    }
}


bool verification(person_t * list,person_t * list2, int entries){
    for (int i = 1; i < entries; ++i) {
        if (list[i].age < list[i - 1].age)
            return false;
        if (list2[i].age < list2[i - 1].age)
            return false;
        if (strcmp(list[i].name, list2[i].name))
            return false;
    }
    return true;
}

void sort_seq(person_t * list, int entries)
{
    int * C = malloc(sizeof(int) * MAX_AGE);
    create_histogram(list,C,entries);
    calc_index(C);
    calc_output(list,C,entries);
    free(C);
}

void sort_ocl(person_t * list, int entries)
{
    int * C = malloc(sizeof(int) * MAX_AGE);
    pre();
    create_histogram_ocl(list,C,entries);
    calc_index_ocl(C);
    calc_output(list,C,entries);
    free(C);
    post();
}



int main(int argc, char** argv) {

    // parse input parameters
    if(argc != 3) {
        printf("Usage: ./countsort_bench [list_size] [rand_seed]\nExample: %s 100 5677\n",argv[0]);
        return EXIT_FAILURE;
    }
    int entries = atoi(argv[1]);
    srand(atoi(argv[2]));
    person_t * list_seq = malloc(sizeof(person_t) * entries);
    person_t * list_ocl = malloc(sizeof(person_t) * entries);
    generate_list(list_seq,entries);
    memcpy(list_ocl,list_seq, sizeof(person_t) * entries);
  //  print_list(list_seq,entries);
    timestamp start_seq = now();
    sort_seq(list_seq,entries);
    timestamp end_seq = now();
    sort_ocl(list_ocl,entries);
    timestamp end_ocl = now();
    printf("Verification: %s\n",verification(list_seq,list_ocl,entries)?"OK":"FAIL");
    printf("Seq: %.3f\n",(end_seq-start_seq)*1000);
    printf("Ocl: %.3f\n",(end_ocl-end_seq)*1000);
    //printf("\nSorted SEQ:\n");
    //print_list(list_seq,entries);
    free(list_seq);
    free(list_ocl);
    return EXIT_SUCCESS;
}
