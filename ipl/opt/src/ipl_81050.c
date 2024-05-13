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

/*! \file   ipl_81050.c
 *  \brief  Internal OS81050 specific functions for INIC Programming Library
 *  \author Roland Trissl (RTR)
 *  \note   For support related to this code contact http://www.microchip.com/support.
 */

#include <stdint.h>
#include "ipl_cfg.h"
#include "ipl.h"
#include "ipf.h"
#include "ipl_81050.h"


#if defined IPL_USE_OS81050 || defined IPL_USE_OS81082

/*------------------------------------------------------------------------------------------------*/
/* FUNCTIONS                                                                                      */
/*------------------------------------------------------------------------------------------------*/

/*! \internal Programs a Firmware. */
uint8_t OS81050_ProgFirmware(uint32_t lData, uint8_t pData[])
{
    uint8_t  res;
    uint32_t loop, adr, startadr, stopadr;

    Ipl_Trace(IPL_TRACETAG_INFO, "OS81050_ProgFirmware called");
    /* Get addresses and sizes from metadata */
    res = Ipl_ParseIpf(&Ipl_IpfData, lData, pData, STRINGTYPE_META);
    if (IPL_RES_OK == res)
    {
        /* Check if IPF contains proper Firmware */
        res = Ipl_ParseIpf(&Ipl_IpfData, lData, pData, STRINGTYPE_FW);
        if (IPL_RES_OK == res)
        {
            /* Check if IPF fits to INIC */
            res = Ipl_CheckChipId();
            if (IPL_RES_OK == res)
            {
                /* Erase INIC FW */
                Ipl_ClrTel();
                Ipl_IplData.Tel[0] = CMD_ERASEPROGMEM;
                Ipl_IplData.Tel[1] = 0x08U;
                Ipl_IplData.Tel[2] = 0x77U;
                Ipl_IplData.TelLen = CMD_ERASEPROGMEM_TXLEN;
                res = Ipl_ExecInicCmd();
                if (IPL_RES_OK == res)
                {
                    /* Select Flash Page 0 */
                    Ipl_ClrTel();
                    Ipl_IplData.Tel[0] = CMD_SETPROGMEMPAGE;
                    Ipl_IplData.Tel[1] = 0x0U;
                    Ipl_IplData.TelLen = CMD_SETPROGMEMPAGE_TXLEN;
                    res = Ipl_ExecInicCmd();
                    if (IPL_RES_OK == res)
                    {
                        /* Clear CRC */
                        Ipl_ClrTel();
                        Ipl_IplData.Tel[0] = CMD_CLEARCRC;
                        Ipl_IplData.TelLen = CMD_CLEARCRC_TXLEN;
                        res = Ipl_ExecInicCmd();
                        if (IPL_RES_OK == res)
                        {
                            /* Write FW Page 0 */
                            startadr = 0x2000U;
                            adr      = startadr;
                            stopadr = 0x10000U;
                            while ( (adr < stopadr) && (IPL_RES_OK == res) )
                            {
                                Ipl_IplData.Tel[0] = CMD_WRITEPROGMEM;
                                Ipl_IplData.Tel[1] = (adr >> 8U) & 0xFFU;
                                Ipl_IplData.Tel[2] = adr & 0xFFU;
                                Ipl_IplData.Tel[3] = 0x20U;
                                for(loop=0U; loop<32U; loop++)
                                {
                                    Ipl_IplData.Tel[4U+loop] = Ipl_PData((loop+Ipl_IpfData.StringOffset+adr)-startadr, lData, pData);
                                }
                                Ipl_IplData.TelLen = 36U;
                                res = Ipl_ExecInicCmd();
                                adr += 0x20U;
                                Ipl_ProgressIndicator(adr-startadr, Ipl_IpfData.StringSize);
                            }
                            if (IPL_RES_OK == res)
                            {
                                /* Select Flash Page 1 */
                                Ipl_ClrTel();
                                Ipl_IplData.Tel[0] = CMD_SETPROGMEMPAGE;
                                Ipl_IplData.Tel[1] = 0x1U;
                                Ipl_IplData.TelLen = CMD_SETPROGMEMPAGE_TXLEN;
                                res = Ipl_ExecInicCmd();
                                if (IPL_RES_OK == res)
                                {
                                    /* Write FW Page 1 */
                                    adr     = 0x10000U;
                                    stopadr = 0x1FBFFU;
                                    while ( (adr < stopadr) && (IPL_RES_OK == res) )
                                    {
                                        Ipl_IplData.Tel[0] = CMD_WRITEPROGMEM;
                                        Ipl_IplData.Tel[1] = (adr >> 8U) & 0xFFU;
                                        Ipl_IplData.Tel[2] = adr & 0xFFU;
                                        Ipl_IplData.Tel[3] = 0x20U;
                                        for(loop=0U; loop<32U; loop++)
                                        {
                                            Ipl_IplData.Tel[4U+loop] = Ipl_PData((loop+Ipl_IpfData.StringOffset+adr)-startadr, lData, pData);
                                        }
                                        Ipl_IplData.TelLen = 36U;
                                        res = Ipl_ExecInicCmd();
                                        adr += 0x20U;
                                        Ipl_ProgressIndicator(adr-startadr, Ipl_IpfData.StringSize);
                                    }
                                    if (IPL_RES_OK == res)
                                    {
                                         /* Get CRC */
                                         Ipl_ClrTel();
                                         Ipl_IplData.Tel[0] = CMD_GETCRC;
                                         Ipl_IplData.Tel[3] = 0x2U;
                                         Ipl_IplData.TelLen = CMD_GETCRC_TXLEN;
                                         res = Ipl_ExecInicCmd();
                                         if (IPL_RES_OK == res)
                                         {
                                             if ( (Ipl_IplData.Tel[4] != 0x00U) || (Ipl_IplData.Tel[5] != 0x00U) )
                                             {
                                                 res = IPL_RES_ERR_WRONG_CRC;
                                             }
                                         }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    Ipl_ProgressIndicator(1U, 1U); /* Set Progress Indicator to 100 */
    Ipl_Trace(Ipl_TraceTag(res), "OS81050_ProgFirmware returned 0x%02X", res);
    return res;
}



/*! \internal Programs a ConfigString. */
uint8_t OS81050_ProgConfigString(uint32_t lData, uint8_t pData[])
{
    uint8_t  res;
    uint32_t loop, page;
    Ipl_Trace(IPL_TRACETAG_INFO, "OS81050_ProgConfigString called");
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
                /* Erase INIC CS */
                Ipl_ClrTel();
                Ipl_IplData.Tel[0] = CMD_LEG_ERASECS;
                Ipl_IplData.Tel[1] = 0x67U;
                Ipl_IplData.TelLen = CMD_LEG_ERASECS_TXLEN;
                res = Ipl_ExecInicCmd();
                if (IPL_RES_OK == res)
                {
                    /* Clear CRC */
                    Ipl_ClrTel();
                    Ipl_IplData.Tel[0] = CMD_CLEARCRC;
                    Ipl_IplData.TelLen = CMD_CLEARCRC_TXLEN;
                    res = Ipl_ExecInicCmd();
                    if (IPL_RES_OK == res)
                    {
                        /* Write CS */
                        for(page=0U; page<=3U; page++)
                        {
                            Ipl_IplData.Tel[0] = CMD_LEG_WRITECS;
                            Ipl_IplData.Tel[1] = 0x00U;
                            Ipl_IplData.Tel[2] = (uint8_t) page*32U;
                            Ipl_IplData.Tel[3] = 0x20U;
                            for(loop=0U; loop<32U; loop++)
                            {
                                Ipl_IplData.Tel[4U+loop] = Ipl_PData(loop+Ipl_IpfData.StringOffset+(uint32_t) (page*32U), lData, pData);
                            }
                            Ipl_IplData.TelLen = 36U;
                            res = Ipl_ExecInicCmd();
                            Ipl_ProgressIndicator((page*32U), Ipl_IpfData.StringSize);
                        }
                        if (IPL_RES_OK == res)
                        {
                            /* Get CRC */
                            Ipl_ClrTel();
                            Ipl_IplData.Tel[0] = CMD_GETCRC;
                            Ipl_IplData.Tel[3] = 0x2U;
                            Ipl_IplData.TelLen = CMD_GETCRC_TXLEN;
                            res = Ipl_ExecInicCmd();
                            if(IPL_RES_OK == res)
                            {
                                if ( (Ipl_IplData.Tel[4] != 0x00U) || (Ipl_IplData.Tel[5] != 0x00U) )
                                {
                                    res = IPL_RES_ERR_WRONG_CRC;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    Ipl_ProgressIndicator(1U, 1U); /* Set Progress Indicator to 100 */
    Ipl_Trace(Ipl_TraceTag(res), "OS81050_ProgConfigString returned 0x%02X", res);
    return res;
}


/*------------------------------------------------------------------------------------------------*/
/* UNSUPPORTED FUNCTIONS                                                                          */
/*------------------------------------------------------------------------------------------------*/

uint8_t OS81050_ReadConfigStringVersion(uint32_t lData, uint8_t pData[])
{
    Ipl_Trace(IPL_TRACETAG_ERR, "OS81050_ReadConfigStringVersion returned 0x%02X", IPL_RES_ERR_NOT_SUPPORTED);
    return IPL_RES_ERR_NOT_SUPPORTED;
}


uint8_t OS81050_ProgConfiguration(uint32_t lData, uint8_t pData[])
{
    Ipl_Trace(IPL_TRACETAG_ERR, "OS81050_ProgConfiguration returned 0x%02X", IPL_RES_ERR_NOT_SUPPORTED);
    return IPL_RES_ERR_NOT_SUPPORTED;
}


uint8_t OS81050_ProgIdentString(uint32_t lData, uint8_t pData[])
{
    Ipl_Trace(IPL_TRACETAG_ERR, "OS81050_ProgIdentString returned 0x%02X", IPL_RES_ERR_NOT_SUPPORTED);
    return IPL_RES_ERR_NOT_SUPPORTED;
}


uint8_t OS81050_ProgPatchString(uint32_t lData, uint8_t pData[])
{
    Ipl_Trace(IPL_TRACETAG_ERR, "OS81050_ProgPatchString returned 0x%02X", IPL_RES_ERR_NOT_SUPPORTED);
    return IPL_RES_ERR_NOT_SUPPORTED;
}


uint8_t OS81050_ProgTestConfiguration(uint32_t lData, uint8_t pData[])
{
    Ipl_Trace(IPL_TRACETAG_ERR, "OS81050_ProgTestConfiguration returned 0x%02X", IPL_RES_ERR_NOT_SUPPORTED);
    return IPL_RES_ERR_NOT_SUPPORTED;
}


uint8_t OS81050_ProgTestPatchString(uint32_t lData, uint8_t pData[])
{
    Ipl_Trace(IPL_TRACETAG_ERR, "OS81050_ProgTestPatchString returned 0x%02X", IPL_RES_ERR_NOT_SUPPORTED);
    return IPL_RES_ERR_NOT_SUPPORTED;
}


uint8_t OS81050_ProgTestConfigString(uint32_t lData, uint8_t pData[])
{
    Ipl_Trace(IPL_TRACETAG_ERR, "OS81050_ProgTestConfigString returned 0x%02X", IPL_RES_ERR_NOT_SUPPORTED);
    return IPL_RES_ERR_NOT_SUPPORTED;
}


uint8_t OS81050_ProgTestIdentString(uint32_t lData, uint8_t pData[])
{
    Ipl_Trace(IPL_TRACETAG_ERR, "OS81050_ProgTestIdentString returned 0x%02X", IPL_RES_ERR_NOT_SUPPORTED);
    return IPL_RES_ERR_NOT_SUPPORTED;
}


#endif
