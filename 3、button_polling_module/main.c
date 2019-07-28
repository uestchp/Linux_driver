#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#define file_name	"/dev/button"

int main(int argc, char **argv)
{
	int fd;
	char key_val[1];

	fd = open(file_name, O_RDWR);
	if(fd < 0) {
		printf("error: can`t open %s", file_name);
		return 0;
	}

	for(;;)	{
		read(fd, key_val, sizeof(key_val));
		if(!key_val[0]) {
			printf("button input: %d\r\n", key_val[0]);
		}
	}

	return 0;
}
