//#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__) || defined(__AVR_ATmega88) || defined(__AVR_ATmega8__) || defined(__AVR_ATmega88__)
///* Onboard LED is connected to pin PB5 in Arduino NG, Diecimila, and Duemilanove */ 
//#define LED_DDR     DDRB
//#define LED_PORT    PORTB
//#define LED_PIN     PINB
//#define LED         PINB5
//
///* Ports for soft UART */
//#ifdef SOFT_UART
//#define UART_PORT   PORTD
//#define UART_PIN    PIND
//#define UART_DDR    DDRD
//#define UART_TX_BIT 1
//#define UART_RX_BIT 0
//#endif
//#endif


//=========================================
//#if defined(XST_X1_02)
//
//#ifdef SOFT_UART
//#define UART_PORT CMSDK_GPIO8->DATAOUT
//#define UART_PIN  CMSDK_GPIO8->DATA 
//#define UART_DDR  CMSDK_GPIO8->OUTENABLESET
//#define UART_TX_BIT 1
//#define UART_RX_BIT 0
//
//#endif
//#endif
