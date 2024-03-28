#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define MEM_SIZE_IN_WORDS 32*1024

int main(int argc, char *argv[]){

    //create an array of instructions 
    int mem[MEM_SIZE_IN_WORDS];
    int count = 0;

    for( int i = 0; i < MEM_SIZE_IN_WORDS; i++ ){
        mem[i] = i;
    }

    //read in words 
    while(!feof(stdin)){
        scanf("%o", &mem[count]);
        count++;
    }

    for(int i = 0; i < count; i++){
        printf("0%06o\n", mem[i]);
    }

}
