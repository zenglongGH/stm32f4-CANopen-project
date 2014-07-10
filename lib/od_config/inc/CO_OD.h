/*******************************************************************************

   File: CO_OD.h
   CANopen Object Dictionary.

   Copyright (C) 2004-2008 Janez Paternoster

   License: GNU Lesser General Public License (LGPL).

   <http://canopennode.sourceforge.net>

   (For more information see <CO_SDO.h>.)
*/
/*
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.


   Author: Janez Paternoster


   This file was automatically generated with CANopenNode Object
   Dictionary Editor. DON'T EDIT THIS FILE MANUALLY !!!!

*******************************************************************************/

#ifndef _CO_OD_H
#define _CO_OD_H


/*******************************************************************************
   CANopen DATA DYPES
*******************************************************************************/
   typedef uint8_t      UNSIGNED8;
   typedef uint16_t     UNSIGNED16;
   typedef uint32_t     UNSIGNED32;
   typedef uint64_t     UNSIGNED64;
   typedef int8_t       INTEGER8;
   typedef int16_t      INTEGER16;
   typedef int32_t      INTEGER32;
   typedef int64_t      INTEGER64;
   typedef float32_t    REAL32;
   typedef float64_t    REAL64;
   typedef char_t       VISIBLE_STRING;
   typedef oChar_t      OCTET_STRING;
   typedef domain_t     DOMAIN;


/*******************************************************************************
   FILE INFO:
      FileName:     IO
      FileVersion:  3.0
      CreationTime: 15:03:02
      CreationDate: 2013-10-30
      CreatedBy:    -
*******************************************************************************/


/*******************************************************************************
   DEVICE INFO:
      VendorName:     Paternoster
      VendorNumber:   0
      ProductName:    CANopenNode
      ProductNumber:  0
*******************************************************************************/


/*******************************************************************************
   FEATURES
*******************************************************************************/
   #define CO_NO_SYNC                     1   //Associated objects: 1005, 1006, 1007, 2103, 2104
   #define CO_NO_EMERGENCY                1   //Associated objects: 1014, 1015
   #define CO_NO_SDO_SERVER               1   //Associated objects: 1200
   #define CO_NO_SDO_CLIENT               0   
   #define CO_NO_RPDO                     4   //Associated objects: 1400, 1401, 1402, 1403, 1600, 1601, 1602, 1603
   #define CO_NO_TPDO                     4   //Associated objects: 1800, 1801, 1802, 1803, 1A00, 1A01, 1A02, 1A03
   #define CO_NO_NMT_MASTER               0   


/*******************************************************************************
   OBJECT DICTIONARY
*******************************************************************************/
   #define CO_OD_NoOfElements             114


/*******************************************************************************
   TYPE DEFINITIONS FOR RECORDS
*******************************************************************************/
/*1018      */ typedef struct{
               UNSIGNED8      maxSubIndex;
               UNSIGNED32     vendorID;
               UNSIGNED32     productCode;
               UNSIGNED32     revisionNumber;
               UNSIGNED32     serialNumber;
               }              OD_identity_t;

/*1200[1]   */ typedef struct{
               UNSIGNED8      maxSubIndex;
               UNSIGNED32     COB_IDClientToServer;
               UNSIGNED32     COB_IDServerToClient;
               }              OD_SDOServerParameter_t;

/*1400[4]   */ typedef struct{
               UNSIGNED8      maxSubIndex;
               UNSIGNED32     COB_IDUsedByRPDO;
               UNSIGNED8      transmissionType;
               }              OD_RPDOCommunicationParameter_t;

/*1600[4]   */ typedef struct{
               UNSIGNED8      numberOfMappedObjects;
               UNSIGNED32     mappedObject1;
               UNSIGNED32     mappedObject2;
               UNSIGNED32     mappedObject3;
               UNSIGNED32     mappedObject4;
               UNSIGNED32     mappedObject5;
               UNSIGNED32     mappedObject6;
               UNSIGNED32     mappedObject7;
               UNSIGNED32     mappedObject8;
               }              OD_RPDOMappingParameter_t;

/*1800[4]   */ typedef struct{
               UNSIGNED8      maxSubIndex;
               UNSIGNED32     COB_IDUsedByTPDO;
               UNSIGNED8      transmissionType;
               UNSIGNED16     inhibitTime;
               UNSIGNED8      compatibilityEntry;
               UNSIGNED16     eventTimer;
               UNSIGNED8      SYNCStartValue;
               }              OD_TPDOCommunicationParameter_t;

/*1A00[4]   */ typedef struct{
               UNSIGNED8      numberOfMappedObjects;
               UNSIGNED32     mappedObject1;
               UNSIGNED32     mappedObject2;
               UNSIGNED32     mappedObject3;
               UNSIGNED32     mappedObject4;
               UNSIGNED32     mappedObject5;
               UNSIGNED32     mappedObject6;
               UNSIGNED32     mappedObject7;
               UNSIGNED32     mappedObject8;
               }              OD_TPDOMappingParameter_t;


/*******************************************************************************
   STRUCTURES FOR VARIABLES IN DIFFERENT MEMORY LOCATIONS
*******************************************************************************/
#define  CO_OD_FIRST_LAST_WORD     0x55 //Any value from 0x01 to 0xFE. If changed, EEPROM will be reinitialized.

/***** Structure for RAM variables ********************************************/
struct sCO_OD_RAM{
               UNSIGNED32     FirstWord;

/*1001      */ UNSIGNED8      errorRegister;
/*1002      */ UNSIGNED32     manufacturerStatusRegister;
/*1003      */ UNSIGNED32     preDefinedErrorField[8];
/*1010      */ UNSIGNED32     storeParameters[1];
/*1011      */ UNSIGNED32     restoreDefaultParameters[1];
/*2100      */ OCTET_STRING   errorStatusBits[10];
/*2103      */ UNSIGNED16     SYNCCounter;
/*2104      */ UNSIGNED16     SYNCTime;
/*2105      */ UNSIGNED8      OPERATING_MODE;
/*2106      */ UNSIGNED8      OPERATING_MODE_DISPLAY;
/*2107      */ UNSIGNED8      STATUS_CODE;
/*2200      */ INTEGER16      JOINT_FORCE_CONSTANT;
/*2201      */ INTEGER16      JOINT_POSITION_CONSTANT;
/*2202      */ INTEGER16      JOINT_VELOCITY_CONSTANT;
/*2203      */ INTEGER16      JOINT_PID_GAIN_CONSTANT;
/*2204      */ INTEGER16      JOINT_FORCE_SETPOINT_MAX[2];
/*2205      */ INTEGER16      JOINT_POSITION_SETPOINT_MAX[2];
/*2206      */ INTEGER16      JOINT_POSITION_SETPOINT_MIN[2];
/*2207      */ INTEGER16      JOINT_VELOCITY_SETPOINT_MAX[2];
/*2208      */ INTEGER16      JOINT_FORCE_SETPOINT[2];
/*2209      */ INTEGER16      JOINT_POSITION_SETPOINT[2];
/*220A      */ INTEGER16      JOINT_VELOCITY_SETPOINT[2];
/*220B      */ INTEGER16      JOINT_FORCE_ESTIMATE[2];
/*220C      */ INTEGER16      JOINT_POSITION_ESTIMATE[2];
/*220D      */ INTEGER16      JOINT_VELOCITY_ESTIMATE[2];
/*220E      */ INTEGER16      JOINT_FORCE_DEMAND[2];
/*220F      */ INTEGER16      JOINT_POSITION_P_GAIN[2];
/*2210      */ INTEGER16      JOINT_POSITION_I_GAIN[2];
/*2211      */ INTEGER16      JOINT_POSITION_D_GAIN[2];
/*2213      */ REAL32         JOINT_POSITION_SENSOR_BIAS[2];
/*2214      */ REAL32         JOINT_POSITION_SENSOR_GAIN[2];
/*2215      */ REAL32         JOINT_POSITION_SENSOR_OFFSET[2];
/*2216      */ REAL32         JOINT_POSITION_DEADBAND[2];
/*2217      */ INTEGER16      JOINT_FORCE_ESTIMATE_MAX[2];
/*2218      */ INTEGER16      JOINT_POSITION_ESTIMATE_MAX[2];
/*2219      */ INTEGER16      JOINT_POSITION_ESTIMATE_MIN[2];
/*221A      */ INTEGER16      JOINT_VELOCITY_ESTIMATE_MAX[2];
/*221B      */ REAL32         JOINT_POSITION_I_LIMIT[2];
/*2300      */ REAL32         MOTOR_FORCE_SETPOINT_MAX[2];
/*2301      */ REAL32         MOTOR_CURRENT_SETPOINT_MAX[2];
/*2302      */ REAL32         MOTOR_FORCE_SETPOINT[2];
/*2303      */ REAL32         MOTOR_CURRENT_SETPOINT[2];
/*2304      */ REAL32         MOTOR_FORCE_ESTIMATE[2];
/*2305      */ REAL32         MOTOR_CURRENT_ESTIMATE[2];
/*2306      */ REAL32         MOTOR_FORCE_FF_CONSTANT[2];
/*2307      */ REAL32         MOTOR_FORCE_P_GAIN[2];
/*2308      */ REAL32         MOTOR_FORCE_I_GAIN[2];
/*2309      */ REAL32         MOTOR_FORCE_D_GAIN[2];
/*230A      */ REAL32         MOTOR_FORCE_ESTIMATE_BREAK_FREQUENCY[2];
/*230B      */ REAL32         MOTOR_CURRENT_P_GAIN[2];
/*230C      */ REAL32         MOTOR_CURRENT_I_GAIN[2];
/*230D      */ REAL32         MOTOR_FORCE_SENSOR_BIAS[2];
/*230E      */ REAL32         MOTOR_FORCE_SENSOR_GAIN[2];
/*230F      */ REAL32         MOTOR_FORCE_SENSOR_OFFSET[2];
/*2310      */ REAL32         MOTOR_CURRENT_SENSOR_BIAS[2];
/*2311      */ REAL32         MOTOR_CURRENT_SENSOR_GAIN[2];
/*2312      */ REAL32         MOTOR_CURRENT_SENSOR_OFFSET[2];
/*2313      */ REAL32         MOTOR_CURRENT_DEMAND[2];
/*2314      */ REAL32         MOTOR_FORCE_DEADBAND[2];
/*2315      */ REAL32         MOTOR_FORCE_ESTIMATE_MAX[2];
/*2316      */ REAL32         MOTOR_CURRENT_ESTIMATE_MAX[2];
/*2317      */ REAL32         MOTOR_FORCE_I_LIMIT[2];
/*2318      */ REAL32         MOTOR_CURRENT_I_LIMIT[2];
/*2319      */ REAL32         MOTOR_FORCE_RATE_OF_CHANGE_ESTIMATE_BREAK_FREQUENCY[2];
/*231A      */ REAL32         MOTOR_POSITION_MEASUREMENT[2];
/*231B      */ REAL32         MOTOR_VELOCITY_ESTIMATE[2];
/*231C      */ REAL32         MOTOR_VELOCITY_ESTIMATE_BREAK_FREQUENCY[2];
/*231D      */ REAL32         MOTOR_POSITION_SENSOR_GAIN[2];
/*231E      */ REAL32         MOTOR_POSITION_SENSOR_OFFSET[2];
/*231F      */ REAL32         MOTOR_FORCE_RATE_OF_CHANGE_ESTIMATE[2];
/*2320      */ REAL32         MOTOR_FORCE_MEASUREMENT[2];
/*2321      */ INTEGER16      JOINT_POSITION_MEASUREMENT[2];
/*2322      */ REAL32         JOINT_POSITION_ESTIMATE_BREAK_FREQUENCY[2];
/*2323      */ REAL32         JOINT_VELOCITY_ESTIMATE_BREAK_FREQUENCY[2];
/*2324      */ REAL32         MOTOR_FORCE_REFERENCE[2];
/*2325      */ REAL32         MOTOR_FORCE_REFERENCE_BREAK_FREQUENCY[2];
/*6000      */ UNSIGNED8      readInput8Bit[8];
/*6200      */ UNSIGNED8      writeOutput8Bit[8];
/*6401      */ INTEGER16      readAnalogueInput16Bit[12];
/*6411      */ INTEGER16      writeAnalogueOutput16Bit[8];

               UNSIGNED32     LastWord;
};

/***** Structure for EEPROM variables *****************************************/
struct sCO_OD_EEPROM{
               UNSIGNED32     FirstWord;



               UNSIGNED32     LastWord;
};


/***** Structure for ROM variables ********************************************/
struct sCO_OD_ROM{
               UNSIGNED32     FirstWord;

/*1000      */ UNSIGNED32     deviceType;
/*1005      */ UNSIGNED32     COB_ID_SYNCMessage;
/*1006      */ UNSIGNED32     communicationCyclePeriod;
/*1007      */ UNSIGNED32     synchronousWindowLength;
/*1008      */ VISIBLE_STRING manufacturerDeviceName[8];
/*1009      */ VISIBLE_STRING manufacturerHardwareVersion[4];
/*100A      */ VISIBLE_STRING manufacturerSoftwareVersion[4];
/*1014      */ UNSIGNED32     COB_ID_EMCY;
/*1015      */ UNSIGNED16     inhibitTimeEMCY;
/*1016      */ UNSIGNED32     consumerHeartbeatTime[4];
/*1017      */ UNSIGNED16     producerHeartbeatTime;
/*1018      */ OD_identity_t  identity;
/*1019      */ UNSIGNED8      synchronousCounterOverflowValue;
/*1029      */ UNSIGNED8      errorBehavior[6];
/*1200[1]   */ OD_SDOServerParameter_t SDOServerParameter[1];
/*1400[4]   */ OD_RPDOCommunicationParameter_t RPDOCommunicationParameter[4];
/*1600[4]   */ OD_RPDOMappingParameter_t RPDOMappingParameter[4];
/*1800[4]   */ OD_TPDOCommunicationParameter_t TPDOCommunicationParameter[4];
/*1A00[4]   */ OD_TPDOMappingParameter_t TPDOMappingParameter[4];
/*1F80      */ UNSIGNED32     NMTStartup;
/*2101      */ UNSIGNED8      CANNodeID;
/*2102      */ UNSIGNED16     CANBitRate;

               UNSIGNED32     LastWord;
};


/***** Declaration of Object Dictionary variables *****************************/
extern struct sCO_OD_RAM CO_OD_RAM;

extern struct sCO_OD_EEPROM CO_OD_EEPROM;

extern struct sCO_OD_ROM CO_OD_ROM;


/*******************************************************************************
   ALIASES FOR OBJECT DICTIONARY VARIABLES
*******************************************************************************/
/*1000, Data Type: UNSIGNED32 */
      #define OD_deviceType                              CO_OD_ROM.deviceType

/*1001, Data Type: UNSIGNED8 */
      #define OD_errorRegister                           CO_OD_RAM.errorRegister

/*1002, Data Type: UNSIGNED32 */
      #define OD_manufacturerStatusRegister              CO_OD_RAM.manufacturerStatusRegister

/*1003, Data Type: UNSIGNED32, Array[8] */
      #define OD_preDefinedErrorField                    CO_OD_RAM.preDefinedErrorField
      #define ODL_preDefinedErrorField_arrayLength       8

/*1005, Data Type: UNSIGNED32 */
      #define OD_COB_ID_SYNCMessage                      CO_OD_ROM.COB_ID_SYNCMessage

/*1006, Data Type: UNSIGNED32 */
      #define OD_communicationCyclePeriod                CO_OD_ROM.communicationCyclePeriod

/*1007, Data Type: UNSIGNED32 */
      #define OD_synchronousWindowLength                 CO_OD_ROM.synchronousWindowLength

/*1008, Data Type: VISIBLE_STRING, Array[8] */
      #define OD_manufacturerDeviceName                  CO_OD_ROM.manufacturerDeviceName
      #define ODL_manufacturerDeviceName_stringLength    8

/*1009, Data Type: VISIBLE_STRING, Array[4] */
      #define OD_manufacturerHardwareVersion             CO_OD_ROM.manufacturerHardwareVersion
      #define ODL_manufacturerHardwareVersion_stringLength 4

/*100A, Data Type: VISIBLE_STRING, Array[4] */
      #define OD_manufacturerSoftwareVersion             CO_OD_ROM.manufacturerSoftwareVersion
      #define ODL_manufacturerSoftwareVersion_stringLength 4

/*1010, Data Type: UNSIGNED32, Array[1] */
      #define OD_storeParameters                         CO_OD_RAM.storeParameters
      #define ODL_storeParameters_arrayLength            1
      #define ODA_storeParameters_saveAllParameters      0

/*1011, Data Type: UNSIGNED32, Array[1] */
      #define OD_restoreDefaultParameters                CO_OD_RAM.restoreDefaultParameters
      #define ODL_restoreDefaultParameters_arrayLength   1
      #define ODA_restoreDefaultParameters_restoreAllDefaultParameters 0

/*1014, Data Type: UNSIGNED32 */
      #define OD_COB_ID_EMCY                             CO_OD_ROM.COB_ID_EMCY

/*1015, Data Type: UNSIGNED16 */
      #define OD_inhibitTimeEMCY                         CO_OD_ROM.inhibitTimeEMCY

/*1016, Data Type: UNSIGNED32, Array[4] */
      #define OD_consumerHeartbeatTime                   CO_OD_ROM.consumerHeartbeatTime
      #define ODL_consumerHeartbeatTime_arrayLength      4

/*1017, Data Type: UNSIGNED16 */
      #define OD_producerHeartbeatTime                   CO_OD_ROM.producerHeartbeatTime

/*1018, Data Type: OD_identity_t */
      #define OD_identity                                CO_OD_ROM.identity

/*1019, Data Type: UNSIGNED8 */
      #define OD_synchronousCounterOverflowValue         CO_OD_ROM.synchronousCounterOverflowValue

/*1029, Data Type: UNSIGNED8, Array[6] */
      #define OD_errorBehavior                           CO_OD_ROM.errorBehavior
      #define ODL_errorBehavior_arrayLength              6
      #define ODA_errorBehavior_communication            0
      #define ODA_errorBehavior_communicationOther       1
      #define ODA_errorBehavior_communicationPassive     2
      #define ODA_errorBehavior_generic                  3
      #define ODA_errorBehavior_deviceProfile            4
      #define ODA_errorBehavior_manufacturerSpecific     5

/*1200[1], Data Type: OD_SDOServerParameter_t, Array[1] */
      #define OD_SDOServerParameter                      CO_OD_ROM.SDOServerParameter

/*1400[4], Data Type: OD_RPDOCommunicationParameter_t, Array[4] */
      #define OD_RPDOCommunicationParameter              CO_OD_ROM.RPDOCommunicationParameter

/*1600[4], Data Type: OD_RPDOMappingParameter_t, Array[4] */
      #define OD_RPDOMappingParameter                    CO_OD_ROM.RPDOMappingParameter

/*1800[4], Data Type: OD_TPDOCommunicationParameter_t, Array[4] */
      #define OD_TPDOCommunicationParameter              CO_OD_ROM.TPDOCommunicationParameter

/*1A00[4], Data Type: OD_TPDOMappingParameter_t, Array[4] */
      #define OD_TPDOMappingParameter                    CO_OD_ROM.TPDOMappingParameter

/*1F80, Data Type: UNSIGNED32 */
      #define OD_NMTStartup                              CO_OD_ROM.NMTStartup

/*2100, Data Type: OCTET_STRING, Array[10] */
      #define OD_errorStatusBits                         CO_OD_RAM.errorStatusBits
      #define ODL_errorStatusBits_stringLength           10

/*2101, Data Type: UNSIGNED8 */
      #define OD_CANNodeID                               CO_OD_ROM.CANNodeID

/*2102, Data Type: UNSIGNED16 */
      #define OD_CANBitRate                              CO_OD_ROM.CANBitRate

/*2103, Data Type: UNSIGNED16 */
      #define OD_SYNCCounter                             CO_OD_RAM.SYNCCounter

/*2104, Data Type: UNSIGNED16 */
      #define OD_SYNCTime                                CO_OD_RAM.SYNCTime

/*2105, Data Type: UNSIGNED8 */
      #define OD_OPERATING_MODE                          CO_OD_RAM.OPERATING_MODE

/*2106, Data Type: UNSIGNED8 */
      #define OD_OPERATING_MODE_DISPLAY                  CO_OD_RAM.OPERATING_MODE_DISPLAY

/*2107, Data Type: UNSIGNED8 */
      #define OD_STATUS_CODE                             CO_OD_RAM.STATUS_CODE

/*2200, Data Type: INTEGER16 */
      #define OD_JOINT_FORCE_CONSTANT                    CO_OD_RAM.JOINT_FORCE_CONSTANT

/*2201, Data Type: INTEGER16 */
      #define OD_JOINT_POSITION_CONSTANT                 CO_OD_RAM.JOINT_POSITION_CONSTANT

/*2202, Data Type: INTEGER16 */
      #define OD_JOINT_VELOCITY_CONSTANT                 CO_OD_RAM.JOINT_VELOCITY_CONSTANT

/*2203, Data Type: INTEGER16 */
      #define OD_JOINT_PID_GAIN_CONSTANT                 CO_OD_RAM.JOINT_PID_GAIN_CONSTANT

/*2204, Data Type: INTEGER16, Array[2] */
      #define OD_JOINT_FORCE_SETPOINT_MAX                CO_OD_RAM.JOINT_FORCE_SETPOINT_MAX
      #define ODL_JOINT_FORCE_SETPOINT_MAX_arrayLength   2
      #define ODA_JOINT_FORCE_SETPOINT_MAX_JOINT_FORCE_SETPOINT_MAX_1 0
      #define ODA_JOINT_FORCE_SETPOINT_MAX_JOINT_FORCE_SETPOINT_MAX_2 1

/*2205, Data Type: INTEGER16, Array[2] */
      #define OD_JOINT_POSITION_SETPOINT_MAX             CO_OD_RAM.JOINT_POSITION_SETPOINT_MAX
      #define ODL_JOINT_POSITION_SETPOINT_MAX_arrayLength 2
      #define ODA_JOINT_POSITION_SETPOINT_MAX_JOINT_POSITION_SETPOINT_MAX_1 0
      #define ODA_JOINT_POSITION_SETPOINT_MAX_JOINT_POSITION_SETPOINT_MAX_2 1

/*2206, Data Type: INTEGER16, Array[2] */
      #define OD_JOINT_POSITION_SETPOINT_MIN             CO_OD_RAM.JOINT_POSITION_SETPOINT_MIN
      #define ODL_JOINT_POSITION_SETPOINT_MIN_arrayLength 2
      #define ODA_JOINT_POSITION_SETPOINT_MIN_JOINT_POSITION_SETPOINT_MIN_1 0
      #define ODA_JOINT_POSITION_SETPOINT_MIN_JOINT_POSITION_SETPOINT_MIN_2 1

/*2207, Data Type: INTEGER16, Array[2] */
      #define OD_JOINT_VELOCITY_SETPOINT_MAX             CO_OD_RAM.JOINT_VELOCITY_SETPOINT_MAX
      #define ODL_JOINT_VELOCITY_SETPOINT_MAX_arrayLength 2
      #define ODA_JOINT_VELOCITY_SETPOINT_MAX_JOINT_VELOCITY_SETPOINT_MAX_1 0
      #define ODA_JOINT_VELOCITY_SETPOINT_MAX_JOINT_VELOCITY_SETPOINT_MAX_2 1

/*2208, Data Type: INTEGER16, Array[2] */
      #define OD_JOINT_FORCE_SETPOINT                    CO_OD_RAM.JOINT_FORCE_SETPOINT
      #define ODL_JOINT_FORCE_SETPOINT_arrayLength       2
      #define ODA_JOINT_FORCE_SETPOINT_JOINT_FORCE_SETPOINT_1 0
      #define ODA_JOINT_FORCE_SETPOINT_JOINT_FORCE_SETPOINT_2 1

/*2209, Data Type: INTEGER16, Array[2] */
      #define OD_JOINT_POSITION_SETPOINT                 CO_OD_RAM.JOINT_POSITION_SETPOINT
      #define ODL_JOINT_POSITION_SETPOINT_arrayLength    2
      #define ODA_JOINT_POSITION_SETPOINT_JOINT_POSITION_SETPOINT_1 0
      #define ODA_JOINT_POSITION_SETPOINT_JOINT_POSITION_SETPOINT_2 1

/*220A, Data Type: INTEGER16, Array[2] */
      #define OD_JOINT_VELOCITY_SETPOINT                 CO_OD_RAM.JOINT_VELOCITY_SETPOINT
      #define ODL_JOINT_VELOCITY_SETPOINT_arrayLength    2
      #define ODA_JOINT_VELOCITY_SETPOINT_JOINT_VELOCITY_SETPOINT_1 0
      #define ODA_JOINT_VELOCITY_SETPOINT_JOINT_VELOCITY_SETPOINT_2 1

/*220B, Data Type: INTEGER16, Array[2] */
      #define OD_JOINT_FORCE_ESTIMATE                    CO_OD_RAM.JOINT_FORCE_ESTIMATE
      #define ODL_JOINT_FORCE_ESTIMATE_arrayLength       2
      #define ODA_JOINT_FORCE_ESTIMATE_JOINT_FORCE_ESTIMATE_1 0
      #define ODA_JOINT_FORCE_ESTIMATE_JOINT_FORCE_ESTIMATE_2 1

/*220C, Data Type: INTEGER16, Array[2] */
      #define OD_JOINT_POSITION_ESTIMATE                 CO_OD_RAM.JOINT_POSITION_ESTIMATE
      #define ODL_JOINT_POSITION_ESTIMATE_arrayLength    2
      #define ODA_JOINT_POSITION_ESTIMATE_JOINT_POSITION_ESTIMATE_1 0
      #define ODA_JOINT_POSITION_ESTIMATE_JOINT_POSITION_ESTIMATE_2 1

/*220D, Data Type: INTEGER16, Array[2] */
      #define OD_JOINT_VELOCITY_ESTIMATE                 CO_OD_RAM.JOINT_VELOCITY_ESTIMATE
      #define ODL_JOINT_VELOCITY_ESTIMATE_arrayLength    2
      #define ODA_JOINT_VELOCITY_ESTIMATE_JOINT_VELOCITY_ESTIMATE_1 0
      #define ODA_JOINT_VELOCITY_ESTIMATE_JOINT_VELOCITY_ESTIMATE_2 1

/*220E, Data Type: INTEGER16, Array[2] */
      #define OD_JOINT_FORCE_DEMAND                      CO_OD_RAM.JOINT_FORCE_DEMAND
      #define ODL_JOINT_FORCE_DEMAND_arrayLength         2
      #define ODA_JOINT_FORCE_DEMAND_JOINT_FORCE_DEMAND_1 0
      #define ODA_JOINT_FORCE_DEMAND_JOINT_FORCE_DEMAND_2 1

/*220F, Data Type: INTEGER16, Array[2] */
      #define OD_JOINT_POSITION_P_GAIN                   CO_OD_RAM.JOINT_POSITION_P_GAIN
      #define ODL_JOINT_POSITION_P_GAIN_arrayLength      2
      #define ODA_JOINT_POSITION_P_GAIN_JOINT_POSITION_P_GAIN_1 0
      #define ODA_JOINT_POSITION_P_GAIN_JOINT_POSITION_P_GAIN_2 1

/*2210, Data Type: INTEGER16, Array[2] */
      #define OD_JOINT_POSITION_I_GAIN                   CO_OD_RAM.JOINT_POSITION_I_GAIN
      #define ODL_JOINT_POSITION_I_GAIN_arrayLength      2
      #define ODA_JOINT_POSITION_I_GAIN_JOINT_POSITION_I_GAIN_1 0
      #define ODA_JOINT_POSITION_I_GAIN_JOINT_POSITION_I_GAIN_2 1

/*2211, Data Type: INTEGER16, Array[2] */
      #define OD_JOINT_POSITION_D_GAIN                   CO_OD_RAM.JOINT_POSITION_D_GAIN
      #define ODL_JOINT_POSITION_D_GAIN_arrayLength      2
      #define ODA_JOINT_POSITION_D_GAIN_JOINT_POSITION_D_GAIN_1 0
      #define ODA_JOINT_POSITION_D_GAIN_JOINT_POSITION_D_GAIN_2 1

/*2213, Data Type: REAL32, Array[2] */
      #define OD_JOINT_POSITION_SENSOR_BIAS              CO_OD_RAM.JOINT_POSITION_SENSOR_BIAS
      #define ODL_JOINT_POSITION_SENSOR_BIAS_arrayLength 2
      #define ODA_JOINT_POSITION_SENSOR_BIAS_JOINT_POSITION_SENSOR_BIAS_1 0
      #define ODA_JOINT_POSITION_SENSOR_BIAS_JOINT_POSITION_SENSOR_BIAS_2 1

/*2214, Data Type: REAL32, Array[2] */
      #define OD_JOINT_POSITION_SENSOR_GAIN              CO_OD_RAM.JOINT_POSITION_SENSOR_GAIN
      #define ODL_JOINT_POSITION_SENSOR_GAIN_arrayLength 2
      #define ODA_JOINT_POSITION_SENSOR_GAIN_JOINT_POSITION_SENSOR_GAIN_1 0
      #define ODA_JOINT_POSITION_SENSOR_GAIN_JOINT_POSITION_SENSOR_GAIN_2 1

/*2215, Data Type: REAL32, Array[2] */
      #define OD_JOINT_POSITION_SENSOR_OFFSET            CO_OD_RAM.JOINT_POSITION_SENSOR_OFFSET
      #define ODL_JOINT_POSITION_SENSOR_OFFSET_arrayLength 2
      #define ODA_JOINT_POSITION_SENSOR_OFFSET_JOINT_POSITION_SENSOR_OFFSET_1 0
      #define ODA_JOINT_POSITION_SENSOR_OFFSET_JOINT_POSITION_SENSOR_OFFSET_2 1

/*2216, Data Type: REAL32, Array[2] */
      #define OD_JOINT_POSITION_DEADBAND                 CO_OD_RAM.JOINT_POSITION_DEADBAND
      #define ODL_JOINT_POSITION_DEADBAND_arrayLength    2
      #define ODA_JOINT_POSITION_DEADBAND_JOINT_POSITION_DEADBAND_1 0
      #define ODA_JOINT_POSITION_DEADBAND_JOINT_POSITION_DEADBAND_2 1

/*2217, Data Type: INTEGER16, Array[2] */
      #define OD_JOINT_FORCE_ESTIMATE_MAX                CO_OD_RAM.JOINT_FORCE_ESTIMATE_MAX
      #define ODL_JOINT_FORCE_ESTIMATE_MAX_arrayLength   2
      #define ODA_JOINT_FORCE_ESTIMATE_MAX_JOINT_FORCE_ESTIMATE_MAX_1 0
      #define ODA_JOINT_FORCE_ESTIMATE_MAX_JOINT_FORCE_ESTIMATE_MAX_2 1

/*2218, Data Type: INTEGER16, Array[2] */
      #define OD_JOINT_POSITION_ESTIMATE_MAX             CO_OD_RAM.JOINT_POSITION_ESTIMATE_MAX
      #define ODL_JOINT_POSITION_ESTIMATE_MAX_arrayLength 2
      #define ODA_JOINT_POSITION_ESTIMATE_MAX_JOINT_POSITION_ESTIMATE_MAX_1 0
      #define ODA_JOINT_POSITION_ESTIMATE_MAX_JOINT_POSITION_ESTIMATE_MAX_2 1

/*2219, Data Type: INTEGER16, Array[2] */
      #define OD_JOINT_POSITION_ESTIMATE_MIN             CO_OD_RAM.JOINT_POSITION_ESTIMATE_MIN
      #define ODL_JOINT_POSITION_ESTIMATE_MIN_arrayLength 2
      #define ODA_JOINT_POSITION_ESTIMATE_MIN_JOINT_POSITION_ESTIMATE_MAX_1 0
      #define ODA_JOINT_POSITION_ESTIMATE_MIN_JOINT_POSITION_ESTIMATE_MAX_2 1

/*221A, Data Type: INTEGER16, Array[2] */
      #define OD_JOINT_VELOCITY_ESTIMATE_MAX             CO_OD_RAM.JOINT_VELOCITY_ESTIMATE_MAX
      #define ODL_JOINT_VELOCITY_ESTIMATE_MAX_arrayLength 2
      #define ODA_JOINT_VELOCITY_ESTIMATE_MAX_JOINT_VELOCITY_ESTIMATE_MAX_1 0
      #define ODA_JOINT_VELOCITY_ESTIMATE_MAX_JOINT_VELOCITY_ESTIMATE_MAX_2 1

/*221B, Data Type: REAL32, Array[2] */
      #define OD_JOINT_POSITION_I_LIMIT                  CO_OD_RAM.JOINT_POSITION_I_LIMIT
      #define ODL_JOINT_POSITION_I_LIMIT_arrayLength     2
      #define ODA_JOINT_POSITION_I_LIMIT_JOINT_POSITION_I_LIMIT_1 0
      #define ODA_JOINT_POSITION_I_LIMIT_JOINT_POSITION_I_LIMIT_2 1

/*2300, Data Type: REAL32, Array[2] */
      #define OD_MOTOR_FORCE_SETPOINT_MAX                CO_OD_RAM.MOTOR_FORCE_SETPOINT_MAX
      #define ODL_MOTOR_FORCE_SETPOINT_MAX_arrayLength   2
      #define ODA_MOTOR_FORCE_SETPOINT_MAX_MOTOR_FORCE_SETPOINT_MAX_1 0
      #define ODA_MOTOR_FORCE_SETPOINT_MAX_MOTOR_FORCE_SETPOINT_MAX_2 1

/*2301, Data Type: REAL32, Array[2] */
      #define OD_MOTOR_CURRENT_SETPOINT_MAX              CO_OD_RAM.MOTOR_CURRENT_SETPOINT_MAX
      #define ODL_MOTOR_CURRENT_SETPOINT_MAX_arrayLength 2
      #define ODA_MOTOR_CURRENT_SETPOINT_MAX_MOTOR_CURRENT_SETPOINT_MAX_1 0
      #define ODA_MOTOR_CURRENT_SETPOINT_MAX_MOTOR_CURRENT_SETPOINT_MAX_2 1

/*2302, Data Type: REAL32, Array[2] */
      #define OD_MOTOR_FORCE_SETPOINT                    CO_OD_RAM.MOTOR_FORCE_SETPOINT
      #define ODL_MOTOR_FORCE_SETPOINT_arrayLength       2
      #define ODA_MOTOR_FORCE_SETPOINT_MOTOR_FORCE_SETPOINT_1 0
      #define ODA_MOTOR_FORCE_SETPOINT_MOTOR_FORCE_SETPOINT_2 1

/*2303, Data Type: REAL32, Array[2] */
      #define OD_MOTOR_CURRENT_SETPOINT                  CO_OD_RAM.MOTOR_CURRENT_SETPOINT
      #define ODL_MOTOR_CURRENT_SETPOINT_arrayLength     2
      #define ODA_MOTOR_CURRENT_SETPOINT_MOTOR_FORCE_ESTIMATE_1 0
      #define ODA_MOTOR_CURRENT_SETPOINT_MOTOR_FORCE_ESTIMATE_2 1

/*2304, Data Type: REAL32, Array[2] */
      #define OD_MOTOR_FORCE_ESTIMATE                    CO_OD_RAM.MOTOR_FORCE_ESTIMATE
      #define ODL_MOTOR_FORCE_ESTIMATE_arrayLength       2
      #define ODA_MOTOR_FORCE_ESTIMATE_MOTOR_FORCE_ESTIMATE_1 0
      #define ODA_MOTOR_FORCE_ESTIMATE_MOTOR_FORCE_ESTIMATE_2 1

/*2305, Data Type: REAL32, Array[2] */
      #define OD_MOTOR_CURRENT_ESTIMATE                  CO_OD_RAM.MOTOR_CURRENT_ESTIMATE
      #define ODL_MOTOR_CURRENT_ESTIMATE_arrayLength     2
      #define ODA_MOTOR_CURRENT_ESTIMATE_MOTOR_CURRENT_DEMAND_1 0
      #define ODA_MOTOR_CURRENT_ESTIMATE_MOTOR_CURRENT_DEMAND_2 1

/*2306, Data Type: REAL32, Array[2] */
      #define OD_MOTOR_FORCE_FF_CONSTANT                 CO_OD_RAM.MOTOR_FORCE_FF_CONSTANT
      #define ODL_MOTOR_FORCE_FF_CONSTANT_arrayLength    2
      #define ODA_MOTOR_FORCE_FF_CONSTANT_MOTOR_FORCE_FF_CONSTANT_1 0
      #define ODA_MOTOR_FORCE_FF_CONSTANT_MOTOR_FORCE_FF_CONSTANT_2 1

/*2307, Data Type: REAL32, Array[2] */
      #define OD_MOTOR_FORCE_P_GAIN                      CO_OD_RAM.MOTOR_FORCE_P_GAIN
      #define ODL_MOTOR_FORCE_P_GAIN_arrayLength         2
      #define ODA_MOTOR_FORCE_P_GAIN_MOTOR_FORCE_P_GAIN_1 0
      #define ODA_MOTOR_FORCE_P_GAIN_MOTOR_FORCE_P_GAIN_2 1

/*2308, Data Type: REAL32, Array[2] */
      #define OD_MOTOR_FORCE_I_GAIN                      CO_OD_RAM.MOTOR_FORCE_I_GAIN
      #define ODL_MOTOR_FORCE_I_GAIN_arrayLength         2
      #define ODA_MOTOR_FORCE_I_GAIN_MOTOR_FORCE_I_GAIN_1 0
      #define ODA_MOTOR_FORCE_I_GAIN_MOTOR_FORCE_I_GAIN_2 1

/*2309, Data Type: REAL32, Array[2] */
      #define OD_MOTOR_FORCE_D_GAIN                      CO_OD_RAM.MOTOR_FORCE_D_GAIN
      #define ODL_MOTOR_FORCE_D_GAIN_arrayLength         2
      #define ODA_MOTOR_FORCE_D_GAIN_MOTOR_FORCE_D_GAIN_1 0
      #define ODA_MOTOR_FORCE_D_GAIN_MOTOR_FORCE_D_GAIN_2 1

/*230A, Data Type: REAL32, Array[2] */
      #define OD_MOTOR_FORCE_ESTIMATE_BREAK_FREQUENCY    CO_OD_RAM.MOTOR_FORCE_ESTIMATE_BREAK_FREQUENCY
      #define ODL_MOTOR_FORCE_ESTIMATE_BREAK_FREQUENCY_arrayLength 2
      #define ODA_MOTOR_FORCE_ESTIMATE_BREAK_FREQUENCY_MOTOR_FORCE_ESTIMATE_BREAK_FREQUENCY_1 0
      #define ODA_MOTOR_FORCE_ESTIMATE_BREAK_FREQUENCY_MOTOR_FORCE_ESTIMATE_BREAK_FREQUENCY_2 1

/*230B, Data Type: REAL32, Array[2] */
      #define OD_MOTOR_CURRENT_P_GAIN                    CO_OD_RAM.MOTOR_CURRENT_P_GAIN
      #define ODL_MOTOR_CURRENT_P_GAIN_arrayLength       2
      #define ODA_MOTOR_CURRENT_P_GAIN_MOTOR_FORCE_SENSOR_BIAS_1 0
      #define ODA_MOTOR_CURRENT_P_GAIN_MOTOR_FORCE_SENSOR_BIAS_2 1

/*230C, Data Type: REAL32, Array[2] */
      #define OD_MOTOR_CURRENT_I_GAIN                    CO_OD_RAM.MOTOR_CURRENT_I_GAIN
      #define ODL_MOTOR_CURRENT_I_GAIN_arrayLength       2
      #define ODA_MOTOR_CURRENT_I_GAIN_MOTOR_FORCE_SENSOR_GAIN_1 0
      #define ODA_MOTOR_CURRENT_I_GAIN_MOTOR_FORCE_SENSOR_GAIN_2 1

/*230D, Data Type: REAL32, Array[2] */
      #define OD_MOTOR_FORCE_SENSOR_BIAS                 CO_OD_RAM.MOTOR_FORCE_SENSOR_BIAS
      #define ODL_MOTOR_FORCE_SENSOR_BIAS_arrayLength    2
      #define ODA_MOTOR_FORCE_SENSOR_BIAS_MOTOR_FORCE_SENSOR_OFFSET_1 0
      #define ODA_MOTOR_FORCE_SENSOR_BIAS_MOTOR_FORCE_SENSOR_OFFSET_2 1

/*230E, Data Type: REAL32, Array[2] */
      #define OD_MOTOR_FORCE_SENSOR_GAIN                 CO_OD_RAM.MOTOR_FORCE_SENSOR_GAIN
      #define ODL_MOTOR_FORCE_SENSOR_GAIN_arrayLength    2
      #define ODA_MOTOR_FORCE_SENSOR_GAIN_MOTOR_FORCE_SENSOR_OFFSET_1 0
      #define ODA_MOTOR_FORCE_SENSOR_GAIN_MOTOR_FORCE_SENSOR_OFFSET_2 1

/*230F, Data Type: REAL32, Array[2] */
      #define OD_MOTOR_FORCE_SENSOR_OFFSET               CO_OD_RAM.MOTOR_FORCE_SENSOR_OFFSET
      #define ODL_MOTOR_FORCE_SENSOR_OFFSET_arrayLength  2
      #define ODA_MOTOR_FORCE_SENSOR_OFFSET_MOTOR_FORCE_SENSOR_OFFSET_1 0
      #define ODA_MOTOR_FORCE_SENSOR_OFFSET_MOTOR_FORCE_SENSOR_OFFSET_2 1

/*2310, Data Type: REAL32, Array[2] */
      #define OD_MOTOR_CURRENT_SENSOR_BIAS               CO_OD_RAM.MOTOR_CURRENT_SENSOR_BIAS
      #define ODL_MOTOR_CURRENT_SENSOR_BIAS_arrayLength  2
      #define ODA_MOTOR_CURRENT_SENSOR_BIAS_MOTOR_FORCE_SENSOR_OFFSET_1 0
      #define ODA_MOTOR_CURRENT_SENSOR_BIAS_MOTOR_FORCE_SENSOR_OFFSET_2 1

/*2311, Data Type: REAL32, Array[2] */
      #define OD_MOTOR_CURRENT_SENSOR_GAIN               CO_OD_RAM.MOTOR_CURRENT_SENSOR_GAIN
      #define ODL_MOTOR_CURRENT_SENSOR_GAIN_arrayLength  2
      #define ODA_MOTOR_CURRENT_SENSOR_GAIN_MOTOR_FORCE_SENSOR_OFFSET_1 0
      #define ODA_MOTOR_CURRENT_SENSOR_GAIN_MOTOR_FORCE_SENSOR_OFFSET_2 1

/*2312, Data Type: REAL32, Array[2] */
      #define OD_MOTOR_CURRENT_SENSOR_OFFSET             CO_OD_RAM.MOTOR_CURRENT_SENSOR_OFFSET
      #define ODL_MOTOR_CURRENT_SENSOR_OFFSET_arrayLength 2
      #define ODA_MOTOR_CURRENT_SENSOR_OFFSET_MOTOR_FORCE_SENSOR_OFFSET_1 0
      #define ODA_MOTOR_CURRENT_SENSOR_OFFSET_MOTOR_FORCE_SENSOR_OFFSET_2 1

/*2313, Data Type: REAL32, Array[2] */
      #define OD_MOTOR_CURRENT_DEMAND                    CO_OD_RAM.MOTOR_CURRENT_DEMAND
      #define ODL_MOTOR_CURRENT_DEMAND_arrayLength       2
      #define ODA_MOTOR_CURRENT_DEMAND_MOTOR_CURRENT_DEMAND_1 0
      #define ODA_MOTOR_CURRENT_DEMAND_MOTOR_CURRENT_DEMAND_2 1

/*2314, Data Type: REAL32, Array[2] */
      #define OD_MOTOR_FORCE_DEADBAND                    CO_OD_RAM.MOTOR_FORCE_DEADBAND
      #define ODL_MOTOR_FORCE_DEADBAND_arrayLength       2
      #define ODA_MOTOR_FORCE_DEADBAND_MOTOR_FORCE_DEADBAND_1 0
      #define ODA_MOTOR_FORCE_DEADBAND_MOTOR_FORCE_DEADBAND_2 1

/*2315, Data Type: REAL32, Array[2] */
      #define OD_MOTOR_FORCE_ESTIMATE_MAX                CO_OD_RAM.MOTOR_FORCE_ESTIMATE_MAX
      #define ODL_MOTOR_FORCE_ESTIMATE_MAX_arrayLength   2
      #define ODA_MOTOR_FORCE_ESTIMATE_MAX_MOTOR_FORCE_ESTIMATE_MAX_1 0
      #define ODA_MOTOR_FORCE_ESTIMATE_MAX_MOTOR_FORCE_ESTIMATE_MAX_2 1

/*2316, Data Type: REAL32, Array[2] */
      #define OD_MOTOR_CURRENT_ESTIMATE_MAX              CO_OD_RAM.MOTOR_CURRENT_ESTIMATE_MAX
      #define ODL_MOTOR_CURRENT_ESTIMATE_MAX_arrayLength 2
      #define ODA_MOTOR_CURRENT_ESTIMATE_MAX_MOTOR_CURRENT_ESTIMATE_MAX_1 0
      #define ODA_MOTOR_CURRENT_ESTIMATE_MAX_MOTOR_CURRENT_ESTIMATE_MAX_2 1

/*2317, Data Type: REAL32, Array[2] */
      #define OD_MOTOR_FORCE_I_LIMIT                     CO_OD_RAM.MOTOR_FORCE_I_LIMIT
      #define ODL_MOTOR_FORCE_I_LIMIT_arrayLength        2
      #define ODA_MOTOR_FORCE_I_LIMIT_MOTOR_FORCE_I_LIMIT_1 0
      #define ODA_MOTOR_FORCE_I_LIMIT_MOTOR_FORCE_I_LIMIT_2 1

/*2318, Data Type: REAL32, Array[2] */
      #define OD_MOTOR_CURRENT_I_LIMIT                   CO_OD_RAM.MOTOR_CURRENT_I_LIMIT
      #define ODL_MOTOR_CURRENT_I_LIMIT_arrayLength      2
      #define ODA_MOTOR_CURRENT_I_LIMIT_MOTOR_CURRENT_I_LIMIT_1 0
      #define ODA_MOTOR_CURRENT_I_LIMIT_MOTOR_CURRENT_I_LIMIT_2 1

/*2319, Data Type: REAL32, Array[2] */
      #define OD_MOTOR_FORCE_RATE_OF_CHANGE_ESTIMATE_BREAK_FREQUENCY CO_OD_RAM.MOTOR_FORCE_RATE_OF_CHANGE_ESTIMATE_BREAK_FREQUENCY
      #define ODL_MOTOR_FORCE_RATE_OF_CHANGE_ESTIMATE_BREAK_FREQUENCY_arrayLength 2
      #define ODA_MOTOR_FORCE_RATE_OF_CHANGE_ESTIMATE_BREAK_FREQUENCY_MOTOR_FORCE_RATE_OF_CHANGE_ESTIMATE_BREAK_FREQUENCY_1 0
      #define ODA_MOTOR_FORCE_RATE_OF_CHANGE_ESTIMATE_BREAK_FREQUENCY_MOTOR_FORCE_RATE_OF_CHANGE_ESTIMATE_BREAK_FREQUENCY_2 1

/*231A, Data Type: REAL32, Array[2] */
      #define OD_MOTOR_POSITION_MEASUREMENT              CO_OD_RAM.MOTOR_POSITION_MEASUREMENT
      #define ODL_MOTOR_POSITION_MEASUREMENT_arrayLength 2
      #define ODA_MOTOR_POSITION_MEASUREMENT_MOTOR_POSITION_MEASUREMENT_1 0
      #define ODA_MOTOR_POSITION_MEASUREMENT_MOTOR_POSITION_MEASUREMENT_2 1

/*231B, Data Type: REAL32, Array[2] */
      #define OD_MOTOR_VELOCITY_ESTIMATE                 CO_OD_RAM.MOTOR_VELOCITY_ESTIMATE
      #define ODL_MOTOR_VELOCITY_ESTIMATE_arrayLength    2
      #define ODA_MOTOR_VELOCITY_ESTIMATE_MOTOR_VELOCITY_ESTIMATE_1 0
      #define ODA_MOTOR_VELOCITY_ESTIMATE_MOTOR_VELOCITY_ESTIMATE_2 1

/*231C, Data Type: REAL32, Array[2] */
      #define OD_MOTOR_VELOCITY_ESTIMATE_BREAK_FREQUENCY CO_OD_RAM.MOTOR_VELOCITY_ESTIMATE_BREAK_FREQUENCY
      #define ODL_MOTOR_VELOCITY_ESTIMATE_BREAK_FREQUENCY_arrayLength 2
      #define ODA_MOTOR_VELOCITY_ESTIMATE_BREAK_FREQUENCY_MOTOR_VELOCITY_ESTIMATE_BREAK_FREQUENCY_1 0
      #define ODA_MOTOR_VELOCITY_ESTIMATE_BREAK_FREQUENCY_MOTOR_VELOCITY_ESTIMATE_BREAK_FREQUENCY_2 1

/*231D, Data Type: REAL32, Array[2] */
      #define OD_MOTOR_POSITION_SENSOR_GAIN              CO_OD_RAM.MOTOR_POSITION_SENSOR_GAIN
      #define ODL_MOTOR_POSITION_SENSOR_GAIN_arrayLength 2
      #define ODA_MOTOR_POSITION_SENSOR_GAIN_MOTOR_POSITION_SENSOR_GAIN_1 0
      #define ODA_MOTOR_POSITION_SENSOR_GAIN_MOTOR_POSITION_SENSOR_GAIN_2 1

/*231E, Data Type: REAL32, Array[2] */
      #define OD_MOTOR_POSITION_SENSOR_OFFSET            CO_OD_RAM.MOTOR_POSITION_SENSOR_OFFSET
      #define ODL_MOTOR_POSITION_SENSOR_OFFSET_arrayLength 2
      #define ODA_MOTOR_POSITION_SENSOR_OFFSET_MOTOR_POSITION_SENSOR_OFFSET_1 0
      #define ODA_MOTOR_POSITION_SENSOR_OFFSET_MOTOR_POSITION_SENSOR_OFFSET_2 1

/*231F, Data Type: REAL32, Array[2] */
      #define OD_MOTOR_FORCE_RATE_OF_CHANGE_ESTIMATE     CO_OD_RAM.MOTOR_FORCE_RATE_OF_CHANGE_ESTIMATE
      #define ODL_MOTOR_FORCE_RATE_OF_CHANGE_ESTIMATE_arrayLength 2
      #define ODA_MOTOR_FORCE_RATE_OF_CHANGE_ESTIMATE_MOTOR_FORCE_RATE_OF_CHANGE_ESTIMATE_1 0
      #define ODA_MOTOR_FORCE_RATE_OF_CHANGE_ESTIMATE_MOTOR_FORCE_RATE_OF_CHANGE_ESTIMATE_2 1

/*2320, Data Type: REAL32, Array[2] */
      #define OD_MOTOR_FORCE_MEASUREMENT                 CO_OD_RAM.MOTOR_FORCE_MEASUREMENT
      #define ODL_MOTOR_FORCE_MEASUREMENT_arrayLength    2
      #define ODA_MOTOR_FORCE_MEASUREMENT_MOTOR_FORCE_MEASUREMENT_1 0
      #define ODA_MOTOR_FORCE_MEASUREMENT_MOTOR_FORCE_MEASUREMENT_2 1

/*2321, Data Type: INTEGER16, Array[2] */
      #define OD_JOINT_POSITION_MEASUREMENT              CO_OD_RAM.JOINT_POSITION_MEASUREMENT
      #define ODL_JOINT_POSITION_MEASUREMENT_arrayLength 2
      #define ODA_JOINT_POSITION_MEASUREMENT_JOINT_POSITION_MEASUREMENT_1 0
      #define ODA_JOINT_POSITION_MEASUREMENT_JOINT_POSITION_MEASUREMENT_2 1

/*2322, Data Type: REAL32, Array[2] */
      #define OD_JOINT_POSITION_ESTIMATE_BREAK_FREQUENCY CO_OD_RAM.JOINT_POSITION_ESTIMATE_BREAK_FREQUENCY
      #define ODL_JOINT_POSITION_ESTIMATE_BREAK_FREQUENCY_arrayLength 2
      #define ODA_JOINT_POSITION_ESTIMATE_BREAK_FREQUENCY_JOINT_POSITION_ESTIMATE_BREAK_FREQUENCY_1 0
      #define ODA_JOINT_POSITION_ESTIMATE_BREAK_FREQUENCY_JOINT_POSITION_ESTIMATE_BREAK_FREQUENCY_2 1

/*2323, Data Type: REAL32, Array[2] */
      #define OD_JOINT_VELOCITY_ESTIMATE_BREAK_FREQUENCY CO_OD_RAM.JOINT_VELOCITY_ESTIMATE_BREAK_FREQUENCY
      #define ODL_JOINT_VELOCITY_ESTIMATE_BREAK_FREQUENCY_arrayLength 2
      #define ODA_JOINT_VELOCITY_ESTIMATE_BREAK_FREQUENCY_JOINT_VELOCITY_ESTIMATE_BREAK_FREQUENCY_1 0
      #define ODA_JOINT_VELOCITY_ESTIMATE_BREAK_FREQUENCY_JOINT_VELOCITY_ESTIMATE_BREAK_FREQUENCY_2 1

/*2324, Data Type: REAL32, Array[2] */
      #define OD_MOTOR_FORCE_REFERENCE                   CO_OD_RAM.MOTOR_FORCE_REFERENCE
      #define ODL_MOTOR_FORCE_REFERENCE_arrayLength      2
      #define ODA_MOTOR_FORCE_REFERENCE_MOTOR_FORCE_REFERENCE_1 0
      #define ODA_MOTOR_FORCE_REFERENCE_MOTOR_FORCE_REFERENCE_2 1

/*2325, Data Type: REAL32, Array[2] */
      #define OD_MOTOR_FORCE_REFERENCE_BREAK_FREQUENCY   CO_OD_RAM.MOTOR_FORCE_REFERENCE_BREAK_FREQUENCY
      #define ODL_MOTOR_FORCE_REFERENCE_BREAK_FREQUENCY_arrayLength 2
      #define ODA_MOTOR_FORCE_REFERENCE_BREAK_FREQUENCY_MOTOR_FORCE_REFERENCE_BREAK_FREQUENCY_1 0
      #define ODA_MOTOR_FORCE_REFERENCE_BREAK_FREQUENCY_MOTOR_FORCE_REFERENCE_BREAK_FREQUENCY_2 1

/*6000, Data Type: UNSIGNED8, Array[8] */
      #define OD_readInput8Bit                           CO_OD_RAM.readInput8Bit
      #define ODL_readInput8Bit_arrayLength              8

/*6200, Data Type: UNSIGNED8, Array[8] */
      #define OD_writeOutput8Bit                         CO_OD_RAM.writeOutput8Bit
      #define ODL_writeOutput8Bit_arrayLength            8

/*6401, Data Type: INTEGER16, Array[12] */
      #define OD_readAnalogueInput16Bit                  CO_OD_RAM.readAnalogueInput16Bit
      #define ODL_readAnalogueInput16Bit_arrayLength     12

/*6411, Data Type: INTEGER16, Array[8] */
      #define OD_writeAnalogueOutput16Bit                CO_OD_RAM.writeAnalogueOutput16Bit
      #define ODL_writeAnalogueOutput16Bit_arrayLength   8


#endif

