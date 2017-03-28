// ucode.c file

char *cmd[]={"getpid", "ps", "chname", "fork", "switch", "wait", "exit", "kgetc", "kputc", "exec", 0};

#define LEN 64

/* strcmp function from: stack overflow http://stackoverflow.com/questions/22973670/creating-my-own-strcmp-function-in-c*/
int strcmp2(char *str1, char *str2)
{	
	while (*str1 && *str2 && *++str1 == *++str2);
	return *str1 - *str2; 
}

int show_menu()
{
	printf("*********************** Menu ***********************************\n");
	printf("* getpid ps  chname  fork  switch  wait  exit kgetc kputc exec *\n");
	printf("****************************************************************\n");
}

int find_cmd(char *name)
{
	int i = 0; 
	for (i = 0; cmd[i]; i++)
	{
		if (strcmp2(cmd[i], name) == 0) { return i; } 
	}
	return 0;
}

int getpid()
{
	return syscall(0,0,0);
}

int ps()
{
	return syscall(1, 0, 0);
}

int chname()
{
	char s[32];
	printf("input new name : ");
	gets(s);
	return syscall(2, s, 0);
}

int fork()
{   
	int child, pid;
	pid = getpid();
	printf("proc %d enter kernel to kfork a child\n", pid); 
	child = syscall(3, 0, 0);
	printf("proc %d kforked a child %d\n", pid, child);
}    

int exec()
{
	char s[64];
	printf("input filename : ");
	gets(s);
	return syscall(9, s, 0);
}

int kswitch()
{
	return syscall(4,0,0);
}

int wait()
{
	int child, exitValue;
	printf("proc %d enter Kernel to wait for a child to die\n", getpid());
	child = syscall(5, &exitValue, 0);
	printf("proc %d back from wait, dead child=%d", getpid(), child);
	if (child>=0)
	{
		printf("exitValue=%d", exitValue);
	}
	printf("\n"); 
} 

int geti()
{
// return an input integer
	int number = 0, i = 0, c = 1; 
	char buff[5];
	gets(buff);
	if (buff[0] == '-') { c = -1; } 
	for (i = 0; i < 5 && buff[i] ; i++)
	{
		number *= 10;
		number += (int)buff[i] - 48;
	}
	number *= c;
	return number;
}

int exit()
{
	int exitValue;
	printf("enter an exitValue: ");
	exitValue = geti();
	printf("exitvalue=%d\n", exitValue);
	printf("enter kernel to die with exitValue=%d\n", exitValue);
	_exit(exitValue);
}

int _exit(int exitValue)
{
	return syscall(6,exitValue,0);
}

int invalid(char *name)
{
	printf("Invalid command : %s\n", name);
}

int kgetc() 
{
	return syscall(7,0,0);
}

int kputc()
{
	printf("enter char: "); 
	return syscall(8, kgetc() ,0);
}






















