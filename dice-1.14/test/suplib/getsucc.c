
#include <stdio.h>
#include <suplib/lists.h>

void *
GetSucc(struct Node *node)
{
    struct Node *next = node->ln_Succ;

    if (next->ln_Succ == NULL)
	next = NULL;
    return((void *)next);
}

