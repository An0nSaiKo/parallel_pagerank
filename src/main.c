#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#define ITERATIONS 50
#define THREADS 1
#define BUFFER_SIZE 200
#define MAX_NODES 50000

typedef struct list_node{
    struct graph_node* dest; // reference to destined neighbor
    struct list_node* next; // pointer to next elem
}list_node;

typedef struct graph_node{
    long int id; // node id
    double score; // current score
    double new_score; // next iteration score
    list_node* head; // list of neighbors
}graph_node;

void init_node(graph_node graph[], int id){
    if(graph[id].id != -1) // already initialized
        return;

    graph[id].id = id;
    graph[id].score = 1.0L;
    graph[id].new_score = 0.0L;

}

void insert_neighbor(graph_node graph[], int id, int dest){
    list_node* new_neighbor = malloc(sizeof(list_node));
    new_neighbor->dest = &graph[dest];
    new_neighbor->next = graph[id].head;
    graph[id].head = new_neighbor;
}

void print_graph(graph_node graph[]){
    for(int i = 0; i < MAX_NODES; ++i){
        if(graph[i].id == -1) // uninitialized nodes are ignored
            continue;
        
        printf("[%ld] : ", graph[i].id);
        for(list_node* it = graph[i].head; it != NULL; it = it->next)
            printf("<%ld> ", it->dest->id);
        putchar('\n');
    }
}

void free_all(graph_node graph[]){
    for(int i = 0 ; i < MAX_NODES; ++i){
        if(graph[i].head == NULL)
            continue;

        list_node* it = graph[i].head;
        list_node* prev;
        while(it->next != NULL){
            prev = it;
            it = it->next;
            free(prev);
        }
    }
}

int main(int argc, char** argv){

    char buffer[BUFFER_SIZE];
    long int src, dest;
    graph_node graph[MAX_NODES] = {[0 ... MAX_NODES-1].id = -1}; // adjacency matrix to simulate the graph 

    while(fgets(buffer, BUFFER_SIZE, stdin)){
        if(buffer[0] == '#') // ignore comments
            continue;

        sscanf(buffer, "%ld %ld\n", &src, &dest);
        init_node(graph, src);
        init_node(graph, dest);
        insert_neighbor(graph, src, dest);
    }

    free_all(graph);

    return 0;
}