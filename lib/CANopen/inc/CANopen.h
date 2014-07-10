/**
 * Main CANopen stack file.
 *
 * It combines Object dictionary (CO_OD) and all other CANopen source files.
 * Configuration information are read from CO_OD.h file. This file may be
 * customized for different CANopen configuration. (One or multiple CANopen
 * device on one or multiple CAN modules.)
 *
 * @file        CANopen.h
 * @ingroup     CO_CANopen
 * @version     SVN: \$Id: CANopen.h 32 2013-03-11 08:24:27Z jani22 $
 * @author      Janez Paternoster
 * @copyright   2010 - 2013 Janez Paternoster
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


#ifndef _CANopen_H
#define _CANopen_H


/**
 * @defgroup CO_CANopen CANopen stack
 * @{
 *
 *
 */


    #include "CO_driver.h"
    #include "CO_OD.h"
    #include "CO_SDO.h"
    #include "CO_Emergency.h"
    #include "CO_NMT_Heartbeat.h"
    #include "CO_SYNC.h"
    #include "CO_PDO.h"
    #include "CO_HBconsumer.h"
    #include "CO_timer.h"
#if CO_NO_SDO_CLIENT == 1
    #include "CO_SDOmaster.h"
#endif


/**
 * @defgroup CO_CANopen_identifiers Default CANopen identifiers
 * @{
 *
 * Default CANopen identifiers for CANopen communication objects. Same as
 * 11-bit addresses of CAN messages. These are default identifiers and
 * can be changed in CANopen. Especially PDO identifiers are confgured
 * in PDO linking phase of the CANopen network configuration.
 */
    #define CO_CAN_ID_NMT_SERVICE       0x000   /**< Network management */
    #define CO_CAN_ID_SYNC              0x080   /**< Synchronous message */
    #define CO_CAN_ID_EMERGENCY         0x080   /**< Emergency messages (+nodeID) */
    #define CO_CAN_ID_TIME_STAMP        0x100   /**< Time stamp message */
    #define CO_CAN_ID_TPDO0             0x180   /**< Default TPDO1 (+nodeID) */
    #define CO_CAN_ID_RPDO0             0x200   /**< Default RPDO1 (+nodeID) */
    #define CO_CAN_ID_TPDO1             0x280   /**< Default TPDO2 (+nodeID) */
    #define CO_CAN_ID_RPDO1             0x300   /**< Default RPDO2 (+nodeID) */
    #define CO_CAN_ID_TPDO2             0x380   /**< Default TPDO3 (+nodeID) */
    #define CO_CAN_ID_RPDO2             0x400   /**< Default RPDO3 (+nodeID) */
    #define CO_CAN_ID_TPDO3             0x480   /**< Default TPDO4 (+nodeID) */
    #define CO_CAN_ID_RPDO3             0x500   /**< Default RPDO5 (+nodeID) */
    #define CO_CAN_ID_TSDO              0x580   /**< SDO response from server (+nodeID) */
    #define CO_CAN_ID_RSDO              0x600   /**< SDO request from client (+nodeID) */
    #define CO_CAN_ID_HEARTBEAT         0x700   /**< Heartbeat message */
/** @} */


/**
 * Number of CAN modules in use.
 *
 * If constant is set globaly to 2, second CAN module is initialized and fifth
 * and sixth RPDO (if exist) are configured to it.
 */
#ifndef CO_NO_CAN_MODULES
    #define CO_NO_CAN_MODULES 1
#endif


/**
 * CANopen stack object combines pointers to all CANopen objects.
 */
typedef struct{
    CO_CANmodule_t     *CANmodule[CO_NO_CAN_MODULES];/**< CAN module objects */
    CO_SDO_t           *SDO;            /**< SDO object */
    CO_EM_t            *EM;             /**< Emergency report object */
    CO_EMpr_t          *EMpr;           /**< Emergency process object */
    CO_NMT_t           *NMT;            /**< NMT object */
    CO_SYNC_t          *SYNC;           /**< SYNC object */
    CO_RPDO_t          *RPDO[CO_NO_RPDO];/**< RPDO objects */
    CO_TPDO_t          *TPDO[CO_NO_TPDO];/**< TPDO objects */
    CO_HBconsumer_t    *HBcons;         /**<  Heartbeat consumer object*/
#if CO_NO_SDO_CLIENT == 1
    CO_SDOclient_t     *SDOclient;      /**< SDO client object */
#endif
}CO_t;


/** CANopen object */
    extern CO_t *CO;


/**
 * Function CO_sendNMTcommand() is simple function, which sends CANopen message.
 * This part of code is an example of custom definition of simple CANopen
 * object. Follow the code in CANopen.c file. If macro CO_NO_NMT_MASTER is 1,
 * function CO_sendNMTcommand can be used to send NMT master message.
 *
 * @param CO CANopen object.
 * @param command NMT command.
 * @param nodeID Node ID.
 *
 * @return 0: Operation completed successfully.
 * @return other: same as CO_CANsend().
 */
#if CO_NO_NMT_MASTER == 1
    uint8_t CO_sendNMTcommand(CO_t *CO, uint8_t command, uint8_t nodeID);
#endif


/**
 * Initialize CANopen stack.
 *
 * Function must be called in the communication reset section.
 *
 * @return #CO_ReturnError_t: CO_ERROR_NO, CO_ERROR_ILLEGAL_ARGUMENT,
 * CO_ERROR_OUT_OF_MEMORY, CO_ERROR_ILLEGAL_BAUDRATE
 */
int16_t CO_init();


/**
 * Delete CANopen object and free memory. Must be called at program exit.
 */
void CO_delete();


/**
 * Process CANopen objects.
 *
 * Function must be called cyclically. It processes all "asynchronous" CANopen
 * objects. Function returns value from CO_NMT_process().
 *
 * @param CO This object
 * @param timeDifference_ms Time difference from previous function call in [milliseconds].
 *
 * @return 0: Normal return, no action.
 * @return 1: Application must provide communication reset.
 * @return 2: Application must provide complete device reset.
 */
uint8_t CO_process(
        CO_t                   *CO,
        uint16_t                timeDifference_ms);


/**
 * Process CANopen SYNC and RPDO objects.
 *
 * Function must be called cyclically from synchronous 1ms task. It processes
 * SYNC and receive PDO CANopen objects.
 *
 * @param CO This object
 */
void CO_process_RPDO(CO_t *CO);


/**
 * Process CANopen TPDO objects.
 *
 * Function must be called cyclically from synchronous 1ms task. It processes
 * transmit PDO CANopen objects.
 *
 * @param CO This object
 */
void CO_process_TPDO(CO_t *CO);


/** @} */
#endif
