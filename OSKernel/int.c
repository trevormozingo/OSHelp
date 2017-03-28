
/*************************************************************************
  usp  1   2   3   4   5   6   7   8   9  10   11   12    13  14  15  16
----------------------------------------------------------------------------
 |uds|ues|udi|usi|ubp|udx|ucx|ubx|uax|upc|ucs|uflag|retPC| a | b | c | d |
----------------------------------------------------------------------------
***************************************************************************/

/****************** syscall handler in C ***************************/

int kcinth()
{
	int a,b,c,d, r, i = 0; 
	
	a = get_word(running->uss, running->usp + 13*2); 
	b = get_word(running->uss, running->usp + 14*2);	
	c = get_word(running->uss, running->usp + 15*2);
	d = get_word(running->uss, running->usp + 16*2);

	switch(a)
	{
		case 0 : r = kgetpid();        break;
		case 1 : r = kps();            break;
		case 2 : r = kchname(b);       break;
		case 3 : r = fork();         break;
		case 4 : r = ktswitch();       break;
		case 5 : r = kkwait(b);        break;
		case 6 : r = kkexit(b);        break;
		case 7 : r = kkgetc(); 		break;
		case 8 : r = kkputc(b); 	break;
		case 9 : r = kexec(b); 		break;
		case 10: r = kpipe(b);		break;
		case 11: r = kpfd();		break;
		case 12: r = kclose(b); 	break;

		case 13: r = kread(b, c, d);	break;
		case 14: r = kwrite(b, c, d); 		break;

		case 99: kkexit(b);            break;
		default: printf("invalid syscall # : %d\n", a); 
	}

	//==> WRITE CODE to let r be the return value to Umode
	put_word(r, running->uss, running->usp + 16);
	return 1; 
}

//============= WRITE C CODE FOR syscall functions ======================

showPipe(PIPE *p)
{
	int i = 0;
	for (i = 0; i < 10; i++) { printf("|%c", p->buf[i]); } printf("\n");
	printf("head = %d, tail = %d data = %d room = %d\n", p->head, p->tail, p->data, p->room); 
	getc();
}

int kwrite(int fd, char *buf, int n)
{
	int r = 0, i = 0; 
	char *cp = buf;

	OFT *op; 
	PIPE *pipe_ptr;

	op = running->fd[fd];

	if (n <= 0) { return 0; } 

	for (i = 0; i < NFD; i++)
	{
		if (running->fd[i] && running->fd[i]->mode == WRITE_PIPE) 
		{
			op = running->fd[fd];
			pipe_ptr = running->fd[fd]->pipe_ptr;
			break;
		}
	}

	if (!pipe_ptr) { printf("no fd"); return 0; }

	while (n) 
	{
		if (!pipe_ptr->nreader) { kexit(-1); } 

		while (pipe_ptr->room)
		{
			pipe_ptr->buf[pipe_ptr->head++] = get_byte(running->uss, cp++);
			r++; 
			pipe_ptr->data++; 
			pipe_ptr->room--; 
			n--;
			showPipe(pipe_ptr);
			if (n == 0) { break; } 
		}
		kwakeup(&pipe_ptr->data);
		if (n == 0) { return r; } 
		ksleep(&pipe_ptr->room);
	}

}

int kread(int fd, char *buf, int n)
{
	int r = 0, i = 0; 
	char *cp = buf; 

	OFT *op; 
	PIPE *pipe_ptr;

	op = running->fd[fd];
	
	if (n <= 0) { return 0; } 

	for (i = 0; i < NFD; i++)
	{
		if (running->fd[i] && running->fd[i]->mode == READ_PIPE) 
		{
			op = running->fd[fd];
			pipe_ptr = running->fd[fd]->pipe_ptr;
			break;
		}
	}

	if (!pipe_ptr) { printf("no fd"); return 0; }

	printf(pipe_ptr->buf);

	while (n) 
	{
		while (pipe_ptr->data)
		{
			put_byte(pipe_ptr->buf[pipe_ptr->tail], running->uss, cp++);
			pipe_ptr->buf[pipe_ptr->tail++] = 0;
			n--;
			r++;
			pipe_ptr->data--;
			pipe_ptr->room++;
			showPipe(pipe_ptr);
			if (n == 0) { break; } 
		}
		if (r) 
		{
			kwakeup(&pipe_ptr->room);
			return r;
		}
		if (pipe_ptr->nwriter)
		{
			kwakeup(&pipe_ptr->room);
			ksleep(&pipe_ptr->data);
			continue;
	       	}
		return 0;
	}
}

int kclose(int fd)
{
	OFT *op; 
	PIPE *pipe_ptr;

	op = running->fd[fd];
	running->fd[fd] = 0;

	if (op->mode == READ_PIPE)
	{
		pipe_ptr = op->pipe_ptr;
		pipe_ptr->nreader--;
		if (--op->refCount == 0)
		{
			if (pipe_ptr->nwriter <= 0)
			{
				pipe_ptr->busy = 0;
				return;
			}
		}
		kwakeup(&pipe_ptr->room);
      		return;
	}
	else
	{
		pipe_ptr = op->pipe_ptr;
		pipe_ptr->nwriter--;
		if (--op->refCount == 0)
		{
			if (pipe_ptr->nreader <= 0)
			{
				pipe_ptr->busy = 0;
				return;
			}
		}
		kwakeup(&pipe_ptr->data);
      		return;
	}
}

int kpfd()
{
	int i = 0;
	printf("index mode refCount\n");
	for (i = 0; i < NFD; i++)
	{
		if (running->fd[i]) { printf("%d 	%d 	%d\n", i, running->fd[i]->mode, running->fd[i]->refCount); }
	}
}

int kpipe(int pd[2]) 
{ 
	int i = 0, k = 0;
	PIPE *pipe_ptr;
	OFT *readOFT, *writeOFT;
	
	for (i = 0; i < NPIPE; i++)
	{
		if (!pipe[i].busy) { pipe_ptr = &pipe[i]; break; }	
		if (i == (NPIPE - 1)) { printf("no pipes left!\n"); return -1; }
	}	

	for (i = 0; i < NOFT; i++)
	{
		if (oft[i].refCount < 1) { readOFT = &oft[i]; readOFT->refCount = 1; break; }
		if (i == (NOFT - 1)) { printf("no OFTs left!\n"); return -1; }
	}

	for (i = 0; i < NOFT; i++)
	{
		if (oft[i].refCount < 1) { writeOFT = &oft[i]; writeOFT->refCount = 1; break;}
		if (i == (NOFT - 1)) { printf("no OFTs left!\n"); readOFT->refCount = 0; return -1; }
	}

	// File descriptor
	for (i = 0; i < NFD; i++)
	{
		if (!running->fd[i])
		{
			running->fd[i] = readOFT;
			put_word(i, running->uss, &pd[0]);
			break;
		}
		if (i == (NFD - 1)) { printf("no fd left!\n"); readOFT->refCount = 0; writeOFT->refCount = 0; return -1; }
	} 

	for (k = 0; k < NFD; k++)
	{
		if (!running->fd[k])
		{
			running->fd[k] = writeOFT;
			put_word(k, running->uss, &pd[1]);
			break;
		}
		if (k == (NFD - 1)) { printf("no fd left!\n"); running->fd[i] = 0; readOFT->refCount = 0; writeOFT->refCount = 0; return -1; }
	} 
 
	readOFT->mode = READ_PIPE;
	writeOFT->mode = WRITE_PIPE;

	readOFT->pipe_ptr = pipe_ptr;
	writeOFT->pipe_ptr = pipe_ptr;

	pipe_ptr->head = 0;
	pipe_ptr->tail = 0;
	pipe_ptr->data = 0;
	pipe_ptr->room=PSIZE;
	pipe_ptr->nreader=1;
	pipe_ptr->nwriter=1;



	return 0;
} 





int kgetpid()
{
	//WRITE YOUR C code
	return running->pid;
}

int kps()
{
	//WRITE C code to print PROC information
	int i = 0;
	char *arr[6]; arr[0] = "FREE"; arr[1] = "READY"; arr[2] = "RUNNING"; arr[3] = "STOPPED"; arr[4] = "SLEEP"; arr[5] = "ZOMBIE";

	printf("name 		| pid 	|	ppid | status | priority\n");

	for (i = 0; i < NPROC; i++)
	{
		if (proc[i].status == FREE) { printf("%s		   %d \n", proc[i].name,  proc[i].pid); continue; } 
		printf("%s		   %d     	%d	%s	%d\n", proc[i].name,  proc[i].pid, proc[i].ppid, arr[proc[i].status], proc[i].priority); 
	}
	return 1; 
}

int kchname(char *name)
{
	//WRITE C CODE to change running's name string;
	int i = 0; char c;
	while (i < 32) {
		c = get_byte(running->uss, name+i); 
		running->name[i] = c;
		if (c == 0)
		{
			break;
		}
		i++;
	} 
}

int kkfork()
{
	//use you kfork() in kernel;
	//return child pid or -1 to Umode!!!
	int p = kfork("/bin/u1")->pid; 
	if (p != 0)
	{
		return p; 
	}
	return -1; 
}

/*
copyImage, kfork, and kexec
were all in Professor Wang's book. */
int copyImage(u16 pseg, u16 cseg, u16 size)
{  
	u16 i;
	for (i=0; i<size; i++)
	{
		//copy the image from the parent process into the child process
		//pseg = parent segment
		//cseg = child segment
		//each word is 2 bytes so you need to increment 2 bytes (one word)
		put_word(get_word(pseg, 2*i), cseg, 2*i);
	}
}

int fork()
{ 
	int pid; u16 segment;
	//fork does a regular kfork except it does not load
	//a u1 image. kfork(0) will make it so it does not load 
	//an image. Instead it will copy the image from the parent
	
	PROC *p = kfork(0);
	if (p==0) { return -1; }

	//get the segment of the child process
	segment = (p->pid+1)*0x1000;

	//now copy the image from the parent process into the child process
	copyImage(running->uss, segment, 32*1024);

	//now you have to set the childs stack
	p->uss = segment;
	p->usp = running->usp;

	//now set the ds, es, cs to segment
	put_word(segment, segment, p->usp);
	put_word(segment, segment, p->usp+2);
	
	// in normal fork -> both process continue from point fork() was called
	// so the program counter would not be set to 0
	put_word(0,       segment, p->usp+2*8); 
	put_word(segment, segment, p->usp+2*10);

	return p->pid;
}

/* 
exec replaces the current process
with a new execution image. So from umode it goes to kmode
replaces the image, then goes back to umode to begin running the
program */
kexec(char *y)
{
	int i, length = 0;
	char filename[64], *cp = filename;
	u16 segment = running->uss;

	while( (*cp++ = get_byte(running->uss, y++)) && length++ < 64 );
	
	//into the current running process -> load a new execution image
	load(filename, running->uss);

	//like kfork, you now want to set up the the umode stack
	for (i = 1; i < 13; i++)
	{
		put_word(0, running->uss, i*-2); 
	}

	//now set the ds, es, cs to segment
	put_word(0x0200, running->uss, -2);
	put_word(running->uss, running->uss, -4);
	put_word(running->uss, running->uss, -22);	
	put_word(running->uss, running->uss, -24);
	running->usp = -24; 

}

int ktswitch() { return tswitch();}

int kkwait(int *status)
{
	return kwait(&status);
}

int kkexit(int value)
{
	return kexit(value); 	
}

int kkgetc()
{
	return getc(); 
}

int kkputc(char b)
{
	return putc(b);
}













