
/*
 *  MAIN.C
 *
 *  (c)Copyright 1990, 1994 Obvious Implementations Corp.  All Rights Reserved
 *
 *  dcc <options> <files>
 */

#include "defs.h"
#ifdef COMMERCIAL
#ifndef unix
#include <lib/rexx.h>
#endif
#endif

#ifndef AZLAT_COMPAT
#define DoLink_Dice	DoLink
#define DoCompile_Dice	DoCompile
#define DoAssemble_Dice DoAssemble
#define DoPrelink_Dice	DoPrelink
#endif

#include "DCC_rev.h"

static char *DCopyright =
"Copyright (c) 1992,1993,1994 Obvious Implementations Corp., All Rights Reserved." VERSTAG;

Prototype   void    myexit(void);
Prototype   int     main(int, char **);
Prototype   void    AddFile(char *);
Prototype   void    help(int);
Prototype   char    *TmpFileName(char *);
Prototype   char    *MungeFile(char *, char *, char *);
Prototype   void    AddName(List *, char *, char *, short);
Prototype   struct NameNode *AddOpt(List *, char *, char *);
Prototype   char    *Tailer(char *);
Prototype   char    *XFilePart(char *);
Prototype   char    *OptListToStr(List *);
Prototype   char    *OptListToStr2(List *, char *);
Prototype   void    run_cmd(char *, char *);
Prototype   int     OutOfDate(char *, char *);
Prototype   void    HandleCFile(struct NameNode *, int);
Prototype   void    HandleAFile(struct NameNode *, int);
Prototype   void    PushTmpFile(char *);
Prototype   void    PopTmpFile(char *);
Prototype   long    LoadSegLock(long, char *);
Prototype   void    DefaultOutName(void);

Prototype   int     DoCompile(char *, char *);
Prototype   int     DoCompile_Dice(char *, char *);
Prototype   int     DoCompile_Aztec(char *, char *);
Prototype   int     DoCompile_Lattice(char *, char *);
Prototype   int     DoAssemble(char *, char *, char *);
Prototype   int     DoAssemble_Dice(char *, char *, char *);
Prototype   int     DoAssemble_Aztec(char *, char *, char *);
Prototype   int     DoAssemble_Lattice(char *, char *, char *);
Prototype   char    *DoPrelink(void);
Prototype   char    *DoPrelink_Dice(void);
Prototype   char    *DoPrelink_Aztec(void);
Prototype   char    *DoPrelink_Lattice(void);
Prototype   int     DoLink(char *);
Prototype   int     DoLink_Dice(char *);
Prototype   int     DoLink_Aztec(char *);
Prototype   int     DoLink_Lattice(char *);
Prototype   char    *PathConvert(char *);
Prototype   void    *GetHead(List *);
Prototype   void    *GetSucc(Node *);
Prototype   void    FlushStoredErrors(char *cfile);
Prototype   int     HandleErrorRexx(char *, char *, int);
Prototype   char    *ScanReplace(char *, char *, char *, char *);
Prototype   char    *FullPathOf(char *);
Prototype   char    *mergestr(const char *s1, const char *s2);

void OrderApp(char *);
void AddLibApp(char *, char);
void DelLibApp(char *, char);
void uexit(int code);

/*
 *  Note that we use exec_dcc if DCC, which only works with 'dcc' programs
 *  thus, the executables are renamed to prevent problems.
 */

Prototype __aligned char Buf[512];
Prototype __aligned char CmdName[64];


__aligned char Buf[512];
__aligned char CmdName[64];
char TmpFile[64];
char ErrOptStr[128];
char *ErrFile;
char *OutFile;
char *OutDir = NULL;
#ifdef AMIGA
char *TmpDir = "T:";
#else
char *TmpDir = "/tmp/";
#endif
char *DLib;
char *AmigaLib;
char *CLib;
char ALibOS[4];
char ALibApp[32] = { "s" };
char CLibApp[32] = { "s" };
char *RexxReplace[10];
List TmpList;
short dasAddSym;
short ErrFileIsTmp;
short NewOpt;
short FastOpt;
short FragOpt;
short ChipOpt;
short MC68020Opt;
short MC68881Opt;
short FFPOpt;
short DDebug;
short RegCallOpt;
short NoHeirOpt;
short NoEnvOpt;
short NoCtlOpt;
short SlashSlashOpt;
short ProfOpt;
short DLinkPostFixOpt;
short UnixOpt;
short UnixRCOpt;
short ForkOpt;
short UnixCommonOpt;
short InlineCompOpt;
short RexxOpt;

long	ExitCode;

char DLINK[32];
char DAS[32];
char DC1[32];
char DCPP[32];

char *RexxHostName = "DCC";

char *SCode[4];
char *IntOpt = "";


typedef struct NameNode {
    struct Node n_Node;
    char    *n_In;
    char    *n_Out;
    short   n_IsType;
} NameNode;

#define IS_CFILE    1
#define IS_AFILE    2
#define IS_OFILE    3
#define IS_LIBFILE  4
#define IS_TMP	    0x100
#define IsMask(istype) ((ubyte)istype)

List   FList;
List   LList;

List   CppOptList;
List   LinkOptList;

short	NoLink;
short	NoAsm;
short	SmallCode = 1;
short	SmallData = 1;
short	ConstCode;	    /*	-ms		    */
short	AbsData;	    /*	-mw, -ma	    */
short	ResOpt;
short	AltSectOpt;
short	SymOpt;
short	RomOpt;
short	ProtoOnlyOpt;
short	NoIntermediateAssembly;
short	PIOpt;
short	GenStackOpt;
short	GenLinkOpt;
short	Verbose;
short	NoDefaultLibs;
short	CompilerOpt = DICE_C;
long	AbsDataStart;	    /*	-mw <addr>  */
char	DebugOpts[64];

NameNode *InlineNode;

extern struct Library *SysBase;

void
myexit()
{
    Node *node;

    while (node = RemHead(&TmpList)) {
	remove(node->ln_Name);
	free(node);
    }
    if (ErrFileIsTmp) {
	fclose(stderr);
	remove(ErrFile);
    }
}

int
main(xac, xav)
int xac;
char *xav[];
{
    int fc = 0;
    int ac;
    char **av;


#ifdef LATTICE
    {
	long n = (long)Buf;
	if (n & 3) {
	    puts("software error, Buf not aligned");
	    uexit(25);
	}
    }
#endif
#ifdef NOTDEF
    expand_args(xac, xav, &ac, &av);
#else
    ac = xac;
    av = xav;
#endif

    NewList(&FList);
    NewList(&LList);

    NewList(&TmpList);

    NewList(&CppOptList);
    NewList(&LinkOptList);

    atexit(myexit);

    if (ac == 1)
	help(0);

    {
	char *ptr = av[0];    /*  cmd name */
	char prefix[32];
	short i;

	for (i = strlen(ptr); i >= 0 && ptr[i] != ':' && ptr[i] != '/'; --i);
	++i;

	ptr = ptr + i;		/*  base name */
	for (i = 0; ptr[i] && ptr[i] != '_'; ++i);
	if (ptr[i] == '_') {
	    strncpy(prefix, ptr, i + 1);
	    prefix[i+1] = 0;
	} else {
	    prefix[0] = 0;
	}
	sprintf(DLINK, "%s%s", prefix, "dlink");
	sprintf(DAS  , "%s%s", prefix, "das");
	sprintf(DC1  , "%s%s", prefix, "dc1");
	sprintf(DCPP , "%s%s", prefix, "dcpp");
	DLib = mergestr(DLIBPRE, prefix);
	DLib = mergestr(DLib, DLIBPOS);
    }
    AmigaLib = mergestr(DLib, "amiga");
    CLib = mergestr(DLib, "c");

    /*
     *  2 x PIOpt + ResOpt
     */

    SCode[0] = mergestr(DLib, "c.o");
    SCode[1] = mergestr(DLib, "c.o");
    SCode[2] = mergestr(DLib, "c_pi.o");
    SCode[3] = mergestr(DLib, "c_pr.o");

    /*
     *	check for -no-env option before processing DCCOPTS enviroment var
     *	check for -no-ctl option before processing DCCOPTS file
     */

    {
	long i;

	for (i = 1; i < ac; ++i) {
	    if (strcmp(av[i], "-no-env") == 0) {
		NoEnvOpt = 1;
		break;
	    }
	    if (strcmp(av[i], "-no-ctl") == 0) {
		NoCtlOpt = 1;
		break;
	    }
	}
    }

    if (NoEnvOpt == 0) {
	char **argv = av;
	ac = ExtArgsEnv(ac, &argv, "DCCOPTS");
	av = argv;
    }
    if (NoCtlOpt == 0) {
	char **argv = av;
	ac = ExtArgsFile(ac, &argv, "DCCOPTS");
	av = argv;
    }

    {
	long i;
	char *dummy;

	for (i = 1; i < ac; ++i) {
	    char *ptr = av[i];

	    if (*ptr == '-') {
		ptr += 2;

		switch(ptr[-1]) {
		case '0':       /*  -020/-030/-040      */
		    MC68020Opt = 1;
		    break;
		case '1':       /*  1.4, 1.3    */
		case '2':       /*  2.0, 2.1..  */
		case '3':
		case '4':
		case '5':
		    if (ptr[0] != '.')
			help(1);
		    AddOpt(&CppOptList, ptr - 2, "");
		    ALibOS[0] = ptr[-1];
		    ALibOS[1] = ptr[1];
		    break;
		case '8':
		    MC68881Opt = 1;
		    break;
		case 'f':
		    if (*ptr == 0)
			FastOpt = 1;
		    else if (*ptr == '0')
			FastOpt = 0;
		    else if (*ptr == 'r') {
			if (stricmp(ptr, "rag0") == 0)
			    FragOpt = 0;
			else
			    FragOpt = 1;
		    } else if (*ptr == 'f')
			FFPOpt = 1;
		    else if (*ptr == 'o') {
			ForkOpt = 1;
			AddLibApp(CLibApp, 'f');
		    }
		    break;
		case 'i':   /*  -ieee   */
		    if (*ptr == 'e')
			FFPOpt = 0;
		    else if (strncmp(ptr, "nt", 2) == 0) {
			IntOpt = (ptr[2]) ? ptr + 2 : av[++i];
		    }
		    break;
		case 'r':
		    if (strcmp(ptr, "om") == 0) {
			RomOpt = 1;
		    } else if (*ptr == '0') {
			ResOpt = 0;
		    } else {
			if (PIOpt && ResOpt == 0)
			    puts("DCC: Warning, -r -pi = -pr");
			ResOpt = 1;
		    }
		    break;
		case 'c':
		    if (*ptr == 0)
			NoLink = 1;
		    else if (stricmp(ptr, "hip") == 0)
			ChipOpt = 1;
		    else if (stricmp(ptr, "hip0") == 0)
			ChipOpt = 0;
		    else
			help(1);
		    break;
		case 'a':
		    if (strcmp(ptr, "ztec") == 0) {
			CompilerOpt = AZTEC_C;
			break;
		    }
		    NoAsm = 1;
		    NoLink= 1;
		    break;
		case 'g':
		    switch (*ptr) {
		    case 's':
			GenStackOpt = 1;
			if (ptr[1] == '0')
			    GenStackOpt = 0;
			break;
		    case 'l':
			GenLinkOpt = 1;
			break;
		    default:
			help(1);
		    }
		    break;
		case 'l':
		    if (strcmp(ptr, "attice") == 0) {
			CompilerOpt = LATTICE_C;
			break;
		    }
		    if (ptr[0] == '0' && ptr[1] == 0) {
			NoDefaultLibs = 1;
			break;
		    }
		    if (*ptr == 0)
			ptr = av[++i];
		    AddName(&LList, ".lib", ptr, IS_LIBFILE);
		    DLinkPostFixOpt = 1;
		    break;
		case 'L':   /*  -Ldir   */
		    if (ptr[0] == '0' && ptr[1] == 0) {
			AddOpt(&LinkOptList, "-L0", "");
			break;
		    }
		    if (*ptr == 0)
			ptr = av[++i];
		    AddOpt(&LinkOptList, "-L", PathConvert(ptr));
		    break;
		case 'I':   /*  -Idir   */
		    if (ptr[0] == '0' && ptr[1] == 0) {
			AddOpt(&CppOptList, "-I0", "");
			break;
		    }
		    if (*ptr == 0)
			ptr = av[++i];
		    AddOpt(&CppOptList, "-I", PathConvert(ptr));
		    break;
		case 'd':   /*  -dice -d<n> -d<debug_opts>  */
		    /*
		     *	note that -d<n> and -s<n> are mutually
		     *	exclusive in that -d1 implies -s and -s0
		     *	implies -d0
		     */

		    if (strcmp(ptr, "ice") == 0) {
			CompilerOpt = DICE_C;
			break;
		    }
		    sprintf(DebugOpts, " -d%s", ptr);

#ifdef LATTICE
		    switch(atoi(ptr)) {
#else
		    switch(strtol(ptr, NULL, 0)) {
#endif
		    case 0:
			SymOpt = 0;
			break;
		    default:
			SymOpt = 1;
		    }
		    break;
		case 'Z':
		    if (atoi(ptr)) {
			DDebug = atoi(ptr);
			break;
		    }
		    break;
		case 'D':   /*  -Ddefine[=str] */
		    if (*ptr == 0)
			ptr = av[++i];
		    AddOpt(&CppOptList, "-D", ptr);
		    break;
		case 'H':   /*  -H<path>=<include_name>    */
		    if (*ptr == 0)
			ptr = av[++i];
		    AddOpt(&CppOptList, "-H", ptr);
		    break;
		case 'u':
		    if (strcmp(ptr, "nixrc") == 0) {
			UnixRCOpt = 1;
		    } else if (strcmp(ptr, "nix") == 0) {
			UnixOpt = 1;
			CLib = mergestr(DLib, "uc");
			AddOpt(&CppOptList, "-unix", "");
		    }
		    break;
		case 'U':   /*  -U      -undefine certain symbols */
		    AddOpt(&CppOptList, "-U", ptr);
		    break;
		case 'o':
		    if (*ptr)
			OutFile = PathConvert(ptr);
		    else
			OutFile = PathConvert(av[++i]);
		    {
			short idx = strlen(OutFile) - 2;
			if (idx >= 0) {
			    if (stricmp(OutFile + idx, ".h") == 0 || stricmp(OutFile + idx, ".c") == 0) {
				puts("ERROR! -o output file may not end in .c or .h!");
				uexit(20);
			    }
			}
		    }
		    break;
		case 'O':
		    if (strcmp(ptr, "0") == 0)
			OutDir = NULL;
		    else if (*ptr)
			OutDir = PathConvert(ptr);
		    else
			OutDir = PathConvert(av[++i]);
		    break;
		case 'E':   /*  error output append */
		    {
			char errtype = '1';
			if (*ptr >= '1' && *ptr <= '9')
			{
			    errtype = *ptr;
			    ptr++;
			}
			if (*ptr == 0)
			ptr = av[++i];

			if (freopen(ptr, "a", stderr)) {
			    if (ErrFile)
				remove(ErrFile);
			    ErrFile = ptr;
			    ErrFileIsTmp = 0;
			    sprintf(ErrOptStr, " -E%c %s", errtype, ptr);
			} else {
			    printf("unable to append to %s\n", ptr);
			}
		    }
		    break;
		case 'p':
		    if (strcmp(ptr, "roto") == 0) {
			ProtoOnlyOpt = 1;
		    } else if (strncmp(ptr, "rof", 3) == 0) {
			if (ptr[3] != '0') {
			    ProfOpt = atoi(ptr + 3);
			    if (ProfOpt == 0)
				ProfOpt = 1;
			    if (ProfOpt >= 2)
				AddLibApp(CLibApp, 'p');
			    if (ProfOpt >= 3)
				AddLibApp(ALibApp, 'p');
			} else {
			    ProfOpt = 0;
			    DelLibApp(ALibApp, 'p');
			    DelLibApp(CLibApp, 'p');
			}
		    } else if (strcmp(ptr, "i") == 0) {
			PIOpt = 1;
			if (ResOpt)
			    puts("DCC: Warning, -r -pi = -pr");
		    } else if (strcmp(ptr, "r") == 0) {
			PIOpt = 1;
			ResOpt = 1;
		    } else {
			help(1);
		    }
		    break;
		case 'T':
		    if (*ptr)
			TmpDir = PathConvert(ptr);
		    else
			TmpDir = PathConvert(av[++i]);
		    break;
		case 'm':
		    switch(*ptr) {
		    case 'C':
			SmallCode = 0;
			break;
		    case 'c':
			SmallCode = 1;
			break;
		    case 'D':
			SmallData = 0;
			DelLibApp(ALibApp, 's');
			DelLibApp(CLibApp, 's');
			AddLibApp(ALibApp, 'l');
			AddLibApp(CLibApp, 'l');
			break;
		    case 'd':
			SmallData = 1;
			DelLibApp(ALibApp, 'l');
			DelLibApp(CLibApp, 'l');
			AddLibApp(ALibApp, 's');
			AddLibApp(CLibApp, 's');
			break;
		    case 'a':
		    case 'w':
			if (ptr[1] == 'a') {
			    AbsData = 0;
			    AbsDataStart = 0;
			} else {
			    AbsData = 1;

			    if (*ptr == 'a')
				AbsData = 2;

			    ++ptr;
			    if (*ptr == 0)
				ptr = av[++i];

#ifdef LATTICE
			    AbsDataStart = atoi(ptr);	/*  bug in lattice */
#else
			    AbsDataStart = strtol(ptr, &dummy, 0);
#endif
			}
			break;
		    case 'r':
			RegCallOpt = 1;
			break;
		    case 'R':
			AddLibApp(CLibApp, 'r');
			AddLibApp(ALibApp, 'r');

			RegCallOpt = 2;
			if (ptr[1] == 'R') {
			    ProtoOnlyOpt = -1;	/*  force prototypes */
			    RegCallOpt = 3;

			    switch(ptr[2]) {
			    case 'X':
				RegCallOpt = 4;
				break;
			    case 'Y':
				RegCallOpt = 5;
				break;
			    case '0':
				RegCallOpt = 0;
				DelLibApp(CLibApp, 'r');
				DelLibApp(ALibApp, 'r');
				if (ProtoOnlyOpt == -1)
				    ProtoOnlyOpt = 0;
				break;
			    }
			}
			break;
		    case 's':
			if (ptr[1])
			    ConstCode = ptr[1] - '0';
			else
			    ConstCode = 1;
			break;
		    case 'S':
			/* XXX REMOVED */
			/*ConstCode = 2;*/
			break;
		    case 'u':
			UnixCommonOpt = 1;
			break;
		    case 'i':
			if (ptr[1] == '0') {
			    if (InlineCompOpt && InlineNode) {
				InlineCompOpt = 0;
				Remove(&InlineNode->n_Node);
				InlineNode = NULL;
			    }
			} else {
			    if (InlineCompOpt == 0) {
				InlineCompOpt = 1;
				InlineNode = AddOpt(&CppOptList, "-D", "__DICE_INLINE");
			    }
			}
			break;
		    default:
			fprintf(stderr, "DCC: bad -m model\n");
			uexit(20);
		    }
		    break;
		case 'R':
#ifdef COMMERCIAL
#ifndef unix
		    /* substitution variables */

		    if (*ptr >= '0' && *ptr <= '9')
			RexxReplace[*ptr - '0'] = (ptr[1]) ? ptr + 1 : av[++i];

		    /* rexx system up?	*/

		    if (RexxSysBase) {
			RexxOpt = 1;
			if (ErrFile == NULL) {
			    char *ptr = TmpFileName(".e");

			    if (freopen(ptr, "a", stderr)) {
				ErrFile = ptr;
				ErrFileIsTmp = 1;
				sprintf(ErrOptStr," -EE %s", ptr);
			    } else {
				printf("unable to append to %s\n", ptr);
			    }
			}
		    } else {
			fprintf(stderr, "DCC: Warning: could open rexxsyslib.library!\n");
		    }
#endif
#endif
		    break;
		case 's':
		    if (strcmp(ptr, "as") == 0) {
			CompilerOpt = LATTICE_C;
		    } else if (strcmp(ptr, "ym") == 0) {
		        dasAddSym = 1;
		    } else if (*ptr == '0') {
			SymOpt = 0;
			DebugOpts[0] = 0;
		    } else {
			SymOpt = 1;
		    }
		    break;
		case 'S':
		    if (*ptr == '0')
			AltSectOpt = 0;
		    else
			AltSectOpt = 1;
		    break;
		case 'v':
		    if (*ptr == '0')
			Verbose = 0;
		    else
			Verbose = 1;
		    break;
		case '/':
		    if (*ptr == '/') {
			if (ptr[1] == '0')
			    SlashSlashOpt = 0;
			else
			    SlashSlashOpt = 1;
			break;
		    }
		    goto def;
		case 'n':
		    if (strcmp(ptr, "ew") == 0) {
			NewOpt = 1;
			break;
		    }
		    if (strcmp(ptr, "ew0") == 0) {
			NewOpt = 0;
			break;
		    }
		    if (strcmp(ptr, "oheir") == 0) {
			NoHeirOpt = 1;
			break;
		    }
		    if (strcmp(ptr, "orom") == 0) {
			RomOpt = 0;
			break;
		    }
		    if (strcmp(ptr, "oproto") == 0) {
			ProtoOnlyOpt = 0;
			break;
		    }
		    if (strcmp(ptr, "o-env") == 0)
			break;
		    if (strcmp(ptr, "o-ctl") == 0)
			break;
		    /* fall through */
		default:
		def:
		    fprintf(stderr, "DCC: bad - option\n");
		    help(1);
		}
		continue;
	    }
	    if (*ptr == '+') {
		ptr += 2;

		switch(ptr[-1]) {
		case 'I':   /*  +Idir   */
		    if (*ptr == 0)
			ptr = av[++i];
		    AddOpt(&CppOptList, "+I", ptr);
		    break;
		default:
		    fprintf(stderr, "DCC: bad + option\n");
		    help(1);
		}
		continue;
	    }
	    if (*ptr == '@') {
		FILE *fi = fopen(ptr + 1, "r");
		char buf[128];

		if (fi == NULL) {
		    printf("unable to open %s\n", ptr + 1);
		    uexit(10);
		}
		while (fgets(buf, sizeof(buf), fi)) {
		    short len = strlen(buf);
		    if (len > 0)
			buf[len-1] = 0;
		    if (buf[0] && buf[0] != ';' && buf[0] != '#') {
			++fc;
			AddFile(buf);
		    }
		}
		fclose(fi);
		continue;
	    }
	    ++fc;
	    AddFile(PathConvert(ptr));
	}
	if (i > ac) {
	    fprintf(stderr, "DCC: file argument missing\n");
	    help(1);
	}
    }

#ifdef AZLAT_COMPAT
    if (CompilerOpt == AZTEC_C) {
	puts("DCC in AZTEC mode");
	FastOpt = 0;
	NoIntermediateAssembly = 1;
    }
    if (CompilerOpt == LATTICE_C) {
	puts("DCC in LATTICE mode");
	FastOpt = 0;
	NoIntermediateAssembly = 1;
    }
#else
    if (CompilerOpt != DICE_C)
	puts("DCC must be recompiled w/ AZLAT_COMPAT defined");
#endif

    /*
     *	Ensure CLibApp and ALibApp ordering and remove duplicates
     */

    OrderApp(CLibApp);
    OrderApp(ALibApp);

    /*
     *	If this run is to generate an executable,
     *
     *	(a) determine the name of the executable and
     *	(b) delete it before compiling anything
     */

    if (NoLink == 0) {
	DefaultOutName();	/*  default output name     */
	remove(OutFile);	/*  make sure it's deleted  */
    }

    /*
     *	Compile sources into assembly
     *	Assemble assembly into objects
     *	Link objects into executable
     */

    {
	NameNode *nn;

	for (nn = GetHead(&FList); nn; nn = GetSucc(&nn->n_Node)) {
	    if (IsMask(nn->n_IsType) == IS_CFILE) {
		HandleCFile(nn, fc);
	    }
	    if (NoAsm == 0 && IsMask(nn->n_IsType) == IS_AFILE) {
		HandleAFile(nn, fc);
	    }
	}
    }

    /*
     *	Link objects into executable
     */

    if (NoLink == 0) {
	char *lfile;

	lfile = DoPrelink();
	if (lfile)
	    PushTmpFile(lfile);
	DoLink(lfile);
	if (lfile) {
	    PopTmpFile(lfile);
	    remove(lfile);
	    free(lfile);
	}
    }
    uexit(ExitCode);
}

void
AddFile(ptr)
char *ptr;
{
    char *t = Tailer(ptr);

    if (strncmp(t, "a", 1) == 0) {
	AddName(&FList, NULL, ptr, IS_AFILE);
    } else
    if (strncmp(t, "o", 1) == 0) {
	AddName(&FList, NULL, ptr, IS_OFILE);
    } else
    if (strncmp(t, "l", 1) == 0) {
	AddName(&FList, NULL, ptr, IS_OFILE);
    } else {
	AddName(&FList, NULL, ptr, IS_CFILE);
    }
}

DoCompile_Dice(in, out)
char *in;
char *out;
{
    char *qq = "";
    char *cptmp = TmpFileName(".i");
    char *code = (SmallCode) ? " -mc" : " -mC";
    char *data = (SmallData) ? " -md" : " -mD";
    char *rc = qq;
    char *absdata;
    char *concode;
    char *res  = (ResOpt) ? " -r" : qq;
    char *verb = (Verbose) ? " -v" : qq;
    char *optsect = (AltSectOpt) ? " -S" : qq;
    char *protoonly = (ProtoOnlyOpt) ? " -proto" : qq;
    char *prof = (ProfOpt) ? " -prof" : qq;
    char *mc68020 = (MC68020Opt) ? " -020" : qq;
    char *mc68881 = (MC68881Opt) ? " -881" : qq;
    char *piopt;
    char *ffp = (FFPOpt) ? " -ffp" : qq;
    char *genstack = (GenStackOpt) ? " -gs" : qq;
    char *genlink  = (GenLinkOpt) ? " -gl" : qq;
    char *slashopt = (SlashSlashOpt) ? " -//" : qq;
    char *forkopt = (ForkOpt) ? " -fork" : qq;
    char *unixcomopt = (UnixCommonOpt) ? " -mu" : qq;
    char *asmopt = (NoAsm) ? " -a" : qq;
    char *intopt = (IntOpt[0]) ? " -int" : qq;

    switch(RegCallOpt) {
    case 1:
	rc = " -mr";
	break;
    case 2:
	rc = " -mR";
	break;
    case 3:
	rc = " -mRR";
	break;
    case 4:
	rc = " -mRRX";
	break;
    case 5:
	rc = " -mRRY";
	break;
    }

    switch(ConstCode) {
    case 1:
	concode = " -ms";
	break;
    case 2:
	concode = " -mS";
	break;
    default:
	concode = qq;
	break;
    }

    switch(AbsData) {
    case 1:
	absdata = " -mw";
	break;
    case 2:
	absdata = " -ma";
	break;
    default:
	absdata = qq;
	break;
    }

    if (PIOpt) {
	if (ResOpt)
	    piopt = " -pr";
	else
	    piopt = " -pi";
	res = qq;
	absdata = qq;
	code = qq;
	data = qq;
    } else {
	piopt = qq;
    }

    PushTmpFile(cptmp);
    sprintf(Buf, "%s %s -o %s%s%s%s%s",
	DCPP, in, cptmp, ErrOptStr, OptListToStr(&CppOptList),
	ffp, slashopt
    );
    run_cmd(in, Buf);
    sprintf(Buf, "%s %s -o %s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
	DC1, cptmp, out, code, data, rc, res, verb,
	optsect, protoonly, prof, concode, absdata, piopt, ErrOptStr,
	mc68020, mc68881, ffp, genstack, genlink, forkopt, unixcomopt,
	asmopt, intopt, IntOpt,
	DebugOpts
    );
    run_cmd(in, Buf);
    PopTmpFile(cptmp);
    remove(cptmp);
    free(cptmp);
    return(0);
}

DoAssemble_Dice(cpath, in, out)
char *cpath;
char *in;
char *out;
{
    short n;

    n = sprintf(Buf, "%s %s-o%s %s%s", DAS, (dasAddSym ? "-s ":""), out, in, ErrOptStr);
    if (cpath)
	sprintf(Buf+n, " -N%s", cpath);
    run_cmd(cpath, Buf);
    return(0);
}

char *
DoPrelink_Dice(void)
{
    NameNode *nn;
    char *ltmp = TmpFileName(".lnk");
    FILE *fi = fopen(ltmp, "w");

    if (fi == NULL) {
	fprintf(stderr, "DCC: couldn't create %s\n", ltmp);
	uexit(20);
    }

    for (nn = GetHead(&FList); nn; nn = GetSucc(&nn->n_Node)) {
	if (IsMask(nn->n_IsType) == IS_OFILE) {
	    fputs(nn->n_In, fi);
	    putc('\n', fi);
	}
    }
    while (nn = (NameNode *)RemHead(&LList)) {
	fputs(nn->n_In, fi);
	putc('\n', fi);
    }

    /*
     *	only small-data version of auto.lib is supported currently
     */

    if (RomOpt == 0 && NoDefaultLibs == 0) {
	fprintf(fi, "%s%s.lib", CLib, CLibApp);
	fprintf(fi, " %s%s%s.lib %sauto%s.lib\n",
	    AmigaLib, ALibOS, ALibApp, DLib, (SmallData ? "s" : "l")
	);
    }
    fclose(fi);
    return(ltmp);
}

/*
 *  dlib:x.o is a special trailer for any autoinit code (in section autoinit,
 *  code)
 *
 *  This section is called in sequence just before main() with ac, av pushed on
 *  the stack.	The idea is that any module may reference an autoinit section to
 *  automatically initialize certain aspects of itself without requiring a call
 *  from the main program.
 */

DoLink_Dice(lfile)
char *lfile;
{
    char *qq = "";
    char *co = " ";
    char *ro = mergestr(DLib, "x.o");
    char *symopt = (SymOpt) ? " -s" : qq;
    char *resopt = (ResOpt) ? " -r" : qq;
    char *fragopt= (FragOpt) ? " -frag" : qq;
    char *chipopt= (ChipOpt) ? " -chip" : qq;
    char *postfix= qq;
    char *piopt;
    char absdata[20];
    char *verb = (Verbose) ? " -v" : qq;

    if (DLinkPostFixOpt) {
	static char PostFix[sizeof(CLibApp)+3];
	postfix = PostFix;
	sprintf(postfix, " -P%s", CLibApp);
    }

    if (RomOpt == 0 && NoDefaultLibs == 0) {
	co = SCode[(short)((PIOpt << 1) | ResOpt)];
    }

    if (AbsData) {
	sprintf(absdata, " -ma 0x%lx", AbsDataStart);
    } else {
	absdata[0] = 0;
    }

    if (PIOpt) {
	if (ResOpt)
	    piopt = " -pr";
	else
	    piopt = " -pi";
	resopt = qq;
	if (AbsData) {
	    absdata[0] = 0;
	    puts("Warning: cannot mix -pi and -ma/-mw");
	}
    } else {
	piopt = qq;
    }
    if (FragOpt) {
	if (ResOpt) {
	    puts("Warning: cannot use -frag with -r");
	    fragopt = qq;
	}
    }

    sprintf(Buf, "%s %s @%s %s -o %s%s%s%s%s%s%s%s%s%s%s%s",
	DLINK, co, lfile, ro, OutFile,
	symopt,
	resopt,
	fragopt,
	piopt,
	absdata,
	postfix,
	chipopt,
	OptListToStr(&LinkOptList),
	ErrOptStr,
	DebugOpts,
	verb
    );
    run_cmd("", Buf);
    free(ro);

    /*
     * Delete temporary objects
     */

    {
	NameNode *nn;

    	for (nn = GetHead(&FList); nn; nn = GetSucc(&nn->n_Node)) {
	    if (nn->n_IsType & IS_TMP)
		remove(nn->n_In);
	}
    }
    return(0);
}



void
help(code)
{
#ifdef _DCC
    printf("%s\n%s\n", VSTRING, DCopyright);
#endif
    uexit(code);
}

char *
TmpFileName(tail)
char *tail;
{
    char *buf = malloc(strlen(TmpDir) + strlen(tail) + 32);
    char dummy = 0;

    sprintf(buf, "%s%06lx%s", TmpDir, (long)&dummy >> 8, tail);
    return(buf);
}

/*
 *  file    -	file to modify
 *  hdr     -	new directory path to put file
 *  tail    -	new suffix
 *
 */

char *
MungeFile(file, hdr, tail)
char *file;
char *hdr;
char *tail;
{
    char *name = malloc(strlen(file) + (hdr ? strlen(hdr) : 0) + (tail ? strlen(tail) : 0) + 2);
    char *ptr;
    short n = 0;

    /*
     *	Prepend header, replacing the absolute portion of the file
     */

    if (hdr) {
	if (ptr = strchr(file, ':'))
	    file = ptr + 1;
	n += sprintf(name + n, "%s", hdr);

	if (n && name[n-1] != ':' && name[n-1] != '/')
	    name[n++] = '/';
    }

    /*
     *	The file
     */

    if (tail && (ptr = strrchr(file, '.'))) {
	n += sprintf(name + n, "%.*s", ptr - file, file);
    } else {
	n += sprintf(name + n, "%s", file);
    }

    /*
     *	Trailer
     */

    if (tail) {
	n += sprintf(name + n, "%s", tail);
    }
    name[n] = 0;
    return(name);
}

void
AddName(List *list, char *tailifnone, char *file, short isType)
{
    NameNode *nn = malloc(sizeof(NameNode));
    short i;

    for (i = strlen(file) - 1; i >= 0 && file[i] != '.'; --i) {
	if (file[i] == '/' || file[i] == ':')
	    i = 0;
    }

    if (i < 0 && tailifnone) {
	nn->n_In = malloc(strlen(file) + strlen(tailifnone) + 1);
	sprintf(nn->n_In, "%s%s", file, tailifnone);
    } else {
	nn->n_In = malloc(strlen(file) + 1);
	strcpy(nn->n_In, file);
    }
    nn->n_Out = NULL;
    nn->n_IsType= isType;
    AddTail(list, &nn->n_Node);
}

NameNode *
AddOpt(list, opt, body)
List *list;
char *opt;
char *body;
{
    NameNode *nn = malloc(sizeof(NameNode));

    nn->n_In = opt;
    nn->n_Out= body;
    AddTail(list, &nn->n_Node);
    return(nn);
}

char *
Tailer(ptr)
char *ptr;
{
    short i;

    for (i = strlen(ptr) - 1; i >= 0 && ptr[i] != '.'; --i);
    if (i < 0)
	return("");
    return(ptr + i + 1);
}

char *
XFilePart(ptr)
char *ptr;
{
    short i;

    for (i = strlen(ptr) - 1; i >= 0 && ptr[i] != ':' && ptr[i] != '/'; --i);
    ++i;
    return(ptr + i);
}

char *
OptListToStr(list)
List *list;
{
    static char Tmp[512];
    short i;
    NameNode *scan;

    i = 0;
    Tmp[0] = 0;
    for (scan = (NameNode *)list->lh_Head; scan != (NameNode *)&list->lh_Tail; scan = (NameNode *)scan->n_Node.ln_Succ) {
	sprintf(Tmp + i, " %s%s", scan->n_In, scan->n_Out);
	i += strlen(Tmp + i);
    }
    return(Tmp);
}

#ifdef AZLAT_COMPAT

char *
OptListToStr2(list, cvt)
List *list;
char *cvt;
{
    static char Tmp[512];
    short i;
    NameNode *scan;

    i = 0;
    for (scan = (NameNode *)list->lh_Head; scan != (NameNode *)&list->lh_Tail; scan = (NameNode *)scan->n_Node.ln_Succ) {
	sprintf(Tmp + i, " %s%s", scan->n_In, scan->n_Out);
	{
	    char *ptr;
	    for (ptr = cvt; *ptr; ptr += 2) {
		if (Tmp[i+2] == ptr[0])
		    Tmp[i+2] = ptr[1];
	    }
	}
	i += strlen(Tmp + i);
    }
    return(Tmp);
}

#endif

/*
 *  run_cmd(buf)	buf[-1] is valid for BCPL stuff, buf[-1] is
 *			long word aligned.
 */

#ifdef AMIGA

void
run_cmd(cfile, buf)
char *cfile;	    /* REXX support */
char *buf;
{
    short i;
    short j = strlen(buf);
    int r;


    if (ErrFile)
	fclose(stderr);

    /*
     *	top (hack to support REXX fix/continue)
     */

top:
    if (Verbose)
	printf("%s\n", buf);


#if INCLUDE_VERSION >= 36
    if (SysBase->lib_Version >= 36) {
	long seg;
	long lock = NULL;

	Process *proc = (Process *)FindTask(NULL);
	CLI *cli = BTOC(proc->pr_CLI, CLI);
	long oldCommandName;

	dbprintf(("cmd-begin\n"));

	for (i = 0; buf[i] && buf[i] != ' '; ++i)
	    ;
	movmem(buf, CmdName + 1, i);
	CmdName[0] = i;
	CmdName[i+1] = 0;

	if (cli) {
	    oldCommandName = (long)cli->cli_CommandName;
	    cli->cli_CommandName = CTOB(CmdName);
	}

	if (seg = (long)FindSegment(CmdName + 1, 0L, 0)) {
	    r = RunCommand(((long *)seg)[2], 16384, buf + i + 1, strlen(buf + i + 1));
	} else if ((lock = _SearchPath(CmdName + 1)) && (seg = LoadSegLock(lock, ""))) {
	    r = RunCommand(seg, 16384, buf + i + 1, strlen(buf + i + 1));
	    UnLoadSeg(seg);
	} else if ((lock = Lock("dcc:bin", SHARED_LOCK)) && (seg = LoadSegLock(lock, CmdName + 1))) {
	    r = RunCommand(seg, 16384, buf + i + 1, strlen(buf + i + 1));
	    UnLoadSeg(seg);
	} else {
	    printf("Unable to find executable %s resident, via your path\n", CmdName + 1);
	    printf("or in DCC:BIN!\n");
	    r = 20;
	}
	if (lock)
	    UnLock(lock);
	if (cli)
	    cli->cli_CommandName = (BSTR)oldCommandName;

	dbprintf(("cmd-end\n"));
    } else {
#else
    {
#endif

#ifdef _DCC
	if (FastOpt == 0) {
#endif
	    if (Execute(buf, NULL, Output()) != -1) {
		printf("Unable to Execute %s\n", buf);
		uexit(20);
	    }
	    r = 0;

#ifdef _DCC
	} else {
	    for (i = 0; buf[i] && buf[i] != ' '; ++i)
		;
	    buf[i] = 0;
	    if (i != j) {
		for (++i; buf[i] == ' '; ++i)
		    ;
	    }
	    r = exec_dcc(buf, buf + i);
	}
#endif
    }
    if (r)
	printf("Exit code %d\n", r);
#ifdef COMMERCIAL
#ifndef unix
    if (RexxOpt)
    {
	if (r)
	{
	    long rr = HandleErrorRexx(cfile, buf, r);
	    if (rr == 2)
		goto top;
	} else {
	    FlushStoredErrors(cfile);
	}
    }
#endif
#endif
    if (ExitCode < r)
	ExitCode = r;
    if (ExitCode > 5)
	uexit(ExitCode);

    /*
     *	close and reopen file to 'sync' it
     */

    if (r && ErrFile)
	freopen(ErrFile, "a", stderr);
}

#else

#include <sys/wait.h>

void
run_cmd(cfile, buf)
char *cfile;	    /* REXX support */
char *buf;
{
    int r;

    if (Verbose)
	printf("%s\n", buf);

    if ((r = vfork()) == 0) {
	execlp("/bin/sh", "/bin/sh", "-c", buf, 0);
	uexit(30);
    } else {
	union wait uwait;
	while (wait(&uwait) != r || WIFEXITED(uwait) == 0)
	    ;
	r = uwait.w_retcode;
    }

    if (r)
	printf("Exit code %d\n", r);
    if (ExitCode < r)
	ExitCode = r;
    if (ExitCode > 5)
	uexit(ExitCode);
}

#endif

#ifdef AMIGA

int
OutOfDate(in, out)
char *in;
char *out;
{
    static FIB *InFib;
    static FIB *OutFib;
    BPTR inLock, outLock;
    FIB *inFib;
    FIB *outFib;
    int r = 1;

    if (NewOpt == 0)
	return(1);

    if (InFib == NULL) {
	InFib = malloc(sizeof(FIB));
	OutFib = malloc(sizeof(FIB));
    }
    inFib = InFib;
    outFib = OutFib;

    if (inLock = Lock(in, SHARED_LOCK)) {
	if (outLock = Lock(out, SHARED_LOCK)) {
	    if (Examine(inLock, inFib) && Examine(outLock, outFib)) {
		if (inFib->fib_Date.ds_Days < outFib->fib_Date.ds_Days)
		    r = 0;
		else if (inFib->fib_Date.ds_Days == outFib->fib_Date.ds_Days) {
		    if (inFib->fib_Date.ds_Minute < outFib->fib_Date.ds_Minute)
			r = 0;
		    else if (inFib->fib_Date.ds_Minute == outFib->fib_Date.ds_Minute) {
			if (inFib->fib_Date.ds_Tick < outFib->fib_Date.ds_Tick)
			    r = 0;
		    }
		}
	    }
	    UnLock(outLock);
	}
	UnLock(inLock);
    }
    return(r);
}

#else

int
OutOfDate(in, out)
char *in;
char *out;
{
    struct stat instat;
    struct stat outstat;
    int r = 1;

    if (NewOpt == 0)
	return(1);

    if (stat(in, &instat) == 0) {
	if (stat(out, &outstat) == 0) {
	    if (instat.st_mtime < outstat.st_mtime)
		r = 0;
	}
    }
    return(r);
}

#endif

void
HandleCFile(nn, fc)
NameNode *nn;
int fc;
{
    char *asmName;
    char *objName;

    if (fc == 1 && OutFile && NoAsm)
	asmName = OutFile;
    else if (NoAsm)
	asmName = MungeFile(nn->n_In, OutDir, ".a");
    else
	asmName = MungeFile(XFilePart(nn->n_In), TmpDir, ".a");

    nn->n_IsType = IS_OFILE;

    if (fc == 1 && OutFile && NoLink) {
	objName = OutFile;
    } else if (NoLink) {
	objName = MungeFile(nn->n_In, OutDir, ".o");
    } else {
	objName = MungeFile(XFilePart(nn->n_In), TmpDir, ".o");
	nn->n_IsType |= IS_TMP;
    }

    if (NoAsm) {	/*  in -> asmName	    */
	if (OutOfDate(nn->n_In, asmName))
	    DoCompile(nn->n_In, asmName);
    } else {		/*  in -> asmName -> objName*/
	if (OutOfDate(nn->n_In, objName)) {
	    PushTmpFile(asmName);
	    if (NoIntermediateAssembly) {
		DoCompile(nn->n_In, objName);
	    } else {
		DoCompile(nn->n_In, asmName);
		if (NoHeirOpt == 0)
		    CreateObjPath(objName);
		DoAssemble(nn->n_In, asmName, objName);
	    }
	    PopTmpFile(asmName);
	    remove(asmName);
	}
    }
    nn->n_In = strdup(objName);
}

void
HandleAFile(nn, fc)
NameNode *nn;
int fc;
{
    char *objName;

    nn->n_IsType = IS_OFILE;

    if (fc == 1 && OutFile && NoLink) {
	objName = OutFile;
    } else if (NoLink) {
	objName = MungeFile(nn->n_In, OutDir, ".o");
    } else {
	objName = MungeFile(XFilePart(nn->n_In), TmpDir, ".o");
	nn->n_IsType |= IS_TMP;
    }

    if (OutOfDate(nn->n_In, objName)) {
	if (NoHeirOpt == 0)
	    CreateObjPath(objName);
	DoAssemble(NULL, nn->n_In, objName);
    }
    nn->n_In = strdup(objName);
}

void
PushTmpFile(name)
char *name;
{
    Node *node = malloc(sizeof(Node) + strlen(name) + 1);
    if (node == NULL) {
	puts("Ran out of memory!");
	uexit(25);
    }
    node->ln_Name = (char *)(node + 1);
    strcpy(node->ln_Name, name);
    AddHead(&TmpList, node);
}

void
PopTmpFile(name)
char *name;
{
    Node *node = RemHead(&TmpList);

    if (node == NULL || strcmp(name, node->ln_Name) != 0) {
	puts("PopTmpFile: software error");
	uexit(20);
    }
    free(node);
}

#ifdef AMIGA

long
LoadSegLock(lock, cmd)
long lock;
char *cmd;
{
    long oldLock;
    long seg;

    oldLock = CurrentDir(lock);
    seg = LoadSeg(cmd);
    CurrentDir(oldLock);
    return(seg);
}

#endif

void
DefaultOutName(void)
{
    NameNode *nn;

    /*
     *	if no output file name set and only one source file was specified,
     *	set output file name based on said file
     */

    if (OutFile == NULL) {
	OutFile = "a.out";

	if (nn = GetHead(&FList)) {
	    /*if (GetSucc(&nn->n_Node) == NULL)*/ {
		char *ptr = strdup(nn->n_In);
		char *p0;

		if (p0 = strrchr(ptr, '.')) {
		    *p0 = 0;
		    OutFile = ptr;
		}
	    }
	}
    }
}

/*
 *	AZTEC C, LATTICE C COMPATIBILITY OPTIONS
 */

#ifdef AZLAT_COMPAT

DoLink(lfile)
char *lfile;
{
    switch(CompilerOpt) {
    case DICE_C:
	return(DoLink_Dice(lfile));
    case LATTICE_C:
	return(DoLink_Lattice(lfile));
    case AZTEC_C:
	return(DoLink_Aztec(lfile));
    }
}

DoCompile(in, out)
char *in;
char *out;
{
    switch(CompilerOpt) {
    case DICE_C:
	return(DoCompile_Dice(in, out));
    case LATTICE_C:
	return(DoCompile_Lattice(in, out));
    case AZTEC_C:
	return(DoCompile_Aztec(in, out));
    }
    return(0);
}

DoAssemble(cfile, in, out)
char *cfile;
char *in;
char *out;
{
    switch(CompilerOpt) {
    case DICE_C:
	return(DoAssemble_Dice(cfile, in, out));
    case LATTICE_C:
	return(DoAssemble_Lattice(cfile, in, out));
    case AZTEC_C:
	return(DoAssemble_Aztec(cfile, in, out));
    }
    return(0);
}

char *
DoPrelink(void)
{
    switch(CompilerOpt) {
    case DICE_C:
	return(DoPrelink_Dice());
    case LATTICE_C:
	return(DoPrelink_Lattice());
    case AZTEC_C:
	return(DoPrelink_Aztec());
    }
    return(0);
}

/*
 *	------------------------------------------------------------------
 */

DoCompile_Lattice(in, out)
char *in;
char *out;
{
    char *qq = "";
    char *cptmp = TmpFileName(".i");
    char *data = (SmallData) ? qq : " -b0";

    sprintf(Buf, "lc -o%s %s %s %s",
	out, OptListToStr2(&CppOptList, "DdIi"), data, in
    );
    run_cmd(in, Buf);

    free(cptmp);
    return(0);
}

DoAssemble_Lattice(cfile, in, out)
char *cfile;
char *in;
char *out;
{
    sprintf(Buf, "asm -o%s %s", out, in);
    run_cmd(cfile, Buf);
    return(0);
}

char *
DoPrelink_Lattice(void)
{
    NameNode *nn;
    char *ltmp = TmpFileName(".lnk");
    FILE *fi = fopen(ltmp, "w");
    short libs = 0;

    if (fi == NULL) {
	fprintf(stderr, "DCC: couldn't create %s\n", ltmp);
	uexit(20);
    }

    for (nn = GetHead(&FList); nn; nn = GetSucc(&nn->n_Node)) {
	if (IsMask(nn->n_IsType) == IS_OFILE) {
	    fputs(nn->n_In, fi);
	    putc('\n', fi);
	}
    }

    while (nn = (NameNode *)RemHead(&LList)) {
	if (libs == 0) {
	    fprintf(fi, "LIB ");
	    libs = 1;
	}
	fputs(nn->n_In, fi);
	putc('\n', fi);
    }
    if (RomOpt == 0 && NoDefaultLibs == 0) {
	if (libs == 0) {
	    fprintf(fi, "LIB ");
	    libs = 1;
	}
	fprintf(fi, "lib:lc.lib lib:amiga.lib\n");
    }

    fclose(fi);
    return(ltmp);
}

DoLink_Lattice(lfile)
char *lfile;
{
    char *qq = "";
    char *co = " ";
    char *symopt = (SymOpt) ? " ADDSYM" : qq;
    char *scopt = (SmallData) ? " SD" : qq;
    char *sdopt = (SmallCode) ? " SC" : qq;

    if (RomOpt == 0 && NoDefaultLibs == 0) {	   /*  RomOpt PIOpt ResOpt */
	static char *SCode[] = { "lib:c.o",         /*    0      0      0   */
				 "lib:cres.o",      /*    0      0      1   */
				 "lib:c.o",         /*    0      1      0   */
				 "lib:cres.o"       /*    0      1      1   */
			       };
	co = SCode[(short)((PIOpt << 1) | ResOpt)];
    }

    sprintf(Buf, "BLink from %s with %s to %s%s%s%s",
	co, lfile, OutFile, symopt, scopt, sdopt
    );
    run_cmd("", Buf);
    return(0);
}

/*
 *  ---------------------------------------------------------------------
 */

DoCompile_Aztec(in, out)
char *in;
char *out;
{
    char *qq = "";
    char *cptmp = TmpFileName(".i");
    char *data = (SmallData) ? qq : qq;

    sprintf(Buf, "cc %s %s %s -o %s",
	OptListToStr2(&CppOptList, ""), data, in, out
    );
    run_cmd(in, Buf);

    free(cptmp);
    return(0);
}

DoAssemble_Aztec(cfile, in, out)
char *cfile;
char *in;
char *out;
{
    sprintf(Buf, "as %s -o %s", in, out);
    run_cmd(cfile, Buf);
    return(0);
}

char *
DoPrelink_Aztec(void)
{
    NameNode *nn;
    char *ltmp = TmpFileName(".lnk");
    FILE *fi = fopen(ltmp, "w");

    if (fi == NULL) {
	fprintf(stderr, "DCC: couldn't create %s\n", ltmp);
	uexit(20);
    }

    for (nn = GetHead(&FList); nn; nn = GetSucc(&nn->n_Node)) {
	if (IsMask(nn->n_IsType) == IS_OFILE) {
	    fputs(nn->n_In, fi);
	    putc('\n', fi);
	}
    }
    while (nn = (NameNode *)RemHead(&LList)) {
	fputs(nn->n_In, fi);
	putc('\n', fi);
    }
    if (RomOpt == 0 && NoDefaultLibs == 0) {
	fprintf(fi, "-lc\n");
    }
    fclose(fi);
    return(ltmp);
}

DoLink_Aztec(lfile)
char *lfile;
{
    char *qq = "";

    sprintf(Buf, "ln -f %s -o %s", lfile, OutFile);
    run_cmd("", Buf);
    return(0);
}

#endif

void
OrderApp(buf)
char *buf;
{
    short i;
    short c;
    char sort[26];

    setmem(sort, sizeof(sort), 0);
    for (i = 0; c = buf[i]; ++i) {
	if (c >= 'a' && c <= 'z')
	    sort[c-'a'] = 1;
    }
    for (i = sizeof(sort) - 1, c = 0; i >= 0; --i) {
	if (sort[i])
	    buf[c++] = i + 'a';
    }
    buf[c] = 0;
}

void
AddLibApp(char *buf, char c)
{
    short i = strlen(buf);

    if (strchr(buf, c) == NULL) {
	buf[i+0] = c;
	buf[i+1] = 0;
    }
}

void
DelLibApp(char *buf, char c)
{
    char *ptr;

    if (ptr = strchr(buf, c))
	movmem(ptr + 1, ptr, strlen(ptr + 1) + 1);
}

char *
PathConvert(path)
char *path;
{
#ifdef _DCC
    if (strstr(path, "./") || strstr(path, "../"))
	return(strdup(UnixToAmigaPath(path)));
#endif
    return(path);
}

void *
GetHead(list)
List *list;
{
    if (list->lh_Head != (Node *)&list->lh_Tail)
	return(list->lh_Head);
    return(NULL);
}

void *
GetSucc(node)
Node *node;
{
    Node *r = node->ln_Succ;

    if (r->ln_Succ)
	return(r);
    return(NULL);
}

char *
mergestr(const char *s1, const char *s2)
{
    char *ptr = malloc(strlen(s1) + strlen(s2) + 1);
    sprintf(ptr, "%s%s", s1, s2);
    return(ptr);
}


#ifdef COMMERCIAL
#ifndef unix

void
FlushStoredErrors(cfile)
char *cfile;
{
    char *fullPath;
    char *port = "DICE_ERROR_PARSER";

    if ( (cfile != NULL) &&
         (cfile[0])      &&
         (FindPort(port) != NULL) &&
         ((fullPath = FullPathOf(cfile)) != NULL))
    {
        char *cmd;
        char *curDirPath = FullPathOf("");

	cmd = ScanReplace("Load Nil: %n %c", fullPath, "nil:", curDirPath);
	if (cmd != NULL)
	{
	    /* Note that we don't care if this fails for any reason, we  */
	    /* just want to give them an opportunity to purge any errors */
	    /* that have been stored                                     */
	    PlaceRexxCommandDirect(NULL,port,cmd,NULL,NULL);
	    free(cmd);
	}
	if (curDirPath != NULL) free(curDirPath);
	free(fullPath);
    }
}

/*
 *  HANDLEERRORREXX()
 *
 *  Scan DCC:CONFIG/DCC.CONFIG to obtain REXX command execution information
 *  then attempt to run the appropriate command.  If the REXX script returns
 *  2 we retry the compilation.  If the REXX script returns anything else
 *  we abort.
 *
 *  cfile can be "", indicating no source file available to handle error(s)
 */

int
HandleErrorRexx(cfile, cmdbuf, rc)
char *cfile;
char *cmdbuf;
int rc;
{
    FILE *fi;
    int r = -1;
    char *buf;
    char *fullPath;
    char *cmdName = "cmd=";
    char *portName = "port=";
    char *rexxCmdName = "rexxcmd=";
    char *curDirPath = FullPathOf("");


    if (buf = malloc(256)) {
	/*
	 *  obtain full path to cFile, if present
	 */

	if ((cfile != NULL) && cfile[0]) {
	    fullPath = FullPathOf(cfile);
	} else {
	    fullPath = strdup("");
	    cmdName = "linkcmd=";
	    portName = "linkport=";
	    rexxCmdName = "linkrexxcmd=";
	}
	if (fullPath) {
	    /*
	     *	open the config file
	     */

	    if (fi = fopen("DCC:CONFIG/DCC.CONFIG", "r")) {
		char *cmd = NULL;
		char *port = NULL;
		char *rexxcmd = NULL;

		while (fgets(buf, 256, fi)) {
		    if (strnicmp(buf, cmdName, strlen(cmdName)) == 0)
			cmd = ScanReplace(buf + strlen(cmdName), fullPath, ErrFile, curDirPath);
		    else if (strnicmp(buf, portName, strlen(portName)) == 0)
			port = ScanReplace(buf + strlen(portName), fullPath, ErrFile, curDirPath);
		    else if (strnicmp(buf, rexxCmdName, strlen(rexxCmdName)) == 0)
			rexxcmd= ScanReplace(buf + strlen(rexxCmdName), fullPath, ErrFile, curDirPath);
		}
		fclose(fi);

		if (Verbose) {
		    printf("cmd=%s port=%s rexxcmd=%s file=%s\n",
			(cmd ? cmd : ""),
			(port ? port : ""),
			(rexxcmd ? rexxcmd : ""),
			fullPath
		    );
		}

		Forbid();
		if (port && rexxcmd) {
		    /*
		    *  Attempt to run the CLI command that starts up the
		    *  editor (or whatever)
		    */

		    {
			short retry = 0;

			while (FindPort(port) == NULL && retry < 5 && cmd) {
			    if (retry == 0)
				Execute(cmd, NULL, NULL);
			    Delay(50);
			    ++retry;
			}
		    }

		    /*
		    *  If port found place a REXX command directly to the port
		    */

		    if (FindPort(port)) {
			long ec;

			Permit();
			r = PlaceRexxCommandDirect(NULL,port,rexxcmd,NULL,&ec);
			Forbid();
			if (Verbose)
			    printf("R = %d %d\n", r, ec);
			ec = r;
		    }
		} else if (cmd) {
		    Execute(cmd, NULL, NULL);
		}
		Permit();
		if (cmd)
		    free(cmd);
		if (port)
		    free(port);
		if (rexxcmd)
		    free(rexxcmd);
	    } else {
		printf("can't open dcc:config/dcc.config\n");
	    }
	    free(fullPath);
	}
	free(buf);
    }
    return(r);
}

DoRexxCommand(msg, port, arg0, pres)
void *msg;
struct MsgPort *port;
char *arg0;
char **pres;
{
    return(20);
}

/*
 *  Handle %f/%e/%d/%n replacement in dcc.config
 */

char *
ScanReplace(buf, cFile, errFile, curDirPath)
char *buf;
char *cFile;
char *errFile;
char *curDirPath;
{
    char *s;
    char *rbuf;
    short i;
    short len;
    short dpindex;  /*	directory part index, char after directory  */
    short fpindex;  /*	file part index, char beginning file	    */

    /*
     *	Split cFile into directory part and file part
     */

    for (dpindex = strlen(cFile) - 1; dpindex >= 0; --dpindex) {
	if ((cFile[dpindex] == ':') || (cFile[dpindex] == '/'))
	    break;
    }
    ++dpindex;
    fpindex = dpindex;

    if (cFile[fpindex] == ':')
	++fpindex;

    /*
     *	Actual scan
     */

    while (*buf == ' ' || *buf == '\t')
	++buf;
    rbuf = NULL;
    for (i = 0; i < 2; ++i) {
	len = 0;

	for (s = buf; *s; ++s) {
	    if (*s == '%') {
		++s;
		switch(*s) {
		case 'f':       /*  %f - source file    */
		    if (rbuf)
			sprintf(rbuf + len, "%s", cFile);
		    len += strlen(cFile);
		    continue;
		case 'e':       /*  %e - error file     */
		    if (rbuf)
			sprintf(rbuf + len, "%s", errFile);
		    len += strlen(errFile);
		    continue;
		case 'd':
		    if (rbuf)
			sprintf(rbuf + len, "%.*s", dpindex, cFile);
		    len += dpindex;
		    continue;
		case 'n':
		    if (rbuf)
			sprintf(rbuf + len, "%s", cFile + fpindex);
		    len += strlen(cFile + fpindex);
		    continue;
		case 'c':
		    if (rbuf)
			sprintf(rbuf + len, "%s", curDirPath);
		    len += strlen(curDirPath);
		    continue;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		    {
			char *aptr = RexxReplace[*s - '0'];

			if (aptr == NULL)
			    aptr = "?";
			if (rbuf)
			    sprintf(rbuf + len, "%s", aptr);
			len += strlen(aptr);
		    }
		    continue;
		}
		--s;
	    }
	    if (*s == '\n')
		continue;
	    if (rbuf)
		rbuf[len] = *s;
	    ++len;
	}
	if (i == 0)
	    rbuf = malloc(len + 1);
    }
    if (rbuf)
	rbuf[len] = 0;
    else
	rbuf = strdup("");
    return(rbuf);
}

char *
FullPathOf(file)
char *file;
{
    BPTR lock;
    BPTR dlock;
    __aligned FIB fib;
    char *s1 = strdup(file);

    if (lock = Lock(file, SHARED_LOCK))
    {
	if (Examine(lock, &fib))
	{
	    short i = 0;
	    free(s1);
	    i = strlen(fib.fib_FileName);
	    s1 = malloc(i + 2);
	    strcpy(s1, fib.fib_FileName);
	    s1[i+1] = '\0';  /* Ensure we have have a null terminator when we */
	                     /* Insert the colon if this is the root only     */
	    while (dlock = ParentDir(lock))
	    {
		UnLock(lock);
		lock = dlock;
		if (Examine(lock, &fib))
		{
		    char *s2;
		    s2 = malloc(strlen(s1) + strlen(fib.fib_FileName) + 2);
		    sprintf(s2, "%s/%s", fib.fib_FileName, s1);
		    free(s1);
		    s1 = s2;
		    i = strlen(fib.fib_FileName);
		}
	    }
	    s1[i] = ':';
	}
	UnLock(lock);
    }
    return(s1);
}

#endif
#endif

/*
 *  Exit with a code, the -unixrc option causes unix style
 *  exit codes to be used (0 on no error or warning, 1 on error)
 */

void
uexit(int code)
{
    if (UnixRCOpt) {
	if (code <= 5)
	    code = 0;
	else
	    code = 1;
    }
    exit(code);
}

