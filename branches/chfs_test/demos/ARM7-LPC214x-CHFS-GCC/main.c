/*
    ChibiOS/RT - Copyright (C) 2006-2007 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <ch.h>
#include <chfs.h>

#include <dev.h>
#include <buf.h>
#include <mmc.h>

#include <test.h>

#include "lpc214x.h"
#include "lpc214x_serial.h"
#include "buzzer.h"
#include "evtimer.h"

/*
 * Red LEDs blinker thread, times are in milliseconds.
 */
/*static WorkingArea(waThread1, 64);
static msg_t Thread1(void *arg) {

  while (TRUE) {
    IO0CLR = 0x00000800;
    chThdSleep(200);
    IO0SET = 0x00000C00;
    chThdSleep(800);
    IO0CLR = 0x00000400;
    chThdSleep(200);
    IO0SET = 0x00000C00;
    chThdSleep(800);
  }
  return 0;
}*/

/*
 * Yellow LED blinker thread, times are in milliseconds.
 */
/*static WorkingArea(waThread2, 64);
static msg_t Thread2(void *arg) {

  while (TRUE) {
    IO0CLR = 0x80000000;
    chThdSleep(200);
    IO0SET = 0x80000000;
    chThdSleep(300);
  }
  return 0;
}*/

/*
 * Executed as event handler at 500mS intervals.
 */
static void TimerHandler(eventid_t id) {

  if (!(IO0PIN & 0x00018000)) {         // Both buttons
    TestThread(&COM1);
    PlaySound(500, 100);
  }
  else {
    if (!(IO0PIN & 0x00008000)) {       // Button 1
      PlaySound(1000, 100);
      PlaySound(1500, 100);
    }
    if (!(IO0PIN & 0x00010000)) {       // Button 2
      FileSystem fs;

      chFSMount(&fs, NULL, DEVICE(0, 1), FALSE, FS_TYPE_FAT);

      chFSUnmount(&fs);
    }
  }
}

/*
 * Entry point, the interrupts are disabled on entry.
 */
int main(int argc, char **argv) {
  static const evhandler_t evhndl[] = {
    TimerHandler
  };
  static EvTimer evt;
  struct EventListener el0;

  /*
   * The main() function becomes a thread here then the interrupts are
   * enabled and ChibiOS/RT goes live.
   */
  chSysInit();

  /*
   * File System initialization.
   */
  chFSInit();

  /*
   * If a button is pressed during the reset then the blinking leds threads
   * are not started in order to make accurate benchmarks.
   */
  if ((IO0PIN & 0x00018000) == 0x00018000) {
//    chThdCreate(NORMALPRIO, 0, waThread1, sizeof(waThread1), Thread1, NULL);
//    chThdCreate(NORMALPRIO, 0, waThread2, sizeof(waThread2), Thread2, NULL);
  }

  /*
   * Normal main() activity, in this demo it serves events generated by
   * various sources.
   */
  evtInit(&evt, 500);                   /* Initializes an event timer object.   */
  evtStart(&evt);                       /* Starts the event timer.              */
  chEvtRegister(&evt.et_es, &el0, 0);   /* Registers on the timer event source. */
  while (TRUE)                          /* Just serve events.                   */
    chEvtWait(ALL_EVENTS, evhndl);
  return 0;
}