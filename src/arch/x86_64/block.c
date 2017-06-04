#include "block.h"
#include "vga.h"
#include "kmalloc.h"
#include "inline_asm.h"
#include "interrupt.h"
#include "process.h"
#include "mbr.h"

#define REG_ERROR 1
#define REG_SECT_CNT 2
#define REG_SECT_NUM 3
#define REG_CYL_LO 4
#define REG_CYL_HI 5
#define REG_DEV_SELECT 6
#define CMD_PORT 7

#define IDENTIFY_CMD 0xEC
#define ATA_CMD_IDENTIFY_PACKET 0xA1

#define CACHE_FLUSH 0xE7

#define ATADEV_PATAPI 1
#define ATADEV_SATAPI 2
#define ATADEV_PATA 3
#define ATADEV_SATA 4
#define ATADEV_UNKNOWN 0

#define NIEN 2

#define BUSY 1<<7
#define DRQ 1<<3
#define ERROR 1
#define MULTIPLE_COMMAND 0xC4

BlockDev *blockDevHead;

ProcessQueue *ataPQ;

ATACmd *ataCmdHead;

ATABlockDev *tempDev;

void init_ata_read_queue() {
    ataPQ = malloc(sizeof(ProcessQueue));

    ataPQ->head = NULL;

    ataCmdHead = NULL;

}

void block_test_thread(int block) {
    uint16_t buffer[256];
    printk("block test thread %d called\n", block);
    ata_read_block(block, buffer, 256);
    printk("TEST: %x\n", buffer[255]);
    //printk("finished reading buffer\n");
    kexit();
}
void read_mbr_block() {
    uint16_t buffer[256];
    ata_read_block(0, buffer, 256);
    printk("MBR TEST: %x\n", buffer[255]);
    parse_mbr(buffer);

    kexit();
}

void read_bpb_block() {
    uint16_t buffer[256];
    ata_read_block(2048, buffer, 256);
    //printk("MBR TEST: %x\n", buffer[255]);
    parse_bpb(buffer);

    kexit();
}

void ata_read_block(uint64_t lba, void *dst, uint64_t len) {
    printk("ata read block called\n");
    //malloc and intialize command

    ATACmd *new = malloc(sizeof(ATACmd));
  
    new->dst = dst;
    new->lba = lba;
    new->len = len;
    new->queue = ataPQ;
    new->next = NULL;
    new->complete = FALSE;
    
    //enqueue command
    enqueue_command(new);

    while (!new->complete) {
        //PROC_block_on
        PROC_block_on(ataPQ, TRUE);
        cli();
    }

    
}

//assuming the queue head is not null
void dequeue_command() {
    //remove head of queue and free it
    if (!ataCmdHead) {
        printk("this shouldn't be null\n");
    }
    else {
        ataCmdHead->complete = TRUE;
        //ATACmd *temp = ataCmdHead;
        ataCmdHead = ataCmdHead->next;
    
       // free(temp);
    }
}


void enqueue_command(ATACmd *cmd) {

    printk("enqueue command\n");
    ATACmd *iter;
    //if queue head is null, add to queue and send read command right away
    if (!ataCmdHead) {
        ataCmdHead = cmd;
        //send read command here!
        int blockNum = cmd->lba; //temporarily doing this
        read_block((BlockDev *) tempDev, blockNum, cmd->dst);
    }
    else {
        //if queue head is not null, add to queue
        iter = ataCmdHead;
        while (iter->next) {
            iter = iter->next;
        }
        iter->next = cmd;
    }
}

void ata_isr() {

    printk("made it into the isr\n");

    inb(tempDev->ataBase + CMD_PORT);

    int blockNum;
    
    
    if (!ataCmdHead) {
        printk("non data related interrupt\n");
    }
    else {
    
        while (inb(tempDev->ataMaster) & BUSY) {

        }
        if (inb(tempDev->ataMaster) & DRQ) {

            printk("data read\n")
;            uint16_t * dstBuffer = ataCmdHead->dst;
            insw(tempDev->ataBase, dstBuffer, 256);
            //printk("hi: %x\n", dstBuffer[255]);

            dequeue_command();
            PROC_unblock_head(ataPQ);
            
        }
        else {
            
            printk("ERROR: data not read\n");
        }

        //send next read command
        if (ataCmdHead) {
            blockNum = ataCmdHead->lba; //TODO: temprorary using lba as block num , do conversion later
            read_block((BlockDev *) tempDev, blockNum, ataCmdHead->dst);
        }
    }

        //clears interrupt. dont hard code this lol
        if (inb(tempDev->ataMaster) & BUSY) {
            printk("error\n");
        }
        //outb(0x3F6, 0);
    
}


//TODO: this isn't done
//assume that destination is large enough!
int read_block(BlockDev *dev, uint64_t blk_num, void *dest) {


    //printk("read block called\n");
    ATABlockDev *this = (ATABlockDev *) dev;

    uint8_t sectorcount_hi = 0;
    uint8_t sectorcount_lo = 1;

    uint64_t lba1,lba2,lba3,lba4,lba5,lba6;

    uint64_t lba = blk_num; //temp

    lba1 = blk_num & 0xFF;// (lba & 0x000000FF) >> 0;//
    lba2 = blk_num >> 8 & 0xFF; //(lba & 0x0000FF00) >> 8;//
    lba3 = blk_num >> 16 & 0xFF; //(lba & 0x00FF0000) >> 16;//
    lba4 = blk_num >> 24 & 0xFF; // (lba & 0xFF000000) >> 24;//
    lba5 = blk_num >> 32 & 0xFF;
    lba6 = blk_num >> 40 & 0xFF;

    /*
    while (inb(this->ataBase+CMD_PORT) & 0x80) {
    }*/

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

   // insw(this->ataBase, dest, 256);

    /*
    int loop = 1;
    while(loop) {
}
    /*
    inb(0x3F6);
    inb(0x3F6);
    inb(0x3F6);
    inb(0x3F6);*/


}

void ata_init() {

    init_ata_read_queue();
    
    ATABlockDev *device = malloc(sizeof(ATABlockDev));

    device->requestHead = NULL;
    device->requestTail = NULL;

   /* int isPrimarySlave, isPrimaryMaster, isSecondarySlave, isSecondaryMaster; 
    Bus primary, secondary;
    primary.base = 0x1F0;
    primary.control = 0x3F6;
    secondary.base = 0x170;
    secondary.control = 0x376;*/

    device->slave = 0;
    device->ataBase = 0x1F0;
    device->ataMaster = 0x3F6;
    device->irq = 14;

    //ata
    tempDev = device;

    IRQ_clear_mask(2);
    IRQ_clear_mask(14); 

    ata_identify(device);

    
    
    /*
    //disable interrupts on slave
    outb(device->ataBase+REG_DEV_SELECT, 0x10);     //select slave
    ata_io_wait(device->ataMaster);
    outb(device->ataMaster, NIEN);
    ata_io_wait(device->ataMaster);   
    outb(device->ataBase+REG_DEV_SELECT, 0); //select master
    ata_io_wait(device->ataMaster);
    if (inb(device->ataMaster) & ( BUSY | DRQ ) == 0) {
        outb(device->ataBase + REG_SECT_NUM, 1);
    }
    else {
        printk("failed\n");    
    } 
    outb(device->ataBase + CMD_PORT, MULTIPLE_COMMAND);

    if (inb(device->ataMaster) & ERROR) {
        printk("error w multiple command\n");    
    }

    inb(device->ataBase);//to clear interrupt
    outb(device->ataMaster, 0); // clear nien to enable interrupts
  */
    




    //TODO:register device



    /*
    uint16_t buffer[256];
    read_block((BlockDev *) device, 0, buffer);
    printk("should be AA55 if everything is working: %x\n", buffer[255]);

    */
    //read_block((BlockDev *) device, 2048, buffer);
    //read_bpb(buffer);
    
   // printk("jmp: %x\n", buffer[0]);
    //inb(device->ataBase + CMD_PORT);
    //insw(device->ataBase, buffer, 256);


    //printk("should be AA55 if everything is working: %x\n", buffer[255]);
    //parse_mbr(buffer);

    //temporarily turning off interrupts for debug purposes



}

void ata_identify(ATABlockDev *device) {
    
    BlockDev *parent = &device->dev;

    int slavebit = device->slave;
    uint16_t buffer[256];
    
    int poll;

   // inb(device->ataBase + CMD_PORT);
   // inb(device->ataMaster);

    //clear nIEN bit to enable interrupts
    outb(device->ataMaster, 0);

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
            //ata_io_wait(device->ataMaster);
            //ata_io_wait(device->ataMaster);

            //poll just because
            poll = inb(device->ataBase + CMD_PORT);
            while(((poll & 8) == 0) && ((poll & 1) == 0)) {   
                poll = inb(device->ataBase + CMD_PORT);
            }
        }
        else {
            poll = inb(device->ataBase + CMD_PORT);
            while(((poll & 8) == 0) && ((poll & 1) == 0)) {   
                poll = inb(device->ataBase + CMD_PORT);
            }
        }
        

    }

    //read in 512 bytes of data (256 words)
    insw(device->ataBase, buffer, 256);

    //another way of reading in data? same results. debug info.
    /*
    int i;
    for (i = 0; i < 256; i++){
			buffer[i] = inw(device->ataBase);
	}*/

    //check 83rd uint16_t to see if 48 bit LBA 
    if (!(buffer[83] & (1<<10))) {
        printk("doesn't support 48 bit lba\n");
        // 60 and 61st uints are how many are 28 bit addressable
        // add support for this at some point? nah
        asm("hlt");
    }

    //TODO: not sure if this length is correct
    uint64_t length = *( (uint64_t *) (buffer + 100));
    parent->totalLen = length;

    outb(device->ataBase + REG_DEV_SELECT, 0x40 | (device->slave << 4));
    ata_io_wait(device->ataMaster);

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

    /*
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
