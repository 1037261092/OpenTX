/**
  ******************************************************************************
  * @file    Project/ee_drivers/i2c.c
  * @author  FrSky Application Team
  * @version V 0.2
  * @date    12-JULY-2012
  * @brief   This file provides a set of functions needed to manage the
  *          communication between I2C peripheral and I2C M24CXX EEPROM.
             CAT5137 added,share the I2C.
  ******************************************************************************
*/

#include "board_taranis.h"

#define EE_CMD_WRITE  (0)
#define EE_CMD_READ   (1)

#define SCL_H         do{I2C_EE_GPIO->BSRRL = I2C_EE_SCL;}while(0)
#define SCL_L         do{I2C_EE_GPIO->BSRRH  = I2C_EE_SCL;}while(0)
#define SDA_H         do{I2C_EE_GPIO->BSRRL = I2C_EE_SDA;}while(0)
#define SDA_L         do{I2C_EE_GPIO->BSRRH  = I2C_EE_SDA;}while(0)
#define SCL_read      (I2C_EE_GPIO->IDR  & I2C_EE_SCL)
#define SDA_read      (I2C_EE_GPIO->IDR  & I2C_EE_SDA)
#define WP_H          do{I2C_EE_WP_GPIO->BSRRL = I2C_EE_WP;}while(0)
#define WP_L          do{I2C_EE_WP_GPIO->BSRRH = I2C_EE_WP;}while(0)

/* Exported functions ------------------------------------------------------- */
void I2C_EE_ByteWrite(uint8_t* pBuffer, uint16_t WriteAddr);
void I2C_EE_PageWrite(uint8_t* pBuffer, uint16_t WriteAddr, uint8_t NumByteToWrite);
void I2C_EE_WaitEepromStandbyState(void);

void I2C_set_volume(register uint8_t volume);
uint8_t I2C_read_volume(void);

#define	I2C_delay()   delay_01us(100);

/**
  * @brief  Configure the used I/O ports pin
  * @param  None
  * @retval None
  */
static void I2C_GPIO_Configuration(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  GPIOB->BSRRH =I2C_EE_WP;	//PB9
  GPIO_InitStructure.GPIO_Pin = I2C_EE_WP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(I2C_EE_WP_GPIO, &GPIO_InitStructure);

  /* Configure I2C_EE pins: SCL and SDA */
  GPIO_InitStructure.GPIO_Pin =  I2C_EE_SCL | I2C_EE_SDA;//PE0,PE1
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(I2C_EE_GPIO, &GPIO_InitStructure);
  
  //Set Idle levels
  SDA_H;
  SCL_H;
}

short I2C_START(void)
{
  SDA_H;
  I2C_delay();
  SCL_H;
  I2C_delay();
  // if (!SDA_read) return 0;
  SDA_L;
  I2C_delay();
  // if (SDA_read) return 0;
  SCL_L;
  I2C_delay();
  return 1;
}

void I2C_STOP(void)
{
  SCL_L;
  I2C_delay();
  SDA_L;
  I2C_delay();
  SCL_H;
  I2C_delay();
  SDA_H;
  I2C_delay();
}

void I2C_ACK(void)
{
  SCL_L;
  I2C_delay();
  SDA_L;
  I2C_delay();
  SCL_H;
  I2C_delay();
  SCL_L;
  I2C_delay();
}

void I2C_NO_ACK(void)
{
  SCL_L;
  I2C_delay();
  SDA_H;
  I2C_delay();
  SCL_H;
  I2C_delay();
  SCL_L;
  I2C_delay();
}

short I2C_WAIT_ACK(void)
{
  short i=50;
  SCL_L;
  I2C_delay();
  SDA_H;
  I2C_delay();
  SCL_H;
  I2C_delay();
  while (i) {
    if(SDA_read) {
      I2C_delay();
      i--;
    }
    else {
      i=2;
      break;
    }
  }
  SCL_L;
  I2C_delay();

  return i;
} 

void I2C_SEND_DATA(char SendByte)
{
  short i=8;
  while (i--) {
    SCL_L;
    // I2C_delay();
    if (SendByte & 0x80)
      SDA_H;
    else
      SDA_L;
    SendByte <<= 1;
    I2C_delay();
    SCL_H;
    I2C_delay();
  }
  SCL_L;
  I2C_delay();
}

char I2C_READ(void)
{ 
  short i=8;
  char ReceiveByte=0;

  SDA_H;
  while (i--) {
    ReceiveByte <<= 1;
    SCL_L;
    I2C_delay();
    SCL_H;
    I2C_delay();
    if (SDA_read) {
      ReceiveByte|=0x01;
    }
  }
  SCL_L;
  return ReceiveByte;
} 

void eepromInit(void)
{
  /* GPIO Periph clock enable */
  RCC_AHB1PeriphClockCmd(I2C_EE_GPIO_CLK, ENABLE);

  /* GPIO configuration */
  I2C_GPIO_Configuration();
}

/**
  * @brief  Writes one byte to the I2C EEPROM.
  * @param  pBuffer : pointer to the buffer  containing the data to be
  *   written to the EEPROM.
  * @param  WriteAddr : EEPROM's internal address to write to.
  * @retval None
  */
void I2C_EE_ByteWrite(uint8_t* pBuffer, uint16_t WriteAddr)
{
  I2C_START();
  I2C_SEND_DATA(I2C_EEPROM_ADDRESS|EE_CMD_WRITE);
  I2C_WAIT_ACK();
#ifdef EE_M24C08
  I2C_SEND_DATA(WriteAddr);
  I2C_WAIT_ACK();
#else
  I2C_SEND_DATA((uint8_t)((WriteAddr&0xFF00)>>8) );
  I2C_WAIT_ACK();
  I2C_SEND_DATA((uint8_t)(WriteAddr&0xFF));
  I2C_WAIT_ACK();
#endif
  I2C_SEND_DATA(*pBuffer);
  I2C_WAIT_ACK();
  I2C_STOP();
}

/**
  * @brief  Reads a block of data from the EEPROM.
  * @param  pBuffer : pointer to the buffer that receives the data read
  *   from the EEPROM.
  * @param  ReadAddr : EEPROM's internal address to read from.
  * @param  NumByteToRead : number of bytes to read from the EEPROM.
  * @retval None
  */
void eepromReadBlock(uint8_t* pBuffer, uint16_t ReadAddr, uint16_t NumByteToRead)
{
  I2C_START();
  I2C_SEND_DATA(I2C_EEPROM_ADDRESS|EE_CMD_WRITE);
  I2C_WAIT_ACK();
#ifdef EE_M24C08
  I2C_SEND_DATA(ReadAddr);
  I2C_WAIT_ACK();
#else
  I2C_SEND_DATA((uint8_t)((ReadAddr & 0xFF00) >> 8));
  I2C_WAIT_ACK();
  I2C_SEND_DATA((uint8_t)(ReadAddr & 0x00FF));
  I2C_WAIT_ACK();
#endif
	
  I2C_START();
  I2C_SEND_DATA(I2C_EEPROM_ADDRESS|EE_CMD_READ);
  I2C_WAIT_ACK();
  while (NumByteToRead) {
    if (NumByteToRead == 1) {
      *pBuffer =I2C_READ();
      I2C_NO_ACK();
      I2C_STOP();
    }
    else {
      *pBuffer =I2C_READ();
      I2C_ACK();
      pBuffer++;
    }
    NumByteToRead--;
  }
}

/**
  * @brief  Writes buffer of data to the I2C EEPROM.
  * @param  pBuffer : pointer to the buffer  containing the data to be
  *   written to the EEPROM.
  * @param  WriteAddr : EEPROM's internal address to write to.
  * @param  NumByteToWrite : number of bytes to write to the EEPROM.
  * @retval None
  */
void eepromWriteBlock(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite)
{
  uint8_t NumOfPage = 0, NumOfSingle = 0, count = 0;
  uint16_t Addr = 0;

  Addr = WriteAddr % I2C_FLASH_PAGESIZE;
  count = I2C_FLASH_PAGESIZE - Addr;
  NumOfPage =  NumByteToWrite / I2C_FLASH_PAGESIZE;
  NumOfSingle = NumByteToWrite % I2C_FLASH_PAGESIZE;

  /* If WriteAddr is I2C_FLASH_PAGESIZE aligned  */
  if (Addr == 0) {
    /* If NumByteToWrite < I2C_FLASH_PAGESIZE */
    if (NumOfPage == 0) {
      I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);

      I2C_EE_WaitEepromStandbyState();
    }
    /* If NumByteToWrite > I2C_FLASH_PAGESIZE */
    else {
      while (NumOfPage--) {
        I2C_EE_PageWrite(pBuffer, WriteAddr, I2C_FLASH_PAGESIZE);
        I2C_EE_WaitEepromStandbyState();
        WriteAddr += I2C_FLASH_PAGESIZE;
        pBuffer += I2C_FLASH_PAGESIZE;
      }

      if (NumOfSingle != 0) {
        I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
        I2C_EE_WaitEepromStandbyState();
      }
    }
  }
  /* If WriteAddr is not I2C_FLASH_PAGESIZE aligned  */
  else {
    /* If NumByteToWrite < I2C_FLASH_PAGESIZE */
    if (NumOfPage== 0) {
      /* If the number of data to be written is more than the remaining space
      in the current page: */
      if (NumByteToWrite > count) {
        /* Write the data conained in same page */
        I2C_EE_PageWrite(pBuffer, WriteAddr, count);
        I2C_EE_WaitEepromStandbyState();

        /* Write the remaining data in the following page */
        I2C_EE_PageWrite((uint8_t*)(pBuffer + count), (WriteAddr + count), (NumByteToWrite - count));
        I2C_EE_WaitEepromStandbyState();
      }
      else {
        I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
        I2C_EE_WaitEepromStandbyState();
      }
    }
    /* If NumByteToWrite > I2C_FLASH_PAGESIZE */
    else {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / I2C_FLASH_PAGESIZE;
      NumOfSingle = NumByteToWrite % I2C_FLASH_PAGESIZE;

      if (count != 0) {
        I2C_EE_PageWrite(pBuffer, WriteAddr, count);
        I2C_EE_WaitEepromStandbyState();
        WriteAddr += count;
        pBuffer += count;
      }

      while (NumOfPage--) {
        I2C_EE_PageWrite(pBuffer, WriteAddr, I2C_FLASH_PAGESIZE);
        I2C_EE_WaitEepromStandbyState();
        WriteAddr +=  I2C_FLASH_PAGESIZE;
        pBuffer += I2C_FLASH_PAGESIZE;
      }
      if (NumOfSingle != 0) {
        I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
        I2C_EE_WaitEepromStandbyState();
      }
    }
  }
}

/**
  * @brief  Writes more than one byte to the EEPROM with a single WRITE cycle.
  * @note   The number of byte can't exceed the EEPROM page size.
  * @param  pBuffer : pointer to the buffer containing the data to be
  *   written to the EEPROM.
  * @param  WriteAddr : EEPROM's internal address to write to.
  * @param  NumByteToWrite : number of bytes to write to the EEPROM.
  * @retval None
  */
void I2C_EE_PageWrite(uint8_t* pBuffer, uint16_t WriteAddr, uint8_t NumByteToWrite)
{
  I2C_START();
  I2C_SEND_DATA(I2C_EEPROM_ADDRESS|EE_CMD_WRITE);
  I2C_WAIT_ACK();

#ifdef EE_M24C08
  I2C_SEND_DATA(WriteAddr);
  I2C_WAIT_ACK();
#else
  I2C_SEND_DATA((uint8_t)((WriteAddr & 0xFF00) >> 8));
  I2C_WAIT_ACK();
  I2C_SEND_DATA((uint8_t)(WriteAddr & 0x00FF));
  I2C_WAIT_ACK();
#endif /* EE_M24C08 */

  /* While there is data to be written */
  while (NumByteToWrite--) {
    I2C_SEND_DATA(*pBuffer);
    I2C_WAIT_ACK();
    pBuffer++;
  }
  I2C_STOP();
}

/**
  * @brief  Wait for EEPROM Standby state
  * @param  None
  * @retval None
  */
void I2C_EE_WaitEepromStandbyState(void)
{
  do {
    I2C_START();
    I2C_SEND_DATA(I2C_EEPROM_ADDRESS|EE_CMD_WRITE);
  } while (0 == I2C_WAIT_ACK());

  I2C_STOP();
}

void setVolume(uint8_t volume)
{
  if (volume > VOLUME_LEVEL_MAX)
    volume = VOLUME_LEVEL_MAX;

  I2C_START();
  I2C_SEND_DATA(I2C_CAT5137_ADDRESS|EE_CMD_WRITE);
  I2C_WAIT_ACK();
  I2C_SEND_DATA(0);
  I2C_WAIT_ACK();
  I2C_SEND_DATA(volume);
  I2C_WAIT_ACK();
  I2C_STOP();
}

uint8_t I2C_read_volume()
{
  uint8_t volume ;
  I2C_START();
  I2C_SEND_DATA(I2C_CAT5137_ADDRESS|EE_CMD_WRITE);
  I2C_WAIT_ACK();
  I2C_SEND_DATA(0);
  I2C_WAIT_ACK();
  I2C_START();
  I2C_SEND_DATA(I2C_CAT5137_ADDRESS|EE_CMD_READ);
  I2C_WAIT_ACK();
  volume = I2C_READ();
  I2C_NO_ACK();
  I2C_STOP();
  return volume ;
}
