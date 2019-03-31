// implement fork from user space

#include <inc/string.h>
#include <inc/lib.h>

// PTE_COW marks copy-on-write page table entries.
// It is one of the bits explicitly allocated to user processes (PTE_AVAIL).
#define PTE_COW		0x800

//
// Custom page fault handler - if faulting page is copy-on-write,
// map in our own private writable copy.
//
static void
pgfault(struct UTrapframe *utf)
{
	void *addr = (void *) utf->utf_fault_va;
	uint32_t err = utf->utf_err;
	int r;
    
	// Check that the faulting access was (1) a write, and (2) to a
	// copy-on-write page.  If not, panic.
	// Hint:
	//   Use the read-only page table mappings at uvpt
	//   (see <inc/memlayout.h>).

	// LAB 4: Your code here.
    	//cprintf("fault %x\n",addr);
    	//cprintf("[%08x] fault %x\n", thisenv->env_id, uvpt[PGNUM(addr)]);
    	pte_t* user_pgdir = (pte_t*)UVPT;
    	pte_t pte = user_pgdir[PGNUM(addr)];
    	//cprintf("enter pgfalut %x\n",pte);
    	if (!(pte & PTE_COW)){
        	panic("the page is not a copy-on-write page");
    	}

	// Allocate a new page, map it at a temporary location (PFTEMP),
	// copy the data from the old page to the new page, then move the new
	// page to the old page's address.
	// Hint:
	//   You should make three system calls.

	// LAB 4: Your code here.

    	if ((r = sys_page_alloc(0, (void*)PFTEMP, PTE_U | PTE_P | PTE_W))<0)
        	panic("user page alloc %e", r);
    	memcpy((void*)PFTEMP, ROUNDDOWN(addr, PGSIZE), PGSIZE);
    	if ((r = sys_page_map(0, (void*)PFTEMP, 0, ROUNDDOWN(addr, PGSIZE), PTE_U | PTE_P | PTE_W))<0)
        	panic("user page map %e", r);
    	if ((r = sys_page_unmap(0, (void*)PFTEMP))<0)
        	panic("user page unmap %e", r);   
	//panic("pgfault not implemented");
}

//
// Map our virtual page pn (address pn*PGSIZE) into the target envid
// at the same virtual address.  If the page is writable or copy-on-write,
// the new mapping must be created copy-on-write, and then our mapping must be
// marked copy-on-write as well.  (Exercise: Why do we need to mark ours
// copy-on-write again if it was already copy-on-write at the beginning of
// this function?)
//
// Returns: 0 on success, < 0 on error.
// It is also OK to panic on error.
//
static int
duppage(envid_t envid, unsigned pn)
{
	int r;

	// LAB 4: Your code here.
    //cprintf("%x %x %x\n",&uvpt[pn],&uvpd[pn/NPTENTRIES],pn*PGSIZE);
    if (!(uvpd[pn/NPTENTRIES] & PTE_P))
    	return 0;
    pte_t pte = uvpt[pn];
    uint32_t perm = PGOFF(pte);
    
    if (!(perm & PTE_P)|| !(perm & PTE_U))
        return 0;
    if (perm & PTE_SHARE){
         r = sys_page_map(0, (void*)(pn*PGSIZE), envid, (void*)(pn*PGSIZE), perm&PTE_SYSCALL);
         return r;    	
    }
    if ((perm & PTE_W) || (perm & PTE_COW)){
        perm &= ~PTE_W;
        perm |= PTE_COW;
        //cprintf("%x %x %x %x\n",uvpt[pn],uvpd[pn/NPTENTRIES],pn*PGSIZE, perm);
        if ((r = sys_page_map(0, (void*)(pn*PGSIZE), envid, (void*)(pn*PGSIZE), perm&PTE_SYSCALL))<0)
            	return r; 
        if ((r = sys_page_set_perm(0, (void*)(pn*PGSIZE), perm&PTE_SYSCALL))<0)
            	return r;
        //cprintf("%x %x %x\n",uvpt[pn],uvpd[pn/NPTENTRIES],pn*PGSIZE);       
    } else {
        if ((r = sys_page_map(0, (void*)(pn*PGSIZE), envid, (void*)(pn*PGSIZE), perm&PTE_SYSCALL))<0)
            	return r;          
    }

	//panic("duppage not implemented");
	return 0;
}

//
// User-level fork with copy-on-write.
// Set up our page fault handler appropriately.
// Create a child.
// Copy our address space and page fault handler setup to the child.
// Then mark the child as runnable and return.
//
// Returns: child's envid to the parent, 0 to the child, < 0 on error.
// It is also OK to panic on error.
//
// Hint:
//   Use uvpd, uvpt, and duppage.
//   Remember to fix "thisenv" in the child process.
//   Neither user exception stack should ever be marked copy-on-write,
//   so you must allocate a new page for the child's user exception stack.
//
void (*_pgfault_handler)(struct UTrapframe *utf);
extern void _pgfault_upcall(void);
envid_t
fork(void)
{
	// LAB 4: Your code here.
    int r;
    set_pgfault_handler(pgfault);
    envid_t envid;
    envid = sys_exofork();
    //cprintf("envid:%x %x\n",envid,_pgfault_handler);
    if (envid < 0)
		return envid;
    if (envid == 0) {
		// We're the child.
		// The copied value of the global variable 'thisenv'
		// is no longer valid (it refers to the parent!).
		// Fix it and return 0.
		//cprintf("test1\n");
		thisenv = &envs[ENVX(sys_getenvid())];
		//cprintf("test2\n");
		return 0;
    }
    pte_t* user_pgdir = (pte_t*)UVPT;
    for (uint32_t i = 0; i<PGNUM(UTOP); i++){
    	if (i == PGNUM(UXSTACKTOP)-1){
    		if ((r=sys_page_alloc(envid, (void*)(UXSTACKTOP-PGSIZE), PTE_U | PTE_W | PTE_P))<0)
    			panic("sys_page_alloc fault");
    		continue;
    	}
        r = duppage(envid, i);
        if (r<0){
            cprintf("%x\n",r);
            return r;
        }
    }
    // Start the child environment running
        if ((r = sys_env_set_pgfault_upcall(envid, _pgfault_upcall)<0))
            panic("set pgfault upcall falut %e" ,r);
	if ((r = sys_env_set_status(envid, ENV_RUNNABLE)) < 0)
		panic("sys_env_set_status: %e", r);
    return envid;
	// panic("fork not implemented");
}

// Challenge!
int
sfork(void)
{
	panic("sfork not implemented");
	return -E_INVAL;
}
