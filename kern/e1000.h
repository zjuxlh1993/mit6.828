#ifndef JOS_KERN_E1000_H
#define JOS_KERN_E1000_H

#define E1000VendorID 0x8086
#define E1000DeviceID 0x100E

#define E1000StatusOffset 0x08

#include <kern/pci.h>

volatile void* e1000_mmio_start;
int E1000_attach (struct pci_func *pcif);

struct MainDeviceDescription
{
    /* data */
    uint64_t device_control_register;
    uint64_t device_status_register;
};


#endif  // SOL >= 6
