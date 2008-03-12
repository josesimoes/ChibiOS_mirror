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
 * @addtogroup Scheduler
 * @{
 */

#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

/** Normal \p chSchReadyI() message.*/
#define RDY_OK      0
/** Returned if the thread was made ready because a timeout.*/
#define RDY_TIMEOUT -1
/** Returned if the thread was made ready because a reset.*/
#define RDY_RESET   -2

#define firstprio(rlp)   ((rlp)->p_next->p_prio)

/**
 * Ready list header.
 */
typedef struct {
  ThreadsQueue          r_queue;
  tprio_t               r_prio;
  cnt_t                 r_preempt;
#ifndef CH_CURRP_REGISTER_CACHE
  Thread                *r_current;
#endif
#ifdef CH_USE_SYSTEMTIME
  volatile systime_t    r_stime;
#endif
} ReadyList;

extern ReadyList rlist;

/*
 * Scheduler APIs.
 */
#ifdef __cplusplus
extern "C" {
#endif
  void chSchInit(void);
  Thread *chSchReadyI(Thread *tp);
  void chSchGoSleepS(tstate_t newstate);
  msg_t chSchGoSleepTimeoutS(tstate_t newstate, systime_t time);
  void chSchWakeupS(Thread *tp, msg_t msg);
  void chSchDoRescheduleI(void);
  void chSchRescheduleS(void);
  bool_t chSchRescRequiredI(void);
#ifdef __cplusplus
}
#endif

#ifdef CH_CURRP_REGISTER_CACHE
register Thread *currp asm(CH_CURRP_REGISTER_CACHE);
#else
#define currp rlist.r_current
#endif

#endif /* _SCHEDULER_H_ */

/** @} */
