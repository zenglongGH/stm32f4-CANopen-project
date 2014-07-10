/*******************************************************************************

   File - CO_OD.c
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


#include "CO_driver.h"
#include "CO_OD.h"
#include "CO_SDO.h"


/*******************************************************************************
   DEFINITION AND INITIALIZATION OF OBJECT DICTIONARY VARIABLES
*******************************************************************************/

/***** Definition for RAM variables *******************************************/
struct sCO_OD_RAM CO_OD_RAM = {
           CO_OD_FIRST_LAST_WORD,

/*1001*/ 0x0,
/*1002*/ 0x0L,
/*1003*/ {0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1010*/ {0x3L},
/*1011*/ {0x1L},
/*2100*/ {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
/*2103*/ 0x0,
/*2104*/ 0x0,
/*2105*/ 0x0,
/*2106*/ 0x0,
/*2107*/ 0x0,
/*2200*/ 0,
/*2201*/ 0,
/*2202*/ 0,
/*2203*/ 0,
/*2204*/ {0, 0},
/*2205*/ {0, 0},
/*2206*/ {0, 0},
/*2207*/ {0, 0},
/*2208*/ {0, 0},
/*2209*/ {0, 0},
/*220A*/ {0, 0},
/*220B*/ {0, 0},
/*220C*/ {0, 0},
/*220D*/ {0, 0},
/*220E*/ {0, 0},
/*220F*/ {0, 0},
/*2210*/ {0, 0},
/*2211*/ {0, 0},
/*2213*/ {0, 0},
/*2214*/ {0, 0},
/*2215*/ {0, 0},
/*2216*/ {0, 0},
/*2217*/ {0, 0},
/*2218*/ {0, 0},
/*2219*/ {0, 0},
/*221A*/ {0, 0},
/*221B*/ {0, 0},
/*2300*/ {0, 0},
/*2301*/ {0, 0},
/*2302*/ {0, 0},
/*2303*/ {0, 0},
/*2304*/ {0, 0},
/*2305*/ {0, 0},
/*2306*/ {0, 0},
/*2307*/ {0, 0},
/*2308*/ {0, 0},
/*2309*/ {0, 0},
/*230A*/ {0, 0},
/*230B*/ {0, 0},
/*230C*/ {0, 0},
/*230D*/ {0, 0},
/*230E*/ {0, 0},
/*230F*/ {0, 0},
/*2310*/ {0, 0},
/*2311*/ {0, 0},
/*2312*/ {0, 0},
/*2313*/ {0, 0},
/*2314*/ {0, 0},
/*2315*/ {0, 0},
/*2316*/ {0, 0},
/*2317*/ {0, 0},
/*2318*/ {0, 0},
/*2319*/ {0, 0},
/*231A*/ {0, 0},
/*231B*/ {0, 0},
/*231C*/ {0, 0},
/*231D*/ {0, 0},
/*231E*/ {0, 0},
/*231F*/ {0, 0},
/*2320*/ {0, 0},
/*2321*/ {0, 0},
/*2322*/ {0, 0},
/*2323*/ {0, 0},
/*2324*/ {0, 0},
/*2325*/ {0, 0},
/*6000*/ {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
/*6200*/ {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
/*6401*/ {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
/*6411*/ {0, 0, 0, 0, 0, 0, 0, 0},

           CO_OD_FIRST_LAST_WORD,
};


/***** Definition for EEPROM variables ****************************************/
struct sCO_OD_EEPROM CO_OD_EEPROM = {
           CO_OD_FIRST_LAST_WORD,



           CO_OD_FIRST_LAST_WORD,
};


/***** Definition for ROM variables *******************************************/
   struct sCO_OD_ROM CO_OD_ROM = {    //constant variables, stored in flash
           CO_OD_FIRST_LAST_WORD,

/*1000*/ 0x0L,
/*1005*/ 0x80L,
/*1006*/ 0x0L,
/*1007*/ 0x0L,
/*1008*/ {'E', 'X', 'T', 'R', 'E', 'M', 'I', 'S'},
/*1009*/ {'1', '.', '0', '0'},
/*100A*/ {'1', '.', '0', '0'},
/*1014*/ 0x80L,
/*1015*/ 0x64,
/*1016*/ {0x0L, 0x0L, 0x0L, 0x0L},
/*1017*/ 0x0,
/*1018*/ {0x4, 0x0L, 0x0L, 0x0L, 0x0L},
/*1019*/ 0x0,
/*1029*/ {0x1, 0x0, 0x1, 0x0, 0x0, 0x0},
/*1200*/{{0x2, 0x600L, 0x580L}},
/*1400*/{{0x2, 0x200L, 0xFF},
/*1401*/ {0x2, 0x300L, 0xFE},
/*1402*/ {0x2, 0x400L, 0xFE},
/*1403*/ {0x2, 0x500L, 0xFE}},
/*1600*/{{0x0, 0x62000108L, 0x62000208L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1601*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1602*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1603*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L}},
/*1800*/{{0x6, 0x180L, 0x1, 0x0, 0x0, 0x0, 0x0},
/*1801*/ {0x6, 0x280L, 0xFE, 0x0, 0x0, 0x0, 0x0},
/*1802*/ {0x6, 0x380L, 0xFE, 0x0, 0x0, 0x0, 0x0},
/*1803*/ {0x6, 0x480L, 0xFE, 0x0, 0x0, 0x0, 0x0}},
/*1A00*/{{0x0, 0x22400110L, 0x22400210L, 0x22400310L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1A01*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1A02*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1A03*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L}},
/*1F80*/ 0x0L,
/*2101*/ 0x30,
/*2102*/ 0x3E8,

           CO_OD_FIRST_LAST_WORD
};


/*******************************************************************************
   STRUCTURES FOR RECORD TYPE OBJECTS
*******************************************************************************/
/*0x1018*/ const CO_OD_entryRecord_t OD_record1018[5] = {
           {(void*)&CO_OD_ROM.identity.maxSubIndex, 0x05,  1},
           {(void*)&CO_OD_ROM.identity.vendorID, 0x85,  4},
           {(void*)&CO_OD_ROM.identity.productCode, 0x85,  4},
           {(void*)&CO_OD_ROM.identity.revisionNumber, 0x85,  4},
           {(void*)&CO_OD_ROM.identity.serialNumber, 0x85,  4}};
/*0x1200*/ const CO_OD_entryRecord_t OD_record1200[3] = {
           {(void*)&CO_OD_ROM.SDOServerParameter[0].maxSubIndex, 0x05,  1},
           {(void*)&CO_OD_ROM.SDOServerParameter[0].COB_IDClientToServer, 0x85,  4},
           {(void*)&CO_OD_ROM.SDOServerParameter[0].COB_IDServerToClient, 0x85,  4}};
/*0x1400*/ const CO_OD_entryRecord_t OD_record1400[3] = {
           {(void*)&CO_OD_ROM.RPDOCommunicationParameter[0].maxSubIndex, 0x05,  1},
           {(void*)&CO_OD_ROM.RPDOCommunicationParameter[0].COB_IDUsedByRPDO, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOCommunicationParameter[0].transmissionType, 0x0D,  1}};
/*0x1401*/ const CO_OD_entryRecord_t OD_record1401[3] = {
           {(void*)&CO_OD_ROM.RPDOCommunicationParameter[1].maxSubIndex, 0x05,  1},
           {(void*)&CO_OD_ROM.RPDOCommunicationParameter[1].COB_IDUsedByRPDO, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOCommunicationParameter[1].transmissionType, 0x0D,  1}};
/*0x1402*/ const CO_OD_entryRecord_t OD_record1402[3] = {
           {(void*)&CO_OD_ROM.RPDOCommunicationParameter[2].maxSubIndex, 0x05,  1},
           {(void*)&CO_OD_ROM.RPDOCommunicationParameter[2].COB_IDUsedByRPDO, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOCommunicationParameter[2].transmissionType, 0x0D,  1}};
/*0x1403*/ const CO_OD_entryRecord_t OD_record1403[3] = {
           {(void*)&CO_OD_ROM.RPDOCommunicationParameter[3].maxSubIndex, 0x05,  1},
           {(void*)&CO_OD_ROM.RPDOCommunicationParameter[3].COB_IDUsedByRPDO, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOCommunicationParameter[3].transmissionType, 0x0D,  1}};
/*0x1600*/ const CO_OD_entryRecord_t OD_record1600[9] = {
           {(void*)&CO_OD_ROM.RPDOMappingParameter[0].numberOfMappedObjects, 0x0D,  1},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[0].mappedObject1, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[0].mappedObject2, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[0].mappedObject3, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[0].mappedObject4, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[0].mappedObject5, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[0].mappedObject6, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[0].mappedObject7, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[0].mappedObject8, 0x8D,  4}};
/*0x1601*/ const CO_OD_entryRecord_t OD_record1601[9] = {
           {(void*)&CO_OD_ROM.RPDOMappingParameter[1].numberOfMappedObjects, 0x0D,  1},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[1].mappedObject1, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[1].mappedObject2, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[1].mappedObject3, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[1].mappedObject4, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[1].mappedObject5, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[1].mappedObject6, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[1].mappedObject7, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[1].mappedObject8, 0x8D,  4}};
/*0x1602*/ const CO_OD_entryRecord_t OD_record1602[9] = {
           {(void*)&CO_OD_ROM.RPDOMappingParameter[2].numberOfMappedObjects, 0x0D,  1},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[2].mappedObject1, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[2].mappedObject2, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[2].mappedObject3, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[2].mappedObject4, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[2].mappedObject5, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[2].mappedObject6, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[2].mappedObject7, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[2].mappedObject8, 0x8D,  4}};
/*0x1603*/ const CO_OD_entryRecord_t OD_record1603[9] = {
           {(void*)&CO_OD_ROM.RPDOMappingParameter[3].numberOfMappedObjects, 0x0D,  1},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[3].mappedObject1, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[3].mappedObject2, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[3].mappedObject3, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[3].mappedObject4, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[3].mappedObject5, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[3].mappedObject6, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[3].mappedObject7, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[3].mappedObject8, 0x8D,  4}};
/*0x1800*/ const CO_OD_entryRecord_t OD_record1800[7] = {
           {(void*)&CO_OD_ROM.TPDOCommunicationParameter[0].maxSubIndex, 0x05,  1},
           {(void*)&CO_OD_ROM.TPDOCommunicationParameter[0].COB_IDUsedByTPDO, 0x8D,  4},
           {(void*)&CO_OD_ROM.TPDOCommunicationParameter[0].transmissionType, 0x0D,  1},
           {(void*)&CO_OD_ROM.TPDOCommunicationParameter[0].inhibitTime, 0x8D,  2},
           {(void*)&CO_OD_ROM.TPDOCommunicationParameter[0].compatibilityEntry, 0x0D,  1},
           {(void*)&CO_OD_ROM.TPDOCommunicationParameter[0].eventTimer, 0x8D,  2},
           {(void*)&CO_OD_ROM.TPDOCommunicationParameter[0].SYNCStartValue, 0x0D,  1}};
/*0x1801*/ const CO_OD_entryRecord_t OD_record1801[7] = {
           {(void*)&CO_OD_ROM.TPDOCommunicationParameter[1].maxSubIndex, 0x05,  1},
           {(void*)&CO_OD_ROM.TPDOCommunicationParameter[1].COB_IDUsedByTPDO, 0x8D,  4},
           {(void*)&CO_OD_ROM.TPDOCommunicationParameter[1].transmissionType, 0x0D,  1},
           {(void*)&CO_OD_ROM.TPDOCommunicationParameter[1].inhibitTime, 0x8D,  2},
           {(void*)&CO_OD_ROM.TPDOCommunicationParameter[1].compatibilityEntry, 0x0D,  1},
           {(void*)&CO_OD_ROM.TPDOCommunicationParameter[1].eventTimer, 0x8D,  2},
           {(void*)&CO_OD_ROM.TPDOCommunicationParameter[1].SYNCStartValue, 0x0D,  1}};
/*0x1802*/ const CO_OD_entryRecord_t OD_record1802[7] = {
           {(void*)&CO_OD_ROM.TPDOCommunicationParameter[2].maxSubIndex, 0x05,  1},
           {(void*)&CO_OD_ROM.TPDOCommunicationParameter[2].COB_IDUsedByTPDO, 0x8D,  4},
           {(void*)&CO_OD_ROM.TPDOCommunicationParameter[2].transmissionType, 0x0D,  1},
           {(void*)&CO_OD_ROM.TPDOCommunicationParameter[2].inhibitTime, 0x8D,  2},
           {(void*)&CO_OD_ROM.TPDOCommunicationParameter[2].compatibilityEntry, 0x0D,  1},
           {(void*)&CO_OD_ROM.TPDOCommunicationParameter[2].eventTimer, 0x8D,  2},
           {(void*)&CO_OD_ROM.TPDOCommunicationParameter[2].SYNCStartValue, 0x0D,  1}};
/*0x1803*/ const CO_OD_entryRecord_t OD_record1803[7] = {
           {(void*)&CO_OD_ROM.TPDOCommunicationParameter[3].maxSubIndex, 0x05,  1},
           {(void*)&CO_OD_ROM.TPDOCommunicationParameter[3].COB_IDUsedByTPDO, 0x8D,  4},
           {(void*)&CO_OD_ROM.TPDOCommunicationParameter[3].transmissionType, 0x0D,  1},
           {(void*)&CO_OD_ROM.TPDOCommunicationParameter[3].inhibitTime, 0x8D,  2},
           {(void*)&CO_OD_ROM.TPDOCommunicationParameter[3].compatibilityEntry, 0x0D,  1},
           {(void*)&CO_OD_ROM.TPDOCommunicationParameter[3].eventTimer, 0x8D,  2},
           {(void*)&CO_OD_ROM.TPDOCommunicationParameter[3].SYNCStartValue, 0x0D,  1}};
/*0x1A00*/ const CO_OD_entryRecord_t OD_record1A00[9] = {
           {(void*)&CO_OD_ROM.TPDOMappingParameter[0].numberOfMappedObjects, 0x0D,  1},
           {(void*)&CO_OD_ROM.TPDOMappingParameter[0].mappedObject1, 0x8D,  4},
           {(void*)&CO_OD_ROM.TPDOMappingParameter[0].mappedObject2, 0x8D,  4},
           {(void*)&CO_OD_ROM.TPDOMappingParameter[0].mappedObject3, 0x8D,  4},
           {(void*)&CO_OD_ROM.TPDOMappingParameter[0].mappedObject4, 0x8D,  4},
           {(void*)&CO_OD_ROM.TPDOMappingParameter[0].mappedObject5, 0x8D,  4},
           {(void*)&CO_OD_ROM.TPDOMappingParameter[0].mappedObject6, 0x8D,  4},
           {(void*)&CO_OD_ROM.TPDOMappingParameter[0].mappedObject7, 0x8D,  4},
           {(void*)&CO_OD_ROM.TPDOMappingParameter[0].mappedObject8, 0x8D,  4}};
/*0x1A01*/ const CO_OD_entryRecord_t OD_record1A01[9] = {
           {(void*)&CO_OD_ROM.TPDOMappingParameter[1].numberOfMappedObjects, 0x0D,  1},
           {(void*)&CO_OD_ROM.TPDOMappingParameter[1].mappedObject1, 0x8D,  4},
           {(void*)&CO_OD_ROM.TPDOMappingParameter[1].mappedObject2, 0x8D,  4},
           {(void*)&CO_OD_ROM.TPDOMappingParameter[1].mappedObject3, 0x8D,  4},
           {(void*)&CO_OD_ROM.TPDOMappingParameter[1].mappedObject4, 0x8D,  4},
           {(void*)&CO_OD_ROM.TPDOMappingParameter[1].mappedObject5, 0x8D,  4},
           {(void*)&CO_OD_ROM.TPDOMappingParameter[1].mappedObject6, 0x8D,  4},
           {(void*)&CO_OD_ROM.TPDOMappingParameter[1].mappedObject7, 0x8D,  4},
           {(void*)&CO_OD_ROM.TPDOMappingParameter[1].mappedObject8, 0x8D,  4}};
/*0x1A02*/ const CO_OD_entryRecord_t OD_record1A02[9] = {
           {(void*)&CO_OD_ROM.TPDOMappingParameter[2].numberOfMappedObjects, 0x0D,  1},
           {(void*)&CO_OD_ROM.TPDOMappingParameter[2].mappedObject1, 0x8D,  4},
           {(void*)&CO_OD_ROM.TPDOMappingParameter[2].mappedObject2, 0x8D,  4},
           {(void*)&CO_OD_ROM.TPDOMappingParameter[2].mappedObject3, 0x8D,  4},
           {(void*)&CO_OD_ROM.TPDOMappingParameter[2].mappedObject4, 0x8D,  4},
           {(void*)&CO_OD_ROM.TPDOMappingParameter[2].mappedObject5, 0x8D,  4},
           {(void*)&CO_OD_ROM.TPDOMappingParameter[2].mappedObject6, 0x8D,  4},
           {(void*)&CO_OD_ROM.TPDOMappingParameter[2].mappedObject7, 0x8D,  4},
           {(void*)&CO_OD_ROM.TPDOMappingParameter[2].mappedObject8, 0x8D,  4}};
/*0x1A03*/ const CO_OD_entryRecord_t OD_record1A03[9] = {
           {(void*)&CO_OD_ROM.TPDOMappingParameter[3].numberOfMappedObjects, 0x0D,  1},
           {(void*)&CO_OD_ROM.TPDOMappingParameter[3].mappedObject1, 0x8D,  4},
           {(void*)&CO_OD_ROM.TPDOMappingParameter[3].mappedObject2, 0x8D,  4},
           {(void*)&CO_OD_ROM.TPDOMappingParameter[3].mappedObject3, 0x8D,  4},
           {(void*)&CO_OD_ROM.TPDOMappingParameter[3].mappedObject4, 0x8D,  4},
           {(void*)&CO_OD_ROM.TPDOMappingParameter[3].mappedObject5, 0x8D,  4},
           {(void*)&CO_OD_ROM.TPDOMappingParameter[3].mappedObject6, 0x8D,  4},
           {(void*)&CO_OD_ROM.TPDOMappingParameter[3].mappedObject7, 0x8D,  4},
           {(void*)&CO_OD_ROM.TPDOMappingParameter[3].mappedObject8, 0x8D,  4}};


/*******************************************************************************
   OBJECT DICTIONARY
*******************************************************************************/
const CO_OD_entry_t CO_OD[CO_OD_NoOfElements] = {
{0x1000, 0x00, 0x85,  4, (void*)&CO_OD_ROM.deviceType},
{0x1001, 0x00, 0x36,  1, (void*)&CO_OD_RAM.errorRegister},
{0x1002, 0x00, 0xB6,  4, (void*)&CO_OD_RAM.manufacturerStatusRegister},
{0x1003, 0x08, 0x8E,  4, (void*)&CO_OD_RAM.preDefinedErrorField[0]},
{0x1005, 0x00, 0x8D,  4, (void*)&CO_OD_ROM.COB_ID_SYNCMessage},
{0x1006, 0x00, 0x8D,  4, (void*)&CO_OD_ROM.communicationCyclePeriod},
{0x1007, 0x00, 0x8D,  4, (void*)&CO_OD_ROM.synchronousWindowLength},
{0x1008, 0x00, 0x05,  8, (void*)&CO_OD_ROM.manufacturerDeviceName[0]},
{0x1009, 0x00, 0x05,  4, (void*)&CO_OD_ROM.manufacturerHardwareVersion[0]},
{0x100A, 0x00, 0x05,  4, (void*)&CO_OD_ROM.manufacturerSoftwareVersion[0]},
{0x1010, 0x01, 0x8E,  4, (void*)&CO_OD_RAM.storeParameters[0]},
{0x1011, 0x01, 0x8E,  4, (void*)&CO_OD_RAM.restoreDefaultParameters[0]},
{0x1014, 0x00, 0x85,  4, (void*)&CO_OD_ROM.COB_ID_EMCY},
{0x1015, 0x00, 0x8D,  2, (void*)&CO_OD_ROM.inhibitTimeEMCY},
{0x1016, 0x04, 0x8D,  4, (void*)&CO_OD_ROM.consumerHeartbeatTime[0]},
{0x1017, 0x00, 0x8D,  2, (void*)&CO_OD_ROM.producerHeartbeatTime},
{0x1018, 0x04, 0x00,  0, (void*)&OD_record1018},
{0x1019, 0x00, 0x0D,  1, (void*)&CO_OD_ROM.synchronousCounterOverflowValue},
{0x1029, 0x06, 0x0D,  1, (void*)&CO_OD_ROM.errorBehavior[0]},
{0x1200, 0x02, 0x00,  0, (void*)&OD_record1200},
{0x1400, 0x02, 0x00,  0, (void*)&OD_record1400},
{0x1401, 0x02, 0x00,  0, (void*)&OD_record1401},
{0x1402, 0x02, 0x00,  0, (void*)&OD_record1402},
{0x1403, 0x02, 0x00,  0, (void*)&OD_record1403},
{0x1600, 0x08, 0x00,  0, (void*)&OD_record1600},
{0x1601, 0x08, 0x00,  0, (void*)&OD_record1601},
{0x1602, 0x08, 0x00,  0, (void*)&OD_record1602},
{0x1603, 0x08, 0x00,  0, (void*)&OD_record1603},
{0x1800, 0x06, 0x00,  0, (void*)&OD_record1800},
{0x1801, 0x06, 0x00,  0, (void*)&OD_record1801},
{0x1802, 0x06, 0x00,  0, (void*)&OD_record1802},
{0x1803, 0x06, 0x00,  0, (void*)&OD_record1803},
{0x1A00, 0x08, 0x00,  0, (void*)&OD_record1A00},
{0x1A01, 0x08, 0x00,  0, (void*)&OD_record1A01},
{0x1A02, 0x08, 0x00,  0, (void*)&OD_record1A02},
{0x1A03, 0x08, 0x00,  0, (void*)&OD_record1A03},
{0x1F80, 0x00, 0x8D,  4, (void*)&CO_OD_ROM.NMTStartup},
{0x2100, 0x00, 0x36, 10, (void*)&CO_OD_RAM.errorStatusBits[0]},
{0x2101, 0x00, 0x0D,  1, (void*)&CO_OD_ROM.CANNodeID},
{0x2102, 0x00, 0x8D,  2, (void*)&CO_OD_ROM.CANBitRate},
{0x2103, 0x00, 0x8E,  2, (void*)&CO_OD_RAM.SYNCCounter},
{0x2104, 0x00, 0x86,  2, (void*)&CO_OD_RAM.SYNCTime},
{0x2105, 0x00, 0x3E,  1, (void*)&CO_OD_RAM.OPERATING_MODE},
{0x2106, 0x00, 0x7E,  1, (void*)&CO_OD_RAM.OPERATING_MODE_DISPLAY},
{0x2107, 0x00, 0x3E,  1, (void*)&CO_OD_RAM.STATUS_CODE},
{0x2200, 0x00, 0xBE,  2, (void*)&CO_OD_RAM.JOINT_FORCE_CONSTANT},
{0x2201, 0x00, 0xBE,  2, (void*)&CO_OD_RAM.JOINT_POSITION_CONSTANT},
{0x2202, 0x00, 0xBE,  2, (void*)&CO_OD_RAM.JOINT_VELOCITY_CONSTANT},
{0x2203, 0x00, 0xBE,  2, (void*)&CO_OD_RAM.JOINT_PID_GAIN_CONSTANT},
{0x2204, 0x02, 0xBE,  2, (void*)&CO_OD_RAM.JOINT_FORCE_SETPOINT_MAX[0]},
{0x2205, 0x02, 0xBE,  2, (void*)&CO_OD_RAM.JOINT_POSITION_SETPOINT_MAX[0]},
{0x2206, 0x02, 0xBE,  2, (void*)&CO_OD_RAM.JOINT_POSITION_SETPOINT_MIN[0]},
{0x2207, 0x02, 0xBE,  2, (void*)&CO_OD_RAM.JOINT_VELOCITY_SETPOINT_MAX[0]},
{0x2208, 0x02, 0xBE,  2, (void*)&CO_OD_RAM.JOINT_FORCE_SETPOINT[0]},
{0x2209, 0x02, 0xBE,  2, (void*)&CO_OD_RAM.JOINT_POSITION_SETPOINT[0]},
{0x220A, 0x02, 0xBE,  2, (void*)&CO_OD_RAM.JOINT_VELOCITY_SETPOINT[0]},
{0x220B, 0x02, 0xBE,  2, (void*)&CO_OD_RAM.JOINT_FORCE_ESTIMATE[0]},
{0x220C, 0x02, 0xBE,  2, (void*)&CO_OD_RAM.JOINT_POSITION_ESTIMATE[0]},
{0x220D, 0x02, 0xBE,  2, (void*)&CO_OD_RAM.JOINT_VELOCITY_ESTIMATE[0]},
{0x220E, 0x02, 0xBE,  2, (void*)&CO_OD_RAM.JOINT_FORCE_DEMAND[0]},
{0x220F, 0x02, 0xBE,  2, (void*)&CO_OD_RAM.JOINT_POSITION_P_GAIN[0]},
{0x2210, 0x02, 0xBE,  2, (void*)&CO_OD_RAM.JOINT_POSITION_I_GAIN[0]},
{0x2211, 0x02, 0xBE,  2, (void*)&CO_OD_RAM.JOINT_POSITION_D_GAIN[0]},
{0x2213, 0x02, 0xBE,  4, (void*)&CO_OD_RAM.JOINT_POSITION_SENSOR_BIAS[0]},
{0x2214, 0x02, 0xBE,  4, (void*)&CO_OD_RAM.JOINT_POSITION_SENSOR_GAIN[0]},
{0x2215, 0x02, 0xBE,  4, (void*)&CO_OD_RAM.JOINT_POSITION_SENSOR_OFFSET[0]},
{0x2216, 0x02, 0xFE,  4, (void*)&CO_OD_RAM.JOINT_POSITION_DEADBAND[0]},
{0x2217, 0x02, 0xBE,  2, (void*)&CO_OD_RAM.JOINT_FORCE_ESTIMATE_MAX[0]},
{0x2218, 0x02, 0xBE,  2, (void*)&CO_OD_RAM.JOINT_POSITION_ESTIMATE_MAX[0]},
{0x2219, 0x02, 0xBE,  2, (void*)&CO_OD_RAM.JOINT_POSITION_ESTIMATE_MIN[0]},
{0x221A, 0x02, 0xBE,  2, (void*)&CO_OD_RAM.JOINT_VELOCITY_ESTIMATE_MAX[0]},
{0x221B, 0x02, 0xBE,  4, (void*)&CO_OD_RAM.JOINT_POSITION_I_LIMIT[0]},
{0x2300, 0x02, 0xBE,  4, (void*)&CO_OD_RAM.MOTOR_FORCE_SETPOINT_MAX[0]},
{0x2301, 0x02, 0xBE,  4, (void*)&CO_OD_RAM.MOTOR_CURRENT_SETPOINT_MAX[0]},
{0x2302, 0x02, 0xBE,  4, (void*)&CO_OD_RAM.MOTOR_FORCE_SETPOINT[0]},
{0x2303, 0x02, 0xBE,  4, (void*)&CO_OD_RAM.MOTOR_CURRENT_SETPOINT[0]},
{0x2304, 0x02, 0xFE,  4, (void*)&CO_OD_RAM.MOTOR_FORCE_ESTIMATE[0]},
{0x2305, 0x02, 0xFE,  4, (void*)&CO_OD_RAM.MOTOR_CURRENT_ESTIMATE[0]},
{0x2306, 0x02, 0xBE,  4, (void*)&CO_OD_RAM.MOTOR_FORCE_FF_CONSTANT[0]},
{0x2307, 0x02, 0xBE,  4, (void*)&CO_OD_RAM.MOTOR_FORCE_P_GAIN[0]},
{0x2308, 0x02, 0xBE,  4, (void*)&CO_OD_RAM.MOTOR_FORCE_I_GAIN[0]},
{0x2309, 0x02, 0xBE,  4, (void*)&CO_OD_RAM.MOTOR_FORCE_D_GAIN[0]},
{0x230A, 0x02, 0xFE,  4, (void*)&CO_OD_RAM.MOTOR_FORCE_ESTIMATE_BREAK_FREQUENCY[0]},
{0x230B, 0x02, 0xBE,  4, (void*)&CO_OD_RAM.MOTOR_CURRENT_P_GAIN[0]},
{0x230C, 0x02, 0xBE,  4, (void*)&CO_OD_RAM.MOTOR_CURRENT_I_GAIN[0]},
{0x230D, 0x02, 0xBE,  4, (void*)&CO_OD_RAM.MOTOR_FORCE_SENSOR_BIAS[0]},
{0x230E, 0x02, 0xBE,  4, (void*)&CO_OD_RAM.MOTOR_FORCE_SENSOR_GAIN[0]},
{0x230F, 0x02, 0xBE,  4, (void*)&CO_OD_RAM.MOTOR_FORCE_SENSOR_OFFSET[0]},
{0x2310, 0x02, 0xBE,  4, (void*)&CO_OD_RAM.MOTOR_CURRENT_SENSOR_BIAS[0]},
{0x2311, 0x02, 0xBE,  4, (void*)&CO_OD_RAM.MOTOR_CURRENT_SENSOR_GAIN[0]},
{0x2312, 0x02, 0xBE,  4, (void*)&CO_OD_RAM.MOTOR_CURRENT_SENSOR_OFFSET[0]},
{0x2313, 0x02, 0xBE,  4, (void*)&CO_OD_RAM.MOTOR_CURRENT_DEMAND[0]},
{0x2314, 0x02, 0xFE,  4, (void*)&CO_OD_RAM.MOTOR_FORCE_DEADBAND[0]},
{0x2315, 0x02, 0xBE,  4, (void*)&CO_OD_RAM.MOTOR_FORCE_ESTIMATE_MAX[0]},
{0x2316, 0x02, 0xBE,  4, (void*)&CO_OD_RAM.MOTOR_CURRENT_ESTIMATE_MAX[0]},
{0x2317, 0x02, 0xBE,  4, (void*)&CO_OD_RAM.MOTOR_FORCE_I_LIMIT[0]},
{0x2318, 0x02, 0xBE,  4, (void*)&CO_OD_RAM.MOTOR_CURRENT_I_LIMIT[0]},
{0x2319, 0x02, 0xBE,  4, (void*)&CO_OD_RAM.MOTOR_FORCE_RATE_OF_CHANGE_ESTIMATE_BREAK_FREQUENCY[0]},
{0x231A, 0x02, 0xBE,  4, (void*)&CO_OD_RAM.MOTOR_POSITION_MEASUREMENT[0]},
{0x231B, 0x02, 0xBE,  4, (void*)&CO_OD_RAM.MOTOR_VELOCITY_ESTIMATE[0]},
{0x231C, 0x02, 0xBE,  4, (void*)&CO_OD_RAM.MOTOR_VELOCITY_ESTIMATE_BREAK_FREQUENCY[0]},
{0x231D, 0x02, 0xBE,  4, (void*)&CO_OD_RAM.MOTOR_POSITION_SENSOR_GAIN[0]},
{0x231E, 0x02, 0xBE,  4, (void*)&CO_OD_RAM.MOTOR_POSITION_SENSOR_OFFSET[0]},
{0x231F, 0x02, 0xBE,  4, (void*)&CO_OD_RAM.MOTOR_FORCE_RATE_OF_CHANGE_ESTIMATE[0]},
{0x2320, 0x02, 0xBE,  4, (void*)&CO_OD_RAM.MOTOR_FORCE_MEASUREMENT[0]},
{0x2321, 0x02, 0xBE,  2, (void*)&CO_OD_RAM.JOINT_POSITION_MEASUREMENT[0]},
{0x2322, 0x02, 0xBE,  4, (void*)&CO_OD_RAM.JOINT_POSITION_ESTIMATE_BREAK_FREQUENCY[0]},
{0x2323, 0x02, 0xBE,  4, (void*)&CO_OD_RAM.JOINT_VELOCITY_ESTIMATE_BREAK_FREQUENCY[0]},
{0x2324, 0x02, 0xBE,  4, (void*)&CO_OD_RAM.MOTOR_FORCE_REFERENCE[0]},
{0x2325, 0x02, 0xBE,  4, (void*)&CO_OD_RAM.MOTOR_FORCE_REFERENCE_BREAK_FREQUENCY[0]},
{0x6000, 0x08, 0x76,  1, (void*)&CO_OD_RAM.readInput8Bit[0]},
{0x6200, 0x08, 0x3E,  1, (void*)&CO_OD_RAM.writeOutput8Bit[0]},
{0x6401, 0x0C, 0xB6,  2, (void*)&CO_OD_RAM.readAnalogueInput16Bit[0]},
{0x6411, 0x08, 0xBE,  2, (void*)&CO_OD_RAM.writeAnalogueOutput16Bit[0]},
};

