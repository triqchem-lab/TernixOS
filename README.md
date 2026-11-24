# TernixOS

Ternix（TritOS）——跑在 **TriRV 三进制 ISA**（45-trit，9-tryte tryte/45-trit word）
上的微内核验证平台。

## 架构定位（ADR-002）

TernixOS **不是 VM，是 OS**——从 UEFI 启动接管硬件、与 Linux 平级。
分层：应用（纯 trit）→ 微内核 → **HAL 接口**（trit 语义硬件抽象）→
HAL 后端（当下=trirv VM 承载；终态=FPGA/ASIC 三值硬件直驱）→ 物理
bit 硬件。编码层只存在于介质边界（网络/ISO）与 HAL↔物理硬件两处；
**TernixOS 内部是纯 trit**。

核心纪律：内核/应用语义从第一天假设 HAL 是硬件加速的——软件模拟
（VM 解释执行）只是 HAL 的后端实现，禁止泄漏进上层语义。

## 内核验证（ADR-001）

首选 **FreeRTOS-Kernel（MIT）**，第二内核 LiteOS-M——验证矩阵对照
（详见 docs/ADR-001）。移植面：纯 C port（TriRV VM 协作式顺序执行
消除了汇编上下文切换/特权栈耦合），四模块 = 上下文切换 / 中断异常 /
系统 Tick / 内存布局。

里程碑 T1–T4 见 docs/TASKS.md：内核编译过链 → 单任务启动 → 双任务
Tick 切换 → 多任务 IPC。仿真平台 = TriRV 双 VM（CVM/PyVM）；硬件
（RTL/FPGA）待软件栈验证后推进。

构建链：tri-tcc（tinycc 子模块 trirv 分支的真 C 前端）+ trirv 主仓
（ISA 真相源/双 VM/链接器）。

## 数学基础研究（既有方向，保留）

使用 \(\mathbb F_9=\mathbb F_3[i]/(i^2+1)\) 作为特征 3 的最小二次扩张
（\(C_3\times C_3\) 加法群、\(C_8^\times\) 循环乘法群、Frobenius
自同构 \(\varphi(x)=x^3\)）；\(n=4k+2\) 时 \(1^n+i^n=0\) 的零值族。
其他方向：十二进制表示层、辫群与构形空间、指数塔、球谐矢量场——
均为待定义约定的研究主题，与内核实现解耦。

> 实现路线：内核验证（本 README 上半）与数学研究（下半）分头推进；
> 三值域代数若进入内核设计（如三值逻辑单元/校验），以 ADR 形式立项。
