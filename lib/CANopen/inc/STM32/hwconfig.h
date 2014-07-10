#ifndef _HWCONFIG_H
#define _HWCONFIG_H


#pragma pack(4) 


/* CAN interface */
#define CLOCK_CAN				            RCC_APB1Periph_CAN1
#define CLOCK_GPIO_CAN     			    RCC_AHB1Periph_GPIOD  

#define CAN_RX_PIN                 GPIO_Pin_11
#define CAN_TX_PIN                 GPIO_Pin_12
#define CAN_GPIO_PORT              GPIOA
#define CAN_GPIO_CLK               RCC_AHB1Periph_GPIOA
#define CAN_AF_PORT                GPIO_AF_CAN1
#define CAN_RX_SOURCE              GPIO_PinSource0
#define CAN_TX_SOURCE              GPIO_PinSource1       

/* LED interface */
#define RCC_APB2Periph_GPIO_LED     RCC_AHB1Periph_GPIOB
#define GPIO_LEDS                   GPIOB
#define GPIO_Pin_Led_GREEN          GPIO_Pin_15
#define GPIO_Pin_Led_RED            GPIO_Pin_14
#define LED_POSITIVE                

/* Debug Interface */
#define USART_DBG                   USART1
#define USART_DBG_CLK               RCC_APB2Periph_USART1
#define GPIO_Remapping_DBG          GPIO_Remap_USART1    

#define USART_DBG_GPIO              GPIOB
#define USART_DBG_GPIO_CLK          RCC_APB2Periph_GPIOB
#define USART_DBG_RxPin             GPIO_Pin_7
#define USART_DBG_TxPin             GPIO_Pin_6
#define USART_DBG_Remap_State       ENABLE



#endif
