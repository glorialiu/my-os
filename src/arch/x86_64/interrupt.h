#include <stdint.h>
#include "memfuncs.h"
#include "inline_asm.h"
#define NUM_IRQS 10
#define NUM_INTERRUPTS 256


extern struct TSS ts_segment;
extern int syscall_flag;

extern void idt_init(void);

extern void IRQ_init(void);
extern void IRQ_set_mask(unsigned char irq);
extern void IRQ_clear_mask(unsigned char irq);
extern int IRQ_get_mask(int IRQline);
extern void IRQ_end_of_interrupt(int irq);

void irq_c_handler(int num, int error, int paramForSysCall);

typedef void (*irq_handler_t)(int, int, void*);

typedef struct IRQT {
    void *arg;
    irq_handler_t handler;
} IRQT;

typedef struct IDT_entry {
    uint64_t offset_15_0:16;
    uint64_t selector:16;
    uint64_t IST:3;
    uint64_t reserved1:5;
    uint64_t type:4;
    uint64_t zero:1;
    uint64_t DPL:2;
    uint64_t present:1;
    uint64_t offset_31_16:16;
    uint64_t offset_63_32:32;
    uint64_t reserved2:32;
} __attribute__((packed)) IDT_entry;

extern void IRQ_set_handler(int irq, irq_handler_t handler, void *arg);


void set_handler_in_IDT(IDT_entry *entry, uint64_t address, uint16_t segment, int tss_st_num);
void set_handler_in_IDT_trap(IDT_entry *entry, uint64_t address, uint16_t segment, int tss_st_num);

void set_handler(IDT_entry *entry, uint64_t address);

void pf_handler(int num, int error, void *arg);
void df_handler(int num, int error, void *arg);
void dummy_handler(int num, int error, void *arg);
void keyboard_handler(int num, int error, void *arg);
void invalid_tss_handler(int num, int error, void *arg);
void gpf_handler(int num, int error, void *arg);
void segment_np_handler(int num, int error, void *arg);
void ata_handler(int num, int error, void *arg);

void set_handler_gpf(IDT_entry *entry, uint64_t address);

void error_code_print(int error);

void TSS_init();

typedef struct IOPB {
    uint64_t ones: 8;
    uint64_t field1: 56;
    uint64_t field2;
} __attribute__((packed)) IOPB;

typedef struct TSS {    
    uint64_t reserved1:32;
    uint64_t privilege_st0;
    uint64_t privilege_st1;
    uint64_t privilege_st2;
    uint64_t reserved2;
    uint64_t interrupt_st1;
    uint64_t interrupt_st2;
    uint64_t interrupt_st3;
    uint64_t interrupt_st4;
    uint64_t interrupt_st5;
    uint64_t interrupt_st6;
    uint64_t interrupt_st7;
    uint64_t reserved3;
    uint64_t reserved4:16;
    uint64_t io_map:16;
} __attribute__((packed)) TSS;

typedef struct TSS_descriptor {
    uint64_t limit_0_15:16;
    uint64_t base_0_23:24;
    uint64_t type:4;
    uint64_t zero:1;
    uint64_t privilege:2;
    uint64_t present:1;
    uint64_t limit_16_19:4;
    uint64_t available:1;
    uint64_t ignored1:2;
    uint64_t granularity:1;
    uint64_t base_24_31:8;
    uint64_t base_32_63:32;
    uint64_t ignored2:8;
    uint64_t zero1:5;
    uint64_t ignored3:19;
} __attribute__((packed)) TSS_descriptor;


typedef struct GDT_entry {
    uint64_t entry;
} __attribute__((packed)) GDT_entry;


extern int reloadSegments;
extern uint64_t CS_start;

extern int irq_gpf_handler;
extern int irq_test_handler;
extern int irq_pf_handler;
extern int irq_df_handler;

extern int irq_handler0;
extern int irq_handler1;
extern int irq_handler2;
extern int irq_handler3;
extern int irq_handler4;
extern int irq_handler5;
extern int irq_handler6;
extern int irq_handler7;
extern int irq_handler8;
extern int irq_handler9;
extern int irq_handler10;
extern int irq_handler11;
extern int irq_handler12;
extern int irq_handler13;
extern int irq_handler14;
extern int irq_handler15;
extern int irq_handler16;
extern int irq_handler17;
extern int irq_handler18;
extern int irq_handler19;
extern int irq_handler20;
extern int irq_handler21;
extern int irq_handler22;
extern int irq_handler23;
extern int irq_handler24;
extern int irq_handler25;
extern int irq_handler26;
extern int irq_handler27;
extern int irq_handler28;
extern int irq_handler29;
extern int irq_handler30;
extern int irq_handler31;
extern int irq_handler32;
extern int irq_handler33;
extern int irq_handler34;
extern int irq_handler35;
extern int irq_handler36;
extern int irq_handler37;
extern int irq_handler38;
extern int irq_handler39;
extern int irq_handler40;
extern int irq_handler41;
extern int irq_handler42;
extern int irq_handler43;
extern int irq_handler44;
extern int irq_handler45;
extern int irq_handler46;
extern int irq_handler47;
extern int irq_handler48;
extern int irq_handler49;
extern int irq_handler50;
extern int irq_handler51;
extern int irq_handler52;
extern int irq_handler53;
extern int irq_handler54;
extern int irq_handler55;
extern int irq_handler56;
extern int irq_handler57;
extern int irq_handler58;
extern int irq_handler59;
extern int irq_handler60;
extern int irq_handler61;
extern int irq_handler62;
extern int irq_handler63;
extern int irq_handler64;
extern int irq_handler65;
extern int irq_handler66;
extern int irq_handler67;
extern int irq_handler68;
extern int irq_handler69;
extern int irq_handler70;
extern int irq_handler71;
extern int irq_handler72;
extern int irq_handler73;
extern int irq_handler74;
extern int irq_handler75;
extern int irq_handler76;
extern int irq_handler77;
extern int irq_handler78;
extern int irq_handler79;
extern int irq_handler80;
extern int irq_handler81;
extern int irq_handler82;
extern int irq_handler83;
extern int irq_handler84;
extern int irq_handler85;
extern int irq_handler86;
extern int irq_handler87;
extern int irq_handler88;
extern int irq_handler89;
extern int irq_handler90;
extern int irq_handler91;
extern int irq_handler92;
extern int irq_handler93;
extern int irq_handler94;
extern int irq_handler95;
extern int irq_handler96;
extern int irq_handler97;
extern int irq_handler98;
extern int irq_handler99;
extern int irq_handler100;
extern int irq_handler101;
extern int irq_handler102;
extern int irq_handler103;
extern int irq_handler104;
extern int irq_handler105;
extern int irq_handler106;
extern int irq_handler107;
extern int irq_handler108;
extern int irq_handler109;
extern int irq_handler110;
extern int irq_handler111;
extern int irq_handler112;
extern int irq_handler113;
extern int irq_handler114;
extern int irq_handler115;
extern int irq_handler116;
extern int irq_handler117;
extern int irq_handler118;
extern int irq_handler119;
extern int irq_handler120;
extern int irq_handler121;
extern int irq_handler122;
extern int irq_handler123;
extern int irq_handler124;
extern int irq_handler125;
extern int irq_handler126;
extern int irq_handler127;
extern int irq_handler128;
extern int irq_handler129;
extern int irq_handler130;
extern int irq_handler131;
extern int irq_handler132;
extern int irq_handler133;
extern int irq_handler134;
extern int irq_handler135;
extern int irq_handler136;
extern int irq_handler137;
extern int irq_handler138;
extern int irq_handler139;
extern int irq_handler140;
extern int irq_handler141;
extern int irq_handler142;
extern int irq_handler143;
extern int irq_handler144;
extern int irq_handler145;
extern int irq_handler146;
extern int irq_handler147;
extern int irq_handler148;
extern int irq_handler149;
extern int irq_handler150;
extern int irq_handler151;
extern int irq_handler152;
extern int irq_handler153;
extern int irq_handler154;
extern int irq_handler155;
extern int irq_handler156;
extern int irq_handler157;
extern int irq_handler158;
extern int irq_handler159;
extern int irq_handler160;
extern int irq_handler161;
extern int irq_handler162;
extern int irq_handler163;
extern int irq_handler164;
extern int irq_handler165;
extern int irq_handler166;
extern int irq_handler167;
extern int irq_handler168;
extern int irq_handler169;
extern int irq_handler170;
extern int irq_handler171;
extern int irq_handler172;
extern int irq_handler173;
extern int irq_handler174;
extern int irq_handler175;
extern int irq_handler176;
extern int irq_handler177;
extern int irq_handler178;
extern int irq_handler179;
extern int irq_handler180;
extern int irq_handler181;
extern int irq_handler182;
extern int irq_handler183;
extern int irq_handler184;
extern int irq_handler185;
extern int irq_handler186;
extern int irq_handler187;
extern int irq_handler188;
extern int irq_handler189;
extern int irq_handler190;
extern int irq_handler191;
extern int irq_handler192;
extern int irq_handler193;
extern int irq_handler194;
extern int irq_handler195;
extern int irq_handler196;
extern int irq_handler197;
extern int irq_handler198;
extern int irq_handler199;
extern int irq_handler200;
extern int irq_handler201;
extern int irq_handler202;
extern int irq_handler203;
extern int irq_handler204;
extern int irq_handler205;
extern int irq_handler206;
extern int irq_handler207;
extern int irq_handler208;
extern int irq_handler209;
extern int irq_handler210;
extern int irq_handler211;
extern int irq_handler212;
extern int irq_handler213;
extern int irq_handler214;
extern int irq_handler215;
extern int irq_handler216;
extern int irq_handler217;
extern int irq_handler218;
extern int irq_handler219;
extern int irq_handler220;
extern int irq_handler221;
extern int irq_handler222;
extern int irq_handler223;
extern int irq_handler224;
extern int irq_handler225;
extern int irq_handler226;
extern int irq_handler227;
extern int irq_handler228;
extern int irq_handler229;
extern int irq_handler230;
extern int irq_handler231;
extern int irq_handler232;
extern int irq_handler233;
extern int irq_handler234;
extern int irq_handler235;
extern int irq_handler236;
extern int irq_handler237;
extern int irq_handler238;
extern int irq_handler239;
extern int irq_handler240;
extern int irq_handler241;
extern int irq_handler242;
extern int irq_handler243;
extern int irq_handler244;
extern int irq_handler245;
extern int irq_handler246;
extern int irq_handler247;
extern int irq_handler248;
extern int irq_handler249;
extern int irq_handler250;
extern int irq_handler251;
extern int irq_handler252;
extern int irq_handler253;
extern int irq_handler254;
extern int irq_handler255;
