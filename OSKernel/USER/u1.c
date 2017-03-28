#include "ucode.c"
int color;
main()
{ 
	char name[64]; int pid, cmd;
	int pd[2];
	long i = 0, j = 0;

	while(1)
	{
		pid = getpid();
		color = 0x0C;
		printf("----------------------------------------------\n");
		printf("I am proc %d in U mode: running segment=%x\n", getpid(), getcs());

		show_menu();
		printf("Command ? ");
		gets(name); 
		if (name[0]==0) 
		{
			continue;
		}
		cmd = find_cmd(name);
		switch(cmd)
		{
			case 0 : printf("pid: %d\n", getpid());   break;
			case 1 : ps();       break;
			case 2 : chname();   break;
			case 3 : fork();    break;
			case 4 : kswitch();  break;
			case 5 : wait();     break;
			case 6 : exit();     break;
			case 7 : kgetc();  	break;
			case 8 : kputc(); 	break; 
			case 9 : exec(); 	break; 
			case 10 :pipe(pd); 	break; 
			case 11 :pfd(); 	break; 
			case 12 :close();	break;

			//These will be changed later
			case 13 :read();	break;
			case 14 :write();	break;

			default: invalid(name); break;
		}
	}
}   










