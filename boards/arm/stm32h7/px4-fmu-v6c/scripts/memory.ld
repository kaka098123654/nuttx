/****************************************************************************
 * boards/arm/stm32h7/px4-fmu-v6c/scripts/memory.ld
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

/* STM32H743II memory layout
 *
 *   1) 128KB DTCM at 0x20000000
 *   2) 512KB AXI SRAM at 0x24000000
 *   3) 288KB SRAM1+2+3 at 0x30000000
 *   4) 64KB  SRAM4 at 0x38000000
 *   5) 4KB backup RAM at 0x38800000
 *   6) 64KB ITCM at 0x00000000
 *   7) 2048KB Flash at 0x08000000
 */

MEMORY
{
  itcm  (rwx) : ORIGIN = 0x00000000, LENGTH =   64K
  flash (rx)  : ORIGIN = 0x08000000, LENGTH = 2048K
  dtcm1 (rwx) : ORIGIN = 0x20000000, LENGTH =   64K
  dtcm2 (rwx) : ORIGIN = 0x20010000, LENGTH =   64K
  sram  (rwx) : ORIGIN = 0x24000000, LENGTH =  512K
  sram1 (rwx) : ORIGIN = 0x30000000, LENGTH =  128K
  sram2 (rwx) : ORIGIN = 0x30020000, LENGTH =  128K
  sram3 (rwx) : ORIGIN = 0x30040000, LENGTH =   32K
  sram4 (rwx) : ORIGIN = 0x38000000, LENGTH =   64K
  bbram (rwx) : ORIGIN = 0x38800000, LENGTH =    4K
}
