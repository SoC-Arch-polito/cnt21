#ifndef _DMA_H_
#define _DMA_H_

#include <stdint.h>
#include "stm32f4xx.h"
#include "common.h"

struct GAL_DMA_Data {

    // struct {
    //     uint32_t dst;
    //     uint32_t src;
    //     uint32_t bytes;
    // } Transfer;

    struct {
        /* The Preemption Priority allows an ISR to be interrupted (or preempted) 
        by a higher IRQ priority. If you don't want preemption at all, set it to
        0 (hightest priority). */
        uint32_t preempt;

        /* The Sub Priority is the classical priority between interrupts.
        Lower value has a higher priority than a higher one. */
        uint32_t sub;
    } IRQ_Priority;

    struct GAL_DMA_Callback Callback;

    DMA_HandleTypeDef hdma;
    HAL_StatusTypeDef last_status;
};

void GAL_DMA_Init(struct GAL_DMA_Data *ddma);
void GAL_DMA_Deinit();
// void GAL_DMA_Start();

#endif