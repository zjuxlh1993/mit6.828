// Fork a binary tree of processes and display their structure.

#include <inc/lib.h>

#define DEPTH 3

void forktree(const char *cur);

void
forkchild(const char *cur, char branch)
{
	char nxt[DEPTH+1];
	nxt[DEPTH] = 0;
	if (strlen(cur) >= DEPTH)
		return;
	snprintf(nxt, DEPTH+1, "%s%c", cur, branch);
	//nxt[strlen(cur) + 1] = 0;
	//cprintf("%x %s\n", thisenv->env_id, nxt);
	if (fork() == 0) {
		forktree(nxt);
		exit();
	}
}

void
forktree(const char *cur)
{
	cprintf("%04x: I am '%s'\n", sys_getenvid(), cur);

	forkchild(cur, '0');
	forkchild(cur, '1');
}

void
umain(int argc, char **argv)
{
	forktree("");
}

