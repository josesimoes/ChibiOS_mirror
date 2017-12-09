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

#include "board.h"
#include "avr_serial.h"

CH_IRQ_HANDLER(TIMER0_COMP_vect) {

  CH_IRQ_PROLOGUE();

  chSysLockFromIsr();
  chSysTimerHandlerI();
  chSysUnlockFromIsr();

  CH_IRQ_EPILOGUE();
}

/*
 * Board initialization code.
 */
void hwinit(void) {

  /*
   * I/O ports setup.
   */
  DDRA   = VAL_DDRA;
  PORTA  = VAL_PORTA;
  DDRB   = VAL_DDRB;
  PORTB  = VAL_PORTB;
  DDRC   = VAL_DDRC;
  PORTC  = VAL_PORTC;
  DDRD   = VAL_DDRD;
  PORTD  = VAL_PORTD;
  DDRE   = VAL_DDRE;
  PORTE  = VAL_PORTE;
  DDRF   = VAL_DDRF;
  PORTF  = VAL_PORTF;
  DDRG   = VAL_DDRG;
  PORTG  = VAL_PORTG;

  /*
   * External interrupts setup, all disabled initially.
   */
  EICRA  = 0x00;
  EICRB  = 0x00;
  EIMSK  = 0x00;

  /*
   * Enables Idle mode for SLEEP instruction.
   */
  MCUCR  = (1 << SE);

  /*
   * Timer 0 setup.
   */
  TCCR0  = (1 << WGM01) | (0 << WGM00) |                // CTC mode.
           (0 << COM01) | (0 << COM00) |                // OC0A disabled (normal I/O).
           (1 << CS02)  | (0 << CS01)  | (0 << CS00);   // CLK/64 clock source.
  OCR0   = F_CPU / 64 / CH_FREQUENCY - 1;
  TCNT0  = 0;                                           // Reset counter.
  TIFR   = (1 << OCF0);                                 // Reset pending (if any).
  TIMSK  = (1 << OCIE0);                                // Interrupt on compare.

  /*
   * Other initializations.
   */
  serial_init();
}