# LiteOS-M → TriRV port（分支 liteos-m，验证矩阵第二列）

## LiteOS-M arch 抽象接口 → TriRV 实现映射（ADR-001 验证矩阵）

| LiteOS-M 接口 | TriRV 实现 | 状态 |
|---|---|---|
| los_context.h（任务上下文切换） | 纯 C 栈保存/恢复（SW/LW 序列——VM 协作式无汇编切换） | T5 落地 |
| los_interrupt.h（中断/异常） | VM take_trap + irq_timer（trirv 批次 4 mipi） | T5 落地 |
| los_atomic.h（原子操作） | ISA A 扩展（LRW/SCW/AMO*）原生映射 | 已有 ISA |
| los_timer.h（系统 Tick） | VM irq_timer + mtimer（trirv 批次 5） | T5 落地 |

## 与 freertos 分支的关系

两分支 = 验证矩阵两列：FreeRTOS 跑通证明 ISA 满足其假设；LiteOS-M
跑通证明对第二套 RTOS 假设也成立。port 层代码不共享（各按内核接口
原生实现），验证结论在 main 分支的 ADR-001 汇总。

## 上游

OpenAtomFoundation/kernel_liteos_m（master，Apache-2.0）。
LiteOS-M 构建体系：Kconfig + make；TriRV 目标 = 新增 arch/trirv 等效目录
（对照官方 C-SKY 移植文档流程）。
