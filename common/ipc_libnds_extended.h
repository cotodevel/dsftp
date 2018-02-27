#ifndef ipc_libnds_extended_commonipc
#define ipc_libnds_extended_commonipc

#include <nds/ndstypes.h>
#include <nds/ipc.h>

#include <nds/ipc.h>
#include <nds/fifocommon.h>

//---------------------------------------------------------------------------------
typedef struct sMyIPC {
//---------------------------------------------------------------------------------
    int16 touchX,   touchY;   // raw x/y
    int16 touchXpx, touchYpx; // TFT x/y pixel

    int16 touchZ1,  touchZ2;  // TSC x-panel measurements
    uint16 tdiode1,  tdiode2;  // TSC temperature diodes
    uint32 temperature;        // TSC computed temperature

    uint16 buttons;            // keypad buttons
    uint16 buttons_xy_folding;  // X, Y, /PENIRQ buttons
    
    union {
        uint8 curtime[8];        // current time response from RTC

        struct {
                u8 rtc_command;
                u8 rtc_year;           //add 2000 to get 4 digit year
                u8 rtc_month;          //1 to 12
                u8 rtc_day;            //1 to (days in month)

                u8 rtc_incr;
                u8 rtc_hours;          //0 to 11 for AM, 52 to 63 for PM
                u8 rtc_minutes;        //0 to 59
                u8 rtc_seconds;        //0 to 59
        };
    };
    u8 touched;				    //TSC touched?
    u8 touch_pendown;           //TSC already held before?
    uint16 battery;            
    uint16 aux;                
	
	//IPC Clock
    //[0]; //yy
    //[1]; //mth
    //[2]; //dd
    //[3]; //wat - day of week?
    //[4]; //hh
    //[5]; //mm
    //[6]; //ss
    u8 clockdata[0x20];
	
    //APU Core
    int	    skipper_cnt1;
    int	    skipper_cnt2;
    int	    skipper_cnt3;
    int	    skipper_cnt4;
    int		counter;

    //IPC APU
    u32 APU_ADDR_CNT;
    u32 APU_ADDR_CMD;

    u8 APU_ADDR_BLKP[4];
    u32 APU_ADDR_BLK;     //deprecated in v6 but declared
    
    uint32 	TIM0, TIM1, TIM2;
    uint32    T0, T1, T2;

    //transfer queue
    u8 status; //see processor ipc read/writes flags
    u32 buf_queue[0x10];
	
  
} tMyIPC;

//Shared Work     027FF000h 4KB    -     -    -    R/W

//IPC Struct
#define MyIPC ((tMyIPC volatile *)(0x027FF000))

//irqs
#define VCOUNT_LINE_INTERRUPT 159

//arm7 specific
#define KEY_XARM7 (1<<0)
#define KEY_YARM7 (1<<1)
#define KEY_HINGE (1<<7)

#ifdef ARM7
#define 	BIOS_IRQFLAGS   *(__irq_flags)
#endif

#define     irq_vector_addr (__irq_vector)

//processor ipc read/writes flags
#define ARM7_BUSYFLAGRD (u8)(0x08)
#define ARM7_BUSYFLAGWR (u8)(0x0f)
#define ARM9_BUSYFLAGRD (u8)(0x80)
#define ARM9_BUSYFLAGWR (u8)(0xf0)

//tell BGUI lib we use libfat
#define FAT_libfat

#define IPC_OK	0x123
#define IPC_WIFI_SYNC	0x124
#define IPC_BACKLIGHT	0x125
#define IPC_GET	0x126
#define IPC_BACKLIGHT_TOP	0x127
#define IPC_BACKLIGHT_BOTTOM	0x128
#define IPC_BRIGHTNESS	0x129
#define IPC_POWEROFF	0x12a
#define IPC_WIFI_INIT	0x12b
#define PM_POWER_DOWN	0x12c


#endif

#ifdef __cplusplus
extern "C" {
#endif

extern void sendbyte_ipc(uint8 word);
extern u8 recvbyte_ipc();

//direct
extern u32 read_ext_cpu(u32 address,u8 read_mode);
extern void write_ext_cpu(u32 address,u32 value,u8 write_mode);

//gbaemu4ds clock opcodes
extern u8 ipc_get_yearbytertc();
extern u8 ipc_get_monthrtc();
extern u8 ipc_get_dayrtc();
extern u8 ipc_get_dayofweekrtc();
extern u8 ipc_get_hourrtc();
extern u8 ipc_get_minrtc();
extern u8 ipc_get_secrtc();

#ifdef __cplusplus
}
#endif