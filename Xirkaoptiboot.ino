

//-xirka optiboot--

#define userAppStartAddr ((uint32_t *)0x2000UL)

#define OPTIBOOT_MAJVER 4
#define OPTIBOOT_MINVER 4

#define SIGNATURE_0 0x1E
#define SIGNATURE_1 0x95
#define SIGNATURE_2 0x0f


#include "stk500.h"
#include <wdt.h>

#define BAUD_RATE 115200L


//uint8_t flash[(32UL<<10)];
#define flash ((volatile uint8_t*)0x0)
uint8_t buff[256];

#define svc(code) asm volatile ("svc %[immediate]"::[immediate] "I" (code))
void putch(char);
uint8_t getch(void);
//static inline void watchdogReset();
//void watchdogConfig(uint8_t x);

//void appStart() __attribute__ ((naked));

void __attribute__ ((naked)) BootJumpASM(uint32_t __attribute__((unused)) SP, uint32_t __attribute__((unused)) RH){
  asm volatile (
      "MSR      MSP,r0 \n"
      "BX       r1"
  );
}

void BootJump(uint32_t *Address){

  if(CONTROL_nPRIV_Msk & __get_CONTROL())
    svc(0);

  if(CONTROL_SPSEL_Msk & __get_CONTROL())
  {  /* MSP is not active */
    __set_MSP( __get_PSP( ) ) ;
    __set_CONTROL( __get_CONTROL( ) & ~CONTROL_SPSEL_Msk ) ;
  }

  SCB->VTOR = (uint32_t)Address;

  BootJumpASM(Address[0], Address[1]);
}

Handler SVC_Handler(void){
  __set_CONTROL(__get_CONTROL() & ~CONTROL_nPRIV_Msk);
}

void __attribute__ ((long_call, section(".data.data_begin"))) setup(void) {
  
  uint8_t ch;

  register uint16_t address = 0; // alamat
  register uint8_t length;

  if(CMSDK_SYSCON->RSTINFO){
    BootJump(userAppStartAddr);
    while(1);
  }
  
  Serial.begin(BAUD_RATE);
  Serial2.begin(115200);
  //delay(1);
  Serial2.println("hello");

  wdt_enable(WDTO_2S);
  
  for(;;) {

    ch = getch();
    
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
      Serial2.print("0x");
      Serial2.println(address, HEX);
      verifySpace();
      }

    else if(ch == STK_UNIVERSAL){
      getNch(4);
      putch(0x00);
    }

    else if(ch == STK_PROG_PAGE){
      
      uint8_t *bufPtr;
      volatile uint8_t *addrPtr;

      getch();
      length = getch();
      getch();

      // While that is going on, read in page contents
      bufPtr = buff;
      do *bufPtr++ = getch();
      while (--length);

      // Read command terminator, start reply

      if((address & 0xFF) == 0){
         Serial2.print("E 0x");
         Serial2.print(address, HEX);
         Serial2.flush();
         __disable_irq();
         while(!(FLASHCTRL->STATUS & FLASH_STAT_INTREADY_Msk)); // Wait until ready
         FLASHCTRL->CONTROL |= FLASH_CTRL_INTERASE_Msk;         // Enable erase mode
         //(*(uint8_t *) 0x2000) = 0x30;                           // Erase address 0x2000
         flash[address] = 0x30;
         FLASHCTRL->CONTROL &= ~FLASH_CTRL_INTERASE_Msk;        // Disable erase mode
         while(!(FLASHCTRL->STATUS & FLASH_STAT_INTREADY_Msk)); // Wait until erase completed
         __enable_irq();
         Serial2.println(" !");
       }
     

     verifySpace();

      // Write to flash
      Serial2.print("P 0x");
      Serial2.print(address, HEX);
      Serial2.flush();
      __disable_irq();
      bufPtr = buff;
      addrPtr = &flash[address];
      ch = length; //Pagesize / 2;
      do{
        // Program
        *addrPtr = *bufPtr;
       while(!(FLASHCTRL->STATUS & FLASH_STAT_INTREADY_Msk)); // Wait until ready
        bufPtr++;
        addrPtr++;
      }while(--ch);
       __enable_irq();
        
    }

    else if(ch == STK_READ_PAGE){

        getch();
        length = getch();
        getch();

      verifySpace();
      
       //Serial.println("Disabling Cache...");
       __disable_irq();
       if((CMSDK_CACHE->SR & CMSDK_CACHE_SR_CS_Msk) == (0b10 << CMSDK_CACHE_SR_CS_Pos))
       CMSDK_CACHE->CCR &=  ~CMSDK_CACHE_CCR_EN_Msk; // Disable cache
       while ((CMSDK_CACHE->SR & CMSDK_CACHE_SR_CS_Msk) != (0b00 << CMSDK_CACHE_SR_CS_Pos)); // Wait until cache is disabled
       __enable_irq();
        
        do {
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
  while(!Serial.available());
  ch = Serial.read();
      wdt_reset();
      return ch;
  }

void putch(char ch) { 
  Serial.print(ch);
}

void getNch(uint8_t count){
  do getch(); while (--count);
  verifySpace();
}

void verifySpace() {
  if(getch() != CRC_EOP){
    SCB->AIRCR = (0x05FAUL << SCB_AIRCR_VECTKEY_Pos) | SCB_AIRCR_SYSRESETREQ_Msk;
    // read https://www.keil.com/pack/doc/CMSIS/Core/html/regMap_pg.html
    while(1);
  }
 putch(STK_INSYNC);
}
