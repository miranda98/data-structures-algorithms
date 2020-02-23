/*Lance-Williams Algorithm for Agglomerative Clustering
  Written by Jason & Miranda
  COMP2521 2019 T1
*/
#include <stdlib.h>
#include <stdio.h>
#include "LanceWilliamsHAC.h"
#include "Graph.h"
#include <limits.h>
#include <float.h>
#include <assert.h>
#include <math.h>
#define numVertices numVerticies

#define INF 999999
#define BLANK -1
#define SINGLE 1
#define COMPLETE 2
#define NEW_VERTEX -2

// Function prototypes
static double LWFormula(int method, double dist_ik, double dist_jk);

/* 
 * Finds Dendrogram using Lance-Williams algorithm (as discussed in the specs) 
   for the given graph g and the specified method for agglomerative clustering.
   Method value 1 represents 'Single linkage', and 2 represents 'Complete linkage'.
   For this assignment, you only need to implement the above two methods. 

   The function returns 'Dendrogram' structure (binary tree) with the required information.
 * 
 */
Dendrogram LanceWilliamsHAC(Graph g, int method) {
    int numV = numVerticies(g);
    Dendrogram finalDend = malloc(sizeof(DNode));

    // Edge case: empty graph
    if (numV == 0) {
        return NULL;
    }

    // Edge case: one node in graph
    if (numV == 1) {
        finalDend->vertex = 0;
        finalDend->right = finalDend->left = NULL;
        return finalDend;
    }

	// Variables for clusters
	Vertex Ci;
	Vertex Cj;

    // Variables for counters
	int i = 0;
	int j = 0;
    int k = 0;

    // Create distance matrix
    double dist[numV][numV];

	// Set every distance in the matrix to infinity, except when the source = dest
	for (i = 0; i < numV; i++) {
		for (j = 0; j < numV; j++) dist[i][j] = INF;
		dist[i][i] = BLANK;
	}

    // Set 'minimum distance' to infinity for finding minimum distance between clusters
    double min = INF;

	// Get distances between every node in the graph
	AdjList outList;
	for (i = 0; i < numV; i++) {
		// List of outgoing nodes and weights FROM i
		outList = outIncident(g, i);

		while (outList != NULL) {
			double weight = outList->weight;
			double distance = 0;

			if (weight != 0) {
				distance = 1/weight;
			} else {
				distance = INF; // 1/0 = INF
			}

			// Set weights of existing edges
			if (outList->weight != 0) {
				// If the distance w-i is smaller than distance i-w, store smaller
				// distance in the matrix for both i-w and w-i (symmetric)
				if (distance < dist[i][outList->w]) {
					dist[i][outList->w] = distance;
                	dist[outList->w][i] = distance;
				}
			}
			
			// Find minimum distance between clusters
			// Store the variables of the closest clusters
			if (distance < min) {
				min = distance;
				Ci = i;
				Cj = outList->w;
			}
			outList = outList->next;
		}
	}

	// Dendrogram Array - Array of clusters
	Dendrogram dendA[numV];

	// Initially, dendA size is the same as the number of nodes in the graph
	// Initialise dendA
	// Each cell in the array will point to 1 dendrogram which has a single vertex in it
	for (i = 0; i < numV; i++) {
		dendA[i] = malloc(sizeof(DNode));
		dendA[i]->vertex = i;
		dendA[i]->left = NULL;
		dendA[i]->right = NULL;
	}
    
    // Merge clusters
    for (i = 0; i < numV; i++) {
        
        //Initialise new dendrogram
        finalDend = malloc(sizeof(DNode));
        finalDend->vertex = BLANK;
        finalDend->left = dendA[Ci];
        finalDend->right = dendA[Cj];
        
        dendA[Ci] = finalDend;
        
        //Update distances using LW formula
        for (k = 0; k < numV; k++) {
            if (k != Ci) {
                dist[Ci][k] = LWFormula(method, dist[Ci][k], dist[Cj][k]);
                dist[k][Ci] = dist[Ci][k];
            }
        }
    }

	for (i = 0; i < numV; i++) {

		if (dendA[i] != NULL) {
			finalDend = dendA[i];
		}
	}

	return finalDend;

}

void freeDendrogram(Dendrogram d) {

    if (d != NULL) {
     	freeDendrogram(d->left);
     	freeDendrogram(d->right);
        //free(d);
    }
    free(d);
}

// Lance Williams formula given the method (single or complete)
// Input the distance between clusters I-K and the distance between clusters J-K
static double LWFormula(int method, double dist_ik, double dist_jk) {

	double LWDist = 0;

	// For the SINGLE method, take the smaller distance
	if (method == SINGLE) {
		LWDist = (0.5*(dist_ik) + 0.5*(dist_jk) - 0.5*fabs(dist_ik - dist_jk));
	}

	// For COMPLETE method, take the larger distance
	if (method == COMPLETE) {
		LWDist = (0.5*(dist_ik) + 0.5*(dist_jk) + 0.5*fabs(dist_ik - dist_jk));
	}

	return LWDist;

}