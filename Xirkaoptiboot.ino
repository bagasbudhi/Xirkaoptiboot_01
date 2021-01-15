//uint16_t charSet[]= {65000, 32796, 16843, 10, 11234};
//uint16_t displayInt;
//int k;
//char ch;


#define OPTIBOOT_MAJVER 4
#define OPTIBOOT_MINVER 4

#define SIGNATURE_0 0x1E
#define SIGNATURE_1 0x95
#define SIGNATURE_2 0x0f

//#define MAKESTR(a) #a
//#define MAKEVER(a, b) MAKESTR(a*256+b

//asm(" .section .version\n"
//    "optiboot_version: .word " MAKEVER(OPTIBOOT_MAJVER, OPTIBOOT_MINVER) "\n"
//    " .section .text\n");

//#include <inttype.h>
//#include <avr/io.h>
//#include <avr/pgmspace.h>

//#include "boot.h"
//#include "pin_defs.h"
#include "stk500.h"
//#include "random.h"


//#define F_CPU 8000000L
#define F_CPU FOSC
#define BAUD_RATE 115200L

//#if (F_CPU/BAUD_RATE) > 280
//#ifndef SOFT_UART
//#define SOFT_UART
//#endif
//#endif

uint8_t flash[(32UL<<10)];
uint8_t buff[256];

void putch(char);
uint8_t getch(void);
//static inline void watchdogReset();
//void watchdogConfig(uint8_t x);
//#ifdef SOFT_UART
//void uartDelay() __attribute__ ((naked));
//#endif

//void appStart() __attribute__ ((naked));




void __attribute__ ((long_call, section(".data.data_begin"))) setup(void) {
  uint8_t ch;

  register uint16_t address = 0; // alamat
  register uint8_t length;

  //asm volatile ("clr __zero_reg__");
  
  Serial.begin(BAUD_RATE);
  
  for(;;) {

    ch = getch();
    
//    if(ch == 'STK_NODEVICE' ){
//      Serial.print("I received: STK_NODEVICE ");
//      //Serial.println(ch);
//      for(k=0;k<5;k++){
//        displayInt = charSet[k]; 
//        Serial.println(displayInt);
//        
//        }
//        Serial.println();
//      
//      }
    
    if(ch == STK_GET_PARAMETER) { //0x41 'A'
      //Serial.print("Get parameter");
      unsigned char which = getch();
      verifySpace();
      
      if(which == 0x82){
        putch(OPTIBOOT_MINVER);
        }else if(which == 0x81){
          putch(OPTIBOOT_MAJVER);
          }else{
            putch(0x03);
            }
      }

    else if(ch == STK_SET_DEVICE){
      getNch(20);
      
      }

    else if(ch == STK_SET_DEVICE_EXT){
      getNch(5);
    }

    else if(ch == STK_LOAD_ADDRESS){
      uint16_t newAddress;
      newAddress = getch();
      newAddress = (newAddress & 0xff) | (getch() << 8);
    #ifdef RAMPZ
      RAMPZ = (newAddress & 0x8000) ? 1:0;
    #endif
      newAddress += newAddress;
      address = newAddress;
      verifySpace();
      }

    else if(ch == STK_UNIVERSAL){
      getNch(4);
      putch(0x00);
    }

    else if(ch == STK_PROG_PAGE){
      
      uint8_t *bufPtr;
      uint8_t *addrPtr;

      getch();
      length = getch();
      getch();

      // While that is going on, read in page contents
      bufPtr = buff;
      do *bufPtr++ = getch();
      while (--length);

      // Read command terminator, start reply
      verifySpace();
      
//      Serial.println("Erasing Sector 0x2000...");
//      __disable_irq();
//      while(!(FLASHCTRL->STATUS & FLASH_STAT_INTREADY_Msk)); // Wait until ready
//      FLASHCTRL->CONTROL |= FLASH_CTRL_INTERASE_Msk;         // Enable erase mode
//      (*(uint8_t *) 0x2000) = 0x30;                           // Erase address 0x2000
//      FLASHCTRL->CONTROL &= ~FLASH_CTRL_INTERASE_Msk;        // Disable erase mode
//      while(!(FLASHCTRL->STATUS & FLASH_STAT_INTREADY_Msk)); // Wait until erase completed
//      //for(volatile int i=0; i<0xFFFFF; i++);
//      __enable_irq();
//      Serial.println("Erase Done!");

//      #ifdef VIRTUAL_BOOT_PARTITION
//      if ((uint16_t)(void*)address == 0) {
//        // This is the reset vector page. We need to live-patch the code so the
//        // bootloader runs.
//        //
//        // Move RESET vector to WDT vector
//        uint16_t vect = buff[0] | (buff[1]<<8);
//        rstVect = vect;
//        wdtVect = buff[8] | (buff[9]<<8);
//        vect -= 4; // Instruction is a relative jump (rjmp), so recalculate.
//        buff[8] = vect & 0xff;
//        buff[9] = vect >> 8;
//
//        // Add jump to bootloader at RESET vector
//        buff[0] = 0x7f;
//        buff[1] = 0xce; // rjmp 0x1d00 instruction
//      }
//      #endif

      // Write to flash
      bufPtr = buff;
      addrPtr = &flash[address];
      ch = 0; //Pagesize / 2;
      do{
        // Program
//        Serial.println("Programming Sector...");
//        __disable_irq();
//        while(!(FLASHCTRL->STATUS & FLASH_STAT_INTREADY_Msk)); // Wait until ready
//        for(int i=0; i<256; i++){
//        *(uint8_t *)(0x2000+i) = (uint8_t) ~i;
//        while(!(FLASHCTRL->STATUS & FLASH_STAT_INTREADY_Msk)); // Wait until ready
//        }
//        __enable_irq();
        *addrPtr = *bufPtr;
        bufPtr++;
        addrPtr++;
      }while(--ch);

        
      }

    else if(ch == STK_READ_PAGE){

        getch();
        length = getch();
        getch();

      verifySpace();
      
//        Serial.println("Disabling Cache...");
//        __disable_irq();
//        if((CMSDK_CACHE->SR & CMSDK_CACHE_SR_CS_Msk) == (0b10 << CMSDK_CACHE_SR_CS_Pos))
//        CMSDK_CACHE->CCR &=  ~CMSDK_CACHE_CCR_EN_Msk; // Disable cache
//        while ((CMSDK_CACHE->SR & CMSDK_CACHE_SR_CS_Msk) != (0b00 << CMSDK_CACHE_SR_CS_Pos)); // Wait until cache is disabled
//        __enable_irq();
        
        do {
          //putch(pgm_read_byte_near(address++));
          putch(flash[address++]);
        }
        while (--length);
        
        
        }

    else if(ch == STK_READ_SIGN){
      
      // READ SIGN - return what Avrdude wants to hear
      verifySpace();
      putch(SIGNATURE_0);
      putch(SIGNATURE_1);
      putch(SIGNATURE_2);
      
      }

    else {
      verifySpace();
      }
      putch(STK_OK);
    }   
  
  
}

void loop() {

}

uint8_t getch(void){

  uint8_t ch;

//  #ifdef SOFT_UART
//    __asm__ __volatile__(
//      "1: sbic %[uartPin],%[uartBit]\n"
//      "   rjmp 1b\n"
//      "   rcall uartDelay\n"
//      "2: rcall uartDelay\n"
//      "   rcall uartDelay\n"
//      "   clc\n"
//      "   sbic %[uartPin],%[uartBit]\n"
//      "   sec\n"
//      "   dec  %[bitCnt]\n"
//      "   breq 3f\n"
//      "   ror  %[ch]\n"
//      "   rjmp .2b\n"
//      "3:\n"
//      :
//        [ch] "=r" (ch)
//      :
//        [bitCnt] "d" (9),
//        [uartPin] "I" (_SFR_IO_ADDR(UART_PIN)),
//        [uartBit] "I" (UART_RX_BIT)
//      :
//        "r25"
//        
//      );
//
//      #endif
while(!Serial.available());
ch = Serial.read();
      return ch;
  }

void putch(char ch) {
//#ifdef SOFT_UART
//  __asm__ __volatile__(
//
//    "   com %[ch]\n"
//    "   sec\n"
//    "1: brcc 2f\n"
//    "   cbi %[uartPort],%[uartBit]\n"
//    "   rjmp 3f\n"
//    "2: sbi %[uartPort],%[uartBit]\n"
//    "   nop\n"
//    "3: rcall uartDelay\n"
//    "   rcall uartDelay\n"
//    "   lsr %[ch]\n"
//    "   dec %[bitcnt]\n"
//    "   brne 1b\n"
//    :
//    :
//      [bitcnt] "d" (10),
//      [ch] "r" (ch),
//      [uartPort] "I" (_SFR_IO_ADDR(UART_PORT)),
//      [uartBit] "I" (UART_TX_BIT)
//    :
//      "r25"
// );
//#endif    

Serial.print(ch);
}

void getNch(uint8_t count){
  do getch(); while (--count);
  verifySpace();
}

void verifySpace() {
  if(getch() != CRC_EOP){
    SCB->AIRCR = (0x05FAUL << SCB_AIRCR_VECTKEY_Pos) | SCB_AIRCR_SYSRESETREQ_Msk;
    while(1);
  }
 putch(STK_INSYNC);
}
