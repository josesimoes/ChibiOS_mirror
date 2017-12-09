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

#include "test.h"

#if CH_USE_SEMAPHORES

#define ALLOWED_DELAY MS2ST(5)

static Semaphore sem1;

static char *sem1_gettest(void) {

  return "Semaphores, FIFO enqueuing test";
}

static void sem1_setup(void) {

  chSemInit(&sem1, 0);
}

static msg_t thread(void *p) {

  chSemWait(&sem1);
  test_emit_token(*(char *)p);
  return 0;
}

static void sem1_execute(void) {

  threads[0] = chThdCreateStatic(wa[0], WA_SIZE, chThdGetPriority()+5, thread, "A");
  threads[1] = chThdCreateStatic(wa[1], WA_SIZE, chThdGetPriority()+1, thread, "B");
  threads[2] = chThdCreateStatic(wa[2], WA_SIZE, chThdGetPriority()+3, thread, "C");
  threads[3] = chThdCreateStatic(wa[3], WA_SIZE, chThdGetPriority()+4, thread, "D");
  threads[4] = chThdCreateStatic(wa[4], WA_SIZE, chThdGetPriority()+2, thread, "E");
  chSemSignal(&sem1);
  chSemSignal(&sem1);
  chSemSignal(&sem1);
  chSemSignal(&sem1);
  chSemSignal(&sem1);
  test_wait_threads();
  test_assert_sequence("ABCDE");
}

const struct testcase testsem1 = {
  sem1_gettest,
  sem1_setup,
  NULL,
  sem1_execute
};

#if CH_USE_SEMAPHORES_TIMEOUT
static char *sem2_gettest(void) {

  return "Semaphores, timeout test";
}

static void sem2_setup(void) {

  chSemInit(&sem1, 0);
}

static void sem2_execute(void) {
  int i;
  systime_t target_time;
  msg_t msg;

  msg= chSemWaitTimeout(&sem1, TIME_IMMEDIATE);
  test_assert(msg == RDY_TIMEOUT, "#1");

  target_time = chSysGetTime() + MS2ST(5 * 500);
  for (i = 0; i < 5; i++) {
    test_emit_token('A' + i);
    msg = chSemWaitTimeout(&sem1, MS2ST(500));
    test_assert(msg == RDY_TIMEOUT, "#2");
    test_assert(isempty(&sem1.s_queue), "#3");    /* Queue not empty */
    test_assert(&sem1.s_cnt != 0, "#4");          /* Counter not zero */
  }
  test_assert_sequence("ABCDE");
  test_assert_time_window(target_time, target_time + ALLOWED_DELAY);
}

const struct testcase testsem2 = {
  sem2_gettest,
  sem2_setup,
  NULL,
  sem2_execute
};
#endif /* CH_USE_SEMAPHORES_TIMEOUT */
#endif /* CH_USE_SEMAPHORES */

/*
 * Test sequence for semaphores pattern.
 */
const struct testcase * const patternsem[] = {
#if CH_USE_SEMAPHORES
  &testsem1,
#if CH_USE_SEMAPHORES_TIMEOUT
  &testsem2,
#endif
#endif
  NULL
};