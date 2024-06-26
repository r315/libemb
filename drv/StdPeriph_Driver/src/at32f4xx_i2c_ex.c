/**
  ******************************************************************************
  * File   : I2C/Poll_MA_TX&SLA_RX/at32f4xx_i2c_ex.c 
  * Version: V1.2.4
  * Date   : 2020-08-26
  * Brief  : The driver of the I2C communication example based on polling mode.
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"
#include "at32f4xx_i2c_ex.h"

/** @addtogroup AT32F4xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup Polling - Master Transmitter & Slave Receiver
  * @brief  The communication is performed in the polling mode.The status of 
  *         all data processing is returned by the same function after 
  *         finishing transfer.
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/** 
  * @brief  I2C Timeout definition  
  */  
#define I2C_TIMEOUT_FLAG          ((uint32_t)35)     /* Timeout 35 ms */
#define I2C_TIMEOUT_ADDR_SLAVE    ((uint32_t)1000)  /* Timeout 1s  */
#define I2C_TIMEOUT_BUSY_FLAG     ((uint32_t)1000)  /* Timeout 1s  */

/** 
  * @brief  I2C Event check flag definition  
  */  
#define I2C_EVT_CHECK_NONE                   ((uint32_t)0x00000000)
#define I2C_EVT_CHECK_ACKFAIL                ((uint32_t)0x00000001)
#define I2C_EVT_CHECK_STOP                   ((uint32_t)0x00000002)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

void DelayMs(uint32_t Ms);

/** @brief  Clears the I2C ADDR pending flag.
  * @param  I2Cx: where x can be 1, 2 or 3 to select the I2C peripheral.
  * @retval None
  */
void I2C_ClearADDRFlag(I2C_Type* I2Cx)
{
  volatile uint32_t tmpreg;  
  
  tmpreg = I2Cx->STS1; 
  
  tmpreg = I2Cx->STS2;
  
  (void)tmpreg;
}  


/** @brief  Clears the I2C STOPF pending flag.
  * @param  I2Cx: where x can be 1, 2 or 3 to select the I2C peripheral.
  * @retval None
  */
void I2C_ClearSTOPFlag(I2C_Type* I2Cx)
{
  volatile uint32_t tmpreg;  
                          
  tmpreg = I2Cx->STS1;
  
  tmpreg = I2Cx->CTRL1 |= I2C_CTRL1_PEN;

  (void)tmpreg;
}  

/**
  * @brief  This function handles I2C Communication Timeout.
  * @param  I2Cx: where x can be 1, 2 or 3 to select the I2C peripheral.
  * @param  Flag: specifies the I2C flag to check.
  * @param  Status: The new Flag status (SET or RESET).
  * @param  Timeout Timeout duration.
  * @retval I2C status.
  */
static I2C_StatusType I2C_WaitOnFlagUntilTimeout(I2C_Type* I2Cx, uint32_t Flag, FlagStatus Status, uint32_t EventCheck, uint32_t Timeout)
{
  /* delay 10 us = ms * 100 */
  Timeout *= 10;
  
  while(I2C_GetFlagStatus(I2Cx, Flag) == Status)
  {
    /* Check ACKFAIL Flag */
    if(EventCheck & I2C_EVT_CHECK_ACKFAIL)
    {
      if(I2C_GetFlagStatus(I2Cx, I2C_FLAG_ACKFAIL) == SET)
      {
        /* Send STOP Condition */
        I2C_GenerateSTOP(I2Cx, ENABLE);

        /* Clear AF Flag */
        I2C_ClearFlag(I2Cx, I2C_FLAG_ACKFAIL);
        
        return I2C_ERROR;
      }    
    }

    /* Check STOP Flag */
    if(EventCheck & I2C_EVT_CHECK_STOP)
    {
      if(I2C_GetFlagStatus(I2Cx, I2C_FLAG_STOPF) == SET)
      {
        /* Clear STOP Flag */
        I2C_ClearSTOPFlag(I2Cx);

        return I2C_ERROR;
      }    
    }    

    /* 10 us delay */
    for(int i = 0; i < SystemCoreClock / 10000000UL; i++){
        asm("nop");
    }
    
    /* Check for the Timeout */
    if((Timeout--) == 0)
    {
      return I2C_TIMEOUT;
    }
  }
  
  return I2C_OK;
}

/**
  * @brief  Helper for generate start condition and send device address
  * @param  I2Cx: where x can be 1, 2 or 3 to select the I2C peripheral.
  * @param  DevAddress Target device address.
  * @param  I2C_Direction Direction Transmit/Receive
  * @param  Timeout Timeout duration.
  * @retval I2C status.
  */
static I2C_StatusType I2C_Start_Transfer(I2C_Type  *I2Cx, uint16_t DevAddress, uint8_t I2C_Direction, uint32_t Timeout)
{
   /* Send START condition */
   I2C_GenerateSTART(I2Cx, ENABLE);
   /* Wait until SB flag is set */
   if(I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_STARTF, RESET, I2C_EVT_CHECK_NONE, Timeout) != I2C_OK)
   {
      /* Send STOP Condition */
      I2C_GenerateSTOP(I2Cx, ENABLE);
      return I2C_ERROR_STEP_2;
   }
   /* Send slave address for write */
   I2C_Send7bitAddress(I2Cx, DevAddress, I2C_Direction);
   /* Wait until ADDR flag is set */
   if(I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_ADDRF, RESET, I2C_EVT_CHECK_ACKFAIL, Timeout) != I2C_OK)
   {
      /* Send STOP Condition */
      I2C_GenerateSTOP(I2Cx, ENABLE);
      return I2C_ERROR_STEP_3;
   }
   /* Clear ADDR flag */
   I2C_ClearADDRFlag(I2Cx);
   return I2C_OK;
}

#if 0
/**
  * @brief  Initializes peripherals used by the I2C.
  * @param  None
  * @retval None
  */
void LowLevel_Init(void)
{
  GPIO_InitType GPIO_InitStructure;
    
  /* I2C_SCL_GPIO_CLK and I2C_SDA_GPIO_CLK Periph clock enable */
  RCC_APB2PeriphClockCmd(I2C_SCL_GPIO_CLK | I2C_SDA_GPIO_CLK, ENABLE);

  /* I2C Periph clock enable */
  RCC_APB1PeriphClockCmd(I2C_CLK, ENABLE);
    
  /* GPIO configuration */  
  /* Configure I2C pins: SCL */
  GPIO_InitStructure.GPIO_Pins = I2C_SCL_PIN;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_Init(I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

  /* Configure I2C pins: SDA */
  GPIO_InitStructure.GPIO_Pins = I2C_SDA_PIN;
  GPIO_Init(I2C_SDA_GPIO_PORT, &GPIO_InitStructure); 
}
#endif 
/**
  * @brief  Initializes peripherals used by the I2C EEPROM driver.
  * @param  None.
  * @retval None.
  */
void I2Cx_Init(I2C_Type* I2Cx)
{ 
  I2C_InitType  I2C_InitStructure;
  
  //LowLevel_Init();
  
  /* I2C configuration */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2CDevice;
  I2C_InitStructure.I2C_FmDutyCycle = I2C_FmDutyCycle_2_1;
  I2C_InitStructure.I2C_OwnAddr1 = I2C_SLAVE_ADDRESS7;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AddrMode = I2C_AddrMode_7bit;
  I2C_InitStructure.I2C_BitRate = I2C_SPEED;
  
  /* I2C Peripheral Enable */
  I2C_Cmd(I2Cx, ENABLE);
  
  /* Apply I2C configuration after enabling it */
  I2C_Initialize(I2Cx, &I2C_InitStructure);
}

/**
  * @brief  Transmits in master mode an amount of data in blocking mode.
  * @param  I2Cx: where x can be 1, 2 or 3 to select the I2C peripheral.
  * @param  DevAddress Target device address.
  * @param  pData Pointer to data buffer.
  * @param  Size Amount of data to be sent.
  * @param  Timeout Timeout duration.
  * @retval I2C status.
  */
I2C_StatusType I2C_Master_Transmit(I2C_Type* I2Cx, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  /* Wait until BUSY flag is reset */
  if(I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_BUSYF, SET, I2C_EVT_CHECK_NONE, I2C_TIMEOUT_BUSY_FLAG) != I2C_OK)
  {
    return I2C_ERROR_STEP_1;
  }

  /* Disable Pos */
  I2C_NACKPositionConfig(I2Cx, I2C_NACKPosition_Current);
  
  /* Send START condition */
  I2C_GenerateSTART(I2Cx, ENABLE);

  /* Wait until SB flag is set */
  if(I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_STARTF, RESET, I2C_EVT_CHECK_NONE, Timeout) != I2C_OK)
  {
    /* Send STOP Condition */
    I2C_GenerateSTOP(I2Cx, ENABLE);
    
    return I2C_ERROR_STEP_2;
  }

  /* Send slave address for write */
  I2C_Send7bitAddress(I2Cx, DevAddress, I2C_Direction_Transmit);
  
  /* Wait until ADDR flag is set */
  if(I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_ADDRF, RESET, I2C_EVT_CHECK_ACKFAIL, Timeout) != I2C_OK)
  {
    /* Send STOP Condition */
    I2C_GenerateSTOP(I2Cx, ENABLE);
    
    return I2C_ERROR_STEP_3;
  }
  
  /* Clear ADDR flag */
  I2C_ClearADDRFlag(I2Cx);  

  while(Size > 0)
  {
    /* Wait until TDE flag is set */
    if(I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_TDE, RESET, I2C_EVT_CHECK_ACKFAIL, Timeout) != I2C_OK)
    {
      /* Send STOP Condition */
      I2C_GenerateSTOP(I2Cx, ENABLE);
      
      return I2C_ERROR_STEP_4;
    }

    /* Write data to DR */
    I2C_SendData(I2Cx, (*pData++));
    Size--;
  }

  /* Wait until BTF flag is set */
  if(I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_BTFF, RESET, I2C_EVT_CHECK_ACKFAIL, Timeout) != I2C_OK)
  {
    /* Send STOP Condition */
    I2C_GenerateSTOP(I2Cx, ENABLE);
    
    return I2C_ERROR_STEP_5;
  }  
  
  /* Send STOP Condition */
  I2C_GenerateSTOP(I2Cx, ENABLE);

  return I2C_OK;
}

/**
  * @brief  Read an amount of data in blocking mode from a specific memory address
  * @param  I2Cx: where x can be 1, 2 or 3 to select the I2C peripheral.
  * @param  DevAddress Target device address.
  * @param  MemAddress Target internal memory address.
  * @param  pData Pointer to data buffer.
  * @param  Size Amount of data to be read.
  * @param  Timeout Timeout duration.
  * @retval I2C status.
  */
I2C_StatusType I2C_Master_Receive(I2C_Type  *I2Cx, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
   /* Wait until BUSY flag is reset */
   if(I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_BUSYF, SET, I2C_EVT_CHECK_NONE, I2C_TIMEOUT_BUSY_FLAG) != I2C_OK)
   {
      return I2C_ERROR_STEP_1;
   }

   /* Disable Pos */
   I2C_NACKPositionConfig(I2Cx, I2C_NACKPosition_Current);
   
   /* Read data from device */
   if(I2C_Start_Transfer(I2Cx, DevAddress, I2C_Direction_Receive, Timeout) != I2C_OK)
   {
      return I2C_ERROR_STEP_5;
   }
   while(Size > 0)
   {
      Size--;
      if(Size > 0)
      {
         I2C_AcknowledgeConfig(I2Cx, ENABLE);
      }
      else
      {
         I2C_AcknowledgeConfig(I2Cx, DISABLE);
         /* Send STOP Condition, will be generated after NAK */
         I2C_GenerateSTOP(I2Cx, ENABLE);
      }
      /* Wait until RDNE flag is set */
      if(I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_RDNE, RESET, I2C_EVT_CHECK_STOP, Timeout) != I2C_OK)
      {
         /* Disable Address Acknowledge */
         I2C_AcknowledgeConfig(I2Cx, DISABLE);
         return I2C_ERROR_STEP_6;
      }
      /* Read data from DR */
      (*pData++) = I2C_ReceiveData(I2Cx);
   }
   return I2C_OK;
}

/**
  * @brief  Receive in slave mode an amount of data in blocking mode.
  * @param  I2Cx: where x can be 1, 2 or 3 to select the I2C peripheral.
  * @param  pData Pointer to data buffer.
  * @param  Size Amount of data to be sent.
  * @param  Timeout Timeout duration.
  * @retval I2C status.
  */
I2C_StatusType I2C_Slave_Receive(I2C_Type* I2Cx, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  /* Wait until BUSY flag is reset */
  if(I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_BUSYF, SET, I2C_EVT_CHECK_NONE, I2C_TIMEOUT_BUSY_FLAG) != I2C_OK)
  {
    return I2C_ERROR_STEP_1;
  }
  
  /* Disable Pos */
  I2C_NACKPositionConfig(I2Cx, I2C_NACKPosition_Current);
  
  /* Enable Address Acknowledge */
  I2C_AcknowledgeConfig(I2Cx, ENABLE); 
 
  /* Wait until ADDR flag is set */
  if(I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_ADDRF, RESET, I2C_EVT_CHECK_NONE, Timeout) != I2C_OK)    
  {
    return I2C_ERROR_STEP_2;
  }
 
  /* Clear ADDR flag */
  I2C_ClearADDRFlag(I2Cx);  

  while(Size > 0)
  {
    /* Wait until RXNE flag is set */
    if(I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_RDNE, RESET, I2C_EVT_CHECK_STOP, Timeout) != I2C_OK)  
    {
      /* Disable Address Acknowledge */
      I2C_AcknowledgeConfig(I2Cx, DISABLE);
      
      return I2C_ERROR_STEP_3;
    }

    /* Read data from DR */
    (*pData++) = I2C_ReceiveData(I2Cx);
    Size--;
  }

  /* Wait until STOP flag is set */
  if(I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_STOPF, RESET, I2C_EVT_CHECK_ACKFAIL, I2C_TIMEOUT_FLAG) != I2C_OK)    
  {
    /* Disable Address Acknowledge */
    I2C_AcknowledgeConfig(I2Cx, DISABLE);

    return I2C_ERROR_STEP_4;
  }

  /* Clear STOP flag */
  I2C_ClearSTOPFlag(I2Cx);

  /* Disable Address Acknowledge */
  I2C_AcknowledgeConfig(I2Cx, DISABLE);

  return I2C_OK;
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT Artery Technology *****END OF FILE****/
