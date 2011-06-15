#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define GRAPH_FILE "graph.txt"

struct node {
    char *neighbors;
    int mark;
    int n_lock;
};

void *init_nodes(struct node* nodes, int count)
{
    int i;
    for(i=0;i<26;i++) {
        nodes[i].neighbors = calloc(26, sizeof(struct node));
    }
    return;

}

void *_print_nodes(struct node* nodes, int count)
{
    int i,j;
    for(i=0;i<26;i++) {
        printf("%c: ",i+65);
        for(j=0;j<26;j++){
            printf("%c,",nodes[i].neighbors[j]);
        }
        printf("\n");
    }
    return;

}
void *_generate_ds(FILE* fp,struct node* nodes,size_t *count)
{
    char unit[7];
    int i,a_node,a_edge;
    while(fgets(unit,8,fp)) {
        *count++;
        a_node = (int)(unit[0]-65);
        a_edge = (int)(unit[3]-65);
        printf("%i %c,%i %c\n",a_node,unit[0],a_edge,unit[3]);
        nodes[a_node].neighbors[a_edge] = unit[3];
    }

    return;
}

void *walk_node(struct node* node) {

}

int main(void)
{
    FILE *fp;
    size_t count = 0;
    int i;
    struct node* nodes = calloc(26,sizeof(struct node)); //A-Z
    init_nodes(nodes,count);

    fp = fopen(GRAPH_FILE, "r");
    if(fp == NULL) {
        perror("failed to open GRAPH_FILE");
        return EXIT_FAILURE;
    }
    _generate_ds(fp,nodes,&count);
    _print_nodes(nodes,count);

    fclose(fp);
    return EXIT_SUCCESS;
}
