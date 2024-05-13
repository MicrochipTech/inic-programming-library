/*------------------------------------------------------------------------------------------------*/
/* (c) 2018 Microchip Technology Inc. and its subsidiaries.                                       */
/*                                                                                                */
/* You may use this software and any derivatives exclusively with Microchip products.             */
/*                                                                                                */
/* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR    */
/* STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,       */
/* MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP       */
/* PRODUCTS, COMBINATION WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.                      */
/*                                                                                                */
/* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR        */
/* CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE,    */
/* HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE       */
/* FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS   */
/* IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE  */
/* PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.                                                  */
/*                                                                                                */
/* MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE TERMS.            */
/*------------------------------------------------------------------------------------------------*/

/*! \file   ipl_dmp.h
 *  \brief  Dump functionality for INIC Programming Library
 *  \author Roland Trissl (RTR)
 *  \note   For support related to this code contact http://www.microchip.com/support.
 */

#ifndef IPL_DMP_H
#define IPL_DMP_H

#include "ipl_cfg.h"

#ifdef IPL_TRACETAG_DUMP

uint8_t Ipl_DumpOtpMem(uint32_t lData, uint8_t pData[]);
uint8_t Ipl_DumpInfoMem(uint32_t lData, uint8_t pData[]);
uint8_t Ipl_DumpProgMem(uint32_t lData, uint8_t pData[]);
uint8_t Ipl_DumpTestMem(uint32_t lData, uint8_t pData[]);
uint8_t Ipl_DumpRam(uint32_t lData, uint8_t pData[]);
uint8_t Ipl_DumpIOReg(uint32_t lData, uint8_t pData[]);
uint8_t Ipl_DumpCPUReg(uint32_t lData, uint8_t pData[]);
uint8_t Ipl_DumpExtIOReg(uint32_t lData, uint8_t pData[]);
uint8_t Ipl_DumpDataBuf(uint32_t lData, uint8_t pData[]);
uint8_t Ipl_DumpRT(uint32_t lData, uint8_t pData[]);
uint8_t Ipl_DumpRF0(uint32_t lData, uint8_t pData[]);
uint8_t Ipl_DumpRF1(uint32_t lData, uint8_t pData[]);
uint8_t Ipl_DumpAll(uint32_t lData, uint8_t pData[]);

#endif

#endif
