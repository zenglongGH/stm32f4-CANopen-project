#include <stdlib.h>
#include <stdio.h>

#include "stm32f4xx.h"	// include stm32f4xx drivers


/* Global variables */



int main(void)
{
  // This enables the floating point unit
  SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));
  
  RCC_ClocksTypeDef     RCC_Clocks;
  RCC_GetClocksFreq(&RCC_Clocks);
 
  initialize_uart();
 
  printf("SYS:%lu H:%lu, P1:%lu, P2:%lu\r\n",
          RCC_Clocks.SYSCLK_Frequency,
          RCC_Clocks.HCLK_Frequency,      // AHB
          RCC_Clocks.PCLK1_Frequency,     // APB1
          RCC_Clocks.PCLK2_Frequency);    // APB2
  

  

  while(1)    // Loop for program execution
  {
    
  };
 
 
  /* program exit ***************************************************************/
  printf("Program Exit!\n\r");
  /* save variables to eeprom */
  DISABLE_INTERRUPTS();


  /* reset - by WD */
  return 0;
  
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

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
