#ifndef _COMM_H_
#define _COMM_H_

#include <stdbool.h>
#include "stm32f4xx.h"
#include "dma.h"
#include "uart.h"

#define ONNEW_VALUE_SET_CB(__fnc__name__, __arg__name__)   void (__fnc__name__)(uint32_t __arg__name__)
#define ONUART_DOWNLOAD_CB(__fnc__name__, __arg__name__)   void (__fnc__name__)(bool __arg__name__)
#define ONNEW_SYSDTTIME_CB(__fnc__name__, __arg__name__)   void (__fnc__name__)(const char *__arg__name__)

struct COMM_Handle {

    struct GAL_UART_Data duart;
    struct GAL_DMA_Data ddma;

    struct {
        uint8_t *basePtr;
        uint16_t size;
    } SrcMemory;

    struct callback_s {
        /* Callback invoked when a SET command is requested via UART.
        @param newValue: the new value to be set */
        ONNEW_VALUE_SET_CB(*newValueSet, newValue);

        /* Callback invoked both when a GET command is requested via UART 
        and when a GET transfer finishes.
        @param xferFinished: true if the transfer is finished, false otherwise */
        ONUART_DOWNLOAD_CB(*onUARTDownload, xferFinished);

        /* Callback invoked when a SET_TIME command is requested via UART.
        @param newDateTime: string with the new date time to be set.
        Format: dd/mm/yyyy hh/mm/ss */
        ONNEW_SYSDTTIME_CB(*onNewSysDateTime, newDateTime);
    } Callback;

};
    
void COMM_Init(struct COMM_Handle *hcomm);
void COMM_Deinit();
void COMM_StartListen();

#endif
