#ifndef _COMMON_H
#define _COMMON_H

#include "stm32f4xx.h"

struct GAL_UART_Callback {
    /* UART receive complete callback */
    void (*RxCompleteCallback) (UART_HandleTypeDef *huart);

    /* UART transfer complete callback */
    void (*TxCompleteCallback) (UART_HandleTypeDef *huart);

    /* UART transfer error callback */
    void (*ErrorCallback) (UART_HandleTypeDef *huart);
};

struct GAL_DMA_Callback {
    /* DMA transfer complete callback */
    void (*XferCompleteCallback)(struct __DMA_HandleTypeDef *hdma); 

    /* DMA transfer error callback */
    void (*XferErrorCallback)(struct __DMA_HandleTypeDef *hdma); 
};

#endif