#include <stdio.h>
#include <stdlib.h>

#include "people.h"

#define DEBUG 1

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
        printf("%s is %d years old\n",list[i].name,list[i].age);
    }
}

void sort(person_t * list, int entries)
{
    int * C = malloc(sizeof(int) * MAX_AGE);
    person_t * temp = malloc(sizeof(person_t)*entries);
    memcpy(temp,list, sizeof(person_t)*entries);
    //init C
    for (int i = 0; i < MAX_AGE; ++i)
        C[i]=0;
    //create histogramm
    for (int j = 0; j < entries; ++j)
        C[list[j].age]++;
    //calc starting index
    int total = 0;
    for (int k = 0; k < MAX_AGE; ++k) {
        int old = C[k];
        C[k] = total;
        total+=old;
    }
    
    //calculate output
    for (int l = 0; l < entries; ++l) {
        memcpy(&(list[C[temp[l].age]]),&(temp[l]), sizeof(person_t));
    }
}

int main(int argc, char** argv) {

    // parse input parameters
    if(argc != 3) {
        printf("Usage: %s [list_size] [rand_seed]\nExample: %s 100 5677\n",argv[0], argv[0]);
        return EXIT_FAILURE;
    }
    int entries = atoi(argv[1]);
    srand(atoi(argv[2]));
    person_t * list = malloc(sizeof(person_t) * entries);
    generate_list(list,entries);
    print_list(list,entries);
    sort(list,entries);
    printf("\nSorted:\n");
    print_list(list,entries);
    free(list);
    return EXIT_SUCCESS;
}
