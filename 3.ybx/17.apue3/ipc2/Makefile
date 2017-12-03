ROOT=..
PLATFORM=$(shell $(ROOT)/systype.sh)
include $(ROOT)/Make.defines.$(PLATFORM)

ifeq "$(PLATFORM)" "linux"
	EXTRALIBS=-pthread
endif

PROGS=bindunix pollmsg sendmsg stat

all:$(PROGS)
	for i in open.fe opend.fe; do \
		(cd $$i && $(MAKE)) ||exit 1; \
	done

clean:
	rm -rf $(PROGS) $(TEMPFILES) *.o
	for i in open.fe opend.fe; do \
		(cd $$i && $(MAKE) clean) ||exit 1;\
	done

include $(ROOT)/Make.libapue.inc
