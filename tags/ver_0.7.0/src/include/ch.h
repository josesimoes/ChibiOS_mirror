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

/**
 * @addtogroup Initialization
 * @{
 */

#ifndef _CH_H_
#define _CH_H_

#define _CHIBIOS_RT_

#include <chconf.h>
#include <chtypes.h>
#include "lists.h"
#include <chcore.h>
#include "delta.h"
#include "scheduler.h"
#include "semaphores.h"
#include "mutexes.h"
#include "events.h"
#include "messages.h"
#include "threads.h"
#include "inline.h"
#include "sleep.h"
#include "queues.h"
#include "serial.h"
#include "heap.h"
#include "mempools.h"
#include "debug.h"

/*
 * Common values.
 */
#ifndef FALSE
#define FALSE       0
#endif
#ifndef TRUE
#define TRUE        (!FALSE)
#endif

#ifdef __cplusplus
extern "C" {
#endif
  void chSysInit(void);
  void chSysTimerHandlerI(void);
#ifdef __cplusplus
}
#endif

#endif /* _CH_H_ */

/** @} */