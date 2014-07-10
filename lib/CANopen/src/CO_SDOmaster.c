/*
 * CANopen Service Data Object - client.
 *
 * @file        CO_SDOmaster.c
 * @ingroup     CO_SDOmaster
 * @version     SVN: \$Id: CO_SDOmaster.c 32 2013-03-11 08:24:27Z jani22 $
 * @author      Janez Paternoster
 * @author      Matej Severkar
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
#include "CO_SDOmaster.h"
#include "crc16-ccitt.h"


#include <stdlib.h> /*  for malloc, free */


/* Client command specifier */
#define CCS_DOWNLOAD_INITIATE           1
#define CCS_DOWNLOAD_SEGMENT            0

#define CCS_UPLOAD_INITIATE             2
#define CCS_UPLOAD_SEGMENT              3

#define CCS_ABORT                       4

#define CCS_UPLOAD_BLOCK                5
#define CCS_DOWNLOAD_BLOCK              6

/* Server Command Specifier */
#define SCS_UPLOAD_INITIATE             2
#define SCS_UPLOAD_SEGMENT              0

#define SCS_DOWNLOAD_INITIATED          3
#define SCS_DOWNLOAD_SEGMENT            1

#define SCS_ABORT                       4

#define SCS_DOWNLOAD_BLOCK              5
#define SCS_UPLOAD_BLOCK                6


/* client states */
#define SDO_STATE_NOTDEFINED            0
#define SDO_STATE_ABORT                 1

/* DOWNLOAD EXPEDITED/SEGMENTED */
#define SDO_STATE_DOWNLOAD_INITIATE     10
#define SDO_STATE_DOWNLOAD_REQUEST      11
#define SDO_STATE_DOWNLOAD_RESPONSE     12

/* UPLOAD EXPEDITED/SEGMENTED */
#define SDO_STATE_UPLOAD_INITIATED      20
#define SDO_STATE_UPLOAD_REQUEST        21
#define SDO_STATE_UPLOAD_RESPONSE       22

/* DOWNLOAD BLOCK */
#define SDO_STATE_BLOCKDOWNLOAD_INITIATE        100
#define SDO_STATE_BLOCKDOWNLOAD_INPORGRES       101
#define SDO_STATE_BLOCKDOWNLOAD_BLOCK_ACK       102
#define SDO_STATE_BLOCKDOWNLOAD_CRC             103
#define SDO_STATE_BLOCKDOWNLOAD_CRC_ACK         104

/* UPLOAD BLOCK */
#define SDO_STATE_BLOCKUPLOAD_INITIATE          200
#define SDO_STATE_BLOCKUPLOAD_INITIATE_ACK      201
#define SDO_STATE_BLOCKUPLOAD_INPROGRES         202
#define SDO_STATE_BLOCKUPLOAD_BLOCK_ACK         203
#define SDO_STATE_BLOCKUPLOAD_BLOCK_ACK_LAST    204
#define SDO_STATE_BLOCKUPLOAD_BLOCK_CRC         205
#define SDO_STATE_BLOCKUPLOAD_BLOCK_END         206

/* COMMON */
#define SDO_STATE_IDLE                          250


/* client return codes */
#define SDO_RETURN_BLOCKDOWNLOAD_INPROGRES      3
#define SDO_RETURN_BLOCKUPLOAD_INPROGRES        3
#define SDO_RETURN_WAITING_SERVER_RESPONSE      1
#define SDO_RETURN_COMMUNICATION_END            0

#define SDO_RETURN_END_ERROR                   -1
#define SDO_RETURN_END_TMO                     -11
#define SDO_RETURN_END_SERVERABORT             -10
#define SDO_RETURN_END_CLIENTABORT             -9


/*
 * Read received message from CAN module.
 *
 * Function will be called (by CAN receive interrupt) every time, when CAN
 * message with correct identifier will be received. For more information and
 * description of parameters see file CO_driver.h.
 */
static int16_t CO_SDOclient_receive(void *object, CO_CANrxMsg_t *msg){
    CO_SDOclient_t *SDO_C;

    SDO_C = (CO_SDOclient_t*)object;    /* this is the correct pointer type of the first argument */

    /* verify message length */
    if(msg->DLC != 8) return CO_ERROR_RX_MSG_LENGTH;

    /* verify message overflow (previous message was not processed yet) */
    if(SDO_C->CANrxNew) return CO_ERROR_RX_OVERFLOW;

    /* copy data and set 'new message' flag */
    SDO_C->CANrxData[0] = msg->data[0];
    SDO_C->CANrxData[1] = msg->data[1];
    SDO_C->CANrxData[2] = msg->data[2];
    SDO_C->CANrxData[3] = msg->data[3];
    SDO_C->CANrxData[4] = msg->data[4];
    SDO_C->CANrxData[5] = msg->data[5];
    SDO_C->CANrxData[6] = msg->data[6];
    SDO_C->CANrxData[7] = msg->data[7];

    SDO_C->CANrxNew = 1;

    /* Optional signal to RTOS, which can resume task, which handles SDO client. */
    if(SDO_C->pFunctSignal)
        SDO_C->pFunctSignal(SDO_C->functArg);

    return CO_ERROR_NO;
}


/******************************************************************************/
int16_t CO_SDOclient_init(
        CO_SDOclient_t         *SDO_C,
        CO_SDO_t               *SDO,
        CO_SDOclientPar_t      *SDOClientPar,
        CO_CANmodule_t         *CANdevRx,
        uint16_t                CANdevRxIdx,
        CO_CANmodule_t         *CANdevTx,
        uint16_t                CANdevTxIdx)
{

    /* verify parameters */
    if(SDOClientPar->maxSubIndex!=3) return CO_ERROR_ILLEGAL_ARGUMENT;

    /* Configure object variables */
    SDO_C->state = 0;

    SDO_C->pst    = 21; /*  block transfer */
    SDO_C->block_size_max = 127; /*  block transfer */

    SDO_C->SDO = SDO;
    SDO_C->SDOClientPar = SDOClientPar;

    SDO_C->pFunctSignal = 0;
    SDO_C->functArg = 0;

    SDO_C->CANdevRx = CANdevRx;
    SDO_C->CANdevRxIdx = CANdevRxIdx;
    SDO_C->CANdevTx = CANdevTx;
    SDO_C->CANdevTxIdx = CANdevTxIdx;

    CO_SDOclient_setup(SDO_C, 0, 0, 0);

    return CO_ERROR_NO;
}


/******************************************************************************/
int8_t CO_SDOclient_setup(
        CO_SDOclient_t         *SDO_C,
        uint32_t                COB_IDClientToServer,
        uint32_t                COB_IDServerToClient,
        uint8_t                 nodeIDOfTheSDOServer)
{
    /* verify parameters */
    if((COB_IDClientToServer&0x7FFFF800L) || (COB_IDServerToClient&0x7FFFF800L) ||
        nodeIDOfTheSDOServer > 127) return -2;

    /* Configure object variables */
    SDO_C->state = 0;

    /* setup Object Dictionary variables */
    if((COB_IDClientToServer & 0x80000000L) || (COB_IDServerToClient & 0x80000000L) || nodeIDOfTheSDOServer == 0){
        /* SDO is NOT used */
        SDO_C->SDOClientPar->COB_IDClientToServer = 0x80000000L;
        SDO_C->SDOClientPar->COB_IDServerToClient = 0x80000000L;
        SDO_C->SDOClientPar->nodeIDOfTheSDOServer = 0;
    }
    else{
        if(COB_IDClientToServer == 0 || COB_IDServerToClient == 0){
            SDO_C->SDOClientPar->COB_IDClientToServer = 0x600 + nodeIDOfTheSDOServer;
            SDO_C->SDOClientPar->COB_IDServerToClient = 0x580 + nodeIDOfTheSDOServer;
        }
        else{
            SDO_C->SDOClientPar->COB_IDClientToServer = COB_IDClientToServer;
            SDO_C->SDOClientPar->COB_IDServerToClient = COB_IDServerToClient;
        }
        SDO_C->SDOClientPar->nodeIDOfTheSDOServer = nodeIDOfTheSDOServer;
    }

    /* configure SDO client CAN reception */
    CO_CANrxBufferInit(
            SDO_C->CANdevRx,            /* CAN device */
            SDO_C->CANdevRxIdx,         /* rx buffer index */
            (uint16_t)SDO_C->SDOClientPar->COB_IDServerToClient,/* CAN identifier */
            0x7FF,                      /* mask */
            0,                          /* rtr */
            (void*)SDO_C,               /* object passed to receive function */
            CO_SDOclient_receive);      /* this function will process received message */

    /* configure SDO client CAN transmission */
    SDO_C->CANtxBuff = CO_CANtxBufferInit(
            SDO_C->CANdevTx,            /* CAN device */
            SDO_C->CANdevTxIdx,         /* index of specific buffer inside CAN module */
            (uint16_t)SDO_C->SDOClientPar->COB_IDClientToServer,/* CAN identifier */
            0,                          /* rtr */
            8,                          /* number of data bytes */
            0);                         /* synchronous message flag bit */

    return 0;
}


/******************************************************************************/
static void CO_SDOclient_abort(CO_SDOclient_t *SDO_C, uint32_t code){
    SDO_C->CANtxBuff->data[0] = 0x80;
    SDO_C->CANtxBuff->data[1] = SDO_C->index & 0xFF;
    SDO_C->CANtxBuff->data[2] = (SDO_C->index>>8) & 0xFF;
    SDO_C->CANtxBuff->data[3] = SDO_C->subIndex;
    memcpySwap4(&SDO_C->CANtxBuff->data[4], (uint8_t*)&code);
    CO_CANsend(SDO_C->CANdevTx, SDO_C->CANtxBuff);
    SDO_C->state = 0;
    SDO_C->CANrxNew = 0;
}


/******************************************************************************/
static uint8_t CO_SDObufferClear(uint8_t *p, uint32_t length){
    uint32_t offset =0;

    if (length ==0)
        return 0;

    do{
        *(p + offset) =0;
        offset +=1;
    }
    while (offset<length);
    return 1;
}


/*******************************************************************************
 *
 * DOWNLOAD
 *
 *
 ******************************************************************************/
int8_t CO_SDOclientDownloadInitiate(
        CO_SDOclient_t         *SDO_C,
        uint16_t                index,
        uint8_t                 subIndex,
        uint8_t                *dataTx,
        uint32_t                dataSize,
        uint8_t                 blockEnable)
{
    /* verify parameters */
    if(dataTx == 0 || dataSize == 0) return -2;

    /* save parameters */
    SDO_C->buffer = dataTx;
    SDO_C->bufferSize = dataSize;

    SDO_C->state = SDO_STATE_DOWNLOAD_INITIATE;

    /* prepare CAN tx message */
    CO_SDObufferClear (&SDO_C->CANtxBuff->data[0], 8);

    SDO_C->index = index;
    SDO_C->subIndex = subIndex;
    SDO_C->CANtxBuff->data[1] = index & 0xFF;
    SDO_C->CANtxBuff->data[2] = index >> 8;
    SDO_C->CANtxBuff->data[3] = subIndex;

    /* if nodeIDOfTheSDOServer == node-ID of this node, then exchange data with this node */
    if(SDO_C->SDOClientPar->nodeIDOfTheSDOServer == SDO_C->SDO->nodeId){
        return 0;
    }

    if(dataSize <= 4){
        uint16_t i;
        /* expedited transfer */
        SDO_C->CANtxBuff->data[0] = 0x23 | ((4-dataSize) << 2);

        /* copy data */
        for(i=dataSize+3; i>=4; i--) SDO_C->CANtxBuff->data[i] = dataTx[i-4];
    }
    else if((SDO_C->bufferSize > SDO_C->pst) && blockEnable != 0){ /*  BLOCK transfer */
        /*  set state of block transfer */
        SDO_C->state = SDO_STATE_BLOCKDOWNLOAD_INITIATE;

        /*  init HEAD of packet */
        SDO_C->CANtxBuff->data[0] = CCS_DOWNLOAD_BLOCK<<5;
        /*  set flag for CRC */
        SDO_C->CANtxBuff->data[0] |= 0x01<<2;

        /*  size indicator: */
        SDO_C->CANtxBuff->data[0] |= 0x01<<1;
        /*  set length of data */
        SDO_C->CANtxBuff->data[4] = (uint8_t) dataSize;
        SDO_C->CANtxBuff->data[5] = (uint8_t) (dataSize >> 8);
        SDO_C->CANtxBuff->data[6] = (uint8_t) (dataSize >> 16);
        SDO_C->CANtxBuff->data[7] = (uint8_t) (dataSize >> 24);

    }
    else{
        uint32_t len;
        /* segmented transfer */
        SDO_C->CANtxBuff->data[0] = 0x21;
        len = dataSize;
        memcpySwap4(&SDO_C->CANtxBuff->data[4], (uint8_t*)&len);
    }

    /* empty receive buffer, reset timeout timer and send message */
    SDO_C->CANrxNew = 0;
    SDO_C->timeoutTimer = 0;
    CO_CANsend(SDO_C->CANdevTx, SDO_C->CANtxBuff);

    return 0;
}


/******************************************************************************/
int8_t CO_SDOclientDownload(
        CO_SDOclient_t         *SDO_C,
        uint16_t                timeDifference_ms,
        uint16_t                SDOtimeoutTime,
        uint32_t               *pSDOabortCode)
{
    /* clear abort code */
    *pSDOabortCode = 0;

    /* if nodeIDOfTheSDOServer == node-ID of this node, then exchange data with this node */
    if(SDO_C->SDO && SDO_C->SDOClientPar->nodeIDOfTheSDOServer == SDO_C->SDO->nodeId){
        /* If SDO server is busy return error */
        if(SDO_C->SDO->state != 0){
            return -9;
        }

        /* init ODF_arg */
        *pSDOabortCode = CO_SDO_initTransfer(SDO_C->SDO, SDO_C->index, SDO_C->subIndex);
        if(*pSDOabortCode){
            return -10;
        }

        /* set buffer */
        SDO_C->SDO->ODF_arg.data = SDO_C->buffer;

        /* write data to the Object dictionary */
        *pSDOabortCode = CO_SDO_writeOD(SDO_C->SDO, SDO_C->bufferSize);
        if(*pSDOabortCode){
            return -10;
        }

        SDO_C->state = 0;
        SDO_C->CANrxNew = 0;
        return 0;
    }


/*  RX data ****************************************************************************************** */
    if(SDO_C->CANrxNew){
        uint8_t SCS = SDO_C->CANrxData[0]>>5;    /* Client command specifier */

        /* ABORT */
        if (SDO_C->CANrxData[0] == (SCS_ABORT<<5)){
            SDO_C->state = SDO_STATE_IDLE;
            SDO_C->CANrxNew    =0;
            memcpySwap4((uint8_t*)pSDOabortCode , &SDO_C->CANrxData[4]);
            SDO_C->CANrxNew = 0;
            return SDO_RETURN_END_SERVERABORT;
        }

        switch (SDO_C->state){

            case SDO_STATE_DOWNLOAD_INITIATE:{

                if (SCS == SCS_DOWNLOAD_INITIATED){
                    if(SDO_C->bufferSize <= 4){
                        /* expedited transfer */
                        SDO_C->state = SDO_STATE_NOTDEFINED;
                        SDO_C->CANrxNew = 0;
                        return SDO_RETURN_COMMUNICATION_END;
                    }
                    else{
                        /* segmented transfer - prepare the first segment */
                        SDO_C->bufferOffset = 0;
                        SDO_C->toggle =0;
                        SDO_C->state = SDO_STATE_DOWNLOAD_REQUEST;
                    }
                }
                else{
                    *pSDOabortCode = 0x05040001L; /* Client/server command specifier not valid or unknown */
                    SDO_C->state = SDO_STATE_ABORT;
                }
                break;
            }

            case SDO_STATE_DOWNLOAD_RESPONSE:{

                if (SCS == SCS_DOWNLOAD_SEGMENT){
                    /* verify toggle bit */
                    if((SDO_C->CANrxData[0]&0x10) != (SDO_C->toggle<<4)){
                        *pSDOabortCode = 0x05030000L;    /* toggle bit not alternated */
                        SDO_C->state = SDO_STATE_ABORT;
                        break;
                    }
                    /* alternate toggle bit */
                    if (SDO_C->toggle ==0x00)
                        SDO_C->toggle =0x01;
                    else
                        SDO_C->toggle =0x00;

                    /* is end of transfer? */
                    if(SDO_C->bufferOffset == SDO_C->bufferSize){
                        SDO_C->state = SDO_STATE_NOTDEFINED;
                        SDO_C->CANrxNew = 0;
                        return SDO_RETURN_COMMUNICATION_END;
                    }
                    SDO_C->state = SDO_STATE_DOWNLOAD_REQUEST;
                }
                else{
                    *pSDOabortCode = 0x05040001L; /* Client/server command specifier not valid or unknown */
                    SDO_C->state = SDO_STATE_ABORT;
                }
                break;
            }

            case SDO_STATE_BLOCKDOWNLOAD_INITIATE:{ /* waiting on reply on block download initiated */
                if (SCS == SCS_DOWNLOAD_BLOCK){
                    uint16_t IndexTmp;
                    IndexTmp = SDO_C->CANrxData[2];
                    IndexTmp = IndexTmp << 8;
                    IndexTmp |= SDO_C->CANrxData[1];

                    if(IndexTmp != SDO_C->index || SDO_C->CANrxData[3] != SDO_C->subIndex) {
                        /*  wrong index */
                        *pSDOabortCode = 0x06040043L;
                        SDO_C->state = SDO_STATE_ABORT;
                        break;
                    }
                    /*  set blksize */
                    SDO_C->block_blksize = SDO_C->CANrxData[4];

                    SDO_C->block_seqno = 0;
                    SDO_C->bufferOffset = 0;
                    SDO_C->bufferOffsetACK = 0;
                    SDO_C->state = SDO_STATE_BLOCKDOWNLOAD_INPORGRES;

                    break;
                }
                else{
                    *pSDOabortCode = 0x05040001L; /* Client/server command specifier not valid or unknown */
                    SDO_C->state = SDO_STATE_ABORT;
                }
            }

            case SDO_STATE_BLOCKDOWNLOAD_INPORGRES:
            case SDO_STATE_BLOCKDOWNLOAD_BLOCK_ACK:{ /*  waiting block ACK */
                if (SCS == SCS_DOWNLOAD_BLOCK){
                    /*  check server subcommand */
                    if((SDO_C->CANrxData[0] & 0x02) == 0){
                        /* wrong server sub command */
                        *pSDOabortCode = 0x05040001; /* Client/server command specifier not valid or unknown */
                        SDO_C->state = SDO_STATE_ABORT;
                        break;
                    }
                    /*  check number of segments */
                    if(SDO_C->CANrxData[1] != SDO_C->block_blksize){
                        /*  NOT all segments transfert sucesfuly */
                        SDO_C->bufferOffsetACK = SDO_C->CANrxData[1] * 7;
                        SDO_C->bufferOffset = SDO_C->bufferOffsetACK;
                    }
                    else{
                        SDO_C->bufferOffsetACK = SDO_C->bufferOffset;
                    }
                    /*  set size of next block */
                    SDO_C->block_blksize = SDO_C->CANrxData[2];
                    SDO_C->block_seqno = 0;

                    if(SDO_C->bufferOffset >= SDO_C->bufferSize)
                        SDO_C->state = SDO_STATE_BLOCKDOWNLOAD_CRC;
                    else
                        SDO_C->state = SDO_STATE_BLOCKDOWNLOAD_INPORGRES;
                }
                else{
                    *pSDOabortCode = 0x05040001L; /* Client/server command specifier not valid or unknown */
                    SDO_C->state = SDO_STATE_ABORT;
                }
                break;
            }

            case SDO_STATE_BLOCKDOWNLOAD_CRC_ACK:{
                if (SCS == SCS_DOWNLOAD_BLOCK){
                    if((SDO_C->CANrxData[0] & 0x01) == 0){
                        /*  wrong server sub command */
                        *pSDOabortCode = 0x05040001; /* Client/server command specifier not valid or unknown */
                        SDO_C->state = SDO_STATE_ABORT;
                        break;
                    }
                    /*  SDO bloct download sucesfuly transferd */
                    SDO_C->state = SDO_STATE_NOTDEFINED;
                    SDO_C->timeoutTimer = 0;
                    SDO_C->CANrxNew = 0;
                    return SDO_RETURN_COMMUNICATION_END;
                }
                else{
                    *pSDOabortCode = 0x05040001L; /* Client/server command specifier not valid or unknown */
                    SDO_C->state = SDO_STATE_ABORT;
                    break;
                }
            }

            default:{
                *pSDOabortCode = 0x05040001L; /* Client command specifier not valid or unknown. */
                SDO_C->state = SDO_STATE_ABORT;
            }
        }
        SDO_C->timeoutTimer = 0;
        SDO_C->CANrxNew = 0;
    }

/*  TMO *********************************************************************************************** */
    if(SDO_C->timeoutTimer < SDOtimeoutTime){
        SDO_C->timeoutTimer += timeDifference_ms;
    }
    if(SDO_C->timeoutTimer >= SDOtimeoutTime){ /*  comunication TMO */
        *pSDOabortCode = 0x05040000L;    /* SDO protocol timed out */
        CO_SDOclient_abort(SDO_C, *pSDOabortCode);
        return SDO_RETURN_END_TMO;
    }

/*  TX data ******************************************************************************************* */
    CO_SDObufferClear (&SDO_C->CANtxBuff->data[0], 8);
    switch (SDO_C->state){
        /*  ABORT */
        case SDO_STATE_ABORT:{
            SDO_C->state = SDO_STATE_NOTDEFINED;
            CO_SDOclient_abort (SDO_C, *pSDOabortCode);
            return SDO_RETURN_END_CLIENTABORT;
        }
            /*  SEGMENTED */
        case SDO_STATE_DOWNLOAD_REQUEST:{
            uint16_t i, j;
            /* calculate length to be sent */
            j = SDO_C->bufferSize - SDO_C->bufferOffset;
            if(j > 7) j = 7;
            /* fill data bytes */
            for(i=0; i<j; i++)
                SDO_C->CANtxBuff->data[i+1] = SDO_C->buffer[SDO_C->bufferOffset + i];

            for(; i<7; i++)
                SDO_C->CANtxBuff->data[i+1] = 0;

            SDO_C->bufferOffset += j;
            /* SDO command specifier */
            SDO_C->CANtxBuff->data[0] = CCS_DOWNLOAD_SEGMENT | ((SDO_C->toggle)<<4) | ((7-j)<<1);
            /* is end of transfer? */
            if(SDO_C->bufferOffset == SDO_C->bufferSize){
                SDO_C->CANtxBuff->data[0] |= 1;
            }
            /* Send next SDO message */
            CO_CANsend(SDO_C->CANdevTx, SDO_C->CANtxBuff);
            SDO_C->state = SDO_STATE_DOWNLOAD_RESPONSE;
            return SDO_RETURN_WAITING_SERVER_RESPONSE;
        }

        /*  BLOCK */
        case SDO_STATE_BLOCKDOWNLOAD_INPORGRES:{
            SDO_C->block_seqno += 1;
            SDO_C->CANtxBuff->data[0] = SDO_C->block_seqno;

            if(SDO_C->block_seqno >= SDO_C->block_blksize){
                SDO_C->state = SDO_STATE_BLOCKDOWNLOAD_BLOCK_ACK;
            }
            /*  set data */
            SDO_C->block_noData = 0;

            uint8_t i;
            for(i = 1; i < 8; i++){
                if(SDO_C->bufferOffset < SDO_C->bufferSize){
                    SDO_C->CANtxBuff->data[i] = *(SDO_C->buffer + SDO_C->bufferOffset);
                }
                else{
                    SDO_C->CANtxBuff->data[i] = 0;
                    SDO_C->block_noData += 1;
                }

                SDO_C->bufferOffset += 1;
            }

            if(SDO_C->bufferOffset >= SDO_C->bufferSize){
                SDO_C->CANtxBuff->data[0] |= 0x80;
                SDO_C->block_blksize = SDO_C->block_seqno;
                SDO_C->state = SDO_STATE_BLOCKDOWNLOAD_BLOCK_ACK;
            }

            /*  tx data */
            SDO_C->timeoutTimer = 0;
            CO_CANsend(SDO_C->CANdevTx, SDO_C->CANtxBuff);

            return SDO_RETURN_BLOCKDOWNLOAD_INPROGRES;
        }

        case SDO_STATE_BLOCKDOWNLOAD_CRC:{
            SDO_C->CANtxBuff->data[0] = (CCS_DOWNLOAD_BLOCK<<5) | (SDO_C->block_noData << 2) | 0x01;

            uint16_t tmp16;

            tmp16 = crc16_ccitt((unsigned char *)SDO_C->buffer, (unsigned int)SDO_C->bufferSize, 0);

            SDO_C->CANtxBuff->data[1] = (uint8_t) tmp16;
            SDO_C->CANtxBuff->data[2] = (uint8_t) (tmp16>>8);

            /*  set state */
            SDO_C->state = SDO_STATE_BLOCKDOWNLOAD_CRC_ACK;
            /*  tx data */
            SDO_C->timeoutTimer = 0;
            CO_CANsend(SDO_C->CANdevTx, SDO_C->CANtxBuff);

            return SDO_RETURN_BLOCKDOWNLOAD_INPROGRES;
        }

        default:{
            break;
        }
    }
    return SDO_RETURN_WAITING_SERVER_RESPONSE;
}


/*******************************************************************************
 *
 * UPLOAD
 *
 ******************************************************************************/
int8_t CO_SDOclientUploadInitiate(
        CO_SDOclient_t         *SDO_C,
        uint16_t                index,
        uint8_t                 subIndex,
        uint8_t                *dataRx,
        uint32_t                dataRxSize,
        uint8_t                 blockEnable)
{
    /* verify parameters */
    if(dataRx == 0 || dataRxSize < 4) return -2;

    /* save parameters */
    SDO_C->buffer = dataRx;
    SDO_C->bufferSize = dataRxSize;
    SDO_C->state = 0x40;

    /* prepare CAN tx message */
    CO_SDObufferClear (&SDO_C->CANtxBuff->data[0], 8);

    SDO_C->index = index;
    SDO_C->subIndex = subIndex;

    SDO_C->CANtxBuff->data[1] = index & 0xFF;
    SDO_C->CANtxBuff->data[2] = index >> 8;
    SDO_C->CANtxBuff->data[3] = subIndex;


    if(blockEnable == 0){
        SDO_C->state = SDO_STATE_UPLOAD_INITIATED;
        SDO_C->CANtxBuff->data[0] = (CCS_UPLOAD_INITIATE<<5);
    }
    else{
        SDO_C->state = SDO_STATE_BLOCKUPLOAD_INITIATE;

        /*  header */
        SDO_C->CANtxBuff->data[0] = (CCS_UPLOAD_BLOCK<<5);

        /*  set CRC */
        SDO_C->CANtxBuff->data[0] |= 0x04;

        /*  set number of segments in block */
        SDO_C->block_blksize = SDO_C->block_size_max;
        if ((SDO_C->block_blksize *7) > SDO_C->bufferSize){
            return -3;
        }

        SDO_C->CANtxBuff->data[4] = SDO_C->block_blksize;
        SDO_C->CANtxBuff->data[5] = SDO_C->pst;


        SDO_C->block_seqno = 0;
    }

    /* if nodeIDOfTheSDOServer == node-ID of this node, then exchange data with this node */
    if(SDO_C->SDOClientPar->nodeIDOfTheSDOServer == SDO_C->SDO->nodeId){
        return 0;
    }

    /* empty receive buffer, reset timeout timer and send message */
    SDO_C->CANrxNew = 0;
    SDO_C->timeoutTimer = 0;
    SDO_C->timeoutTimerBLOCK =0;
    CO_CANsend(SDO_C->CANdevTx, SDO_C->CANtxBuff);

    return 0;
}


/******************************************************************************/
int8_t CO_SDOclientUpload(
        CO_SDOclient_t         *SDO_C,
        uint16_t                timeDifference_ms,
        uint16_t                SDOtimeoutTime,
        uint32_t               *pDataSize,
        uint32_t               *pSDOabortCode)
{
    uint16_t indexTmp;
    uint32_t tmp32;

    /* clear abort code */
    *pSDOabortCode = 0;

    /* if nodeIDOfTheSDOServer == node-ID of this node, then exchange data with this node */
    if(SDO_C->SDO && SDO_C->SDOClientPar->nodeIDOfTheSDOServer == SDO_C->SDO->nodeId){
        /* If SDO server is busy return error */
        if(SDO_C->SDO->state != 0){
            *pSDOabortCode = 0x06040047L;    /* General internal incompatibility in the device. */
            return -9;
        }

        /* init ODF_arg */
        *pSDOabortCode = CO_SDO_initTransfer(SDO_C->SDO, SDO_C->index, SDO_C->subIndex);
        if(*pSDOabortCode){
            return -10;
        }

        /* set buffer and length if domain */
        SDO_C->SDO->ODF_arg.data = SDO_C->buffer;
        if(SDO_C->SDO->ODF_arg.ODdataStorage == 0)
            SDO_C->SDO->ODF_arg.dataLength = SDO_C->bufferSize;

        /* read data from the Object dictionary */
        *pSDOabortCode = CO_SDO_readOD(SDO_C->SDO, SDO_C->bufferSize);
        if(*pSDOabortCode){
            return -10;
        }

        /* set data size */
        *pDataSize = SDO_C->SDO->ODF_arg.dataLength;

        /* is SDO buffer too small */
        if(SDO_C->SDO->ODF_arg.lastSegment == 0){
            *pSDOabortCode = 0x05040005L;    /* Out of memory */
            return -10;
        }

        SDO_C->state = 0;
        SDO_C->CANrxNew = 0;
        return 0;
    }


/*  RX data ******************************************************************************** */
    if(SDO_C->CANrxNew){
        uint8_t SCS = SDO_C->CANrxData[0]>>5;    /* Client command specifier */

        /*  ABORT */
        if (SDO_C->CANrxData[0] == (SCS_ABORT<<5)){
            SDO_C->state = SDO_STATE_IDLE;
            SDO_C->CANrxNew =0;
            memcpySwap4((uint8_t*)pSDOabortCode , &SDO_C->CANrxData[4]);
            return SDO_RETURN_END_SERVERABORT;
        }
        switch (SDO_C->state){
            case SDO_STATE_UPLOAD_INITIATED:{



                if (SCS == SCS_UPLOAD_INITIATE){
                    if(SDO_C->CANrxData[0] & 0x02){
                        uint8_t size;
                        /* Expedited transfer */
                        if(SDO_C->CANrxData[0] & 0x01)/* is size indicated */
                            size = 4 - ((SDO_C->CANrxData[0]>>2)&0x03);    /* size */
                        else
                            size = 4;

                        *pDataSize = size;

                        /* copy data */
                        while(size--) SDO_C->buffer[size] = SDO_C->CANrxData[4+size];
                        SDO_C->state = 0;
                        SDO_C->CANrxNew = 0;

                        return SDO_RETURN_COMMUNICATION_END;
                    }
                    else{
                        /* segmented transfer - prepare first segment */
                        SDO_C->bufferOffset = 0;
                        SDO_C->state = SDO_STATE_UPLOAD_REQUEST;

                        SDO_C->toggle =0;
                        /* continue with segmented upload */
                    }
                }
                else{
                    *pSDOabortCode = 0x05040001L; /* Client/server command specifier not valid or unknown */
                    SDO_C->state = SDO_STATE_ABORT;
                }
                break;
            }

            case SDO_STATE_UPLOAD_RESPONSE:{
                if (SCS == SCS_UPLOAD_SEGMENT){
                     uint16_t size, i;
                    /* verify toggle bit */
                    if((SDO_C->CANrxData[0] &0x10) != (~SDO_C->toggle &0x10)){
                        *pSDOabortCode = 0x05030000L;    /* toggle bit not alternated */
                        SDO_C->state = SDO_STATE_ABORT;
                        break;
                    }
                    /* get size */
                    size = 7 - ((SDO_C->CANrxData[0]>>1)&0x07);
                    /* verify length */
                    if((SDO_C->bufferOffset + size) > SDO_C->bufferSize){
                        *pSDOabortCode = 0x05040005L;    /* Out of memory */
                        SDO_C->state = SDO_STATE_ABORT;
                        break;
                    }
                    /* copy data to buffer */
                    for(i=0; i<size; i++)
                    SDO_C->buffer[SDO_C->bufferOffset + i] = SDO_C->CANrxData[1 + i];
                    SDO_C->bufferOffset += size;
                    /* If no more segments to be uploaded, finish communication */
                    if(SDO_C->CANrxData[0] & 0x01){
                        *pDataSize = SDO_C->bufferOffset;
                        SDO_C->state = SDO_STATE_NOTDEFINED;
                        SDO_C->CANrxNew = 0;
                        return SDO_RETURN_COMMUNICATION_END;
                    }
                    /* set state */
                    SDO_C->state = SDO_STATE_UPLOAD_REQUEST;
                    break;
                }
                else{
                    *pSDOabortCode = 0x05040001L; /* Client/server command specifier not valid or unknown */
                    SDO_C->state = SDO_STATE_ABORT;
                }
                break;
            }

            case SDO_STATE_BLOCKUPLOAD_INITIATE:{
                if (SCS == SCS_UPLOAD_BLOCK){ /*  block upload initiate response */

                    SDO_C->state = SDO_STATE_BLOCKUPLOAD_INITIATE_ACK;

                    /*  SCR support */
                    if((SDO_C->CANrxData[0] & 0x04) != 0)
                        SDO_C->crcEnabled = 1; /*  CRC suported */
                    else
                        SDO_C->crcEnabled = 0; /*  CRC not suported */

                    /*  chech Index ans subnindex */
                    indexTmp = SDO_C->CANrxData[2];
                    indexTmp = indexTmp << 8;
                    indexTmp |= SDO_C->CANrxData[1];

                    if(indexTmp != SDO_C->index || SDO_C->CANrxData[3] != SDO_C->subIndex){
                        *pSDOabortCode = 0x06040043L;
                        SDO_C->state = SDO_STATE_ABORT;
                    }

                    /*  set length */
                    if(SDO_C->CANrxData[0]&0x02){
                        uint32_t len;
                        memcpySwap4((uint8_t*)&len, &SDO_C->CANrxData[4]);
                        SDO_C->dataSize = len;
                    }
                    else{
                        SDO_C->dataSize = 0;
                    }

                    /*  check available buffer size */
                    if (SDO_C->dataSize > SDO_C->bufferSize){
                        *pSDOabortCode = 0x05040005;
                        SDO_C->state = SDO_STATE_ABORT;
                    }

                    SDO_C->dataSizeTransfered =0;
                }
                else if (SCS == SCS_UPLOAD_INITIATE){ /*  switch to regular segmented transfer */
                    if(SDO_C->CANrxData[0] & 0x02){
                        uint8_t size;
                        /* Expedited transfer */
                        if(SDO_C->CANrxData[0] & 0x01)/* is size indicated */
                            size = 4 - ((SDO_C->CANrxData[0]>>2)&0x03);    /* size */
                        else
                            size = 4;

                        *pDataSize = size;

                        /* copy data */
                        while(size--) SDO_C->buffer[size] = SDO_C->CANrxData[4+size];
                        SDO_C->state = 0;
                        SDO_C->CANrxNew = 0;

                        return SDO_RETURN_COMMUNICATION_END;
                    }
                    else{
                        /* segmented transfer - prepare first segment */
                        SDO_C->bufferOffset = 0;
                        SDO_C->state = SDO_STATE_UPLOAD_REQUEST;

                        SDO_C->toggle =0;
                        /* continue with segmented upload */
                    }
                }
                else{ /*  unknown SCS */
                    *pSDOabortCode = 0x05040001L; /* Client command specifier not valid or unknown. */
                    SDO_C->state = SDO_STATE_ABORT;
                }
                break;
            }

            case SDO_STATE_BLOCKUPLOAD_INPROGRES:{
                /*  check correct seqno */
                SDO_C->timeoutTimerBLOCK = 0;
                if((SDO_C->CANrxData[0] & 0x7f) != (SDO_C->block_seqno +1)){


                    /*  wait block tmo to send block ack */
                    if (SDO_C->block_seqno ==0){
                    }
                    else if ((SDO_C->CANrxData[0] & 0x7f) != SDO_C->block_seqno ){
                        SDO_C->state = SDO_STATE_BLOCKUPLOAD_BLOCK_ACK;
                    }
                    else if (SDO_C->CANrxData[0] & 0x80)
                        SDO_C->state = SDO_STATE_BLOCKUPLOAD_BLOCK_ACK_LAST;

                }
                else{
                    uint8_t i;
                    for (i=1; i<8; i++){
                        *(SDO_C->buffer + SDO_C->dataSizeTransfered) = SDO_C->CANrxData[i];
                        SDO_C->dataSizeTransfered +=1;
                        if (SDO_C->dataSizeTransfered >= SDO_C->bufferSize){
                            *pSDOabortCode = 0x05040005;
                            SDO_C->state = SDO_STATE_ABORT;
                            break;
                        }
                    }
                    SDO_C->block_seqno += 1;

                    if (*pSDOabortCode ==0){
                        if (SDO_C->CANrxData[0]&0x80){
                            if (SDO_C->dataSize !=0){
                                if (SDO_C->dataSize > SDO_C->dataSizeTransfered){
                                    *pSDOabortCode = 0x06070010L;
                                    SDO_C->state = SDO_STATE_ABORT;
                                }
                                else
                                    SDO_C->state = SDO_STATE_BLOCKUPLOAD_BLOCK_ACK_LAST;
                            }
                            else
                                SDO_C->state = SDO_STATE_BLOCKUPLOAD_BLOCK_ACK_LAST;
                        }
                        else{
                            if(SDO_C->block_seqno >= SDO_C->block_blksize){ /*  last segment in block */
                                    SDO_C->state = SDO_STATE_BLOCKUPLOAD_BLOCK_ACK;
                            }

                        }
                    }
                    SDO_C->CANrxNew = 0;
                }
                break;
            }

            case SDO_STATE_BLOCKUPLOAD_BLOCK_CRC:{
                if (SCS == SCS_UPLOAD_BLOCK){
                    tmp32 = ((SDO_C->CANrxData[0]>>2) & 0x07);
                    SDO_C->dataSizeTransfered -= tmp32;

                    SDO_C->state = SDO_STATE_BLOCKUPLOAD_BLOCK_END;
                    if (SDO_C->crcEnabled){
                        uint16_t tmp16;
                        memcpySwap2((uint8_t*)&tmp16, &SDO_C->CANrxData[1]);

                        if (tmp16 != crc16_ccitt((unsigned char *)SDO_C->buffer, (unsigned int)SDO_C->dataSizeTransfered, 0)){
                            *pSDOabortCode = 0x05040004L;
                            SDO_C->state = SDO_STATE_ABORT;
                        }
                        else{
                            SDO_C->state = SDO_STATE_BLOCKUPLOAD_BLOCK_END;
                        }
                    }
                    else{
                        SDO_C->state = SDO_STATE_BLOCKUPLOAD_BLOCK_END;
                    }

                }
                else{
                    *pSDOabortCode = 0x08000000L;
                    SDO_C->state = SDO_STATE_ABORT;
                }
                break;
            }

            default:{
                *pSDOabortCode = 0x05040001L; /* Client command specifier not valid or unknown. */
                SDO_C->state = SDO_STATE_ABORT;
            }
        }
        SDO_C->timeoutTimer = 0;
        SDO_C->CANrxNew = 0;
    }

/*  TMO *************************************************************************************************** */
    if(SDO_C->timeoutTimer < SDOtimeoutTime){
        SDO_C->timeoutTimer += timeDifference_ms;
        if (SDO_C->state == SDO_STATE_BLOCKUPLOAD_INPROGRES)
            SDO_C->timeoutTimerBLOCK += timeDifference_ms;
    }
    if(SDO_C->timeoutTimer >= SDOtimeoutTime){ /*  comunication TMO */
        *pSDOabortCode = 0x05040000L;    /* SDO protocol timed out */
        CO_SDOclient_abort(SDO_C, *pSDOabortCode);
        return SDO_RETURN_END_TMO;
    }
    if(SDO_C->timeoutTimerBLOCK >= (SDOtimeoutTime/2)){ /*  block TMO */
        SDO_C->state = SDO_STATE_BLOCKUPLOAD_BLOCK_ACK;
    }


/*  TX data ******************************************************************************** */
    CO_SDObufferClear (&SDO_C->CANtxBuff->data[0], 8);

    switch (SDO_C->state){
        case SDO_STATE_ABORT:{
            SDO_C->state = SDO_STATE_NOTDEFINED;
            CO_SDOclient_abort (SDO_C, *pSDOabortCode);
            return SDO_RETURN_END_SERVERABORT;
        }

        /*  SEGMENTED UPLOAD */
        case SDO_STATE_UPLOAD_REQUEST:{
            SDO_C->CANtxBuff->data[0] = (CCS_UPLOAD_SEGMENT<<5) | (SDO_C->toggle & 0x10);
            CO_CANsend(SDO_C->CANdevTx, SDO_C->CANtxBuff);

            SDO_C->state = SDO_STATE_UPLOAD_RESPONSE;
            SDO_C->toggle = ~SDO_C->toggle;

            return SDO_RETURN_WAITING_SERVER_RESPONSE;
        }
        /*  BLOCK */
        case SDO_STATE_BLOCKUPLOAD_INITIATE_ACK:{
            SDO_C->timeoutTimerBLOCK = 0;
            SDO_C->block_seqno = 0;
            SDO_C->state = SDO_STATE_BLOCKUPLOAD_INPROGRES;

            /*  header */
            SDO_C->CANtxBuff->data[0] = (CCS_UPLOAD_BLOCK<<5) | 0x03;
            CO_CANsend(SDO_C->CANdevTx, SDO_C->CANtxBuff);

            return SDO_RETURN_BLOCKUPLOAD_INPROGRES;
        }

        case SDO_STATE_BLOCKUPLOAD_BLOCK_ACK_LAST:{
            /*  header */
            SDO_C->CANtxBuff->data[0] = (CCS_UPLOAD_BLOCK<<5) | 0x02;
            SDO_C->CANtxBuff->data[1] = SDO_C->block_seqno;

            SDO_C->block_seqno = 0;
            SDO_C->timeoutTimerBLOCK = 0;

            SDO_C->state = SDO_STATE_BLOCKUPLOAD_BLOCK_CRC;

            SDO_C->CANtxBuff->data[2] = SDO_C->block_blksize;
            CO_CANsend(SDO_C->CANdevTx, SDO_C->CANtxBuff);

            return SDO_RETURN_BLOCKUPLOAD_INPROGRES;
        }

        case SDO_STATE_BLOCKUPLOAD_BLOCK_ACK:{
            /*  header */
            SDO_C->CANtxBuff->data[0] = (CCS_UPLOAD_BLOCK<<5) | 0x02;
            SDO_C->CANtxBuff->data[1] = SDO_C->block_seqno;

            /*  set next block size */
            if (SDO_C->dataSize != 0){
                if(SDO_C->dataSizeTransfered >= SDO_C->dataSize){
                    SDO_C->block_blksize = 0;
                    SDO_C->state = SDO_STATE_BLOCKUPLOAD_BLOCK_CRC;
                }
                else{
                    tmp32 = ((SDO_C->dataSize - SDO_C->dataSizeTransfered) / 7);
                    if(tmp32 >= SDO_C->block_size_max){
                        SDO_C->block_blksize = SDO_C->block_size_max;
                    }
                    else{
                        if((SDO_C->dataSize - SDO_C->dataSizeTransfered) % 7 == 0)
                            SDO_C->block_blksize = tmp32;
                        else
                            SDO_C->block_blksize = tmp32 + 1;
                    }
                    SDO_C->block_seqno = 0;
                    SDO_C->timeoutTimerBLOCK = 0;
                    SDO_C->state = SDO_STATE_BLOCKUPLOAD_INPROGRES;
                }
            }
            else{
                SDO_C->block_seqno = 0;
                SDO_C->timeoutTimerBLOCK = 0;

                SDO_C->state = SDO_STATE_BLOCKUPLOAD_INPROGRES;
            }
            SDO_C->CANtxBuff->data[2] = SDO_C->block_blksize;
            CO_CANsend(SDO_C->CANdevTx, SDO_C->CANtxBuff);

            return SDO_RETURN_BLOCKUPLOAD_INPROGRES;
        }

        case SDO_STATE_BLOCKUPLOAD_BLOCK_END:{
            SDO_C->CANtxBuff->data[0] = (CCS_UPLOAD_BLOCK<<5) | 0x01;

            CO_CANsend(SDO_C->CANdevTx, SDO_C->CANtxBuff);

            *pDataSize = SDO_C->dataSizeTransfered;
            return SDO_RETURN_COMMUNICATION_END;
        }

        default:
            break;
    }

    return SDO_RETURN_WAITING_SERVER_RESPONSE;
}
