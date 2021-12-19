#ifndef _USART_H_
#define _USART_H_

#include "stm32f4xx.h"
#include "dma.h"
#include "common.h"

struct GAL_UART_Data {

    struct IRQPriority_s IRQ_Priority;

    struct {
        /* UART receive complete callback */
        void (*RxCompleteCallback) (UART_HandleTypeDef *huart);

        /* UART transfer complete callback */
        void (*TxCompleteCallback) (UART_HandleTypeDef *huart);

        /* UART transfer error callback */
        void (*ErrorCallback) (UART_HandleTypeDef *huart);
    } Callback;

    UART_HandleTypeDef huart;
    HAL_StatusTypeDef last_status;
};

void GAL_UART_Init(struct GAL_UART_Data *duart);
void GAL_UART_Deinit();
void GAL_UART_LINKDMA(struct GAL_DMA_Data *ddma);
void GAL_UART_Transmit(uint8_t *pData, uint16_t size);

/* Used to transmit data overt UART via DMA.
!! It replaces DMA's transfer complete interrupt !!
At the end of transfer, a UART interrupt is fired,
it will call Callback.TxCompleteCallback() */
HAL_StatusTypeDef GAL_UART_Transmit_DMA(uint8_t *pData, uint16_t size);

/* Pay Attention:
when you received 'size' bytes, the UART will stop firing 
interrupt 'till a new GAL_UART_AsyncRecv is called. */
void GAL_UART_AsyncRecv(uint8_t *pData, uint16_t size);

#endif