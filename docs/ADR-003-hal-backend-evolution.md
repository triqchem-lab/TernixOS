# ADR-003 HAL 后端演进：Type-1 Hypervisor 愿景与执行引擎阶梯

状态 ✅（2026-11-24；终态愿景定案 + 执行引擎阶梯 + 历史戒律）

## 1. 终态愿景（采纳外部评审论述）

**HAL = Type-1 Bare-metal Hypervisor**（为三进制优化的 KVM/Xen 同构物）：

```
UEFI (bit) → TernixOS 引导（获得 VMX Root / Ring -1）
  → HAL 初始化：EPT 页表 / VT-x 上下文 / PCI 直通
    → 三进制运行环境（Guest，Ring 0）：纯 trit 内存/虚拟设备/三进制指令
      → 三进制应用（Ring 3）
```

- **内存零拷贝**：EPT 把连续物理 bit 内存映射为 trit 数组；越界由硬件
  VM-Exit 触发（无软件页表模拟）。
- **Trap & Emulate**：硬件资源访问走 VMCALL/VM-Exit，VMRESUME 让出——
  系统级开销极低。
- **独占核心 pin**：物理核直接分配给三进制进程，硬件级抢占保证隔离。
- **云原生部署**：HAL 只依赖标准 VT-x/EPT（现代云服务器标配）——
  TernixOS ISO 上传 AWS/阿里云作为自定义镜像即可启动。

## 2. 执行引擎阶梯（当下 → 终态）

| 阶段 | 引擎 | 性能 | 状态 |
|---|---|---|---|
| E1 | CVM computed-goto（**直接线程化**——BTB 分支预测优化，评审主张已隐含实现） | 基准 | ✅ 现役 |
| E2 | 解释器微优化（3b.4 hint 缓存强化：回跳目标直查表） | +微量 | 批次 3b.4 |
| E3 | Trit-JIT（三进制加法循环 → 宿主 SIMD 机器码，AVX-512 并行 trit 运算） | 高潜力 | ⚠ **历史戒律见 §3** |
| E4 | Type-1 Hypervisor HAL（VT-x/EPT 全量） | 系统级≈原生 | 大工程立项（非批次任务） |

**系统级任务（调度/FS）在 E1 下已近似原生**——损耗集中在计算密集
（trit 运算的 bit 翻译微操作），这正是 E3/E4 的靶点。

## 3. 历史戒律：主仓 AOT/JIT 曾实测证伪

PERF_REPORT.md（trirv 主仓）记载：「常驻原生内核（AOT/JIT）：实测证伪，
已放弃」。E3 立项前**必须**：
1. 复核当时证伪的实验口径（哪些假设不成立——JIT 翻译成本 vs 常驻收益）；
2. TernixOS 语境差异验证（Guest 代码形态/长驻进程占比是否改善权衡）；
3. GO 条件量化后立项——不带着「评审说了算」直接写代码。

## 4. 与既有 ADR 的关系

- ADR-002 的「HAL 后端 A（VM 承载）」= 本 ADR 的 E1/E2/E3 阶段；
  「后端 B（FPGA/ASIC）」= E4 之后（三值硬件成熟）。
- E4（Type-1 HAL）与 E3（JIT）**正交**：hypervisor 提供 EPT/隔离/
  注入；Guest 内执行引擎（解释器/JIT）独立演进。
- ADR-001 FreeRTOS port 适配 HAL 接口——HAL 后端演进对 port 透明
  （EPT/VM-Exit 由 HAL 内部消化）。

## 5. 分发影响（采纳评审 §四）

- ISO 直发（HAL[hypervisor] + 微内核一体）：U 盘 UEFI 启动/云自定义
  镜像上传皆可（标准 VT-x/EPT）。
- tri-pkg 纯 trit 指令流：网络 byte 流（物理约束）→ TernixOS FS 内
  纯 trit（无兼容层）。
