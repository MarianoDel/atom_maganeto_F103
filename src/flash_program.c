/**
  *****************************************************************************
  * @title   FLASH_Program.c
  * @author  CooCox
  * @date    31 Oct 2012
  * @brief   This example provides a description of how to program the STM32F
  *          10x FLASH.
  *          the result of the programming operation is stored in
  *          to the MemoryProgramStatus variable.
  *          Four LEDs are toggled if t
  *          he flash program has been down.
  *          This example has been tested on
  *           KEIL MCBSTM32 board, STM32F103RBT6 device.
  *******************************************************************************
  */
////// The above comment is automatically generated by CoIDE ///////////////////
#include "flash_program.h"
// #include "stm32f10x_flash.h"
#include "stm32f10x.h"
#include "GTK_Hard.h"
#include "answers_defs.h"

//New buffer.
//es un sizeof(session_slot_aux) * 5 >> 2 (porque es un int)
unsigned int auxBuffer_flash [((sizeof(session_typedef) >> 2) * 5)];

extern session_typedef session_slot_aux;

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
/* Define the STM32F10x FLASH Page Size depending on the used STM32 device */
#if defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_CL) || defined (STM32F10X_XL)
  #define FLASH_PAGE_SIZE    ((uint16_t)0x800)
#else
  #define FLASH_PAGE_SIZE    ((uint16_t)0x400)
#endif

#define PAGE_NUM_SELECTED	126
#define BANK1_WRITE_START_ADDR  ((uint32_t)0x08000000 + FLASH_PAGE_SIZE * PAGE_NUM_SELECTED)
#define BANK1_WRITE_END_ADDR    (BANK1_WRITE_START_ADDR + FLASH_PAGE_SIZE)

//#define BANK1_WRITE_START_ADDR  ((uint32_t)0x08060000)
//#define BANK1_WRITE_END_ADDR    ((uint32_t)0x08060800)

#ifdef STM32F10X_XL
  #define BANK2_WRITE_START_ADDR   ((uint32_t)0x08088000)
  #define BANK2_WRITE_END_ADDR     ((uint32_t)0x0808C000)
#endif /* STM32F10X_XL */
/* Private macro -------------------------------------------------------------*/
/** @defgroup FLASH_Private_Defines
  * @{
  */ 

/* Flash Access Control Register bits */
#define ACR_LATENCY_Mask         ((uint32_t)0x00000038)
#define ACR_HLFCYA_Mask          ((uint32_t)0xFFFFFFF7)
#define ACR_PRFTBE_Mask          ((uint32_t)0xFFFFFFEF)

/* Flash Access Control Register bits */
#define ACR_PRFTBS_Mask          ((uint32_t)0x00000020) 

/* Flash Control Register bits */
#define CR_PG_Set                ((uint32_t)0x00000001)
#define CR_PG_Reset              ((uint32_t)0x00001FFE) 
#define CR_PER_Set               ((uint32_t)0x00000002)
#define CR_PER_Reset             ((uint32_t)0x00001FFD)
#define CR_MER_Set               ((uint32_t)0x00000004)
#define CR_MER_Reset             ((uint32_t)0x00001FFB)
#define CR_OPTPG_Set             ((uint32_t)0x00000010)
#define CR_OPTPG_Reset           ((uint32_t)0x00001FEF)
#define CR_OPTER_Set             ((uint32_t)0x00000020)
#define CR_OPTER_Reset           ((uint32_t)0x00001FDF)
#define CR_STRT_Set              ((uint32_t)0x00000040)
#define CR_LOCK_Set              ((uint32_t)0x00000080)

/* FLASH Mask */
#define RDPRT_Mask               ((uint32_t)0x00000002)
#define WRP0_Mask                ((uint32_t)0x000000FF)
#define WRP1_Mask                ((uint32_t)0x0000FF00)
#define WRP2_Mask                ((uint32_t)0x00FF0000)
#define WRP3_Mask                ((uint32_t)0xFF000000)
#define OB_USER_BFB2             ((uint16_t)0x0008)

/* FLASH Keys */
#define RDP_Key                  ((uint16_t)0x00A5)
#define FLASH_KEY1               ((uint32_t)0x45670123)
#define FLASH_KEY2               ((uint32_t)0xCDEF89AB)

/* FLASH BANK address */
#define FLASH_BANK1_END_ADDRESS   ((uint32_t)0x807FFFF)

/* Delay definition */   
#define EraseTimeout          ((uint32_t)0x000B0000)
#define ProgramTimeout        ((uint32_t)0x00002000)
/**
  * @}
  */ 

/* Private variables ---------------------------------------------------------*/
uint32_t EraseCounter = 0x00, Address = 0x00;
uint32_t Data = 0x3210ABCD;
uint32_t NbrOfPage = 0x00;
volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;
volatile TestStatus MemoryProgramStatus = PASSED;

#ifdef STM32F10X_XL
volatile TestStatus MemoryProgramStatus2 = PASSED;
#endif /* STM32F10X_XL */

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief    After Reset, the Flash memory Program/Erase Controller is locked.
  * To unlock it, the FLASH_Unlock function is used. Before programming the
  * desired addresses, an erase operation is performed using the flash erase
  * page feature. The erase procedure starts with the calculation of the number
  * of pages to be used. Then all these pages will be erased one by one by
  * calling FLASH_ErasePage function.
  * Once this operation is finished, the programming operation will be performed by
  * using the FLASH_ProgramWord function. The written data is then checked and the
  * result of the programming operation is stored into the MemoryProgramStatus variable.
  * @param  None
  * @retval None
  */
char FLASH_Program(session_typedef * pslot, unsigned char slot)
{
	unsigned char i = 0;
	unsigned short slots_size_in_bytes;

    /*!< At this stage the microcontroller clock setting is already configured,
    this is done through SystemInit() function which is called from startup
    file (startup_stm32f10x_md.c) before to branch to application main.
    To reconfigure the default setting of SystemInit() function, refer to
    system_stm32f10x.c file
    */
	//char i;
    /* Initialize Leds mounted on MCBSTM32 board */
    //GPIO_InitTypeDef  GPIO_InitStructure;
    /* Initialize LED which connected to PB8,PB9,PB10,PB11, Enable the Clock*/
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    /* Configure the GPIO_LED pin */
 /*   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
*/
/* Porgram FLASH Bank1 ********************************************************/
    /* Unlock the Flash Bank1 Program Erase controller */
    FLASH_UnlockBank1();

    /* Define the number of page to be erased */
    NbrOfPage = (BANK1_WRITE_END_ADDR - BANK1_WRITE_START_ADDR) / FLASH_PAGE_SIZE;

    /* Clear All pending flags */
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

    /* Erase the FLASH pages */
    Address = BANK1_WRITE_START_ADDR;

    //Leo y guardo en un vector lo actual de la memoria
    slots_size_in_bytes = sizeof(session_slot_aux) * 5;		//de los 5 slots en memoria
    slots_size_in_bytes = slots_size_in_bytes >> 2;			//lo paso int PUDE HBER UN ERROR EN SLOT 5 pero nunc se usa

    for (i = 0; i < slots_size_in_bytes; i++)
    {
    	auxBuffer_flash[i] = *(__IO uint32_t*) Address;
    	Address += 4;
    }

    //borro la pagina completa de 2Kbyes
    for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
    {
        FLASHStatus = FLASH_ErasePage(BANK1_WRITE_START_ADDR + (FLASH_PAGE_SIZE * EraseCounter));
    }

    //ajusto la direccion y meto la nueva info
    Address = BANK1_WRITE_START_ADDR;
    saveData2(&auxBuffer_flash[0], pslot, BANK1_WRITE_START_ADDR, slot);

    FLASH_LockBank1();

    checkData2(&auxBuffer_flash[0], BANK1_WRITE_START_ADDR);

    if( MemoryProgramStatus == FAILED)
    {
        //Error en el grabado.
    	return FIN_ERROR;
    }

    //Grabado OK.
    return FIN_OK;
}



#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  while (1)
  {
  }
}
#endif
void saveData2(unsigned int * pMemory, session_typedef * pslot, unsigned int start_addr, unsigned char slot)
{
	unsigned short slot_init;
	unsigned char * pslot_read;
	unsigned char * pslot_write;
	unsigned char i;

	unsigned short slots_size_in_bytes;

	slot_init = slot;
	slot_init -= 1;
	//modificcion 23-03-2015 TODO se puede poner cd slot en un bnco distinto y no usar las estructurs auxilires en RAM
	slot_init *= sizeof(session_slot_aux);

	pslot_read = (unsigned char *) pslot;
	pslot_write = ((unsigned char *)pMemory + slot_init);

    slots_size_in_bytes = sizeof(session_slot_aux);

	//guardo en l porcion de slot correspondiente los nuevos vlores dentro del vector originl de bkp
	for (i = 0; i < slots_size_in_bytes; i++)
	{
		*pslot_write = *pslot_read;

		pslot_write++;
		pslot_read++;
	}


	 Address = start_addr;
	 slots_size_in_bytes = sizeof(session_slot_aux) * 5;
	 slots_size_in_bytes = slots_size_in_bytes >> 2;		//pso  int

	 for (i = 0; i < slots_size_in_bytes; i++)
	 {
		 FLASHStatus = FLASH_ProgramWord(Address, auxBuffer_flash[i]);
		 Address += 4;
	 }
}
void saveData(char * pData, char fin, uint32_t addInicial)
{
	char i=0;

	//ptrSaveData = pData;

	Address = addInicial;
	i = 0;
	while (i < fin)
	{

		Data = (*(pData + i) << 24);
		i++;
		Data |= (*(pData + i) << 16);
		i++;
		Data |= (*(pData + i) << 8);
		i++;
		Data |= *(pData + i);
		i++;

		FLASHStatus = FLASH_ProgramWord(Address, Data);
		Address = Address + 4;
	}
}

void checkData2(unsigned int * pData, uint32_t addInicial)
{
	unsigned char i;
	unsigned short slots_size_in_bytes;

    Address = addInicial;

    slots_size_in_bytes = sizeof(session_slot_aux) * 5;		//de los 5 slots en memoria
    slots_size_in_bytes = slots_size_in_bytes >> 2;			//lo paso int PUDE HBER UN ERROR EN SLOT 5 pero nunc se usa

    for (i = 0; i < slots_size_in_bytes; i++)
    {
    	Data = *(__IO uint32_t*) Address;
    	Address += 4;

    	if (Data != auxBuffer_flash[i])
    	{
    		MemoryProgramStatus = FAILED;
    		i = slots_size_in_bytes;
    	}
    }
}

void checkData(char * pData, char fin, uint32_t addInicial)
{
	unsigned char i;
	unsigned int j;

    /* Check the correctness of written data */
    Address = addInicial;
    i = 0;
    while((Address < BANK1_WRITE_END_ADDR) && (MemoryProgramStatus != FAILED) && (i < fin))
    {
    	Data = *((__IO uint32_t*) Address); //Leo el dato.

    	j = (*(pData + i) << 24);
    	i++;
    	j |= (*(pData + i) << 16);
    	i++;
    	j |= (*(pData + i) << 8);
    	i++;
    	j |= *(pData + i);
    	i++;

    	if (Data != j)
    		MemoryProgramStatus = FAILED;

    	Address += 4;

    }
}

//void readDataFromFlash(char * pData, uint32_t addInicial)
unsigned char readDataFromFlash(session_typedef * pslot, unsigned char slot)
{
	unsigned short slot_init;
	unsigned char * ptr1, *ptr2;
	unsigned char i;
	unsigned short slots_size_in_bytes;

    slots_size_in_bytes = sizeof(session_slot_aux);

	slot_init = slot;
	slot_init -= 1;
	slot_init *= slots_size_in_bytes;

	ptr1 = (unsigned char *) pslot;
	ptr2 = (unsigned char *) (BANK1_WRITE_START_ADDR + slot_init);

	//while (ptr1 <= (((unsigned char *) &pslot->stage_3_burst_mode_off) + 1))
	for (i = 0; i < slots_size_in_bytes; i++)
	{
		*ptr1 = *ptr2;

		ptr1++;
		ptr2++;
	}

    return FIN_OK;
}

/**
  * @brief  Unlocks the FLASH Bank1 Program Erase Controller.
  * @note   This function can be used for all STM32F10x devices.
  *         - For STM32F10X_XL devices this function unlocks Bank1.
  *         - For all other devices it unlocks Bank1 and it is 
  *           equivalent to FLASH_Unlock function.
  * @param  None
  * @retval None
  */
void FLASH_UnlockBank1(void)
{
  /* Authorize the FPEC of Bank1 Access */
  FLASH->KEYR = FLASH_KEY1;
  FLASH->KEYR = FLASH_KEY2;
}

/**
  * @brief  Clears the FLASH's pending flags.
  * @note   This function can be used for all STM32F10x devices.
  *         - For STM32F10X_XL devices, this function clears Bank1 or Bank2�s pending flags
  *         - For other devices, it clears Bank1�s pending flags.
  * @param  FLASH_FLAG: specifies the FLASH flags to clear.
  *   This parameter can be any combination of the following values:         
  *     @arg FLASH_FLAG_PGERR: FLASH Program error flag       
  *     @arg FLASH_FLAG_WRPRTERR: FLASH Write protected error flag      
  *     @arg FLASH_FLAG_EOP: FLASH End of Operation flag           
  * @retval None
  */
void FLASH_ClearFlag(uint32_t FLASH_FLAG)
{
#ifdef STM32F10X_XL
  /* Check the parameters */
  assert_param(IS_FLASH_CLEAR_FLAG(FLASH_FLAG)) ;

  if((FLASH_FLAG & 0x80000000) != 0x0)
  {
    /* Clear the flags */
    FLASH->SR2 = FLASH_FLAG;
  }
  else
  {
    /* Clear the flags */
    FLASH->SR = FLASH_FLAG;
  }  

#else
  /* Check the parameters */
  // assert_param(IS_FLASH_CLEAR_FLAG(FLASH_FLAG)) ;
  
  /* Clear the flags */
  FLASH->SR = FLASH_FLAG;
#endif /* STM32F10X_XL */
}

/**
  * @brief  Erases a specified FLASH page.
  * @note   This function can be used for all STM32F10x devices.
  * @param  Page_Address: The page address to be erased.
  * @retval FLASH Status: The returned value can be: FLASH_BUSY, FLASH_ERROR_PG,
  *         FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
  */
FLASH_Status FLASH_ErasePage(uint32_t Page_Address)
{
  FLASH_Status status = FLASH_COMPLETE;
  /* Check the parameters */
  // assert_param(IS_FLASH_ADDRESS(Page_Address));

#ifdef STM32F10X_XL
  if(Page_Address < FLASH_BANK1_END_ADDRESS)  
  {
    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastBank1Operation(EraseTimeout);
    if(status == FLASH_COMPLETE)
    { 
      /* if the previous operation is completed, proceed to erase the page */
      FLASH->CR|= CR_PER_Set;
      FLASH->AR = Page_Address; 
      FLASH->CR|= CR_STRT_Set;
    
      /* Wait for last operation to be completed */
      status = FLASH_WaitForLastBank1Operation(EraseTimeout);

      /* Disable the PER Bit */
      FLASH->CR &= CR_PER_Reset;
    }
  }
  else
  {
    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastBank2Operation(EraseTimeout);
    if(status == FLASH_COMPLETE)
    { 
      /* if the previous operation is completed, proceed to erase the page */
      FLASH->CR2|= CR_PER_Set;
      FLASH->AR2 = Page_Address; 
      FLASH->CR2|= CR_STRT_Set;
    
      /* Wait for last operation to be completed */
      status = FLASH_WaitForLastBank2Operation(EraseTimeout);
      
      /* Disable the PER Bit */
      FLASH->CR2 &= CR_PER_Reset;
    }
  }
#else
  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation(EraseTimeout);
  
  if(status == FLASH_COMPLETE)
  { 
    /* if the previous operation is completed, proceed to erase the page */
    FLASH->CR|= CR_PER_Set;
    FLASH->AR = Page_Address; 
    FLASH->CR|= CR_STRT_Set;
    
    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation(EraseTimeout);
    
    /* Disable the PER Bit */
    FLASH->CR &= CR_PER_Reset;
  }
#endif /* STM32F10X_XL */

  /* Return the Erase Status */
  return status;
}

/**
  * @brief  Locks the FLASH Bank1 Program Erase Controller.
  * @note   this function can be used for all STM32F10x devices.
  *         - For STM32F10X_XL devices this function Locks Bank1.
  *         - For all other devices it Locks Bank1 and it is equivalent 
  *           to FLASH_Lock function.
  * @param  None
  * @retval None
  */
void FLASH_LockBank1(void)
{
  /* Set the Lock Bit to lock the FPEC and the CR of  Bank1 */
  FLASH->CR |= CR_LOCK_Set;
}

/**
  * @brief  Programs a word at a specified address.
  * @note   This function can be used for all STM32F10x devices.
  * @param  Address: specifies the address to be programmed.
  * @param  Data: specifies the data to be programmed.
  * @retval FLASH Status: The returned value can be: FLASH_ERROR_PG,
  *         FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT. 
  */
FLASH_Status FLASH_ProgramWord(uint32_t Address, uint32_t Data)
{
  FLASH_Status status = FLASH_COMPLETE;
  __IO uint32_t tmp = 0;

  /* Check the parameters */
  // assert_param(IS_FLASH_ADDRESS(Address));

#ifdef STM32F10X_XL
  if(Address < FLASH_BANK1_END_ADDRESS - 2)
  { 
    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastBank1Operation(ProgramTimeout); 
    if(status == FLASH_COMPLETE)
    {
      /* if the previous operation is completed, proceed to program the new first 
        half word */
      FLASH->CR |= CR_PG_Set;
  
      *(__IO uint16_t*)Address = (uint16_t)Data;
      /* Wait for last operation to be completed */
      status = FLASH_WaitForLastOperation(ProgramTimeout);
 
      if(status == FLASH_COMPLETE)
      {
        /* if the previous operation is completed, proceed to program the new second 
        half word */
        tmp = Address + 2;

        *(__IO uint16_t*) tmp = Data >> 16;
    
        /* Wait for last operation to be completed */
        status = FLASH_WaitForLastOperation(ProgramTimeout);
        
        /* Disable the PG Bit */
        FLASH->CR &= CR_PG_Reset;
      }
      else
      {
        /* Disable the PG Bit */
        FLASH->CR &= CR_PG_Reset;
       }
    }
  }
  else if(Address == (FLASH_BANK1_END_ADDRESS - 1))
  {
    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastBank1Operation(ProgramTimeout);

    if(status == FLASH_COMPLETE)
    {
      /* if the previous operation is completed, proceed to program the new first 
        half word */
      FLASH->CR |= CR_PG_Set;
  
      *(__IO uint16_t*)Address = (uint16_t)Data;

      /* Wait for last operation to be completed */
      status = FLASH_WaitForLastBank1Operation(ProgramTimeout);
      
	  /* Disable the PG Bit */
      FLASH->CR &= CR_PG_Reset;
    }
    else
    {
      /* Disable the PG Bit */
      FLASH->CR &= CR_PG_Reset;
    }

    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastBank2Operation(ProgramTimeout);

    if(status == FLASH_COMPLETE)
    {
      /* if the previous operation is completed, proceed to program the new second 
      half word */
      FLASH->CR2 |= CR_PG_Set;
      tmp = Address + 2;

      *(__IO uint16_t*) tmp = Data >> 16;
    
      /* Wait for last operation to be completed */
      status = FLASH_WaitForLastBank2Operation(ProgramTimeout);
        
      /* Disable the PG Bit */
      FLASH->CR2 &= CR_PG_Reset;
    }
    else
    {
      /* Disable the PG Bit */
      FLASH->CR2 &= CR_PG_Reset;
    }
  }
  else
  {
    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastBank2Operation(ProgramTimeout);

    if(status == FLASH_COMPLETE)
    {
      /* if the previous operation is completed, proceed to program the new first 
        half word */
      FLASH->CR2 |= CR_PG_Set;
  
      *(__IO uint16_t*)Address = (uint16_t)Data;
      /* Wait for last operation to be completed */
      status = FLASH_WaitForLastBank2Operation(ProgramTimeout);
 
      if(status == FLASH_COMPLETE)
      {
        /* if the previous operation is completed, proceed to program the new second 
        half word */
        tmp = Address + 2;

        *(__IO uint16_t*) tmp = Data >> 16;
    
        /* Wait for last operation to be completed */
        status = FLASH_WaitForLastBank2Operation(ProgramTimeout);
        
        /* Disable the PG Bit */
        FLASH->CR2 &= CR_PG_Reset;
      }
      else
      {
        /* Disable the PG Bit */
        FLASH->CR2 &= CR_PG_Reset;
      }
    }
  }
#else
  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation(ProgramTimeout);
  
  if(status == FLASH_COMPLETE)
  {
    /* if the previous operation is completed, proceed to program the new first 
    half word */
    FLASH->CR |= CR_PG_Set;
  
    *(__IO uint16_t*)Address = (uint16_t)Data;
    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation(ProgramTimeout);
 
    if(status == FLASH_COMPLETE)
    {
      /* if the previous operation is completed, proceed to program the new second 
      half word */
      tmp = Address + 2;

      *(__IO uint16_t*) tmp = Data >> 16;
    
      /* Wait for last operation to be completed */
      status = FLASH_WaitForLastOperation(ProgramTimeout);
        
      /* Disable the PG Bit */
      FLASH->CR &= CR_PG_Reset;
    }
    else
    {
      /* Disable the PG Bit */
      FLASH->CR &= CR_PG_Reset;
    }
  }         
#endif /* STM32F10X_XL */
   
  /* Return the Program Status */
  return status;
}

/**
  * @brief  Waits for a Flash operation to complete or a TIMEOUT to occur.
  * @note   This function can be used for all STM32F10x devices, 
  *         it is equivalent to FLASH_WaitForLastBank1Operation.
  *         - For STM32F10X_XL devices this function waits for a Bank1 Flash operation
  *           to complete or a TIMEOUT to occur.
  *         - For all other devices it waits for a Flash operation to complete 
  *           or a TIMEOUT to occur.
  * @param  Timeout: FLASH programming Timeout
  * @retval FLASH Status: The returned value can be: FLASH_ERROR_PG,
  *         FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
  */
FLASH_Status FLASH_WaitForLastOperation(uint32_t Timeout)
{ 
  FLASH_Status status = FLASH_COMPLETE;
   
  /* Check for the Flash Status */
  status = FLASH_GetBank1Status();
  /* Wait for a Flash operation to complete or a TIMEOUT to occur */
  while((status == FLASH_BUSY) && (Timeout != 0x00))
  {
    status = FLASH_GetBank1Status();
    Timeout--;
  }
  if(Timeout == 0x00 )
  {
    status = FLASH_TIMEOUT;
  }
  /* Return the operation status */
  return status;
}

/**
  * @brief  Returns the FLASH Bank1 Status.
  * @note   This function can be used for all STM32F10x devices, it is equivalent
  *         to FLASH_GetStatus function.
  * @param  None
  * @retval FLASH Status: The returned value can be: FLASH_BUSY, FLASH_ERROR_PG,
  *         FLASH_ERROR_WRP or FLASH_COMPLETE
  */
FLASH_Status FLASH_GetBank1Status(void)
{
  FLASH_Status flashstatus = FLASH_COMPLETE;
  
  if((FLASH->SR & FLASH_FLAG_BANK1_BSY) == FLASH_FLAG_BSY) 
  {
    flashstatus = FLASH_BUSY;
  }
  else 
  {  
    if((FLASH->SR & FLASH_FLAG_BANK1_PGERR) != 0)
    { 
      flashstatus = FLASH_ERROR_PG;
    }
    else 
    {
      if((FLASH->SR & FLASH_FLAG_BANK1_WRPRTERR) != 0 )
      {
        flashstatus = FLASH_ERROR_WRP;
      }
      else
      {
        flashstatus = FLASH_COMPLETE;
      }
    }
  }
  /* Return the Flash Status */
  return flashstatus;
}
