/*
 * CANopen SYNC object.
 *
 * @file        CO_SYNC.c
 * @ingroup     CO_SYNC
 * @version     SVN: \$Id: CO_SYNC.c 32 2013-03-11 08:24:27Z jani22 $
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

#include "CANopen.h"

#include "CO_driver.h"
#include "CO_SDO.h"
#include "CO_Emergency.h"
#include "CO_NMT_Heartbeat.h"
#include "CO_SYNC.h"

#include <stdlib.h> /*  for malloc, free */

//extern int sync_count;

/*
 * Read received message from CAN module.
 *
 * Function will be called (by CAN receive interrupt) every time, when CAN
 * message with correct identifier will be received. For more information and
 * description of parameters see file CO_driver.h.
 */
static int16_t CO_SYNC_receive(void *object, CO_CANrxMsg_t *msg){
    CO_SYNC_t *SYNC;

    SYNC = (CO_SYNC_t*)object;   /* this is the correct pointer type of the first argument */

    if(*SYNC->operatingState == CO_NMT_OPERATIONAL || *SYNC->operatingState == CO_NMT_PRE_OPERATIONAL){
    
        if(SYNC->counterOverflowValue){
            if(msg->DLC != 1){
                CO_errorReport(SYNC->EM, ERROR_SYNC_LENGTH, msg->DLC | 0x0100);
                return CO_ERROR_NO;
            }
            SYNC->counter = msg->data[0];
        }
        else{
            if(msg->DLC != 0){
                CO_errorReport(SYNC->EM, ERROR_SYNC_LENGTH, msg->DLC);
                return CO_ERROR_NO;
            }
        }

        if(*SYNC->operatingState == CO_NMT_OPERATIONAL){
          SYNC->running = 1;
          
        }
        
        //if(SYNC->timer == 0)
        //  printf("sync->timer == 0\r\n");
        //sync_count++;
        SYNC->timer = 0;
        
        CO_TIMER_ISR();
    }

    return CO_ERROR_NO;
}


/*
 * Function for accessing _COB ID SYNC Message_ (index 0x1005) from SDO server.
 *
 * For more information see file CO_SDO.h.
 */
static uint32_t CO_ODF_1005(CO_ODF_arg_t *ODF_arg){
    CO_SYNC_t *SYNC;
    uint32_t *value;

    SYNC = (CO_SYNC_t*) ODF_arg->object;
    value = (uint32_t*) ODF_arg->data;

    if(!ODF_arg->reading){
        uint8_t configureSyncProducer = 0;

        /* only 11-bit CAN identifier is supported */
        if(*value & 0x20000000L) return 0x06090030L; /* Invalid value for parameter (download only). */

        /* is 'generate Sync messge' bit set? */
        if(*value & 0x40000000L){
            /* if bit was set before, value can not be changed */
            if(SYNC->isProducer) return 0x08000022L;   /* Data cannot be transferred or stored to the application because of the present device state. */
            configureSyncProducer = 1;
        }
        SYNC->COB_ID = *value & 0x7FF;

        if(configureSyncProducer){
            uint8_t len = 0;
            if(SYNC->counterOverflowValue){
                len = 1;
                SYNC->counter = 0;
                SYNC->running = 0;
                SYNC->timer = 0;
            }
            SYNC->CANtxBuff = CO_CANtxBufferInit(
                    SYNC->CANdevTx,         /* CAN device */
                    SYNC->CANdevTxIdx,      /* index of specific buffer inside CAN module */
                    SYNC->COB_ID,           /* CAN identifier */
                    0,                      /* rtr */
                    len,                    /* number of data bytes */
                    0);                     /* synchronous message flag bit */
            SYNC->isProducer = 1;
        }
        else{
            SYNC->isProducer = 0;
        }

        CO_CANrxBufferInit(
                SYNC->CANdevRx,         /* CAN device */
                SYNC->CANdevRxIdx,      /* rx buffer index */
                SYNC->COB_ID,           /* CAN identifier */
                0x7FF,                  /* mask */
                0,                      /* rtr */
                (void*)SYNC,            /* object passed to receive function */
                CO_SYNC_receive);       /* this function will process received message */
    }

    return 0;
}


/*
 * Function for accessing _Communication cycle period_ (index 0x1006) from SDO server.
 *
 * For more information see file CO_SDO.h.
 */
static uint32_t CO_ODF_1006(CO_ODF_arg_t *ODF_arg){
    CO_SYNC_t *SYNC;
    uint32_t *value;

    SYNC = (CO_SYNC_t*) ODF_arg->object;
    value = (uint32_t*) ODF_arg->data;

    if(!ODF_arg->reading){
        /* period transition from 0 to something */
        if(SYNC->periodTime == 0 && *value)
            SYNC->counter = 0;

        SYNC->periodTime = *value;
        SYNC->periodTimeoutTime = *value / 2 * 3;
        /* overflow? */
        if(SYNC->periodTimeoutTime < *value) SYNC->periodTimeoutTime = 0xFFFFFFFFL;

        SYNC->running = 0;
        SYNC->timer = 0;
    }

    return 0;
}


/**
 * Function for accessing _Synchronous counter overflow value_ (index 0x1019) from SDO server.
 *
 * For more information see file CO_SDO.h.
 */
static uint32_t CO_ODF_1019(CO_ODF_arg_t *ODF_arg){
    CO_SYNC_t *SYNC;
    uint8_t *value;

    SYNC = (CO_SYNC_t*) ODF_arg->object;
    value = (uint8_t*) ODF_arg->data;

    if(!ODF_arg->reading){
        uint8_t len = 0;

        if(SYNC->periodTime) return 0x08000022L; /* Data cannot be transferred or stored to the application because of the present device state. */
        SYNC->counterOverflowValue = *value;
        if(SYNC->counterOverflowValue) len = 1;

        SYNC->CANtxBuff = CO_CANtxBufferInit(
                SYNC->CANdevTx,         /* CAN device */
                SYNC->CANdevTxIdx,      /* index of specific buffer inside CAN module */
                SYNC->COB_ID,           /* CAN identifier */
                0,                      /* rtr */
                len,                    /* number of data bytes */
                0);                     /* synchronous message flag bit */
    }

    return 0;
}


/******************************************************************************/
int16_t CO_SYNC_init(
        CO_SYNC_t              *SYNC,
        CO_EM_t                *EM,
        CO_SDO_t               *SDO,
        uint8_t                *operatingState,
        uint32_t                COB_ID_SYNCMessage,
        uint32_t                communicationCyclePeriod,
        uint8_t                 synchronousCounterOverflowValue,
        CO_CANmodule_t         *CANdevRx,
        uint16_t                CANdevRxIdx,
        CO_CANmodule_t         *CANdevTx,
        uint16_t                CANdevTxIdx)
{
    uint8_t len = 0;

    /* Configure object variables */
    SYNC->isProducer = (COB_ID_SYNCMessage&0x40000000L) ? 1 : 0;
    SYNC->COB_ID = COB_ID_SYNCMessage&0x7FF;

    SYNC->periodTime = communicationCyclePeriod;
    SYNC->periodTimeoutTime = communicationCyclePeriod / 2 * 3;
    /* overflow? */
    if(SYNC->periodTimeoutTime < communicationCyclePeriod) SYNC->periodTimeoutTime = 0xFFFFFFFFL;

    SYNC->counterOverflowValue = synchronousCounterOverflowValue;
    if(synchronousCounterOverflowValue) len = 1;

    SYNC->curentSyncTimeIsInsideWindow = 1;
    CANdevTx->curentSyncTimeIsInsideWindow = &SYNC->curentSyncTimeIsInsideWindow; /* parameter inside CAN module. */

    SYNC->running = 0;
    SYNC->timer = 0;
    SYNC->counter = 0;

    SYNC->EM = EM;
    SYNC->operatingState = operatingState;
    SYNC->CANdevRx = CANdevRx;
    SYNC->CANdevRxIdx = CANdevRxIdx;

    /* Configure Object dictionary entry at index 0x1005, 0x1006 and 0x1019 */
    CO_OD_configure(SDO, 0x1005, CO_ODF_1005, (void*)SYNC, 0, 0);
    CO_OD_configure(SDO, 0x1006, CO_ODF_1006, (void*)SYNC, 0, 0);
    CO_OD_configure(SDO, 0x1019, CO_ODF_1019, (void*)SYNC, 0, 0);

    /* configure SYNC CAN reception */
    CO_CANrxBufferInit(
            CANdevRx,               /* CAN device */
            CANdevRxIdx,            /* rx buffer index */
            SYNC->COB_ID,           /* CAN identifier */
            0x7FF,                  /* mask */
            0,                      /* rtr */
            (void*)SYNC,            /* object passed to receive function */
            CO_SYNC_receive);       /* this function will process received message */

    /* configure SYNC CAN transmission */
    SYNC->CANdevTx = CANdevTx;
    SYNC->CANdevTxIdx = CANdevTxIdx;
    SYNC->CANtxBuff = CO_CANtxBufferInit(
            CANdevTx,               /* CAN device */
            CANdevTxIdx,            /* index of specific buffer inside CAN module */
            SYNC->COB_ID,           /* CAN identifier */
            0,                      /* rtr */
            len,                    /* number of data bytes */
            0);                     /* synchronous message flag bit */

    return CO_ERROR_NO;
}


/******************************************************************************/
uint8_t CO_SYNC_process(
        CO_SYNC_t              *SYNC,
        uint32_t                timeDifference_us,
        uint32_t                ObjDict_synchronousWindowLength)
{
    uint8_t ret = 0;
    //uint32_t timerNew;

    if(*SYNC->operatingState == CO_NMT_OPERATIONAL || *SYNC->operatingState == CO_NMT_PRE_OPERATIONAL){
        /* was SYNC just received */
        if(SYNC->running && SYNC->timer == 0)
            ret = 1;

        /* update sync timer, no overflow */
        DISABLE_INTERRUPTS();
        //timerNew = SYNC->timer + timeDifference_us;
        //if(timerNew > SYNC->timer){
          //sync_count++;
          //SYNC->timer = timerNew;
        //}
        SYNC->timer += timeDifference_us;
        ENABLE_INTERRUPTS();

        /* SYNC producer */
        if(SYNC->isProducer && SYNC->periodTime){
            if(SYNC->timer >= SYNC->periodTime){
                if(++SYNC->counter > SYNC->counterOverflowValue) SYNC->counter = 1;
                SYNC->running = 1;
                SYNC->timer = 0;
                SYNC->CANtxBuff->data[0] = SYNC->counter;
                CO_CANsend(SYNC->CANdevTx, SYNC->CANtxBuff);
                ret = 1;
            }
        }

        /* Synchronous PDOs are allowed only inside time window */
        if(ObjDict_synchronousWindowLength){
            if(SYNC->timer > ObjDict_synchronousWindowLength){
                if(SYNC->curentSyncTimeIsInsideWindow == 1){
                    ret = 2;
                }
                SYNC->curentSyncTimeIsInsideWindow = 0;
            }
            else{
                SYNC->curentSyncTimeIsInsideWindow = 1;
            }
        }
        else{
            SYNC->curentSyncTimeIsInsideWindow = 1;
        }

        /* Verify timeout of SYNC */
        if(SYNC->periodTime && SYNC->timer > SYNC->periodTimeoutTime && *SYNC->operatingState == CO_NMT_OPERATIONAL)
            CO_errorReport(SYNC->EM, ERROR_SYNC_TIME_OUT, SYNC->timer);
    }

    if(*SYNC->operatingState != CO_NMT_OPERATIONAL){
        //printf("*SYNC->operatingState != CO_NMT_OPERATIONAL\r\n");
        SYNC->running = 0;
    }

    return ret;
}
