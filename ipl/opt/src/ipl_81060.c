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

/*! \file   ipl_81060.c
 *  \brief  Internal OS81060 specific functions for INIC Programming Library
 *  \author Roland Trissl (RTR)
 *  \note   For support related to this code contact http://www.microchip.com/support.
 */

#include <stdint.h>
#include "ipl_cfg.h"
#include "ipl.h"
#include "ipf.h"
#include "ipl_81060.h"


#if defined IPL_USE_OS81060 || defined IPL_USE_OS81092

/*------------------------------------------------------------------------------------------------*/
/* FUNCTIONS                                                                                      */
/*------------------------------------------------------------------------------------------------*/

/*! \internal Programs a ConfigString. */
uint8_t OS81060_ProgConfigString(uint32_t lData, uint8_t pData[])
{
    uint8_t  res;
    uint32_t loop;
    Ipl_Trace(IPL_TRACETAG_INFO, "OS81060_ProgConfigString called");
    /* Get addresses and sizes from metadata */
    res = Ipl_ParseIpf(&Ipl_IpfData, lData, pData, STRINGTYPE_META);
    if (IPL_RES_OK == res)
    {
        /* Check if IPF contains proper ConfigString */
        res = Ipl_ParseIpf(&Ipl_IpfData, lData, pData, STRINGTYPE_CONFIG);
        if (IPL_RES_OK == res)
        {
            /* Check if IPF fits to INIC */
            res = Ipl_CheckChipId();
            if (IPL_RES_OK == res)
            {
                /* Write CS */
                Ipl_IplData.Tel[0] = CMD_LEG_WRITECS;
                Ipl_IplData.Tel[1] = 0x00U;
                Ipl_IplData.Tel[2] = 0x00U;
                Ipl_IplData.Tel[3] = 0x20U;
                for(loop=0U; loop<32U; loop++)
                {
                    Ipl_IplData.Tel[4U+loop] = Ipl_PData(loop+Ipl_IpfData.StringOffset, lData, pData);
                }
                Ipl_IplData.TelLen = 36U;
                res = Ipl_ExecInicCmd();
            }
        }
    }
    Ipl_Trace(Ipl_TraceTag(res), "OS81060_ProgConfigString returned 0x%02X", res);
    return res;
}


/*! \internal Reads the ConfigString version. */
uint8_t OS81060_ReadConfigStringVersion(uint32_t lData, uint8_t pData[])
{
    uint8_t res;
    Ipl_Trace(IPL_TRACETAG_INFO, "OS81060_ReadConfigStringVersion called");
    /* Get CS Version */
    Ipl_ClrTel();
    Ipl_IplData.Tel[0] = CMD_LEG_GETCSINFO;
    Ipl_IplData.Tel[3] = 0x20U;
    Ipl_IplData.TelLen = CMD_LEG_GETCSINFO_TXLEN;
    res = Ipl_ExecInicCmd();
    if (IPL_RES_OK == res)
    {
        Ipl_InicData.CfgsVersionValid       = VERSION_VALID;
        Ipl_InicData.CfgsCustMajorVersion   = Ipl_IplData.Tel[6];
        Ipl_InicData.CfgsCustMinorVersion   = Ipl_IplData.Tel[7];
        Ipl_InicData.CfgsCustReleaseVersion = Ipl_IplData.Tel[8];
        Ipl_Trace(Ipl_TraceTag(res), "OS81060_ReadConfigStringVersion returned 0x%02X - V%u.%u.%u", res,
                  Ipl_InicData.CfgsCustMajorVersion, Ipl_InicData.CfgsCustMinorVersion, Ipl_InicData.CfgsCustReleaseVersion);
    }
    else
    {
        Ipl_Trace(Ipl_TraceTag(res), "OS81060_ReadConfigStringVersion returned 0x%02X", res);
    }
    return res;
}


/*------------------------------------------------------------------------------------------------*/
/* UNSUPPORTED FUNCTIONS                                                                          */
/*------------------------------------------------------------------------------------------------*/

uint8_t OS81060_ProgFirmware(uint32_t lData, uint8_t pData[])
{
    Ipl_Trace(IPL_TRACETAG_ERR, "OS81060_ProgFirmware returned 0x%02X", IPL_RES_ERR_NOT_SUPPORTED);
    return IPL_RES_ERR_NOT_SUPPORTED;
}


uint8_t OS81060_ProgConfiguration(uint32_t lData, uint8_t pData[])
{
    Ipl_Trace(IPL_TRACETAG_ERR, "OS81060_ProgConfiguration returned 0x%02X", IPL_RES_ERR_NOT_SUPPORTED);
    return IPL_RES_ERR_NOT_SUPPORTED;
}


uint8_t OS81060_ProgIdentString(uint32_t lData, uint8_t pData[])
{
    Ipl_Trace(IPL_TRACETAG_ERR, "OS81060_ProgIdentString returned 0x%02X", IPL_RES_ERR_NOT_SUPPORTED);
    return IPL_RES_ERR_NOT_SUPPORTED;
}


uint8_t OS81060_ProgPatchString(uint32_t lData, uint8_t pData[])
{
    Ipl_Trace(IPL_TRACETAG_ERR, "OS81060_ProgPatchString returned 0x%02X", IPL_RES_ERR_NOT_SUPPORTED);
    return IPL_RES_ERR_NOT_SUPPORTED;
}


uint8_t OS81060_ProgTestConfiguration(uint32_t lData, uint8_t pData[])
{
    Ipl_Trace(IPL_TRACETAG_ERR, "OS81060_ProgTestConfiguration returned 0x%02X", IPL_RES_ERR_NOT_SUPPORTED);
    return IPL_RES_ERR_NOT_SUPPORTED;
}


uint8_t OS81060_ProgTestPatchString(uint32_t lData, uint8_t pData[])
{
    Ipl_Trace(IPL_TRACETAG_ERR, "OS81060_ProgTestPatchString returned 0x%02X", IPL_RES_ERR_NOT_SUPPORTED);
    return IPL_RES_ERR_NOT_SUPPORTED;
}


uint8_t OS81060_ProgTestConfigString(uint32_t lData, uint8_t pData[])
{
    Ipl_Trace(IPL_TRACETAG_ERR, "OS81060_ProgTestConfigString returned 0x%02X", IPL_RES_ERR_NOT_SUPPORTED);
    return IPL_RES_ERR_NOT_SUPPORTED;
}


uint8_t OS81060_ProgTestIdentString(uint32_t lData, uint8_t pData[])
{
    Ipl_Trace(IPL_TRACETAG_ERR, "OS81060_ProgTestIdentString returned 0x%02X", IPL_RES_ERR_NOT_SUPPORTED);
    return IPL_RES_ERR_NOT_SUPPORTED;
}


#endif
