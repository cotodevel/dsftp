// Exec.cpp (this is -*- C++ -*-)
// 
// \author: Bjoern Giesler <bjoern@giesler.de>
// 
// 
// 
// $Author: giesler $
// $Locker$
// $Revision$
// $Date: 2002-08-19 10:41:28 +0200 (Mon, 19 Aug 2002) $

#define _GLIBCXX_USE_CXX11_ABI 0

/* system includes */
#include <stdio.h>
#include <string.h>
#include <nds.h>
#include <nds/arm9/console.h>
#include <nds/registers_alt.h>

/* my includes */
#include "Exec.h"

#define LF_LOADARM9				0x00000001
#define LF_LOADARM7				0x00000002
#define LF_CLEARMEM				0x00000004
#define LF_LOADGBFS				0x00000008
#define LF_MEMDIRECT		  	        0x00000010
#define LF_NOVBLANK				0x00000020
#define LF_GBABOOT				0x00000020

#define EXTERNAL_DATA(name) \
  extern const uint8 name[]; \
  extern const uint8 name##_end[]; \
  extern const uint32 name##_size
EXTERNAL_DATA(exec_stub_bin);
EXTERNAL_DATA(load_bin);

#define VERBOSE

int exec(const char *command)
{
  // "..." ignored atm
  // Set up VRAM, to put the binary in. 
  //  Bank A might be used for text out (will be used by the stub)
  //  Bank B,D to carry over param&binary
  vramSetMainBanks(VRAM_A_MAIN_BG_0x06000000, VRAM_B_MAIN_BG_0x06020000,      
		   VRAM_C_LCD, VRAM_D_MAIN_BG_0x06040000);    
						
  //set the video mode
  videoSetMode(MODE_0_2D | DISPLAY_BG0_ACTIVE);
  lcdSwap();
  
  // black backdrop
  BG_PALETTE[0]=RGB15(0,0,0);
  BG0_CR = BG_MAP_BASE(31);
  BG_PALETTE[255] = RGB15(31,31,31);
  
  // set up debug output 
  //consoleInitDefault((u16*)SCREEN_BASE_BLOCK(31), (u16*)CHAR_BASE_BLOCK(0), 16);
  
  consoleDemoInit();
  
#ifdef VERBOSE
  iprintf("Loading exec() stub...\n") ;
#endif
  
  // you can load the stub from whatever place: gbfs,wifi,fat & whatever
  //long *buffer = (long *)malloc(256*1024);
  //FAT_FILE *stub = FAT_fopen("exec_stub.bin","r") ;
  //FAT_fread(buffer,256*1024,1,stub) ;
  //FAT_fclose(stub) ;

  //  long *buffer = (long*)exec_stub_bin;
  long *buffer = (long*)load_bin;
  
#ifdef VERBOSE
  iprintf("copying over...\n") ;
#endif

  // we copy wordwise, as bytewise write access to VRAM is a bad idea
  int i ;
  long *target = (long *)0x6020000 ;
  for (i=0;i<64*1024;i++) {
    target[i] = buffer[i] ;
  } ;
  
#ifdef VERBOSE
  iprintf("Setting filename...\n") ;
#endif
  char *exec_filename = (char *)0x0605FC00 ;
  // don't use str* functions as we can't write bytewise to VRAM
  for (i=0;i<=(strlen(command)+1)/4;i++)
    {
      ((vu32 *)exec_filename)[i] = ((vu32 *)command)[i] ;
    }

  *(vu32 *)0x0605FFFC = 0x0605FC00 ;
  *(unsigned long *)0x0605FFF8 = (LF_LOADARM9 |
				  LF_LOADARM7 |
				  LF_CLEARMEM |
				  LF_LOADGBFS |
				  LF_NOVBLANK);

#ifdef VERBOSE
  iprintf("giving control to stub...\n") ;
#endif
  
  typedef void (*stub_type)(void) ;
  
  stub_type stb = (stub_type)0x06020000 ;

  //  while(1) swiWaitForVBlank();
  
  stb() ;

  return 0;
}

