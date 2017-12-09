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

#include <ch.h>

/** @cond never*/
ReadyList rlist;
/** @endcond */

/**
 * Scheduler initialization.
 * @note Internally invoked by the \p chSysInit().
 */
void chSchInit(void) {

  fifo_init(&rlist.r_queue);
  rlist.r_prio = NOPRIO;
  rlist.r_preempt = CH_TIME_QUANTUM;
#ifdef CH_USE_SYSTEMTIME
  rlist.r_stime = 0;
#endif
}

/**
 * Inserts a thread in the Ready List.
 * @param tp the Thread to be made ready
 * @param msg message to the awakened thread
 * @return the Thread pointer
 * @note The function must be called in the system mutex zone.
 * @note The function does not reschedule, the \p chSchRescheduleS() should
 *       be called soon after.
 * @note The function is not meant to be used in the user code directly.
 */
#ifdef CH_OPTIMIZE_SPEED
/* NOTE: it is inlined in this module only.*/
INLINE void chSchReadyI(Thread *tp, msg_t msg) {
#else
void chSchReadyI(Thread *tp, msg_t msg) {
#endif
  Thread *cp = rlist.r_queue.p_next;

  tp->p_state = PRREADY;
  tp->p_rdymsg = msg;
  while (cp->p_prio >= tp->p_prio)
    cp = cp->p_next;
  /* Insertion on p_prev.*/
  tp->p_prev = (tp->p_next = cp)->p_prev;
  tp->p_prev->p_next = cp->p_prev = tp;
}

/**
 * Puts the current thread to sleep into the specified state, the next highest
 * priority thread becomes running. The threads states are described into
 * \p threads.h
 * @param newstate the new thread state
 * @return the wakeup message
 * @note The function must be called in the system mutex zone.
 * @note The function is not meant to be used in the user code directly.
 */
void chSchGoSleepS(tstate_t newstate) {
  Thread *otp;

  (otp = currp)->p_state = newstate;
  (currp = fifo_remove(&rlist.r_queue))->p_state = PRCURR;
  rlist.r_preempt = CH_TIME_QUANTUM;
#ifdef CH_USE_TRACE
  chDbgTrace(otp, currp);
#endif
  chSysSwitchI(otp, currp);
}

#ifdef CH_USE_VIRTUAL_TIMERS
static void wakeup(void *p) {

  if (((Thread *)p)->p_state == PRWTSEM)
    chSemFastSignalI(((Thread *)p)->p_wtsemp);
  chSchReadyI(p, RDY_TIMEOUT);
}

/**
 * Puts the current thread to sleep into the specified state, the next highest
 * priority thread becomes running. The thread is automatically awakened after
 * the specified time elapsed.
 * @param newstate the new thread state
 * @param time the number of ticks before the operation timouts
 * @return the wakeup message, it is \p RDY_TIMEOUT if a timeout occurs
 * @note The function must be called in the system mutex zone.
 * @note The function is not meant to be used in the user code directly.
 */
msg_t chSchGoSleepTimeoutS(tstate_t newstate, systime_t time) {
  VirtualTimer vt;

  chVTSetI(&vt, time, wakeup, currp);
  chSchGoSleepS(newstate);
  if (chVTIsArmedI(&vt))
    chVTResetI(&vt);
  return currp->p_rdymsg;
}
#endif /* CH_USE_VIRTUAL_TIMERS */

/**
 * Wakeups a thread, the thread is inserted into the ready list or made
 * running directly depending on its relative priority compared to the current
 * thread.
 * @param ntp the Thread to be made ready
 * @param msg message to the awakened thread
 * @note The function must be called in the system mutex zone.
 * @note The function is not meant to be used in the user code directly.
 * @note It is equivalent to a \p chSchReadyI() followed by a
 *       \p chSchRescheduleS() but much more efficient.
 */
void chSchWakeupS(Thread *ntp, msg_t msg) {

  if (ntp->p_prio <= currp->p_prio)
    chSchReadyI(ntp, msg);
  else {
    Thread *otp = currp;
    chSchReadyI(otp, RDY_OK);
    (currp = ntp)->p_state = PRCURR;
    ntp->p_rdymsg = msg;
    rlist.r_preempt = CH_TIME_QUANTUM;
#ifdef CH_USE_TRACE
    chDbgTrace(otp, ntp);
#endif
    chSysSwitchI(otp, ntp);
  }
}

/**
 * Performs a reschedulation. It is meant to be called if
 * \p chSchRescRequired() evaluates to \p TRUE.
 */
void chSchDoRescheduleI(void) {
  Thread *otp = currp;

  chSchReadyI(otp, RDY_OK);
  (currp = fifo_remove(&rlist.r_queue))->p_state = PRCURR;
  rlist.r_preempt = CH_TIME_QUANTUM;
#ifdef CH_USE_TRACE
  chDbgTrace(otp, currp);
#endif
  chSysSwitchI(otp, currp);
}

/**
 * If a thread with an higher priority than the current thread is in the
 * ready list then it becomes running.
 * @note The function must be called in the system mutex zone.
 */
void chSchRescheduleS(void) {

  if (isempty(&rlist.r_queue) || firstprio(&rlist.r_queue) <= currp->p_prio)
    return;

  chSchDoRescheduleI();
}

/**
 * Evaluates if a reschedulation is required.
 * @return \p TRUE if there is a thread that should go in running state
 *         immediatly else \p FALSE.
 */
bool_t chSchRescRequiredI(void) {

  if (isempty(&rlist.r_queue))
    return FALSE;

  if (rlist.r_preempt) {
    if (firstprio(&rlist.r_queue) <= currp->p_prio)
      return FALSE;
  }
  else { /* Time quantum elapsed. */
    if (firstprio(&rlist.r_queue) < currp->p_prio)
      return FALSE;
  }
  return TRUE;
}

/** @} */