# ADR-002 架构定位：裸金属 OS + HAL 分层（终态即起点设计）

状态 ✅（2026-11-24，定位定案；外部评审论述采纳为决策文本）

## 1. 定位

**TernixOS 不是 VM，是 OS**——从 UEFI 启动接管硬件、与 Linux 平级的
独立体系。对外暴露 trit 语义硬件抽象，不暴露 bit 语义。

```
应用（纯 trit 语义）
  ↓ 三进制 syscall
TernixOS 微内核（调度/三进制 FS/网络栈）
  ↓ HAL 接口（trit 语义硬件抽象）
HAL 实现（后端可替换）
  ├── 后端 A（当下）：VM 承载（trit 数组+解释执行，trirv CVM/PyVM）
  └── 后端 B（终态）：FPGA/ASIC 三值硬件直驱
物理 bit 硬件
```

## 2. 编码边界（唯一允许 byte 流存在的两处）

| 层级 | 状态 |
|---|---|
| 网络/安装介质（ISO、tri-pkg 传输） | byte 流（物理约束） |
| **TernixOS 文件系统/内存/寄存器** | **纯 trit，无编码** |
| HAL ↔ 物理 bit 硬件 | byte 流（物理约束） |

类比契约：云 VPS 的 Linux 看不到物理 CPU 微码/磁盘扇区/网卡 MAC——
只看到 vCPU/块设备/vNIC。TernixOS 应用看不到 bit 字节/bit 文件系统——
只看到 trit word/tryte 地址/三进制文件系统。**OS 对硬件的隔离是合法
契约，TernixOS 与 Linux 平级，非寄生关系。**

## 3. 分发链路（无二进制 OS 参与）

1. 安装介质 `TernixOS.iso` → UEFI 直接引导（UEFI 是标准，如加载
   GRUB 一样加载 TernixOS EFI 引导程序）；
2. 安装器由 TernixOS 自带（分区/三进制文件系统格式化/包解压）；
3. 软件包 tri-pkg：网络传输=byte 流（物理约束），落地 TernixOS FS
   后解码为 trit 文件——编码层只在介质边界；
4. 构建农场：tri-tcc 交叉编译（宿主 x86_64 执行），**产物不依赖任何
   二进制 OS 运行**；可复现构建（源码→比特级一致 .trpkg）。

## 4. HAL 与"软件模拟"的关系（核心工程纪律）

- HAL 是 trit 语义的**接口定义**（trit 内存/trit 块设备/trit 字符流/
  tick/中断面）；
- 当下后端 = trirv VM 承载（解释执行，性能低）——**这只是 HAL 的
  一个后端实现，不是 TernixOS 的主体语义**；
- 终态后端 = FPGA（三值门）/ASIC——HAL 退化为薄驱动层；
- **架构纪律**：TernixOS 核心语义从第一天假设"HAL 是硬件加速的"；
  软件模拟细节禁止泄漏进内核/应用语义（Linux 不因奔腾/Zen4 而改语义，
  同理）。

## 5. 与 TriRV 主仓路线（B0）的关系

- B0 拍板「纯虚拟先行、硬件直通后置」——本 ADR 即其终态在 TernixOS
  侧的架构化：**当下 HAL 后端 A（VM）= B0 的"纯虚拟"；后端 B（硬件）
  = B0 的"直通后置"终态**。两者用同一 HAL 接口衔接，切换零内核改动。
- FreeRTOS port（ADR-001）适配的对象是 **HAL 接口**（不是 VM 内部）——
  port 经 HAL，HAL 再落 VM 后端。

## 6. 当下状态与缺口（诚实清单）

- port/trirv 现直接调 VM 语义——需重构为「port → HAL 接口 → VM 后端」
  （HAL 接口定义 = 批次 T-arch，插在 T1 前）；
- 仓库尚无 HAL 目录/接口文件——T-arch 产出 `hal/trirv_hal.h`（接口）
  与 `hal/backend_vm.c`（VM 后端实现）；
- 分发链路（ISO/UEFI/安装器/tri-pkg）为终态设计，未开工——里程碑
  排 M 系列尾部（内核验证完成后再立）。
