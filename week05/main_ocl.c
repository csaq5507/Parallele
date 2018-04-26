#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//#include "utils.h"


int main(int argc, char** argv) {
	
    int exit;
	long n;
    long ones=0;
    time_t t;

    srand((unsigned) time(&t));

	printf("Enter a Inputsize!\n"); 
    scanf("%d", &n);
	printf("Your input is:%d\n", n);
    char* bytearray= malloc(sizeof(char)*n);
    #pragma omp parallel for
    for(long i=0; i<n;i++){
        bytearray[i]=rand()%2;
       /* printf("%d",bytearray[i]);
        if(i%20==0)printf("\n");
        */
        if(bytearray[i]==1) ones++;
    }
    printf("There are %d 1 entrys!\n", ones);
    printf("Press 2 to Continue\n");  
    scanf("%d", &exit);
    return(EXIT_SUCCESS);
}