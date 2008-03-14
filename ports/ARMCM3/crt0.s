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

/*
 * Generic ARM-CortexM3 startup file for ChibiOS/RT.
 */

.set    CONTROL_MODE_PRIVILEGED, 0
.set    CONTROL_MODE_UNPRIVILEGED, 1
.set    CONTROL_USE_MSP, 0
.set    CONTROL_USE_PSP, 0

.text
.balign 2
.syntax unified

/*
 * Reset handler.
 */
.global ResetHandler
ResetHandler:
        /*
         * Stack pointers initialization.
         */
        ldr     r0, =__ram_end__
        ldr     r1, =__main_stack_size__
        sub     r0, r0, r1
        msr     PSP, r0
//        ldr     r1, =__process_stack_size__
//        sub     r0, r0, r1
        /*
         * Data initialization.
         * NOTE: It assumes that the DATA size is a multiple of 4.
         */
        ldr     r1, =_textdata
        ldr     r2, =_data
        ldr     r3, =_edata
dloop:
        cmp     r2, r3
        ittt    lo
        ldrlo   r0, [r1], #4
        strlo   r0, [r2], #4
        blo     dloop
        /*
         * BSS initialization.
         * NOTE: It assumes that the BSS size is a multiple of 4.
         */
        mov     r0, #0
        ldr     r1, =_bss_start
        ldr     r2, =_bss_end
bloop:
        cmp     r1, r2
        itt     lo
        strlo   r0, [r1], #4
        blo     bloop
        /*
         * Switches to the Process Stack and disables the interrupts globally.
         */
        mov     r0, #CONTROL_MODE_PRIVILEGED | CONTROL_USE_PSP
        msr     CONTROL, r0
        cpsid   i
        /*
         * Application-provided HW initialization routine.
         */
        bl      hwinit
        /*
         * main(0, NULL).
         */
        mov     r0, #0
        mov     r1, r0
        bl      main
        bl      chSysHalt
