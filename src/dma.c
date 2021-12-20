#include "dma.h"

static struct GAL_DMA_Data *pddma = NULL;

void GAL_DMA_Init(struct GAL_DMA_Data *ddma) {

    if (pddma) {
        return;
    }

    pddma = ddma;
    __DMA1_CLK_ENABLE();

    ddma->hdma.Instance = DMA1_Stream1;
    ddma->hdma.Init.Channel = DMA_CHANNEL_0;
    ddma->hdma.Init.Direction = DMA_MEMORY_TO_PERIPH;
    ddma->hdma.Init.MemInc = DMA_MINC_ENABLE;
    ddma->hdma.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    ddma->hdma.Init.Mode = DMA_NORMAL;
    ddma->hdma.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    ddma->hdma.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    ddma->hdma.Init.MemBurst = DMA_MBURST_SINGLE;
    ddma->hdma.XferCpltCallback = ddma->Callback.XferCompleteCallback;
    ddma->hdma.XferErrorCallback = ddma->Callback.XferErrorCallback;

    ddma->last_status = HAL_DMA_Init(&ddma->hdma);

    HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, ddma->IRQ_Priority.preempt, ddma->IRQ_Priority.sub);
    HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);

}

void GAL_DMA_Deinit() {
    HAL_NVIC_DisableIRQ(DMA1_Stream1_IRQn);
    HAL_DMA_DeInit(&pddma->hdma);
    pddma = NULL;
}

void GAL_DMA_Start() {
    // pddma->last_status = HAL_DMA_Start_IT(&pddma->hdma, pddma->Transfer.src, pddma->Transfer.dst, pddma->Transfer.bytes);
}

void DMA1_Stream1_IRQHandler() {
    // HAL_DMA_IRQHandler() will call the correct callback if set so nothing to do here
    HAL_DMA_IRQHandler(&pddma->hdma);
}