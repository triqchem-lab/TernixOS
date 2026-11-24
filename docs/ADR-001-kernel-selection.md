# ADR-001 验证内核选型：FreeRTOS 内核（MIT）首选，LiteOS-M 第二

状态 ✅（2026-11-24）

## 背景

TriRV（45-trit 三进制 ISA）需要跑通一个真实 RTOS 来验证 ISA 的系统级
正确性（任务/上下文/Tick/中断面），仿真平台 = TriRV 双 VM（CVM/PyVM，
B0 虚拟化优先——即本项目的"QEMU"）。

## 决策

**首选 FreeRTOS-Kernel（MIT）**；LiteOS-M 为第二内核（验证矩阵对照）。

## 理由（偏离外部建议的"LiteOS-M 首选"，按 TriRV 实际条件修正）

1. **移植面最小**：FreeRTOS port 层 = `port.c` + `portmacro.h` 两个文件；
   LiteOS-M 需要先吃透其 Kconfig/liteos 构建体系——验证 ISA 的目标是
   最快跑通，构建系统是无关成本。
2. **纯 C port 可行（TriRV VM 的关键性质）**：CVM/PyVM 是协作式顺序
   执行、无硬件特权级/中断栈差异——FreeRTOS 的汇编上下文切换在 TriRV
   上退化为**普通 C 函数的栈保存/恢复**（32×45-trit 经 SW/LW 序列，
   ISA 原生支持）。LiteOS-M 的 port 同样可实现，但其接口层更厚。
3. **许可**：FreeRTOS-Kernel = MIT（与主仓/数学研究方向无冲突）。
4. **RISC-V port 先例**：`chip_specific_extensions` 机制处理非标准
   寄存器——TriRV 的三值寄存器堆按同一思路挂在 port 层。

## 验证矩阵（第二内核 LiteOS-M 的价值）

FreeRTOS 跑通只证明"TriRV 满足 FreeRTOS 的假设"。LiteOS-M 的
arch 接口清单（los_context/los_interrupt/los_atomic/los_timer）作为
第二套假设对照——两套都过，才能说 ISA 对 RTOS 生态成立，而非恰好
迎合单一内核。

## 假设与风险（2026-11-24，外部评审 5 条的逐条裁定 + 实证）

**评审三处事实更正**（TriRV 实际规格）：
1. tryte = **5 trit**（243 值），非 6——「9 tryte=54 trit 不对齐」的推导
   基于 6-trit tryte 假设；实际 9 tryte = 45 trit = word，**精确对齐**。
2. 45-trit word 是 D5 拍板规格（≈2^71.3，long=pointer 同宽），非笔误；
   21-trit RV-T 提案已被 ADR-38 前身复核否决（装不下 int64）。
3. TriRV VM **已是抢占式**：`step()` 末尾在 MIE=1 且 irq_timer 挂起时
   异步注入 trap（任意指令退休点）——已用 8 指令死循环程序实证
   （死循环 → mtimer 注入 → mcause=MTIMER → mtvec 分发 → trap 处理
   执行）。"纯 C port"假设成立，方案 B（VM 升级）无需。

| 假设 | 裁定 | 风险与缓解 |
|---|---|---|
| VM 协作式 | ❌ **实际是抢占式**（步末异步注入，已实证） | 无——FreeRTOS 抢占语义原生支持 |
| 无硬件中断屏蔽 | ❌ **有**：mstatus.MIE/MPIE + mie/mip CSR 模拟（trap 自动 MIE→MPIE） | `portDISABLE_INTERRUPTS` = 清 mstatus.MIE（真实 CSR 口径，非软件标志） |
| 45-trit word 与 C ABI | ✅ 一致：int=pointer=45 trit=9 tryte（tri-tcc PTR_SIZE=9） | `portmacro.h` 加 `_Static_assert` 锁死 |
| 无 mtimecmp CSR | ✅ 成立：VM 现 CSR 白名单无 mtimer 读写面 | `vPortSetupTimerInterrupt` 依赖 trirv 批次 5（mtimer CSR+注入 API） |
| 栈 4096 单位 | **tryte** 口径（4096 tryte = 455 word ≈ FreeRTOS 128–256 word 惯例的 1.7×） | configMINIMAL_STACK_SIZE 按 word 计 ≥128；printf 任务加栈 |

## 分支模型（2026-11-24）

- `main`：平台骨架（port 接口/文档），挂首选内核 vendor（FreeRTOS-Kernel）。
- `freertos`：FreeRTOS 工作线（与 main 同起点）。
- `liteos-m`：LiteOS-M 工作线（kernel/LiteOS_M 子模块 +
  port/trirv_liteos 接口映射表——验证矩阵第二列）。
- 两分支 port 层代码不共享（各按内核接口原生实现）；验证结论回
  main 的本 ADR 汇总。

## 移植四模块 → TriRV 实现映射

| 外部分析的模块 | TriRV 实现 | 依赖 |
|---|---|---|
| 上下文切换 | 纯 C 栈保存/恢复（SW/LW 序列，45-trit word） | 无新 ISA |
| 中断/异常 | VM take_trap + irq_timer（已有基建） | AMP mipi（trirv 批次 4） |
| 系统 Tick | VM irq_timer + mtimer CSR | trirv 批次 5 |
| 链接脚本 | trirv_cc 内存布局（GBASE=200/栈基址 4096）+ TRV1/TRV2 容器 | 已有 |

