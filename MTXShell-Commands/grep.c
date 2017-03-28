#include "ucode.c"

int main(int argc, char *argv[])
{
	int fd = 0, term = 0, n = 0, lr=0, rr = 0, i = 0;

	char buf[1024], c, input;
	char port[64];

	STAT st_tty, st0, st1;
	gettty(port);
	stat(port, &st_tty);

	fstat(0, &st0); 
	fstat(1, &st1);

	if (st_tty.st_dev != st0.st_dev || st_tty.st_ino != st0.st_ino) { lr = 1; }
	if (st_tty.st_dev != st1.st_dev || st_tty.st_ino != st1.st_ino) { rr = 1; }


	if (argv[2]) { fd = open(argv[2], O_RDONLY); }
	if (!lr && !fd) { close(fd); return; }

	while (n = read(fd, &c, 1))
	{
		buf[i++] = c;

		if (c == '\n' && !rr) 
		{ 
			if (strstr(buf, argv[1]))
			{
				write(1, buf, i - 1);
				write(2, "\r", 1);
				write(1, "\n", 1);
			}
			bzero(buf, sizeof(buf));
			i = 0;
		}
		else if (c == '\n')
		{
			if (strstr(buf, argv[1])) 
			{ 
				write(1, buf, i); 
			}
			bzero(buf, sizeof(buf));
			i = 0;
		}
	}
	close(fd);
}








