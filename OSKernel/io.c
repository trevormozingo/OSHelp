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
                putc(c);
        }

}

int rpuOct(u32 x)
{
        char c;

        if (x)
        {
                c = table[x % BASE8];
                rpuOct(x / BASE8);
                putc(c);
        }

}

int rpuHex(u32 x)
{
        char c;

        if (x)
        {
                c = table[x % BASE16];
                rpuHex(x / BASE16);
                putc(c);
        }

}

int rpu(u32 x)
{
	char c;

	if (x)
	{
		c = table[x % BASE10];
     		rpu(x / BASE10);
     		putc(c);
	}

}

int printu(u32 x)
{

	if (x==0)
	{
		putc('0');
	}

	else
	{
     		rpu(x);
  		putc(' ');
	}

}

//Use putc() to write YOUR OWN prints(char *s) to print a string.

int prints(char *s)
{
	int i = 0;

	while (*s)
	{
		putc(*s);
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
		putc('0');
	}

	if(x < 0)
	{
		putc('-');
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
                putc('0');
        }


	rpuOct(x);
}

int printx(u32 x)
{
	putc('0');
	putc('x');

	if (x == 0)
	{
		putc('0');
	}

	rpuHex(x);
}

//WRITE a myprintf(char *fmt, ...) for formatted printing

int printf(char *fmt, ...)
{
	char *cp = fmt;				//cp will point to the start of the string
	int *ip = (int *)&fmt + 1; 		//ip will point the the first parameter in the stack (consider)

	while(*cp)
	{

		if (*cp != '%') 
		{
			putc(*cp);
			if (*cp == '\n')
			{
				/* http://stackoverflow.com/questions/11320135/assembly-newlines */	
    				putc(0x0D);
			}
		}

		else
		{

			switch(*(cp+1))
			{
				case 'c':
					putc((char)*ip++);
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
					putc(*cp);
					break;
			}

		}

		cp++;

	}
	
}

void gets(char s[])
{
    char *s2 = s;
    while (*(s2-1) != '\r')
    {
        *s2++ = getc();
	putc(*(s2 - 1));
    }
    *(s2-1) = 0;
}


int getInt()
{
	int number = 0, i = 0;
	char buff[5];
	gets(buff);
	for (i = 0; i < 5 && buff[i] ; i++)
	{
		number *= 10;
		number += (int)buff[i] - 48;
	}
	return number;
}

















