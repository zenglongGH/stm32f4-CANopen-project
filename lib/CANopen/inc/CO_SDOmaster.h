/**
 * CANopen Service Data Object - client protocol.
 *
 * @file        CO_SDOmaster.h
 * @ingroup     CO_SDOmaster
 * @version     SVN: \$Id: CO_SDOmaster.h 32 2013-03-11 08:24:27Z jani22 $
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


#ifndef _CO_SDO_CLIENT_H
#define _CO_SDO_CLIENT_H


/**
 * @defgroup CO_SDOmaster SDO client
 * @ingroup CO_CANopen
 * @{
 *
 * CANopen Service Data Object - client protocol.
 *
 * @see @ref CO_SDO
 */


/**
 * SDO Client Parameter. The same as record from Object dictionary (index 0x1280+).
 */
typedef struct{
    /** Equal to 3 */
    uint8_t             maxSubIndex;
    /** Communication object identifier for client transmission. Meaning of the specific bits:
        - Bit 0...10: 11-bit CAN identifier.
        - Bit 11..30: reserved, set to 0.
        - Bit 31: if 1, SDO client object is not used. */
    uint32_t            COB_IDClientToServer;
    /** Communication object identifier for message received from server. Meaning of the specific bits:
        - Bit 0...10: 11-bit CAN identifier.
        - Bit 11..30: reserved, set to 0.
        - Bit 31: if 1, SDO client object is not used. */
    uint32_t            COB_IDServerToClient;
    /** Node-ID of the SDO server */
    uint8_t             nodeIDOfTheSDOServer;
}CO_SDOclientPar_t;


/**
 * SDO client object
 */
typedef struct{
    /** From CO_SDOclient_init() */
    CO_SDOclientPar_t  *SDOClientPar;
    /** From CO_SDOclient_init() */
    CO_SDO_t           *SDO;
    /** Internal state of the SDO client */
    uint8_t             state;
    /** Pointer to data buffer supplied by user */
    uint8_t            *buffer;
    /** By download application indicates data size in buffer.
    By upload application indicates buffer size */
    uint32_t            bufferSize;
    /** Offset in buffer of next data segment being read/written */
    uint32_t            bufferOffset;
    /** Acknowledgement */
    uint32_t            bufferOffsetACK;
    /** data length to be uploaded in block transfer */
    uint32_t            dataSize;
    /** Data length transfered in block transfer */
    uint32_t            dataSizeTransfered;
    /** Timeout timer for SDO communication */
    uint16_t            timeoutTimer;
    /** Timeout timer for SDO block transfer */
    uint16_t            timeoutTimerBLOCK;
    /** Index of current object in Object Dictionary */
    uint16_t            index;
    /** Subindex of current object in Object Dictionary */
    uint8_t             subIndex;
    /** From CO_SDOclient_init() */
    CO_CANmodule_t     *CANdevRx;
    /** From CO_SDOclient_init() */
    uint16_t            CANdevRxIdx;
    /** Flag indicates, if new SDO message received from CAN bus.
    It is not cleared, until received message is completely processed. */
    uint16_t            CANrxNew;      /* must be 2-byte variable because of correct alignment of CANrxData */
    /** 8 data bytes of the received message */
    uint8_t             CANrxData[8];  /* take care for correct (word) alignment! */
    /** Pointer to optional external function. If defined, it is called from high
    priority interrupt after new CAN SDO response message is received. Function
    may wake up external task, which processes SDO client functions */
    void              (*pFunctSignal)(uint32_t arg);
    /** Optional argument, which is passed to above function */
    uint32_t            functArg;
    /** From CO_SDOclient_init() */
    CO_CANmodule_t     *CANdevTx;
    /** CAN transmit buffer inside CANdevTx for CAN tx message */
    CO_CANtx_t         *CANtxBuff;
    /** From CO_SDOclient_init() */
    uint16_t            CANdevTxIdx;
    /** Toggle bit toggled with each subsequent in segmented transfer */
    uint8_t             toggle;
    /** Server threshold for switch back to segmented transfer, if data size is small.
    Set in CO_SDOclient_init(). Can be changed by application. 0 Disables switching. */
    uint8_t             pst;
    /** Maximum number of segments in one block. Set in CO_SDOclient_init(). Can
    be changed by application to 2 .. 127. */
    uint8_t             block_size_max;
    /** Last sector number */
    uint8_t             block_seqno;
    /** Block size in current transfer */
    uint8_t             block_blksize;
    /** Number of bytes in last segment that do not contain data */
    uint8_t             block_noData;
    /** Server CRC support in block transfer */
    uint8_t             crcEnabled;

}CO_SDOclient_t;


/**
 * Initialize SDO client object.
 *
 * Function must be called in the communication reset section.
 *
 * @param SDO_C This object will be initialized.
 * @param SDO SDO server object. It is used in case, if client is accessing
 * object dictionary from its own device. If NULL, it will be ignored.
 * @param SDOClientPar Pointer to _SDO Client Parameter_ record from Object
 * dictionary (index 0x1280+). Will be written.
 * @param CANdevRx CAN device for SDO client reception.
 * @param CANdevRxIdx Index of receive buffer in the above CAN device.
 * @param CANdevTx CAN device for SDO client transmission.
 * @param CANdevTxIdx Index of transmit buffer in the above CAN device.
 *
 * @return #CO_ReturnError_t: CO_ERROR_NO or CO_ERROR_ILLEGAL_ARGUMENT.
 */
int16_t CO_SDOclient_init(
        CO_SDOclient_t         *SDO_C,
        CO_SDO_t               *SDO,
        CO_SDOclientPar_t      *SDOClientPar,
        CO_CANmodule_t         *CANdevRx,
        uint16_t                CANdevRxIdx,
        CO_CANmodule_t         *CANdevTx,
        uint16_t                CANdevTxIdx);


/**
 * Setup SDO client object.
 *
 * Function must be called before new SDO communication. If previous SDO
 * communication was with the same node, function does not need to be called.
 *
 * @param SDO_C This object.
 * @param COB_IDClientToServer See CO_SDOclientPar_t. If zero, then
 * nodeIDOfTheSDOServer is used with default COB-ID.
 * @param COB_IDServerToClient See CO_SDOclientPar_t. If zero, then
 * nodeIDOfTheSDOServer is used with default COB-ID.
 * @param nodeIDOfTheSDOServer Node-ID of the SDO server. If zero, SDO client
 * object is not used. If it is the same as node-ID of this node, then data will
 * be exchanged with this node (without CAN communication).
 *
 * @return 0: Success
 * @return -2: Wrong arguments
 */
int8_t CO_SDOclient_setup(
        CO_SDOclient_t         *SDO_C,
        uint32_t                COB_IDClientToServer,
        uint32_t                COB_IDServerToClient,
        uint8_t                 nodeIDOfTheSDOServer);


/**
 * Initiate SDO download communication.
 *
 * Function initiates SDO download communication with server specified in
 * CO_SDOclient_init() function. Data will be written to remote node.
 * Function is nonblocking.
 *
 * @param SDO_C This object.
 * @param index Index of object in object dictionary in remote node.
 * @param subIndex Subindex of object in object dictionary in remote node.
 * @param dataTx Pointer to data to be written. Data must be valid untill end
 * of communication. Note that data are aligned in little-endian
 * format, because CANopen itself uses little-endian. Take care,
 * when using processors with big-endian.
 * @param dataSize Size of data in dataTx.
 * @param blockEnable Try to initiate block transfer.
 *
 * @return 0: Success
 * @return -2: Wrong arguments
 */
int8_t CO_SDOclientDownloadInitiate(
        CO_SDOclient_t         *SDO_C,
        uint16_t                index,
        uint8_t                 subIndex,
        uint8_t                *dataTx,
        uint32_t                dataSize,
        uint8_t                 blockEnable);


/**
 * Process SDO download communication.
 *
 * Function must be called cyclically untill it returns <=0. It Proceeds SDO
 * download communication initiated with CO_SDOclientDownloadInitiate().
 * Function is nonblocking.
 *
 * @param SDO_C This object.
 * @param timeDifference_ms Time difference from previous function call in [milliseconds].
 * @param SDOtimeoutTime Timeout time for SDO communication in milliseconds.
 * @param pSDOabortCode Pointer to external variable written by this function
 * in case of error in communication.
 *
 * @return 2: Server responded, new client request was sent.
 * @return 1: Waiting for server response.
 * @return 0: End of communication.
 * @return -1: SDO BLOCK initiate failed, try segmented.
 * @return -3: Error: communication was not properly initiated.
 * @return -9: Error: SDO server busy.
 * @return -10: Error in SDO communication. SDO abort code is in value pointed by pSDOabortCode.
 * @return -11: Error: timeout in SDO communication, SDO abort code is in value pointed by pSDOabortCode.
 */
int8_t CO_SDOclientDownload(
        CO_SDOclient_t         *SDO_C,
        uint16_t                timeDifference_ms,
        uint16_t                SDOtimeoutTime,
        uint32_t               *pSDOabortCode);


/**
 * Initiate SDO upload communication.
 *
 * Function initiates SDO upload communication with server specified in
 * CO_SDOclient_init() function. Data will be read from remote node.
 * Function is nonblocking.
 *
 * @param SDO_C This object.
 * @param index Index of object in object dictionary in remote node.
 * @param subIndex Subindex of object in object dictionary in remote node.
 * @param dataRx Pointer to data buffer data will be written. Buffer must be
 * valid until end of communication. Note that data are aligned
 * in little-endian format, because CANopen itself uses
 * little-endian. Take care, when using processors with big-endian.
 * @param dataRxSize Size of dataRx.
 * @param blockEnable Try to initiate block transfer.
 *
 * @return 0: Success.
 * @return -2: Wrong arguments.
 * @return -3: Too small buffer size.
 */
int8_t CO_SDOclientUploadInitiate(
        CO_SDOclient_t         *SDO_C,
        uint16_t                index,
        uint8_t                 subIndex,
        uint8_t                *dataRx,
        uint32_t                dataRxSize,
        uint8_t                 blockEnable);


/**
 * Process SDO upload communication.
 *
 * Function must be called cyclically until it returns <=0. It Proceeds SDO
 * upload communication initiated with CO_SDOclientUploadInitiate().
 * Function is nonblocking.
 *
 * @param SDO_C This object.
 * @param timeDifference_ms Time difference from previous function call in [milliseconds].
 * @param SDOtimeoutTime Timeout time for SDO communication in milliseconds.
 * @param pDataSize pointer to external variable, where size of received
 * data will be written.
 * @param pSDOabortCode Pointer to external variable written by this function
 * in case of error in communication.
 *
 * @return 3: Block upload in progress.
 * @return 2: Server responded, new client request was sent.
 * @return 1: Waiting for server response.
 * @return 0: End of communication.
 * @return -3: Error: communication was not properly initiated.
 * @return -9: Error: SDO server busy.
 * @return -10: Error in SDO communication. SDO abort code is in value pointed by pSDOabortCode.
 * @return -11: Error: timeout in SDO communication, SDO abort code is in value pointed by pSDOabortCode.
 */
int8_t CO_SDOclientUpload(
        CO_SDOclient_t         *SDO_C,
        uint16_t                timeDifference_ms,
        uint16_t                SDOtimeoutTime,
        uint32_t               *pDataSize,
        uint32_t               *pSDOabortCode);


/** @} */
#endif
