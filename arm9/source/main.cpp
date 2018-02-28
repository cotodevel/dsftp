/*---------------------------------------------------------------------------------
	$Id: template.c,v 1.4 2005/09/17 23:15:13 wntrmute Exp $

	Basic Hello World

	$Log: template.c,v $
	Revision 1.4  2005/09/17 23:15:13  wntrmute
	corrected iprintAt in templates
	
	Revision 1.3  2005/09/05 00:32:20  wntrmute
	removed references to IPC struct
	replaced with API functions
	
	Revision 1.2  2005/08/31 01:24:21  wntrmute
	updated for new stdio support

	Revision 1.1  2005/08/03 06:29:56  wntrmute
	added templates


---------------------------------------------------------------------------------*/

#include "nds.h"
#include <nds/arm9/console.h> //basic print funcionality
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netdb.h>

#include "BController.h"

extern "C" {
  
// Dswifi stub functions
void *sgIP_malloc(int size) { return malloc(size); }
void sgIP_free(void * ptr) { free(ptr); }

}

int main(void)
{
  BController::get()->run();

  return 0;
}
