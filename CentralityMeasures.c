// Graph ADT interface for Ass2 (COMP2521)
// By Jason & Miranda - April 2019
#include "CentralityMeasures.h"
#include "Dijkstra.h"
#include "PQ.h"
#include <stdlib.h>
#include <stdio.h>

typedef PredNode* PredList;

// Function prototypes
static NodeValues normaliseWF(NodeValues closeness, NodeValues reach);
static NodeValues newNodeValues(int n);
static int countAdjList(AdjList l);
static int calcBetweenness(ShortestPaths sp, double *appearances, int dest);

/*
Functions
*/

// Given a graph, returns a NodeValues struct with out degree centrality calculated
// for each node in the graph
NodeValues outDegreeCentrality(Graph g) {
	// Variable initialisation
	int graphSize = numVerticies(g);
	NodeValues outDeg = newNodeValues(graphSize);
	int i = 0;

	// For each node in the graph, count outDegree
	for (i = 0; i < graphSize; i++) {
		AdjList out = outIncident(g, i);
		outDeg.values[i] = countAdjList(out);
	}

	return outDeg;
}

// Given a graph, returns a NodeValues struct with in degree centrality calculated
// for each node in the graph
NodeValues inDegreeCentrality(Graph g) {
	// Variable initialisation
	int graphSize = numVerticies(g);
	NodeValues inDeg = newNodeValues(graphSize);
	int i = 0;

	// For each node in the graph, count inDegree
	for (i = 0; i < graphSize; i++) {
		AdjList in = inIncident(g, i);
		inDeg.values[i] = countAdjList(in);
	}

	return inDeg;
}

// Given a graph, returns a NodeValues struct with degree centrality calculated
// for each node in the graph
NodeValues degreeCentrality(Graph g) {
	// Variable initialisation
	int graphSize = numVerticies(g);
	NodeValues deg = newNodeValues(graphSize);

	NodeValues outDeg = outDegreeCentrality(g);
	NodeValues inDeg = inDegreeCentrality(g);

	// For each node in the graph, add inDeg and outDeg into deg
	int i;
	for (i = 0; i < graphSize; i++) {
		deg.values[i] = outDeg.values[i] + inDeg.values[i];
	}

	freeNodeValues(outDeg);
	freeNodeValues(inDeg);

	return deg;
}

// Given a graph g, returns a NodeValues struct with closeness centrality calculated
// for each node in the graph
// NOTE: uses dijkstra function which is not working for some tests
NodeValues closenessCentrality(Graph g) {
	// Variable initialisation
	// sumShortestPaths provides the sum of all shortest paths from the indexed node
	// reach provides the number of nodes a particular indexed node can reach
	int graphSize = numVerticies(g);
	NodeValues sumShortestPaths = newNodeValues(graphSize);
	NodeValues reach = newNodeValues(graphSize);
	ShortestPaths sp;

	// For every node in graph, run dijkstra's and create shortest paths variable
	int i;
	for (i = 0; i < numVerticies(g); i++) {
		sp = dijkstra(g, i);

		// For each node, sum all shortest paths and calculate reach
		int j;
		for (j = 0; j < sp.noNodes; j++) {
			// For path to itself i->j it should be 0 so no need to consider
			sumShortestPaths.values[i] = sumShortestPaths.values[i] + sp.dist[j];
			
			// If dist != 0 it means it can reach the node, so increment reach
			// NOTE: THIS DOES NOT INCLUDE ITSELF
			if (sp.dist[j] != 0) reach.values[i]++;

		}

		// Increment since the earlier counter did not include itself as a node it can reach
		reach.values[i]++;
	}
	freeShortestPaths(sp);
	return normaliseWF(sumShortestPaths, reach);
}

// Given two NodeValues structs with the sum of the shortest paths and reach of each node
// in a graph, applies the Wasserman and Faust formula for closeness centrality calculation
// and returns a NodeValues struct with the results
static NodeValues normaliseWF(NodeValues closeness, NodeValues reach) {
	int i = 0;
	double nV = closeness.noNodes; // No. nodes in graph

	// For every node in closeness.values array
	for (i = 0; i < nV; i++) {
		double n = reach.values[i];

		// If closeness is NOT zero
		if (closeness.values[i] != 0) {
			
			// Apply Wasserman and Faust formula
			closeness.values[i] = ((n-1)/(nV-1)) * ((n-1)/(closeness.values[i]));
		}
	}

	freeNodeValues(reach);
	return closeness;
}

// Given a graph g, returns a NodeValues struct with betweenness centrality calculated
// for each node in the graph
NodeValues betweennessCentrality(Graph g) {

	// Variable initialisation
	int graphSize = numVerticies(g);
	NodeValues betweenness = newNodeValues(graphSize);
	ShortestPaths sp;

	double appearances[graphSize]; // Counter for number of appearances of each node
	int spCounter; // Counter for total number of shortest paths between 2 nodes
	int i; // Counter for FROM nodes
	int j; // Counter for TO nodes
	int v; // Counter for betweenness

	// Set all values of appearances to 0
	for (i = 0; i < graphSize; i++) {
		appearances[i] = 0;
	}

	// For each node in graph i
	for (i = 0; i < graphSize; i++) {

		// Find all shortest paths from i->all other nodes using dijkstra's
		sp = dijkstra(g, i);

		// For each node in i->j
		for (j = 0; j < graphSize; j++) {
			
			// if NOT to itself && AT LEAST ONE path exists
			if (i != j && sp.pred[j] != NULL) {

				// Calculate betweenness, populating appearances array and
				// calculating total number of shortest paths between i->j
				spCounter = calcBetweenness(sp, appearances, j);

				// Sum betweenness for each
				for (v = 0; v < graphSize; v++) {
						betweenness.values[v] = betweenness.values[v] + (appearances[v]/(double)spCounter);
						appearances[v] = 0; // Reset appearances array
				}
			}
		}
	}
	freeShortestPaths(sp);
	return betweenness;
}

NodeValues betweennessCentralityNormalised(Graph g) {

	// Variable initialisation
	NodeValues nrmlBetweenness = betweennessCentrality(g);
	int n = numVerticies(g);

	// For each node in nrmlBetweenness
	int i;
	for (i = 0; i < n; i++) {

		// Apply normalisation using formula given
		nrmlBetweenness.values[i] = nrmlBetweenness.values[i]/((n-1)*(n-2));
	}

	return nrmlBetweenness;
}

// Given a NodeValues struct, prints each node and its' value out
void showNodeValues(NodeValues values) {
	int i;

	// Loop through and print
	for (i = 0; i < values.noNodes; i++) {
		printf("%d: %lf\n", i, values.values[i]);
	}
}

// Given a NodeValues struct, frees all memory associated with it
void freeNodeValues(NodeValues values) {
	free(values.values);
}

/* 
STATIC HELPER FUNCTIONS
*/

// Given an integer, returns a new NodeValues struct with noNodes = integer given
static NodeValues newNodeValues(int n) {
	// Variable initialisation
	NodeValues new = {0};
	new.noNodes = n;
	new.values = malloc(n*sizeof(double));

	// Set everything in values to 0
	int i;
	for (i = 0; i < n; i++) new.values[i] = 0;

	return new;
}

// Given an AdjList pointer, returns the length of the AdjList
static int countAdjList(AdjList l) {
	int count = 0;

	// Loop through AdjList and count length
	while (l != NULL) {
		count++;
		l = l->next;
	}

	return count;
}

// Recursive function that:
// - Given a ShortestPaths struct, pointer to array of doubles and a destination vertex
// - Returns the sum of the total number of shortest
//   paths between the source and destination vertex
// - Updates array of doubles to represent the 
//   appearances of each node on all the shortest paths
static int calcBetweenness(ShortestPaths sp, double *appearances, int dest) {
	// If at dest, return 1
	if (sp.src == dest) {
		return 1;
	}

	// If not at dest, continue
	int spCounter = 0;
	int temp;

	// Loop through predLL and recurse
	PredNode *curr = sp.pred[dest];
	while (curr != NULL) {

		temp = calcBetweenness(sp, appearances, curr->v);

		// Update the appearances of vertex v in array
		if (curr->v != sp.src) appearances[curr->v] = appearances[curr->v] + temp;

		// Sum shortest paths found
		spCounter = spCounter + temp;

		// Continue through predLL
		curr = curr->next;
	}
	
	return spCounter;
}