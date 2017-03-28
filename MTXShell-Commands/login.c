#include "ucode.c"

char *tty, buf[1024], buf2[1024], *ln, *params[64], uname[64], pass[64], temp[64], *executable, *home;
int in, out, err, pw, uid, gid;

int chkUser();
int flush();

int main(int argc, char *argv[])
{
	int i = 0; 

	close(0); 
	close(1); 

	in = open(argv[1], O_RDONLY);
	out = open(argv[1], O_WRONLY);
	err = open(argv[1], O_WRONLY);

	settty(argv[1]);
	pw = open("/etc/passwd", O_RDONLY);

	while (1)
	{	
		lseek(pw, 0, 0);
		flush();

		printf("login: ");  gets(uname); 
		printf("\npassword: "); gets(pass);
		printf("\n");

		read(pw, buf, sizeof(buf)); 
		ln = strtok(buf, "\r\n");

		while(ln)
		{
			params[i++] = ln;
			ln = strtok(0, "\r\n");
		}

		if (chkUser())
		{
			chuid(uid, gid);
			chdir(home);
			close(pw);

			
			strcpy(buf2, executable);
			strcat(buf2, " ");
			strcat(buf2, home);

			exec(buf2);
			
		}
		printf("login failed...\n");
		i = 0;
	}
}

int chkUser()
{
	int i = 0; 
	
	while (params[i])
	{
		if (strcmp(strtok(params[i], ":"), uname) == 0 && strcmp(strtok(NULL, ":"), pass) == 0)
		{
			gid = atoi(strtok(NULL, ":"));
			uid = atoi(strtok(NULL, ":"));

			strtok(NULL, ":");

			home = strtok(NULL, ":");
			executable = strtok(NULL, ":");

			return 1;
		}
		i++;
	}
}

int flush()
{
	bzero(buf, sizeof(buf));
	bzero(params, sizeof(params)/4);
	bzero(uname, sizeof(uname));
	bzero(pass, sizeof(pass));
	bzero(temp, sizeof(temp));
}









