/*
 * CAN module object for Microchip STM32F4xx microcontroller.
 *
 * @file        CO_driver.c
 * @version     SVN: \$Id: CO_driver.h 278 2013-03-04 17:11:47Z jani $
 * @author      Janez Paternoster
 * @author      Ondrej Netik
 * @copyright   2004 - 2013 Janez Paternoster, Ondrej Netik
 *
 * This file is part of CANopenNode, an opensource CANopen Stack.
 * Project home page is <http://canopennode.sourceforge.net>.
 * For more information on CANopen see <http://www.can-cia.org/>.
 *
 * CANopenNode is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */



// Custom
#include "CO_config.h"
#include "trace.h"

#include <stm32f4xx_conf.h>
#include <stdint.h>

#include "CO_driver.h"
#include "CO_Emergency.h"

#include "CO_OD.h"

void InitCanLeds() {
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_AHB1PeriphClockCmd(RCC_APB1Periph_GPIO_LED, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_Led_GREEN | GPIO_Pin_Led_RED;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIO_LEDS, &GPIO_InitStructure);
    
    CanLedsOff(eCoLed_Green | eCoLed_Red);
}

void CanLedsSet(eCoLeds led) {
    if (led & eCoLed_Green)
        CanLedsOn(eCoLed_Green);
    else
        CanLedsOff(eCoLed_Green);

    if (led & eCoLed_Red)
        CanLedsOn(eCoLed_Red);
    else
        CanLedsOff(eCoLed_Red);
}

void CanLedsOn(eCoLeds led) {
    if (led & eCoLed_Green) {
#ifdef LED_POSITIVE
        GPIO_SetBits(GPIO_LEDS, GPIO_Pin_Led_GREEN);
#else
        GPIO_ResetBits(GPIO_LEDS, GPIO_Pin_Led_GREEN);
#endif
    }
    if (led & eCoLed_Red) {
#ifdef LED_POSITIVE
        GPIO_SetBits(GPIO_LEDS, GPIO_Pin_Led_RED);
#else
        GPIO_ResetBits(GPIO_LEDS, GPIO_Pin_Led_RED);
#endif
    }
}

void CanLedsOff(eCoLeds led) {
    if (led & eCoLed_Green) {
#ifdef LED_POSITIVE
        GPIO_ResetBits(GPIO_LEDS, GPIO_Pin_Led_GREEN);
#else
        GPIO_SetBits(GPIO_LEDS, GPIO_Pin_Led_GREEN);
#endif
    }
    if (led & eCoLed_Red) {
#ifdef LED_POSITIVE
        GPIO_ResetBits(GPIO_LEDS, GPIO_Pin_Led_RED);
#else
        GPIO_SetBits(GPIO_LEDS, GPIO_Pin_Led_RED);
#endif
    }
}

/******************************************************************************/
#ifdef BIG_ENDIAN
void memcpySwap2(uint8_t* dest, uint8_t* src){
    *(dest++) = *(src+1);
    *(dest) = *(src);
}

void memcpySwap4(uint8_t* dest, uint8_t* src){
    src += 3;
    *(dest++) = *(src--);
    *(dest++) = *(src--);
    *(dest++) = *(src--);
    *(dest) = *(src);
}
#else
void memcpySwap2(uint8_t* dest, uint8_t* src){
    *(dest++) = *(src++);
    *(dest) = *(src);
}

void memcpySwap4(uint8_t* dest, uint8_t* src){
    *(dest++) = *(src++);
    *(dest++) = *(src++);
    *(dest++) = *(src++);
    *(dest) = *(src);
}
#endif


/*******************************************************************************
   Macro and Constants - CAN module registers
 *******************************************************************************/
void CO_CANsetConfigurationMode(CAN_TypeDef *CANbaseAddress) {
//    unsigned int wait_ack = 0;

    /* Request initialisation */
//    CANbaseAddress->MCR |= CAN_MCR_INRQ;

    /* Wait the acknowledge */
//    while (((CANbaseAddress->MSR & CAN_MSR_INAK) != CAN_MSR_INAK) && (wait_ack != INAK_TIMEOUT))
//        wait_ack++;

    /* Check acknowledge */
//    if ((CANbaseAddress->MSR & CAN_MSR_INAK) != CAN_MSR_INAK) {
        // enter to configuration mode failed

//    }


}

/******************************************************************************/
void CO_CANsetNormalMode(CAN_TypeDef *CANbaseAddress) {
//    unsigned int wait_ack = 0;

    /* Request leave initialisation */
//    CANbaseAddress->MCR &= ~(uint32_t) CAN_MCR_INRQ;

    /* Wait the acknowledge */
//    while (((CANbaseAddress->MSR & CAN_MSR_INAK) == CAN_MSR_INAK) && (wait_ack != INAK_TIMEOUT))
//        wait_ack++;

    /* ...and check acknowledged */
//    if ((CANbaseAddress->MSR & CAN_MSR_INAK) == CAN_MSR_INAK) {
        // leave initialization state failed

//    }

}

static void CO_CanInterruptEnDis(CAN_TypeDef *CANbaseAddress, uint8_t enb) {
    CAN_ITConfig(CANbaseAddress,
    		//CAN_IT_TME | // Tx
        CAN_IT_FMP0 |  // Rx
        CAN_IT_ERR |  // Error Interrupt
        CAN_IT_BOF |  // BusOff interrupt
        CAN_IT_EPV |  // Error Passive
        CAN_IT_EWG    // Error Warning
        , enb);
}

/******************************************************************************/
int16_t CO_CANmodule_init(
        CO_CANmodule_t *CANmodule,
        CAN_TypeDef *CANbaseAddress,
        CO_CANrx_t *rxArray,
        uint16_t rxSize,
        CO_CANtx_t *txArray,
        uint16_t txSize,
        uint16_t CANbitRate) {
    CAN_InitTypeDef CAN_InitStructure;
    CAN_FilterInitTypeDef CAN_FilterInitStruct;
    GPIO_InitTypeDef GPIO_InitStructure;
    int i;

    CANmodule->CANbaseAddress = CANbaseAddress;
    CANmodule->rxArray = rxArray;
    CANmodule->rxSize = rxSize;
    CANmodule->txArray = txArray;
    CANmodule->txSize = txSize;
    CANmodule->curentSyncTimeIsInsideWindow = 0;
    CANmodule->bufferInhibitFlag = 0;
    CANmodule->transmittingAborted = 0;
    CANmodule->firstCANtxMessage = 1;
    CANmodule->CANtxCount = 0;
    CANmodule->errOld = 0;
    CANmodule->EM = 0;

    CO_CanInterruptEnDis(CANbaseAddress, DISABLE);

    for (i = 0; i < rxSize; i++) {
        CANmodule->rxArray[i].ident = 0;
        CANmodule->rxArray[i].pFunct = 0;
    }
    for (i = 0; i < txSize; i++) {
        CANmodule->txArray[i].bufferFull = 0;
    }

    /* Setting Clock of CAN HW */
    RCC_APB1PeriphClockCmd(CLOCK_CAN, ENABLE);
    RCC_AHB1PeriphClockCmd(CLOCK_GPIO_CAN, ENABLE);
    
	  /* Connect CAN pins to AF9 */
    GPIO_PinAFConfig(GPIO_CAN, CAN_RX_SOURCE, CAN_AF_PORT);
    GPIO_PinAFConfig(GPIO_CAN, CAN_TX_SOURCE, CAN_AF_PORT); 

	  /* Configure CAN RX and TX pins */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_CAN_RX | GPIO_Pin_CAN_TX;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIO_CAN, &GPIO_InitStructure);

    /* DeInit CAN controler */
    CAN_DeInit(CANmodule->CANbaseAddress);

    /* CAN cell init */
    CAN_InitStructure.CAN_TTCM = DISABLE;
    CAN_InitStructure.CAN_ABOM = DISABLE;
    CAN_InitStructure.CAN_AWUM = DISABLE;
    CAN_InitStructure.CAN_NART = DISABLE;
    CAN_InitStructure.CAN_RFLM = DISABLE;
    CAN_InitStructure.CAN_TXFP = DISABLE;
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
    CAN_InitStructure.CAN_BS1 = CAN_BS1_15tq;
    CAN_InitStructure.CAN_BS2 = CAN_BS2_5tq;
    
    switch(OD_CANBitRate){
      case 1000:
        CAN_InitStructure.CAN_Prescaler=2;
        break;
      case 500:
        CAN_InitStructure.CAN_Prescaler=4;
        break;
      case 400:
        CAN_InitStructure.CAN_Prescaler=5;
        break;
      case 250:
        CAN_InitStructure.CAN_Prescaler=8;
        break;
      case 200:
        CAN_InitStructure.CAN_Prescaler=10;
        break;
      case 125:
        CAN_InitStructure.CAN_Prescaler=16;
        break;
      case 100:
        CAN_InitStructure.CAN_Prescaler=20;
        break;
      case 50:
        CAN_InitStructure.CAN_Prescaler=40;
        break;
      case 40:
        CAN_InitStructure.CAN_Prescaler=80;
        break;
      case 10:
        CAN_InitStructure.CAN_Prescaler=200;
        break;
      default:
        CAN_InitStructure.CAN_Prescaler=2;
        break;
    }
    //CAN_InitStructure.CAN_Prescaler=1;        // 2000 kbit/s
    //CAN_InitStructure.CAN_Prescaler=2;        // 1000 kbit/s
    //CAN_InitStructure.CAN_Prescaler=4;        //  500 kbit/s
    //CAN_InitStructure.CAN_Prescaler=5;        //  400 kbit/s
    //CAN_InitStructure.CAN_Prescaler=8;        //  250 kbit/s
    //CAN_InitStructure.CAN_Prescaler=10;       //  200 kbit/s
    //CAN_InitStructure.CAN_Prescaler=16;       //  125 kbit/s
    //CAN_InitStructure.CAN_Prescaler=20;       //  100 kbit/s
    //CAN_InitStructure.CAN_Prescaler=40;       //   50 kbit/s
    //CAN_InitStructure.CAN_Prescaler=80;       //   40 kbit/s
    //CAN_InitStructure.CAN_Prescaler=200;      //   10 kbit/s
    
    CAN_InitStructure.CAN_Prescaler = 2;
    
    TRACE_DEBUG("CAN_Init ");
    uint8_t result;
    if ((result = CAN_Init(CANmodule->CANbaseAddress, &CAN_InitStructure)) != CAN_InitStatus_Success) {
        TRACE_DEBUG_WP("res=%d\n\r", result);
        return result;
    }
    
    // nastavime 1 filtr, ktery prijima vse
    memset(&CAN_FilterInitStruct, 0, sizeof (CAN_FilterInitStruct));
    CAN_FilterInitStruct.CAN_FilterNumber = 1;
    CAN_FilterInitStruct.CAN_FilterIdHigh = 0;
    CAN_FilterInitStruct.CAN_FilterIdLow = 0;
    CAN_FilterInitStruct.CAN_FilterMaskIdHigh = 0;
    CAN_FilterInitStruct.CAN_FilterMaskIdLow = 0;
    CAN_FilterInitStruct.CAN_FilterFIFOAssignment = 0; // pouzivame jen FIFO0
    CAN_FilterInitStruct.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitStruct.CAN_FilterScale = CAN_FilterScale_32bit;
    CAN_FilterInitStruct.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStruct);

    // povoleni preruseni od CAN1
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    // Enable CAN1 RX interrupt
    NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
    NVIC_Init(&NVIC_InitStructure);
    
    // Enable CAN1 Error interrupt
    NVIC_InitStructure.NVIC_IRQChannel = CAN1_SCE_IRQn;
    NVIC_Init(&NVIC_InitStructure);

    // Enable CAN1 TX interrupt
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannel = CAN1_TX_IRQn;
    NVIC_Init(&NVIC_InitStructure);

    CAN_OperatingModeRequest(CANmodule->CANbaseAddress, CAN_OperatingMode_Normal);
    
    CO_CanInterruptEnDis(CANbaseAddress, ENABLE);
    printf("Ok\n\r");

    return CO_ERROR_NO;
}

/******************************************************************************/
void CO_CANmodule_disable(CO_CANmodule_t *CANmodule){
    CAN_DeInit(CANmodule->CANbaseAddress);
}

/******************************************************************************/
uint16_t CO_CANrxMsg_readIdent(CO_CANrxMsg_t *rxMsg) {
    return (rxMsg->ident >> 2) & 0x7FF;
}

/******************************************************************************/
int16_t CO_CANrxBufferInit(CO_CANmodule_t *CANmodule,
        uint16_t index,
        uint16_t ident,
        uint16_t mask,
        uint8_t rtr,
        void *object,
        int16_t(*pFunct)(void *object, CO_CANrxMsg_t *message)) {
    CO_CANrx_t *rxBuffer;
    uint16_t RXF, RXM;

    //safety
    if (!CANmodule || !object || !pFunct || index >= CANmodule->rxSize) {
        return CO_ERROR_ILLEGAL_ARGUMENT;
    }

    //buffer, which will be configured
    rxBuffer = CANmodule->rxArray + index;

    //Configure object variables
    rxBuffer->object = object;
    rxBuffer->pFunct = pFunct;


    //CAN identifier and CAN mask, bit aligned with CAN module registers
    RXF = (ident & 0x07FF) << 2;
    if (rtr) RXF |= 0x02;
    RXM = (mask & 0x07FF) << 2;
    RXM |= 0x02;

    //configure filter and mask
    if (RXF != rxBuffer->ident || RXM != rxBuffer->mask) {
        rxBuffer->ident = RXF;
        rxBuffer->mask = RXM;
    }

    return CO_ERROR_NO;
}

/******************************************************************************/
CO_CANtx_t *CO_CANtxBufferInit(
        CO_CANmodule_t *CANmodule,
        uint16_t index,
        uint16_t ident,
        uint8_t rtr,
        uint8_t noOfBytes,
        uint8_t syncFlag) {
    //safety
    if (!CANmodule || CANmodule->txSize <= index) return 0;

    //get specific buffer
    CO_CANtx_t *buffer = &CANmodule->txArray[index];

    //CAN identifier, bit aligned with CAN module registers

    uint32_t TXF = 0;
    TXF = ident << (uint32_t)21;
    TXF &= 0xFFE00000;
    if (rtr) TXF |= 0x02;

    buffer->ident = TXF;

    //write to buffer
    buffer->DLC = noOfBytes;
    buffer->bufferFull = 0;
    buffer->syncFlag = syncFlag ? 1 : 0;

    return buffer;
}

int8_t getFreeTxBuff(CO_CANmodule_t *CANmodule) {
	uint8_t txBuff = 0;
	for (txBuff = 0; txBuff <= 3; txBuff++)
		//if (CAN_TransmitStatus(CANmodule->CANbaseAddress, txBuff) == CAN_TxStatus_Ok)
		switch (txBuff) {
		case (CAN_TXMAILBOX_0 ):
			if (CANmodule->CANbaseAddress->TSR & CAN_TSR_TME0 )
				return txBuff;
			else
				break;
		case (CAN_TXMAILBOX_1 ):
			if (CANmodule->CANbaseAddress->TSR & CAN_TSR_TME1 )
				return txBuff;
			else
				break;
		case (CAN_TXMAILBOX_2 ):
			if (CANmodule->CANbaseAddress->TSR & CAN_TSR_TME2 )
				return txBuff;
			else
				break;
		}
	return -1;
}

/******************************************************************************/
int16_t CO_CANsend(CO_CANmodule_t *CANmodule, CO_CANtx_t *buffer) {
   //Code related to CO_CANclearPendingSyncPDOs() function:
   if(CANmodule->transmittingAborted){
    //if message was aborted on buffer, set interrupt flag
    //TODO:  if(CAN_REG(addr, C_TXBUF0 + C_TXCON) & 0x40){CANmodule->transmittingAborted--; CAN_REG(addr, C_INTF) |= 0x04;}
   }
  
   //Was previous message sent or it is still waiting?
   if(buffer->bufferFull){
      if(!CANmodule->firstCANtxMessage)//don't set error, if bootup message is still on buffers
         CO_errorReport((CO_EM_t*)CANmodule->EM, ERROR_CAN_TX_OVERFLOW, 0);
      return CO_ERROR_TX_OVERFLOW;
   }

   //messages with syncFlag set (synchronous PDOs) must be transmited inside preset time window
   if(CANmodule->curentSyncTimeIsInsideWindow && buffer->syncFlag && !(*CANmodule->curentSyncTimeIsInsideWindow)){
      CO_errorReport((CO_EM_t*)CANmodule->EM, ERROR_TPDO_OUTSIDE_WINDOW, 0);
      return CO_ERROR_TX_PDO_WINDOW;
   }

   //if CAN TB buffer0 is free, copy message to it
   int8_t txBuff = getFreeTxBuff(CANmodule);
   if( (txBuff!=-1)  && CANmodule->CANtxCount == 0){
      CANmodule->bufferInhibitFlag = buffer->syncFlag;
      CO_CANsendToModule(CANmodule, buffer, txBuff);
      //CAN_Transmit(CANmodule, buffer);
   }
   //if no buffer is free, message will be sent by interrupt
   else{
      buffer->bufferFull = 1;
      CANmodule->CANtxCount++;
      // vsechny buffery jsou plny, musime povolit preruseni od vysilace, odvysilat az v preruseni
      CAN_ITConfig(CANmodule->CANbaseAddress, CAN_IT_TME, ENABLE);
   }
   
   return CO_ERROR_NO;
}
/******************************************************************************/
void CO_CANclearPendingSyncPDOs(CO_CANmodule_t *CANmodule) {
    DISABLE_INTERRUPTS();

    if (CANmodule->bufferInhibitFlag) {
        CANmodule->CANbaseAddress->TSR |= CAN_TSR_ABRQ0 | CAN_TSR_ABRQ1 | CAN_TSR_ABRQ2;
        ENABLE_INTERRUPTS();
        CO_errorReport((CO_EM_t*) CANmodule->EM, ERROR_TPDO_OUTSIDE_WINDOW, 0);
    } else
        ENABLE_INTERRUPTS();
}

/******************************************************************************/
void CO_CANverifyErrors(CO_CANmodule_t *CANmodule) {
   uint32_t err;
   CO_EM_t* EM = (CO_EM_t*)CANmodule->EM;

   err = CANmodule->CANbaseAddress->ESR;
   // if(CAN_REG(CANmodule->CANbaseAddress, C_INTF) & 4) err |= 0x80;

   if(CANmodule->errOld != err){
      CANmodule->errOld = err;

      //CAN RX bus overflow
      if(CANmodule->CANbaseAddress->RF0R & 0x08){
         CO_errorReport(EM, ERROR_CAN_RXB_OVERFLOW, err);
         CANmodule->CANbaseAddress->RF0R &=~0x08;//clear bits
      }

      //CAN TX bus off
      if(err & 0x04) CO_errorReport(EM, ERROR_CAN_TX_BUS_OFF, err);
      else           CO_errorReset(EM, ERROR_CAN_TX_BUS_OFF, err);

      //CAN TX or RX bus passive
      if(err & 0x02){
         // HACK by Mike
         // if(!CANmodule->firstCANtxMessage) CO_errorReport(EM, ERROR_CAN_TX_BUS_PASSIVE, err);
      }
      else{
         int16_t wasCleared;
         wasCleared =        CO_errorReset(EM, ERROR_CAN_TX_BUS_PASSIVE, err);
         if(wasCleared == 1) CO_errorReset(EM, ERROR_CAN_TX_OVERFLOW, err);
      }


      //CAN TX or RX bus warning
      if(err & 0x01){
         CO_errorReport(EM, ERROR_CAN_BUS_WARNING, err);
      }
      else{
         CO_errorReset(EM, ERROR_CAN_BUS_WARNING, err);
      }
   }
}

/******************************************************************************/
int CO_CANrecFromModule(CO_CANmodule_t *CANmodule, uint8_t FIFONumber, CO_CANrxMsg_t* RxMessage) {
    if( (CANmodule->CANbaseAddress->RF0R & CAN_RF0R_FMP0) > 0) {  // We really have something
        RxMessage->IDE = (uint8_t) 0x04 & CANmodule->CANbaseAddress->sFIFOMailBox[FIFONumber].RIR;
        if (RxMessage->IDE == CAN_Id_Standard) {
            RxMessage->ident = (uint32_t) 0x000007FF & (CANmodule->CANbaseAddress->sFIFOMailBox[FIFONumber].RIR >> 21);
        } else {
            RxMessage->ExtId = (uint32_t) 0x1FFFFFFF & (CANmodule->CANbaseAddress->sFIFOMailBox[FIFONumber].RIR >> 3);
        }

        RxMessage->RTR = (uint8_t) 0x02 & CANmodule->CANbaseAddress->sFIFOMailBox[FIFONumber].RIR;
        /* Get the DLC */
        RxMessage->DLC = (uint8_t) 0x0F & CANmodule->CANbaseAddress->sFIFOMailBox[FIFONumber].RDTR;
        /* Get the FMI */
        RxMessage->FMI = (uint8_t) 0xFF & (CANmodule->CANbaseAddress->sFIFOMailBox[FIFONumber].RDTR >> 8);
        /* Get the data field */
        RxMessage->data[0] = (uint8_t) 0xFF & CANmodule->CANbaseAddress->sFIFOMailBox[FIFONumber].RDLR;
        RxMessage->data[1] = (uint8_t) 0xFF & (CANmodule->CANbaseAddress->sFIFOMailBox[FIFONumber].RDLR >> 8);
        RxMessage->data[2] = (uint8_t) 0xFF & (CANmodule->CANbaseAddress->sFIFOMailBox[FIFONumber].RDLR >> 16);
        RxMessage->data[3] = (uint8_t) 0xFF & (CANmodule->CANbaseAddress->sFIFOMailBox[FIFONumber].RDLR >> 24);
        RxMessage->data[4] = (uint8_t) 0xFF & CANmodule->CANbaseAddress->sFIFOMailBox[FIFONumber].RDHR;
        RxMessage->data[5] = (uint8_t) 0xFF & (CANmodule->CANbaseAddress->sFIFOMailBox[FIFONumber].RDHR >> 8);
        RxMessage->data[6] = (uint8_t) 0xFF & (CANmodule->CANbaseAddress->sFIFOMailBox[FIFONumber].RDHR >> 16);
        RxMessage->data[7] = (uint8_t) 0xFF & (CANmodule->CANbaseAddress->sFIFOMailBox[FIFONumber].RDHR >> 24);
        /* Release the FIFO */
        /* Release FIFO0 */
        if (FIFONumber == CAN_FIFO0) {
            CANmodule->CANbaseAddress->RF0R |= CAN_RF0R_RFOM0;
        }/* Release FIFO1 */
        else /* FIFONumber == CAN_FIFO1 */ {
            CANmodule->CANbaseAddress->RF1R |= CAN_RF1R_RFOM1;
        }
        return 0;
    }
    return -1;
}

/******************************************************************************/
// Interrupt from Receiver
void CO_CANinterrupt_Rx(CO_CANmodule_t *CANmodule) {
    // Preruseni od prijmu
    CO_CANrxMsg_t rcvMsg;
    if (CO_CANrecFromModule(CANmodule, CAN_FIFO0, &rcvMsg) == 0) {
        //CAN module filters are not used, message with any standard 11-bit identifier
        //has been received. Search rxArray form CANmodule for the same CAN-ID.
        uint16_t index;
        uint8_t msgMatched = 0;
        CO_CANrx_t *msgBuff = CANmodule->rxArray;
        for (index = 0; index < CANmodule->rxSize; index++) {
            uint16_t msg = (rcvMsg.ident << 2) | (rcvMsg.RTR ? 2 : 0);
            if (((msg ^ msgBuff->ident) & msgBuff->mask) == 0) {
                msgMatched = 1;
                break;
            }
            msgBuff++;
        }
        //Call specific function, which will process the message
        if (msgMatched && msgBuff->pFunct){
            //printf("Calling rx handler\n\r");
            msgBuff->pFunct(msgBuff->object, &rcvMsg);
        }
    }
}

/******************************************************************************/
// Interrupt from Transeiver
void CO_CANinterrupt_Tx(CO_CANmodule_t *CANmodule) {
    //First CAN message (bootup) was sent successfully
	CAN_ITConfig(CANmodule->CANbaseAddress, CAN_IT_TME, DISABLE); // Transmit mailbox empty interrupt

    CANmodule->firstCANtxMessage = 0;
    //Are there any new messages waiting to be send and buffer is free
    if (CANmodule->CANtxCount > 0) {
        uint16_t index; //index of transmitting message
        //search through whole array of pointers to transmit message buffers.
        for (index = 0; index < CANmodule->txSize; index++) {
            //get specific buffer
            CO_CANtx_t *buffer = &CANmodule->txArray[index];
            //if message buffer is full, send it.
            if (buffer->bufferFull) {
                // dokud je volny nejaky tx buffer
                int8_t txBuff = getFreeTxBuff(CANmodule);
                if (txBuff == -1) {
                	// neni vse odvysilano a neni volny vysilac
                	CAN_ITConfig(CANmodule->CANbaseAddress, CAN_IT_TME, ENABLE); // Transmit mailbox empty interrupt
                	break;
                }
                //messages with syncFlag set (synchronous PDOs) must be transmited inside preset time window
                CANmodule->bufferInhibitFlag = 0;
                if (CANmodule->curentSyncTimeIsInsideWindow && buffer->syncFlag) {
                    if (!(*CANmodule->curentSyncTimeIsInsideWindow)) {
                        CO_errorReport((CO_EM_t*) CANmodule->EM, ERROR_TPDO_OUTSIDE_WINDOW, 0);
                        //release buffer
                        buffer->bufferFull = 0;
                        CANmodule->CANtxCount--;
                        continue; // continue with next message
                    }
                    CANmodule->bufferInhibitFlag = 1;
                }
                //Copy message to CAN buffer
                CO_CANsendToModule(CANmodule, buffer, txBuff);
                //release buffer
                buffer->bufferFull = 0;
                CANmodule->CANtxCount--;
            }
        }//end of for loop
    }
}

/******************************************************************************/
void CO_CANinterrupt_Status(CO_CANmodule_t *CANmodule) {
  // status is evalved with pooling
}

/******************************************************************************/
void CO_CANsendToModule(CO_CANmodule_t *CANmodule, CO_CANtx_t *buffer, uint8_t transmit_mailbox) {
    if ((transmit_mailbox >= 0) & (transmit_mailbox <= 3)) {
        /* Set up the Id */
        CANmodule->CANbaseAddress->sTxMailBox[transmit_mailbox].TIR &= TMIDxR_TXRQ;
        // RTR is included in ident
        CANmodule->CANbaseAddress->sTxMailBox[transmit_mailbox].TIR |= buffer->ident;
        /* Set up the DLC */
        buffer->DLC &= (uint8_t) 0x0000000F;
        CANmodule->CANbaseAddress->sTxMailBox[transmit_mailbox].TDTR &= (uint32_t) 0xFFFFFFF0;
        CANmodule->CANbaseAddress->sTxMailBox[transmit_mailbox].TDTR |= buffer->DLC;
        
        /* Set up the data field */
        CANmodule->CANbaseAddress->sTxMailBox[transmit_mailbox].TDLR = (((uint32_t) buffer->data[3] << 24) |
                ((uint32_t) buffer->data[2] << 16) |
                ((uint32_t) buffer->data[1] << 8) |
                ((uint32_t) buffer->data[0]));
        CANmodule->CANbaseAddress->sTxMailBox[transmit_mailbox].TDHR = (((uint32_t) buffer->data[7] << 24) |
                ((uint32_t) buffer->data[6] << 16) |
                ((uint32_t) buffer->data[5] << 8) |
                ((uint32_t) buffer->data[4]));

        // Transmit mailbox empty interrupt
        CAN_ITConfig(CANmodule->CANbaseAddress, CAN_IT_TME, ENABLE);
        /* Request transmission */
        //printf("CO_CANsendToModule - TIR:  0x%X\r\n", CANmodule->CANbaseAddress->sTxMailBox[transmit_mailbox].TIR);
        //printf("CO_CANsendToModule - TDTR: 0x%X\r\n", CANmodule->CANbaseAddress->sTxMailBox[transmit_mailbox].TDTR);
        //printf("CO_CANsendToModule - TDLR: 0x%X\r\n", CANmodule->CANbaseAddress->sTxMailBox[transmit_mailbox].TDLR);
        //printf("CO_CANsendToModule - TDHR: 0x%X\r\n", CANmodule->CANbaseAddress->sTxMailBox[transmit_mailbox].TDHR);
        CANmodule->CANbaseAddress->sTxMailBox[transmit_mailbox].TIR |= TMIDxR_TXRQ;
        //printf("CO_CANsendToModule - 4\r\n");
    }
}
