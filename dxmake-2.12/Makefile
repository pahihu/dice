# Makefile.bsdport - BSD Ports Makefile
#
# (c)Copyright 1992-2000 Matthew Dillon, All Rights Reserved

PREFIX ?= /usr/local
PROG= dxmake
PROTOS= dmake-protos.h
SRCS= buffer.c cmdlist.c convert.c depend.c main.c parse.c run.c subs.c var.c lists.c cond.c
HFILES= defs.h dmake_str.h lists.h tokens.h dmake-protos.h
BINDIR= $(PREFIX)/bin/
MANDIR= $(PREFIX)/man/man
TARFILES= $(SRCS) Makefile Makefile.portable dxmake.1 $(HFILES)
CFLAGS= -Wall -Wstrict-prototypes
REV= 2.12
PORTTAR= /home/dillon/htdocs/FreeBSDPorts/$(PROG)-$(REV).tar.gz

all:	$(PROTOS)

$(PROTOS) : $(SRCS)
	(cd ${.CURDIR}; cat $(SRCS)) | egrep '^Prototype' > $(PROTOS)

tar:	clean
	rm -f dmake-protos.h
	make -f Makefile.portable dmake-protos.h
	rm -rf /tmp/$(PROG)-$(REV)
	mkdir /tmp/$(PROG)-$(REV)
	tar cf - $(TARFILES) | (cd /tmp/$(PROG)-$(REV); tar xvpf -)
	(cd /tmp; tar czf $(PORTTAR) $(PROG)-$(REV))
	chown dillon $(PORTTAR)
	md5 $(PORTTAR)

.include <bsd.prog.mk>

