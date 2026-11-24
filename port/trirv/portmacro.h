/* portmacro.h —— TriRV port 层（ADR-001 + 假设风险表裁定口径）
 *
 * TriRV：45-trit word = 9 tryte（tryte=5 trit）；int/pointer 同宽；
 * VM 抢占式（step 末异步 trap 注入）；中断屏蔽 = mstatus.MIE（CSR）。
 */
#ifndef PORTMACRO_H
#define PORTMACRO_H

#include <stdint.h>

/* ABI 锁死（评审建议 5：编译期报错替代栈帧偏移错位） */
#define TRIRV_WORD_TRYTES   9
#define TRIRV_WORD_TRITS    45
_Static_assert(sizeof(void *) == sizeof(int), "TriRV: int 与 pointer 必须同宽（45 trit）");

#define portSTACK_TYPE      int
#define portBASE_TYPE       int
typedef portSTACK_TYPE StackType_t;
typedef int BaseType_t;
typedef unsigned int UBaseType_t;
typedef unsigned long TickType_t;

#define portMAX_DELAY       ( ( TickType_t ) 0x7fffffff )
#define portSTACK_ALIGNMENT 9      /* tryte（word 对齐） */
#define portBYTE_ALIGNMENT  9

/* 中断屏蔽：真实 CSR 口径（mstatus.MIE，trap/MRET 硬件语义已由双 VM 实现）。
 * csr 指令由 tri-tcc 生成（T-1.7 CSR 已过链）；此处包装为内联函数。 */
void vPortDisableInterrupts( void );   /* csrrw mstatus ← 清 MIE */
void vPortEnableInterrupts( void );    /* csrrw mstatus ← 置 MIE */
#define portDISABLE_INTERRUPTS()    vPortDisableInterrupts()
#define portENABLE_INTERRUPTS()     vPortEnableInterrupts()

#define portYIELD()             vPortYield()
#define portEND_SWITCHING_ISR( x ) do { if( x ) vPortYield(); } while(0)
#define portYIELD_FROM_ISR( x ) vPortYieldFromISR()

void vPortEnterCritical( void );   /* MIE 清 + 调度锁计数 */
void vPortExitCritical( void );    /* 计数归 0 时恢复 MIE */

#endif /* PORTMACRO_H */
