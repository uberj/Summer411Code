#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define GRAPH_FILE "graph.txt"
#define NUM_CPU 4
#define NUM_NODES 26

struct pt_arg {
    struct node* start_node;
    int size;
};

struct node {
    struct node** neighbors; //immutable
    struct node** _view;
    char letter;
    unsigned int *marks;
    int n_lock;
};

/*
 * Initialize the "nodes" datastructure. It is a 
 *
 */
void *init_nodes(struct node* nodes, int count)
{
    int i;
    for(i=0;i<NUM_NODES;i++) {
        nodes[i].neighbors = calloc(NUM_NODES, sizeof(struct node*));
        nodes[i]._view = calloc(NUM_NODES, sizeof(struct node*));
        nodes[i].marks = calloc(NUM_NODES, sizeof(unsigned int));
    }
    return;

}

void *_print_nodes(struct node* nodes, int count)
{
    int i,j;
    for(i=0;i<NUM_NODES;i++) {
        printf("%c: ",i+65);
        for(j=0;j<NUM_NODES;j++){
            if(!nodes[i]._view[j]) {
                // None is letter
                printf(" ,");
                continue;
            }else{
                //printf("%c,",nodes[i].neighbors[j]->letter);
                printf("%c,",nodes[i]._view[j]->letter);
            }
        }
        printf("\n");
    }
    return;

}

void *_generate_ds(FILE* fp,struct node* nodes)
{
    char unit[7];
    int i,count,a_node,a_edge;
    count = 0;;
    while(fgets(unit,8,fp)) {
        count++;
        a_node = (int)(unit[0]-65);
        a_edge = (int)(unit[3]-65);
        printf("%i %c,%i %c\n",a_node,unit[0],a_edge,unit[3]);
        // Deal with node
        nodes[a_node].letter = unit[0]; // Redundant
        nodes[a_edge].letter = unit[3]; // Redundant

        // This will need to change if you want to have an directed graph.
        // X connected to X
        nodes[a_node].neighbors[a_node] = &nodes[a_node]; // Redundant
        nodes[a_node]._view[a_node] = &nodes[a_node]; // Redundant
        // Y connected to X (comment out these lines if you want a directed graph
        nodes[a_edge].neighbors[a_node] = &nodes[a_node];
        nodes[a_edge]._view[a_node] = &nodes[a_node];
        // X connected to Y
        nodes[a_node].neighbors[a_edge] = &nodes[a_edge];
        nodes[a_node]._view[a_edge] = &nodes[a_edge];
        // Y connected to Y
        nodes[a_edge].neighbors[a_edge] = &nodes[a_edge]; // Redundant
        nodes[a_edge]._view[a_edge] = &nodes[a_edge]; // Redundant
    }
    return;
}

void *walk_node( struct node* static_node, struct node* local_node) {
    int i;
    struct node node_loc,node_stat;
    for(i=0;i<NUM_NODES;i++) {
        if( local_node->neighbors[i] == static_node ){
            // We are looking at ourselves.
            continue;
        } else if ( local_node->neighbors[i] != 0 ){
            if( static_node->_view[i] != local_node->neighbors[i] ){
                static_node->_view[i] = local_node->neighbors[i];
            }
            if( static_node->marks[i] == 0 ){
                // Need a way to do i+1 if we've been there before.
                static_node->marks[i] = 1;
                walk_node(static_node, static_node->_view[i]);
            }
        }
    }
}

void *init_arg(struct node *nodes, struct pt_arg* pt_arg){
    int i,node_count,node_per_thread,extra;

    node_per_thread = NUM_NODES/NUM_CPU;
    extra = NUM_NODES%node_per_thread;
    for(i=0;i<NUM_CPU;i++){
        pt_arg[i].start_node = &nodes[i*node_per_thread];
        pt_arg[i].size = node_per_thread;
    }
    pt_arg[i-1].size += extra;
}

void *calc_connected(void *pt_arg) {
    int i;
    struct node* start_node = ((struct pt_arg *)pt_arg)->start_node;
    for( i=0; i<(int)((struct pt_arg*)pt_arg)->size ;i++ ){
        if(((struct node*)start_node)->letter == 0){
            printf("Empty node\n");
            pthread_exit(NULL);
        }
        printf("Non-Empty node\n");
        walk_node( &start_node[i], &start_node[i] );
        printf("Done with node: %c\n", ((struct node*)&start_node[i])->letter);
    }
    pthread_exit(NULL);
}


int main(void)
{
    FILE *fp;
    size_t count = 0;
    int i,j,rc,numb_threads;
    void * status;
    struct node* nodes = calloc(NUM_NODES,sizeof(struct node)); //A-Z
    pthread_t threads[NUM_CPU];
    pthread_attr_t attr;
    struct pt_arg pt_arg[NUM_CPU];

    /* We want to joni these threads later. */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);


    init_nodes(nodes,count);
    init_arg(nodes,pt_arg);

    fp = fopen(GRAPH_FILE, "r");
    if(fp == NULL) {
        perror("failed to open GRAPH_FILE");
        return EXIT_FAILURE;
    }
    _generate_ds(fp,nodes);
    _print_nodes(nodes,count);
    // Do threads here
    /*
        We need to divide up the workload to each CPU
        node_per_thread = NUM_NODES/NUM_CPU
    */
    for(i=0;i<NUM_CPU;i++){
        rc = pthread_create (&threads[i], &attr, calc_connected, (void *)&pt_arg[i]);
        if (rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
        //calc_connected(&nodes[i],&nodes[i]);
    }
    // end threads
    pthread_attr_destroy(&attr);
    for(i=0; i<NUM_CPU; i++) {
        rc = pthread_join(threads[i], &status);
        if (rc) {
            printf("ERROR; return code from pthread_join() is %d\n", rc);
            exit(-1);
        }
        printf("Main: completed join with thread %ld having a status of %ld\n",i,(long)status);
    }

    printf("Crunched\n");
    _print_nodes(nodes,count);

    fclose(fp);
    printf("Main is done.\n");
    pthread_exit(NULL);
}
