#ifndef  CLIB_DTCLASS_PROTOS_H
#define  CLIB_DTCLASS_PROTOS_H

/*
**	C prototypes. For use with 32 bit integers only.
**
*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef  INTUITION_INTUITION_H
#include <intuition/intuition.h>
#endif
#ifndef  INTUITION_CLASSES_H
#include <intuition/classes.h>
#endif
Class *ObtainEngine( VOID );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif   /* CLIB_DTCLASS_PROTOS_H */
