#ifndef JOS_KERN_E1000_H
#define JOS_KERN_E1000_H

#define E1000VendorID 0x8086
#define E1000DeviceID 0x100E

#include <kern/pci.h>

int E1000_attach (struct pci_func *pcif);

#endif  // SOL >= 6
