/*
 * CAN module object for Microchip STM32F4xx microcontroller.
 *
 * @file        CO_driver.h
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


#ifndef _CO_DRIVER_H
#define _CO_DRIVER_H

#include "stm32f4xx_conf.h"

#define PACKED_STRUCT               __attribute__((packed))
#define ALIGN_STRUCT_DWORD          __attribute__((aligned(4)))


/* Peripheral addresses */
#define ADDR_CAN1    CAN1


/* Disabling interrupts */
#define DISABLE_INTERRUPTS()        __set_PRIMASK(1);
#define ENABLE_INTERRUPTS()         __set_PRIMASK(0);


/* Data types */
    typedef unsigned char           uint8_t;
    typedef unsigned short int      uint16_t;
    typedef unsigned long int       uint32_t;
    typedef unsigned long long int  uint64_t;
    typedef signed char             int8_t;
    typedef signed short int        int16_t;
    typedef signed long int         int32_t;
    typedef signed long long int    int64_t;
    typedef float                   float32_t;
    typedef long double             float64_t;
    typedef char                    char_t;
    typedef unsigned char           oChar_t;
    typedef unsigned char           domain_t;


/* Return values */
typedef enum{
    CO_ERROR_NO                 = 0,
    CO_ERROR_ILLEGAL_ARGUMENT   = -1,
    CO_ERROR_OUT_OF_MEMORY      = -2,
    CO_ERROR_TIMEOUT            = -3,
    CO_ERROR_ILLEGAL_BAUDRATE   = -4,
    CO_ERROR_RX_OVERFLOW        = -5,
    CO_ERROR_RX_PDO_OVERFLOW    = -6,
    CO_ERROR_RX_MSG_LENGTH      = -7,
    CO_ERROR_RX_PDO_LENGTH      = -8,
    CO_ERROR_TX_OVERFLOW        = -9,
    CO_ERROR_TX_PDO_WINDOW      = -10,
    CO_ERROR_TX_UNCONFIGURED    = -11,
    CO_ERROR_PARAMETERS         = -12,
    CO_ERROR_DATA_CORRUPT       = -13,
    CO_ERROR_CRC                = -14
}CO_ReturnError_t;


/* CAN receive message structure as aligned in CAN module.
 * prevzato z stm32f10_can.h - velikostne polozky a poradi odpovidaji. */
typedef struct{
    uint32_t    ident;          /* Standard Identifier */
    uint32_t    ExtId;          /* Specifies the extended identifier */
    uint8_t     IDE;            /* Specifies the type of identifier for the
                                   message that will be received */
    uint8_t     RTR;            /* Remote Transmission Request bit */
    uint8_t     DLC;            /* Data length code (bits 0...3) */
    uint8_t     data[8];        /* 8 data bytes */
    uint8_t     FMI;            /* Specifies the index of the filter the message
                                   stored in the mailbox passes through */
}CO_CANrxMsg_t;


/* Received message object */
typedef struct{
    uint16_t            ident;
    uint16_t            mask;
    void               *object;
    int16_t           (*pFunct)(void *object, CO_CANrxMsg_t *message);
}CO_CANrx_t;


/* Transmit message object. */
typedef struct{
    uint32_t            ident;
    uint8_t             DLC;
    uint8_t             data[8];
    volatile uint8_t    bufferFull;
    volatile uint8_t    syncFlag;
}CO_CANtx_t;/* ALIGN_STRUCT_DWORD; */


/* CAN module object. */
typedef struct{
    CAN_TypeDef        *CANbaseAddress;         /* STM32F4xx specific */
    CO_CANrx_t         *rxArray;
    uint16_t            rxSize;
    CO_CANtx_t         *txArray;
    uint16_t            txSize;
    volatile uint8_t   *curentSyncTimeIsInsideWindow;
    volatile uint8_t    useCANrxFilters;
    volatile uint8_t    bufferInhibitFlag;
    volatile uint8_t    firstCANtxMessage;
    volatile uint16_t   CANtxCount;
    uint32_t            errOld;
    void               *EM;
    uint8_t             transmittingAborted;    /* STM32F4xx specific */
}CO_CANmodule_t;


/* Init CAN Led Interface */
typedef enum {
    eCoLed_None = 0,
    eCoLed_Green = 1,
    eCoLed_Red = 2,
} eCoLeds;

void InitCanLeds();
void CanLedsOn(eCoLeds led);
void CanLedsOff(eCoLeds led);
void CanLedsSet(eCoLeds led);


/* Endianes */
#ifdef __BIG_ENDIAN__
    #define BIG_ENDIAN
#endif
void memcpySwap2(uint8_t* dest, uint8_t* src);
void memcpySwap4(uint8_t* dest, uint8_t* src);


/* Request CAN configuration or normal mode */
void CO_CANsetConfigurationMode(CAN_TypeDef *CANbaseAddress);
void CO_CANsetNormalMode(CAN_TypeDef *CANbaseAddress);


/* Initialize CAN module object. */
int16_t CO_CANmodule_init(
        CO_CANmodule_t         *CANmodule,
        CAN_TypeDef            *CANbaseAddress,
        CO_CANrx_t             *rxArray,
        uint16_t                rxSize,
        CO_CANtx_t             *txArray,
        uint16_t                txSize,
        uint16_t                CANbitRate);


/* Switch off CANmodule. */
void CO_CANmodule_disable(CO_CANmodule_t *CANmodule);


/* Read CAN identifier */
uint16_t CO_CANrxMsg_readIdent(CO_CANrxMsg_t *rxMsg);


/* Configure CAN message receive buffer. */
int16_t CO_CANrxBufferInit(
        CO_CANmodule_t         *CANmodule,
        uint16_t                index,
        uint16_t                ident,
        uint16_t                mask,
        uint8_t                 rtr,
        void                   *object,
        int16_t               (*pFunct)(void *object, CO_CANrxMsg_t *message));


/* Configure CAN message transmit buffer. */
CO_CANtx_t *CO_CANtxBufferInit(
        CO_CANmodule_t         *CANmodule,
        uint16_t                index,
        uint16_t                ident,
        uint8_t                 rtr,
        uint8_t                 noOfBytes,
        uint8_t                 syncFlag);


/* Send CAN message. */
int16_t CO_CANsend(CO_CANmodule_t *CANmodule, CO_CANtx_t *buffer);


/* Clear all synchronous TPDOs from CAN module transmit buffers. */
void CO_CANclearPendingSyncPDOs(CO_CANmodule_t *CANmodule);


/* Verify all errors of CAN module. */
void CO_CANverifyErrors(CO_CANmodule_t *CANmodule);


/* CAN interrupts receives and transmits CAN messages. */
void CO_CANinterrupt_Rx(CO_CANmodule_t *CANmodule);

void CO_CANinterrupt_Tx(CO_CANmodule_t *CANmodule);

void CO_CANinterrupt_Status(CO_CANmodule_t *CANmodule);

void CO_CANsendToModule(CO_CANmodule_t *CANmodule, CO_CANtx_t *buffer, uint8_t transmit_mailbox);

int CO_CANrecFromModule(CO_CANmodule_t *CANmodule, uint8_t FIFONumber, CO_CANrxMsg_t* RxMessage);


#endif
