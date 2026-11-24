/* portmacro.h —— TriRV port 层类型与常量（ADR-001，FreeRTOS port 接口）
 *
 * TriRV 字长 45 trit = 9 tryte；int/pointer 同宽。VM 协作式顺序执行
 * （无硬件特权级/中断栈），port 层纯 C。
 * 状态：接口草案（批次 T1 落地实现；见 docs/TASKS.md）。 */

#ifndef PORTMACRO_H
#define PORTMACRO_H

/* TriRV 字类型：45-trit，3 补码（tri-tcc 下 int 即 45 trit） */
#define portSTACK_TYPE          int
#define portBASE_TYPE           int

typedef portSTACK_TYPE StackType_t;
typedef int BaseType_t;
typedef unsigned int UBaseType_t;
typedef unsigned long TickType_t;      /* tick 计数：VM irq_timer 口径 */

#define portMAX_DELAY           ( ( TickType_t ) 0xffffffff )

/* TriRV VM 无字节序概念（tryte=5 trit）；本宏仅供 FreeRTOS 内核断言 */
#define portBYTE_ORDER          0

/* 栈对齐：9 tryte（word 对齐，与 trirv 调用约定一致） */
#define portSTACK_ALIGNMENT     9
#define portBYTE_ALIGNMENT      9

/* 任务切换：VM 协作式——yield 即调用 portYield 的 C 实现（无 trap） */
#define portYIELD()             vPortYield()
#define portEND_SWITCHING_ISR( x ) do { if( x ) vPortYield(); } while(0)
#define portYIELD_FROM_ISR( x ) vPortYieldFromISR()

/* 临界区：单核协作调度下 = 禁抢占窗口（调度器锁） */
#define portENTER_CRITICAL()    vPortEnterCritical()
#define portEXIT_CRITICAL()     vPortExitCritical()
#define portDISABLE_INTERRUPTS() do { } while(0)
#define portENABLE_INTERRUPTS()  do { } while(0)

/* FreeRTOS 内核接口钩子 */
void vPortYield( void );
void vPortYieldFromISR( void );
void vPortEnterCritical( void );
void vPortExitCritical( void );

#endif /* PORTMACRO_H */
