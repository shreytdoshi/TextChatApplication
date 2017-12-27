#ifndef INCLUDE_LINKEDLIST_H_
#define INCLUDE_LINKEDLIST_H_

struct Node
{
	int port;
	int fd;
	char * ip;
	char * hostname;
	struct Node *next;
};

void push(struct Node** head_ref, int port, char * ipAddress, char * hostname, int fd);
struct Node* SortedMerge(struct Node* a, struct Node* b);
void FrontBackSplit(struct Node* source,
		struct Node** frontRef, struct Node** backRef);
void MergeSort(struct Node** headRef);
void printList(struct Node *node);

#endif /* INCLUDE_LINKEDLIST_H_ */
