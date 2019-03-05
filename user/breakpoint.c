// program to cause a breakpoint trap

#include <inc/lib.h>

void
umain(int argc, char **argv)
{
	asm volatile("int $3");
	cprintf("test single step!\n");
	asm volatile("int $3");
}

