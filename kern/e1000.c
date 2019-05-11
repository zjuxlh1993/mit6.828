#include <kern/e1000.h>
#include <kern/pmap.h>
#include <kern/pci.h>
#include <inc/error.h>

// LAB 6: Your driver code here

#define E1000Set(offset,value) *((uint32_t*)((uint32_t)(e1000_mmio_start)+(offset)))=(uint32_t)value
#define E1000Get(offset) *((uint32_t*)((uint32_t)(e1000_mmio_start)+(offset)))

uint32_t transmit_list_head;
uint32_t transmit_list_tail;

int e1000_transmit_init();

int E1000_attach (struct pci_func *pcif)
{ 
    int r=0;
    pci_func_enable(pcif);
    e1000_mmio_start = mmio_map_region(pcif->reg_base[0], pcif->reg_size[0]);
    cprintf("e1000 mmio base:%0x8 size:%0x8 kva:%0x8\n",pcif->reg_base[0], pcif->reg_size[0], e1000_mmio_start);
    struct MainDeviceDescription* des = (struct MainDeviceDescription*)e1000_mmio_start;
    cprintf("the device status register is %08x\n", des->device_status_register);

    r = e1000_transmit_init();
    if (r<0)
        return r;

    return 0;
}

int e1000_transmit_init()
{
    E1000Set(E1000TDBALOffset, PADDR((void*)transmit_descriptor_list));
    E1000Set(E1000TDBAHOffset, 0x0);
    //make sure TDT & TDH are 0
    E1000Set(E1000TDHOffset, 0x0);
    E1000Set(E1000TDTOffset, 0x0);

    //initialize the transmit contorl register
    E1000Set(E1000TCTLOffset, E1000Get(E1000TCTLOffset)|(~TCTL_EN));
    E1000Set(E1000TCTLOffset, E1000Get(E1000TCTLOffset)|(~TCTL_PSP));
    E1000Set(E1000TCTLOffset, (E1000Get(E1000TCTLOffset)&(~TCTL_CT))|TCTL_CT_ETH);
    E1000Set(E1000TCTLOffset, (E1000Get(E1000TCTLOffset)&(~TCTL_COLD))|TCTL_COLD_1000_FULL_DUPLEX);
    //set transmit ipg
    E1000Set(E1000TIPGOffset, TIPG_IPGR1_8023 | TIPG_IPGR2_8023 | TIPG_IPGT_8023);
    E1000Set(E1000TDLENOffset,TRANSMIT_DES_LIST_NUMBER);
    return 0;
}