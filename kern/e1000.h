#ifndef JOS_KERN_E1000_H
#define JOS_KERN_E1000_H

#define E1000VendorID 0x8086
#define E1000DeviceID 0x100E

#define E1000StatusOffset 0x08
#define E1000TDLENOffset 0x3808
#define E1000TDBALOffset 0x3800
#define E1000TDBAHOffset 0x3804

#define TRANSMIT_DES_LIST_NUMBER 64

#include <kern/pci.h>

volatile void* e1000_mmio_start;
int E1000_attach (struct pci_func *pcif);




struct MainDeviceDescription
{
    /* data */
    uint64_t device_control_register;
    uint64_t device_status_register;
};

struct TransmitDes
{
	uint64_t addr;
	uint16_t length;
	uint8_t cso;
	uint8_t cmd;
	uint8_t status;
	uint8_t css;
	uint16_t special;
};

void* e1000_memory_base_address;
struct TransmitDes transmit_descriptor_list[TRANSMIT_DES_LIST_NUMBER] __attribute__((aligned(128)));

#endif  // SOL >= 6
