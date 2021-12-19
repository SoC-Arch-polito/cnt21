#ifndef _COMMON_H
#define _COMMON_H

#include "stm32f4xx.h"

struct IRQPriority_s {
    /* The Preemption Priority allows an ISR to be interrupted (or preempted) 
    by a higher IRQ priority. If you don't want preemption at all, set it to
    0 (hightest priority). */
    uint32_t preempt;

    /* The Sub Priority is the classical priority between interrupts.
    Lower value has a higher priority than a higher one. */
    uint32_t sub;
};

#endif
