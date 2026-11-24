/* port.c —— TriRV port 层（ADR-001；ADR-002 架构关系：本 port 适配
 * 的是 TernixOS **HAL 接口**——HAL 当下后端=trirv VM 承载，终态=
 * FPGA/ASIC 直驱；port 不感知后端差异，切换零改动。）
 * 状态：接口骨架，T1–T4 落地实现）。
 *
 * TriRV VM 的协作式顺序执行消除了 FreeRTOS port 的三大硬件耦合：
 *   1. 上下文切换 = 普通 C 栈帧保存/恢复（SW/LW 序列，ISA 原生）；
 *   2. 中断 = VM take_trap / irq_timer（trirv 批次 5 接 mtimer）；
 *   3. Tick = VM 调度器每 N 步注入一次 tick 回调。
 * 因此本文件不需要汇编。
 */
#include "FreeRTOS.h"
#include "task.h"
#include "portmacro.h"

/* T2：栈帧初始化——任务首跑时栈顶布局（参数 a0 + 入口返回桩） */
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack,
                                    TaskFunction_t pxCode,
                                    void *pvParameters )
{
    (void)pxTopOfStack; (void)pxCode; (void)pvParameters;
    /* T2 落地：压入 return-stub 地址与 pvParameters（a0），
     * stub 调 pxCode(pvParameters) 后走 vTaskEndScheduler。 */
    return pxTopOfStack;
}

BaseType_t xPortStartScheduler( void )
{
    /* T3 落地：注册 tick 回调到 VM（每 TICK_MS 注入），进入首任务。 */
    return 0;
}

void vPortYield( void )
{
    /* T3：保存当前 TCB 上下文 → vTaskSwitchContext → 恢复下一任务。
     * 协作式 VM 下等价于一次普通 C 调用链。 */
}

void vPortEnterCritical( void ) { /* T3：调度器锁++ */ }
void vPortExitCritical( void )  { /* T3：调度器锁--，0 时开抢占 */ }
