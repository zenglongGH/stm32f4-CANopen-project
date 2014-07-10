/*
 * config.h
 *
 *  Created on: Dec 4, 2012
 *      Author: Niklas Norin
 */

#ifndef CONFIG_H_
#define CONFIG_H_

//#include "stm32f4_discovery.h"
#include "stm32f4xx.h"

/* LED interface */
#define RCC_APB1Periph_GPIO_LED     RCC_AHB1Periph_GPIOD
#define GPIO_LEDS                   GPIOD
#define GPIO_Pin_Led_GREEN          GPIO_Pin_2
#define GPIO_Pin_Led_RED            GPIO_Pin_3
#define LED_POSITIVE                

/* CAN interface */
#define CLOCK_CAN		                RCC_APB1Periph_CAN1
#define CLOCK_GPIO_CAN              RCC_AHB1Periph_GPIOD

#define GPIO_Pin_CAN_RX             GPIO_Pin_0
#define GPIO_Pin_CAN_TX             GPIO_Pin_1
#define GPIO_CAN                    GPIOD
#define CAN_GPIO_CLK                RCC_AHB1Periph_GPIOD
#define CAN_AF_PORT                 GPIO_AF_CAN1
#define CAN_RX_SOURCE               GPIO_PinSource0
#define CAN_TX_SOURCE               GPIO_PinSource1     

/* Mailboxes definition - Taken from stm32f4xx_can.c */
#define TMIDxR_TXRQ       ((uint32_t)0x00000001) /* Transmit mailbox request */
#define CAN_TXMAILBOX_0   ((uint8_t)0x00)
#define CAN_TXMAILBOX_1   ((uint8_t)0x01)
#define CAN_TXMAILBOX_2   ((uint8_t)0x02)

/* Debug Interface */
//#define USART_DBG                   USART2
//#define USART_DBG_CLK               RCC_APB1Periph_USART2

//#define USART_DBG_GPIO              GPIOA
//#define USART_DBG_GPIO_CLK          RCC_AHB1Periph_GPIOA
//#define USART_DBG_RxPin             GPIO_Pin_3
//#define USART_DBG_TxPin             GPIO_Pin_2

#endif /* CONFIG_H_ */
