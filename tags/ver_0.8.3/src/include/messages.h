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
 * @addtogroup Messages
 * @{
 */

#ifndef _MESSAGES_H_
#define _MESSAGES_H_

#ifdef CH_USE_MESSAGES

/**
 * Evaluates to TRUE if the thread has pending messages.
 */
#define chMsgIsPendingI(tp) \
        ((tp)->p_msgqueue.p_next != (Thread *)&(tp)->p_msgqueue)

/**
 * Returns the first message in the queue.
 */
#define chMsgGetI(tp) \
        ((tp)->p_msgqueue.p_next->p_msg)

#ifdef __cplusplus
extern "C" {
#endif
  msg_t chMsgSend(Thread *tp, msg_t msg);
  msg_t chMsgWait(void);
  msg_t chMsgGet(void);
  void chMsgRelease(msg_t msg);

#ifdef CH_USE_MESSAGES_EVENT
  msg_t chMsgSendWithEvent(Thread *tp, msg_t msg, EventSource *esp);
#endif
#ifdef __cplusplus
}
#endif

#endif /* CH_USE_MESSAGES */

#endif /* _MESSAGES_H_ */

/** @} */