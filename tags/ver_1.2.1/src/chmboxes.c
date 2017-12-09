/*
    ChibiOS/RT - Copyright (C) 2009 Giovanni Di Sirio.

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

                                      ---

    A special exception to the GPL can be applied should you wish to distribute
    a combined work that includes ChibiOS/RT, without being obliged to provide
    the source code for any proprietary components. See the file exception.txt
    for full details of how and when the exception can be applied.
*/

/**
 * @file chmboxes.c
 * @brief Mailboxes code.
 * @addtogroup Mailboxes
 * @{
 */

#include <ch.h>

#if CH_USE_MAILBOXES && CH_USE_SEMAPHORES_TIMEOUT
/**
 * @brief Initializes a Mailbox object.
 *
 * @param[out] mbp the pointer to the Mailbox structure to be initialized
 * @param[in] buf the circular messages buffer
 * @param[in] n the buffer size as number of @p msg_t
 */
void chMBInit(Mailbox *mbp, msg_t *buf, cnt_t n) {

  chDbgCheck((mbp != NULL) && (buf != NULL) && (n > 0), "chMBInit");

  mbp->mb_buffer = mbp->mb_wrptr = mbp->mb_rdptr = buf;
  mbp->mb_top = &buf[n];
  chSemInit(&mbp->mb_emptysem, n);
  chSemInit(&mbp->mb_fullsem, 0);
}

/**
 * @brief Resets a Mailbox object.
 * @details All the waiting threads are resumed with status @p RDY_RESET and
 *          the queued messages are lost.
 *
 * @param[in] mbp the pointer to an initialized Mailbox object
 */
void chMBReset(Mailbox *mbp) {

  chDbgCheck(mbp != NULL, "chMBReset");

  chSysLock();
  mbp->mb_wrptr = mbp->mb_rdptr = mbp->mb_buffer;
  chSemResetI(&mbp->mb_emptysem, mbp->mb_top - mbp->mb_buffer);
  chSemResetI(&mbp->mb_fullsem, 0);
  chSchRescheduleS();
  chSysUnlock();
}

/**
 * @brief Posts a message into a mailbox.
 * @details The invoking thread waits until a empty slot in the mailbox becomes
 *          available or the specified time runs out.
 *
 * @param[in] mbp the pointer to an initialized Mailbox object
 * @param[in] msg the message to be posted on the mailbox
 * @param[in] time the number of ticks before the operation timeouts,
 *                 the following special values are allowed:
 *                 - @a TIME_IMMEDIATE immediate timeout.
 *                 - @a TIME_INFINITE no timeout.
 *                 .
 * @return The operation status.
 * @retval RDY_OK if the message was correctly posted.
 * @retval RDY_RESET if the mailbox was reset while waiting.
 * @retval RDY_TIMEOUT if the operation timed out.
 */
msg_t chMBPost(Mailbox *mbp, msg_t msg, systime_t time) {
  msg_t rdymsg;

  chSysLock();
  rdymsg = chMBPostS(mbp, msg, time);
  chSysUnlock();
  return rdymsg;
}

/**
 * @brief Posts a message into a mailbox.
 * @details The invoking thread waits until a empty slot in the mailbox becomes
 *          available or the specified time runs out.
 *
 * @param[in] mbp the pointer to an initialized Mailbox object
 * @param[in] msg the message to be posted on the mailbox
 * @param[in] time the number of ticks before the operation timeouts,
 *                 the following special values are allowed:
 *                 - @a TIME_IMMEDIATE immediate timeout.
 *                 - @a TIME_INFINITE no timeout.
 *                 .
 * @return The operation status.
 * @retval RDY_OK if the message was correctly posted.
 * @retval RDY_RESET if the mailbox was reset while waiting.
 * @retval RDY_TIMEOUT if the operation timed out.
 */
msg_t chMBPostS(Mailbox *mbp, msg_t msg, systime_t time) {
  msg_t rdymsg;

  chDbgCheck(mbp != NULL, "chMBPostS");

  rdymsg = chSemWaitTimeoutS(&mbp->mb_emptysem, time);
  if (rdymsg == RDY_OK) {
    *mbp->mb_wrptr++ = msg;
    if (mbp->mb_wrptr >= mbp->mb_top)
      mbp->mb_wrptr = mbp->mb_buffer;
    chSemSignalI(&mbp->mb_fullsem);
    chSchRescheduleS();
  }
  return rdymsg;
}

/**
 * @brief Posts an high priority message into a mailbox.
 * @details The invoking thread waits until a empty slot in the mailbox becomes
 *          available or the specified time runs out.
 *
 * @param[in] mbp the pointer to an initialized Mailbox object
 * @param[in] msg the message to be posted on the mailbox
 * @param[in] time the number of ticks before the operation timeouts,
 *                 the following special values are allowed:
 *                 - @a TIME_IMMEDIATE immediate timeout.
 *                 - @a TIME_INFINITE no timeout.
 *                 .
 * @return The operation status.
 * @retval RDY_OK if the message was correctly posted.
 * @retval RDY_RESET if the mailbox was reset while waiting.
 * @retval RDY_TIMEOUT if the operation timed out.
 */
msg_t chMBPostAhead(Mailbox *mbp, msg_t msg, systime_t time) {
  msg_t rdymsg;

  chSysLock();
  rdymsg = chMBPostAheadS(mbp, msg, time);
  chSysUnlock();
  return rdymsg;
}

/**
 * @brief Posts an high priority message into a mailbox.
 * @details The invoking thread waits until a empty slot in the mailbox becomes
 *          available or the specified time runs out.
 *
 * @param[in] mbp the pointer to an initialized Mailbox object
 * @param[in] msg the message to be posted on the mailbox
 * @param[in] time the number of ticks before the operation timeouts,
 *                 the following special values are allowed:
 *                 - @a TIME_IMMEDIATE immediate timeout.
 *                 - @a TIME_INFINITE no timeout.
 *                 .
 * @return The operation status.
 * @retval RDY_OK if the message was correctly posted.
 * @retval RDY_RESET if the mailbox was reset while waiting.
 * @retval RDY_TIMEOUT if the operation timed out.
 */
msg_t chMBPostAheadS(Mailbox *mbp, msg_t msg, systime_t time) {
  msg_t rdymsg;

  chDbgCheck(mbp != NULL, "chMBPostAheadS");

  rdymsg = chSemWaitTimeoutS(&mbp->mb_emptysem, time);
  if (rdymsg == RDY_OK) {
    if (--mbp->mb_rdptr < mbp->mb_buffer)
      mbp->mb_rdptr = mbp->mb_top - 1;
    *mbp->mb_rdptr = msg;
    chSemSignalI(&mbp->mb_fullsem);
    chSchRescheduleS();
  }
  return rdymsg;
}

/**
 * @brief Retrieves a message from a mailbox.
 * @details The invoking thread waits until a message is posted in the mailbox
 *          or the specified time runs out.
 *
 * @param[in] mbp the pointer to an initialized Mailbox object
 * @param[out] msgp pointer to a message variable for the received message
 * @param[in] time the number of ticks before the operation timeouts,
 *                 the following special values are allowed:
 *                 - @a TIME_IMMEDIATE immediate timeout.
 *                 - @a TIME_INFINITE no timeout.
 *                 .
 * @return The operation status.
 * @retval RDY_OK if a message was correctly fetched.
 * @retval RDY_RESET if the mailbox was reset while waiting.
 * @retval RDY_TIMEOUT if the operation timed out.
 */
msg_t chMBFetch(Mailbox *mbp, msg_t *msgp, systime_t time) {
  msg_t rdymsg;

  chSysLock();
  rdymsg = chMBFetchS(mbp, msgp, time);
  chSysUnlock();
  return rdymsg;
}

/**
 * @brief Retrieves a message from a mailbox.
 * @details The invoking thread waits until a message is posted in the mailbox
 *          or the specified time runs out.
 *
 * @param[in] mbp the pointer to an initialized Mailbox object
 * @param[out] msgp pointer to a message variable for the received message
 * @param[in] time the number of ticks before the operation timeouts,
 *                 the following special values are allowed:
 *                 - @a TIME_IMMEDIATE immediate timeout.
 *                 - @a TIME_INFINITE no timeout.
 *                 .
 * @return The operation status.
 * @retval RDY_OK if a message was correctly fetched.
 * @retval RDY_RESET if the mailbox was reset while waiting.
 * @retval RDY_TIMEOUT if the operation timed out.
 */
msg_t chMBFetchS(Mailbox *mbp, msg_t *msgp, systime_t time) {
  msg_t rdymsg;

  chDbgCheck((mbp != NULL) && (msgp != NULL), "chMBFetchS");

  rdymsg = chSemWaitTimeoutS(&mbp->mb_fullsem, time);
  if (rdymsg == RDY_OK) {
    *msgp = *mbp->mb_rdptr++;
    if (mbp->mb_rdptr >= mbp->mb_top)
      mbp->mb_rdptr = mbp->mb_buffer;
    chSemSignalI(&mbp->mb_emptysem);
    chSchRescheduleS();
  }
  return rdymsg;
}
#endif /* CH_USE_MAILBOXES && CH_USE_SEMAPHORES_TIMEOUT */

/** @} */