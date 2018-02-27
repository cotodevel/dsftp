#include "touch_ipc.h"
#include <nds.h>
#include <nds/touch.h>
#include "ipc_libnds_extended.h"

//usage: struct touchPosition touchposition;
//touchReadXY_gbaemu4ds(&touchposition);

void touchReadXY_ipc(touchPosition * touchpos_inst){
    
    touchpos_inst->rawx =   MyIPC->touchX;
    touchpos_inst->rawy =   MyIPC->touchY;
    
    //TFT x/y pixel
    touchpos_inst->px   =   MyIPC->touchXpx;
    touchpos_inst->py   =   MyIPC->touchYpx;
    
    touchpos_inst->z1   =   MyIPC->touchZ1;
    touchpos_inst->z2   =   MyIPC->touchZ2;
}