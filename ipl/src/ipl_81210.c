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

/*! \file   ipl_81210.c
 *  \brief  Internal OS81210 specific functions for INIC Programming Library
 *  \author Roland Trissl (RTR)
 *  \note   For support related to this code contact http://www.microchip.com/support.
 */

#include <stdint.h>
#include <stddef.h>
#include "ipl_cfg.h"
#include "ipl.h"
#include "ipf.h"
#include "ipl_81210.h"


/* INICnet 50 product family */
#if defined IPL_USE_OS81210 || defined IPL_USE_OS81212 || defined IPL_USE_OS81214|| defined IPL_USE_OS81216

/*------------------------------------------------------------------------------------------------*/
/* FUNCTION PROTOTYPES                                                                            */
/*------------------------------------------------------------------------------------------------*/

static uint8_t OS81210_ClearTestMem(void);
static uint8_t OS81210_ProgOTPMem(uint32_t addr, uint32_t nOfBytes, uint8_t pData[]);
static uint8_t OS81210_VerifyPatchString(uint32_t addr, uint32_t nOfBytes, uint8_t pData[]);
static uint8_t OS81210_ProgROMConfiguration(uint32_t stdAddr, uint32_t ovrlAddr, uint32_t cfgSize, uint8_t cfgData[]);
static uint8_t OS81210_ProgTestMem(uint32_t addr, uint32_t nOfBytes, uint8_t pData[], uint8_t clearData);


/*------------------------------------------------------------------------------------------------*/
/* FUNCTIONS                                                                                      */
/*------------------------------------------------------------------------------------------------*/

/*! \internal Reads the ConfigString version. IPF data is needed. (DUPUG 4.5.6) */
uint8_t OS81210_ReadConfigStringVersion(uint32_t lData, uint8_t pData[])
{
    uint8_t  res;
    uint32_t addr;
    Ipl_Trace(IPL_TRACETAG_INFO, "OS81210_ReadConfigStringVersion called");
    Ipl_InicData.CfgsVersionValid = VERSION_INVALID;
    /* Get addresses and sizes from metadata */
    res = Ipl_ParseIpf(&Ipl_IpfData, lData, pData, STRINGTYPE_META);
    if (IPL_RES_OK == res)
    {
        /* Check if IPF fits to INIC */
        res = Ipl_CheckChipId();
        if (IPL_RES_OK == res)
        {
            addr = Ipl_IpfData.Meta.CfgsOvrlStartAddr; /*! \internal Jira UN-577 */
            Ipl_ClrTel();
            Ipl_IplData.Tel[0] = CMD_VERIFYOTPMEM;
            Ipl_IplData.Tel[1] = (addr >> 8U) & 0xFFU; /*! \internal Jira UN-577 */
            Ipl_IplData.Tel[2] = addr & 0xFFU;         /*! \internal Jira UN-577 */
            Ipl_IplData.Tel[3] = Ipl_IpfData.Meta.CfgsSize & 0xFFU; /* 34U */
            Ipl_IplData.TelLen = 4U;
            res = Ipl_ExecInicCmd();
            if (IPL_RES_OK == res)
            {
                Ipl_Trace(IPL_TRACETAG_INFO, "OS81210_ReadConfigStringVersion (R)");
                addr = Ipl_IpfData.Meta.CfgsOvrlStartAddr + 1U;
                Ipl_ClrTel();
                Ipl_IplData.Tel[0] = CMD_READOTPMEM;
                Ipl_IplData.Tel[1] = (addr >> 8U) & 0xFFU;
                Ipl_IplData.Tel[2] = addr & 0xFFU;
                Ipl_IplData.Tel[3] = 3U;
                Ipl_IplData.TelLen = CMD_READOTPMEM_TXLEN;
                res = Ipl_ExecInicCmd();
                if (IPL_RES_OK == res)
                {
                    Ipl_InicData.CfgsVersionValid       = VERSION_VALID;
                    Ipl_InicData.CfgsCustMajorVersion   = Ipl_IplData.Tel[4];
                    Ipl_InicData.CfgsCustMinorVersion   = Ipl_IplData.Tel[5];
                    Ipl_InicData.CfgsCustReleaseVersion = Ipl_IplData.Tel[6];
                }
            }
            else
            {
                addr = Ipl_IpfData.Meta.CfgsStdStartAddr;  /*! \internal Jira UN-577 */
                Ipl_ClrTel();
                Ipl_IplData.Tel[0] = CMD_VERIFYOTPMEM;
                Ipl_IplData.Tel[1] = (addr >> 8U) & 0xFFU; /*! \internal Jira UN-577 */
                Ipl_IplData.Tel[2] = addr & 0xFFU;         /*! \internal Jira UN-577 */
                Ipl_IplData.Tel[3] = Ipl_IpfData.Meta.CfgsSize & 0xFFU; /* 34U */
                Ipl_IplData.TelLen = 4U;
                res = Ipl_ExecInicCmd();
                if (IPL_RES_OK == res)
                {
                    Ipl_Trace(IPL_TRACETAG_INFO, "OS81210_ReadConfigStringVersion (M)");
                    addr = Ipl_IpfData.Meta.CfgsStdStartAddr + 1U;
                    Ipl_ClrTel();
                    Ipl_IplData.Tel[0] = CMD_READOTPMEM;
                    Ipl_IplData.Tel[1] = (addr >> 8U) & 0xFFU;
                    Ipl_IplData.Tel[2] = addr & 0xFFU;
                    Ipl_IplData.Tel[3] = 3U;
                    Ipl_IplData.TelLen = CMD_READOTPMEM_TXLEN;
                    res = Ipl_ExecInicCmd();
                    if (IPL_RES_OK == res)
                    {
                        Ipl_InicData.CfgsVersionValid       = VERSION_VALID;
                        Ipl_InicData.CfgsCustMajorVersion   = Ipl_IplData.Tel[4];
                        Ipl_InicData.CfgsCustMinorVersion   = Ipl_IplData.Tel[5];
                        Ipl_InicData.CfgsCustReleaseVersion = Ipl_IplData.Tel[6];
                    }
                }
                else
                {
                    Ipl_Trace(IPL_TRACETAG_INFO, "OS81210_ReadConfigStringVersion (L)");
                    addr = Ipl_IpfData.Meta.CfgsDefStartAddr + 1U;
                    Ipl_ClrTel();
                    Ipl_IplData.Tel[0] = CMD_SETPROGMEMPAGE;
                    Ipl_IplData.Tel[1] = (addr >> 16U) & 0xFFU;
                    Ipl_IplData.TelLen = CMD_SETPROGMEMPAGE_TXLEN;
                    res = Ipl_ExecInicCmd();
                    if (IPL_RES_OK == res)
                    {
                        Ipl_ClrTel();
                        Ipl_IplData.Tel[0] = CMD_READPROGMEM;
                        Ipl_IplData.Tel[1] = (addr >> 8U) & 0xFFU;
                        Ipl_IplData.Tel[2] = addr & 0xFFU;
                        Ipl_IplData.Tel[3] = 3U;
                        Ipl_IplData.TelLen = CMD_READPROGMEM_TXLEN;
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
        }
    }
    if (IPL_RES_OK == res)
    {
        Ipl_Trace(Ipl_TraceTag(res), "OS81210_ReadConfigStringVersion returned 0x%02X - V%u.%u.%u", res,
                  Ipl_InicData.CfgsCustMajorVersion, Ipl_InicData.CfgsCustMinorVersion, Ipl_InicData.CfgsCustReleaseVersion);
    }
    else
    {
        Ipl_Trace(Ipl_TraceTag(res), "OS81210_ReadConfigStringVersion returned 0x%02X", res);
    }
    return res;
}


/*! \internal Programs a Configuration (CS+IS) to OTP. */ /* TBT */
uint8_t OS81210_ProgConfiguration(uint32_t lData, uint8_t pData[])
{
    uint8_t res;
    Ipl_Trace(IPL_TRACETAG_INFO, "OS81210_ProgConfiguration called");
    res = OS81210_ProgConfigString(lData, pData);
    if (IPL_RES_OK == res)
    {
        res = OS81210_ProgIdentString(lData, pData);
    }
    Ipl_Trace(Ipl_TraceTag(res), "OS81210_ProgConfiguration returned 0x%02X", res);
    return res;
}


/*! \internal Programs a ConfigString to OTP. */
uint8_t OS81210_ProgConfigString(uint32_t lData, uint8_t pData[])
{
    uint8_t res;
    Ipl_Trace(IPL_TRACETAG_INFO, "OS81210_ProgConfigString called");
    /* Get addresses and sizes from metadata */
    res = Ipl_ParseIpf(&Ipl_IpfData, lData, pData, STRINGTYPE_META);
    if (IPL_RES_OK == res)
    {
        /* Check if IPF contains proper ConfigString */
        res = Ipl_ParseIpf(&Ipl_IpfData, lData, pData, STRINGTYPE_CS);
        if (IPL_RES_OK == res)
        {
            /* Flashing Conf should only be allowed if Fw version fits */
            res = Ipl_CheckInicFwVersion();
            if (IPL_RES_OK == res)
            {
                res = OS81210_ClearTestMem();
                if (IPL_RES_OK == res)
                {
                    res = OS81210_ProgROMConfiguration(Ipl_IpfData.Meta.CfgsStdStartAddr, Ipl_IpfData.Meta.CfgsOvrlStartAddr, Ipl_IpfData.Meta.CfgsSize, pData);
                }
            }
        }
    }
    Ipl_Trace(Ipl_TraceTag(res), "OS81210_ProgConfigString returned 0x%02X", res);
    return res;
}


/*! \internal Programs an IdentString to OTP. */  /* TBT */
uint8_t OS81210_ProgIdentString(uint32_t lData, uint8_t pData[])
{
    uint8_t res;
    Ipl_Trace(IPL_TRACETAG_INFO, "OS81210_ProgIdentString called");
    /* Get addresses and sizes from metadata */
    res = Ipl_ParseIpf(&Ipl_IpfData, lData, pData, STRINGTYPE_META);
    if (IPL_RES_OK == res)
    {
        /* Check if IPF contains proper IdentString */
        res = Ipl_ParseIpf(&Ipl_IpfData, lData, pData, STRINGTYPE_IS);
        if (IPL_RES_OK == res)
        {
            /* Flashing Conf should only be allowed if Fw version fits */
            res = Ipl_CheckInicFwVersion();
            if (IPL_RES_OK == res)
            {
                res = OS81210_ClearTestMem();
                if (IPL_RES_OK == res)
                {
                    res = OS81210_ProgROMConfiguration(Ipl_IpfData.Meta.IdentsStdStartAddr, Ipl_IpfData.Meta.IdentsOvrlStartAddr,
                                                   Ipl_IpfData.Meta.IdentsSize, pData);
                }
            }
        }
    }
    Ipl_Trace(Ipl_TraceTag(res), "OS81210_ProgIdentString returned 0x%02X", res);
    return res;
}


/*! \internal Programs a Configuration to the test memory. */  /* TBT */
uint8_t OS81210_ProgTestConfiguration(uint32_t lData, uint8_t pData[])
{
    uint8_t res;
    Ipl_Trace(IPL_TRACETAG_INFO, "OS81210_ProgTestConfiguration called");
    res = OS81210_ProgTestConfigString(lData, pData);
    if (IPL_RES_OK == res)
    {
        res = OS81210_ProgTestIdentString(lData, pData);
    }
    Ipl_Trace(Ipl_TraceTag(res), "OS81210_ProgTestConfiguration returned 0x%02X", res);
    return res;
}


/*! \internal Programs a PatchString to the test memory. */  /* TBT */
uint8_t OS81210_ProgTestPatchString(uint32_t lData, uint8_t pData[])
{
    uint8_t res;
    Ipl_Trace(IPL_TRACETAG_INFO, "OS81210_ProgTestPatchString called");
    /* Get addresses and sizes from metadata */
    res = Ipl_ParseIpf(&Ipl_IpfData, lData, pData, STRINGTYPE_META);
    if (IPL_RES_OK == res)
    {
        /* Check if IPF contains proper PatchString */
        res = Ipl_ParseIpf(&Ipl_IpfData, lData, pData, STRINGTYPE_PS);
        if (IPL_RES_OK == res)
        {
            /* Flashing Conf should only be allowed if Fw version fits */
            res = Ipl_CheckInicFwVersion();
            if (IPL_RES_OK == res)
            {
                res = OS81210_ClearTestMem();
                if (IPL_RES_OK == res)
                {
                    res = OS81210_ProgTestMem(Ipl_IpfData.Meta.PatchsStdStartAddr, Ipl_IpfData.Meta.PatchsSize, pData, TESTMEM_DATA);
                }
            }
        }
    }
    Ipl_Trace(Ipl_TraceTag(res), "OS81210_ProgTestPatchString returned 0x%02X", res);
    return res;
}


/*! \internal Programs a ConfigString to the test memory. */  /* TBT */
uint8_t OS81210_ProgTestConfigString(uint32_t lData, uint8_t pData[])
{
    uint8_t res;
    Ipl_Trace(IPL_TRACETAG_INFO, "OS81210_ProgTestConfigString called");
    /* Get addresses and sizes from metadata */
    res = Ipl_ParseIpf(&Ipl_IpfData, lData, pData, STRINGTYPE_META);
    if (IPL_RES_OK == res)
    {
        /* Check if IPF contains proper ConfigString */
        res = Ipl_ParseIpf(&Ipl_IpfData, lData, pData, STRINGTYPE_CS);
        if (IPL_RES_OK == res)
        {
            /* Flashing Conf should only be allowed if Fw version fits */
            res = Ipl_CheckInicFwVersion();
            if (IPL_RES_OK == res)
            {
                res = OS81210_ClearTestMem();
                if (IPL_RES_OK == res)
                {
                    res = OS81210_ProgTestMem(Ipl_IpfData.Meta.CfgsTestStartAddr, Ipl_IpfData.Meta.CfgsSize, pData, TESTMEM_DATA);
                }
            }
        }
    }
    Ipl_Trace(Ipl_TraceTag(res), "OS81210_ProgTestConfigString returned 0x%02X", res);
    return res;
}


/*! \internal Programs an IdentString to the test memory. */  /* TBT */
uint8_t OS81210_ProgTestIdentString(uint32_t lData, uint8_t pData[])
{
    uint8_t res;
    Ipl_Trace(IPL_TRACETAG_INFO, "OS81210_ProgTestIdentString called");
    /* Get addresses and sizes from metadata */
    res = Ipl_ParseIpf(&Ipl_IpfData, lData, pData, STRINGTYPE_META);
    if (IPL_RES_OK == res)
    {
        /* Check if IPF contains proper IdentString */
        res = Ipl_ParseIpf(&Ipl_IpfData, lData, pData, STRINGTYPE_IS);
        if (IPL_RES_OK == res)
        {
            /* Flashing Conf should only be allowed if Fw version fits */
            res = Ipl_CheckInicFwVersion();
            if (IPL_RES_OK == res)
            {
                res = OS81210_ClearTestMem();
                if (IPL_RES_OK == res)
                {
                    res = OS81210_ProgTestMem(Ipl_IpfData.Meta.IdentsTestStartAddr, Ipl_IpfData.Meta.IdentsSize, pData, TESTMEM_DATA);
                }
            }
        }
    }
    Ipl_Trace(Ipl_TraceTag(res), "OS81210_ProgTestIdentString returned 0x%02X", res);
    return res;
}


/*! \internal Clears the test memory. (DUPUG 4.5.1) */  /* TBT */
static uint8_t OS81210_ClearTestMem(void)
{
    uint8_t  res;
    Ipl_Trace(IPL_TRACETAG_INFO, "OS81210_ClearTestMem called");
    if (INIC_TESTMEM_UNCLEARED == Ipl_InicData.TestMemCleared)
    {
        res = OS81210_ProgTestMem(0x00U, Ipl_IpfData.Meta.ChipTestMemSize, NULL, TESTMEM_CLEAR);
        Ipl_InicData.TestMemCleared = INIC_TESTMEM_CLEARED;
    }
    else
    {
        Ipl_Trace(IPL_TRACETAG_INFO, "OS81210_ClearTestMem Test memory already cleared before");
        res = IPL_RES_OK;
    }
    Ipl_Trace(Ipl_TraceTag(res), "OS81210_ClearTestMem returned 0x%02X", res);
    return res;
}


/*! \internal Programs the test memory. (DUPUG 4.5.2) */  /* TBT */
static uint8_t OS81210_ProgTestMem(uint32_t addr, uint32_t nOfBytes, uint8_t pData[], uint8_t clearData)
{
    uint8_t  res, i;
    uint32_t len;
    uint32_t data = 0U;
    uint32_t size = nOfBytes;
    Ipl_Trace(IPL_TRACETAG_INFO, "OS81210_ProgTestMem called with Addr 0x%04X, nOfBytes %u clearData: %u", addr, nOfBytes, clearData);
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
        /* Write Test Memory */
        Ipl_ClrTel();
        Ipl_IplData.Tel[0] = CMD_WRITETESTMEM;
        Ipl_IplData.Tel[1] = (addr >> 8) & 0xFFU;
        Ipl_IplData.Tel[2] = addr & 0xFFU;
        Ipl_IplData.Tel[3] = len & 0xFFU;
        for (i=0U; i<len; i++)
        {
            if (TESTMEM_CLEAR == clearData)
            {
                Ipl_IplData.Tel[4U+i] = 0U;
            }
            else
            {
                Ipl_IplData.Tel[4U+i] = Ipl_PData(i+data+Ipl_IpfData.StringOffset, (uint32_t) nOfBytes, pData);
            }
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
    Ipl_Trace(Ipl_TraceTag(res), "OS81210_ProgTestMem returned 0x%02X", res);
    return res;
}


/*! \internal Programs a patch string. (DUPUG 4.5.3) */  /* TBT */
uint8_t OS81210_ProgPatchString(uint32_t lData, uint8_t pData[])
{
    uint8_t res;
    Ipl_Trace(IPL_TRACETAG_INFO, "OS81210_ProgPatchString called");
    /* Get addresses and sizes from metadata */
    res = Ipl_ParseIpf(&Ipl_IpfData, lData, pData, STRINGTYPE_META);
    if (IPL_RES_OK == res)
    {
        /* Check if IPF contains proper PatchString */
        res = Ipl_ParseIpf(&Ipl_IpfData, lData, pData, STRINGTYPE_PS);
        if (IPL_RES_OK == res)
        {
            /* Flashing Conf should only be allowed if Fw version fits */
            res = Ipl_CheckInicFwVersion();
            if (IPL_RES_OK == res)
            {
                res = OS81210_ClearTestMem();
                if (IPL_RES_OK == res)
                {
                    /* Program OTP Memory */
                    res = OS81210_ProgOTPMem(Ipl_IpfData.Meta.PatchsStdStartAddr, Ipl_IpfData.Meta.PatchsSize, pData);
                    if (IPL_RES_OK == res)
                    {
                        res = OS81210_VerifyPatchString(Ipl_IpfData.Meta.PatchsStdStartAddr, Ipl_IpfData.Meta.PatchsSize, pData);
                    }
                }
            }
        }
    }
    Ipl_Trace(Ipl_TraceTag(res), "OS81210_ProgPatchString returned 0x%02X", res);
    return res;
}


/*! \internal Programs the OTP memory. (DUPUG 4.5.4) */
static uint8_t OS81210_ProgOTPMem(uint32_t addr, uint32_t nOfBytes, uint8_t pData[])
{
    uint8_t  res;
    uint32_t index;
    uint32_t size = nOfBytes;
    Ipl_Trace(IPL_TRACETAG_INFO, "OS81210_ProgOTPMem called with Addr 0x%04X, nOfBytes %u", addr, nOfBytes);
    index = 0U;
    do
    {
        Ipl_ProgressIndicator(size-nOfBytes, size); /* Update Progress Indicator */
        /* Write OTP Memory */
        Ipl_ClrTel();
        Ipl_IplData.Tel[0] = CMD_WRITEOTPMEM;
        Ipl_IplData.Tel[1] = (addr >> 8) & 0xFFU;
        Ipl_IplData.Tel[2] = addr & 0xFFU;
        Ipl_IplData.Tel[3] = 1U;
        Ipl_IplData.Tel[4] = Ipl_PData(index+Ipl_IpfData.StringOffset, 1U, pData);
        Ipl_IplData.TelLen = 5U;
        res = Ipl_ExecInicCmd();
        index++;
        addr++;
        nOfBytes--;
    } while ((nOfBytes != 0U) && (IPL_RES_OK == res));
    Ipl_ProgressIndicator(1U, 1U); /* Set Progress Indicator to 100 */
    Ipl_Trace(Ipl_TraceTag(res), "OS81210_ProgOTPMem returned 0x%02X", res);
    return res;
}


/*! \internal Verifies a written patch string. (DUPUG 4.5.5) */  /* TBT */
static uint8_t OS81210_VerifyPatchString(uint32_t addr, uint32_t nOfBytes, uint8_t pData[])
{
    uint8_t  res, i;
    uint32_t len;
    uint8_t  data_read[INIC_MAX_PATCHSTRINGSIZE];
    uint32_t data = 0U;
    uint32_t size = nOfBytes;
    Ipl_Trace(IPL_TRACETAG_INFO, "OS81210_VerifyPatchString called with Addr 0x%04X, nOfBytes %u", addr, nOfBytes);
    if (INIC_MAX_PATCHSTRINGSIZE > nOfBytes)
    {
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
            /* Read OTP Memory */
            Ipl_ClrTel();
            Ipl_IplData.Tel[0] = CMD_READOTPMEM;
            Ipl_IplData.Tel[1] = (addr >> 8) & 0xFFU;
            Ipl_IplData.Tel[2] = addr & 0xFFU;
            Ipl_IplData.Tel[3] = len & 0xFFU;
            Ipl_IplData.TelLen = 4U;
            res = Ipl_ExecInicCmd();
            for (i=0U; i<len; i++)
            {
                data_read[i + data] = Ipl_IplData.Tel[4U+i];

            }
            if (0U == nOfBytes)
            {
                break;
            }
            addr += Ipl_IpfData.Meta.BmMaxDataLength;
            data += Ipl_IpfData.Meta.BmMaxDataLength;
        } while ((nOfBytes != 0U) && (IPL_RES_OK == res));
        if (IPL_RES_OK == res)
        {
            for (i=0U; i<size; i++)
            {
                if (data_read[i] != Ipl_PData(i+Ipl_IpfData.StringOffset, nOfBytes, pData))
                {
                    res = IPL_RES_ERR_PS_INCORRECT;
                }
            }
        }
        Ipl_ProgressIndicator(1U, 1U); /* Set Progress Indicator to 100 */
    }
    else
    {
        res = IPL_RES_ERR_MAX_PATCHSTRINGLEN;
    }
    Ipl_Trace(Ipl_TraceTag(res), "OS81210_VerifyPatchString returned 0x%02X", res);
    return res;
}


/*! \internal Programs the ROM Configuration. (DUPUG 4.5.7) */  /* TBT */
static uint8_t OS81210_ProgROMConfiguration(uint32_t stdAddr, uint32_t ovrlAddr, uint32_t cfgSize, uint8_t cfgData[])
{
    uint8_t  res;
    uint32_t addr;
    Ipl_Trace(IPL_TRACETAG_INFO, "OS81210_ProgROMConfiguration called with StdAddr 0x%04X, OvrlAddr 0x%04X, CfgSize %u",
                  stdAddr, ovrlAddr, cfgSize);
    /* Verify OTP Memory */
    Ipl_ClrTel();
    Ipl_IplData.Tel[0] = CMD_VERIFYOTPMEM;
    Ipl_IplData.Tel[1] = (ovrlAddr >> 8U) & 0xFFU;
    Ipl_IplData.Tel[2] = ovrlAddr & 0xFFU;
    Ipl_IplData.Tel[3] = (uint8_t) cfgSize;
    Ipl_IplData.TelLen = CMD_VERIFYOTPMEM_TXLEN;
    res = Ipl_ExecInicCmd();
    if (IPL_RES_OK == res)
    {
        res = IPL_RES_ERR_NO_OTP_SPACE;
    }
    else
    {
        if (0x10U == res)
        {
            addr = ovrlAddr;
            res = OS81210_ProgOTPMem(addr, cfgSize, cfgData);
            if (IPL_RES_OK != res)
            {
                res = IPL_RES_ERR_PROG_OTP;
            }
            else
            {
                /* Verify OTP Memory */
                Ipl_ClrTel();
                Ipl_IplData.Tel[0] = CMD_VERIFYOTPMEM;
                Ipl_IplData.Tel[1] = (addr >> 8U) & 0xFFU;
                Ipl_IplData.Tel[2] = addr & 0xFFU;
                Ipl_IplData.Tel[3] = (uint8_t) cfgSize;
                Ipl_IplData.TelLen = CMD_VERIFYOTPMEM_TXLEN;
                res = Ipl_ExecInicCmd();
                if (IPL_RES_OK != res)
                {
                    res = IPL_RES_ERR_VERIFYOTPMEM;
                }
            }
        }
        else if (0x27U == res)
        {
            addr = stdAddr;
            /* Verify OTP Memory */
            Ipl_ClrTel();
            Ipl_IplData.Tel[0] = CMD_VERIFYOTPMEM;
            Ipl_IplData.Tel[1] = (addr >> 8U) & 0xFFU;
            Ipl_IplData.Tel[2] = addr & 0xFFU;
            Ipl_IplData.Tel[3] = (uint8_t) cfgSize;
            Ipl_IplData.TelLen = CMD_VERIFYOTPMEM_TXLEN;
            res = Ipl_ExecInicCmd();
            if (IPL_RES_OK != res)
            {
                res = OS81210_ProgOTPMem(addr, cfgSize, cfgData);
                if (IPL_RES_OK != res)
                {
                    addr = ovrlAddr;
                    res  = OS81210_ProgOTPMem(addr, cfgSize, cfgData);
                }
            }
            else
            {
                addr = ovrlAddr;
                res  = OS81210_ProgOTPMem(addr, cfgSize, cfgData);
            }
            /* Verify OTP Memory */
            if (IPL_RES_OK == res)
            {
                Ipl_ClrTel();
                Ipl_IplData.Tel[0] = CMD_VERIFYOTPMEM;
                Ipl_IplData.Tel[1] = (addr >> 8U) & 0xFFU;
                Ipl_IplData.Tel[2] = addr & 0xFFU;
                Ipl_IplData.Tel[3] = (uint8_t) cfgSize;
                Ipl_IplData.TelLen = CMD_VERIFYOTPMEM_TXLEN;
                res = Ipl_ExecInicCmd();
                if (IPL_RES_OK != res)
                {
                    res = IPL_RES_ERR_VERIFYOTPMEM;
                }
            }
        }
        else
        {
            res = IPL_RES_ERR_CMD_CC_BAD;
        }
    }
    Ipl_Trace(Ipl_TraceTag(res), "OS81210_ProgROMConfiguration returned 0x%02X", res);
    return res;
}


/*------------------------------------------------------------------------------------------------*/
/* UNSUPPORTED FUNCTIONS                                                                          */
/*------------------------------------------------------------------------------------------------*/

uint8_t OS81210_ProgFirmware(uint32_t lData, uint8_t pData[])
{
    Ipl_Trace(IPL_TRACETAG_ERR, "OS81210_ProgFirmware returned 0x%02X", IPL_RES_ERR_NOT_SUPPORTED);
    return IPL_RES_ERR_NOT_SUPPORTED;
}

#endif
