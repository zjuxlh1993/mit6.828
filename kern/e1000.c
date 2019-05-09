#include <kern/e1000.h>
#include <kern/pmap.h>
#include <kern/pci.h>
#include <inc/error.h>

// LAB 6: Your driver code here

#define E1000OFFSET(offset)

int E1000_attach (struct pci_func *pcif)
{ 
    int r;

    struct PageInfo *pg = page_alloc(ALLOC_ZERO);
	if (!pg)
		return -E_NO_MEM;

    pci_func_enable(pcif);
    e1000_mmio_start = mmio_map_region(pcif->reg_base[0], PGSIZE);
    struct MainDeviceDescription* des = (struct MainDeviceDescription*)e1000_mmio_start;
    cprintf("the device status register is %08x", des->device_status_register);

    r = e1000_transmit_init();
    if (r<0)
        return r;

    return 0;
}

int e1000_transmit_init()
{

}