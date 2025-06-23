#ifndef _APP_H_
#define _APP_H_

#include "fsl_lpuart.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Debug UART (LPUART4) */
#define DEBUG_LPUART          LPUART4
#define DEBUG_LPUART_CLK_FREQ CLOCK_GetLPFlexCommClkFreq(4u)

/* PMS3003 Sensor UART (LPUART2) */
#define PMS_LPUART          LPUART2
#define PMS_LPUART_CLK_FREQ CLOCK_GetLPFlexCommClkFreq(2u)
#define PMS_BAUDRATE        9600u

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void BOARD_InitHardware(void);
void BOARD_InitPMS3003UART(void);
void readPMSData(void);

#endif /* _APP_H_ */