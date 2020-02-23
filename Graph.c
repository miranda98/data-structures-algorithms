// Graph ADT interface for Ass2 (COMP2521)
// By Jason and Miranda - April 2019
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "Graph.h"

#define TRUE 1
#define FALSE 0

// Graph representation struct taken and modified from COMP2521 Week 4 Lecture, April 2019
struct GraphRep{
	AdjList *vertices;
	int nV;
	int nE;
} GraphRep;

// Local adjList helper function prototypes
static adjListNode *newAdjListNode(Vertex vtx, int wt);
static int inAdjList(AdjList L, Vertex vtx);
static AdjList insertAdjList(AdjList L, Vertex vtx, int wt);
static AdjList removeAdjListNode(AdjList L, Vertex vtx);
static void showAdjList(AdjList L);
static int getWeight(AdjList L, Vertex v);
static void freeAdjList(AdjList L);


/*
Graph functions
*/

// Given number of nodes, returns a Graph (pointer to GraphRep)
// Function taken and modified from COMP2521 Week 4 Lecture, April 2019
Graph newGraph(int noNodes) {
	assert(noNodes >= 0);

	// Counter for allocating memory for array
	int i;

	// Initialise graph
	Graph g = malloc(sizeof(GraphRep));
	assert(g != NULL);
	g->nV = noNodes;
	g->nE = 0;

	// Allocate memory for array of AdjList
	g->vertices = malloc(noNodes * sizeof(AdjList));
	assert(g->vertices != NULL);
	for (i = 0; i < noNodes; i++) {
		g->vertices[i] = NULL;
	}

	return g;
}

// Given a graph, returns number of vertices in graph
int numVerticies(Graph g) {
	assert(g != NULL);

	return g->nV;
}

// Given a graph, source vertex, destination vertex and weight, adds
// directed edge between them
// Function taken and modified from COMP2521 Week 4 Lecture, April 2019
void insertEdge(Graph g, Vertex src, Vertex dest, int weight) {
	assert(g != NULL);

	if (!inAdjList(g->vertices[src], dest)) {
		g->vertices[src] = insertAdjList(g->vertices[src], dest, weight);
		g->nE++;
	}
}

// Given a graph, source and destination, removes the directed edge between them
// Function taken and modified from COMP2521 Week 4 Lecture, April 2019
void removeEdge(Graph g, Vertex src, Vertex dest) {
	assert(g != NULL);

	if (inAdjList(g->vertices[src], dest)) {
		g->vertices[src] = removeAdjListNode(g->vertices[src], dest);
		g->nE--;
	}
}
// Given a graph, source and destination, checks if there is an edge between them
// Function taken and modified from COMP2521 Week 4 Lecture, April 2019
bool adjacent(Graph g, Vertex src, Vertex dest) {
	assert(g != NULL);
	return inAdjList(g->vertices[src], dest);
}

AdjList outIncident(Graph g, Vertex v) {
	return g->vertices[v];
}

AdjList inIncident(Graph g, Vertex v) {
	AdjList inVertices = NULL;
	int i;

	for (i = 0; i < g->nV; i++) {
		if (adjacent(g, i, v)) {
			inVertices = insertAdjList(inVertices, i, getWeight(g->vertices[i], v));
		}
	}
	return inVertices;
}

// Given a graph, displays it
// Function taken and modified from COMP2521 Week 4 Lecture, April 2019
void  showGraph(Graph g) {
	assert(g != NULL);
	int i;

	printf("Number of vertices: %d\n", g->nV);
	printf("Number of edges: %d\n", g->nE);
	for (i = 0; i < g->nV; i++) {
		printf("%d - ", i);
		showAdjList(g->vertices[i]);
	}
}

// Given a graph, frees all memory associated with it
void  freeGraph(Graph g) {
	assert(g != NULL);
	int i;

	for (i = 0; i < g->nV; i++) freeAdjList(g->vertices[i]);

	free(g);
}

/*
AdjList helper functions
*/

// Given vertex and weight, returns pointer to adjListNode
// Function taken and modified from COMP2521 Week 4 Lecture, April 2019
static adjListNode *newAdjListNode(Vertex vtx, int wt) {
	adjListNode *new = malloc(sizeof(adjListNode));
	assert(new != NULL);
	new->w = vtx;
	new->weight = wt;
	new->next = NULL;
	return new;
}

// Given AdjList and vertex, checks if vertex is in AdjList
// Function taken and modified from COMP2521 Week 4 Lecture, April 2019
static int inAdjList(AdjList L, Vertex vtx) {
	if (L == NULL) {
		return FALSE;
	}
	if (L->w == vtx) {
		return TRUE;
	}

	return inAdjList(L->next, vtx);
}

// Given AdjList, vertex and weight, adds node to AdjList
// Function taken and modified from COMP2521 Week 4 Lecture, April 2019
static AdjList insertAdjList(AdjList L, Vertex vtx, int wt) {
	if (inAdjList(L, vtx)) return L;

	adjListNode *new = newAdjListNode(vtx, wt);
	new->next = L;
	return new;
}

// Given AdjList and vertex, removes vertex from AdjList
// Function taken and modified from COMP2521 Week 4 Lecture, April 2019
// Possible memory leak from redirection of pointers and no freeing of memory
static AdjList removeAdjListNode(AdjList L, Vertex vtx) {
	if (L == NULL) return L;
	if (L->w == vtx) return L->next;

	L->next = removeAdjListNode(L->next, vtx);
	return L;
}

// Given AdjList, prints it out
// Function taken and modified from COMP2521 Week 4 Lecture, April 2019
static void showAdjList(AdjList L) {
	if (L == NULL) putchar('\n');
	else {
		printf("%d", L->w);
		showAdjList(L->next);
	}
}


// Given AdjList and vertex, returns weight of vertex in AdjList
// Can only be used when already checked that vertex is in L
static int getWeight(AdjList L, Vertex v) {
	assert(L != NULL);
	if (L->w == v) return L->weight;

	while (L->next->w != v) {
		L = L->next;
	}
	return L->next->weight;
}

// Given AdjList, frees memory associated with it
// Function taken and modified from COMP2521 Week 4 Lecture, April 2019
static void freeAdjList(AdjList L) {
	if (L != NULL) {
		freeAdjList(L->next);
		free(L);
	}
}

