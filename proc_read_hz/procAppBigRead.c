/*  CDDapp.c */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#define CMD1 1
#define CMD2 2
#define MYNUM 0x88888888
#define MYSTR "Eureka!"

int main() {
	int fd, len, wlen;
	char str[128];
	int num, rnum;

	strcpy(str, MYSTR);

	// open 
	if((fd = open("/proc/hello", O_RDONLY)) == -1) {
		fprintf(stderr,"ERR:on open():%s\n",strerror(errno));
		exit(0);
	}

	// read 
	// if ((len = read(fd, str, sizeof(str))) == -1) {
	if ((len = read(fd, str, 1000000)) == -1) {
		fprintf(stderr,"ERR:on read():%s\n",strerror(errno));
		exit(1);
	}
	fprintf(stdout, "%s (len=%ld)\n", str, strlen(str));

	{
	int c;
	// c=getchar();
	}

	close(fd);

	return 0;
}
