#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#define file_name	"/dev/led"

int main(int argc, char **argv)
{
	int fd;
	char val;

	fd = open(file_name, O_RDWR);
	if(fd < 0){
		printf("error: can`t open %s", file_name);
		return 0;
	}

	if (!strcmp("on", argv[1])) {
		val = 0;
		write(fd, &val, 1);
	} else if (!strcmp("off", argv[1])) {
		val = 1;
		write(fd, &val, 1);
	} else {
		printf("error: parm error");
	}
	return 0;
}
