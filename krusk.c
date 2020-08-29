// C program for Kruskal's algorithm to find Minimum Spanning Tree 
// of a given connected, undirected and weighted graph 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include<pthread.h>

int ind1=-1;//indicator for thread1
int ind2=-1;//indicator for thread2

// a structure to represent a weighted edge in graph 
struct Edge 
{ 
	int src, dest, weight,sts; 
}; 

// a structure to represent a connected, undirected 
// and weighted graph 
struct Graph 
{ 
	// V-> Number of vertices, E-> Number of edges 
	int V, E; 

	// graph is represented as an array of edges. 
	// Since the graph is undirected, the edge 
	// from src to dest is also edge from dest 
	// to src. Both are counted as 1 edge here. 
	struct Edge* edge; 
}; 



// Creates a graph with V vertices and E edges 
struct Graph* createGraph(int V, int E) 
{ 
	struct Graph* graph =(struct Graph*)malloc(sizeof(struct Graph)); 
	graph->V = V; 
	graph->E = E; 

	graph->edge =(struct Edge*)malloc(E*sizeof(struct Edge)); 

	return graph; 
} 

// A structure to represent a subset for union-find 
struct subset 
{ 
	int parent; 
	int rank; 
}; 

//thread args
struct targ{
int st,end,*ind;
struct Graph* graph;
struct subset* subsets;
};

// A utility function to find set of an element i 
// (uses path compression technique) 
int find(struct subset subsets[], int i) 
{ 
	// find root and make root as parent of i 
	// (path compression) 
	if (subsets[i].parent != i) 
		subsets[i].parent = find(subsets, subsets[i].parent); 

	return subsets[i].parent; 
} 

// A function that does union of two sets of x and y 
// (uses union by rank) 
void Union(struct subset subsets[], int x, int y) 
{ 
	int xroot = find(subsets, x); 
	int yroot = find(subsets, y); 

	// Attach smaller rank tree under root of high 
	// rank tree (Union by Rank) 
	if (subsets[xroot].rank < subsets[yroot].rank) 
		subsets[xroot].parent = yroot; 
	else if (subsets[xroot].rank > subsets[yroot].rank) 
		subsets[yroot].parent = xroot; 

	// If ranks are same, then make one as root and 
	// increment its rank by one 
	else
	{ 
		subsets[yroot].parent = xroot; 
		subsets[xroot].rank++; 
	} 
} 

// Compare two edges according to their weights. 
// Used in qsort() for sorting an array of edges 
int myComp(const void* a, const void* b) 
{ 
	struct Edge* a1 = (struct Edge*)a; 
	struct Edge* b1 = (struct Edge*)b; 
	return a1->weight > b1->weight; 
} 

//helper thread

void ht(struct targ *t){

int sz=(t->end)-(t->st);
int i=t->st;
//printf("\nht called %d\n",sz);
while(1){
//printf("\nchk1\n");
if(*(t->ind)==1){
//printf("\nbrk\n");
break;
}


struct Edge next_edge = t->graph->edge[i]; 

		int x = find(t->subsets, next_edge.src); 
		int y = find(t->subsets, next_edge.dest); 

		//if cycle found
		if (x == y) 
		{ 
                // printf("\nmarked\n");
		t->graph->edge[i].sts=1;
		}
i++; 
if(i==t->end+1)
i=t->st;

//printf("\nchk2\n");
}


}

// The main function to construct MST using Kruskal's algorithm 
void KruskalMST(struct Graph* graph) 
{ 
	int V = graph->V; 
	struct Edge result[V]; // Tnis will store the resultant MST 
	int e = 0; // An index variable, used for result[] 
	int i = 0; // An index variable, used for sorted edges 
        pthread_t thread1, thread2;
	// Step 1: Sort all the edges in non-decreasing 
	// order of their weight. If we are not allowed to 
	// change the given graph, we can create a copy of 
	// array of edges 
	qsort(graph->edge, graph->E, sizeof(graph->edge[0]), myComp); 

	// Allocate memory for creating V ssubsets 
	struct subset *subsets = 
		(struct subset*) malloc( V * sizeof(struct subset) ); 

	// Create V subsets with single elements 
	for (int v = 0; v < V; ++v) 
	{ 
		subsets[v].parent = v; 
		subsets[v].rank = 0; 
	} 


struct targ *t=(struct targ*)malloc(sizeof(struct targ));
t->st=150;
t->end=250;
t->graph=graph;
t->subsets=subsets;
t->ind=&ind1;

struct targ *t1=(struct targ*)malloc(sizeof(struct targ));
t1->st=251;
t1->end=299;
t1->graph=graph;
t1->subsets=subsets;
t1->ind=&ind2;
//calling the helper function
pthread_create(&thread1, NULL, (void*)ht,t);
pthread_create(&thread2, NULL, (void*)ht,t1);

	// Number of edges to be taken is equal to V-1 
	while (e < V - 1 && i < graph->E) 
	{ 
		// Step 2: Pick the smallest edge. And increment 
		// the index for next iteration 
               		struct Edge next_edge = graph->edge[i++]; 
               //printf("\nmain thread running\n");  
              if(graph->edge[i-1].sts!=1){
                
                int x = find(subsets, next_edge.src); 
		int y = find(subsets, next_edge.dest); 

		// If including this edge does't cause cycle, 
		// include it in result and increment the index 
		// of result for next edge 
		if (x != y) 
		{ 
			result[e++] = next_edge; 
			Union(subsets, x, y); 
		}
                } 
		 
		// Else discard the next_edge 

          if(i==150)
           ind1=1;
           else if(i==251)
           ind2=1;
	} 

	// print the contents of result[] to display the 
	// built MST 

pthread_join(thread1,NULL);
pthread_join(thread2,NULL);

	printf("Following are the edges in the constructed MST\n"); 
	for (i = 0; i < e; ++i) 
		printf("%d -- %d == %d\n", result[i].src, result[i].dest, result[i].weight); 
	return; 
} 

// Driver program to test above functions 
int main() 
{ 
	/* Let us create following weighted graph 
			10 
		0--------1 
		| \	 | 
	6| 5\ |15 
		|	 \ | 
		2--------3 
			4	 */
	int V = 64; // Number of vertices in graph 
	int E = 300; // Number of edges in graph 
	struct Graph* graph = createGraph(V, E); 
char c;
FILE *fptr;
    if ((fptr = fopen("ip.txt", "r")) == NULL)
    {
        printf("Error! opening file");
        // Program exits if file pointer returns NULL.
        exit(1);         
    }
int i=0;
while(!feof(fptr))
{
fscanf(fptr,"%c %d %d %d\n",&c,&graph->edge[i].src,&graph->edge[i].dest,&graph->edge[i].weight);
/*
fscanf(fptr,&c);
//scanf("%c",&c);
fscanf(fptr,&graph->edge[i].src);
fscanf(fptr,&graph->edge[i].dest);
fscanf(fptr,&graph->edge[i].weight);
*/
i++;
//scanf("%d %d %d",&graph->edge[i].src,&graph->edge[i].dest,&graph->edge[i].weight);
graph->edge[i].sts=0; 
}
	
fclose(fptr);
    


clock_t totalTime = clock();
	KruskalMST(graph); 

totalTime = (clock()-totalTime);
float t=totalTime;
	printf("\nTime taken: %f",t);

	return 0; 
} 

