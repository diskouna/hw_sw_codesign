#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xaxidma.h"


#define TX_BUFFER_BASE   XPAR_XBRAM_0_BASEADDR 
#define RX_BUFFER_BASE   XPAR_XBRAM_0_BASEADDR + 0x100

#define TRANSFER_SIZE    4    // 4  Bytes per transfert
#define TRANSFER_COUNT   10
#define TRANSFER_LENGTH  (TRANSFER_COUNT * TRANSFER_SIZE)      


XAxiDma AxiDma;

int main()
{
    init_platform();
    int Status;
    int32_t volatile *TxBufferPtr = (int32_t volatile *)TX_BUFFER_BASE;
    int32_t volatile *RxBufferPtr = (int32_t volatile *)RX_BUFFER_BASE;

    xil_printf("AXI DMA - incrementer_core \n\r");

    XAxiDma_Config *Config = XAxiDma_LookupConfig(XPAR_XAXIDMA_0_BASEADDR);
    if (!Config) {
        xil_printf("No configuration found for DMA %d\n\r", XPAR_XAXIDMA_0_BASEADDR);
        return XST_FAILURE;
    }
    Status = XAxiDma_CfgInitialize(&AxiDma, Config);
    if (Status != XST_SUCCESS) {
        xil_printf("DMA initialization failed\n\r");
        return XST_FAILURE;
    }
    xil_printf("DMA initialization done\n\r");

    // Flush and invalidate the buffer to avoid cache coherency issues
    Xil_DCacheFlushRange((UINTPTR)TxBufferPtr, TRANSFER_LENGTH);
    Xil_DCacheInvalidateRange((UINTPTR)RxBufferPtr, TRANSFER_LENGTH);

    for (int i = 0; i < TRANSFER_COUNT; i++)
        TxBufferPtr[i] = i;
    Status = XAxiDma_SimpleTransfer(&AxiDma, (UINTPTR)TxBufferPtr, TRANSFER_LENGTH, XAXIDMA_DMA_TO_DEVICE);
    if (Status != XST_SUCCESS) {
        xil_printf("DMA transmit failed\n\r");
        return XST_FAILURE;
    }

    Status = XAxiDma_SimpleTransfer(&AxiDma, (UINTPTR)RxBufferPtr, TRANSFER_LENGTH, XAXIDMA_DEVICE_TO_DMA);
    if (Status != XST_SUCCESS) {
        xil_printf("DMA receive failed\n\r");
        return XST_FAILURE;
    }

    // Polling Dma busy flag
    while (XAxiDma_Busy(&AxiDma, XAXIDMA_DMA_TO_DEVICE));
    while (XAxiDma_Busy(&AxiDma, XAXIDMA_DEVICE_TO_DMA));
    xil_printf("DMA transfers completed\n\r");

    for (int i = 0; i < TRANSFER_COUNT; i++) {
        xil_printf("Data %02d - transmitted:  %d vs  received: %d \n\r", i, TxBufferPtr[i], RxBufferPtr[i]);
    }

    cleanup_platform();
    return 0;
}
