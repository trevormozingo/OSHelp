#include "ucode.c"

int main(int argc, char *argv[])
{
	int fd = 0, n = 0, lr = 0, rr = 0, i = 0;

	char buf[1024], c;
	char port[64];

	STAT st_tty, st0, st1;

	gettty(port);
	stat(port, &st_tty);

	fstat(0, &st0); 
	fstat(1, &st1);
	
	//from KC wang's book 
	if (st_tty.st_dev != st0.st_dev || st_tty.st_ino != st0.st_ino) { lr = 1; }
	if (st_tty.st_dev != st1.st_dev || st_tty.st_ino != st1.st_ino) { rr = 1; }


	if (argv[1]) { fd = open(argv[1], O_RDONLY); }
	
	if (fd < 0) { return; }

/*
CASE FD == 0

	CASE: NO LEFT REDIRECTION

	cat             write char to fd2, on enter write to fd1 
	cat > f		write char to fd2, on enter write to fd1 
	cat | 		write char to fd2, on enter write to fd1 

	CASE: WITH LEFT REDIRECTION

		CASE: ONLY LEFT REDIRECTION

		| cat		write to fd1 & fd2 on newline
		cat < f1	write to fd1 & fd2 on newline


		CASE: BOTH RIGHT AND LEFT REDIRECTION

		| cat | 	write to fd1
CASE FD == N
	
	
*/

	//CASE FD == 0
	if (!fd)
	{
		while (n = read(fd, &c, 1))
		{
			if (c == '\r') { c = '\n'; }
			//if no left redirection
			if (!lr)
			{
				//(store data for later for fd1)
				buf[i++] = c;
				if (c == '\n') { write(2, "\r", 1); } 
				//write a char to fd2
				write(2, &c, 1);
				//on newline, write data to console
				if (c == '\n')
				{
					write(1, buf, i - 1);
					write(2, "\r", 1);
					write(1, "\n", 1);
					i = 0;
				}
			}
			//
			else 
			{
				if (c == '\n' && !rr) { write(2, "\r", 1); } 
				write(1, &c, 1);
			}
		}
	}
	else
	{
		while (n = read(fd, &c, 1))
		{
			if (c == '\n' && !rr) { write(2, "\r", 1); } 
			write(1, &c, 1);
		}
	}
	close(fd);
}






