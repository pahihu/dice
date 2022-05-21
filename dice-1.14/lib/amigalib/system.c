
/*
 *  System
 *
 *    (c)Copyright 1992-1997 Obvious Implementations Corp.  Redistribution and
 *    use is allowed under the terms of the DICE-LICENSE FILE,
 *    DICE-LICENSE.TXT.
 *
 */

#include <exec/types.h>
#ifdef INCLUDE_VERSION
#include <dos/dosextens.h>
#include <utility/tagitem.h>
#endif
#include <clib/dos_protos.h>

#ifndef HYPER
#define HYPER
#endif

typedef struct TagItem	 TagItem;

LONG
HYPER ## System(cmd, tags)
UBYTE *cmd;
TagItem *tags;
{
#ifdef INCLUDE_VERSION
    return(SystemTagList(cmd, tags));
#else
    return 0;
#endif
}

