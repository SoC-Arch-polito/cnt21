#include "comm.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>

struct COMM_Handle *phcomm;
static uint8_t buf[0x20];
static uint8_t send_etx = 0;
static uint8_t buf_cnt = 0;
static volatile uint32_t setValue = 0;

static void txCpltCback(UART_HandleTypeDef *huart) {

    if (send_etx) {
        buf[0] = '\n';
        GAL_UART_Transmit_DMA(&buf[0], 0x1);
        send_etx = 0;
    }

}

static void rxCpltCback(UART_HandleTypeDef *huart) {
    int len = 0, i;

    GAL_UART_Transmit(buf + buf_cnt + len, 0x1); // echo back

    if (buf[buf_cnt] == '\r') {

        buf[buf_cnt + 0] = '\0';
        buf[buf_cnt + 1] = '\n';

        // do things
        if (!strncmp((char *)buf, "set", 0x3) && buf[0x3] == ' ') {
            setValue = atol((char *)buf + 4);
            for (i = 4; i < buf_cnt && (buf[i] >= '0' && buf[i] < '9'); i++) {
                buf[buf_cnt + 2 + 6 + i] = buf[i];
            }

            buf[buf_cnt + (len = (2 + 6 + i + 0))] = '\r';
            strcpy((char *)buf + buf_cnt + 2, "New value");
            buf[buf_cnt + 9 + 2] = ' ';
        } else if (!strncmp((char *)buf, "get", 0x3)) {
            GAL_UART_Transmit(buf + buf_cnt + 1, 1); 
            send_etx = 1;
            GAL_UART_Transmit_DMA(phcomm->SrcMemory.basePtr, phcomm->SrcMemory.size);
            len = -2;
        }
        
        buf[buf_cnt + len + 1] = '\n';
        GAL_UART_Transmit_DMA(buf + buf_cnt, len + 2); 
        buf_cnt = 0xff;
    } 

    GAL_UART_AsyncRecv(buf + ++buf_cnt, 0x1);

}

void COMM_Init(struct COMM_Handle *hcomm) {

    if (phcomm) {
        return;
    }
    
    phcomm = hcomm;

    hcomm->ddma.IRQ_Priority.preempt = 0;
    hcomm->ddma.IRQ_Priority.sub = 0;
    hcomm->ddma.Callback.XferCompleteCallback = NULL;
    hcomm->ddma.Callback.XferErrorCallback = NULL;
    GAL_DMA_Init(&hcomm->ddma);

    hcomm->duart.IRQ_Priority.preempt = 1;
    hcomm->duart.IRQ_Priority.sub = 0;
    hcomm->duart.Callback.RxCompleteCallback = rxCpltCback;
    hcomm->duart.Callback.TxCompleteCallback = txCpltCback;
    hcomm->duart.Callback.ErrorCallback = NULL;
    GAL_UART_Init(&hcomm->duart);
    
    GAL_UART_LINKDMA(&hcomm->ddma);
}

void COMM_Deinit() {
    GAL_DMA_Deinit();
    GAL_UART_Deinit();
    phcomm = NULL;
}

void COMM_StartListen() {
    GAL_UART_AsyncRecv(buf, 1); 
}