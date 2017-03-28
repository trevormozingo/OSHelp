//TREVOR MOZINGO
//CPT_S 360
//LAB ASSIGNMENT 1

#include <stdio.h>
#include <stdlib.h>

//UTILITY FUNCTIONS AND VARIABLES FOR LAB ASSIGNMENT

#define BASE8 8
#define BASE10 10
#define BASE16 16

typedef unsigned int u32;
char *table = "0123456789ABCDEF";

int rpuInt(int x)
{
        char c;

        if (x)
        {
                c = table[x % BASE10];
                rpuInt(x / BASE10);
                putchar(c);
        }

}

int rpuOct(u32 x)
{
        char c;

        if (x)
        {
                c = table[x % BASE8];
                rpuOct(x / BASE8);
                putchar(c);
        }

}

int rpuHex(u32 x)
{
        char c;

        if (x)
        {
                c = table[x % BASE16];
                rpuHex(x / BASE16);
                putchar(c);
        }

}

int rpu(u32 x)
{
	char c;

	if (x)
	{
		c = table[x % BASE10];
     		rpu(x / BASE10);
     		putchar(c);
	}

}

int printu(u32 x)
{

	if (x==0)
	{
		putchar('0');
	}

	else
	{
     		rpu(x);
  		putchar(' ');
	}

}

//Use putchar() to write YOUR OWN prints(char *s) to print a string.

int prints(char *s)
{
	int i = 0;

	while (*s)
	{
		putchar(*s);
		s++;
		i++;
	}

	return i;
}

//WRITE YOUR OWN functions:

int printd(int x)
{

	if (x == 0)
	{
		putchar('0');
	}

	if(x < 0)
	{
		putchar('-');
		rpuInt(x*(-1));
	}

	else
	{
		rpuInt(x);
	}

}

int printo(u32 x)
{

        if (x == 0)
        {
                putchar('0');
        }


	rpuOct(x);
}

int printx(u32 x)
{
	putchar('0');
	putchar('x');

	if (x == 0)
	{
		putchar('0');
	}

	rpuHex(x);
}

//WRITE a myprintf(char *fmt, ...) for formatted printing

int myprintf(char *fmt, ...)
{
	char *cp = fmt;				//cp will point to the start of the string
	int *ip = (int *)&fmt + 1; 		//ip will point the the first parameter in the stack (consider)

	while(*cp)
	{

		if (*cp != '%') 
		{
			putchar(*cp);
		}

		else
		{

			switch(*(cp+1))
			{
				case 'c':
					putchar((char)*ip++);
					cp++;
					break;
				case 's':
					prints((char *)*ip++);
					cp++;
					break;
				case 'u':
					printu((u32)*ip++);
					cp++;
					break;
				case 'd':
					printd((int)*ip++);
					cp++;
					break;
				case 'o':
					printo((u32)*ip++);
					cp++;
					break;
				case 'x':
					printx((u32)*ip++);
					cp++;
					break;
				default:
					putchar(*cp);
					break;
			}

		}

		cp++;

	}

}


mymain(int argc, char *argv[ ], char *env[ ])
{
	int i;

	myprintf("in mymain(): argc=%d\n", argc);

	for (i=0; i < argc; i++)
	{
		myprintf("argv[%d] = %s\n", i, argv[i]);
  	}

// WRITE CODE TO PRINT THE env strings

	while(*env)
	{
		printf("%s\n", *env++);
	}


	myprintf("---------- testing YOUR myprintf() ---------\n");
	myprintf("this is a test\n");
	myprintf("testing a=%d b=%x c=%c s=%s\n", 123, 123, 'a', "testing");
	myprintf("string=%s, a=%d  b=%u  c=%o  d=%x\n",
           "testing string", -1024, 1024, 1024, 1024);
	myprintf("mymain() return to main() in assembly\n");

}






























