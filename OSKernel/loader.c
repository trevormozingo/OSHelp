#include "ext2.h"

char buf1[1024], buf2[1024];
int iblk;

DIR *dp;
INODE *ip; 
GD *gp; 

typedef struct header
{
	u32 ID_space;         // 0x04100301:combined I&D or 0x04200301:separate I&D
	u32 magic_number;     // 0x00000020
	u32 tsize;            // code section size in bytes
	u32 dsize;            // initialized data section size in bytes
	u32 bsize;            // bss section size in bytes
	u32 zero;             // 0
	u32 total_size;       // total memory size, including heap
	u32 symbolTable_size; // only if symbol table is present
} HD;

int get_block(u16 blk, char *buf)
{ 
	      	//you have 18 tracks
	 	//
	      	//CYL          HEAD              SECTOR
	diskr( blk/18, ((2*blk)%36)/18, (((2*blk)%36)%18), buf);
}

GD *getGD()
{
	get_block(2, buf1); 
	return(GD *)buf1; 
}

INODE *getRT()
{
 	get_block(((2 - 1) / 8) + iblk, buf1);
	return (INODE *)buf1 + (2 - 1) % 8;
}

INODE *getIN(int ino)
{
	get_block(((ino - 1) / 8) + iblk, buf1);
	return(INODE *)buf1 + (ino - 1) % 8;
}

/*
See "MY" Cpts 360 project for these functions. I added some minor modifications.
https://github.com/trevormozingo/Other/blob/master/Research/UNIX%20VFS/UNIX%20VFS%20Release%201.3/source.c
*/
int search(INODE *ip, char *filename)
{ 
	int i = 0;
	char *cp, c; 
	
	//only check the direct blocks for directories
	//(directories are pretty small)
	for (i = 0; i < 12; i++)
	{	
		//if there is a data block
		if (ip->i_block[i])  
		{
			//load the data block into the buffer
			get_block((u16)ip->i_block[i], buf2); 

			//set up a dir pointer to the data block 
			dp = (DIR *)buf2;	
			cp = buf2;

			//now check the data block to see if the directory exists
			while (cp < buf2 + 1024)		
			{	
				//if the name length % 4 == 0 -> an odd character is appended? 
				//temporarily make it null
				c = dp->name[dp->name_len];
				dp->name[dp->name_len] = 0;

				//check to see if the directory was found
				if (!strncmp(filename, dp->name, dp->name_len))
				{
			      		return dp->inode;
				}

				//put the character back 
				dp->name[dp->name_len] = c;
	
			   	cp += dp->rec_len;         
			   	dp = (DIR *)cp;
			}
		}
	}
	return 0;
}

/*
See my Cpts 360 project for these functions. I added some minor modifications.
https://github.com/trevormozingo/Other/blob/master/Research/UNIX%20VFS/UNIX%20VFS%20Release%201.3/source.c
*/
int getino(char *filename)
{
	int ino = 2;
	char *token;
	
	//get the group descriptor
	//this is needed to find the 
	//inode begin block 
	gp = getGD();				/* get the group descriptor using the global buffers	*/
	iblk = gp->bg_inode_table;		/* get the inode begin block 				*/
	ip = getRT();				/* get the root inode					*/
	
	token = strtok(filename, "/"); 				
	while (token  != 0)
	{
		//now search the current directory for the next directory
		ino = search(ip, token); 			
		if (!ino) { break; }	

		//move to the next directory			
		ip = getIN(ino);
		//get next directory to search
		token = strtok(0, "/");
	}
	//return the inode number of that directory
	return ino;
}

load(char *filename, u16 segment)
{	
	HD *hp; 
	u32 *up;

	int ino;
	int i;						
	long tsize, dsize, bsize;

	ino = getino(filename); 	
	ip = getIN(ino);

	if ((u16)ip->i_block[12]) { get_block((u16)ip->i_block[12], buf2); } 

	setes(segment);

	for (i = 0; i < 12; i++)
	{
		if ((u16)ip->i_block[i])
		{

			get_block((u16)ip->i_block[i],0);
			inces();
		}
	}

	if ((u16)ip->i_block[12])
	{

		up = (u32 *)buf2;
		while(*up)
		{
			get_block((u16)*up, 0);
			up++;
			inces();
		}
	}
	setes(0x1000);

	get_block((u16)ip->i_block[0], buf2);
	hp = (HD *)buf2;

	tsize = hp->tsize;
	dsize = hp->dsize;
	bsize = hp->bsize;

	//shift over the image 32 bits
	/*
	for (i = 0; i < tsize + dsize + bsize; i+=2)
	{
		put_word(get_word(segment, i+32), segment, i);
	}
	*/
}
































































