#include "comm.h"

#define XMIT_CONST_ARRAY(_arr) (GAL_UART_Transmit_DMA((uint8_t *)_arr, sizeof(_arr)))
#define XMITSYNC_CONST_ARRAY(_arr) (GAL_UART_Transmit((uint8_t *)_arr, sizeof(_arr)))

#define STX 0x02
#define ETX 0x03

typedef enum cmd {
    SETC = 0x0A,
    GETC = 0xA0,
    DOWN = 0xAA,
    ACKX = 0xDD
} cmd_t;

enum fields_e {
    F_STX = 0x0,
    F_CMD = 0x1,
    F_LEN = 0x2,
    F_BUF = 0x6,
    F_ETX = 0xa
};

struct COMM_Handle *phcomm;
static uint8_t buf[0x20];
static uint8_t send_etx = 0;
static const uint8_t ACK_FRAME[] = { STX, ACKX, ETX };

static void txCpltCback(UART_HandleTypeDef *huart) {

    if (send_etx) {
        GAL_UART_Transmit_DMA(&buf[F_ETX], 0x1);
        send_etx = 0;
    }

}

static void rxCpltCback(UART_HandleTypeDef *huart) {

    if (buf[F_STX] != STX) {
        GAL_UART_AsyncRecv(buf, F_ETX + 1); // asking again for everything
        return;
    }

    if (buf[F_CMD] == SETC) {
        // settings received, set it up

        // sending ACK
        if (buf[F_ETX] == ETX) {
            XMIT_CONST_ARRAY(ACK_FRAME);
        }

    } else if (buf[F_CMD] == GETC) {

        // Sending ACK
        XMITSYNC_CONST_ARRAY(ACK_FRAME);

        // Sending new frame containing SrcMemory data
        buf[F_CMD] = DOWN;
        *((uint32_t *)(buf + F_LEN)) = 0x00000000 | phcomm->SrcMemory.size;
        GAL_UART_Transmit(buf, F_LEN + 4);

        send_etx = 1; // At 1, after transmission complete, ETX will be sent
        GAL_UART_Transmit_DMA(phcomm->SrcMemory.basePtr, phcomm->SrcMemory.size);
    }

    GAL_UART_AsyncRecv(buf, F_ETX + 1); // asking again for everything
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
    GAL_UART_AsyncRecv(buf, F_ETX + 1);
}
