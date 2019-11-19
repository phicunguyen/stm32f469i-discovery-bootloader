# stm32f469i-discovery-bootloader
CubeMx FreeRtos - Create a bootloader to read an application from sd card and program it into the flash then execute it.

1. Using CubeMx to generate the bootloader (please refer to the bootloader.doc)
2. Using CubeMx to generate a blink led.   (please refer to the blink_led.doc)

The bootloader program:
  1. Run a tash to toggle the LD1 (LED1) every 200ms and also reading PA0 pin (user button)
  2. If PA0 pin (user button) press then go check if a micro-sd card present.
  3. if present, then read the led.bin from the sd card, if not continous toggle LED1.
  4. Store the data in the led.bin at address 0x10000 (take a while).
  5. Jump to address 0x10000 and and blink LED2 every 200ms. (LED1 should stop blinking).
  
The led program must do the following.
  1. Modify STM32F469NI_FLASH.ld
  
    from FLASH (rx)      : ORIGIN = 0x8000000, LENGTH = 2048K
    to   FLASH (rx)      : ORIGIN = 0x8010000, LENGTH = 2048K  (where is the application (led) reside.
     
  2. Change VECT_TAB_OFFSET system_stm32f4xx.c 
  
    from #define VECT_TAB_OFFSET  0x00000
    to   #define VECT_TAB_OFFSET  0x10000 
  
  Achieved.
    1. How to mount the SD Card
    2. How to open a file in SD Card.
    3. How to read the file from SD Card.
    4. How erase flash and program it.
    5. Where bootloader and application address reside.
       bootlader addrest start at 0x000000 and up to the file size.
       application address start at 0x10000 and up to the file size.
       (application address don't have to be at 0x10000 (it depends on how big your bootloader).
       (For my bootloader, it has to display the LCD, program the stm32, FPGA and external flash
          that why it need that much memory).
    
  Have fun and enjoy it.
