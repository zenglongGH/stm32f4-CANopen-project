/*******************************************************************************

   File: eeprom.h
   Object for retentive storage of Object Dictionary.

   Copyright (C) 2004-2010 Janez Paternoster

   License: GNU Lesser General Public License (LGPL).

   <http://canopennode.sourceforge.net>
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

*******************************************************************************/

#ifndef _EEPROM_H
#define _EEPROM_H


#include "CO_driver.h"
#include "CO_OD.h"

/*******************************************************************************
   Topic: EEPROM

   Usage of device file system or SRAM for storing non-volatile variables.

   Two blocks of CANopen Object Dictionary data are stored as non-volatile:
   OD_EEPROM - Stored is in internal battery powered SRAM from address 0. Data
               are stored automatically on change. No data corruption control
               is made. Data are load on startup.
   OD_ROM    - Stored in file named "OD_ROM01.dat". Data integrity is
               verified with CRC.
               Data are stored on special CANopen command - Writing 0x65766173
               into Object dictionary (index 1010, subindex 1). Default values
               are restored after reset, if writing 0x64616F6C into (1011, 1).
*******************************************************************************/


/*******************************************************************************
   Object: EE_t

   Variables for eeprom object.

   Variables:
      OD_EEPROMAddress        - See parameters in <EE_init_1>.
      OD_EEPROMSize           - OD_EEPROMSize in words (not bytes).
      OD_ROMAddress           - See parameters in <EE_init_1>.
      OD_ROMSize              - See parameters in <EE_init_1>.
      pSRAM                   - Pointer to start address of the battery powered SRAM
      OD_EEPROMCurrentIndex   - Internal variable controls the OD_EEPROM vrite.
*******************************************************************************/
typedef struct{
   UNSIGNED32    *OD_EEPROMAddress;
   UNSIGNED32     OD_EEPROMSize;
   UNSIGNED8     *OD_ROMAddress;
   UNSIGNED32     OD_ROMSize;
   UNSIGNED32    *pSRAM;
   UNSIGNED32     OD_EEPROMCurrentIndex;
}EE_t;


/*******************************************************************************
   Function: EE_init_1

   First part of eeprom initialization. Called once after microcontroller reset.

   Allocate memory for object, configure SPI port for use with 25LCxxx, read
   eeprom and store to OD_EEPROM and OD_ROM.

   Variables:
      ppEE                    - Pointer to address of eeprom object <EE_t>.
      OD_EEPROMAddress        - Address of OD_EEPROM structure from object dictionary.
      OD_EEPROMSize           - Size of OD_EEPROM structure from object dictionary.
      OD_ROMAddress           - Address of OD_ROM structure from object dictionary.
      OD_ROMSize              - Size of OD_ROM structure from object dictionary.

   Return:
      CO_ERROR_NO             - Operation completed successfully.
      CO_ERROR_OUT_OF_MEMORY  - Memory allocation failed.
      CO_ERROR_DATA_CORRUPT   - Data in eeprom corrupt.
      CO_ERROR_CRC            - CRC from MBR does not match the CRC of OD_ROM block in eeprom.
*******************************************************************************/
INTEGER16 EE_init_1(
      EE_t            **ppEE,
      UNSIGNED8        *OD_EEPROMAddress,
      UNSIGNED32        OD_EEPROMSize,
      UNSIGNED8        *OD_ROMAddress,
      UNSIGNED32        OD_ROMSize);


/*******************************************************************************
   Function: EE_delete

   Delete EEPROM object and free memory.

   Parameters:
      ppEE              - Pointer to pointer to EEPROM object <EE_t>.
                          Pointer to object is set to 0.
*******************************************************************************/
void EE_delete(EE_t **ppEE);


/*******************************************************************************
   Function: EE_init_2

   Second part of eeprom initialization. Called after CANopen communication reset.

   Call functions CO_OD_configureArgumentForODF() and CO_errorReport() if necessary.

   Variables:
      EE          - Pointer eeprom object <EE_t>.
      EEStatus    - Return value from <EE_init_1>.
      SDO         - Pointer to SDO object <CO_SDO_t>.
      EM          - Pointer to Emergency object <CO_emergencyReport_t>.
*******************************************************************************/
#define EE_init_2(EE, EEStatus, SDO, EM)                                   \
   CO_OD_configureArgumentForODF(SDO, 0x1010, (void*)EE);                  \
   CO_OD_configureArgumentForODF(SDO, 0x1011, (void*)EE);                  \
   if(EEStatus) CO_errorReport(EM, ERROR_NON_VOLATILE_MEMORY, EEStatus)


/*******************************************************************************
   Function: EE_process

   Process eeprom object.

   Function must be called cyclically. It strores variables from OD_EEPROM data
   block into eeprom byte by byte (only if values are different).

   Parameters:
      EE       - Pointer to eeprom object <EE_t>.
*******************************************************************************/
void EE_process(EE_t *EE);


/*******************************************************************************
   Function: CRC16

   Calculate 16 bit CRC code from string.

   See DALLAS Application Note 27.

   Parameters:
      str         - Pointer to array of characters.
      len         - Length of above array.

   Return:
      Calculated CRC value of the str.
*******************************************************************************/
UNSIGNED16 CRC16(UNSIGNED8 *str, UNSIGNED16 len);


#endif
