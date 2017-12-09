/*
    ChibiOS/RT - Copyright (C) 2010 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.

                                      ---

    A special exception to the GPL can be applied should you wish to distribute
    a combined work that includes ChibiOS/RT, without being obliged to provide
    the source code for any proprietary components. See the file exception.txt
    for full details of how and when the exception can be applied.
*/

#include "ch.h"
#include "test.h"

/**
 * @page test_sem Semaphores test
 *
 * <h2>Description</h2>
 * This module implements the test sequence for the @ref semaphores subsystem.
 *
 * <h2>Objective</h2>
 * Objective of the test module is to cover 100% of the @ref semaphores code.
 *
 * <h2>Preconditions</h2>
 * The module requires the following kernel options:
 * - @p CH_USE_SEMAPHORES
 * .
 * In case some of the required options are not enabled then some or all tests
 * may be skipped.
 *
 * <h2>Test Cases</h2>
 * - @subpage test_sem_001
 * - @subpage test_sem_002
 * - @subpage test_sem_003
 * .
 * @file testsem.c
 * @brief Semaphores test source file
 * @file testsem.h
 * @brief Semaphores test header file
 */

#if CH_USE_SEMAPHORES

#define ALLOWED_DELAY MS2ST(5)

/*
 * Note, the static initializers are not really required because the
 * variables are explicitly initialized in each test case. It is done in order
 * to test the macros.
 */
static SEMAPHORE_DECL(sem1, 0);

/**
 * @page test_sem_001 Enqueuing test
 *
 * <h2>Description</h2>
 * Five threads with randomized priorities are enqueued to a semaphore then
 * awakened one at time.<br>
 * The test expects that the threads reach their goal in FIFO order or
 * priority order depending on the CH_USE_SEMAPHORES_PRIORITY configuration
 * setting.
 */
static char *sem1_gettest(void) {

  return "Semaphores, enqueuing";
}

static void sem1_setup(void) {

  chSemInit(&sem1, 0);
}

static msg_t thread1(void *p) {

  chSemWait(&sem1);
  test_emit_token(*(char *)p);
  return 0;
}

static void sem1_execute(void) {

  threads[0] = chThdCreateStatic(wa[0], WA_SIZE, chThdGetPriority()+5, thread1, "A");
  threads[1] = chThdCreateStatic(wa[1], WA_SIZE, chThdGetPriority()+1, thread1, "B");
  threads[2] = chThdCreateStatic(wa[2], WA_SIZE, chThdGetPriority()+3, thread1, "C");
  threads[3] = chThdCreateStatic(wa[3], WA_SIZE, chThdGetPriority()+4, thread1, "D");
  threads[4] = chThdCreateStatic(wa[4], WA_SIZE, chThdGetPriority()+2, thread1, "E");
  chSemSignal(&sem1);
  chSemSignal(&sem1);
  chSemSignal(&sem1);
  chSemSignal(&sem1);
  chSemSignal(&sem1);
  test_wait_threads();
#if CH_USE_SEMAPHORES_PRIORITY
  test_assert_sequence(1, "ADCEB");
#else
  test_assert_sequence(1, "ABCDE");
#endif
}

/**
 * @page test_sem_002 Timeout test
 *
 * <h2>Description</h2>
 * The three possible semaphore waiting modes (do not wait, wait with timeout,
 * wait without timeout) are explored.<br>
 * The test expects that the semaphore wait function returns the correct value
 * in each of the above scenario and that the semaphore structure status is
 * correct after each operation.
 */
const struct testcase testsem1 = {
  sem1_gettest,
  sem1_setup,
  NULL,
  sem1_execute
};

static char *sem2_gettest(void) {

  return "Semaphores, timeout";
}

static void sem2_setup(void) {

  chSemInit(&sem1, 0);
}

static msg_t thread2(void *p) {

  (void)p;
  chThdSleepMilliseconds(50);
  chSysLock();
  chSemSignalI(&sem1); /* For coverage reasons */
  chSchRescheduleS();
  chSysUnlock();
  return 0;
}

static void sem2_execute(void) {
  int i;
  systime_t target_time;
  msg_t msg;

  /*
   * Testing special case TIME_IMMEDIATE.
   */
  msg = chSemWaitTimeout(&sem1, TIME_IMMEDIATE);
  test_assert(1, msg == RDY_TIMEOUT, "wrong wake-up message");
  test_assert(2, isempty(&sem1.s_queue), "queue not empty");
  test_assert(3, sem1.s_cnt == 0, "counter not zero");

  /*
   * Testing not timeout condition.
   */
  threads[0] = chThdCreateStatic(wa[0], WA_SIZE, chThdGetPriority() - 1,
                                 thread2, 0);
  msg = chSemWaitTimeout(&sem1, MS2ST(500));
  test_wait_threads();
  test_assert(4, msg == RDY_OK, "wrong wake-up message");
  test_assert(5, isempty(&sem1.s_queue), "queue not empty");
  test_assert(6, sem1.s_cnt == 0, "counter not zero");

  /*
   * Testing timeout condition.
   */
  test_wait_tick();
  target_time = chTimeNow() + MS2ST(5 * 500);
  for (i = 0; i < 5; i++) {
    test_emit_token('A' + i);
    msg = chSemWaitTimeout(&sem1, MS2ST(500));
    test_assert(7, msg == RDY_TIMEOUT, "wrong wake-up message");
    test_assert(8, isempty(&sem1.s_queue), "queue not empty");
    test_assert(9, sem1.s_cnt == 0, "counter not zero");
  }
  test_assert_sequence(10, "ABCDE");
  test_assert_time_window(11, target_time, target_time + ALLOWED_DELAY);
}

const struct testcase testsem2 = {
  sem2_gettest,
  sem2_setup,
  NULL,
  sem2_execute
};

#if CH_USE_SEMSW
/**
 * @page test_sem_003 Atomic signal-wait test
 *
 * <h2>Description</h2>
 * This test case explicitly address the @p chSemWaitSignal() function. A
 * thread is created that performs a wait and a signal operations.
 * The tester thread is awakened from an atomic wait/signal operation.<br>
 * The test expects that the semaphore wait function returns the correct value
 * in each of the above scenario and that the semaphore structure status is
 * correct after each operation.
 */

static char *sem3_gettest(void) {

  return "Semaphores, atomic signal-wait";
}

static void sem3_setup(void) {

  chSemInit(&sem1, 0);
}

static msg_t thread3(void *p) {

  (void)p;
  chSemWait(&sem1);
  chSemSignal(&sem1);
  return 0;
}

static void sem3_execute(void) {

  threads[0] = chThdCreateStatic(wa[0], WA_SIZE, chThdGetPriority()+1, thread3, 0);
  chSemSignalWait(&sem1, &sem1);
  test_assert(1, isempty(&sem1.s_queue), "queue not empty");
  test_assert(2, sem1.s_cnt == 0, "counter not zero");

  chSemSignalWait(&sem1, &sem1);
  test_assert(3, isempty(&sem1.s_queue), "queue not empty");
  test_assert(4, sem1.s_cnt == 0, "counter not zero");
}

const struct testcase testsem3 = {
  sem3_gettest,
  sem3_setup,
  NULL,
  sem3_execute
};
#endif /* CH_USE_SEMSW */
#endif /* CH_USE_SEMAPHORES */

/*
 * Test sequence for semaphores pattern.
 */
const struct testcase * const patternsem[] = {
#if CH_USE_SEMAPHORES
  &testsem1,
  &testsem2,
#if CH_USE_SEMSW
  &testsem3,
#endif
#endif
  NULL
};