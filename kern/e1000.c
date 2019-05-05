#include <kern/e1000.h>
#include <kern/pmap.h>
#include <kern/pci.h>

// LAB 6: Your driver code here



int E1000_attach (struct pci_func *pcif)
{
    e1000_mmio_start = mmio_map_region(pcif->reg_base[0], PGSIZE);
    pci_func_enable(pcif);
    struct MainDeviceDescription* des = (struct MainDeviceDescription*)e1000_mmio_start;
    cprintf("the device status register is %08x\n", des->device_status_register);
    return 0;
}
