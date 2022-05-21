
/*
 *  RUN.C
 */

#include "defs.h"

#ifdef sgi
#define vfork	fork
#endif

typedef struct CommandLineInterface CLI;
typedef struct Process		    Process;

Prototype long Execute_Command(char *, short);
Prototype void InitCommand(void);
Prototype long LoadSegLock(long, char *);

char RootPath[512];

extern struct Library *SysBase;

void
InitCommand(void)
{
    getcwd(RootPath, sizeof(RootPath));
}

/*
 *  cmd[-1] is valid space and, in fact, must be long word aligned!
 */

long
Execute_Command(char *cmd, short ignore)
{
    register char *ptr;

    for (ptr = cmd; *ptr && *ptr != ' ' && *ptr != '\t' && *ptr != '\n'; ++ptr)
	;


    /*
     *	Internal MakeDir because AmigaDOS 2.04's MakeDir is unreliable
     *	with RunCommand() (it can crash)
     *
     *	Internal CD because we special case it
     */

    if (ptr - cmd == 2 && strncasecmp(cmd, "cd", 2) == 0) {
	short err = 0;

	while (*ptr == ' ' || *ptr == '\t')
	    ++ptr;
	{
	    short len = strlen(ptr);	/*  XXX HACK HACK */
	    if (len && ptr[len-1] == '\n')
		ptr[len-1] = 0;
	}
	if (*ptr == 0)
	    err = chdir(RootPath);
	else
	    err = chdir(ptr);
	if (err != 0) {
	    err = 20;
	    printf("Unable to cd %s\n", ptr);
	}
	return((ignore) ? 0 : err);
    }

    /*
     * run command cmd
     *
     */

    {
	int err;

	if ((err = vfork()) == 0) {
	    execlp("/bin/sh", "/bin/sh", "-c", cmd, 0);
	    exit(30);
	} else {
	    int uwait;

	    while (wait(&uwait) != err || WIFEXITED(uwait) == 0)
		;
	    err = WEXITSTATUS(uwait);
	}
	if (err)
	    printf("Exit code %d %s\n", err, (ignore) ? "(Ignored)":"");
	if (ignore)
	    return(0);
	return(err);
    }
}

