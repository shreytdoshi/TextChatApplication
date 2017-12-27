// A complete working C program to demonstrate all insertion methods
// on Linked List
//Taken from geeksforgeeks.com
#include <stdio.h>
#include <stdlib.h>


#include "../include/global.h"
#include "../include/logger.h"

// A linked list node
struct Node
{
	int port;
	int fd;
	char * ip;
	char * hostname;
	struct Node *next;
};

/* Given a reference (pointer to pointer) to the head of a list and
   an int, inserts a new node on the front of the list. */
void push(struct Node** head_ref, int port, char * ipAddress, char * hostname, int fd)
{
	/* 1. allocate node */
	struct Node* new_node = (struct Node*) malloc(sizeof(struct Node));

	/* 2. put in the data  */
	new_node->port  = port;
	new_node->ip = ipAddress;
	new_node->hostname = hostname;
	new_node->fd = fd;

	/* 3. Make next of new node as head */
	new_node->next = (*head_ref);

	/* 4. move the head to point to the new node */
	(*head_ref)    = new_node;
}

/* See http://www.geeksforgeeks.org/?p=3622 for details of this
   function */
struct Node* SortedMerge(struct Node* a, struct Node* b)
{
	struct Node* result = NULL;

	/* Base cases */
	if (a == NULL)
		return(b);
	else if (b==NULL)
		return(a);

	/* Pick either a or b, and recur */
	if (a->port <= b->port)
	{
		result = a;
		result->next = SortedMerge(a->next, b);
	}
	else
	{
		result = b;
		result->next = SortedMerge(a, b->next);
	}
	return(result);
}

/* UTILITY FUNCTIONS */
/* Split the nodes of the given list into front and back halves,
     and return the two lists using the reference parameters.
     If the length is odd, the extra node should go in the front list.
     Uses the fast/slow pointer strategy.  */
void FrontBackSplit(struct Node* source,
		struct Node** frontRef, struct Node** backRef)
{
	struct Node* fast;
	struct Node* slow;
	if (source==NULL || source->next==NULL)
	{
		/* length < 2 cases */
		*frontRef = source;
		*backRef = NULL;
	}
	else
	{
		slow = source;
		fast = source->next;

		/* Advance 'fast' two nodes, and advance 'slow' one node */
		while (fast != NULL)
		{
			fast = fast->next;
			if (fast != NULL)
			{
				slow = slow->next;
				fast = fast->next;
			}
		}

		/* 'slow' is before the midpoint in the list, so split it in two
      at that point. */
		*frontRef = source;
		*backRef = slow->next;
		slow->next = NULL;
	}
}

/* sorts the linked list by changing next pointers (not data) */
void MergeSort(struct Node** headRef)
{
	struct Node* head = *headRef;
	struct Node* a;
	struct Node* b;

	/* Base case -- length 0 or 1 */
	if ((head == NULL) || (head->next == NULL))
	{
		return;
	}

	/* Split head into 'a' and 'b' sublists */
	FrontBackSplit(head, &a, &b);

	/* Recursively sort the sublists */
	MergeSort(&a);
	MergeSort(&b);

	/* answer = merge the two sorted lists together */
	*headRef = SortedMerge(a, b);
}

/* Function to print nodes in a given linked list */
void printList(struct Node *node)
{
	while(node!=NULL)
	{
		int count = 1;
		cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", count, node->hostname, node->ip, node->port);
		node = node->next;
		count++;
	}
}

