#include <kern/e1000.h>
#include <kern/pmap.h>
#include <kern/pci.h>

// LAB 6: Your driver code here

#define E1000OFFSET(offset) (void*)(e1000_mmio_start+offset)

int E1000_attach (struct pci_func *pcif)
{
    pci_func_enable(pcif);
    e1000_mmio_start = mmio_map_region(pcif->reg_base[0], pcif->reg_size[0]);
    struct MainDeviceDescription* des = (struct MainDeviceDescription*)e1000_mmio_start;
    cprintf("the device status register is %0x\n", *(uint32_t*)E1000OFFSET(E1000StatusOffset));
    return 0;
}
