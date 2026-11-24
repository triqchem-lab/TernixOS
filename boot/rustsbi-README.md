# rustsbi —— SBI 运行时服务层的参照实现（fork：clearnature/rustsbi）

上游 rustsbi/rustsbi（MIT + MulanPSL2，1355★，活跃）。**与我们未 fork 前
无关 ubios**——两仓同属 Rust 固件生态但分工不同：

| 仓库 | 角色 | TriRV 对应 |
|---|---|---|
| udk/ubios | UEFI 替代（引导面：系统表/启动服务） | TSBI-Boot（三值引导规范） |
| rustsbi | SBI 运行时服务（M 态固件→S 态 OS 的调用面：console/timer/中断委托/PMP/AIA） | TSBI-RT（三值 SBI 运行时规范） |

## 可复用面（节省的开发时间）

- `library/sbi-spec` + `sbi-rt`：SBI 调用规范与 runtime 侧封装的
  **结构范本**——TriRV 需产出对应的三值规范（TSBI），结构照此设计；
- `firmware/prototyper`：多板卡 toml 配置 → 链接脚本生成 → driver/
  devicetree 解析/heap——**正是 TernixOS 需要的"内存布局与链接脚本"先例**
  （移植四模块之四）；
- `library/riscv-aia`：中断控制器抽象（对比批次 4 的 mipi 设计）。

## 需 TriRV 化重写的部分（不可直接复用）

RISC-V 汇编上下文、CSR 操作、SBI 调用编码（基于 RISC-V SBI spec）——
TriRV 的 TSBI 规范需从 ISA 语义重新定义（45-trit 寄存器堆、三值调用
约定），代码结构可参照、语义不可照抄。
