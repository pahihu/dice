
/*
 *  Misc. support routines
 */

#include "defs.h"

int
align(int n)
{
    if (n & 3)
	return(4 - (n & 3));
    return(0);
}

