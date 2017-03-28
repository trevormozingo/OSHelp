#include "loader.c"

PROC *kfork(char *filename)
{
	int i;
	PROC *p = get_proc(&freeList);

	if (!p) 
	{
		printf("no more PROC, kfork() failed\n");
		return 0;
	}

	p->status = READY;
	p->priority = 1; 		
	p->ppid = running->pid; 	
	p->parent = running;				
	for (i=1; i<10; i++) 		
	{
		p->kstack[SSIZE-i]= 0 ;
	}
	p->kstack[SSIZE-1] = (int)goUmode;
	p->ksp = &p->kstack[SSIZE-9]; 
	enqueue(&readyQueue, p); 

	p->uss = (p->pid + 1) * 0x1000;	

	if (filename) { load(filename, p->uss); }

	for (i = 1; i < 13; i++)
	{
		put_word(0, p->uss,  i*-2); 
	}

	put_word(0x0200, p->uss, -2);
	put_word(p->uss, p->uss, -4);
	put_word(p->uss, p->uss, -22);	
	put_word(p->uss, p->uss, -24);
	p->usp = -24; 


	for (i = 0; i < NFD; i++)
	{
		if (running->fd[i])
		{
			p->fd[i] = running->fd[i];
			p->fd[i]->refCount++;
			if (p->fd[i]->mode == READ_PIPE)
			{
				p->fd[i]->pipe_ptr->nreader++;
			}
			if (p->fd[i]->mode == WRITE_PIPE)
			{
				p->fd[i]->pipe_ptr->nwriter++;
			}
		}
	}

	nproc++;
	return p;		 	
}













































