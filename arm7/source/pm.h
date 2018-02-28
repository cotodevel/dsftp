/* This is -*- C -*-)

   pm.h   
   \author: Bjoern Giesler <bjoern@giesler.de>
   
   
   
   $Author: giesler $
   $Locker$
   $Revision$
   $Date: 2002-08-19 10:41:28 +0200 (Mon, 19 Aug 2002) $
 */

#ifndef PM_H
#define PM_H

/* system includes */
#include <nds.h>

/* my includes */
#define 	PM_CONTROL_REG   0
#define 	PM_BATTERY_REG   1
#define 	PM_AMPLIFIER_REG   2
#define 	PM_READ_REGISTER   (1<<7)
#define 	PM_SOUND_AMP   BIT(0)
#define 	PM_SOUND_MUTE   BIT(1)
#define 	PM_BACKLIGHT_BOTTOM   BIT(2)
#define 	PM_BACKLIGHT_TOP   BIT(3)
#define 	PM_SYSTEM_PWR   BIT(6)
#define 	PM_POWER_DOWN   BIT(6)
#define 	PM_LED_CONTROL(m)   ((m)<<4)
#define 	PM_LED_ON   (0<<4)
#define 	PM_LED_SLEEP   (1<<4)
#define 	PM_LED_BLINK   (3<<4)
#define 	PM_AMP_OFFSET   2
#define 	PM_AMP_ON   1
#define 	PM_AMP_OFF   0

void pmSwitchBacklight(bool top, bool bottom);
void pmGetBacklight(bool* top, bool* bottom);

u8 pmGetBacklightBrightness();
void pmSetBacklightBrightness(u8 brightness);

void pmPowerOff();

#endif /* PM_H */
