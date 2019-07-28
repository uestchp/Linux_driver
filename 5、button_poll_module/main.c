#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <poll.h>

#define file_name	"/dev/button"

int main(int argc, char **argv)
{
	int fd;
	char key_val;
	int ret;
	struct pollfd fds[1];

	fd = open(file_name, O_RDWR);
	if(fd < 0) {
		printf("error: can`t open %s", file_name);
		return 0;
	}
	fds[0].fd = fd;
	fds[0].events = POLLIN;

	for(;;)	{
		ret = poll(fds, 1, 1000);
		if(ret == 0) {
			printf("time out");
		}
		else {
			read(fd, &key_val, 1);
			printf("button input: %d\r\n", key_val);
		}
	}

	return 0;
}
