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

    struct IRQPriority_s IRQ_Priority;

    struct {
        /* DMA transfer complete callback */
        void (*XferCompleteCallback)(struct __DMA_HandleTypeDef *hdma); 

        /* DMA transfer error callback */
        void (*XferErrorCallback)(struct __DMA_HandleTypeDef *hdma); 
    } Callback;

    DMA_HandleTypeDef hdma;
    HAL_StatusTypeDef last_status;
};

void GAL_DMA_Init(struct GAL_DMA_Data *ddma);
void GAL_DMA_Deinit();
// void GAL_DMA_Start();

#endif