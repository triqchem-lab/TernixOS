# boot — TernixOS 引导固件层（替代 BIOS/UEFI）

核心子项目：**ubios**（子模块 clearnature/udk @ trirv 分支内的 `ubios/`
目录，Rust 实现，上游 hust-open-atom-club/udk）。

## 与 ADR-002 的关系

- 终态启动流：ubios 替代 UEFI → 直接引导 TernixOS 微内核 + HAL
  （跳过 bit 固件的 UEFI 层——ubios 是我们可控的引导固件）；
- 过渡期：TriRV VM 仿真平台无 UEFI 参与需引导——boot/udk 的 ubios
  按 TriRV 虚拟平台适配（VM 启动面）。

## 结构

- `boot/udk/`：udk monorepo 子模块（ubios + udkfw + odc）；
- 上游同步：merge hust-open-atom-club/udk main；
- TriRV 适配：trirv 分支开发。
