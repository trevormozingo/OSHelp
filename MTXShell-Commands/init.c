#include "ucode.c"

int pid, child, status, ser0, ser1;
int in, out, s0in, s0out, s1in, s1out;

main(int argc, char *argv[])
{
	in  = open("/dev/tty0", O_RDONLY);
	out = open("/dev/tty0", O_WRONLY);

	child = fork();

	if (child) 
	{  	
		ser0 = fork(); 
		if (ser0)
		{
			ser1 = fork();
			if (ser1)
			{
				parent();
			}
			else { exec("login /dev/ttyS1"); return; }
		}
		else { exec("login /dev/ttyS0"); return; }  
	}
	else { exec("login /dev/tty0");  return;  }
}

int parent()
{
	int pid, status;
	while (1)
	{
		pid = wait(&status);
		if (pid == child)
		{
			child = fork(); 
			if (child) { continue; } 
			else { exec("login /dev/tty0");  return; } 
		}
		else if (pid == ser0)
		{
			ser0 = fork(); 
			if (ser0) { continue; } 
			else { exec("login /dev/ttyS0");  return; } 
		}
		else if (pid == ser1)
		{
			ser1 = fork(); 
			if (ser1) { continue; } 
			else { exec("login /dev/ttyS1");  return; } 
		}
	}
}






















