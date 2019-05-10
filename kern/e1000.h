#ifndef JOS_KERN_E1000_H
#define JOS_KERN_E1000_H

#define E1000VendorID 0x8086
#define E1000DeviceID 0x100E

#define E1000StatusOffset 0x08
#define E1000TCTLOffset 0x400
#define E1000TIPGOffset 0x410
#define E1000TDLENOffset 0x3808
#define E1000TDBALOffset 0x3800
#define E1000TDBAHOffset 0x3804
#define E1000TDHOffset 0x3810
#define E1000TDTOffset 0x3814

#define TCTL_EN 0X02
#define TCTL_PSP 0X08
#define TCTL_CT 0XFF0
#define TCTL_CT_ETH 0x100
#define TCTL_COLD 0X3FF000
#define TCTL_COLD_1000_HALF_DUPLEX 0X200000
#define TCTL_COLD_1000_FULL_DUPLEX 0x040000
#define TCTL_COLD_10_100_HALF_DUPLEX 0x040000

#define TIPG_IPGT 0x3FF
#define TIPG_IPGR1 0xFFC00
#define TIPG_IPGR2 0X3FF00000
#define TIPG_IPGT_8023 0X00A
#define TIPG_IPGR1_8023 0X01000
#define TIPG_IPGR2_8023 0X00600000


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
