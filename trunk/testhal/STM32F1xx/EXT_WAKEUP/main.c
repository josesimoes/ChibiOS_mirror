/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011 Giovanni Di Sirio.

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
#include "ch.h"
#include "hal.h"

#include "shell.h"
#include "chprintf.h"



/* Wake up callback.*/
static void extcb2(EXTDriver *extp, expchannel_t channel) {

  (void)extp;
  (void)channel;

  chSysLockFromIsr();
  /* we MUST reinit clocks after waking up if use HSE or HSI+PLL */
  stm32_clock_init();

  extChannelDisableI(&EXTD1, 10);
  chSysUnlockFromIsr();
}


static const EXTConfig extcfg = {
  {
   {EXT_CH_MODE_DISABLED, NULL},
   {EXT_CH_MODE_DISABLED, NULL},
   {EXT_CH_MODE_DISABLED, NULL},
   {EXT_CH_MODE_DISABLED, NULL},
   {EXT_CH_MODE_DISABLED, NULL},
   {EXT_CH_MODE_DISABLED, NULL},
   {EXT_CH_MODE_DISABLED, NULL},
   {EXT_CH_MODE_DISABLED, NULL},
   {EXT_CH_MODE_DISABLED, NULL},
   {EXT_CH_MODE_DISABLED, NULL},
   {EXT_CH_MODE_FALLING_EDGE, extcb2},
   {EXT_CH_MODE_DISABLED, NULL},
   {EXT_CH_MODE_DISABLED, NULL},
   {EXT_CH_MODE_DISABLED, NULL},
   {EXT_CH_MODE_DISABLED, NULL},
   {EXT_CH_MODE_DISABLED, NULL},
  },
  EXT_MODE_EXTI(0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                EXT_MODE_GPIOA,
                0,
                0,
                0,
                0,
                0)
};



static void cmd_reboot(BaseChannel *chp, int argc, char *argv[]){
  (void)argv;
  if (argc > 0) {
    chprintf(chp, "Usage: rboot\r\n");
    return;
  }
  chprintf(chp, "rebooting...\r\n");
  chThdSleepMilliseconds(100); // time to print out message in terminal
  NVIC_SystemReset();
}

static void cmd_sleep(BaseChannel *chp, int argc, char *argv[]){
  (void)argv;
  if (argc > 0) {
    chprintf(chp, "Usage: sleep\r\n");
    return;
  }
  chprintf(chp, "Going to sleep. Type any character to wake up.\r\n");

  chThdSleepMilliseconds(200); // time to print out message in terminal
  extChannelEnable(&EXTD1, 10);

  PWR->CR |= (PWR_CR_LPDS | PWR_CR_CSBF | PWR_CR_CWUF);
  PWR->CR &= ~PWR_CR_PDDS; // explicit clear PDDS, just to be safe
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
  __WFI();
}


static const ShellCommand commands[] = {
  {"reboot", cmd_reboot},
  {"sleep", cmd_sleep},
  {NULL, NULL}
};

static const ShellConfig shell_cfg1 = {
  (BaseChannel *)&SD1,
  commands
};

/*
 * Application entry point.
 */
int main(void) {

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  /*
   * Activates the EXT driver 1.
   */
  extStart(&EXTD1, &extcfg);

  /* Activates the serial driver using the driver default configuration. */
  sdStart(&SD1, NULL);

  /* Shell manager initialization. */
  shellInit();
  palSetPadMode(IOPORT1, 9, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
  palSetPadMode(IOPORT1, 10, PAL_MODE_INPUT);

  static WORKING_AREA(waShell, 512);
  shellCreateStatic(&shell_cfg1, waShell, sizeof(waShell), NORMALPRIO);

  /*
   * Normal main() thread activity, in this demo it enables and disables the
   * button EXT channel using 5 seconds intervals.
   */
  
  chThdSleepMilliseconds(2000); // timeuot to differ reboot and wake up from sleep
  while (TRUE) {
    chThdSleepMilliseconds(100);
    palTogglePad(IOPORT3, GPIOC_LED);
    chThdExit(0);
  }
}
