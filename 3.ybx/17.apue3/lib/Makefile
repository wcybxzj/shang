#
# Makefile for misc library.
#
ROOT=..
PLATFORM=$(shell $(ROOT)/systype.sh)
include $(ROOT)/Make.defines.$(PLATFORM)

LIBMISC=libapue.a

OBJS = cliconn.o  servlisten.o servaccept.o daemonize.o errorlog.o \
	writen.o senderr.o sendfd.o bufargs.o recvfd.o spipe.o error.o writen.o

all:	$(LIBMISC)

$(LIBMISC):	$(OBJS)
	$(AR) rv $(LIBMISC) $?
	$(RANLIB) $(LIBMISC)


clean:
	rm -f *.o a.out core temp.* $(LIBMISC)

