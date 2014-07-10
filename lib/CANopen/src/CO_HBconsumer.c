/*
 * CANopen Heartbeat consumer object.
 *
 * @file        CO_HBconsumer.c
 * @ingroup     CO_HBconsumer
 * @version     SVN: \$Id: CO_HBconsumer.c 32 2013-03-11 08:24:27Z jani22 $
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
#include "CO_HBconsumer.h"

#include <stdlib.h> /*  for malloc, free */

/*
 * Read received message from CAN module.
 *
 * Function will be called (by CAN receive interrupt) every time, when CAN
 * message with correct identifier will be received. For more information and
 * description of parameters see file CO_driver.h.
 */
static int16_t CO_HBcons_receive(void *object, CO_CANrxMsg_t *msg){
    CO_HBconsNode_t *HBconsNode;

    HBconsNode = (CO_HBconsNode_t*) object; /* this is the correct pointer type of the first argument */

    /* verify message length */
    if(msg->DLC != 1) return CO_ERROR_RX_MSG_LENGTH;

    /* verify message overflow (previous message was not processed yet) */
    if(HBconsNode->CANrxNew) return CO_ERROR_RX_OVERFLOW;

    /* copy data and set 'new message' flag */
    HBconsNode->NMTstate = msg->data[0];
    HBconsNode->CANrxNew = 1;

    return CO_ERROR_NO;
}


/*
 * Configure one monitored node.
 */
static void CO_HBconsumer_monitoredNodeConfig(
        CO_HBconsumer_t        *HBcons,
        uint8_t                 idx)
{
    uint16_t COB_ID;
    uint16_t NodeID = (uint16_t)((HBcons->HBconsTime[idx]>>16)&0xFF);
    CO_HBconsNode_t *monitoredNode = &HBcons->monitoredNodes[idx];

    monitoredNode->time = (uint16_t)HBcons->HBconsTime[idx];
    monitoredNode->NMTstate = 0;
    monitoredNode->monStarted = 0;

    /* is channel used */
    if(NodeID && monitoredNode->time){
        COB_ID = NodeID + 0x700;
    }
    else{
        COB_ID = 0;
        monitoredNode->time = 0;
    }

    /* configure Heartbeat consumer CAN reception */
    CO_CANrxBufferInit(
            HBcons->CANdevRx,
            HBcons->CANdevRxIdxStart + idx,
            COB_ID,
            0x7FF,
            0,
            (void*)&HBcons->monitoredNodes[idx],
            CO_HBcons_receive);
}


/*
 * OD function for accessing _Consumer Heartbeat Time_ (index 0x1016) from SDO server.
 *
 * For more information see file CO_SDO.h.
 */
static uint32_t CO_ODF_1016(CO_ODF_arg_t *ODF_arg){
    CO_HBconsumer_t *HBcons;
    uint32_t *value;

    HBcons = (CO_HBconsumer_t*) ODF_arg->object;
    value = (uint32_t*) ODF_arg->data;

    if(!ODF_arg->reading){
        uint8_t NodeID;
        uint16_t HBconsTime;
        uint8_t i;

        NodeID = (*value >> 16) & 0xFF;
        HBconsTime = *value & 0xFFFF;

        if(*value & 0xFF800000)
            return 0x06040043L;  /* General parameter incompatibility reason. */

        if(HBconsTime && NodeID){
            /* there must not be more entries with same index and time different than zero */
            for(i = 0; i<HBcons->numberOfMonitoredNodes; i++){
                uint32_t objectCopy = HBcons->HBconsTime[i];
                uint8_t NodeIDObj = (objectCopy >> 16) & 0xFF;
                uint16_t HBconsTimeObj = objectCopy & 0xFFFF;
                if((ODF_arg->subIndex-1)!=i && HBconsTimeObj && (NodeID == NodeIDObj))
                    return 0x06040043L;  /* General parameter incompatibility reason. */
            }
        }

        /* Configure */
        CO_HBconsumer_monitoredNodeConfig(HBcons, ODF_arg->subIndex-1);
    }

    return 0;
}


/******************************************************************************/
int16_t CO_HBconsumer_init(
        CO_HBconsumer_t        *HBcons,
        CO_EM_t                *EM,
        CO_SDO_t               *SDO,
        const uint32_t         *HBconsTime,
        CO_HBconsNode_t        *monitoredNodes,
        uint8_t                 numberOfMonitoredNodes,
        CO_CANmodule_t         *CANdevRx,
        uint16_t                CANdevRxIdxStart)
{
    uint8_t i;

    /* Configure object variables */
    HBcons->EM = EM;
    HBcons->HBconsTime = HBconsTime;
    HBcons->monitoredNodes = monitoredNodes;
    HBcons->numberOfMonitoredNodes = numberOfMonitoredNodes;
    HBcons->allMonitoredOperational = 0;
    HBcons->CANdevRx = CANdevRx;
    HBcons->CANdevRxIdxStart = CANdevRxIdxStart;

    for(i=0; i<HBcons->numberOfMonitoredNodes; i++)
        CO_HBconsumer_monitoredNodeConfig(HBcons, i);

    /* Configure Object dictionary entry at index 0x1016 */
    CO_OD_configure(SDO, 0x1016, CO_ODF_1016, (void*)HBcons, 0, 0);

    return CO_ERROR_NO;
}


/******************************************************************************/
void CO_HBconsumer_process(
        CO_HBconsumer_t        *HBcons,
        uint8_t                 NMTisPreOrOperational,
        uint16_t                timeDifference_ms)
{
    uint8_t i;
    uint8_t AllMonitoredOperationalCopy;
    CO_HBconsNode_t *monitoredNode;

    AllMonitoredOperationalCopy = 5;
    monitoredNode = &HBcons->monitoredNodes[0];

    if(NMTisPreOrOperational){
        for(i=0; i<HBcons->numberOfMonitoredNodes; i++){
            if(monitoredNode->time){/* is node monitored */
                /* Verify if new Consumer Heartbeat message received */
                if(monitoredNode->CANrxNew){
                    if(monitoredNode->NMTstate){
                        /* not a bootup message */
                        monitoredNode->monStarted = 1;
                        monitoredNode->timeoutTimer = 0;  /* reset timer */
                        timeDifference_ms = 0;
                    }
                    monitoredNode->CANrxNew = 0;
                }
                /* Verify timeout */
                if(monitoredNode->timeoutTimer < monitoredNode->time) monitoredNode->timeoutTimer += timeDifference_ms;

                if(monitoredNode->monStarted){
                    if(monitoredNode->timeoutTimer >= monitoredNode->time){
                        CO_errorReport(HBcons->EM, ERROR_HEARTBEAT_CONSUMER, i);
                        monitoredNode->NMTstate = 0;
                    }
                    else if(monitoredNode->NMTstate == 0){
                        /* there was a bootup message */
                        CO_errorReport(HBcons->EM, ERROR_HEARTBEAT_CONSUMER_REMOTE_RESET, i);
                    }
                }
                if(monitoredNode->NMTstate != CO_NMT_OPERATIONAL)
                    AllMonitoredOperationalCopy = 0;
            }
            monitoredNode++;
        }
    }
    else{ /* not in (pre)operational state */
        for(i=0; i<HBcons->numberOfMonitoredNodes; i++){
            monitoredNode->NMTstate = 0;
            monitoredNode->CANrxNew = 0;
            monitoredNode->monStarted = 0;
            monitoredNode++;
        }
        AllMonitoredOperationalCopy = 0;
    }
    HBcons->allMonitoredOperational = AllMonitoredOperationalCopy;
}
