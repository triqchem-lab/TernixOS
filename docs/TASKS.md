# TernixOS 移植任务清单（微内核验证 TriRV ISA）

> 仿真平台 = TriRV 双 VM（CVM/PyVM）；构建链 = tri-tcc（真 C 前端）+
> trirv_cc（对照）。上游依赖：trirv 主仓（工具链）、FreeRTOS-Kernel（MIT）。

## 里程碑 T1–T4（对齐 ADR-001 验证矩阵）

- [ ] **T1 内核编译过链**：FreeRTOS-Kernel 核心（tasks/list/queue/timers
      + portable 层通用文件）经 tri-tcc 编译通过，产出内核对象集。
      验收：编译率报告（文件级）+ tri-tcc 缺口清单（回 trirv 仓补方言）。
- [ ] **T2 单任务启动**：pxPortInitialiseStack + xPortStartScheduler 落地，
      单任务（计数循环）跑至 vTaskEndScheduler，串口输出退出码。
      验收：CVM 冒烟 exit=约定值；PyVM 一致。
- [ ] **T3 双任务协作切换**：portYield 纯 C 切换落地 + Tick 注入
      （VM 调度器回调），两任务乒乓计数。
      验收：Tick 驱动的切换计数正确（双 VM 一致）。
- [ ] **T4 多任务+IPC**：队列 + 信号量 + 3 任务（生产/消费/看门狗）。
      验收：集成场景退出码；PCI（FreeRTOS 官方）风格冒烟测试子集。
- [ ] **T5 第二内核 LiteOS-M**：arch 接口清单对照实现
      （los_context/atomic/interrupt/timer），验证矩阵第二列。

## 前置依赖（trirv 主仓游标）

- 批次 3b（TRV2 默认化/加载器迁移）——T1 前完成 tri-tcc 产物口径确认。
- 批次 4 AMP/中断（mipi/mhartid）——T3 的核间中断面。
- 批次 5 固件 mtimer——T3 的 tick 面。

## 结构

```
kernel/FreeRTOS-Kernel/   MIT 内核 vendor（子模块）
port/trirv/               TriRV port 层（纯 C，无汇编——VM 协作式）
app/                      验证应用（T2–T4 的 main）
tools/                    构建脚本（tri-tcc/trirv_cc 链）
docs/                     ADR 与移植笔记（含既有数学研究方向）
```

## T0 裸机冒烟（T1 前置，2026-11-24 插入；部分已被 ADR-001 裁定实验证明）

- [x] **T0.1 异步抢占实证**：8 指令死循环程序——MIE=1+irq_timer 挂起，
      任意指令退休点 trap（mcause=MTIMER）→ mtvec 分发 → trap 处理执行。
      （ADR-001「假设与风险」节的裁定实验，PyVM 亲核。）
- [ ] **T0.2 C 完整版**：tri-tcc 编译等价 C 程序（volatile tick_count +
      irq_handler + 死循环 + 串口计数输出），双 VM 冒烟。
      验收：tick 计数递增不受死循环阻塞；exit 口径双 VM 一致。
