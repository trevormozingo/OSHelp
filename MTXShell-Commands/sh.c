#include "ucode.c"

char home[64];

int builtIn(char *command);
int evaluateLine(char *command);
int scan(char *command, char **head, char **tail);
int tokenize(char *command, char *params[]);
int execute(char *command);

int main(int argc, char *argv[])
{
	char buf[64];
	int pid, status;

	strcpy(home, argv[1]);

	while (1)
	{
		bzero(buf, sizeof(buf));

		printf("$ "); gets(buf);
		
		if (builtIn(buf)) { continue; }

		pid = fork(); 

		if (pid) { pid = wait(&status); }
		else { evaluateLine(buf, 0); return; }
		//else { evaluateLine(buf); return; }
	}
}

int builtIn(char *command)
{
	char buf[64], *params[64];
	
	if (strstr(command, "cd") || strstr(command, "logout") || strstr(command, "su"))
	{
		strcpy(buf, command);
		tokenize(buf, params);

		if (!strcmp(params[0], "cd"))
		{
			if (params[1]) { chdir(params[1]); }
			else {  chdir(home); }
		}
		else if (!strcmp(params[0], "logout") || !strcmp(params[0], "exit"))
		{
			exit(0);
		}


		return 1;
	}
	return 0;
}

int tokenize(char *command, char *params[])
{
	int i = 0;
	char *ptr;
	ptr = strtok(command, " \n");

	while (ptr)
	{
		params[i++] = ptr;
		ptr = strtok(0, " \n");
	}
}

// do_command
int execute(char *command)
{
	int i = 0;

	char buf[64], cmdCpy[64], *params[64], *ptr;

	strcpy(buf, command);
	strcpy(cmdCpy, command);
	tokenize(buf, params);

	for (i = 0; params[i]; i++)
	{
		if (!strcmp(params[i], "<") || !strcmp(params[i], ">") || !strcmp(params[i], ">>"))
		{
			if (!strcmp(params[i], "<")) { close(0); open(params[i + 1], O_RDONLY); }
			
			if (!strcmp(params[i], ">")) { close(1); open(params[i + 1], O_WRONLY|O_CREAT); }
			else if (!strcmp(params[i], ">>")) { close(1); open(params[i + 1], O_RDWR|O_APPEND|O_CREAT); }
		}
	}
	
	if ((ptr = strstr(cmdCpy, "<")) != 0)
	{
		while (ptr < cmdCpy + sizeof(cmdCpy)) { *ptr++ = 0; } 
	}
	else if ((ptr = strstr(cmdCpy, ">")) != 0)
	{
		while (ptr < cmdCpy + sizeof(cmdCpy)) { *ptr++ = 0; } 
	}

	exec(cmdCpy);
}

// scan
int evaluateLine(char *command, int *pd)
{
	int pid, status, lpd[2], tailExists = 0;
	char *head, *tail;

	if (command[0] == 0) { return; }

	if (pd) { close(pd[0]); dup2(pd[1], 1); close(pd[1]); }

	tailExists = scan(command, &head, &tail);

	if (!tailExists) {  execute(head); }
	else 
	{ 
		pipe(lpd);
		pid = fork();

		if (pid) 
		{ 
			close(lpd[1]); dup2(lpd[0],0); close(lpd[0]);
			execute(tail); 
		}
		else{ evaluateLine(head, lpd); }
	}
}


/*
int evaluateLine(char *command)
{
	int pid, status, pd[2], tailExists = 0;
	char *head, *tail;

	if (command[0] == 0) { return; }

	tailExists = scan(command, &head, &tail);

	if (!tailExists) {  execute(head); }
	else 
	{ 
		pipe(pd);
		pid = fork();

		if (pid) 
		{ 
			close(pd[1]);	/* close the write end of the pipe	* /
			close(0);	/* close close stdin			* /
			dup(pd[0]);	/* replace stdin with the pipe read end * /

			//pid = wait(&status); 
			execute(tail); 
		}
		else 
		{ 
			close(pd[0]);	/* close the read end of the pipe 	* /
			close(1);	/* close stdout				* /
			dup(pd[1]);	/* replace the write end with stdout	* /
			
			evaluateLine(head); 
		}
	}
}
*/


int scan(char *command, char **head, char **tail)
{
	char *last, *curr;

	curr = command;
	last = 0;
	*head = command;

	while (*curr) 
	{ 
		if (*curr == '|') { last = curr; }
		curr++;
	}
	
	if (last) { *last = 0; *tail = ++last; }
	else { *tail = 0; }
	return *tail;
}




























