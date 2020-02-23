// Dijkstra ADT interface for Ass2 (COMP2521)
// By Jason and Miranda - April 2019
#include "Dijkstra.h"
#include "PQ.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <limits.h>


#define INF INT_MAX

/*
PredNode Linked List helper function prototypes
*/

static PredNode *newPredNode(int val);
static void appendPredNode(PredNode *head, int val);
static void clearPredList(PredNode *head);

/*
Dijsktra's shortest path
*/

// Given a graph and a vertex in the graph, returns ShortestPaths
// struct showing shortest paths to all nodes from vertex given
ShortestPaths dijkstra(Graph g, Vertex v) {
	assert (g != NULL);

	// Initialise shortest paths
	ShortestPaths paths;
	paths.src = v;		// vertex v is the source
	paths.noNodes = numVerticies(g);
	paths.dist = malloc(paths.noNodes*sizeof(int));
	paths.pred = malloc(paths.noNodes*sizeof(PredNode*));

	// Initialise all distances to infinity
	// Initialise all values in predecessor array to point to NULL
	int i; // Counter to increment through the arrays

    for (i = 0; i < paths.noNodes; i++) {
		paths.dist[i] = INF;
		paths.pred[i] = NULL;
    }

	// Set distance from source->source to 0
	paths.dist[v] = 0;

	// Create new priority queue
	PQ pq = newPQ();

	// Add source vertex to priority queue
	ItemPQ source;
	source.key = paths.src;		// = v 
	source.value = 0;	// No weight
	addPQ(pq, source);

	// While there are still items in the PQ
	while (!PQEmpty(pq)) {

	    // Dequeue 1 node
	    ItemPQ curr = dequeuePQ(pq);

	    // Variables for clarity
	    Vertex currVertex = curr.key;
	    int currDist = curr.value;

	    // Find all edges going out from vertex
	    AdjList currAdj = outIncident(g, currVertex);

	    // Loop through all adj edges going out from curr node
	   	while (currAdj != NULL) {

	   		Vertex currAdjVertex = currAdj->w;
	   		int currAdjDist = currAdj->weight;

	   		// If not back at source
	   		if (currAdjVertex != v) {

	   			// If unvisited node
	   			if (paths.pred[currAdjVertex] == NULL) {

	   				// Update ShortestPaths
	   				paths.pred[currAdjVertex] = newPredNode(currVertex);
	   				paths.dist[currAdjVertex] = currDist + currAdjDist;

	   				// Add new vertex to PQ
	   				ItemPQ new;
	   				new.value = currDist + currAdjDist;
	   				new.key = currAdjVertex;
	   				addPQ(pq, new);
	   			
	   			// If visited node
	   			// If new distance from source to curr node + distance from curr node to currAdj node
	   			// <= existing shortest path from source to currAdj node in the PQ
	   			} else if (currDist + currAdjDist <= paths.dist[currAdjVertex]) {

	    			// If the new distance is a new SHORTER path
	   				if (currDist + currAdjDist < paths.dist[currAdjVertex]) {

	   					// Clear the predLL
	   					clearPredList(paths.pred[currAdjVertex]);
	   					paths.pred[currAdjVertex] = NULL;

	   					// Update PQ
	   					ItemPQ new;
	   					new.value = currDist + currAdjDist;
	   					new.key = currAdjVertex;
	   					addPQ(pq, new); // Add should include update

	   				}

	    			// If predLL is empty, make head point to new node
	   				if (paths.pred[currAdjVertex] == NULL) {
	   					paths.pred[currAdjVertex] = newPredNode(currVertex);

	   				// If predLL already has a value in it, append
	   				} else {
	   					appendPredNode(paths.pred[currAdjVertex], currVertex);
	   				}

	   				// Update information in paths
	    			paths.dist[currAdjVertex] = currDist + currAdjDist;
	   			}
   			}
	    	// Go to next adjacent node
	   		currAdj = currAdj->next;
	    }
	}

	// If there is no path (distance == infinity), set distance to 0
	for (i = 0; i < paths.noNodes; i++) {
		if (paths.dist[i] == INF) {
			paths.dist[i] = 0;
		}
	}

	freePQ(pq);
	return paths;
}

// Given ShortestPaths struct, prints out all information
void showShortestPaths(ShortestPaths paths) {
	assert(paths.noNodes != 0);
	// assert(paths != NULL)

	int i;
	int j;

	for (i = 0; i < paths.noNodes; i++) {
		printf("Node %d\nDistance\n", i);

		for (j = 0; j < paths.noNodes; j++) {
			if (i == j) {
				printf("%d : X", j);
			} else {
				printf("%d : %d", j, paths.dist[j]);
			}
		}
		printf("\nPreds: \n");
		for (j = 0; j < paths.noNodes; j++) {
			printf("%d : ", j);

			PredNode *curr = paths.pred[j];
			while (curr != NULL) {
				printf("[%d]->", curr->v);
				curr = curr->next;
			}
			printf("NULL\n");
		}
	}
}

// Given ShortestPaths struct, will free all memory associated with it
void freeShortestPaths(ShortestPaths paths) {
	assert(paths.noNodes != 0);
	// assert(paths != NULL)

	int i;
	for (i = 0; i < paths.noNodes; i++) {
		assert (paths.pred != NULL);
		free(paths.pred[i]);
	}
	free(paths.pred);
	free(paths.dist);
}

/*
Predecessor linked list helper functions
*/

// Given a value, returns a pointer to a new PredNode with value in it
static PredNode *newPredNode(int val) {
	PredNode *new = malloc(sizeof(PredNode));
	assert(new != NULL);
	new->v = val;
	new->next = NULL;

	return new;
}

// Given a pointer to a PredNode and an integer, finds the next available slot and
// appends a new node with value equal to the integer given
static void appendPredNode(PredNode *head, int val) {
	assert(head != NULL);

	PredNode *new = newPredNode(val);
	PredNode *head2 = head;

	// Loop until null
	while (head2->next != NULL) {
		head2 = head2->next;
	}

	if(val != head2->v) {
		head2->next = new;
	}
}

// Given a pointer to a PredNode, frees the rest of the linked list following the node
// and sets the first node value to NULL and next to NULL
static void clearPredList(PredNode *head) {
	// If already empty, return
	if (head == NULL) return;

	PredNode *curr = head;
	PredNode *tmp = head;

	while (tmp != NULL) {
		curr = tmp->next;
		free(tmp);
		tmp = curr;
	}
}
