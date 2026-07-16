/****************************************************************************
 * boards/arm/stm32h7/px4-fmu-v6c/src/stm32_sdmmc.c
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
#include <errno.h>
#include <debug.h>

#include <nuttx/sdio.h>
#include <nuttx/mmcsd.h>

#include "stm32_gpio.h"
#include "stm32_sdmmc.h"

#include "px4-fmu-v6c.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: stm32_sdmmc_initialize
 *
 * Description:
 *   Called by stm32_bringup() to initialize the SDMMC card slot.
 *
 ****************************************************************************/

#ifdef CONFIG_STM32_SDMMC
int stm32_sdmmc_initialize(void)
{
  struct sdio_dev_s *sdmmc;

  /* Configure SDMMC2 GPIOs (AF should be set by hardware init via board.h) */

  stm32_configgpio(GPIO_SDMMC2_D0);
  stm32_configgpio(GPIO_SDMMC2_D1);
  stm32_configgpio(GPIO_SDMMC2_D2);
  stm32_configgpio(GPIO_SDMMC2_D3);
  stm32_configgpio(GPIO_SDMMC2_CK);
  stm32_configgpio(GPIO_SDMMC2_CMD);

  /* Mount the SDIO-based MMC/SD block driver */

  sdmmc = sdio_initialize(CONFIG_NSH_MMCSDSLOTNO);
  if (!sdmmc)
    {
      ferr("ERROR: Failed to initialize SDIO slot %d\n",
           CONFIG_NSH_MMCSDSLOTNO);
      return -ENODEV;
    }

  int ret = mmcsd_slotinitialize(CONFIG_NSH_MMCSDMINOR, sdmmc);
  if (ret != OK)
    {
      ferr("ERROR: Failed to bind SDIO to the MMC/SD driver: %d\n", ret);
      return ret;
    }

  return OK;
}
#endif /* CONFIG_STM32_SDMMC */
