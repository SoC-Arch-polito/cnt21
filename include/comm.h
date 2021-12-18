#ifndef _COMM_H_
#define _COMM_H_

#include "stm32f4xx.h"
#include "dma.h"
#include "uart.h"

struct COMM_Handle {
    struct GAL_UART_Data duart;
    struct GAL_DMA_Data ddma;

    struct {
        uint8_t *basePtr;
        uint16_t size;
    } SrcMemory;

};
    
void COMM_Init(struct COMM_Handle *hcomm);
void COMM_Deinit();
void COMM_StartListen();

#endif