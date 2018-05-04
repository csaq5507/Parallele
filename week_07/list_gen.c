#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
    free(list);
    return EXIT_SUCCESS;
}
