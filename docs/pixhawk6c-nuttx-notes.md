# Pixhawk 6C NuttX 移植与学习笔记

> 日期: 2026-07-16  
> 板子: Holybro Pixhawk 6C (FMUv6C)  
> MCU: STM32H743II (Cortex-M7, 480MHz, 2MB Flash, 1MB SRAM)

---

## 1. 硬件概要

| 项目 | 规格 |
|------|------|
| FMU 处理器 | STM32H743II (Cortex-M7 @ 480MHz) |
| IO 处理器 | STM32F103 (Cortex-M3 @ 72MHz, 通过 USART6 通讯) |
| HSE 晶振 | 16 MHz |
| Flash | 2 MB |
| SRAM | 1 MB (DTCM 128K + AXI 512K + SRAM1/2/3 288K + SRAM4 64K) |
| 传感器 | ICM-42688-P, BMI055, IST8310 (磁力计), MS5611 (气压计) |

### UART 映射

| USART | 功能 | RX | TX | 备注 |
|-------|------|-----|-----|------|
| USART1 | GPS1 | PA10 | PB6 | |
| USART2 | TELEM3 | PA3 | PD5 | |
| **USART3** | **Debug Console** | **PD9** | **PD8** | **NuttX 串口终端** |
| UART5 | TELEM2 | PD2 | PC12 | CTS/RTS |
| USART6 | PX4IO | PC7 | PC6 | 连接 STM32F103 |
| UART7 | TELEM1 | PE7 | PE8 | RTS/CTS |
| UART8 | GPS2 | PE0 | PE1 | |

### 其他外设引脚

| 外设 | 引脚 |
|------|------|
| SPI1 (传感器) | SCK=PA5, MISO=PA6, MOSI=PA7 |
| SPI2 (FRAM) | SCK=PD3, MISO=PC2, MOSI=PC3 |
| I2C1 | SCL=PB8, SDA=PB7 |
| I2C2 | SCL=PB10, SDA=PB11 |
| I2C4 | SCL=PD12, SDA=PD13 |
| CAN1 | RX=PD0, TX=PD1 |
| CAN2 | RX=PB5, TX=PB13 |
| SDMMC2 (MicroSD) | CK=PD6, CMD=PD7, D0=PB14, D1=PB15, D2=PB3, D3=PB4 |
| USB OTG FS | DM=PA11, DP=PA12, VBUS=PA9 |
| LED Red | PD10 (低电平有效) |
| LED Blue | PD11 (低电平有效) |
| Buzzer | PB0 (TIM3_CH3) |

### Debug 口引脚 (JST SM10B)

| Pin | 信号 | 用途 |
|-----|------|------|
| 1 | Vtref (+3.3V) | 参考电压 |
| 2 | Console TX (OUT) | USART3 TX → 串口模块 RX |
| 3 | Console RX (IN) | USART3 RX → 串口模块 TX |
| 4 | SWDIO | SWD 数据 |
| 5 | SWCLK | SWD 时钟 |
| 9 | nRST | 复位 |
| 10 | GND | 地线 |

---

## 2. 时钟配置

```
HSE (16MHz)
  └─ PLL1: VCO = 16MHz/1 × 60 = 960MHz
       ├─ PLL1P = 960/2 = 480MHz → SYSCLK
       ├─ PLL1Q = 960/4 = 240MHz → SPI, SDMMC
       └─ PLL1R = 960/8 = 120MHz

  ├─ PLL2: VCO = 16MHz/4 × 48 = 192MHz
  │    ├─ PLL2P/Q/R = 96MHz → SPI, ADC
  │
  └─ PLL3: VCO = 16MHz/4 × 48 = 192MHz
       └─ PLL3Q = 48MHz → USB

总线时钟:
  SYSCLK = 480MHz
  HCLK   = 240MHz  (SYSCLK/2)
  PCLK1  = 120MHz  (HCLK/2)
  PCLK2  = 120MHz
  PCLK3  = 120MHz
  PCLK4  = 120MHz

Flash Wait States: 2
```

---

## 3. 文件结构

```
boards/arm/stm32h7/px4-fmu-v6c/
├── CMakeLists.txt              # 顶层 CMake
├── Kconfig                      # 板级 Kconfig 菜单
├── configs/
│   └── nsh/
│       └── defconfig            # NSH 默认配置
├── include/
│   └── board.h                  # ★ 时钟树 + 全部引脚定义
├── scripts/
│   ├── Make.defs                # 链接脚本 + 编译选项
│   ├── flash.ld                 # Flash 链接脚本
│   └── memory.ld               # 内存布局定义
└── src/
    ├── CMakeLists.txt
    ├── Make.defs                # 源文件列表 (Make)
    ├── px4-fmu-v6c.h            # 板级内部头文件
    ├── stm32_boot.c             # 早期初始化 (stm32_boardinitialize)
    ├── stm32_bringup.c          # 后期初始化 (procfs, SDMMC)
    ├── stm32_spi.c              # SPI 总线 chip select
    ├── stm32_autoleds.c         # ARCH_LEDS (诊断 LED)
    ├── stm32_userleds.c         # 用户可控 LED
    └── stm32_sdmmc.c            # MicroSD 初始化
```

### Kconfig 注册

在 `nuttx/boards/Kconfig` 中有两处修改：

1. **板子选择条目** (约 1986 行)：
```
config ARCH_BOARD_PX4_FMU_V6C
    bool "Holybro Pixhawk 6C (PX4 FMUv6C)"
    depends on ARCH_CHIP_STM32H743II
    select ARCH_HAVE_LEDS
```

2. **板子名映射** (约 3868 行)：
```
default "px4-fmu-v6c" if ARCH_BOARD_PX4_FMU_V6C
```

3. **Kconfig 引用** (约 4632 行)：
```
source "boards/arm/stm32h7/px4-fmu-v6c/Kconfig"
```

---

## 4. 构建与烧录

### 编译

```bash
cd nuttx
./tools/configure.sh px4-fmu-v6c:nsh
make -j$(nproc)
```

产物：
- `nuttx` — ELF (含调试符号)
- `nuttx.bin` — 原始二进制 (~140KB)
- `nuttx.hex` — Intel HEX

### 烧录 (OpenOCD + J-Link)

```bash
openocd \
  -f interface/jlink.cfg \
  -c "transport select swd" \
  -c "reset_config srst_only srst_nogate" \
  -f target/stm32h7x.cfg \
  -c "init" \
  -c "reset init" \
  -c "halt" \
  -c "program nuttx.bin 0x08000000 verify" \
  -c "reset" \
  -c "exit"
```

> **注意**: STM32H7 必须使用 `reset_config srst_only srst_nogate`，否则 halt 会超时。

### Console 连接

Debug 口 Pin 2 (TX) / Pin 3 (RX) / Pin 10 (GND)，波特率 **115200 8N1**。

---

## 5. 启动流程

```
上电 → __start (vectors)
  → stm32_boardinitialize()          [stm32_boot.c]
       ├─ stm32_spidev_initialize()  (SPI CS 配置)
       └─ board_autoled_initialize() (LED 初始化)
  → board_late_initialize()          [stm32_boot.c]
       └─ stm32_bringup()            [stm32_bringup.c]
            ├─ mount /proc           (procfs)
            └─ sdio_initialize()     (SDMMC, 若启用)
  → nsh_main()                       → NuttShell 提示符
```

---

## 6. NSH 常用命令

```bash
help           # 列出所有命令
ls /           # 列出根目录
ls /dev        # 设备节点
ls /proc       # 进程信息
cat /proc/mem  # 内存详情
ps             # 任务列表
free           # 内存统计
date           # 系统时间
```

---

## 7. 学习路线

```
第一阶段: NSH Shell 探索
  ├─ help / ps / free / ls /dev / ls /proc
  └─ 理解 NuttX 是一个类 POSIX 的 RTOS

第二阶段: 读懂启动流程
  ├─ stm32_boot.c    → 第一个入口函数
  ├─ stm32_bringup.c → 第二阶段挂载文件系统
  └─ board.h         → 时钟树 + 引脚映射

第三阶段: 逐步添加硬件功能
  ├─ MicroSD 卡 (SDMMC2) → 读写文件
  ├─ USB Device (CDC ACM) → USB 虚拟串口
  ├─ 编写 Hello World 应用
  └─ SPI 传感器驱动
```

---

## 8. GDB 调试

### 8.1 启动调试环境

**终端1 — 启动 OpenOCD (GDB Server, 保持运行):**
```bash
cd nuttx
openocd \
  -f interface/jlink.cfg \
  -c "transport select swd" \
  -c "reset_config srst_only srst_nogate" \
  -f target/stm32h7x.cfg
```

**终端2 — 启动 GDB:**
```bash
cd nuttx
arm-none-eabi-gdb nuttx
target extended-remote :3333
monitor reset init
monitor halt
```

### 8.2 GDB 命令速查

#### 断点

| 命令 | 说明 |
|------|------|
| `b stm32_boardinitialize` | 在函数入口设断点 |
| `b stm32_boot.c:42` | 在指定文件行号设断点 |
| `hb __start` | 硬件断点 (Cortex-M Flash 调试) |
| `tbreak func` | 临时断点 (命中一次后自动删除) |
| `b func if x>5` | 条件断点 (x>5 时才停下) |
| `info breakpoints` / `i b` | 查看所有断点 |
| `d 1` / `delete 1` | 删除 1 号断点 |
| `disable 1` / `enable 1` | 禁用/启用 1 号断点 |
| `ignore 1 10` | 忽略 1 号断点前 10 次命中 |

#### 运行控制

| 命令 | 说明 |
|------|------|
| `c` / `continue` | 继续全速运行 |
| `s` / `step` | 单步进入 (进入函数内部) |
| `n` / `next` | 单步跳过 (不进入函数) |
| `si` / `stepi` | 汇编级单步进入 |
| `ni` / `nexti` | 汇编级单步跳过 |
| `finish` / `fin` | 执行到当前函数返回 |
| `until 123` | 执行到第 123 行 |
| `Ctrl+C` | 暂停正在运行的程序 |
| `monitor reset halt` | 复位并停在第一条指令 |

#### 查看数据

| 命令 | 说明 |
|------|------|
| `p var` / `print var` | 打印变量值 |
| `p/x var` | 十六进制打印 |
| `p/t var` | 二进制打印 |
| `p/d var` | 十进制打印 |
| `p *ptr` | 解引用指针 |
| `p arr[0]@10` | 打印数组前 10 个元素 |
| `info locals` | 显示当前栈帧所有局部变量 |
| `info args` | 显示当前函数参数 |
| `display var` | 每次停下时自动打印 var |
| `undisplay 1` | 取消自动显示 |
| `set var x=5` | 修改变量值 |

#### 调用栈

| 命令 | 说明 |
|------|------|
| `bt` / `backtrace` | 查看调用栈 |
| `bt full` | 调用栈 + 每帧的局部变量 |
| `frame 2` / `f 2` | 切换到 2 号栈帧 |
| `up` / `down` | 上移/下移栈帧 |

#### 寄存器与内存

| 命令 | 说明 |
|------|------|
| `info registers` / `i r` | 查看所有寄存器 |
| `p/x $pc` | 查看程序计数器 |
| `p/x $sp` | 查看栈指针 |
| `x/10x $sp` | 查看栈顶 10 个字 (hex) |
| `x/s addr` | 以字符串方式查看地址 |
| `x/i $pc` | 反汇编当前指令 |
| `x/20i $pc` | 反汇编当前地址起 20 条指令 |
| `disassemble func` | 反汇编指定函数 |

#### 监视点

| 命令 | 说明 |
|------|------|
| `watch var` | 写监视点 (变量被写入时停下) |
| `rwatch var` | 读监视点 (变量被读取时停下) |
| `awatch var` | 读写监视点 |
| `info watchpoints` | 查看所有监视点 |

#### 源码与反汇编

| 命令 | 说明 |
|------|------|
| `l` / `list` | 显示当前位置源码 |
| `list func` | 显示指定函数源码 |
| `list file.c:42` | 显示指定文件指定行 |
| `disassemble` / `disas` | 反汇编当前函数 |
| `layout src` | TUI 源码窗口 |
| `layout asm` | TUI 汇编窗口 |
| `layout split` | TUI 源码+汇编分屏 |
| `tui disable` | 退出 TUI 模式 |
| `Ctrl+x a` | 切换 TUI 模式 |

#### OpenOCD 专用 (monitor)

| 命令 | 说明 |
|------|------|
| `monitor reset init` | 复位并初始化调试接口 |
| `monitor reset halt` | 复位并立即暂停 CPU |
| `monitor halt` | 暂停 CPU |
| `monitor resume` | 恢复运行 |
| `load` | 下载 nuttx ELF 到 Flash |
| `monitor flash write_image erase nuttx.bin 0x08000000` | 烧录 bin 文件 |

#### 其他

| 命令 | 说明 |
|------|------|
| `info threads` | 查看所有线程 |
| `thread 2` | 切换到 2 号线程 |
| `info files` | 查看加载的符号文件 |
| `file nuttx` | 重新加载符号文件 |
| `quit` / `q` | 退出 GDB |

### 8.3 典型调试场景

**场景1 — 追踪完整启动流程:**
```
(gdb) b stm32_boardinitialize
(gdb) b board_late_initialize
(gdb) b nsh_main
(gdb) c
```
每到一个断点用 `bt` 看调用栈，`n` 单步跟踪执行路径。

**场景2 — 细看 bringup 初始化:**
```
(gdb) b stm32_bringup
(gdb) c
(gdb) n          # 单步看 procfs 挂载
(gdb) n          # 单步看 SD 卡初始化（若启用）
```

**场景3 — 直接运行到 NSH (不停中间断点):**
```
(gdb) b nsh_main
(gdb) c
```
此时串口终端应出现 `nsh>`。

**场景4 — 调试 panic 或 crash:**
```
(gdb) b up_assert       # 断言失败时停下
(gdb) b up_hardfault    # HardFault 时停下
(gdb) c
# crash 后:
(gdb) bt                # 看调用栈定位问题
(gdb) info registers    # 看寄存器状态
```

**场景5 — 从上电第一条指令开始单步:**
```
# 连接后不要用 monitor reset init，改用 reset halt:
(gdb) monitor reset halt      # ★ 复位后立即停在第一条指令

# 此时 CPU 停在复位向量，PC ≈ 0x08000000
(gdb) display/i $pc           # 每次停下自动显示当前汇编指令
(gdb) stepi                   # 单步一条汇编指令
(gdb) si                      # stepi 缩写，回车可重复执行

# 快速跳到 C 入口:
(gdb) hb __start              # 硬件断点
(gdb) c
(gdb) b stm32_boardinitialize
(gdb) c
(gdb) n                       # C 源码级单步开始
```
> **说明**: `monitor reset halt` 与 `monitor reset init; monitor halt` 的区别在于，前者让 OpenOCD 在复位释放后**立即**暂停 CPU，此时 CPU 尚未执行任何用户代码。STM32H7 的复位向量位于 `0x08000000`（SP 初始值）和 `0x08000004`（PC 初始值 → `__start`）。`hb` 设置硬件断点（使用 CoreSight FPB 单元），不会修改 Flash 内容。

### 8.4 GDB 快捷脚本 (.gdbinit)

创建 `.gdbinit` 避免每次手动输入连接命令：

```bash
cat > nuttx/.gdbinit << 'EOF'
target extended-remote :3333
monitor reset init
monitor halt

# 常用断点（按需取消注释）
# b stm32_boardinitialize
# b stm32_bringup
# b nsh_main
EOF
```

之后只需 `arm-none-eabi-gdb nuttx` 即可自动连接。

### 8.5 恢复 PX4 固件

如果需要恢复原厂 PX4 固件，使用 QGroundControl 重新烧录即可，或从 PX4 源码编译：
```bash
make px4_fmu-v6c_default upload
```

---

## 9. 参考资源

- PX4 FMUv6C 源码: `PX4/PX4-Autopilot/boards/px4/fmu-v6c/`
- FMUv6C 引脚 PDF: Holybro Docs → System Diagram & Pinout
- NuttX 官方文档: https://nuttx.apache.org/docs/
- STM32H743 参考手册: RM0433

---

## 附录A: 快速操作速查

### 修改配置后重新编译烧录（完整流程）

```bash
cd nuttx
make distclean
./tools/configure.sh px4-fmu-v6c:nsh
make -j$(nproc)
# 烧录
openocd -f interface/jlink.cfg \
  -c "transport select swd" \
  -c "reset_config srst_only srst_nogate" \
  -f target/stm32h7x.cfg \
  -c "init" -c "reset init" -c "halt" \
  -c "program nuttx.bin 0x08000000 verify" \
  -c "reset" -c "exit"
```

### 仅修改源码重新编译烧录（不需 distclean）

```bash
cd nuttx
make -j$(nproc)
# 然后执行上面的烧录命令
```

### GDB 快速启动

```bash
# 终端1
openocd -f interface/jlink.cfg -c "transport select swd" \
  -c "reset_config srst_only srst_nogate" -f target/stm32h7x.cfg

# 终端2
arm-none-eabi-gdb nuttx
(gdb) target extended-remote :3333
(gdb) monitor reset init
(gdb) monitor halt
(gdb) b nsh_main
(gdb) c
```

### 当前配置

| 功能 | 状态 |
|------|------|
| 串口 Console (USART3, 115200) | ✅ 已启用 |
| procfs (`/proc`) | ✅ 已启用 |
| NSH Shell | ✅ 已启用 |
| LEDs (Red/Blue) | ✅ 已配置 |
| SDMMC2 (MicroSD) | 🔜 待启用 |
| USB CDC ACM | 🔜 待启用 |
| SPI 传感器 | 🔜 待启用 |