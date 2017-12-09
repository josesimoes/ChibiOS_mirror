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

#ifndef _BOARD_H_
#define _BOARD_H_

/*
 * Setup for the Eurotek 356FX_TFT43 proto board.
 */

/*
 * Board identifier.
 */
#define BOARD_DEMO_348FX_GEN

/*
 * Clock constants.
 */
#define CRYSTAL_FREQ    4000000.0           // quarzo da 4M
#define CRYSTAL_SUBFRQ  32768.0             // quarzo subclock da 32768Hz

#define SYSTEM_FREQ    	48000000.0          // frequenza di sistema con PLL=6x
#define CLKP1_FREQ    	16000000.0          // frequenza periferal clock CLKP1
#define CLKP2_FREQ    	16000000.0          // frequenza periferal clock CLKP2 -> usato per CAN & soundgen


/*
 * Pin definitions for the board.
 */

#define	PinLED_LIFE		PDR09_P3
#define	PinLED_SLOW_PLC PDR09_P1
#define	PinLED_FAST_PLC PDR09_P2
#define	PinLED_WDOG 	PDR09_P7


/*
 * Initial I/O ports settings.
 */


#ifdef __cplusplus
extern "C" {
#endif
  void hwinit(void);
#ifdef __cplusplus
}
#endif

#endif /* _BOARD_H_ */