
/*
 *  STRPBRK.C
 *
 *    (c)Copyright 1992-1997 Obvious Implementations Corp.  Redistribution and
 *    use is allowed under the terms of the DICE-LICENSE FILE,
 *    DICE-LICENSE.TXT.
 */

#include <string.h>

#ifndef HYPER
#define HYPER(x) x
#endif

char *
HYPER(strpbrk)(str, toks)
const char *str;
const char *toks;
{
    char c;

    while (c = *str) {
	const char *p = toks;
	while (*p) {
	    if (*p == c)
		return(str);
	    ++p;
	}
	++str;
    }
    return(NULL);
}

