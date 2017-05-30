#include "block.h"
#include "vga.h"
#include "kmalloc.h"
#include "inline_asm.h"

#define REG_ERROR 1
#define REG_SECT_CNT 2
#define REG_SECT_NUM 3
#define REG_CYL_LO 4
#define REG_CYL_HI 5
#define REG_DEV_SELECT 6
#define CMD_PORT 7

#define IDENTIFY_CMD 0xEC
#define ATA_CMD_IDENTIFY_PACKET 0xA1

#define ATADEV_PATAPI 1
#define ATADEV_SATAPI 2
#define ATADEV_PATA 3
#define ATADEV_SATA 4
#define ATADEV_UNKNOWN 0

BlockDev *blockDevHead;



void init_block_devices() {
    
    ATABlockDev *device = malloc(sizeof(ATABlockDev));

    device->requestHead = NULL;
    device->requestTail = NULL;

    int isPrimarySlave, isPrimaryMaster, isSecondarySlave, isSecondaryMaster;    

    Bus primary, secondary;
    
    primary.base = 0x1F0;
    primary.control = 0x3F6;
    secondary.base = 0x170;
    secondary.control = 0x376;
    
    //try to detect device and each bus/channel
    isPrimaryMaster = detect_devtype(0, &primary); 
    isPrimarySlave = detect_devtype(1, &primary);
    isSecondaryMaster = detect_devtype(0, &secondary); 
    isSecondarySlave = detect_devtype(1, &secondary);


    printk("primary master: ");
    printType(isPrimaryMaster);
    printk("primary slave: ");
    printType(isPrimarySlave);
    printk("secondary master: ");
    printType(isSecondaryMaster);
    printk("secondary slave: ");
    printType(isSecondarySlave);

    /*
    device->slave = 0; //set to master
    if (isPrimarySlave || isSecondarySlave) {
        device->slave = 1;
    }

    if (isPrimarySlave || isPrimaryMaster) {
        //its primary
        device->ataBase = 0x1F0;
        device->ataMaster = 0x3F6; //???
        device->irq = 14;
    }
    else {
        //its secondary
        device->ataBase = 0x170;
        device->ataMaster = 0x376; //???
        device->irq = 15;
    }

    device->ataBase = 0x1F0;
    device->ataMaster = 0x3F6; //???
    device->irq = 14;*/


    //temporarily hard coding for debugging: found atapi device in secondary master
    //no other drives exist
    device->slave = 0; 
    device->ataBase = 0x170;
    device->ataMaster = 0x376;
    device->irq = 15;


    ata_identify(device); 

    //TODO:register device
    //finished with initialization?


}

void ata_identify(ATABlockDev *device) {
    
    BlockDev *parent = &device->dev;

    int slavebit = device->slave;
    uint16_t buffer[256];
    
    int poll;

    //send packet command
    outb(device->ataBase + REG_DEV_SELECT, 0xA0 | slavebit << 4);
    
    //delay 1ms
    ata_io_wait(device->ataMaster);

    //set appropriate ports to 0
    outb(device->ataBase + 2, 0);
    outb(device->ataBase + 3, 0);
    outb(device->ataBase + 4, 0);
    outb(device->ataBase + 5, 0);
    
    //send IDENTIFY cmd
    outb(device->ataBase + CMD_PORT, IDENTIFY_CMD);

    //delay 1ms
    ata_io_wait(device->ataMaster);

    if (inb(device->ataBase + CMD_PORT) == 0) {
        printk("THE DRIVE DOES NOT EXIST\n");   
        asm("hlt");
    }
    else {
        //the drive exists

        //poll 
        while ((inb(device->ataBase + CMD_PORT) & 0x80) != 0) {
            printk("%d ", inb(device->ataBase + CMD_PORT));
        }

        if (inb(device->ataBase + 4) && inb(device->ataBase + 5)) {
            printk("NOT ATA DEVICE\n");

            int cl =  inb(device->ataBase + 4);
            int ch = inb(device->ataBase + 5);

            if (cl == 0x14 && ch == 0xEB) {
                printk("ATAPI DEVICE DETECTED\n");
            }
            else {
                printk("NOT A DEVICE\n");
            }   


            //send IDENTIFY PACKET DEVICE cmd (ATAPI version of IDENTIFY)
            outb(device->ataBase + CMD_PORT, ATA_CMD_IDENTIFY_PACKET);

            //sleep for a bit
            ata_io_wait(device->ataMaster);
            ata_io_wait(device->ataMaster);

            //poll just because
            poll = inb(device->ataBase + CMD_PORT);
            while(((poll & 8) == 0) && ((poll & 1) == 0)) {   
                poll = inb(device->ataBase + CMD_PORT);
            }

            printk("atapi: finished polling\n");
        }
        else {
            
            //printk("check: %d\n", inb(device->ataBase + CMD_PORT));

            poll = inb(device->ataBase + CMD_PORT);
            //printk("before loop: %d " , poll);
            while(((poll & 8) == 0) && ((poll & 1) == 0)) {   
                poll = inb(device->ataBase + CMD_PORT);
               // printk("%d " , poll);
            }

            printk("ata: finished polling\n");
        }
        

    }

    //read in 512 bytes of data (256 words)
    insw(device->ataBase, buffer, 256);


    int i;
    //another way of reading in data? same results
    /*for (i = 0; i < 256; i++){
			buffer[i] = inw(device->ataBase);
	}*/

    //print out for debugging
    for (i = 0; i < 256; i++) {

        printk("%d ", buffer[i]);
        //printk("%d: %d \n", i, buffer[i]);
       /* if (buffer[i] == 0) {
            printk("*IDX: %d* ", i);
        }*/
    }



    //check 83 to see if 48 bit LBA 

    printk("83: %d\n", buffer[83]);
    printk("60: %d, 61: %d\n", buffer[60], buffer[61]);
    if (buffer[83] & (1<<10)) {
        //supports it
        printk("supports 48 bit lba!\n");
    }
    else {
        //does not. welp!
        printk("doesn't support 48 bit lba\n");
    }

    uint64_t length;

    length = (uint64_t) buffer[100] << 48 | (uint64_t) buffer[101] << 32 | (uint64_t) buffer[102] << 16 | (uint64_t) buffer[103];

    parent->totalLen = length;

    printk("length: %d\n", length);

}





//OS dev wiki stuff
/* on Primary bus: ctrl->base =0x1F0, ctrl->dev_ctl =0x3F6. REG_CYL_LO=4, REG_CYL_HI=5, REG_DEVSEL=6 */
int detect_devtype (int slavebit, Bus *dev) {

    ata_soft_reset(dev);		/* waits until master drive is ready again */
	outb(dev->base + REG_DEV_SELECT, 0xA0 | slavebit<<4);

    ata_io_wait(dev->control);

	unsigned cl=inb(dev->base + REG_CYL_LO);	/* get the "signature bytes" */
	unsigned ch=inb(dev->base + REG_CYL_HI);
 
	/* differentiate ATA, ATAPI, SATA and SATAPI */
	if (cl==0x14 && ch==0xEB) return ATADEV_PATAPI;
	if (cl==0x69 && ch==0x96) return ATADEV_SATAPI;
	if (cl==0 && ch == 0) return ATADEV_PATA;
	if (cl==0x3c && ch==0xc3) return ATADEV_SATA;
	return ATADEV_UNKNOWN;
}

//os wiki
//software reset 
void ata_soft_reset(Bus *dev) {
    outb(dev->control, 0x04);
	outb(dev->control, 0x00);
}
//os wiki
//delays 1ms. each inb approx 100ns.
void ata_io_wait(uint16_t port){
    int i = 0;

    for (i = 0; i < 10000; i++) {
        inb(port);
    }
}


int BLK_register(BlockDev *dev) {
    //TODO
    
}

//TODO: this isn't done
//assume that destination is large enough!
int read_block(BlockDev *dev, uint64_t blk_num, void *dest) {

    ATABlockDev *this = (ATABlockDev *) dev;

    uint8_t sectorcount_hi, sectorcount_lo;

    uint8_t lba1,lba2,lba3,lba4,lba5,lba6;

    //TODO:what is "sectorcount"??

    lba1 = blk_num & 0xFF;
    lba2 = blk_num >> 8 & 0xFF;
    lba3 = blk_num >> 16 & 0xFF;
    lba4 = blk_num >> 24 & 0xFF;
    lba5 = blk_num >> 32 & 0xFF;
    lba6 = blk_num >> 40 & 0xFF;

    outb(this->ataBase + REG_DEV_SELECT, 0x40 | (this->slave << 4));

    outb (this->ataBase + 2, sectorcount_hi);
    outb (this->ataBase + 3, lba4);
    outb (this->ataBase + 4, lba5);
    outb (this->ataBase + 5, lba6);

    outb (this->ataBase + 2, sectorcount_lo);
    outb (this->ataBase + 3, lba1);
    outb (this->ataBase + 4, lba2);
    outb (this->ataBase + 5, lba3);

    outb(this->ataBase + CMD_PORT, 0x24);

    //TODO:read them all into dest with insw 
}


void printType(int type) {

    if (type == 1) {
        printk("ATAPI\n");
    }
    else if (type == 2) {
        printk("SATAPI\n");
    }
    else if (type == 3) {
        printk("ATA\n");
    }
    else if (type == 4) {
        printk("SATA\n");
    }
    else {
        printk("UNKNOWN\n");
    }

}
