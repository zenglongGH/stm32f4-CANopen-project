/*
 * CANopen NMT and Heartbeat producer object.
 *
 * @file        CO_NMT_Heartbeat.c
 * @ingroup     CO_NMT_Heartbeat
 * @version     SVN: \$Id: CO_NMT_Heartbeat.c 32 2013-03-11 08:24:27Z jani22 $
 * @author      Janez Paternoster
 * @copyright   2004 - 2013 Janez Paternoster
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


#include "CO_driver.h"
#include "CO_SDO.h"
#include "CO_Emergency.h"
#include "CO_NMT_Heartbeat.h"

#include <stdlib.h> /*  for malloc, free */

/*
 * Read received message from CAN module.
 *
 * Function will be called (by CAN receive interrupt) every time, when CAN
 * message with correct identifier will be received. For more information and
 * description of parameters see file CO_driver.h.
 */
static int16_t CO_NMT_receive(void *object, CO_CANrxMsg_t *msg){
    CO_NMT_t *NMT;
    uint8_t command, nodeId;

    NMT = (CO_NMT_t*)object;   /* this is the correct pointer type of the first argument */

    /* verify message length */
    if(msg->DLC != 2) return CO_ERROR_RX_MSG_LENGTH;

    nodeId = msg->data[1];

    if(nodeId == 0 || nodeId == NMT->nodeId){
        command = msg->data[0];

        switch(command){
            case CO_NMT_ENTER_OPERATIONAL:      if(!(*NMT->EMpr->errorRegister))
                                                    NMT->operatingState = CO_NMT_OPERATIONAL;   break;
            case CO_NMT_ENTER_STOPPED:          NMT->operatingState = CO_NMT_STOPPED;           break;
            case CO_NMT_ENTER_PRE_OPERATIONAL:  NMT->operatingState = CO_NMT_PRE_OPERATIONAL;   break;
            case CO_NMT_RESET_NODE:             NMT->resetCommand = 2;                          break;
            case CO_NMT_RESET_COMMUNICATION:    NMT->resetCommand = 1;                          break;
            default: CO_errorReport(NMT->EMpr->EM, ERROR_NMT_WRONG_COMMAND, command);
        }
    }

    return CO_ERROR_NO;
}


/******************************************************************************/
int16_t CO_NMT_init(
        CO_NMT_t               *NMT,
        CO_EMpr_t              *EMpr,
        uint8_t                 nodeId,
        uint16_t                firstHBTime,
        CO_CANmodule_t         *NMT_CANdev,
        uint16_t                NMT_rxIdx,
        uint16_t                CANidRxNMT,
        CO_CANmodule_t         *HB_CANdev,
        uint16_t                HB_txIdx,
        uint16_t                CANidTxHB)
{

    /* blinking bytes */
    NMT->LEDflickering          = 0;
    NMT->LEDblinking            = 0;
    NMT->LEDsingleFlash         = 0;
    NMT->LEDdoubleFlash         = 0;
    NMT->LEDtripleFlash         = 0;
    NMT->LEDquadrupleFlash      = 0;

    /* Configure object variables */
    NMT->operatingState         = CO_NMT_INITIALIZING;
    NMT->LEDgreenRun            = -1;
    NMT->LEDredError            = 1;
    NMT->nodeId                 = nodeId;
    NMT->firstHBTime            = firstHBTime;
    NMT->resetCommand           = 0;
    NMT->HBproducerTimer        = 0xFFFF;
    NMT->EMpr                   = EMpr;

    /* configure NMT CAN reception */
    CO_CANrxBufferInit(
            NMT_CANdev,         /* CAN device */
            NMT_rxIdx,          /* rx buffer index */
            CANidRxNMT,         /* CAN identifier */
            0x7FF,              /* mask */
            0,                  /* rtr */
            (void*)NMT,         /* object passed to receive function */
            CO_NMT_receive);    /* this function will process received message */

    /* configure HB CAN transmission */
    NMT->HB_CANdev = HB_CANdev;
    NMT->HB_TXbuff = CO_CANtxBufferInit(
            HB_CANdev,          /* CAN device */
            HB_txIdx,           /* index of specific buffer inside CAN module */
            CANidTxHB,          /* CAN identifier */
            0,                  /* rtr */
            1,                  /* number of data bytes */
            0);                 /* synchronous message flag bit */
                                
    return CO_ERROR_NO;
}


/******************************************************************************/
void CO_NMT_blinkingProcess50ms(CO_NMT_t *NMT){

    if(++NMT->LEDflickering >= 1) NMT->LEDflickering = -1;

    if(++NMT->LEDblinking >= 4) NMT->LEDblinking = -4;

    if(++NMT->LEDsingleFlash >= 4) NMT->LEDsingleFlash = -20;

    switch(++NMT->LEDdoubleFlash){
        case    4:  NMT->LEDdoubleFlash = -104; break;
        case -100:  NMT->LEDdoubleFlash =  100; break;
        case  104:  NMT->LEDdoubleFlash =  -20; break;
    }

    switch(++NMT->LEDtripleFlash){
        case    4:  NMT->LEDtripleFlash = -104; break;
        case -100:  NMT->LEDtripleFlash =  100; break;
        case  104:  NMT->LEDtripleFlash = -114; break;
        case -110:  NMT->LEDtripleFlash =  110; break;
        case  114:  NMT->LEDtripleFlash =  -20; break;
    }

    switch(++NMT->LEDquadrupleFlash){
        case    4:  NMT->LEDquadrupleFlash = -104; break;
        case -100:  NMT->LEDquadrupleFlash =  100; break;
        case  104:  NMT->LEDquadrupleFlash = -114; break;
        case -110:  NMT->LEDquadrupleFlash =  110; break;
        case  114:  NMT->LEDquadrupleFlash = -124; break;
        case -120:  NMT->LEDquadrupleFlash =  120; break;
        case  124:  NMT->LEDquadrupleFlash =  -20; break;
    }
}


/******************************************************************************/
uint8_t CO_NMT_process(
        CO_NMT_t               *NMT,
        uint16_t                timeDifference_ms,
        uint16_t                HBtime,
        uint32_t                NMTstartup,
        uint8_t                 errorRegister,
        const uint8_t          *errorBehavior)
{
    uint8_t CANpassive;

    //printf("CO_NMT_process\r\n");
    if(NMT->HBproducerTimer < HBtime) NMT->HBproducerTimer += timeDifference_ms;

    /* Heartbeat producer message & Bootup message */
    if((HBtime && NMT->HBproducerTimer >= HBtime) || NMT->operatingState == CO_NMT_INITIALIZING){
        printf("CO_NMT_process\r\n");
        NMT->HBproducerTimer = 0;

        NMT->HB_TXbuff->data[0] = NMT->operatingState;
        //printf("CAN Sending...\r\n");
        CO_CANsend(NMT->HB_CANdev, NMT->HB_TXbuff);
        //printf("Sent.\r\n");
        if(NMT->operatingState == CO_NMT_INITIALIZING){
            if(HBtime > NMT->firstHBTime) NMT->HBproducerTimer = HBtime - NMT->firstHBTime;

            if((NMTstartup & 0x04) == 0) NMT->operatingState = CO_NMT_OPERATIONAL;
            else                         NMT->operatingState = CO_NMT_PRE_OPERATIONAL;
        }
    }

    //printf(" 1\r\n");
    /* CAN passive flag */
    CANpassive = 0;
    if(CO_isError(NMT->EMpr->EM, ERROR_CAN_TX_BUS_PASSIVE) || CO_isError(NMT->EMpr->EM, ERROR_CAN_RX_BUS_PASSIVE)){
        printf("CANpassive!!\n\r");
        CANpassive = 0; // HACK by Mike
    }

    //printf(" 2\r\n");
    /* CANopen green RUN LED (DR 303-3) */
    switch(NMT->operatingState){
        case CO_NMT_STOPPED:          NMT->LEDgreenRun = NMT->LEDsingleFlash;   break;
        case CO_NMT_PRE_OPERATIONAL:  NMT->LEDgreenRun = NMT->LEDblinking;      break;
        case CO_NMT_OPERATIONAL:      NMT->LEDgreenRun = 1;                     break;
    }

    //printf(" 3\r\n");
    /* CANopen red ERROR LED (DR 303-3) */
    if(CO_isError(NMT->EMpr->EM, ERROR_CAN_TX_BUS_OFF))
        NMT->LEDredError = 1;

    else if(CO_isError(NMT->EMpr->EM, ERROR_SYNC_TIME_OUT))
        NMT->LEDredError = NMT->LEDtripleFlash;

    else if(CO_isError(NMT->EMpr->EM, ERROR_HEARTBEAT_CONSUMER) || CO_isError(NMT->EMpr->EM, ERROR_HEARTBEAT_CONSUMER_REMOTE_RESET))
        NMT->LEDredError = NMT->LEDdoubleFlash;

    else if(CANpassive || CO_isError(NMT->EMpr->EM, ERROR_CAN_BUS_WARNING))
        NMT->LEDredError = NMT->LEDsingleFlash;

    else if(errorRegister)
        NMT->LEDredError = (NMT->LEDblinking>=0)?-1:1;

    else
        NMT->LEDredError = -1;

    //printf(" 4\r\n");
    /* in case of error enter pre-operational state */
    if(errorBehavior && (NMT->operatingState == CO_NMT_OPERATIONAL)){
        if(CANpassive && (errorBehavior[2] == 0 || errorBehavior[2] == 2)){
          printf("CANpassive && (errorBehavior[2] == 0 || errorBehavior[2] == 2)\n\r");
          errorRegister |= 0x10;
        }
        
        if(errorRegister){
            /* Communication error */
            if(errorRegister&0x10){
                if(errorBehavior[1] == 0){
                    printf("errorBehavior[1] == 0\n\r");
                    NMT->operatingState = CO_NMT_PRE_OPERATIONAL;
                }
                else if(errorBehavior[1] == 2){
                    NMT->operatingState = CO_NMT_STOPPED;
                }
                else if(CO_isError(NMT->EMpr->EM, ERROR_CAN_TX_BUS_OFF)
                     || CO_isError(NMT->EMpr->EM, ERROR_HEARTBEAT_CONSUMER)
                     || CO_isError(NMT->EMpr->EM, ERROR_HEARTBEAT_CONSUMER_REMOTE_RESET))
                {
                    if(errorBehavior[0] == 0){
                        printf("errorBehavior[0] == 0\n\r");
                        NMT->operatingState = CO_NMT_PRE_OPERATIONAL;
                    }
                    else if(errorBehavior[0] == 2){
                        NMT->operatingState = CO_NMT_STOPPED;
                    }
                }
            }

            /* Generic error */
            if(errorRegister&0x01){
                if      (errorBehavior[3] == 0){
                  printf("errorBehavior[3] == 0\n\r");
                  NMT->operatingState = CO_NMT_PRE_OPERATIONAL;
                }
                else if (errorBehavior[3] == 2) NMT->operatingState = CO_NMT_STOPPED;
            }

            /* Device profile error */
            if(errorRegister&0x20){
                if      (errorBehavior[4] == 0){
                  printf("errorBehavior[4] == 0\n\r");
                  NMT->operatingState = CO_NMT_PRE_OPERATIONAL;
                }
                else if (errorBehavior[4] == 2) NMT->operatingState = CO_NMT_STOPPED;
            }

            /* Manufacturer specific error */
            if(errorRegister&0x80){
                if      (errorBehavior[5] == 0){
                  printf("errorBehavior[5] == 0\n\r");
                  NMT->operatingState = CO_NMT_PRE_OPERATIONAL;
                }
                else if (errorBehavior[5] == 2) NMT->operatingState = CO_NMT_STOPPED;
            }

            /* if operational state is lost, send HB immediatelly. */
            if(NMT->operatingState != CO_NMT_OPERATIONAL)
                NMT->HBproducerTimer = HBtime;
        }
    }

    if(NMT->resetCommand)
      printf("NMT->resetCommand = 0x%X\r\n", NMT->resetCommand);
      
    return NMT->resetCommand;
}
