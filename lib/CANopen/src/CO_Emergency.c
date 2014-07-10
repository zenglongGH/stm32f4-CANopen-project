/*
 * CANopen Emergency object.
 *
 * @file        CO_Emergency.c
 * @ingroup     CO_Emergency
 * @version     SVN: \$Id: CO_Emergency.c 32 2013-03-11 08:24:27Z jani22 $
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

#include <stdlib.h> /*  for malloc, free */


/*
 * Function for accessing _Pre-Defined Error Field_ (index 0x1003) from SDO server.
 *
 * For more information see file CO_SDO.h.
 */
static uint32_t CO_ODF_1003(CO_ODF_arg_t *ODF_arg){
    CO_EMpr_t *EMpr;
    uint8_t *value;

    EMpr = (CO_EMpr_t*) ODF_arg->object;
    value = (uint8_t*) ODF_arg->data;

    if(ODF_arg->reading){
        uint8_t noOfErrors;
        noOfErrors = EMpr->preDefErrNoOfErrors;

        if(ODF_arg->subIndex == 0)
            *value = noOfErrors;
        else if(ODF_arg->subIndex > noOfErrors)
            return 0x08000024L;  /* No data available. */
    }
    else{
        /* only '0' may be written to subIndex 0 */
        if(ODF_arg->subIndex == 0){
            if(*value == 0)
                EMpr->preDefErrNoOfErrors = 0;
            else
                return 0x06090030L;  /* Invalid value for parameter */
        }
        else
            return 0x06010002L;  /* Attempt to write a read only object. */
    }

    return 0;
}


/*
 * Function for accessing _COB ID EMCY_ (index 0x1014) from SDO server.
 *
 * For more information see file CO_SDO.h.
 */
static uint32_t CO_ODF_1014(CO_ODF_arg_t *ODF_arg){
    uint8_t *nodeId;
    uint32_t *value;

    nodeId = (uint8_t*) ODF_arg->object;
    value = (uint32_t*) ODF_arg->data;

    /* add nodeId to the value */
    if(ODF_arg->reading)
        *value += *nodeId;

    return 0;
}


/******************************************************************************/
int16_t CO_EM_init(
        CO_EM_t                *EM,
        CO_EMpr_t              *EMpr,
        CO_SDO_t               *SDO,
        uint8_t                *errorStatusBits,
        uint8_t                 errorStatusBitsSize,
        uint8_t                *errorRegister,
        uint32_t               *preDefErr,
        uint8_t                 preDefErrSize,
        CO_CANmodule_t         *CANdev,
        uint16_t                CANdevTxIdx,
        uint16_t                CANidTxEM)
{
    uint8_t i;

    /* Configure object variables */
    EM->errorStatusBits         = errorStatusBits;
    EM->errorStatusBitsSize     = errorStatusBitsSize; if(errorStatusBitsSize < 6) return CO_ERROR_ILLEGAL_ARGUMENT;
    EM->bufEnd                  = EM->buf + CO_EM_INTERNAL_BUFFER_SIZE * 8;
    EM->bufWritePtr             = EM->buf;
    EM->bufReadPtr              = EM->buf;
    EM->bufFull                 = 0;
    EM->wrongErrorReport        = 0;
    EM->errorReportBusy         = 0;
    EM->errorReportBusyError    = 0;
    EMpr->EM                    = EM;
    EMpr->errorRegister         = errorRegister;
    EMpr->preDefErr             = preDefErr;
    EMpr->preDefErrSize         = preDefErrSize;
    EMpr->preDefErrNoOfErrors   = 0;
    EMpr->inhibitEmTimer        = 0;

    /* clear error status bits */
    for(i=0; i<errorStatusBitsSize; i++) EM->errorStatusBits[i] = 0;

    /* Configure Object dictionary entry at index 0x1003 and 0x1014 */
    CO_OD_configure(SDO, 0x1003, CO_ODF_1003, (void*)EMpr, 0, 0);
    CO_OD_configure(SDO, 0x1014, CO_ODF_1014, (void*)&SDO->nodeId, 0, 0);

    /* configure emergency message CAN transmission */
    EMpr->CANdev = CANdev;
    EMpr->CANdev->EM = (void*)EM; /* update pointer inside CAN device. */
    EMpr->CANtxBuff = CO_CANtxBufferInit(
            CANdev,             /* CAN device */
            CANdevTxIdx,        /* index of specific buffer inside CAN module */
            CANidTxEM,          /* CAN identifier */
            0,                  /* rtr */
            8,                  /* number of data bytes */
            0);                 /* synchronous message flag bit */

    return CO_ERROR_NO;
}


/******************************************************************************/
void CO_EM_process(
        CO_EMpr_t              *EMpr,
        uint8_t                 NMTisPreOrOperational,
        uint16_t                timeDifference_100us,
        uint16_t                EMinhTime)
{

    CO_EM_t *EM = EMpr->EM;
    uint8_t errorRegister;

    /* verify errors from driver and other */
    CO_CANverifyErrors(EMpr->CANdev);
    if(EM->errorReportBusyError){
        CO_errorReport(EM, ERROR_ERROR_REPORT_BUSY, EM->errorReportBusyError);
        EM->errorReportBusyError = 0;
    }
    if(EM->wrongErrorReport){
        CO_errorReport(EM, ERROR_WRONG_ERROR_REPORT, EM->wrongErrorReport);
        EM->wrongErrorReport = 0;
    }


    /* calculate Error register */
    errorRegister = 0;
    /* generic error */
    if(EM->errorStatusBits[5])
        errorRegister |= 0x01;
        
    /* communication error (overrun, error state) */
    if(EM->errorStatusBits[2] || EM->errorStatusBits[3]){
        printf("EM->errorStatusBits[2] || EM->errorStatusBits[3]\n\r");
        errorRegister |= 0x10;
    }
    *EMpr->errorRegister = (*EMpr->errorRegister & 0xEE) | errorRegister;

    /* inhibit time */
    if(EMpr->inhibitEmTimer < EMinhTime) EMpr->inhibitEmTimer += timeDifference_100us;

    /* send Emergency message. */
    if(     NMTisPreOrOperational &&
            !EMpr->CANtxBuff->bufferFull &&
            EMpr->inhibitEmTimer >= EMinhTime &&
            (EM->bufReadPtr != EM->bufWritePtr || EM->bufFull))
    {
        /* copy data from emergency buffer into CAN buffer and preDefinedErrorField buffer */
        uint8_t* EMdataPtr = EM->bufReadPtr;
        uint8_t* CANtxData = EMpr->CANtxBuff->data;
        uint32_t preDEF;
        uint8_t* ppreDEF = (uint8_t*) &preDEF;
        *(CANtxData++) = *EMdataPtr; *(ppreDEF++) = *(EMdataPtr++);
        *(CANtxData++) = *EMdataPtr; *(ppreDEF++) = *(EMdataPtr++);
        *(CANtxData++) = *EMpr->errorRegister; *(ppreDEF++) = *EMpr->errorRegister; EMdataPtr++;
        *(CANtxData++) = *EMdataPtr; *(ppreDEF++) = *(EMdataPtr++);
        *(CANtxData++) = *(EMdataPtr++);
        *(CANtxData++) = *(EMdataPtr++);
        *(CANtxData++) = *(EMdataPtr++);
        *(CANtxData++) = *(EMdataPtr++);

        /* Update read buffer pointer and reset inhibit timer */
        if(EMdataPtr == EM->bufEnd) EM->bufReadPtr = EM->buf;
        else                        EM->bufReadPtr = EMdataPtr;
        EMpr->inhibitEmTimer = 0;

        /* verify message buffer overflow, then clear full flag */
        if(EM->bufFull == 2){
            EM->bufFull = 0;
            CO_errorReport(EM, ERROR_EMERGENCY_BUFFER_FULL, 0);
        }
        else
            EM->bufFull = 0;

        /* write to 'pre-defined error field' (object dictionary, index 0x1003) */
        if(EMpr->preDefErr){
            uint8_t i;

            if(EMpr->preDefErrNoOfErrors < EMpr->preDefErrSize)
                EMpr->preDefErrNoOfErrors++;
            for(i=EMpr->preDefErrNoOfErrors-1; i>0; i--)
                EMpr->preDefErr[i] = EMpr->preDefErr[i-1];
            EMpr->preDefErr[0] = preDEF;
        }

        CO_CANsend(EMpr->CANdev, EMpr->CANtxBuff);
    }

    return;
}


/******************************************************************************/
int8_t CO_errorReport(CO_EM_t *EM, uint8_t errorBit, uint16_t errorCode, uint32_t infoCode){
    uint8_t index = errorBit >> 3;
    uint8_t bitmask = 1 << (errorBit & 0x7);
    uint8_t *errorStatusBits = &EM->errorStatusBits[index];
    uint8_t *bufWritePtrCopy;

    /* if error was allready reported, return */
    if((*errorStatusBits & bitmask) != 0) return 0;

    if(!EM) return -1;

    /* if errorBit value not supported, send emergency 'ERROR_WRONG_ERROR_REPORT' */
    if(index >= EM->errorStatusBitsSize){
        EM->wrongErrorReport = errorBit;
        return -1;
    }

    /* set error bit */
    if(errorBit) *errorStatusBits |= bitmask; /* any error except NO_ERROR */

    /* set busy flag. If allready busy, prepare for emergency and return. */
    DISABLE_INTERRUPTS();
    if(EM->errorReportBusy++){
        EM->errorReportBusy--;
        ENABLE_INTERRUPTS();
        EM->errorReportBusyError = errorBit;
        return -3;
    }
    ENABLE_INTERRUPTS();

    /* verify buffer full */
    if(EM->bufFull){
        EM->bufFull = 2;
        EM->errorReportBusy--;
        return -2;
    }

    /* copy data for emergency message */
    bufWritePtrCopy = EM->bufWritePtr;
    memcpySwap2(bufWritePtrCopy, (uint8_t*)&errorCode);
    bufWritePtrCopy += 3;   /* third bit is Error register - written later */
    *(bufWritePtrCopy++) = errorBit;
    memcpySwap4(bufWritePtrCopy, (uint8_t*)&infoCode);
    bufWritePtrCopy += 4;

    /* Update write buffer pointer */
    if(bufWritePtrCopy == EM->bufEnd) EM->bufWritePtr = EM->buf;
    else                              EM->bufWritePtr = bufWritePtrCopy;

    /* verify buffer full, clear busy flag and return */
    if(EM->bufWritePtr == EM->bufReadPtr) EM->bufFull = 1;
    EM->errorReportBusy--;
    return 1;
}


/******************************************************************************/
int8_t CO_errorReset(CO_EM_t *EM, uint8_t errorBit, uint16_t errorCode, uint32_t infoCode){
    uint8_t index = errorBit >> 3;
    uint8_t bitmask = 1 << (errorBit & 0x7);
    uint8_t *errorStatusBits = &EM->errorStatusBits[index];
    uint8_t *bufWritePtrCopy;

    /* if error is allready cleared, return */
    if((*errorStatusBits & bitmask) == 0) return 0;

    if(!EM) return -1;

    /* if errorBit value not supported, send emergency 'ERROR_WRONG_ERROR_REPORT' */
    if(index >= EM->errorStatusBitsSize){
        EM->wrongErrorReport = errorBit;
        return -1;
    }

    /* set busy flag. If allready busy, return. */
    DISABLE_INTERRUPTS();
    if(EM->errorReportBusy++){
        EM->errorReportBusy--;
        ENABLE_INTERRUPTS();
        return -3;
    }
    ENABLE_INTERRUPTS();

    /* erase error bit */
    *errorStatusBits &= ~bitmask;

    /* verify buffer full */
    if(EM->bufFull){
        EM->bufFull = 2;
        EM->errorReportBusy--;
        return -2;
    }

    /* copy data for emergency message */
    bufWritePtrCopy = EM->bufWritePtr;
    *(bufWritePtrCopy++) = 0;
    *(bufWritePtrCopy++) = 0;
    *(bufWritePtrCopy++) = 0;
    *(bufWritePtrCopy++) = errorBit;
    memcpySwap4(bufWritePtrCopy, (uint8_t*)&infoCode);
    bufWritePtrCopy += 4;

    /* Update write buffer pointer */
    if(bufWritePtrCopy == EM->bufEnd) EM->bufWritePtr = EM->buf;
    else                              EM->bufWritePtr = bufWritePtrCopy;

    /* verify buffer full, clear busy flag and return */
    if(EM->bufWritePtr == EM->bufReadPtr) EM->bufFull = 1;
    EM->errorReportBusy--;
    return 1;
}


/******************************************************************************/
int8_t CO_isError(CO_EM_t *EM, uint8_t errorBit, uint16_t errorCode){
    uint8_t index = errorBit >> 3;
    uint8_t bitmask = 1 << (errorBit & 0x7);

    if((EM->errorStatusBits[index] & bitmask)) return 1;

    return 0;
}
