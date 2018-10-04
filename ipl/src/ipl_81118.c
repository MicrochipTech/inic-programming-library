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

/*! \file ipl_81118.c
 *  \brief Internal OS81118 specific functions for INIC Programming Library
 *  \author Roland Trissl (RTR)
 */

#include <stdint.h>
#include "ipl_cfg.h"
#include "ipl.h"
#include "ipf.h"
#include "ipl_81118.h"


/* INICnet150 product family */
#if defined IPL_USE_OS81118 || defined IPL_USE_OS81119

/*------------------------------------------------------------------------------------------------*/
/* FUNCTION PROTOTYPES                                                                            */
/*------------------------------------------------------------------------------------------------*/

static uint8_t OS81118_GetActiveConfigPage(void);
static uint8_t OS81118_ProgCSIS(uint32_t lData, uint8_t pData[]);
static uint8_t OS81118_ProgConf(uint32_t lData, uint8_t pData[]);
static uint8_t OS81118_ProgInfoMem(uint32_t addr, uint32_t nOfBytes, uint8_t pData[]);


/*------------------------------------------------------------------------------------------------*/
/* FUNCTIONS                                                                                      */
/*------------------------------------------------------------------------------------------------*/

/*! \internal Reads the ConfigString version. IPF data is needed. (DUPUG 4.4.3) */
uint8_t OS81118_ReadConfigStringVersion(uint32_t lData, uint8_t pData[])
{
    uint8_t  res;
    uint32_t addr;
    Ipl_Trace(IPL_TRACETAG_INFO, "OS81118_ReadConfigStringVersion called");
    Ipl_InicData.CfgsVersionValid = VERSION_INVALID;
    /* Get addresses and sizes from metadata */
    res = Ipl_ParseIpf(&Ipl_IpfData, lData, pData, STRINGTYPE_META);
    if (IPL_RES_OK == res)
    {
        /* Check if IPF fits to INIC */
        res = Ipl_CheckChipId();
        if (IPL_RES_OK == res)
        {
            addr = ( (uint32_t) Ipl_IpfData.Meta.ChipInfoMemSectionSize * (uint32_t) Ipl_InicData.CfgsActiveConfigPage)
            + Ipl_IpfData.Meta.CfgsStdStartAddr + 2U;
            res = OS81118_GetActiveConfigPage();
            if (IPL_RES_OK == res)
            {
                Ipl_ClrTel();
                Ipl_IplData.Tel[0] = CMD_READINFOMEM;
                Ipl_IplData.Tel[1] = (addr >> 8U) & 0xFFU;
                Ipl_IplData.Tel[2] = addr & 0xFFU;
                Ipl_IplData.Tel[3] = 3U;
                Ipl_IplData.TelLen = CMD_READINFOMEM_TXLEN;
                res = Ipl_ExecInicCmd();
                if (IPL_RES_OK == res)
                {
                    Ipl_InicData.CfgsVersionValid       = VERSION_VALID;
                    Ipl_InicData.CfgsCustMajorVersion   = Ipl_IplData.Tel[4];
                    Ipl_InicData.CfgsCustMinorVersion   = Ipl_IplData.Tel[5];
                    Ipl_InicData.CfgsCustReleaseVersion = Ipl_IplData.Tel[6];
                }
            }
        }
    }
    if (IPL_RES_OK == res)
    {
        Ipl_Trace(Ipl_TraceTag(res), "OS81118_ReadConfigStringVersion returned 0x%02X - V%u.%u.%u", res,
                  Ipl_InicData.CfgsCustMajorVersion, Ipl_InicData.CfgsCustMinorVersion, Ipl_InicData.CfgsCustReleaseVersion);
    }
    else
    {
        Ipl_Trace(Ipl_TraceTag(res), "OS81118_ReadConfigStringVersion returned 0x%02X", res);
    }
    return res;
}


/*! \internal Programs a Firmware. (DUPUG 4.4.1) */
uint8_t OS81118_ProgFirmware(uint32_t lData, uint8_t pData[])
{
    uint8_t  res, i;
    uint32_t addr, len, nOfBytes;
    uint16_t crc;
    uint32_t data = 0U;
    Ipl_Trace(IPL_TRACETAG_INFO, "OS81118_ProgFirmware called");
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
                /* Erase Program Memory */
                Ipl_ClrTel();
                Ipl_IplData.Tel[0] = CMD_ERASEPROGMEM;
                Ipl_IplData.Tel[1] = (Ipl_IpfData.Meta.BmSize / Ipl_IpfData.Meta.ChipPrgMemSectionSize) & 0xFFU;
                Ipl_IplData.Tel[2] = (Ipl_IpfData.StringSize / Ipl_IpfData.Meta.ChipPrgMemSectionSize) & 0xFFU;
                Ipl_IplData.TelLen = CMD_ERASEPROGMEM_TXLEN;
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
                        /* Set Program Memory Page */
                        Ipl_ClrTel();
                        addr     = Ipl_IpfData.ProgAddr;
                        nOfBytes = Ipl_IpfData.StringSize;
                        Ipl_IplData.Tel[0] = CMD_SETPROGMEMPAGE;
                        Ipl_IplData.Tel[1] = (addr / Ipl_IpfData.Meta.ChipPrgMemPageSize) & 0xFFU;
                        Ipl_IplData.TelLen = CMD_SETPROGMEMPAGE_TXLEN;
                        res = Ipl_ExecInicCmd();
                        if (IPL_RES_OK == res)
                        {
                            do
                            {
                                Ipl_ProgressIndicator(Ipl_IpfData.StringSize-nOfBytes, Ipl_IpfData.StringSize);
                                if (nOfBytes >=  Ipl_IpfData.Meta.BmMaxDataLength)
                                {
                                    len       = Ipl_IpfData.Meta.BmMaxDataLength;
                                    nOfBytes -= Ipl_IpfData.Meta.BmMaxDataLength;
                                }
                                else
                                {
                                    len      = nOfBytes;
                                    nOfBytes = 0U;
                                }
                                /* Write Program Memory */
                                Ipl_ClrTel();
                                Ipl_IplData.Tel[0] = CMD_WRITEPROGMEM;
                                Ipl_IplData.Tel[1] = (addr >> 8) & 0xFFU;
                                Ipl_IplData.Tel[2] = addr & 0xFFU;
                                Ipl_IplData.Tel[3] = len & 0xFFU;
                                for (i=0U; i<len; i++)
                                {
                                    Ipl_IplData.Tel[4U+i] = Ipl_PData(i+Ipl_IpfData.StringOffset+data, lData, pData);
                                }
                                Ipl_IplData.TelLen = len + 4U;
                                res = Ipl_ExecInicCmd();
                                if (0U == nOfBytes)
                                {
                                    break;
                                }
                                addr += Ipl_IpfData.Meta.BmMaxDataLength;
                                data += Ipl_IpfData.Meta.BmMaxDataLength;
                                if ((0U == (addr % Ipl_IpfData.Meta.ChipPrgMemPageSize)) && (IPL_RES_OK == res))
                                {
                                    /* Set Program Memory Page */
                                    Ipl_ClrTel();
                                    Ipl_IplData.Tel[0] = CMD_SETPROGMEMPAGE;
                                    Ipl_IplData.Tel[1] = (addr / Ipl_IpfData.Meta.ChipPrgMemPageSize) & 0xFFU;
                                    Ipl_IplData.TelLen = CMD_SETPROGMEMPAGE_TXLEN;
                                    res = Ipl_ExecInicCmd();
                                }
                            } while ((nOfBytes != 0U) && (IPL_RES_OK == res));
                            if (IPL_RES_OK == res)
                            {
                                /* Get CRC */
                                Ipl_ClrTel();
                                Ipl_IplData.Tel[0] = CMD_GETCRC;
                                Ipl_IplData.Tel[3] = 0x02U;
                                Ipl_IplData.TelLen = CMD_GETCRC_TXLEN;
                                res = Ipl_ExecInicCmd();
                                if (IPL_RES_OK == res)
                                {
                                    crc =  (uint16_t) (((uint16_t) Ipl_IplData.Tel[4]) << 8U);
                                    crc += ((uint16_t) Ipl_IplData.Tel[5]) & 0x00FFU;
                                    if (crc != 0U)
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
    Ipl_ProgressIndicator(1U, 1U); /* Set Progress Indicator to 100 */
    Ipl_Trace(Ipl_TraceTag(res), "OS81118_ProgFirmware returned 0x%02X", res);
    return res;
}


/*! \internal Programs a Configuration (Config or CS+IS). (DUPUG 4.4.4) */
uint8_t OS81118_ProgConfiguration(uint32_t lData, uint8_t pData[])
{
    uint8_t res;
    Ipl_Trace(IPL_TRACETAG_INFO, "OS81118_ProgConfiguration called");
    /* Get addresses and sizes from metadata */
    res = Ipl_ParseIpf(&Ipl_IpfData, lData, pData, STRINGTYPE_META);
    if (IPL_RES_OK == res)
    {
        /* Check what format should be used */
        res = Ipl_ParseIpf(&Ipl_IpfData, lData, pData, STRINGTYPE_CONFIG);
        if (IPL_RES_OK == res)
        {
            /* Flashing Conf should only be allowed if Fw version fits */
            res = Ipl_CheckInicFwVersion();
            if (IPL_RES_OK == res)
            {
                res = OS81118_ProgConf(lData, pData);
            }
        }
        else
        {
            res = Ipl_ParseIpf(&Ipl_IpfData, lData, pData, STRINGTYPE_CS);
            if (IPL_RES_OK == res)
            {
                res = Ipl_ParseIpf(&Ipl_IpfData, lData, pData, STRINGTYPE_IS);
                if (IPL_RES_OK == res)
                {
                    /* Flashing Conf should only be allowed if Fw version fits */
                    res = Ipl_CheckInicFwVersion();
                    if (IPL_RES_OK == res)
                    {
                        res = OS81118_ProgCSIS(lData, pData);
                    }
                }
            }
        }
    }
    Ipl_Trace(Ipl_TraceTag(res), "OS81118_ProgConfiguration returned 0x%02X", res);
    return res;
}


/*! \internal Returns the Active Configuration Page which is used. (DUPUG 4.4.2) */
static uint8_t OS81118_GetActiveConfigPage(void)
{
    uint8_t res;
    Ipl_Trace(IPL_TRACETAG_INFO, "OS81118_GetActiveConfigPage called");
    Ipl_ClrTel();
    Ipl_IplData.Tel[0] = CMD_VERIFYINFOMEM;
    Ipl_IplData.Tel[1] = (((uint16_t) Ipl_IpfData.Meta.CfgsStdStartAddr) >> 8U) & 0xFFU;
    Ipl_IplData.Tel[2] = Ipl_IpfData.Meta.CfgsStdStartAddr & 0xFFU;
    Ipl_IplData.Tel[3] = (uint8_t) Ipl_IpfData.Meta.CfgsSize;
    Ipl_IplData.TelLen = CMD_VERIFYINFOMEM_TXLEN;
    res = Ipl_ExecInicCmd();
    if (IPL_RES_OK != res)
    {
        Ipl_InicData.CfgsActiveConfigPage = 0xFFU;
    }
    else
    {
        Ipl_ClrTel();
        Ipl_IplData.Tel[0] = CMD_VERIFYINFOMEM;
        Ipl_IplData.Tel[1] = (Ipl_IpfData.Meta.IdentsStdStartAddr >> 8U) & 0xFFU;
        Ipl_IplData.Tel[2] = Ipl_IpfData.Meta.IdentsStdStartAddr & 0xFFU;
        Ipl_IplData.Tel[3] = (uint8_t) Ipl_IpfData.Meta.IdentsSize;
        Ipl_IplData.TelLen = CMD_VERIFYINFOMEM_TXLEN;
        res = Ipl_ExecInicCmd();
        if (IPL_RES_OK != res)
        {
            Ipl_InicData.CfgsActiveConfigPage = 0xFFU;
        }
        else
        {
            Ipl_InicData.CfgsActiveConfigPage = 0U;
        }
    }
    Ipl_Trace(Ipl_TraceTag(res), "OS81118_GetActiveConfigPage returned 0x%02X", res);
    return res;
}


/*! \internal Programs a Configuration (Config). */
static uint8_t OS81118_ProgConf(uint32_t lData, uint8_t pData[])
{
    uint8_t  res;
    uint32_t addr;
    Ipl_Trace(IPL_TRACETAG_INFO, "OS81118_ProgConf called");
    /* Erase Info Memory */
    Ipl_ClrTel();
    Ipl_IplData.Tel[0] = CMD_ERASEINFOMEM;
    Ipl_IplData.Tel[2] = Ipl_IpfData.Meta.ChipNumOfInfoMemSections;
    Ipl_IplData.TelLen = CMD_ERASEINFOMEM_TXLEN;
    res = Ipl_ExecInicCmd();
    if (IPL_RES_OK == res)
    {
        /* Program Info Memory */
        Ipl_IpfData.ProgAddr = Ipl_IpfData.Meta.CfgsStdStartAddr; /*! \internal Jira UN-368 */
        res = OS81118_ProgInfoMem((uint32_t) Ipl_IpfData.ProgAddr, Ipl_IpfData.StringSize, pData);
        if (IPL_RES_OK == res)
        {
            /* Verify CS Info Memory */
            Ipl_ClrTel();
            addr = Ipl_IpfData.Meta.CfgsStdStartAddr;
            Ipl_IplData.Tel[0] = CMD_VERIFYINFOMEM;
            Ipl_IplData.Tel[1] = (addr >> 8U) & 0xFFU;
            Ipl_IplData.Tel[2] = addr & 0xFFU;
            Ipl_IplData.Tel[3] = (uint8_t) Ipl_IpfData.Meta.CfgsSize;
            Ipl_IplData.TelLen = CMD_VERIFYINFOMEM_TXLEN;
            res = Ipl_ExecInicCmd();
            if (IPL_RES_OK == res)
            {
                Ipl_ClrTel();
                addr = Ipl_IpfData.Meta.IdentsStdStartAddr;
                Ipl_IplData.Tel[0] = CMD_VERIFYINFOMEM;
                Ipl_IplData.Tel[1] = (addr >> 8U) & 0xFFU;
                Ipl_IplData.Tel[2] = addr & 0xFFU;
                Ipl_IplData.Tel[3] = (uint8_t) Ipl_IpfData.Meta.IdentsSize;
                Ipl_IplData.TelLen = CMD_VERIFYINFOMEM_TXLEN;
                res = Ipl_ExecInicCmd();
                if (IPL_RES_OK != res)
                {
                    res = IPL_RES_ERR_IS_INVALID;
                }
            }
            else
            {
                res = IPL_RES_ERR_CS_INVALID;
            }
        }
    }
    Ipl_Trace(Ipl_TraceTag(res), "OS81118_ProgConf returned 0x%02X", res);
    return res;
}


/*! \internal Programs a Configuration (CS+IS). */
static uint8_t OS81118_ProgCSIS(uint32_t lData, uint8_t pData[])
{
    uint8_t  res;
    uint32_t addr;
    Ipl_Trace(IPL_TRACETAG_INFO, "OS81118_ProgCSIS called");
    /* Erase Info Memory */
    Ipl_ClrTel();
    Ipl_IplData.Tel[0] = CMD_ERASEINFOMEM;
    Ipl_IplData.Tel[2] = Ipl_IpfData.Meta.ChipNumOfInfoMemSections;
    Ipl_IplData.TelLen = CMD_ERASEINFOMEM_TXLEN;
    res = Ipl_ExecInicCmd();
    if (IPL_RES_OK == res)
    {
        /* Program CS Info Memory */
        res = Ipl_ParseIpf(&Ipl_IpfData, lData, pData, STRINGTYPE_CS);
        if (IPL_RES_OK == res)
        {
            Ipl_IpfData.ProgAddr = Ipl_IpfData.Meta.CfgsStdStartAddr;
            res = OS81118_ProgInfoMem((uint32_t) Ipl_IpfData.ProgAddr, Ipl_IpfData.StringSize, pData);
            if (IPL_RES_OK == res)
            {
                /* Program IS Info Memory */
                res = Ipl_ParseIpf(&Ipl_IpfData, lData, pData, STRINGTYPE_IS);
                if (IPL_RES_OK == res)
                {
                    Ipl_IpfData.ProgAddr = Ipl_IpfData.Meta.IdentsStdStartAddr;
                    res = OS81118_ProgInfoMem((uint32_t) Ipl_IpfData.ProgAddr, Ipl_IpfData.StringSize, pData);
                    if (IPL_RES_OK == res)
                    {
                        /* Verify CS Info Memory */
                        Ipl_ClrTel();
                        addr = Ipl_IpfData.Meta.CfgsStdStartAddr;
                        Ipl_IplData.Tel[0] = CMD_VERIFYINFOMEM;
                        Ipl_IplData.Tel[1] = (addr >> 8U) & 0xFFU;
                        Ipl_IplData.Tel[2] = addr & 0xFFU;
                        Ipl_IplData.Tel[3] = (uint8_t) Ipl_IpfData.Meta.CfgsSize;
                        Ipl_IplData.TelLen = CMD_VERIFYINFOMEM_TXLEN;
                        res = Ipl_ExecInicCmd();
                        if (IPL_RES_OK == res)
                        {
                            Ipl_ClrTel();
                            addr = Ipl_IpfData.Meta.IdentsStdStartAddr;
                            Ipl_IplData.Tel[0] = CMD_VERIFYINFOMEM;
                            Ipl_IplData.Tel[1] = (addr >> 8U) & 0xFFU;
                            Ipl_IplData.Tel[2] = addr & 0xFFU;
                            Ipl_IplData.Tel[3] = (uint8_t) Ipl_IpfData.Meta.IdentsSize;
                            Ipl_IplData.TelLen = CMD_VERIFYINFOMEM_TXLEN;
                            res = Ipl_ExecInicCmd();
                            if (IPL_RES_OK != res)
                            {
                                res = IPL_RES_ERR_IS_INVALID;
                            }
                        }
                        else
                        {
                            res = IPL_RES_ERR_CS_INVALID;
                        }
                    }
                    else
                    {
                        res = IPL_RES_ERR_PROG_INFOMEM;
                    }
                }
            }
            else
            {
                res = IPL_RES_ERR_PROG_INFOMEM;
            }
        }
    }
    Ipl_Trace(Ipl_TraceTag(res), "OS81118_ProgCSIS returned 0x%02X", res);
    return res;
}


/*! \internal Programs Info Memory. (DUPUG 4.4.5) */
static uint8_t OS81118_ProgInfoMem(uint32_t addr, uint32_t nOfBytes, uint8_t pData[])
{
    uint8_t  res, i;
    uint32_t len;
    uint32_t data = 0U;
    uint32_t size = nOfBytes;
    Ipl_Trace(IPL_TRACETAG_INFO, "OS81118_ProgInfoMem called with Addr 0x%04X, NofBytes %u", addr, nOfBytes);
    do
    {
        Ipl_ProgressIndicator(size-nOfBytes, size); /* Update Progress Indicator */
        if (nOfBytes >= Ipl_IpfData.Meta.BmMaxDataLength)
        {
            len       = Ipl_IpfData.Meta.BmMaxDataLength;
            nOfBytes -= Ipl_IpfData.Meta.BmMaxDataLength;
        }
        else
        {
            len      = nOfBytes;
            nOfBytes = 0U;
        }
        /* Write Info Memory */
        Ipl_ClrTel();
        Ipl_IplData.Tel[0] = CMD_WRITEINFOMEM;
        Ipl_IplData.Tel[1] = (addr >> 8) & 0xFFU;
        Ipl_IplData.Tel[2] = addr & 0xFFU;
        Ipl_IplData.Tel[3] = len & 0xFFU;
        for (i=0U; i<len; i++)
        {
            Ipl_IplData.Tel[4U+i] = Ipl_PData(i+data+Ipl_IpfData.StringOffset, size, pData);
        }
        Ipl_IplData.TelLen = len + 4U;
        res = Ipl_ExecInicCmd();
        if (0U == nOfBytes)
        {
            break;
        }
        addr += Ipl_IpfData.Meta.BmMaxDataLength;
        data += Ipl_IpfData.Meta.BmMaxDataLength;
    } while ((nOfBytes != 0U) && (IPL_RES_OK == res));
    Ipl_ProgressIndicator(1U, 1U); /* Set Progress Indicator to 100 */
    Ipl_Trace(Ipl_TraceTag(res), "OS81118_ProgInfoMem returned 0x%02X", res);
    return res;
}

#endif
