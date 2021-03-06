#include <inc/assert.h>
#include <inc/x86.h>
#include <kern/spinlock.h>
#include <kern/env.h>
#include <kern/pmap.h>
#include <kern/monitor.h>

void sched_halt(void);

// Choose a user environment to run and run it.
void
sched_yield(void)
{
	struct Env *idle;
	// Implement simple round-robin scheduling.
	//
	// Search through 'envs' for an ENV_RUNNABLE environment in
	// circular fashion starting just after the env this CPU was
	// last running.  Switch to the first such environment found.
	//
	// If no envs are runnable, but the environment previously
	// running on this CPU is still ENV_RUNNING, it's okay to
	// choose that environment. Make sure curenv is not null before
	// dereferencing it.
	//
	// Never choose an environment that's currently running on
	// another CPU (env_status == ENV_RUNNING). If there are
	// no runnable environments, simply drop through to the code
	// below to halt the cpu.

	// LAB 4: Your code here.
	//warn("%x %x %x %x %x",curenv, cpunum(), cpus, cpus[1].cpu_id);
	//warn("sched_yield env0:%d env1:%d env:%d",envs[0].env_status,envs[1].env_status,envs[2].env_status);
	uint32_t env_id;
	if (curenv != NULL)
		env_id = ENVX(curenv->env_id);
	else
		env_id = NENV-1;
	//warn("test %d",envs[NENV-1].env_status);
	for (uint32_t i = (env_id + 1)%NENV; i!=env_id; i = (i+1)%NENV){
		if (envs[i].env_status == ENV_RUNNABLE){
			if (curenv && curenv->env_status == ENV_RUNNING)
				curenv->env_status = ENV_RUNNABLE;
			curenv = &envs[i];
			curenv->env_status = ENV_RUNNING;
			//warn("cpu %d choose %x",cpunum(), curenv->env_id);
			//warn("ENV_RUNNABLE env0:%d env1:%d env:%d",envs[0].env_status,envs[1].env_status,envs[2].env_status);
			env_run(curenv);
		}
	}
	// warn("%x id request sched %x", env_id, env_id);
	if (curenv && curenv->env_status == ENV_RUNNING){
		//warn("cpu %d choose %x",cpunum(),curenv->env_id);
		//warn("ENV_RUNNING env0:%d env1:%d env:%d",envs[0].env_status,envs[1].env_status,envs[2].env_status);
		env_run(curenv);		
	}
	//warn("nothing!");
	// sched_halt never returns
	//warn("nothing!");
	sched_halt();
}

// Halt this CPU when there is nothing to do. Wait until the
// timer interrupt wakes it up. This function never returns.
//
void
sched_halt(void)
{
	int i;
	
	// For debugging and testing purposes, if there are no runnable
	// environments in the system, then drop into the kernel monitor.
	for (i = 0; i < NENV; i++) {
		if ((envs[i].env_status == ENV_RUNNABLE ||
		     envs[i].env_status == ENV_RUNNING ||
		     envs[i].env_status == ENV_DYING))
			break;
	}
	//warn("nothing!");
	if (i == NENV) {
		cprintf("No runnable environments in the system!\n");
		while (1)
			monitor(NULL);
	}
	// warn("nothing!");
	// Mark that no environment is running on this CPU
	curenv = NULL;
	lcr3(PADDR(kern_pgdir));
	//warn("nothing!");
	// Mark that this CPU is in the HALT state, so that when
	// timer interupts come in, we know we should re-acquire the
	// big kernel lock
	xchg(&thiscpu->cpu_status, CPU_HALTED);
	// warn("nothing!");
	// Release the big kernel lock as if we were "leaving" the kernel
	unlock_kernel();
	// warn("nothing!");
	// Reset stack pointer, enable interrupts and then halt.
	asm volatile (
		"movl $0, %%ebp\n"
		"movl %0, %%esp\n"
		"pushl $0\n"
		"pushl $0\n"
		// Uncomment the following line after completing exercise 13
		"sti\n"
		"1:\n"
		"hlt\n"
		"jmp 1b\n"
	: : "a" (thiscpu->cpu_ts.ts_esp0));
}

