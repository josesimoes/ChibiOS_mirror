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
#include <test.h>

#include "board.h"
#include <sam7x_serial.h>

static WorkingArea(waThread1, 64);
static msg_t Thread1(void *arg) {

  while (TRUE) {
    AT91C_BASE_PIOB->PIO_SODR = PIOB_LCD_BL;            // LCD on.
    chThdSleep(100);
    AT91C_BASE_PIOB->PIO_CODR = PIOB_LCD_BL;            // LCD off.
    chThdSleep(900);
  }
  return 0;
}

/*
 * Entry point, note, the main() function is already a thread in the system
 * on entry.
 */
int main(int argc, char **argv) {

  /*
   * Creates the blinker thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

  /*
   * Normal main() thread activity.
   */
  while (TRUE) {
    chThdSleep(500);
    if (!(AT91C_BASE_PIOB->PIO_PDSR & PIOB_SW1))
      chFDDWrite(&COM1, (uint8_t *)"Hello World!\r\n", 14);
    if (!(AT91C_BASE_PIOB->PIO_PDSR & PIOB_SW2))
      TestThread(&COM1);
  }

  return 0;
}