
#include <stdio.h>
#include <suplib/lists.h>

void *
GetHead(struct List *list)
{
    struct Node *node = list->lh_Head;

    if (node->ln_Succ == NULL)
	node = NULL;
    return((void *)node);
}

