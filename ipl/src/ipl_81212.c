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

/*! \file   ipl_81212.c
 *  \brief  Internal OS81212 specific functions for INIC Programming Library
 *  \author Roland Trissl (RTR)
 *  \note   For support related to this code contact http://www.microchip.com/support.
 */

#include <stdint.h>
#include "ipl_cfg.h"
#include "ipl.h"
#include "ipf.h"
#include "ipl_81210.h"
#include "ipl_81212.h"


#ifdef IPL_USE_OS81212

/*------------------------------------------------------------------------------------------------*/
/* FUNCTIONS                                                                                      */
/*------------------------------------------------------------------------------------------------*/

/*! \internal Reads the ConfigString version. IPF data is needed. (DUPUG 4.5.6) */
uint8_t OS81212_ReadConfigStringVersion(uint32_t lData, uint8_t pData[])
{
    return OS81210_ReadConfigStringVersion(lData, pData);
}


/*! \internal Programs a Configuration (CS+IS) to OTP. */
uint8_t OS81212_ProgConfiguration(uint32_t lData, uint8_t pData[])
{
    return OS81210_ProgConfiguration(lData, pData);
}


/*! \internal Programs a ConfigString to OTP. */
uint8_t OS81212_ProgConfigString(uint32_t lData, uint8_t pData[])
{
    return OS81210_ProgConfigString(lData, pData);
}


/*! \internal Programs an IdentString to OTP. */
uint8_t OS81212_ProgIdentString(uint32_t lData, uint8_t pData[])
{
    return OS81210_ProgIdentString(lData, pData);
}


/*! \internal Programs a patch string. (DUPUG 4.5.3) */
uint8_t OS81212_ProgPatchString(uint32_t lData, uint8_t pData[])
{
    return OS81210_ProgPatchString(lData, pData);
}


/*! \internal Programs a Configuration to the test memory. */
uint8_t OS81212_ProgTestConfiguration(uint32_t lData, uint8_t pData[])
{
    return OS81210_ProgTestConfiguration(lData, pData);
}


/*! \internal Programs a PatchString to the test memory. */
uint8_t OS81212_ProgTestPatchString(uint32_t lData, uint8_t pData[])
{
    return OS81210_ProgTestPatchString(lData, pData);
}


/*! \internal Programs a ConfigString to the test memory. */
uint8_t OS81212_ProgTestConfigString(uint32_t lData, uint8_t pData[])
{
    return OS81210_ProgTestConfigString(lData, pData);
}


/*! \internal Programs an IdentString to the test memory. */
uint8_t OS81212_ProgTestIdentString(uint32_t lData, uint8_t pData[])
{
    return OS81210_ProgTestIdentString(lData, pData);
}


/*------------------------------------------------------------------------------------------------*/
/* UNSUPPORTED FUNCTIONS                                                                          */
/*------------------------------------------------------------------------------------------------*/

uint8_t OS81212_ProgFirmware(uint32_t lData, uint8_t pData[])
{
    Ipl_Trace(IPL_TRACETAG_ERR, "OS81212_ProgFirmware returned 0x%02X", IPL_RES_ERR_NOT_SUPPORTED);
    return IPL_RES_ERR_NOT_SUPPORTED;
}

#endif
