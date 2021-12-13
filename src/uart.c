#include "uart.h"

static struct GAL_UART_Data *pduart = NULL;

static void __UART_DMATransmitCplt(DMA_HandleTypeDef *hdma) {
    UART_HandleTypeDef *huart = (UART_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

    /* DMA Normal mode*/
    if ((hdma->Instance->CR & DMA_SxCR_CIRC) == 0U) {
        huart->TxXferCount = 0x00U;

        /* Disable the DMA transfer for transmit request by setting the DMAT bit
           in the UART CR3 register */
        ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAT);

        /* Enable the UART Transmit Complete Interrupt */
        ATOMIC_SET_BIT(huart->Instance->CR1, USART_CR1_TCIE);

    } else { /* DMA Circular mode */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
        /*Call registered Tx complete callback*/
        huart->TxCpltCallback(huart);
#else
        /*Call legacy weak Tx complete callback*/
        HAL_UART_TxCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
    }

}

void GAL_UART_Init(struct GAL_UART_Data *duart) {

    if (pduart) {
        return;
    }

    pduart = duart;
    __UART4_CLK_ENABLE();

    pduart->huart.Instance = UART4;
    pduart->huart.Init.BaudRate = 115200;
    pduart->huart.Init.WordLength = UART_WORDLENGTH_8B;
    pduart->huart.Init.StopBits = UART_STOPBITS_1;
    pduart->huart.Init.Parity = UART_PARITY_NONE;
    pduart->huart.Init.Mode = UART_MODE_TX_RX;
    pduart->huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    pduart->huart.Init.OverSampling = UART_OVERSAMPLING_16;

    pduart->last_status = HAL_UART_Init(&pduart->huart);

    HAL_NVIC_SetPriority(UART4_IRQn, pduart->IRQ_Priority.preempt, pduart->IRQ_Priority.sub);
    HAL_NVIC_EnableIRQ(UART4_IRQn);

}

void GAL_UART_Deinit() {
    HAL_NVIC_DisableIRQ(UART4_IRQn);
    pduart->last_status = HAL_UART_DeInit(&pduart->huart);
    pduart = NULL;
}

void GAL_UART_LINKDMA(struct GAL_DMA_Data *ddma) {
    __HAL_LINKDMA(&pduart->huart, hdmatx, ddma->hdma);
}

void GAL_UART_AsyncRecv(uint8_t *pData, uint16_t size) {
    pduart->last_status = HAL_UART_Receive_IT(&pduart->huart, pData, size);
}

void GAL_UART_Transmit(uint8_t *pData, uint16_t size) {
    pduart->last_status = HAL_UART_Transmit(&pduart->huart, pData, size, 0);
}

HAL_StatusTypeDef GAL_UART_Transmit_DMA(uint8_t *pData, uint16_t size) {
    uint32_t *tmp;

    while (HAL_UART_GetState(&pduart->huart) == HAL_UART_STATE_BUSY);

    if (pduart->huart.gState == HAL_UART_STATE_READY) {
        if ((pData == NULL) || (size == 0U)) {
            return HAL_ERROR;
        }

        /* Process Locked */
        __HAL_LOCK(&pduart->huart);

        pduart->huart.pTxBuffPtr = pData;
        pduart->huart.TxXferSize = size;
        pduart->huart.TxXferCount = size;

        pduart->huart.ErrorCode = HAL_UART_ERROR_NONE;
        pduart->huart.gState = HAL_UART_STATE_BUSY_TX;

        /* Set the UART DMA transfer complete callback */
        pduart->huart.hdmatx->XferCpltCallback = __UART_DMATransmitCplt;

        /* Enable the UART transmit DMA stream */
        tmp = (uint32_t *)&pData;
        HAL_DMA_Start_IT(pduart->huart.hdmatx, *(uint32_t *)tmp,(uint32_t)&pduart->huart.Instance->DR, size);

        /* Clear the TC flag in the SR register by writing 0 to it */
        // __HAL_UART_CLEAR_FLAG(&pduart->huart, UART_FLAG_TC);

        /* Process Unlocked */
        __HAL_UNLOCK(&pduart->huart);

        /* Enable the DMA transfer for transmit request by setting the DMAT bit
           in the UART CR3 register */
        ATOMIC_SET_BIT(pduart->huart.Instance->CR3, USART_CR3_DMAT);
        return HAL_OK;
    }

    return HAL_BUSY;
}

void UART4_IRQHandler() {
    HAL_UART_IRQHandler(&pduart->huart);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

    if (pduart->Callback.RxCompleteCallback) {
        pduart->Callback.RxCompleteCallback(huart);
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {

    if (pduart->Callback.TxCompleteCallback) {
        pduart->Callback.TxCompleteCallback(huart);
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {

    if (pduart->Callback.ErrorCallback) {
        pduart->Callback.ErrorCallback(huart);
    }
}
