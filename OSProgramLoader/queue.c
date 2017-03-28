printList(char *s, PROC *queue)
{
        		
	printf("%s: ", s);
	while (queue) 
	{
		printf("[%d|%d]->", queue->pid, queue->priority);
		queue = queue->next; 
	}
	printf("\n");
}

printList2(char *s, PROC *queue)
{
        		
	printf("%s: ", s);
	while (queue) 
	{
		printf("[%d|event: %d]->", queue->pid, queue->event);
		queue = queue->next; 
	}
	printf("\n");
}

put_proc(PROC **list, PROC *p)  
{			
	(*p).status = FREE;
	if (!(*list)) { (*list) = p; return; }
	(*p).next = (*list);
	(*list) = p;
}

enqueue(PROC **queue, PROC *p)
{	
	PROC *ptr = *queue;
	(*p).next = 0;
	if (!(*queue)) 
	{ 
		*queue = p; 
		return; 
	}					
	if ((*queue)->priority < p->priority) 
	{
		(*p).next = (*queue);
		*queue = p;
		return;
	}
	while ((*ptr).next->priority >= (*p).priority && ((*ptr).next))
	{
		ptr = (*ptr).next;
	}
	(*p).next = (*ptr).next;
	(*ptr).next = p;
}
 
PROC *dequeue(PROC **queue)
{
        			//remove a PROC with the highest priority (the first one in queue)
        			//return its pointer;
	PROC *tmp = *queue; 
	if (*queue != 0 ) { *queue = (*queue)->next; } 
	tmp->next = 0;
	return tmp;
}
  
PROC *get_proc(PROC **list)     // e.g. get_proc(&freeList); 
{
				//get a FREE PROC from *list; 
				//return FREE PROC pointer; 
				//return 0 if no more FREE PROCs
	if ((*list)) { return dequeue(list); } 
	return 0;
}  











































































