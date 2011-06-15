#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define GRAPH_FILE "graph.txt"
#define NUM 26

struct node {
    struct node** neighbors; //immutable
    struct node** _view;
    char letter;
    unsigned int *marks;
    int n_lock;
};

void *init_nodes(struct node* nodes, int count)
{
    int i;
    for(i=0;i<NUM;i++) {
        nodes[i].neighbors = calloc(NUM, sizeof(struct node*));
        nodes[i]._view = calloc(NUM, sizeof(struct node*));
        nodes[i].marks = calloc(NUM, sizeof(unsigned int));
    }
    return;

}

void *_print_nodes(struct node* nodes, int count)
{
    int i,j;
    for(i=0;i<NUM;i++) {
        printf("%c: ",i+65);
        for(j=0;j<NUM;j++){
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
        // Y connected to X
        nodes[a_edge].neighbors[a_node] = &nodes[a_node];
        nodes[a_edge]._view[a_node] = &nodes[a_node];
        // X connected to Y
        nodes[a_node].neighbors[a_edge] = &nodes[a_edge];
        nodes[a_node]._view[a_edge] = &nodes[a_edge];
        // Y connected to Y
        nodes[a_edge].neighbors[a_edge] = &nodes[a_edge]; // Redundant
        nodes[a_edge]._view[a_edge] = &nodes[a_edge]; // Redundant
    }
    // Now make a mutable _view array for each node.
    return;
}

void *walk_node(struct node* static_node, struct node* local_node) {
    int i;
    struct node node_loc,node_stat;
    for(i=0;i<NUM;i++) {
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

int main(void)
{
    FILE *fp;
    size_t count = 0;
    int i;
    struct node* nodes = calloc(NUM,sizeof(struct node)); //A-Z
    init_nodes(nodes,count);

    fp = fopen(GRAPH_FILE, "r");
    if(fp == NULL) {
        perror("failed to open GRAPH_FILE");
        return EXIT_FAILURE;
    }
    _generate_ds(fp,nodes);
    _print_nodes(nodes,count);
    // Do threads here
    for(i=0;i<NUM;i++){
        walk_node(&nodes[i],&nodes[i]);
    }
    // end threads
    printf("Crunched\n");
    _print_nodes(nodes,count);

    fclose(fp);
    return EXIT_SUCCESS;
}
