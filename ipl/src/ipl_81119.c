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

/*! \file ipl_81119.c
 *  \brief Internal OS81119 specific functions for INIC Programming Library
 *  \author Roland Trissl (RTR)
 */

#include <stdint.h>
#include "ipl_cfg.h"
#include "ipl.h"
#include "ipf.h"
#include "ipl_81118.h"
#include "ipl_81119.h"


#ifdef IPL_USE_OS81119

/*------------------------------------------------------------------------------------------------*/
/* FUNCTIONS                                                                                      */
/*------------------------------------------------------------------------------------------------*/

/*! \internal Reads the ConfigString version. IPF data is needed. (DUPUG 4.4.3) */
uint8_t OS81119_ReadConfigStringVersion(uint32_t lData, uint8_t pData[])
{
    return OS81118_ReadConfigStringVersion(lData, pData);
}


/*! \internal Programs a Firmware. (DUPUG 4.4.1) */
uint8_t OS81119_ProgFirmware(uint32_t lData, uint8_t pData[])
{
    return OS81118_ProgFirmware(lData, pData);
}


/*! \internal Programs a Configuration (Config or CS+IS). (DUPUG 4.4.4) */
uint8_t OS81119_ProgConfiguration(uint32_t lData, uint8_t pData[])
{
    return OS81118_ProgConfiguration(lData, pData);
}

#endif
