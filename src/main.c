#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <assert.h>

#define ITERATIONS 50
#define THREADS 4
#define BUFFER_SIZE 200
#define MAX_NODES 50000

#ifdef DEBUG 
#define DPRINTF(...) fprintf(stderr, __VA_ARGS__)
#else
#define DPRINTF(...) ;
#endif // DEBUG


pthread_t threads[THREADS]; // array of threads to do work
pthread_barrier_t update_barrier; // barrier for the score computation work
pthread_barrier_t computation_barrier; // barrier for the score update work
size_t graph_size = 0; // keeping the actual size of the graph
int thread_count;

typedef struct list_node{
    struct graph_node* src; // reference of incoming neighbor
    struct list_node* next; // pointer to next elem
}list_node;

typedef struct graph_node{
    long int id; // node id
    double score; // current score
    double new_score; // next iteration score
    double to_send; // score to send to neighbors
    int outcoming_links; // count of outcoming neighbors
    list_node* head; // list of INCOMING neighbors
}graph_node;

void init_node(graph_node graph[], int id){
    if(graph[id].id != -1) // already initialized
        return;

    graph_size++;
    graph[id].id = id;
    graph[id].score = 1.0L;
    graph[id].new_score = 0.0L;
    graph[id].outcoming_links = 0;
}

void insert_neighbor(graph_node graph[], int src, int dest){
    list_node* new_neighbor = malloc(sizeof(list_node));
    new_neighbor->src = &graph[src];
    new_neighbor->next = graph[dest].head;
    graph[dest].head = new_neighbor;
    graph[src].outcoming_links++;
    graph[src].to_send = 
    0.85L / (double)graph[src].outcoming_links; /* updating to_send with the intertion
                                           of every new neighbor  */ 
}

void print_graph(graph_node graph[]){
    for(int i = 0; i < graph_size; ++i){
        
        printf("[%ld] : ", graph[i].id);
        for(list_node* it = graph[i].head; it != NULL; it = it->next)
            printf("<%ld> ", it->src->id);
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

typedef struct thread_arg{
    graph_node* graph;
    int from;
    int to;
}thread_arg;

void* score_exchange(void* _data){
    thread_arg* data = (thread_arg*) _data;
    graph_node* graph = data->graph;

    for(int i = data->from; i < data->to; ++i){
        // next base score is 15% of the current score
        graph[i].new_score = 0.15L * graph[i].score;
        // iterate list and take score from neighbors
        for(list_node* it = graph[i].head; it != NULL; it = it->next){
            graph[i].new_score += graph[it->src->id].to_send;
        }
    }
    DPRINTF("thread from %d to %d has finished score_exchange.\n", data->from, data->to);

    return NULL;
}

void* score_update(void* _data){
    thread_arg* data = (thread_arg*) _data;
    graph_node* graph = data->graph;

    for(int i = data->from; i < data->to; ++i){
        graph[i].score = graph[i].new_score;
        graph[i].to_send = graph[i].score * 0.85L / graph[i].outcoming_links;
    }
    DPRINTF("thread from %d to %d has finished score_update.\n", data->from, data->to);

    return NULL;
}

void iteration(graph_node graph[]){
    thread_arg data[THREADS] = {[0 ... THREADS-1] = {.graph = graph}};
    size_t from = 0;
    size_t interval = graph_size / THREADS;

    for(int i = 0; i < THREADS; ++i, from += interval){
        data[i].from = from;
        data[i].to = (i < THREADS-1 ? from+interval : graph_size);
        pthread_create(&threads[i], NULL, &score_exchange, &data[i]);
    }

    for(int i = 0; i < THREADS; ++i){
        pthread_join(threads[i], NULL);
    }

    for(int i = 0; i < THREADS; ++i){
        pthread_create(&threads[i], NULL, &score_update, &data[i]);
    }

    for(int i = 0; i < THREADS; ++i){
        pthread_join(threads[i], NULL);
    }
}

int main(int argc, char** argv){

    char buffer[BUFFER_SIZE];
    long int src, dest;
    graph_node graph[MAX_NODES] = {[0 ... MAX_NODES-1].id = -1}; // adjacency matrix to simulate the graph 
    clock_t begin = clock();

    assert(argc > 1);
    FILE* fin;
    
    if(!(fin = fopen(argv[1], "r")) ){
        fprintf(stderr, "Error reading file %s : %s\n", argv[1], strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("========= < THREADS = %d >=========\n", THREADS);

    /******************* READ INPUT ************************/

    while(fgets(buffer, BUFFER_SIZE, fin)){
        if(buffer[0] == '#') // ignore comments
            continue;

        sscanf(buffer, "%ld %ld\n", &src, &dest);
        init_node(graph, src);
        init_node(graph, dest);
        insert_neighbor(graph, src, dest);
    }
    clock_t end = clock();

    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Serial part excecution : %lf ms\n", time_spent);
    
    /******************* PARALLEL PART ************************/

    begin = clock();
    for(int i = 0; i < ITERATIONS; ++i){
        iteration(graph);
    }
    end = clock();

    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Parallel part excecution : %lf ms\n", time_spent);

    /******************* PRINT RESULTS ************************/

    char filename[100] = {0};
    sprintf(filename, "%s.res", argv[1]);
    FILE* fout = fopen(filename, "w");
    printf("Results file : %s\n", filename);
    for(int i = 0; i < graph_size; ++i){
        fprintf(fout, "[%d] : %.3lf\n", i, graph[i].score);
    }

    free_all(graph);
    return 0;
}