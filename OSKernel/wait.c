

int ksleep(int event)
{
	running->event = event;  	//record the event that triggered sleep 	
	running->status = SLEEP; 	//change status to sleep 		
	enqueue(&sleepList, running);	//store the proc into the sleep list
	tswitch();			/* call tswitch() --> because the proc is sleeping,
					   it will not go to the readyQueue -> it will just stay in the proc array [ ] */
					//After ksleep() is called -> current proc goest to sleep and the first proc
					//in the readyQueue becomes running -> 
}
/*
e.g.
 
 ready -> p0
 sleep -> NULL;
 running -> p1

 call sleep()
 
 ready -> NULL;
 sleep -> p1
 running -> p0
*/

int kwakeup(int event)
{
	int i; PROC *p;
	for (i=1; i<NPROC; i++)
	{
		p = &proc[i];
		if (p->status == SLEEP && p->event == event)	//for all sleeping processes with matching event #
		{
			p->event = 0;      			
			p->status = READY; 			//wake up the process and store it in the ready queue
			enqueue(&readyQueue, p);
		} 
	}
	sleepList = 0;
}
/*
 kwakeup looks through proc[ ] for sleeping processes. For all 
sleeping processes, -> if the wakeup event # == the sleep event #
kwakeup will wakeup those processes, and enter them into the ready queue
*/

int kexit(int exitValue)
{
	PROC *p; int i, wakeupP1 = 0; 
	if (running->pid==1 && nproc>2)	/* if p1 trys to exit, but other procs exist -> throw error
					   p1 is needed to collect the ZOMBIE procs, so it cannot die
					   when there are other active procs				 	*/
	{ 
		printf("other procs still exist, P1 can't die yet\n");
		return -1;
	}
	for (i = 1; i < NPROC; i++)					/* when p# exits -> pass all of the children p1 */
	{
		p = &proc[i];
		if (p->status != FREE && p->ppid == running->pid) 	/* dispose of children by passing them to p1 	*/
		{
			p->ppid = 1;
			p->parent = &proc[1];
			wakeupP1++;
		}
	}
	running->exitCode = exitValue;	/* turn the current process into a zombie. 				*/	
	running->status = ZOMBIE;
	kwakeup(running->parent); 	/* if needed -> wake up the parent process				*/

	if (wakeupP1)
	{
		kwakeup(&proc[1]);
	}
	tswitch();               
}

/* 
kwait:
the current running -> p# will wait for all ZOMBIE children (or children processes to finish) 

if p# has children, but the children are not ZOMBIES, then p# will be put to sleep until it is 
woken up from the ZOMBIE child. 

if p# has ZOMBIE children, it will return the childs exit code and pid. It will then return that
processes to freelist

*/
int kwait(int *status)  
{
	PROC *p; int i, hasChild = 0;
	while(1)
	{                             
		for (i=1; i<NPROC; i++)
		{             
			p = &proc[i];
			if (p->status != FREE && p->ppid == running->pid)
			{ 
				hasChild = 1;                
				if (p->status == ZOMBIE)
				{    
					*status = p->exitCode;    
					p->status = FREE;         
					put_proc(&freeList, p);  
					nproc--;                 
					return(p->pid);          
				}
			}
		}
		if (!hasChild) { return -1; }      
		ksleep(running);
	}
}









































































































































