// PQ ADT interface for Ass2 (COMP2521)
// By Jason and Miranda - April 2019
#include "PQ.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

// Priority Queue representation struct taken and modified from COMP2521 Week 6 Lecture, April 2019
struct PQRep {
	ItemPQ *items;	// Array of ItemPQs
	int nItems;		// # of ItemPQs in array
	int nSlots; 	// # of elements in array
} PQRep;

// Function prototypes
static void fixUp(ItemPQ a[], int i);
static void fixDown(ItemPQ a[], int i, int N);
static void swap(ItemPQ a[], int i, int j);
static int less(ItemPQ a, ItemPQ b);

// Returns a new PQ
// Function taken and modified from COMP2521 Week 6 Lecture, April 2019
PQ newPQ() {
	PQ new = malloc(sizeof(PQRep));
	assert(new != NULL);

	// Allocate memory for array
	new->items = malloc(sizeof(ItemPQ)*10);
	new->nItems = 0;
	new->nSlots = 10;
	assert(new->items != NULL);

   return new;
}

// Given a PQ, checks if it's empty
int PQEmpty(PQ pq) {
	return (pq->nItems == 0);
}

// Given a PQ and an ItemPQ, inserts the item into the PQ
// Function taken and modified from COMP2521 Week 6 Lecture, April 2019
void addPQ(PQ pq, ItemPQ element) {
	
	int i;
	int exists = 0; // Boolean tracking if element already in PQ

	// Loop through all elements in pq->items to check if element already exists
	for (i = 1; i < pq->nItems+1; i++) {
		// Check if current element is equal
		if (pq->items[i].key == element.key) {
			exists = 1;
		}
	}

	// If it already exists in the PQ
	if (exists == 1) {
		// Update don't add
		updatePQ(pq, element);
	// Else, add to PQ
	} else {
		pq->nItems++;

		// Check if memory ran out
		if (pq->nItems == pq->nSlots) {

			// Reallocate double the memory
			pq->items = realloc(pq->items, sizeof(ItemPQ)*(pq->nSlots*2));
			pq->nSlots = pq->nSlots*2;
		}
		
		pq->items[pq->nItems] = element;
		fixUp(pq->items, pq->nItems);
	}
}

ItemPQ dequeuePQ(PQ pq) {
	ItemPQ throwAway = pq->items[1];

	// Overwrite first by last
	pq->items[1] = pq->items[pq->nItems];
	pq->nItems--;

	// Move new root to correct position
	fixDown(pq->items, 1, pq->nItems);
	return throwAway;
}

// Updates item with a given 'key' value, by updating that item's value to the given 'value'.
// If item with 'key' does not exist in the queue, no action is taken
void updatePQ(PQ pq, ItemPQ element) {

	int i;
	int oldValue;
	// Loop through all elements in pq->items
	for (i = 1; i < pq->nItems+1; i++) {
		// Check if current element is the one that needs updating
		if (pq->items[i].key == element.key) {

			// Keep old value for comparison
			oldValue = pq->items[i].value;

			// update the value
			pq->items[i].value = element.value;

			// If new value > oldValue, need to fixDown, not fixUp
			if (element.value > oldValue) {
				fixDown(pq->items, i, pq->nItems);
			} else {
				fixUp(pq->items, i);
			}

			break;
		}
	}
}

void  showPQ(PQ pq) {
	assert (pq != NULL);
	printf("##### PRINTING PQ #####\n");
	printf ("Number of items in the array: %d\n", pq->nItems);
	printf ("Number of elements in the array: %d\n", pq->nSlots);

	int i;

	for (i = 1; i <= pq->nItems; i++) {
		printf("key: %d - value: %d\n", pq->items[i].key, pq->items[i].value);
	}
}

void  freePQ(PQ pq) {
	assert (pq != NULL);
	free(pq->items);
	free(pq);
}

/*
Static helper functions
*/

// Function taken and modified from COMP2521 Week 6 Lecture, April 2019
void fixUp(ItemPQ a[], int i) {
    while (i > 1 && less(a[i],a[i/2])) {
		swap(a, i, i/2);
		i = i/2;  // integer division
    }
}

// Function taken and modified from COMP2521 Week 6 Lecture, April 2019
// Note: function in the lecture is bottom-up while this fixUp should be top-down
static void fixDown(ItemPQ a[], int i, int N) {

	while (2*i <= N) {
		// Computer address of left child
		int j = 2*i;

		// Choose larger of 2 children
		if (j < N && less(a[j+1], a[j])) j++;
		if (less(a[i], a[j])) break;
		swap(a, i, j);

		// Move one level down the heap
		i = j;
	}
}

// Given an array of ItemPQ, indexes i and j, swaps elements at i and j around
// Function taken and modified from COMP2521 Week 6 Lecture, April 2019
static void swap(ItemPQ a[], int i, int j) {
	ItemPQ tmp = a[i];
	a[i] = a[j];
	a[j] = tmp;
}

// Given two ItemPQ, checks if the first is less than the second by value and returns
// a boolean value
static int less(ItemPQ a, ItemPQ b) {
	return (a.value < b.value);
}