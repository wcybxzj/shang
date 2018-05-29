/*
从Linux 2.6.27版本开始，新增了不少系统调用，其中包括eventfd，
它的主要是用于进程或者线程间的通信(如通知/等待机制的实现)。
#include <sys/eventfd.h>
int eventfd(unsigned int initval, intflags);

参数解释：
如果是2.6.26或之前版本的内核，flags 必须设置为0。

Flags支持一下标志位：

EFD_NONBLOCK
类似于使用O_NONBLOCK标志设置文件描述符。

EFD_CLOEXEC
类似open以O_CLOEXEC标志打开,O_CLOEXEC 应该表示执行exec()时，
之前通过open()打开的文件描述符会自动关闭测试时，在open()之后，
调用一下exec()，在新的进程中检测描述符是否已经关闭

Initval:
初始化计数器值，该值（暂时取名为A）保存在内核。
函数返回一个文件描述符，与打开的其他文件一样，可以进行读写操作。

Read:
如果计数器A的值不为0时，读取成功，获得到该值。
如果A的值为0，非阻塞模式时，会直接返回失败，并把error置为EINVAL
如果为阻塞模式，一直会阻塞到A为非0为止。

Write:
将缓冲区写入的8字节整形值加到内核计数器上，即会增加8字节的整数在计数器A上，
如果A的值达到0xfffffffffffffffe时，就会阻塞（在阻塞模式下），直到A的值被read。
阻塞和非阻塞情况同上面read一样。
*/

#include <sys/eventfd.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>             /* Definition of uint64_t */

#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)

/*
$ ./a.out 1 2 4 7 14
Child writing 1 to efd
Child writing 2 to efd
Child writing 4 to efd
Child writing 7 to efd
Child writing 14 to efd
Child completed write loop
Parent about to read
Parent read 28 (0x1c) from efd
*/
int
main(int argc, char *argv[])
{
	int efd, j;
	uint64_t u;
	ssize_t s;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <num>...\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	efd = eventfd(0, 0);
	if (efd == -1)
		handle_error("eventfd");

	switch (fork()) {
		case 0:
			for (j = 1; j < argc; j++) {
				printf("Child writing %s to efd\n", argv[j]);
				u = strtoull(argv[j], NULL, 0);
				/* strtoull() allows various bases */
				s = write(efd, &u, sizeof(uint64_t));
				if (s != sizeof(uint64_t))
					handle_error("write");
			}
			printf("Child completed write loop\n");

			exit(EXIT_SUCCESS);

		default:
			sleep(2);

			printf("Parent about to read\n");
			s = read(efd, &u, sizeof(uint64_t));
			if (s != sizeof(uint64_t))
				handle_error("read");
			printf("Parent read %llu (0x%llx) from efd\n",
					(unsigned long long) u, (unsigned long long) u);
			exit(EXIT_SUCCESS);

		case -1:
			handle_error("fork");
	}
}

