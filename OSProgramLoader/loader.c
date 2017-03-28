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
	
	//get the inode number of the file
	ino = getino(filename); 	

	//now get the inode of that executable file
	ip = getIN(ino);


	//while you still have the offset reference to the kernel (ES still 0x1000) 
	//you have to get the indirect block for later
	//if you do not get it now ES will not be 0x1000 and will not look in the
	//right place for the buf
	if ((u16)ip->i_block[12]) { get_block((u16)ip->i_block[12], buf2); } 

	//now set the ES segment to the umode image beginning of where to load it
	setes(segment);
	
	//for all direct blocks -> load the image into memory
	for (i = 0; i < 12; i++)
	{
		if ((u16)ip->i_block[i])
		{
			//!!!! now load the block into the umode image at ES + offset (bx) 
			//so we use 0 as the offset for the bx register
			//incrementing the bx register by 1024 each time would also work
			// get_block((u16)ip->i_block[i], i * 1024); 

			get_block((u16)ip->i_block[i],0);

			//now increment the ES register 1 block 
			inces();
		}
	}

	//see if there are indirect blocks
	if ((u16)ip->i_block[12])
	{
		//up is a long (32 bit) size
		//set it to the the indirect block
/*
indirect block 

up
|
v
[ 581 | 582 | 583 | 0 | 0 | 0 |                      . . .                              | 836 ]


*/
		up = (u32 *)buf2;
		while(*up)
		{
			//load the indirect block 
			get_block((u16)*up, 0);
			//next indirect block number
			up++;
			//next load ES (+= 1024) 
			inces();
		}
	}
	//now set ES back to kernel mode space
	setes(0x1000);

	//get the header (you need to know the code data and bss size ) 
	get_block((u16)ip->i_block[0], buf2);
	hp = (HD *)buf2;
	tsize = hp->tsize;
	dsize = hp->dsize;
	bsize = hp->bsize;

	//shift over the image 32 bits
	for (i = 0; i < tsize + dsize + bsize; i++)
	{
		put_word(get_word(segment, i+32), segment, i);
	}

/*

you can zero out the bss section if you wanted
so start at: 
tsize + bsize
and while you have not reached the end of bss
set bss to 0
*/

}
































































