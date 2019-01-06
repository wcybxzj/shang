WARN=-Wall -W

FINAL_CFLAGS= $(STD) $(WARN) -g -g3 -gdwarf-2 -lm -lpthread

PROGS= 2_1.waitpid 2_2.man_waitpid 5.few 6.sleep 7.mysh_strsep 8.mysh_strtok 9.mysh_return_static \
	10.mysh_return_malloc_pointer 11.mysu 12.geteuid 13.mycat 14.passwd 19.times \
	21.umask 22.main_return 23.fork_var_address 24.exit_status \
	primer0 primer1 primer2 primerN

all:$(PROGS)

2_1.waitpid:2_1.waitpid.o
	$(CC) $(FINAL_CFLAGS) -o 2_1.waitpid 2_1.waitpid.o

2_2.man_waitpid:2_2.man_waitpid.o
	$(CC) $(FINAL_CFLAGS) -o 2_2.man_waitpid 2_2.man_waitpid.o

clean:
	rm -f $(PROGS) *.o a.out






