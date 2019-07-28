#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>

#define file_name	"/dev/button"

int fd;

void signal_function(int signum)
{
	unsigned char key_val;
	read(fd, &key_val, 1);
	printf("key value: 0x%x\n",key_val);
}

int main(int argc, char **argv)
{
	int flags;

	signal(SIGIO, signal_function);	

	fd = open(file_name, O_RDWR);
	if(fd < 0) {
		printf("error: can`t open %s", file_name);
		return 0;
	}

	fcntl(fd, F_SETOWN, getpid());
	flags = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, flags | FASYNC);
	
	for(;;)	{
		sleep(1000);
	}

	return 0;
}
