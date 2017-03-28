#include "type.h"

PROC proc[NPROC], *running, *freeList, *readyQueue, *sleepList;
int procSize = sizeof(PROC);
int nproc = 0;

char *pname[]={"Sun", "Mercury", "Venus", "Earth",  "Mars", "Jupiter", 
"Saturn", "Uranus", "Neptune" };

int body()
{
	char c;
	printf("proc %d starts from body()\n", running->pid);
	while(1) 
	{
		printf("\n------------------------------------------------------------------------------\n");
		printf("[%d]", nproc);
		printList("freelist =>", freeList);		
		printList("readyQueue =>", readyQueue);
		printList("sleepList =>", sleepList);

		printf("running => [%d|%d]", running->pid, running->priority);
		printf("\n\n");
		printf("proc %d running: parent=%d\n",running->pid,running->ppid);
		printf("enter a char [s|f|q|z|a|w|u]  : ");
		c = getc(); 
		printf("\n");
		switch(c)
		{
			case 's' : do_tswitch(); break;
			case 'f' : do_kfork(); break;
			case 'q' : do_exit(); break;
			case 'z' : do_sleep(); break;
			case 'a' : do_wakeup(); break;
			case 'w' : do_wait(); break;
			case 'u' : goUmode(); break;
			default: printf("invalid command\n"); break;
		}
	}
}

#include "queue.c"
#include "wait.c"             // YOUR wait.c   file
#include "kernel.c"           // YOUR kernel.c file
#include "int.c"              // YOUR int.c    file
int color;

int init()
{
	PROC *p; int i;
	color = 0x0C;
	printf("init ....");

	for (i=0; i<NPROC; i++)
	{
		p = &proc[i];
		p->pid = i;
		p->status = FREE;
		p->priority = 0;  
		strcpy(proc[i].name, pname[i]);
		p->next = &proc[i+1];
	}

	freeList = &proc[0];      
	proc[NPROC-1].next = 0;
	readyQueue = sleepList = 0;
	p = get_proc(&freeList);
	p->status = RUNNING;
	p->ppid   = 0;
	p->parent = p;
	running = p;
	nproc = 1;
	printf("done\n");
} 

int scheduler()
{
	if (running->status == RUNNING)
	{	
		running->status = READY;
		enqueue(&readyQueue, running);
	}
	running = dequeue(&readyQueue);
	running->status = RUNNING;
	color = running->pid + 0x0A;
}

int int80h();

int set_vector(u16 segment, u16 handler)
{
	put_word(handler, 0, 80<<2);
	put_word(0x1000,  0,(80<<2) + 2);
}

main()
{
	printf("MTX starts in main()\n");
	init();      			
	set_vector(80, int80h);
	kfork("/bin/u1");    		
	while(1)
	{
		printf("P0 running!\n");
		while(!readyQueue);
		printf("P0 switch process\n");
		tswitch();         	
	}
}

int do_kfork( )
{
	PROC *p = kfork("/bin/u1");
	if (p == 0) 
	{ 
		printf("kfork failed\n"); return -1; 
	}
	printf("PROC %d kfork a child %d\n", running->pid, p->pid);
	return p->pid;
}

int do_tswitch(){ tswitch(); }

do_exit() 
{ 
	int c;
	printf("exit event: ");
	c = getInt();
	kexit(c);
}

do_sleep() 
{ 
	int c;
	printf("sleep event: ");
	c = getInt();
	ksleep(c);
}

do_wakeup() 
{
 	int c;
	printf("wake event: ");
	c = getInt();	
	kwakeup(c);
}	

do_wait() 
{ 
	int pid, status;
	pid = kwait(&status);	/* returns the pid of the ZOMBIE proc & status (exit code of proc)	*/
	printf("pid: %d ==> status: %d", pid, status);
}













