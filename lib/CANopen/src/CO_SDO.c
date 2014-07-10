/*
 * CANopen Service Data Object - server.
 *
 * @file        CO_SDO.c
 * @ingroup     CO_SDO
 * @version     SVN: \$Id: CO_SDO.c 32 2013-03-11 08:24:27Z jani22 $
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
#include "crc16-ccitt.h"

#include <stdlib.h> /*  for malloc, free */


/* State machine, values of the SDO->state variable */
#define STATE_IDLE                     0x00
#define STATE_DOWNLOAD_INITIATE        0x11
#define STATE_DOWNLOAD_SEGMENTED       0x12
#define STATE_DOWNLOAD_BLOCK_INITIATE  0x14
#define STATE_DOWNLOAD_BLOCK_SUBBLOCK  0x15
#define STATE_DOWNLOAD_BLOCK_END       0x16
#define STATE_UPLOAD_INITIATE          0x21
#define STATE_UPLOAD_SEGMENTED         0x22
#define STATE_UPLOAD_BLOCK_INITIATE    0x24
#define STATE_UPLOAD_BLOCK_INITIATE_2  0x25
#define STATE_UPLOAD_BLOCK_SUBBLOCK    0x26
#define STATE_UPLOAD_BLOCK_END         0x27

/* Client command specifier, see DS301 */
#define CCS_DOWNLOAD_INITIATE          1
#define CCS_DOWNLOAD_SEGMENT           0
#define CCS_UPLOAD_INITIATE            2
#define CCS_UPLOAD_SEGMENT             3
#define CCS_DOWNLOAD_BLOCK             6
#define CCS_UPLOAD_BLOCK               5
#define CCS_ABORT                      0x80


#if CO_SDO_BUFFER_SIZE < 7
    #error CO_SDO_BUFFER_SIZE must be greater than 7
#endif

/*
 * Read received message from CAN module.
 *
 * Function will be called (by CAN receive interrupt) every time, when CAN
 * message with correct identifier will be received. For more information and
 * description of parameters see file CO_driver.h.
 */
static int16_t CO_SDO_receive(void *object, CO_CANrxMsg_t *msg){
    CO_SDO_t *SDO;

    //printf("CO_SDO_receive\n\r");

    SDO = (CO_SDO_t*)object;   /* this is the correct pointer type of the first argument */

    /* verify message length */
    if(msg->DLC != 8) return CO_ERROR_RX_MSG_LENGTH;

    /* verify message overflow (previous message was not processed yet) */
    if(SDO->CANrxNew) return CO_ERROR_RX_OVERFLOW;

    /* copy data and set 'new message' flag */
    SDO->CANrxData[0] = msg->data[0];
    SDO->CANrxData[1] = msg->data[1];
    SDO->CANrxData[2] = msg->data[2];
    SDO->CANrxData[3] = msg->data[3];
    SDO->CANrxData[4] = msg->data[4];
    SDO->CANrxData[5] = msg->data[5];
    SDO->CANrxData[6] = msg->data[6];
    SDO->CANrxData[7] = msg->data[7];

    SDO->CANrxNew = 1;

    return CO_ERROR_NO;
}


/*
 * Function for accessing _SDO server parameter_ (index 0x1200+) from SDO server.
 *
 * For more information see file CO_SDO.h.
 */
static uint32_t CO_ODF_1200(CO_ODF_arg_t *ODF_arg){
    uint8_t *nodeId;
    uint32_t *value;

    nodeId = (uint8_t*) ODF_arg->object;
    value = (uint32_t*) ODF_arg->data;

    /* if SDO reading Object dictionary 0x1200, add nodeId to the value */
    if(ODF_arg->reading && ODF_arg->subIndex > 0)
        *value += *nodeId;

    return 0;
}


/******************************************************************************/
int16_t CO_SDO_init(
        CO_SDO_t               *SDO,
        uint16_t                COB_IDClientToServer,
        uint16_t                COB_IDServerToClient,
        uint16_t                ObjDictIndex_SDOServerParameter,
        CO_SDO_t               *parentSDO,
        const CO_OD_entry_t    *OD,
        uint16_t                ODSize,
        CO_OD_extension_t      *ODExtensions,
        uint8_t                 nodeId,
        CO_CANmodule_t         *CANdevRx,
        uint16_t                CANdevRxIdx,
        CO_CANmodule_t         *CANdevTx,
        uint16_t                CANdevTxIdx)
{

    /* configure own object dictionary */
    if(parentSDO == 0){
        uint16_t i;

        SDO->ownOD = 1;
        SDO->OD = OD;
        SDO->ODSize = ODSize;
        SDO->ODExtensions = ODExtensions;

        /* clear pointers in ODExtensions */
        for(i=0; i<ODSize; i++){
            SDO->ODExtensions[i].pODFunc = 0;
            SDO->ODExtensions[i].object = 0;
            SDO->ODExtensions[i].flags = 0;
        }
    }
    /* copy object dictionary from parent */
    else{
        SDO->ownOD = 0;
        SDO->OD = parentSDO->OD;
        SDO->ODSize = parentSDO->ODSize;
        SDO->ODExtensions = parentSDO->ODExtensions;
    }

    /* Configure object variables */
    SDO->nodeId = nodeId;
    SDO->state = STATE_IDLE;
    SDO->CANrxNew = 0;

    /* Configure Object dictionary entry at index 0x1200 */
    if(ObjDictIndex_SDOServerParameter == 0x1200)
        CO_OD_configure(SDO, ObjDictIndex_SDOServerParameter, CO_ODF_1200, (void*)&SDO->nodeId, 0, 0);

    /* configure SDO server CAN reception */
    CO_CANrxBufferInit(
            CANdevRx,               /* CAN device */
            CANdevRxIdx,            /* rx buffer index */
            COB_IDClientToServer,   /* CAN identifier */
            0x7FF,                  /* mask */
            0,                      /* rtr */
            (void*)SDO,             /* object passed to receive function */
            CO_SDO_receive);        /* this function will process received message */

    /* configure SDO server CAN transmission */
    SDO->CANdevTx = CANdevTx;
    SDO->CANtxBuff = CO_CANtxBufferInit(
            CANdevTx,               /* CAN device */
            CANdevTxIdx,            /* index of specific buffer inside CAN module */
            COB_IDServerToClient,   /* CAN identifier */
            0,                      /* rtr */
            8,                      /* number of data bytes */
            0);                     /* synchronous message flag bit */

    return CO_ERROR_NO;
}


/******************************************************************************/
uint16_t CO_OD_configure(
        CO_SDO_t               *SDO,
        uint16_t                index,
        uint32_t              (*pODFunc)(CO_ODF_arg_t *ODF_arg),
        void                   *object,
        uint8_t                *flags,
        uint8_t                 flagsSize)
{
    uint16_t entryNo;
    CO_OD_extension_t *ext;
    uint8_t maxSubIndex;

    entryNo = CO_OD_find(SDO, index);
    if(entryNo == 0xFFFF) return 0xFFFF;   /* index not found in Object dictionary */

    ext = &SDO->ODExtensions[entryNo];
    maxSubIndex = SDO->OD[entryNo].maxSubIndex;

    ext->pODFunc = pODFunc;
    ext->object = object;
    if(flags != NULL && flagsSize != 0 && flagsSize == maxSubIndex){
        uint16_t i;
        ext->flags = flags;
        for(i=0; i<=maxSubIndex; i++){
            ext->flags[i] = 0;
        }
    }
    else{
        ext->flags = NULL;
    }

    return entryNo;
}


/******************************************************************************/
uint16_t CO_OD_find(CO_SDO_t *SDO, uint16_t index){
    /* Fast search in ordered Object Dictionary. If indexes are mixed, this won't work. */
    /* If Object Dictionary has up to 2^N entries, then N is max number of loop passes. */
    uint16_t cur, min, max;
    const CO_OD_entry_t* object;

    min = 0;
    max = SDO->ODSize - 1;
    while(min < max){
        cur = (min + max) / 2;
        object = &SDO->OD[cur];
        /* Is object matched */
        if(index == object->index){
            return cur;
        }
        if(index < object->index){
            max = cur;
            if(max) max--;
        }
        else
            min = cur + 1;
    }

    if(min == max){
        object = &SDO->OD[min];
        /* Is object matched */
        if(index == object->index){
            return min;
        }
    }

    return 0xFFFF;  /* object does not exist in OD */
}


/******************************************************************************/
uint16_t CO_OD_getLength(CO_SDO_t *SDO, uint16_t entryNo, uint8_t subIndex){
    const CO_OD_entry_t* object = &SDO->OD[entryNo];

    if(entryNo == 0xFFFF) return 0;

    if(object->maxSubIndex == 0){    /* Object type is Var */
        if(object->pData == 0)/* data type is domain */
            return CO_SDO_BUFFER_SIZE;
        else
            return object->length;
    }
    else if(object->attribute != 0){ /* Object type is Array */
        if(subIndex == 0) return 1;
        else if(object->pData == 0)/* data type is domain */
            return CO_SDO_BUFFER_SIZE;
        return object->length;
    }
    else{                            /* Object type is Record */
        if(((const CO_OD_entryRecord_t*)(object->pData))[subIndex].pData == 0)/* data type is domain */
            return CO_SDO_BUFFER_SIZE;
        else
            return ((const CO_OD_entryRecord_t*)(object->pData))[subIndex].length;
    }
}


/******************************************************************************/
uint16_t CO_OD_getAttribute(CO_SDO_t *SDO, uint16_t entryNo, uint8_t subIndex){
    const CO_OD_entry_t* object = &SDO->OD[entryNo];

    if(entryNo == 0xFFFF) return 0;

    if(object->maxSubIndex == 0){    /* Object type is Var */
        return object->attribute;
    }
    else if(object->attribute != 0){ /* Object type is Array */
        uint16_t attr = object->attribute;
        if(subIndex == 0){
            /* First subIndex is readonly */
            attr &= ~(CO_ODA_WRITEABLE | CO_ODA_RPDO_MAPABLE);
            attr |= CO_ODA_READABLE;
        }
        return attr;
    }
    else{                            /* Object type is Record */
        return ((const CO_OD_entryRecord_t*)(object->pData))[subIndex].attribute;
    }
}


/******************************************************************************/
void* CO_OD_getDataPointer(CO_SDO_t *SDO, uint16_t entryNo, uint8_t subIndex){
    const CO_OD_entry_t* object = &SDO->OD[entryNo];

    if(entryNo == 0xFFFF) return 0;

    if(object->maxSubIndex == 0){    /* Object type is Var */
        return object->pData;
    }
    else if(object->attribute != 0){ /* Object type is Array */
        if(subIndex==0){
            /* this is the data, for the subIndex 0 in the array */
            return (void*) &object->maxSubIndex;
        }
        else if(object->pData == 0)/* data type is domain */
            return 0;
        else
            return (void*)(((int8_t*)object->pData) + ((subIndex-1) * object->length));
    }
    else{                            /* Object Type is Record */
        return ((const CO_OD_entryRecord_t*)(object->pData))[subIndex].pData;
    }
}


/******************************************************************************/
uint8_t* CO_OD_getFlagsPointer(CO_SDO_t *SDO, uint16_t entryNo, uint8_t subIndex){
    CO_OD_extension_t* ext;

    if(entryNo == 0xFFFF || SDO->ODExtensions == 0) return 0;

    ext = &SDO->ODExtensions[entryNo];

    return &ext->flags[subIndex];
}


/******************************************************************************/
uint32_t CO_SDO_initTransfer(CO_SDO_t *SDO, uint16_t index, uint8_t subIndex){

    /* find object in Object Dictionary */
    SDO->entryNo = CO_OD_find(SDO, index);
    if(SDO->entryNo == 0xFFFF)
        return 0x06020000L;     /* object does not exist in OD */

    /* verify existance of subIndex */
    if(subIndex > SDO->OD[SDO->entryNo].maxSubIndex)
        return 0x06090011L;     /* Sub-index does not exist. */

    /* pointer to data in Object dictionary */
    SDO->ODF_arg.ODdataStorage = CO_OD_getDataPointer(SDO, SDO->entryNo, subIndex);

    /* fill ODF_arg */
    SDO->ODF_arg.object = NULL;
    if(SDO->ODExtensions){
        CO_OD_extension_t *ext = &SDO->ODExtensions[SDO->entryNo];
        SDO->ODF_arg.object = ext->object;
    }
    SDO->ODF_arg.data = SDO->databuffer;
    SDO->ODF_arg.dataLength = CO_OD_getLength(SDO, SDO->entryNo, subIndex);
    SDO->ODF_arg.attribute = CO_OD_getAttribute(SDO, SDO->entryNo, subIndex);
    SDO->ODF_arg.pFlags = CO_OD_getFlagsPointer(SDO, SDO->entryNo, subIndex);
    SDO->ODF_arg.index = index;
    SDO->ODF_arg.subIndex = subIndex;

    SDO->ODF_arg.firstSegment = 1;
    SDO->ODF_arg.lastSegment = 1;

    /* indicate total data length, if not domain */
    SDO->ODF_arg.dataLengthTotal = (SDO->ODF_arg.ODdataStorage) ? SDO->ODF_arg.dataLength : 0;

    /* verify length */
    if(SDO->ODF_arg.dataLength > CO_SDO_BUFFER_SIZE)
        return 0x06040047L;     /* general internal incompatibility in the device */

    return 0;
}


/******************************************************************************/
uint32_t CO_SDO_readOD(CO_SDO_t *SDO, uint16_t SDOBufferSize){
    uint8_t *SDObuffer = SDO->ODF_arg.data;
    uint8_t *ODdata = (uint8_t*)SDO->ODF_arg.ODdataStorage;
    uint16_t length = SDO->ODF_arg.dataLength;
    CO_OD_extension_t *ext = 0;

    /* is object readable? */
    if(!(SDO->ODF_arg.attribute & CO_ODA_READABLE))
        return 0x06010001L;     /* attempt to read a write-only object */

    /* find extension */
    if(SDO->ODExtensions){
        ext = &SDO->ODExtensions[SDO->entryNo];
    }

    /* copy data from OD to SDO buffer if not domain */
    if(ODdata){
        DISABLE_INTERRUPTS();
        while(length--) *(SDObuffer++) = *(ODdata++);
        ENABLE_INTERRUPTS();
    }
    /* if domain, Object dictionary function MUST exist */
    else{
        if(ext->pODFunc == NULL) return 0x06040047L;     /* general internal incompatibility in the device */
    }

    /* call Object dictionary function if registered */
    SDO->ODF_arg.reading = 1;
    if(ext->pODFunc){
        uint32_t abortCode = ext->pODFunc(&SDO->ODF_arg);
        if(abortCode) return abortCode;

        /* dataLength (upadted by pODFunc) must be inside limits */
        if(SDO->ODF_arg.dataLength == 0 || SDO->ODF_arg.dataLength > SDOBufferSize)
            return 0x06040047L;     /* general internal incompatibility in the device */
    }
    SDO->ODF_arg.firstSegment = 0;

    /* swap data if processor is not little endian (CANopen is) */
#ifdef BIG_ENDIAN
    if(SDO->ODF_arg.attribute & CO_ODA_MB_VALUE){
        uint16_t len = SDO->ODF_arg.dataLength;
        uint8_t *buf1 = SDO->ODF_arg.data;
        uint8_t *buf2 = buf1 + len - 1;

        len /= 2;
        while(len--){
            uint8_t b = *buf1;
            *(buf1++) = *buf2;
            *(buf2--) = b;
        }
    }
#endif

    return 0;
}


/******************************************************************************/
uint32_t CO_SDO_writeOD(CO_SDO_t *SDO, uint16_t length){
    uint8_t *SDObuffer = SDO->ODF_arg.data;
    uint8_t *ODdata = (uint8_t*)SDO->ODF_arg.ODdataStorage;

    /* is object writeable? */
    if(!(SDO->ODF_arg.attribute & CO_ODA_WRITEABLE))
        return 0x06010002L;     /* attempt to write a read-only object */

    /* length of domain data is application specific and not verified */
    if(ODdata == 0)
        SDO->ODF_arg.dataLength = length;

    /* verify length except for domain data type */
    else if(SDO->ODF_arg.dataLength != length)
        return 0x06070010L;     /* Length of service parameter does not match */

    /* swap data if processor is not little endian (CANopen is) */
#ifdef BIG_ENDIAN
    if(SDO->ODF_arg.attribute & CO_ODA_MB_VALUE){
        uint16_t len = SDO->ODF_arg.dataLength;
        uint8_t *buf1 = SDO->ODF_arg.data;
        uint8_t *buf2 = buf1 + len - 1;

        len /= 2;
        while(len--){
            uint8_t b = *buf1;
            *(buf1++) = *buf2;
            *(buf2--) = b;
        }
    }
#endif

    /* call Object dictionary function if registered */
    SDO->ODF_arg.reading = 0;
    if(SDO->ODExtensions){
        CO_OD_extension_t *ext = &SDO->ODExtensions[SDO->entryNo];

        if(ext->pODFunc){
            uint32_t abortCode = ext->pODFunc(&SDO->ODF_arg);
            if(abortCode) return abortCode;
        }
    }
    SDO->ODF_arg.firstSegment = 0;

    /* copy data from SDO buffer to OD if not domain */
    if(ODdata){
        DISABLE_INTERRUPTS();
        while(length--) *(ODdata++) = *(SDObuffer++);
        ENABLE_INTERRUPTS();
    }

    return 0;
}


/******************************************************************************/
static void CO_SDO_abort(CO_SDO_t *SDO, uint32_t code){
    SDO->CANtxBuff->data[0] = 0x80;
    SDO->CANtxBuff->data[1] = SDO->ODF_arg.index & 0xFF;
    SDO->CANtxBuff->data[2] = (SDO->ODF_arg.index>>8) & 0xFF;
    SDO->CANtxBuff->data[3] = SDO->ODF_arg.subIndex;
    memcpySwap4(&SDO->CANtxBuff->data[4], (uint8_t*)&code);
    SDO->state = STATE_IDLE;
    SDO->CANrxNew = 0;
    CO_CANsend(SDO->CANdevTx, SDO->CANtxBuff);
}


/******************************************************************************/
int8_t CO_SDO_process(
        CO_SDO_t               *SDO,
        uint8_t                 NMTisPreOrOperational,
        uint16_t                timeDifference_ms,
        uint16_t                SDOtimeoutTime)
{
    uint8_t state = STATE_IDLE;
    uint8_t timeoutSubblockDownolad = 0;

    /* return if idle */
    if(SDO->state == STATE_IDLE && SDO->CANrxNew == 0){
        return 0;
    }

    /* SDO is allowed to work only in operational or pre-operational NMT state */
    if(!NMTisPreOrOperational){
        SDO->state = STATE_IDLE;
        SDO->CANrxNew = 0;
        return 0;
    }

    /* Is something new to process? */
    if(!SDO->CANtxBuff->bufferFull && (SDO->CANrxNew || SDO->state == STATE_UPLOAD_BLOCK_SUBBLOCK)){
        uint8_t CCS = SDO->CANrxData[0] >> 5;   /* Client command specifier */

        /* reset timeout */
        if(SDO->state != STATE_UPLOAD_BLOCK_SUBBLOCK)
            SDO->timeoutTimer = 0;

        /* clear response buffer */
        SDO->CANtxBuff->data[0] = SDO->CANtxBuff->data[1] = SDO->CANtxBuff->data[2] = SDO->CANtxBuff->data[3] = 0;
        SDO->CANtxBuff->data[4] = SDO->CANtxBuff->data[5] = SDO->CANtxBuff->data[6] = SDO->CANtxBuff->data[7] = 0;

        /* Is abort from client? */
        if(SDO->CANrxNew && SDO->CANrxData[0] == CCS_ABORT){
            SDO->state = STATE_IDLE;
            SDO->CANrxNew = 0;
            return -1;
        }

        /* continue with previous SDO communication or start new */
        if(SDO->state != STATE_IDLE){
            state = SDO->state;
        }
        else{
            uint32_t abortCode;

            /* Is client command specifier valid */
            if(CCS != CCS_DOWNLOAD_INITIATE && CCS != CCS_UPLOAD_INITIATE &&
                CCS != CCS_DOWNLOAD_BLOCK && CCS != CCS_UPLOAD_BLOCK){
                CO_SDO_abort(SDO, 0x05040001L);/* Client command specifier not valid or unknown. */
                return -1;
            }

            /* init ODF_arg */
            abortCode = CO_SDO_initTransfer(SDO, (uint16_t)SDO->CANrxData[2]<<8 | SDO->CANrxData[1], SDO->CANrxData[3]);
            if(abortCode){
                CO_SDO_abort(SDO, abortCode);
                return -1;
            }

            /* download */
            if(CCS == CCS_DOWNLOAD_INITIATE || CCS == CCS_DOWNLOAD_BLOCK){
                if(!(SDO->ODF_arg.attribute & CO_ODA_WRITEABLE)){
                    CO_SDO_abort(SDO, 0x06010002L); /* attempt to write a read-only object */
                    return -1;
                }

                /* set state machine to normal or block download */
                if(CCS == CCS_DOWNLOAD_INITIATE)
                    state = STATE_DOWNLOAD_INITIATE;
                else
                    state = STATE_DOWNLOAD_BLOCK_INITIATE;
            }

            /* upload */
            else{
                abortCode = CO_SDO_readOD(SDO, CO_SDO_BUFFER_SIZE);
                if(abortCode){
                    CO_SDO_abort(SDO, abortCode);
                    return -1;
                }

                /* if data size is large enough set state machine to block upload, otherwise set to normal transfer */
                if(CCS == CCS_UPLOAD_BLOCK && SDO->ODF_arg.dataLength > SDO->CANrxData[5])
                    state = STATE_UPLOAD_BLOCK_INITIATE;
                else
                    state = STATE_UPLOAD_INITIATE;
            }
        }
    }

    /* verify SDO timeout */
    if(SDO->timeoutTimer < SDOtimeoutTime) SDO->timeoutTimer += timeDifference_ms;
    if(SDO->timeoutTimer >= SDOtimeoutTime){
        if(SDO->state == STATE_DOWNLOAD_BLOCK_SUBBLOCK && SDO->sequence && !SDO->CANtxBuff->bufferFull){
            timeoutSubblockDownolad = 1;
            state = SDO->state;
        }
        else{
            CO_SDO_abort(SDO, 0x05040000L); /* SDO protocol timed out */
            return -1;
        }
    }

    /* return immediately if still idle */
    if(state == STATE_IDLE) return 0;

    /* state machine (buffer is freed (SDO->CANrxNew = 0;) at the end) */
    switch(state){
        uint32_t abortCode;
        uint16_t len, i, err;

        case STATE_DOWNLOAD_INITIATE:{
            /* default response */
            SDO->CANtxBuff->data[0] = 0x60;
            SDO->CANtxBuff->data[1] = SDO->CANrxData[1];
            SDO->CANtxBuff->data[2] = SDO->CANrxData[2];
            SDO->CANtxBuff->data[3] = SDO->CANrxData[3];

            /* Expedited transfer */
            if(SDO->CANrxData[0] & 0x02){
                /* is size indicated? Get message length */
                if(SDO->CANrxData[0] & 0x01)
                    len = 4 - ((SDO->CANrxData[0]>>2)&0x03);
                else
                    len = 4;

                /* copy data to SDO buffer */
                SDO->ODF_arg.data[0] = SDO->CANrxData[4];
                SDO->ODF_arg.data[1] = SDO->CANrxData[5];
                SDO->ODF_arg.data[2] = SDO->CANrxData[6];
                SDO->ODF_arg.data[3] = SDO->CANrxData[7];

                /* write data to the Object dictionary */
                abortCode = CO_SDO_writeOD(SDO, len);
                if(abortCode){
                    CO_SDO_abort(SDO, abortCode);
                    return -1;
                }

                /* finish the communication */
                SDO->state = STATE_IDLE;
                CO_CANsend(SDO->CANdevTx, SDO->CANtxBuff);
            }

            /* Segmented transfer */
            else{
                /* verify length if size is indicated */
                if(SDO->CANrxData[0]&0x01){
                    uint32_t lenRx;
                    memcpySwap4((uint8_t*)&lenRx, &SDO->CANrxData[4]);
                    SDO->ODF_arg.dataLengthTotal = lenRx;

                    /* verify length except for domain data type */
                    if(lenRx != SDO->ODF_arg.dataLength && SDO->ODF_arg.ODdataStorage != 0){
                        CO_SDO_abort(SDO, 0x06070010L);  /* Length of service parameter does not match */
                        return -1;
                    }
                }
                SDO->bufferOffset = 0;
                SDO->sequence = 0;
                SDO->state = STATE_DOWNLOAD_SEGMENTED;
                CO_CANsend(SDO->CANdevTx, SDO->CANtxBuff);
            }
            break;
        }

        case STATE_DOWNLOAD_SEGMENTED:{
            /* verify client command specifier */
            if((SDO->CANrxData[0]&0xE0) != 0x00){
                CO_SDO_abort(SDO, 0x05040001L);/* Client command specifier not valid or unknown. */
                return -1;
            }

            /* verify toggle bit */
            i = (SDO->CANrxData[0]&0x10) ? 1 : 0;
            if(i != SDO->sequence){
                CO_SDO_abort(SDO, 0x05030000L);/* toggle bit not alternated */
                return -1;
            }

            /* get size of data in message */
            len = 7 - ((SDO->CANrxData[0]>>1)&0x07);

            /* verify length. Domain data type enables length larger than SDO buffer size */
            if((SDO->bufferOffset + len) > SDO->ODF_arg.dataLength){
                if(SDO->ODF_arg.ODdataStorage != 0){
                    CO_SDO_abort(SDO, 0x06070012L);  /* Length of service parameter too high */
                    return -1;
                }
                else{
                    /* empty buffer in domain data type */
                    SDO->ODF_arg.lastSegment = 0;
                    abortCode = CO_SDO_writeOD(SDO, SDO->bufferOffset);
                    if(abortCode){
                        CO_SDO_abort(SDO, abortCode);
                        return -1;
                    }

                    SDO->ODF_arg.dataLength = CO_SDO_BUFFER_SIZE;
                    SDO->bufferOffset = 0;
                }
            }

            /* copy data to buffer */
            for(i=0; i<len; i++)
                SDO->ODF_arg.data[SDO->bufferOffset++] = SDO->CANrxData[i+1];

            /* If no more segments to be downloaded, write data to the Object dictionary */
            if(SDO->CANrxData[0] & 0x01){
                SDO->ODF_arg.lastSegment = 1;
                abortCode = CO_SDO_writeOD(SDO, SDO->bufferOffset);
                if(abortCode){
                    CO_SDO_abort(SDO, abortCode);
                    return -1;
                }

                /* finish */
                SDO->state = STATE_IDLE;
            }

            /* download segment response and alternate toggle bit */
            SDO->CANtxBuff->data[0] = 0x20 | (SDO->sequence ? 0x10 : 0x00);
            SDO->sequence = (SDO->sequence) ? 0 : 1;
            CO_CANsend(SDO->CANdevTx, SDO->CANtxBuff);
            break;
        }

        case STATE_DOWNLOAD_BLOCK_INITIATE:{
            /* verify client command specifier and subcommand */
            if((SDO->CANrxData[0]&0xE1) != 0xC0){
                CO_SDO_abort(SDO, 0x05040001L);/* Client command specifier not valid or unknown. */
                return -1;
            }

            /* prepare response */
            SDO->CANtxBuff->data[0] = 0xA4;
            SDO->CANtxBuff->data[1] = SDO->CANrxData[1];
            SDO->CANtxBuff->data[2] = SDO->CANrxData[2];
            SDO->CANtxBuff->data[3] = SDO->CANrxData[3];

            /* blksize */
            SDO->blksize = (CO_SDO_BUFFER_SIZE > (7*127)) ? 127 : (CO_SDO_BUFFER_SIZE / 7);
            SDO->CANtxBuff->data[4] = SDO->blksize;

            /* is CRC enabled */
            SDO->crcEnabled = (SDO->CANrxData[0] & 0x04) ? 1 : 0;
            SDO->crc = 0;

            /* verify length if size is indicated */
            if(SDO->CANrxData[0]&0x02){
                uint32_t lenRx;
                memcpySwap4((uint8_t*)&lenRx, &SDO->CANrxData[4]);
                SDO->ODF_arg.dataLengthTotal = lenRx;

                /* verify length except for domain data type */
                if(lenRx != SDO->ODF_arg.dataLength && SDO->ODF_arg.ODdataStorage != 0){
                    CO_SDO_abort(SDO, 0x06070010L);  /* Length of service parameter does not match */
                    return -1;
                }
            }

            SDO->bufferOffset = 0;
            SDO->sequence = 0;
            SDO->state = STATE_DOWNLOAD_BLOCK_SUBBLOCK;

            /* send response */
            CO_CANsend(SDO->CANdevTx, SDO->CANtxBuff);
            break;
        }

        case STATE_DOWNLOAD_BLOCK_SUBBLOCK:{
            err = 1;

            /* no new message received, SDO timeout occured, try to response */
            if(timeoutSubblockDownolad){
                SDO->CANrxData[0] = 0;
            }
            else{
                /* if waiting for first segment and received wrong segment just ignore it */
                if(SDO->sequence == 0 && (SDO->CANrxData[0] & 0x7f) != 1){
                    break;
                }

                /* previous segment is received again, ignore it */
                else if((SDO->CANrxData[0] & 0x7f) == SDO->sequence){
                    break;
                }

                /* correct segment is received, copy data */
                else if((SDO->CANrxData[0] & 0x7f) == (SDO->sequence+1)){
                    SDO->sequence++;
                    err = 0;
                    for(i=0; i<7; i++)
                        SDO->ODF_arg.data[SDO->bufferOffset++] = SDO->CANrxData[i+1];
                }
            }

            /* send response if: wrong segment or last segment or end of transfer or timeout */
            if(err || SDO->sequence >= SDO->blksize || (SDO->CANrxData[0] & 0x80) || timeoutSubblockDownolad){
                /* prepare response */
                SDO->CANtxBuff->data[0] = 0xA2;
                SDO->CANtxBuff->data[1] = SDO->sequence;
                SDO->sequence = 0;

                /* empty buffer in domain data type if not last segment */
                if(SDO->ODF_arg.ODdataStorage == 0 && SDO->bufferOffset && !(SDO->CANrxData[0] & 0x80)){
                    /* calculate CRC on next bytes, if enabled */
                    if(SDO->crcEnabled)
                        SDO->crc = crc16_ccitt(SDO->ODF_arg.data, SDO->bufferOffset, SDO->crc);

                    /* write data to the Object dictionary */
                    SDO->ODF_arg.lastSegment = 0;
                    abortCode = CO_SDO_writeOD(SDO, SDO->bufferOffset);
                    if(abortCode){
                        CO_SDO_abort(SDO, abortCode);
                        return -1;
                    }

                    SDO->ODF_arg.dataLength = CO_SDO_BUFFER_SIZE;
                    SDO->bufferOffset = 0;
                }

                /* blksize */
                len = CO_SDO_BUFFER_SIZE - SDO->bufferOffset;
                SDO->blksize = (len > (7*127)) ? 127 : (len / 7);
                SDO->CANtxBuff->data[2] = SDO->blksize;

                /* last segment */
                if((SDO->CANrxData[0] & 0x80) && !err){
                    SDO->state = STATE_DOWNLOAD_BLOCK_END;
                }

                /* send response */
                CO_CANsend(SDO->CANdevTx, SDO->CANtxBuff);
            }

            /* don't clear SDO->CANrxNew flag on timeout */
            if(timeoutSubblockDownolad) return 1;

            break;
        }

        case STATE_DOWNLOAD_BLOCK_END:{
            /* verify client command specifier and subcommand */
            if((SDO->CANrxData[0]&0xE1) != 0xC1){
                CO_SDO_abort(SDO, 0x05040001L);/* Client command specifier not valid or unknown. */
                return -1;
            }

            /* number of bytes in the last segment of the last block that do not contain data. */
            len = (SDO->CANrxData[0]>>2) & 0x07;
            SDO->bufferOffset -= len;

            /* calculate and verify CRC, if enabled */
            if(SDO->crcEnabled){
                uint16_t crc;
                SDO->crc = crc16_ccitt(SDO->ODF_arg.data, SDO->bufferOffset, SDO->crc);

                memcpySwap2((uint8_t*)&crc, &SDO->CANrxData[1]);

                if(SDO->crc != crc){
                    CO_SDO_abort(SDO, 0x05040004);   /* CRC error (block mode only). */
                    return -1;
                }
            }

            /* write data to the Object dictionary */
            SDO->ODF_arg.lastSegment = 1;
            abortCode = CO_SDO_writeOD(SDO, SDO->bufferOffset);
            if(abortCode){
                CO_SDO_abort(SDO, abortCode);
                return -1;
            }

            /* send response */
            SDO->CANtxBuff->data[0] = 0xA1;
            CO_CANsend(SDO->CANdevTx, SDO->CANtxBuff);
            SDO->state = STATE_IDLE;
            break;
        }

        case STATE_UPLOAD_INITIATE:{
            /* default response */
            SDO->CANtxBuff->data[1] = SDO->CANrxData[1];
            SDO->CANtxBuff->data[2] = SDO->CANrxData[2];
            SDO->CANtxBuff->data[3] = SDO->CANrxData[3];

            /* Expedited transfer */
            if(SDO->ODF_arg.dataLength <= 4){
                for(i=0; i<SDO->ODF_arg.dataLength; i++)
                    SDO->CANtxBuff->data[4+i] = SDO->ODF_arg.data[i];

                SDO->CANtxBuff->data[0] = 0x43 | ((4-SDO->ODF_arg.dataLength) << 2);
                CO_CANsend(SDO->CANdevTx, SDO->CANtxBuff);

                /* finish */
                SDO->state = STATE_IDLE;
            }

            /* Segmented transfer */
            else{
                SDO->bufferOffset = 0;
                SDO->sequence = 0;
                SDO->state = STATE_UPLOAD_SEGMENTED;

                /* indicate data size, if known */
                if(SDO->ODF_arg.dataLengthTotal){
                    uint32_t len = SDO->ODF_arg.dataLengthTotal;
                    memcpySwap4(&SDO->CANtxBuff->data[4], (uint8_t*)&len);
                    SDO->CANtxBuff->data[0] = 0x41;
                }
                else{
                    SDO->CANtxBuff->data[0] = 0x40;
                }

                /* send response */
                CO_CANsend(SDO->CANdevTx, SDO->CANtxBuff);
            }
            break;
        }

        case STATE_UPLOAD_SEGMENTED:{
            /* verify client command specifier */
            if((SDO->CANrxData[0]&0xE0) != 0x60){
                CO_SDO_abort(SDO, 0x05040001L);/* Client command specifier not valid or unknown. */
                return -1;
            }

            /* verify toggle bit */
            i = (SDO->CANrxData[0]&0x10) ? 1 : 0;
            if(i != SDO->sequence){
                CO_SDO_abort(SDO, 0x05030000L);/* toggle bit not alternated */
                return -1;
            }

            /* calculate length to be sent */
            len = SDO->ODF_arg.dataLength - SDO->bufferOffset;
            if(len > 7) len = 7;

            /* If data type is domain, re-fill the data buffer if neccessary and indicated so. */
            if(SDO->ODF_arg.ODdataStorage == 0 && len < 7 && SDO->ODF_arg.lastSegment == 0){
                /* copy previous data to the beginning */
                for(i=0; i<len; i++)
                    SDO->ODF_arg.data[i] = SDO->ODF_arg.data[SDO->bufferOffset+i];

                /* move the beginning of the data buffer */
                SDO->ODF_arg.data += len;
                SDO->ODF_arg.dataLength = CO_OD_getLength(SDO, SDO->entryNo, SDO->ODF_arg.subIndex) - len;

                /* read next data from Object dictionary function */
                abortCode = CO_SDO_readOD(SDO, CO_SDO_BUFFER_SIZE);
                if(abortCode){
                    CO_SDO_abort(SDO, abortCode);
                    return -1;
                }

                /* return to the original data buffer */
                SDO->ODF_arg.data -= len;
                SDO->ODF_arg.dataLength +=  len;
                SDO->bufferOffset = 0;

                /* re-calculate the length */
                len = SDO->ODF_arg.dataLength;
                if(len > 7) len = 7;
            }

            /* fill response data bytes */
            for(i=0; i<len; i++)
                SDO->CANtxBuff->data[i+1] = SDO->ODF_arg.data[SDO->bufferOffset++];

            /* first response byte */
            SDO->CANtxBuff->data[0] = 0x00 | (SDO->sequence ? 0x10 : 0x00) | ((7-len)<<1);
            SDO->sequence = (SDO->sequence) ? 0 : 1;

            /* verify end of transfer */
            if(SDO->bufferOffset==SDO->ODF_arg.dataLength && SDO->ODF_arg.lastSegment){
                SDO->CANtxBuff->data[0] |= 0x01;
                SDO->state = STATE_IDLE;
            }

            /* send response */
            CO_CANsend(SDO->CANdevTx, SDO->CANtxBuff);
            break;
        }

        case STATE_UPLOAD_BLOCK_INITIATE:{
            /* default response */
            SDO->CANtxBuff->data[1] = SDO->CANrxData[1];
            SDO->CANtxBuff->data[2] = SDO->CANrxData[2];
            SDO->CANtxBuff->data[3] = SDO->CANrxData[3];

            /* calculate CRC, if enabled */
            if(SDO->CANrxData[0] & 0x04){
                SDO->crcEnabled = 1;
                SDO->crc = crc16_ccitt(SDO->ODF_arg.data, SDO->ODF_arg.dataLength, 0);
            }
            else{
                SDO->crcEnabled = 0;
                SDO->crc = 0;
            }

            /* Number of segments per block */
            SDO->blksize = SDO->CANrxData[4];

            /* verify client subcommand and blksize */
            if((SDO->CANrxData[0]&0x03) != 0x00 || SDO->blksize < 1 || SDO->blksize > 127){
                CO_SDO_abort(SDO, 0x05040001L);/* Client command specifier not valid or unknown. */
                return -1;
            }

            /* verify if SDO data buffer is large enough */
            if((SDO->blksize*7) > SDO->ODF_arg.dataLength && !SDO->ODF_arg.lastSegment){
                CO_SDO_abort(SDO, 0x05040002); /* Invalid block size (block mode only). */
                return -1;
            }

            /* indicate data size, if known */
            if(SDO->ODF_arg.dataLengthTotal){
                uint32_t len = SDO->ODF_arg.dataLengthTotal;
                memcpySwap4(&SDO->CANtxBuff->data[4], (uint8_t*)&len);
                SDO->CANtxBuff->data[0] = 0xC6;
            }
            else{
                SDO->CANtxBuff->data[0] = 0xC4;
            }

            /* send response */
            SDO->state = STATE_UPLOAD_BLOCK_INITIATE_2;
            CO_CANsend(SDO->CANdevTx, SDO->CANtxBuff);
            break;
        }

        case STATE_UPLOAD_BLOCK_INITIATE_2:{
            /* verify client command specifier and subcommand */
            if((SDO->CANrxData[0]&0xE3) != 0xA3){
                CO_SDO_abort(SDO, 0x05040001L);/* Client command specifier not valid or unknown. */
                return -1;
            }

            SDO->bufferOffset = 0;
            SDO->sequence = 0;
            SDO->endOfTransfer = 0;
            SDO->CANrxNew = 0;
            SDO->state = STATE_UPLOAD_BLOCK_SUBBLOCK;
            /* continue in next case */
        }

        case STATE_UPLOAD_BLOCK_SUBBLOCK:{
            /* is block confirmation received */
            if(SDO->CANrxNew){
                uint8_t ackseq;
                uint16_t j;

                /* verify client command specifier and subcommand */
                if((SDO->CANrxData[0]&0xE3) != 0xA2){
                    CO_SDO_abort(SDO, 0x05040001L);/* Client command specifier not valid or unknown. */
                    return -1;
                }

                ackseq = SDO->CANrxData[1];   /* sequence number of the last segment, that was received correctly. */

                /* verify if response is too early */
                if(ackseq > SDO->sequence){
                    CO_SDO_abort(SDO, 0x05040002); /* Invalid block size (block mode only). */
                    return -1;
                }

                /* end of transfer */
                if(SDO->endOfTransfer && ackseq == SDO->blksize){
                    /* first response byte */
                    SDO->CANtxBuff->data[0] = 0xC1 | ((7 - SDO->lastLen) << 2);

                    /* CRC */
                    if(SDO->crcEnabled)
                        memcpySwap2(&SDO->CANtxBuff->data[1], (uint8_t*)&SDO->crc);

                    SDO->state = STATE_UPLOAD_BLOCK_END;

                    /* send response */
                    CO_CANsend(SDO->CANdevTx, SDO->CANtxBuff);
                    break;
                }

                /* move remaining data to the beginning */
                for(i=ackseq*7, j=0; i<SDO->ODF_arg.dataLength; i++, j++)
                    SDO->ODF_arg.data[j] = SDO->ODF_arg.data[i];

                /* set remaining data length in buffer */
                SDO->ODF_arg.dataLength -= ackseq * 7;

                /* new block size */
                SDO->blksize = SDO->CANrxData[2];

                /* If data type is domain, re-fill the data buffer if neccessary and indicated so. */
                if(SDO->ODF_arg.ODdataStorage == 0 && SDO->ODF_arg.dataLength < (SDO->blksize*7) && SDO->ODF_arg.lastSegment == 0){
                    /* move the beginning of the data buffer */
                    len = SDO->ODF_arg.dataLength; /* length of valid data in buffer */
                    SDO->ODF_arg.data += len;
                    SDO->ODF_arg.dataLength = CO_OD_getLength(SDO, SDO->entryNo, SDO->ODF_arg.subIndex) - len;

                    /* read next data from Object dictionary function */
                    abortCode = CO_SDO_readOD(SDO, CO_SDO_BUFFER_SIZE);
                    if(abortCode){
                        CO_SDO_abort(SDO, abortCode);
                        return -1;
                    }

                    /* calculate CRC on next bytes, if enabled */
                    if(SDO->crcEnabled)
                        SDO->crc = crc16_ccitt(SDO->ODF_arg.data, SDO->ODF_arg.dataLength, SDO->crc);

                  /* return to the original data buffer */
                    SDO->ODF_arg.data -= len;
                    SDO->ODF_arg.dataLength +=  len;
                }

                /* verify if SDO data buffer is large enough */
                if((SDO->blksize*7) > SDO->ODF_arg.dataLength && !SDO->ODF_arg.lastSegment){
                    CO_SDO_abort(SDO, 0x05040002); /* Invalid block size (block mode only). */
                    return -1;
                }

                SDO->bufferOffset = 0;
                SDO->sequence = 0;
                SDO->endOfTransfer = 0;

                /* clear flag here */
                SDO->CANrxNew = 0;
            }

            /* return, if all segments was allready transfered or on end of transfer */
            if(SDO->sequence == SDO->blksize || SDO->endOfTransfer)
                return 1;/* don't clear the SDO->CANrxNew flag, so return directly */

            /* reset timeout */
            SDO->timeoutTimer = 0;

            /* calculate length to be sent */
            len = SDO->ODF_arg.dataLength - SDO->bufferOffset;
            if(len > 7) len = 7;

            /* fill response data bytes */
            for(i=0; i<len; i++)
                SDO->CANtxBuff->data[i+1] = SDO->ODF_arg.data[SDO->bufferOffset++];

            /* first response byte */
            SDO->CANtxBuff->data[0] = ++SDO->sequence;

            /* verify end of transfer */
            if(SDO->bufferOffset==SDO->ODF_arg.dataLength && SDO->ODF_arg.lastSegment){
                SDO->CANtxBuff->data[0] |= 0x80;
                SDO->lastLen = len;
                SDO->blksize = SDO->sequence;
                SDO->endOfTransfer = 1;
            }

            /* send response */
            CO_CANsend(SDO->CANdevTx, SDO->CANtxBuff);

            /* don't clear the SDO->CANrxNew flag, so return directly */
            return 1;
        }

        case STATE_UPLOAD_BLOCK_END:{
            /* verify client command specifier */
            if((SDO->CANrxData[0]&0xE1) != 0xA1){
                CO_SDO_abort(SDO, 0x05040001L);/* Client command specifier not valid or unknown. */
                return -1;
            }

            SDO->state = STATE_IDLE;
            break;
        }

        default:{
            CO_SDO_abort(SDO, 0x06040047L);/* general internal incompatibility in the device */
            return -1;
        }
    }

    /* free buffer */
    SDO->CANrxNew = 0;

    if(SDO->state != STATE_IDLE) return 1;

    return 0;
}
