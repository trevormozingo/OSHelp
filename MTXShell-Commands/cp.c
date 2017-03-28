/* I got the MACROS from the linux manual:
http://linux.die.net/include/sys/stat.h & 
http://image.slidesharecdn.com/chapter3-110310143429-phpapp02/95/chapter-3-25-728.jpg?cb=1299767731 */

#define	__S_ISTYPE(mode, mask)	(((mode) &  0170000) == (mask))
#define	S_ISDIR(mode)	 __S_ISTYPE((mode), 0040000)
#define	S_ISREG(mode)	 __S_ISTYPE((mode), 0100000)

#include "ucode.c"


int cp(char *src, char *dest);
int f2f(char *src, char *dest);
int d2d(char *src, char *dest);
int d2d2(char *src, char *dest);

int transfer(int fd1, int fd2);

int main(int argc, char *argv[])
{

	if (!argv[1] || !argv[2]) { return; }

	cpy(argv[1], argv[2]);

}

int transfer(int fd1, int fd2)
{
	int n = 0;
	char buf[1024];
	while (n = read(fd1, buf, sizeof(buf))) 
	{ 
		write(fd2, buf, n);
	}
}

int cpy(char *src, char *dest)
{
	STAT path1, path2;
	
	//f -> f		copy file to file
	//f -> dir		cd into dir, create file
	//dir -> dir		for all files in dir
	//drr -> f		error

	stat(src, &path1);
	stat(dest, &path2);

	//Simple Copy
	if (S_ISREG(path1.st_mode)) { f2f(src, dest); }

	//directory to directory
	else if (S_ISDIR(path1.st_mode) && S_ISDIR(path2.st_mode)) { d2d(src, dest); }

	//directory to file (exit)
	else if (S_ISDIR(path1.st_mode) && S_ISREG(path2.st_mode)) { return; }

	//directory to non existing directory
	else if (S_ISDIR(path1.st_mode)) { d2d2(src, dest); }
}

int f2f(char *src, char *dest)
{
	int fd1 = 0, fd2 = 0;
	char file[128], newfile[128], *ptr;
	STAT path;

	stat(dest, &path);

	if (S_ISDIR(path.st_mode))
	{
		strcpy(file, src);

		ptr = strtok(file, " \n/");
		while (ptr)
		{
			bzero(newfile, sizeof(newfile));
			strcpy(newfile, ptr);
			ptr = strtok(0, " \n/");
		}
		bzero(file);
		strcpy(file, dest);
		strcat(file, "/");
		strcat(file, newfile);
		dest = file;
	}

	fd1 = open(src, O_RDONLY);
	fd2 = open(dest, O_WRONLY|O_CREAT);
	transfer(fd1, fd2);
	close (fd1);
	close(fd2);
}


int d2d(char *src, char *dest)
{
	char recursivePath[128], ccd[128];
	char buf[1024], *cp;
	char dirEntry[64];
	int fd = 0, i = 0, n = 0, first = 1;
	DIR *dp;
	STAT path, path2;
	
	fd = open(src, O_RDONLY);

	while(n = read(fd, buf, sizeof(buf)))
	{
		cp = buf;
		dp = (DIR *)buf;

		if (first == 1)
		{
			cp += dp->rec_len;
			dp = (DIR *)cp;
			cp += dp->rec_len;
			dp = (DIR *)cp;
			first = 0;
		}

		while (cp < buf + sizeof(buf))
		{	
			bzero(ccd, sizeof(ccd));
			bzero(recursivePath, sizeof(recursivePath));
			bzero(dirEntry, sizeof(dirEntry));


			strncpy(dirEntry, dp->name, dp->name_len);
			strcpy(recursivePath, src);
			strcat(recursivePath, "/");
			strcat(recursivePath, dirEntry);

			strcpy(ccd, dest);
			strcat(ccd, "/");
			strcat(ccd, dirEntry);

			stat(recursivePath, &path);

			if (S_ISDIR(path.st_mode))
			{ 
				stat(ccd, &path2);
				if(!S_ISDIR(path2.st_mode)) { mkdir(ccd); }
			}

			cpy(recursivePath, ccd);
		
			cp += dp->rec_len;
			dp = (DIR *)cp;
		}
		bzero(buf, sizeof(buf));
	}

	close(fd);
}

int d2d2(char *src, char *dest)
{
	mkdir(dest);
	d2d(src, dest);
}






































