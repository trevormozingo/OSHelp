	
        .globl _main, _prints      
        .globl _getc,_putc

_getc:
        xorb   ah,ah           ! clear ah
        int    0x16            ! call BIOS to get a char in AX
        andb   al,#0x7F        ! 7-bit ascii  
        ret 
	
        !---------------------------------------------------
        ! int putc(char c)  function: print a char to screen
        !---------------------------------------------------
_putc:           
        push   bp
	mov    bp,sp

        movb   al,4[bp]        ! get the char into aL
        movb   ah,#14          ! aH = 14
        mov    bx,#11       ! cyan
        int    0x10            ! call BIOS to display the char

        pop    bp
	ret
        

