#include "board.h"
#include "app.h"
#include "fsl_lpuart.h"
#include "fsl_debug_console.h"
#include <stdio.h>
#include "pin_mux.h"

// PMS3003 variables
static uint32_t pm1 = 0;
static uint32_t pm2_5 = 0;
static uint32_t pm10 = 0;

void readPMSData(void) {
    static uint8_t pmsData[32];
    static uint8_t index = 0;
    uint8_t value;
    PRINTF("READ PASSED\r\n");
    while (LPUART_GetStatusFlags(PMS_LPUART) & kLPUART_RxDataRegFullFlag) {
        value = LPUART_ReadByte(PMS_LPUART);
        PRINTF("WHILE PASSED\r\n");
        // Check for header bytes
        if (index == 0 && value != 0x42) continue;
        if (index == 1 && value != 0x4D) {
            index = 0;
            continue;
        }
        
        if (index < 32) pmsData[index++] = value;
        
        if (index == 32) {
            // Verify checksum
            uint16_t checksum = 0;
            for (uint8_t i = 0; i < 30; i++) checksum += pmsData[i];
            
            uint16_t frameChecksum = (pmsData[30] << 8) + pmsData[31];
            
            if (checksum == frameChecksum) {
                pm1 = (pmsData[4] << 8) + pmsData[5];
                pm2_5 = (pmsData[6] << 8) + pmsData[7];
                pm10 = (pmsData[8] << 8) + pmsData[9];
                
                PRINTF("PM1.0: %d, PM2.5: %d, PM10: %d ug/m3\r\n", pm1, pm2_5, pm10);
            } else {
                PRINTF("Checksum error!\r\n");
            }
            index = 0;
        }
    }
}

void BOARD_InitPMS3003UART(void) {
    lpuart_config_t config;
    LPUART_GetDefaultConfig(&config);
    config.baudRate_Bps = PMS_BAUDRATE;
    config.enableTx = true;
    config.enableRx = true;
    // Debug prints
    PRINTF("Initializing LPUART2 at %d baud\r\n", PMS_BAUDRATE);
    PRINTF("Clock freq: %d Hz\r\n", PMS_LPUART_CLK_FREQ);

    LPUART_Init(PMS_LPUART, &config, PMS_LPUART_CLK_FREQ);
    status_t status = LPUART_Init(PMS_LPUART, &config, PMS_LPUART_CLK_FREQ);
    PRINTF("Init status: %d\r\n", status);
    // Verify pin configuration
    PRINTF("PORT4_0 state: 0x%08X\r\n", PORT4->PCR[0]);
    PRINTF("PORT4_2 state: 0x%08X\r\n", PORT4->PCR[2]);
}

int main(void)
{
    // Initialize hardware
    BOARD_InitPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();  // LPUART4 for debug
    
    // Initialize PMS3003 UART
    BOARD_InitPMS3003UART();
    
    PRINTF("PMS3003 Sensor Ready!\r\n");
    PRINTF("Using LPUART2 (P4_0-RX, P4_2-TX)\r\n");

    while (1) {
        readPMSData();
        SDK_DelayAtLeastUs(1000000, CLOCK_GetCoreSysClkFreq()); // 1s delay
    }
}