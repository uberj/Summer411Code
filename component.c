#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define GRAPH_FILE "graph.txt"
#define NUM 3

struct node {
    struct node* neighbors;
    char letter;
    int mark;
    int n_lock;
};

void *init_nodes(struct node** nodes, int count)
{
    int i;
    for(i=0;i<NUM;i++) {
        nodes[i] = calloc(1,sizeof(struct node));
        nodes[i]->letter = 0;
        nodes[i]->letter = 0;
        nodes[i]->neighbors = calloc(NUM, sizeof(struct node));
    }
    return;

}

void *_print_nodes(struct node** nodes, int count)
{
    int i,j;
    for(i=0;i<NUM;i++) {
        printf("%c: ",i+65);
        for(j=0;j<NUM;j++){
            if(! nodes[i]->neighbors[j].letter ) {
                // None is letter
                printf(" ,");
                continue;
            }else{
                printf("%c,",nodes[i]->neighbors[j].letter);
            }
        }
        printf("\n");
    }
    return;

}

void *_generate_ds(FILE* fp,struct node** nodes,size_t *count)
{
    char unit[7];
    int i,a_node,a_edge;
    while(fgets(unit,8,fp)) {
        *count++;
        a_node = (int)(unit[0]-65);
        a_edge = (int)(unit[3]-65);
        printf("%i %c,%i %c\n",a_node,unit[0],a_edge,unit[3]);
        // Deal with node
        nodes[a_node]->letter = unit[0]; // Redundant
        nodes[a_node]->neighbors[a_node] = *nodes[a_node];
        nodes[a_node]->neighbors[a_edge] = *nodes[a_edge];
        // Give letter to edge
        nodes[a_edge]->letter = unit[3];

    }

    return;
}

void *walk_node(struct node* static_node, struct node* local_node) {
    int i;
    struct node node_loc,node_stat;
    for(i=0;i<NUM;i++) {
        if( ( local_node->neighbors[i].letter != 0 ) &&
            ( local_node->neighbors[i].letter != static_node->letter)) {
            // Local is next to i, Static doesn't know about i
            static_node->neighbors[i].mark = 1;
            static_node->neighbors[i] = local_node->neighbors[i];
            walk_node( static_node, &local_node->neighbors[i] );

        }
        printf("HERE\n");
    }

}

int main(void)
{
    FILE *fp;
    size_t count = 0;
    int i;
    struct node** nodes = calloc(NUM,sizeof(struct node*)); //A-Z
    init_nodes(nodes,count);

    fp = fopen(GRAPH_FILE, "r");
    if(fp == NULL) {
        perror("failed to open GRAPH_FILE");
        return EXIT_FAILURE;
    }
    _generate_ds(fp,nodes,&count);
    _print_nodes(nodes,count);
    // Do threads here
    for(i=0;i<NUM;i++){
        walk_node(nodes[i],nodes[i]);
    }
    // end threads
    printf("Crunched\n");
    _print_nodes(nodes,count);

    fclose(fp);
    return EXIT_SUCCESS;
}
