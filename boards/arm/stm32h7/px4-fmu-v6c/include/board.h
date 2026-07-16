/****************************************************************************
 * boards/arm/stm32h7/px4-fmu-v6c/include/board.h
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

#ifndef __BOARDS_ARM_STM32H7_PX4_FMU_V6C_INCLUDE_BOARD_H
#define __BOARDS_ARM_STM32H7_PX4_FMU_V6C_INCLUDE_BOARD_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#ifndef __ASSEMBLY__
#  include <stdint.h>
#endif

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Clocking *****************************************************************/

/* The px4_fmu-v6c board provides the following clock sources:
 *
 *   X1: 16 MHz crystal for HSE
 *
 * So we have these clock source available within the STM32
 *
 *   HSI: 16 MHz RC factory-trimmed
 *   HSE: 16 MHz crystal
 *   LSI: 32 KHz RC
 *   LSE: 32.768 kHz
 */

#define STM32_BOARD_XTAL        16000000ul

#define STM32_HSI_FREQUENCY     16000000ul
#define STM32_LSI_FREQUENCY     32000
#define STM32_HSE_FREQUENCY     STM32_BOARD_XTAL
#define STM32_LSE_FREQUENCY     32768

/* Main PLL Configuration.
 *
 * PLL source is HSE = 16,000,000
 *
 * PLL_VCOx = (STM32_HSE_FREQUENCY / PLLM) * PLLN
 * Subject to:
 *
 *     1 <= PLLM <= 63
 *     4 <= PLLN <= 512
 *   150 MHz <= PLL_VCOL <= 420MHz
 *   192 MHz <= PLL_VCOH <= 836MHz
 *
 * SYSCLK  = PLL_VCO / PLLP
 * CPUCLK  = SYSCLK / D1CPRE
 * Subject to
 *
 *   PLLP1   = {2, 4, 6, 8, ..., 128}
 *   CPUCLK <= 480 MHz
 */

#define STM32_BOARD_USEHSE

#define STM32_PLLCFG_PLLSRC      RCC_PLLCKSELR_PLLSRC_HSE

/* PLL1, wide 4 - 8 MHz input, enable DIVP, DIVQ, DIVR
 *
 *   PLL1_VCO = (16 MHz / 1) * 60 = 960 MHz
 *
 *   PLL1P = PLL1_VCO/2  = 960 MHz / 2   = 480 MHz
 *   PLL1Q = PLL1_VCO/4  = 960 MHz / 4   = 240 MHz
 *   PLL1R = PLL1_VCO/8  = 960 MHz / 8   = 120 MHz
 */

#define STM32_PLLCFG_PLL1CFG    (RCC_PLLCFGR_PLL1VCOSEL_WIDE | \
                                 RCC_PLLCFGR_PLL1RGE_4_8_MHZ | \
                                 RCC_PLLCFGR_DIVP1EN | \
                                 RCC_PLLCFGR_DIVQ1EN | \
                                 RCC_PLLCFGR_DIVR1EN)
#define STM32_PLLCFG_PLL1M       RCC_PLLCKSELR_DIVM1(1)
#define STM32_PLLCFG_PLL1N       RCC_PLL1DIVR_N1(60)
#define STM32_PLLCFG_PLL1P       RCC_PLL1DIVR_P1(2)
#define STM32_PLLCFG_PLL1Q       RCC_PLL1DIVR_Q1(4)
#define STM32_PLLCFG_PLL1R       RCC_PLL1DIVR_R1(8)

#define STM32_VCO1_FREQUENCY     ((STM32_HSE_FREQUENCY / 1) * 60)
#define STM32_PLL1P_FREQUENCY    (STM32_VCO1_FREQUENCY / 2)
#define STM32_PLL1Q_FREQUENCY    (STM32_VCO1_FREQUENCY / 4)
#define STM32_PLL1R_FREQUENCY    (STM32_VCO1_FREQUENCY / 8)

/* PLL2, wide 4 - 8 MHz input, enable DIVP, DIVQ, DIVR
 *
 *   PLL2_VCO = (16 MHz / 4) * 48 = 192 MHz
 *
 *   PLL2P = PLL2_VCO/2  = 192 MHz / 2   = 96 MHz
 *   PLL2Q = PLL2_VCO/2  = 192 MHz / 2   = 96 MHz
 *   PLL2R = PLL2_VCO/2  = 192 MHz / 2   = 96 MHz
 */
#define STM32_PLLCFG_PLL2CFG    (RCC_PLLCFGR_PLL2VCOSEL_WIDE | \
                                 RCC_PLLCFGR_PLL2RGE_4_8_MHZ | \
                                 RCC_PLLCFGR_DIVP2EN | \
                                 RCC_PLLCFGR_DIVQ2EN | \
                                 RCC_PLLCFGR_DIVR2EN)
#define STM32_PLLCFG_PLL2M       RCC_PLLCKSELR_DIVM2(4)
#define STM32_PLLCFG_PLL2N       RCC_PLL2DIVR_N2(48)
#define STM32_PLLCFG_PLL2P       RCC_PLL2DIVR_P2(2)
#define STM32_PLLCFG_PLL2Q       RCC_PLL2DIVR_Q2(2)
#define STM32_PLLCFG_PLL2R       RCC_PLL2DIVR_R2(2)

#define STM32_VCO2_FREQUENCY     ((STM32_HSE_FREQUENCY / 4) * 48)
#define STM32_PLL2P_FREQUENCY    (STM32_VCO2_FREQUENCY / 2)
#define STM32_PLL2Q_FREQUENCY    (STM32_VCO2_FREQUENCY / 2)
#define STM32_PLL2R_FREQUENCY    (STM32_VCO2_FREQUENCY / 2)

/* PLL3, wide 4 - 8 MHz input, enable DIVQ
 *
 *   PLL3_VCO = (16 MHz / 4) * 48 = 192 MHz
 *
 *   PLL3Q = PLL3_VCO/4  = 192 MHz / 4   = 48 MHz
 */
#define STM32_PLLCFG_PLL3CFG    (RCC_PLLCFGR_PLL3VCOSEL_WIDE | \
                                 RCC_PLLCFGR_PLL3RGE_4_8_MHZ | \
                                 RCC_PLLCFGR_DIVQ3EN)
#define STM32_PLLCFG_PLL3M      RCC_PLLCKSELR_DIVM3(4)
#define STM32_PLLCFG_PLL3N      RCC_PLL3DIVR_N3(48)
#define STM32_PLLCFG_PLL3P      RCC_PLL3DIVR_P3(2)
#define STM32_PLLCFG_PLL3Q      RCC_PLL3DIVR_Q3(4)
#define STM32_PLLCFG_PLL3R      RCC_PLL3DIVR_R3(2)

#define STM32_VCO3_FREQUENCY    ((STM32_HSE_FREQUENCY / 4) * 48)
#define STM32_PLL3Q_FREQUENCY   (STM32_VCO3_FREQUENCY / 4)

/* SYSCLK = PLL1P = 480 MHz
 * CPUCLK = SYSCLK / 1 = 480 MHz
 */

#define STM32_RCC_D1CFGR_D1CPRE  (RCC_D1CFGR_D1CPRE_SYSCLK)
#define STM32_SYSCLK_FREQUENCY   (STM32_PLL1P_FREQUENCY)
#define STM32_CPUCLK_FREQUENCY   (STM32_SYSCLK_FREQUENCY / 1)

/* Configure Clock Assignments */

/* AHB clock (HCLK) is SYSCLK/2 (240 MHz max)
 * HCLK1 = HCLK2 = HCLK3 = HCLK4
 */

#define STM32_RCC_D1CFGR_HPRE   RCC_D1CFGR_HPRE_SYSCLKd2        /* HCLK  = SYSCLK / 2 */
#define STM32_ACLK_FREQUENCY    (STM32_CPUCLK_FREQUENCY / 2)    /* ACLK in D1, HCLK3 in D1 */
#define STM32_HCLK_FREQUENCY    (STM32_CPUCLK_FREQUENCY / 2)    /* HCLK in D2, HCLK4 in D3 */
#define STM32_BOARD_HCLK        STM32_HCLK_FREQUENCY

/* APB1 clock (PCLK1) is HCLK/2 (120 MHz) */

#define STM32_RCC_D2CFGR_D2PPRE1  RCC_D2CFGR_D2PPRE1_HCLKd2       /* PCLK1 = HCLK / 2 */
#define STM32_PCLK1_FREQUENCY     (STM32_HCLK_FREQUENCY / 2)

/* APB2 clock (PCLK2) is HCLK/2 (120 MHz) */

#define STM32_RCC_D2CFGR_D2PPRE2  RCC_D2CFGR_D2PPRE2_HCLKd2       /* PCLK2 = HCLK / 2 */
#define STM32_PCLK2_FREQUENCY     (STM32_HCLK_FREQUENCY / 2)

/* APB3 clock (PCLK3) is HCLK/2 (120 MHz) */

#define STM32_RCC_D1CFGR_D1PPRE   RCC_D1CFGR_D1PPRE_HCLKd2        /* PCLK3 = HCLK / 2 */
#define STM32_PCLK3_FREQUENCY     (STM32_HCLK_FREQUENCY / 2)

/* APB4 clock (PCLK4) is HCLK/2 (120 MHz) */

#define STM32_RCC_D3CFGR_D3PPRE   RCC_D3CFGR_D3PPRE_HCLKd2        /* PCLK4 = HCLK / 2 */
#define STM32_PCLK4_FREQUENCY     (STM32_HCLK_FREQUENCY / 2)

/* Timer clock frequencies
 * Timers driven from APB1/2 will be twice PCLK1/2
 */

#define STM32_APB1_TIM2_CLKIN   (2 * STM32_PCLK1_FREQUENCY)
#define STM32_APB1_TIM3_CLKIN   (2 * STM32_PCLK1_FREQUENCY)
#define STM32_APB1_TIM4_CLKIN   (2 * STM32_PCLK1_FREQUENCY)
#define STM32_APB1_TIM5_CLKIN   (2 * STM32_PCLK1_FREQUENCY)
#define STM32_APB1_TIM6_CLKIN   (2 * STM32_PCLK1_FREQUENCY)
#define STM32_APB1_TIM7_CLKIN   (2 * STM32_PCLK1_FREQUENCY)
#define STM32_APB1_TIM12_CLKIN  (2 * STM32_PCLK1_FREQUENCY)
#define STM32_APB1_TIM13_CLKIN  (2 * STM32_PCLK1_FREQUENCY)
#define STM32_APB1_TIM14_CLKIN  (2 * STM32_PCLK1_FREQUENCY)
#define STM32_APB2_TIM1_CLKIN   (2 * STM32_PCLK2_FREQUENCY)
#define STM32_APB2_TIM8_CLKIN   (2 * STM32_PCLK2_FREQUENCY)
#define STM32_APB2_TIM15_CLKIN  (2 * STM32_PCLK2_FREQUENCY)
#define STM32_APB2_TIM16_CLKIN  (2 * STM32_PCLK2_FREQUENCY)
#define STM32_APB2_TIM17_CLKIN  (2 * STM32_PCLK2_FREQUENCY)

/* Peripheral clock sources */

#define STM32_RCC_D2CCIP2R_I2C123SRC  RCC_D2CCIP2R_I2C123SEL_HSI
#define STM32_RCC_D3CCIPR_I2C4SRC     RCC_D3CCIPR_I2C4SEL_HSI
#define STM32_RCC_D2CCIP1R_SPI123SRC  RCC_D2CCIP1R_SPI123SEL_PLL2
#define STM32_RCC_D2CCIP1R_SPI45SRC   RCC_D2CCIP1R_SPI45SEL_PLL2
#define STM32_RCC_D3CCIPR_SPI6SRC     RCC_D3CCIPR_SPI6SEL_PLL2
#define STM32_RCC_D2CCIP2R_USBSRC     RCC_D2CCIP2R_USBSEL_PLL3
#define STM32_RCC_D2CCIP2R_USART234578_SEL RCC_D2CCIP2R_USART234578SEL_RCC
#define STM32_RCC_D2CCIP2R_USART16_SEL     RCC_D2CCIP2R_USART16SEL_RCC
#define STM32_RCC_D3CCIPR_ADCSRC      RCC_D3CCIPR_ADCSEL_PLL2
#define STM32_RCC_D2CCIP1R_FDCANSEL   RCC_D2CCIP1R_FDCANSEL_HSE
#define STM32_FDCANCLK                STM32_HSE_FREQUENCY

/* FLASH wait states
 *
 *  ------------ ---------- -----------
 *  Vcore        MAX ACLK   WAIT STATES
 *  ------------ ---------- -----------
 *  1.15-1.26 V     70 MHz    0
 *  (VOS1 level)   140 MHz    1
 *                 210 MHz    2
 *  ------------ ---------- -----------
 */

#define BOARD_FLASH_WAITSTATES 2

/* SDMMC definitions ********************************************************/

/* Init 400kHz, freq = PLL1Q/(2*div) div = PLL1Q/(2*freq)
 *        = 240 MHz / (2*400kHz) = 300
 */

#define STM32_SDMMC_INIT_CLKDIV     (300 << STM32_SDMMC_CLKCR_CLKDIV_SHIFT)

/* 25 MHz Max, 25 MHz = PLL1Q/(2*div), div = PLL1Q/(2*25MHz)
 * div = 240 / 50 = 4.8, round up to 5 for 24 MB/s
 */

#define STM32_SDMMC_MMCXFR_CLKDIV   (5 << STM32_SDMMC_CLKCR_CLKDIV_SHIFT)
#define STM32_SDMMC_SDXFR_CLKDIV    (5 << STM32_SDMMC_CLKCR_CLKDIV_SHIFT)

#define STM32_SDMMC_CLKCR_EDGE      STM32_SDMMC_CLKCR_NEGEDGE

/* LED definitions **********************************************************/

/* The Pixhawk 6C board has three LEDs:
 *   LED_RED   (PD10, active low)
 *   LED_GREEN (controlled by IO processor, not on FMU directly)
 *   LED_BLUE  (PD11, active low)
 *
 * If CONFIG_ARCH_LEDS is not defined, then the user can control the LEDs.
 */

#define BOARD_LED1        0
#define BOARD_LED2        1
#define BOARD_NLEDS       2

#define BOARD_LED_RED     BOARD_LED1
#define BOARD_LED_BLUE    BOARD_LED2

#define BOARD_LED1_BIT    (1 << BOARD_LED1)
#define BOARD_LED2_BIT    (1 << BOARD_LED2)

/* If CONFIG_ARCH_LEDS is defined, the usage by the board port is:
 *
 *   SYMBOL                     Meaning                      LED state
 *                                                        Red    Blue
 *   ----------------------  --------------------------  ------ ------
 */

#define LED_STARTED        0 /* NuttX has been started   OFF    OFF   */
#define LED_HEAPALLOCATE   1 /* Heap has been allocated  OFF    ON    */
#define LED_IRQSENABLED    2 /* Interrupts enabled       ON     OFF   */
#define LED_STACKCREATED   3 /* Idle stack created       ON     ON    */
#define LED_INIRQ          4 /* In an interrupt          N/C    GLOW  */
#define LED_SIGNAL         5 /* In a signal handler      GLOW   N/C   */
#define LED_ASSERTION      6 /* An assertion failed      GLOW   GLOW  */
#define LED_PANIC          7 /* The system has crashed   Blink  OFF   */
#define LED_IDLE           8 /* MCU is in sleep mode     ON     OFF   */

/* Alternate function pin selections ****************************************/

/* USART1 - GPS1 */

#define GPIO_USART1_RX   GPIO_USART1_RX_2    /* PA10 */
#define GPIO_USART1_TX   GPIO_USART1_TX_3    /* PB6  */

/* USART2 - TELEM3 */

#define GPIO_USART2_RX   GPIO_USART2_RX_1    /* PA3  */
#define GPIO_USART2_TX   GPIO_USART2_TX_2    /* PD5  */

/* USART3 - Debug Console (NuttX serial console) */

#define GPIO_USART3_RX   GPIO_USART3_RX_3    /* PD9  */
#define GPIO_USART3_TX   GPIO_USART3_TX_3    /* PD8  */

/* UART5 - TELEM2 (with flow control) */

#define GPIO_UART5_RX    GPIO_UART5_RX_3     /* PD2  */
#define GPIO_UART5_TX    GPIO_UART5_TX_3     /* PC12 */
#define GPIO_UART5_CTS   (GPIO_ALT | GPIO_AF8 | GPIO_PORTC | GPIO_PIN9 | \
                          GPIO_PULLDOWN)     /* PC9  */

/* USART6 - PX4IO (to STM32F103 IO processor) */

#define GPIO_USART6_RX   GPIO_USART6_RX_1    /* PC7  */
#define GPIO_USART6_TX   GPIO_USART6_TX_1    /* PC6  */

/* UART7 - TELEM1 (with flow control) */

#define GPIO_UART7_RX    GPIO_UART7_RX_3     /* PE7  */
#define GPIO_UART7_TX    GPIO_UART7_TX_3     /* PE8  */
#define GPIO_UART7_RTS   GPIO_UART7_RTS_1    /* PE9  */
#define GPIO_UART7_CTS   (GPIO_UART7_CTS_1 | GPIO_PULLDOWN)  /* PE10 */

/* UART8 - GPS2 */

#define GPIO_UART8_RX    GPIO_UART8_RX_1     /* PE0  */
#define GPIO_UART8_TX    GPIO_UART8_TX_1     /* PE1  */

/* CAN */

#define GPIO_CAN1_RX     GPIO_CAN1_RX_3      /* PD0  */
#define GPIO_CAN1_TX     GPIO_CAN1_TX_3      /* PD1  */
#define GPIO_CAN2_RX     GPIO_CAN2_RX_2      /* PB5  */
#define GPIO_CAN2_TX     GPIO_CAN2_TX_1      /* PB13 */

/* SPI
 *
 * SPI1 - Sensors (ICM-42688-P, BMI055, MS5611)
 * SPI2 - FRAM
 */

#define ADJ_SLEW_RATE(p)  (((p) & ~GPIO_SPEED_MASK) | (GPIO_SPEED_2MHz))

#define GPIO_SPI1_SCK    ADJ_SLEW_RATE(GPIO_SPI1_SCK_1)    /* PA5  */
#define GPIO_SPI1_MISO   GPIO_SPI1_MISO_1                  /* PA6  */
#define GPIO_SPI1_MOSI   GPIO_SPI1_MOSI_1                  /* PA7  */

#define GPIO_SPI2_SCK    ADJ_SLEW_RATE(GPIO_SPI2_SCK_5)    /* PD3  */
#define GPIO_SPI2_MISO   GPIO_SPI2_MISO_2                  /* PC2  */
#define GPIO_SPI2_MOSI   GPIO_SPI2_MOSI_3                  /* PC3  */

/* I2C */

#define GPIO_I2C1_SCL    GPIO_I2C1_SCL_2                   /* PB8  */
#define GPIO_I2C1_SDA    GPIO_I2C1_SDA_1                   /* PB7  */

#define GPIO_I2C1_SCL_GPIO \
  (GPIO_OUTPUT | GPIO_OPENDRAIN | GPIO_SPEED_50MHz | \
   GPIO_OUTPUT_SET | GPIO_PORTB | GPIO_PIN8)
#define GPIO_I2C1_SDA_GPIO \
  (GPIO_OUTPUT | GPIO_OPENDRAIN | GPIO_SPEED_50MHz | \
   GPIO_OUTPUT_SET | GPIO_PORTB | GPIO_PIN7)

#define GPIO_I2C2_SCL    GPIO_I2C2_SCL_1                   /* PB10 */
#define GPIO_I2C2_SDA    GPIO_I2C2_SDA_1                   /* PB11 */

#define GPIO_I2C2_SCL_GPIO \
  (GPIO_OUTPUT | GPIO_OPENDRAIN | GPIO_SPEED_50MHz | \
   GPIO_OUTPUT_SET | GPIO_PORTB | GPIO_PIN10)
#define GPIO_I2C2_SDA_GPIO \
  (GPIO_OUTPUT | GPIO_OPENDRAIN | GPIO_SPEED_50MHz | \
   GPIO_OUTPUT_SET | GPIO_PORTB | GPIO_PIN11)

#define GPIO_I2C4_SCL    GPIO_I2C4_SCL_1                   /* PD12 */
#define GPIO_I2C4_SDA    GPIO_I2C4_SDA_1                   /* PD13 */

/* SDMMC2 - MicroSD card slot
 *
 *   SDMMC2_CK  - PD6
 *   SDMMC2_CMD - PD7
 *   SDMMC2_D0  - PB14
 *   SDMMC2_D1  - PB15
 *   SDMMC2_D2  - PB3
 *   SDMMC2_D3  - PB4
 */

#define GPIO_SDMMC2_CK   GPIO_SDMMC2_CK_1    /* PD6  */
#define GPIO_SDMMC2_CMD  GPIO_SDMMC2_CMD_1   /* PD7  */
#define GPIO_SDMMC2_D0   (GPIO_SDMMC2_D0_0 | GPIO_SPEED_100MHz)  /* PB14 */
#define GPIO_SDMMC2_D1   (GPIO_SDMMC2_D1_0 | GPIO_SPEED_100MHz)  /* PB15 */
#define GPIO_SDMMC2_D2   (GPIO_SDMMC2_D2_2 | GPIO_SPEED_100MHz)  /* PB3  */
#define GPIO_SDMMC2_D3   (GPIO_SDMMC2_D3_0 | GPIO_SPEED_100MHz)  /* PB4  */

/* USB OTG FS
 *
 *   OTG_FS_DM  - PA11
 *   OTG_FS_DP  - PA12
 *   OTG_FS_VBUS - PA9
 */

#define GPIO_OTGFS_VBUS  (GPIO_INPUT | GPIO_PULLDOWN | GPIO_SPEED_100MHz | \
                          GPIO_PORTA | GPIO_PIN9)

/* LEDs GPIO */

#define GPIO_LED_RED     (GPIO_OUTPUT | GPIO_PUSHPULL | GPIO_SPEED_50MHz | \
                          GPIO_OUTPUT_SET | GPIO_PORTD | GPIO_PIN10)
#define GPIO_LED_BLUE    (GPIO_OUTPUT | GPIO_PUSHPULL | GPIO_SPEED_50MHz | \
                          GPIO_OUTPUT_SET | GPIO_PORTD | GPIO_PIN11)

#endif /* __BOARDS_ARM_STM32H7_PX4_FMU_V6C_INCLUDE_BOARD_H */
