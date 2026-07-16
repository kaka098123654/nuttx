/****************************************************************************
 * boards/arm/stm32h7/px4-fmu-v6c/src/stm32_autoleds.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdint.h>
#include <stdbool.h>
#include <debug.h>

#include <nuttx/board.h>

#include "arm_internal.h"
#include "stm32_gpio.h"

#include "px4-fmu-v6c.h"
#include <arch/board/board.h>

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void led_setonoff(unsigned int bits)
{
  /* LEDs are active low: SET = off, CLEAR = on */

  stm32_gpiowrite(GPIO_LED_RED,
                  (bits & BOARD_LED1_BIT) == 0);
  stm32_gpiowrite(GPIO_LED_BLUE,
                  (bits & BOARD_LED2_BIT) == 0);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: board_autoled_initialize
 ****************************************************************************/

void board_autoled_initialize(void)
{
  /* Configure LED GPIOs for output */

  stm32_configgpio(GPIO_LED_RED);
  stm32_configgpio(GPIO_LED_BLUE);
}

/****************************************************************************
 * Name: board_autoled_on
 ****************************************************************************/

void board_autoled_on(int led)
{
  switch (led)
    {
      case LED_STARTED:       /* All off */
        led_setonoff(0);
        break;

      case LED_HEAPALLOCATE:  /* Red off, Blue on */
        led_setonoff(BOARD_LED2_BIT);
        break;

      case LED_IRQSENABLED:   /* Red on, Blue off */
        led_setonoff(BOARD_LED1_BIT);
        break;

      case LED_STACKCREATED:  /* Both on */
        led_setonoff(BOARD_LED1_BIT | BOARD_LED2_BIT);
        break;

      default:
        break;
    }
}

/****************************************************************************
 * Name: board_autoled_off
 ****************************************************************************/

void board_autoled_off(int led)
{
  switch (led)
    {
      case LED_STARTED:       /* All off */
        led_setonoff(0);
        break;

      case LED_HEAPALLOCATE:  /* Red off, Blue off */
        led_setonoff(0);
        break;

      case LED_IRQSENABLED:   /* All off */
        led_setonoff(0);
        break;

      case LED_STACKCREATED:  /* All off */
        led_setonoff(0);
        break;

      default:
        break;
    }
}
