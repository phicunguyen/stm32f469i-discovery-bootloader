/**
  ******************************************************************************
  * @file   fatfs.c
  * @brief  Code for fatfs applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#include "fatfs.h"

uint8_t retSD;    /* Return value for SD */
char SDPath[4];   /* SD logical drive path */
FATFS SDFatFS;    /* File system object for SD logical drive */
FIL SDFile;       /* File object for SD */

/* USER CODE BEGIN Variables */
typedef  void (*pFunction)(void);
uint32_t JumpAddress;
pFunction Jump_To_Application;

void flash_download(void);
void sysreset_Jump(void);
void jumptoapplicaion(void);

static uint8_t RAM_Buf[BUFFER_SIZE] = {0x00};
/* USER CODE END Variables */    

void MX_FATFS_Init(void) 
{
  /*## FatFS: Link the SD driver ###########################*/
  retSD = FATFS_LinkDriver(&SD_Driver, SDPath);

  /* USER CODE BEGIN Init */
  flash_download();
  /* additional user code for init */     
  /* USER CODE END Init */
}

/**
  * @brief  Gets Time from RTC 
  * @param  None
  * @retval Time in DWORD
  */
DWORD get_fattime(void)
{
  /* USER CODE BEGIN get_fattime */
  return 0;
  /* USER CODE END get_fattime */  
}

/* USER CODE BEGIN Application */
void flash_program(void) {
	DWORD addr = APPLICATION_ADDRESS;
	UINT bytesread;
	DWORD length = f_size(&SDFile);
	uint16_t block_size = length/BUFFER_SIZE;
	uint16_t block_rem  = length % BUFFER_SIZE;
	uint32_t *ptr;
	if (block_rem)
		block_size += 1;
	for (int i=0; i < block_size; i++) {
		f_read (&SDFile, RAM_Buf, BUFFER_SIZE, (void *)&bytesread);
		ptr = (uint32_t *)RAM_Buf;
		for (int j=0; j < bytesread/4; j++) {
			FLASH_If_Write(addr, *ptr++);
			addr+=4;
		}
	}
}
void flash_download(void) {
	//open file from sdcard
	if(f_mount(&SDFatFS, (TCHAR const*)"",0) == FR_OK) {
		if(f_open(&SDFile, DOWNLOAD_FILENAME, FA_OPEN_EXISTING | FA_READ) == FR_OK) {

			FLASH_If_FlashUnlock();							//unlock flash ready to program.
			FLASH_If_EraseSectors(APPLICATION_ADDRESS);		//erase the application code.
			flash_program();								//program the flash
			f_close(&SDFile);								//close sd card
			FLASH_If_Flashlock();							//unlock the flash
			//sysreset_Jump();							//jump to applicion
			jumptoapplicaion();
		}
	}
}

void sysreset_Jump(void)
{
  /* Software reset */
  NVIC_SystemReset();
}

void jumptoapplicaion(void) {
    /* Jump to user application */
	HAL_RCC_DeInit();
	SysTick->CTRL 	= 0;
	SysTick->LOAD 	= 0;
	SysTick->VAL	= 0;

	__set_PRIMASK(1);

    JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);
    Jump_To_Application = (pFunction) JumpAddress;
    /* Initialize user application's Stack Pointer */
    __set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
    Jump_To_Application();
}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
