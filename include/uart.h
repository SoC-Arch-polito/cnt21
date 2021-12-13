#ifndef _USART_H_
#define _USART_H_

#include "stm32f4xx.h"
#include "dma.h"
#include "common.h"

struct GAL_UART_Data {

    struct {
        /* The Preemption Priority allows an ISR to be interrupted (or preempted) 
        by a higher IRQ priority. If you don't want preemption at all, set it to
        0 (hightest priority). */
        uint32_t preempt;

        /* The Sub Priority is the classical priority between interrupts.
        Lower value has a higher priority than a higher one. */
        uint32_t sub;
    } IRQ_Priority;

    struct GAL_UART_Callback Callback;

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
