/**
 * CANopen Emergency protocol.
 *
 * @file        CO_Emergency.h
 * @ingroup     CO_Emergency
 * @version     SVN: \$Id: CO_Emergency.h 32 2013-03-11 08:24:27Z jani22 $
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


#ifndef _CO_EMERGENCY_H
#define _CO_EMERGENCY_H


/**
 * @defgroup CO_Emergency Emergency
 * @ingroup CO_CANopen
 * @{
 *
 * CANopen Emergency protocol.
 *
 * Error control and Emergency is used for control internal error state
 * and for sending a CANopen Emergency message.
 *
 * In case of error condition stack or application calls CO_errorReport()
 * function with indication of the error. Specific error condition is reported
 * (with CANopen Emergency message) only the first time after it occurs.
 * Internal state of the error condition is controlled with
 * @ref CO_EM_errorStatusBits. Specific error condition can also be reset by
 * CO_errorReset() function. If so, Emergency message is sent with
 * ERROR_NO_ERROR indication.
 *
 * Some error conditions are informative and some are critical. Critical error
 * conditions sets the @ref CO_EM_errorRegister.
 *
 * Latest errors can be read from _Pre Defined Error Field_ (object dictionary,
 * index 0x1003). @ref CO_EM_errorStatusBits can also be read form CANopen
 * object dictionary.
 *
 * ###Emergency message contents:
 *
 *   Byte | Description
 *   -----|-----------------------------------------------------------
 *   0..1 | @ref CO_EM_errorCode.
 *   2    | @ref CO_EM_errorRegister.
 *   3    | Index of error condition (see @ref CO_EM_errorStatusBits).
 *   4..7 | Additional argument informative to CO_errorReport() function.
 *
 * ####Contents of _Pre Defined Error Field_ (object dictionary, index 0x1003):
 * bytes 0..3 are equal to bytes 0..3 in the Emergency message.
 *
 * @see @ref CO_CANopen_identifiers
 */


/**
 * @defgroup CO_EM_errorCode CANopen Error code
 *
 * Standard error codes.
 *
 * ###Error codes according to CiA DS-301:
 *   Code | Description
 *   -----|-------------------------------
 *   00xx | Error Reset or No Error.
 *   10xx | Generic Error.
 *   20xx | Current.
 *   21xx | Current, device input side.
 *   22xx | Current inside the device.
 *   23xx | Current, device output side.
 *   30xx | Voltage.
 *   31xx | Mains Voltage.
 *   32xx | Voltage inside the device.
 *   33xx | Output Voltage.
 *   40xx | Temperature.
 *   41xx | Ambient Temperature.
 *   42xx | Device Temperature.
 *   50xx | Device Hardware.
 *   60xx | Device Software.
 *   61xx | Internal Software.
 *   62xx | User Software.
 *   63xx | Data Set.
 *   70xx | Additional Modules.
 *   80xx | Monitoring.
 *   81xx | Communication.
 *   8110 | CAN Overrun (Objects lost).
 *   8120 | CAN in Error Passive Mode.
 *   8130 | Life Guard Error or Heartbeat Error.
 *   8140 | recovered from bus off.
 *   8150 | CAN-ID collision.
 *   82xx | Protocol Error.
 *   8210 | PDO not processed due to length error.
 *   8220 | PDO length exceeded.
 *   8230 | DAM MPDO not processed, destination object not available.
 *   8240 | Unexpected SYNC data length.
 *   8250 | RPDO timeout.
 *   90xx | External Error.
 *   F0xx | Additional Functions.
 *   FFxx | Device specific.
 *
 * ###Error codes according to CiA DS-401:
 *   Code | Description
 *   -----|-------------------------------
 *   2310 | Current at outputs too high (overload).
 *   2320 | Short circuit at outputs.
 *   2330 | Load dump at outputs.
 *   3110 | Input voltage too high.
 *   3120 | Input voltage too low.
 *   3210 | Internal voltage too high.
 *   3220 | Internal voltage too low.
 *   3310 | Output voltage too high.
 *   3320 | Output voltage too low.
 */


/**
 * @defgroup CO_EM_errorRegister CANopen Error register
 *
 * In object dictionary on index 0x1001.
 *
 * Error register is calculated from critical internal @ref CO_EM_errorStatusBits.
 * Generic and communication bits are calculated in CO_EM_process
 * function, device profile or manufacturer specific bits may be calculated
 * inside the application.
 *
 * Internal errors may prevent device to stay in NMT Operational state. Details
 * are described in _Error Behavior_ object in Object Dictionary at index 0x1029.
 *
 * ###Specific bits:
 *   Bit | Description
 *   ----|-------------------------------
 *    0  | Generic error.
 *    1  | Current.
 *    2  | Voltage.
 *    3  | Temperature.
 *    4  | Communication error (overrun, error state).
 *    5  | Device profile specific.
 *    6  | Reserved (always 0).
 *    7  | Manufacturer specific.
 */


/**
 * @defgroup CO_EM_errorStatusBits Error status bits
 * @{
 *
 * Internal indication of the error condition.
 *
 * Each error condition is specified by unique index from 0x00 up to 0xFF.
 * Variable  (from manufacturer section in the Object
 * Dictionary) contains up to 0xFF bits (32bytes) for the identification of the
 * specific error condition. (Type of the variable is CANopen OCTET_STRING.)
 *
 * If specific error occurs in the stack or in the application, CO_errorReport()
 * sets specific bit in the _Error Status Bits_ variable. If bit was already
 * set, function returns without any action. Otherwise it prepares emergency
 * message.
 *
 * CO_errorReport(), CO_errorReset() or CO_isError() functions are called
 * with unique index as an argument. (However CO_errorReport(), for example, may
 * be used with the same index on multiple places in the code.)
 *
 * Macros defined below are combination of two constants: index and
 * @ref CO_EM_errorCode. They represents specific error conditions. They are
 * used as double argument for CO_errorReport(), CO_errorReset() and
 * CO_isError() functions.
 *
 * Stack uses first 6 bytes of the _Error Status Bits_ variable. Device profile
 * or application may define own macros for Error status bits. Note that
 * _Error Status Bits_ must be large enough (up to 32 bytes).
 */

/** @name Informative communication or protocol errors */
/** @{ */
    #define ERROR_NO_ERROR                             0x00, 0x0000 /**< Error Reset or No Error */
    #define ERROR_CAN_BUS_WARNING                      0x01, 0x0000 /**< CAN bus warning limit reached */
    #define ERROR_RXMSG_WRONG_LENGTH                   0x02, 0x8200 /**< Wrong data length of the received CAN message */
    #define ERROR_RXMSG_OVERFLOW                       0x03, 0x8200 /**< Previous received CAN message wasn't processed yet */
    #define ERROR_RPDO_WRONG_LENGTH                    0x04, 0x8210 /**< Wrong data length of received PDO */
    #define ERROR_RPDO_OVERFLOW                        0x05, 0x8200 /**< Previous received PDO wasn't processed yet */
    #define ERROR_CAN_RX_BUS_PASSIVE                   0x06, 0x8120 /**< CAN receive bus is passive */
    #define ERROR_CAN_TX_BUS_PASSIVE                   0x07, 0x8120 /**< CAN transmit bus is passive */
    #define ERROR_NMT_WRONG_COMMAND                    0x08, 0x8200 /**< Wrong NMT command received */
    #define ERROR_09_unused                            0x09, 0x1000 /**< (unused) */
    #define ERROR_0A_unused                            0x0A, 0x1000 /**< (unused) */
    #define ERROR_0B_unused                            0x0B, 0x1000 /**< (unused) */
    #define ERROR_0C_unused                            0x0C, 0x1000 /**< (unused) */
    #define ERROR_0D_unused                            0x0D, 0x1000 /**< (unused) */
    #define ERROR_0E_unused                            0x0E, 0x1000 /**< (unused) */
    #define ERROR_0F_unused                            0x0F, 0x1000 /**< (unused) */
/** @} */
/** @name Critical communication or protocol errors */
/** @{ */
    #define ERROR_10_unused                            0x10, 0x1000 /**< (unused) */
    #define ERROR_11_unused                            0x11, 0x1000 /**< (unused) */
    #define ERROR_CAN_TX_BUS_OFF                       0x12, 0x8140 /**< CAN transmit bus is off */
    #define ERROR_CAN_RXB_OVERFLOW                     0x13, 0x8110 /**< CAN module receive buffer has overflowed */
    #define ERROR_CAN_TX_OVERFLOW                      0x14, 0x8110 /**< CAN transmit buffer has overflowed */
    #define ERROR_TPDO_OUTSIDE_WINDOW                  0x15, 0x8100 /**< TPDO is outside SYNC window */
    #define ERROR_16_unused                            0x16, 0x1000 /**< (unused) */
    #define ERROR_17_unused                            0x17, 0x1000 /**< (unused) */
    #define ERROR_SYNC_TIME_OUT                        0x18, 0x8100 /**< SYNC message timeout */
    #define ERROR_SYNC_LENGTH                          0x19, 0x8240 /**< Unexpected SYNC data length */
    #define ERROR_PDO_WRONG_MAPPING                    0x1A, 0x8200 /**< Error with PDO mapping */
    #define ERROR_HEARTBEAT_CONSUMER                   0x1B, 0x8130 /**< Heartbeat consumer timeout */
    #define ERROR_HEARTBEAT_CONSUMER_REMOTE_RESET      0x1C, 0x8130 /**< Heartbeat consumer detected remote node reset */
    #define ERROR_1D_unused                            0x1D, 0x1000 /**< (unused) */
    #define ERROR_1E_unused                            0x1E, 0x1000 /**< (unused) */
    #define ERROR_1F_unused                            0x1F, 0x1000 /**< (unused) */
/** @} */
/** @name Informative generic errors */
/** @{ */
    #define ERROR_EMERGENCY_BUFFER_FULL                0x20, 0x1000 /**< Emergency buffer is full, Emergency message wasn't sent */
    #define ERROR_ERROR_REPORT_BUSY                    0x21, 0x1000 /**< CO_errorReport() is busy, Emergency message wasn't sent */
    #define ERROR_MICROCONTROLLER_RESET                0x22, 0x1000 /**< Microcontroller has just started */
    #define ERROR_23_unused                            0x23, 0x1000 /**< (unused) */
    #define ERROR_24_unused                            0x24, 0x1000 /**< (unused) */
    #define ERROR_25_unused                            0x25, 0x1000 /**< (unused) */
    #define ERROR_26_unused                            0x26, 0x1000 /**< (unused) */
    #define ERROR_27_unused                            0x27, 0x1000 /**< (unused) */
/** @} */
/** @name Critical generic errors */
/** @{ */
    #define ERROR_WRONG_ERROR_REPORT                   0x28, 0x6100 /**< Wrong parameters to CO_errorReport() function */
    #define ERROR_ISR_TIMER_OVERFLOW                   0x29, 0x6100 /**< Timer task has overflowed */
    #define ERROR_MEMORY_ALLOCATION_ERROR              0x2A, 0x6100 /**< Unable to allocate memory for objects */
    #define ERROR_GENERIC_ERROR                        0x2B, 0x1000 /**< Generic error, test usage */
    #define ERROR_MAIN_TIMER_OVERFLOW                  0x2C, 0x6100 /**< Mainline function exceeded maximum execution time */
    #define ERROR_INCONSISTENT_OBJECT_DICTIONARY       0x2D, 0x6100 /**< Object dictionary does not match the software */
    #define ERROR_CALCULATION_OF_PARAMETERS            0x2E, 0x6300 /**< Error in calculation of device parameters */
    #define ERROR_NON_VOLATILE_MEMORY                  0x2F, 0x5000 /**< Error with access to non volatile device memory */
/** @} */
/** @} */


/**
 * Size of internal buffer, whwre emergencies are stored after CO_errorReport().
 * Buffer is cleared by CO_EM_process().
 */
#define CO_EM_INTERNAL_BUFFER_SIZE      10


/**
 * Emergerncy object for CO_errorReport(). It contains error buffer, to which new emergency
 * messages are written, when CO_errorReport() is called. This object is included in
 * CO_EMpr_t object.
 */
typedef struct{
    uint8_t            *errorStatusBits;/**< From CO_EM_init() */
    uint8_t             errorStatusBitsSize;/**< From CO_EM_init() */
    /** Internal buffer for storing unsent emergency messages.*/
    uint8_t             buf[CO_EM_INTERNAL_BUFFER_SIZE * 8];
    uint8_t            *bufEnd;         /**< End+1 address of the above buffer */
    uint8_t            *bufWritePtr;    /**< Write pointer in the above buffer */
    uint8_t            *bufReadPtr;     /**< Read pointer in the above buffer */
    uint8_t             bufFull;        /**< True if above buffer is full */
    uint8_t             wrongErrorReport;/**< Error in arguments to CO_errorReport() */
    uint8_t             errorReportBusy;/**< Lock mechanism for CO_errorReport() */
    /** Error, higher priority task called CO_errorReport() while it was busy */
    uint8_t             errorReportBusyError;
}CO_EM_t;


/**
 * Report error condition.
 *
 * Function is called on any error condition inside CANopen stack and may also
 * be called by application on custom error condition. Emergency message is sent
 * after the first occurance of specific error. In case of critical error, device
 * will not be able to stay in NMT_OPERATIONAL state.
 *
 * Function is short and may be used form any task or interrupt.
 *
 * @param EM Emergency object.
 * @param errorBit, errorCode Use macro from @ref CO_EM_errorStatusBits.
 * @param infoCode 32 bit value is passed to bytes 4...7 of the Emergency message.
 * It contains optional additional information inside emergency message.
 *
 * @return -3: CO_errorReport is busy, message is deleted.
 * @return -2: Emergency buffer is full, message is deleted.
 * @return -1: Error in arguments.
 * @return  0: Error was already present before, no action was performed.
 * @return  1: Error is new, Emergency will be send.
 */
int8_t CO_errorReport(CO_EM_t *EM, uint8_t errorBit, uint16_t errorCode, uint32_t infoCode);


/**
 * Reset error condition.
 *
 * Function is called if any error condition is solved. Emergency message is sent
 * with @ref CO_EM_errorCode 0x0000.
 *
 * Function is short and may be used form any task or interrupt.
 *
 * @param EM Emergency object.
 * @param errorBit, errorCode Use macro from @ref CO_EM_errorStatusBits.
 * @param infoCode 32 bit value is passed to bytes 4...7 of the Emergency message.
 *
 * @return -3: CO_errorReport is busy, message is deleted.
 * @return -2: Emergency buffer is full, message is deleted.
 * @return -1: Error in arguments.
 * @return  0: Error was already present before, no action was performed.
 * @return  1: Error bit is cleared, Emergency with @ref CO_EM_errorCode 0 will be send.
 */
int8_t CO_errorReset(CO_EM_t *EM, uint8_t errorBit, uint16_t errorCode, uint32_t infoCode);


/**
 * Check specific error condition.
 *
 * Function returns 1, if specific internal error is present. Otherwise it returns 0.
 *
 * @param EM Emergency object.
 * @param errorBit, errorCode Use macro from @ref CO_EM_errorStatusBits.
 *
 * @return 0: Error is not present.
 * @return 1: Error is present.
 */
int8_t CO_isError(CO_EM_t *EM, uint8_t errorBit, uint16_t errorCode);


#ifdef CO_DOXYGEN
/** Skip section, if CO_SDO.h is not included */
    #define _CO_SDO_H
#endif
#ifdef _CO_SDO_H


/**
 * Error control and Emergency object. It controls internal error state and
 * sends emergency message, if error condition was reported. Object is initialized
 * by CO_EM_init(). It contains CO_EM_t object.
 */
typedef struct{
    uint8_t            *errorRegister;  /**< From CO_EM_init() */
    uint32_t           *preDefErr;      /**< From CO_EM_init() */
    uint8_t             preDefErrSize;  /**< From CO_EM_init() */
    uint8_t             preDefErrNoOfErrors;/**< Number of active errors in preDefErr */
    uint16_t            inhibitEmTimer; /**< Internal timer for emergency message */
    CO_EM_t            *EM;             /**< CO_EM_t sub object is included here */
    CO_CANmodule_t     *CANdev;         /**< From CO_EM_init() */
    CO_CANtx_t         *CANtxBuff;      /**< CAN transmit buffer */
}CO_EMpr_t;


/**
 * Initialize Error control and Emergency object.
 *
 * Function must be called in the communication reset section.
 *
 * @param EMpr This object will be initialized.
 * @param EM Emergency object defined separately. Will be included in EMpr and
 * initialized too.
 * @param SDO SDO server object.
 * @param errorStatusBits Pointer to _Error Status Bits_ array from Object Dictionary
 * (manufacturer specific section). See @ref CO_EM_errorStatusBits.
 * @param errorStatusBitsSize Total size of the above array. Must be >= 6.
 * @param errorRegister Pointer to _Error Register_ (Object dictionary, index 0x1001).
 * @param preDefErr Pointer to _Pre defined error field_ array from Object
 * dictionary, index 0x1003.
 * @param preDefErrSize Size of the above array.
 * @param CANdev CAN device for Emergency transmission.
 * @param CANdevTxIdx Index of transmit buffer in the above CAN device.
 * @param CANidTxEM CAN identifier for Emergency message.
 *
 * @return #CO_ReturnError_t CO_ERROR_NO or CO_ERROR_ILLEGAL_ARGUMENT.
 */
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
        uint16_t                CANidTxEM);


/**
 * Process Error control and Emergency object.
 *
 * Function must be called cyclically. It verifies some communication errors,
 * calculates bit 0 and bit 4 from _Error register_ and sends emergency message
 * if necessary.
 *
 * @param EMpr This object.
 * @param NMTisPreOrOperational True if this node is NMT_PRE_OPERATIONAL or NMT_OPERATIONAL.
 * @param timeDifference_100us Time difference from previous function call in [100 * microseconds].
 * @param EMinhTime _Inhibit time EMCY_ (object dictionary, index 0x1015).
 */
void CO_EM_process(
        CO_EMpr_t              *EMpr,
        uint8_t                 NMTisPreOrOperational,
        uint16_t                timeDifference_100us,
        uint16_t                EMinhTime);


#endif

/** @} */
#endif
