#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#define ITERATIONS 50
#define THREADS 1
#define BUFFER_SIZE 200

typedef char bool;

bool adjacency_matrix[37000][37000]; // adjacency matrix to simulate the graph 

int main(){

    char buffer[BUFFER_SIZE];
    long int src, dest;

    while(fgets(buffer, BUFFER_SIZE, stdin)){
        if(buffer[0] == '#') // ignore comments
            continue;

        sscanf(buffer, "%ld %ld\n", &src, &dest);
        adjacency_matrix[src][dest] = 1;
    }

    return 0;
}