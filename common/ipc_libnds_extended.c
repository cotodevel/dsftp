#include "ipc_libnds_extended.h"

#include <nds.h>
#include <nds/ndstypes.h>
#include <nds/system.h>
#include <nds/input.h>
#include <nds/interrupts.h>

#include <nds/bios.h>
#include <nds/ipc.h>
#include <stdlib.h>


#ifdef ARM7
#include <nds/arm7/clock.h>
#include <nds/arm7/touch.h>

//coto: original libnds touchscreen usage.
void extendedIPC()
{
	uint16 but=0, batt=0;
	int t1=0, t2=0;
	uint32 temp=0;
	u8 clock_buf[sizeof(MyIPC->clockdata)];
	u32 i;
	
    touchPosition tempPos;
    touchReadXY(&tempPos);
     
	// Read the touch screen
	but = REG_KEYXY;
	batt = touchRead(TSC_MEASURE_BATTERY);
 
	// Read the time
	//rtcGetTime((uint8 *)clock_buf);
	//BCDToInteger((uint8 *)&(ct[1]), 7);
	
	// Read the time
	rtcGetTimeAndDate(clock_buf);
	
	// Read the temperature
	temp = touchReadTemperature(&t1, &t2);
 
	// Update the MyIPC struct
	MyIPC->buttons	            	= REG_KEYINPUT;
    MyIPC->buttons_xy_folding		= but;
	MyIPC->touched                  = (u8)CheckStylus();
    MyIPC->touch_pendown           = (u8)touchPenDown();
    
    //raw x/y
    MyIPC->touchX    = tempPos.rawx;
    MyIPC->touchY    = tempPos.rawy;
    
    //TFT x/y pixel
    MyIPC->touchXpx = tempPos.px;
    MyIPC->touchYpx = tempPos.py;    
    
	MyIPC->touchZ1 = tempPos.z1;
	MyIPC->touchZ2 = tempPos.z2;
	MyIPC->battery		= batt;
 
	//Get time
    for(i=0; i< sizeof(clock_buf); i++){
        MyIPC->clockdata[i] = clock_buf[i];
    }
 
	MyIPC->temperature = temp;
	MyIPC->tdiode1 = t1;
	MyIPC->tdiode2 = t2;	
}
#endif

tMyIPC * getIPC(){
	return (tMyIPC *)MyIPC;
}

//arm7 / arm9 shared:
//usage: if(getKeys() & KEY_L) ...
u32 getKeys(){
	tMyIPC * IPCInst = getIPC();
	return (( ((~REG_KEYINPUT)&0x3ff) | (((~IPCInst->buttons_xy_folding)&3)<<10) | (((~IPCInst->buttons_xy_folding)<<6) & (KEY_TOUCH|KEY_LID) ))^KEY_LID);
}

//keys


//clock opcodes
u8 ipc_get_yearbytertc(){
	return (u8)(u32)MyIPC->clockdata[0];
}

u8 ipc_get_monthrtc(){
	return (u8)(u32)MyIPC->clockdata[1];
}

u8 ipc_get_dayrtc(){
	return (u8)(u32)MyIPC->clockdata[2];
}

u8 ipc_get_dayofweekrtc(){
	return (u8)(u32)MyIPC->clockdata[3];
}


u8 ipc_get_hourrtc(){
	return (u8)(u32)MyIPC->clockdata[4];
}

u8 ipc_get_minrtc(){
	return (u8)(u32)MyIPC->clockdata[5];
}

u8 ipc_get_secrtc(){
	return (u8)(u32)MyIPC->clockdata[6];
}