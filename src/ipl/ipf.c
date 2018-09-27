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

/*! \file ipf.c
 *  \brief Internal functions to parse IPF data for INIC Programming Library
 *  \author Roland Trissl (RTR)
 */

#include <stdint.h>
#include <stddef.h>
#include "ipl_cfg.h"
#include "ipl.h"
#include "ipf.h"

/*------------------------------------------------------------------------------------------------*/
/* VARIABLES                                                                                      */
/*------------------------------------------------------------------------------------------------*/

Ipl_IpfData_t Ipl_IpfData;

/*------------------------------------------------------------------------------------------------*/
/* FUNCTION PROTOTYPES                                                                            */
/*------------------------------------------------------------------------------------------------*/

static uint8_t Ipl_SetStdMetaProps(Ipl_IpfData_t *ipf, uint32_t pid, uint32_t pval, uint8_t ptype);
#ifdef IPL_LEGACY_IPF
static uint8_t Ipl_SetDefaultMetaProps(Ipl_IpfData_t *ipf);
#endif
static uint8_t Ipl_CheckMetaPType(uint32_t pid, uint32_t pval, uint8_t ptype_act, uint8_t ptype_ref);
static void    Ipl_TraceIpf(uint32_t nOfBytes, uint8_t pData[]);


/*------------------------------------------------------------------------------------------------*/
/* FUNCTIONS                                                                                      */
/*------------------------------------------------------------------------------------------------*/

/*! \internal Parses the referred IPF data for the referred string. */
uint8_t Ipl_ParseIpf(Ipl_IpfData_t *ipf, uint32_t lData, uint8_t pData[], uint8_t stringType)
{
    uint32_t offset = 6U;
    uint8_t  res    = IPL_RES_ERR_IPF_INVALID; /*! \internal Jira UN-373 */
    uint32_t offset_s;
    uint8_t  ptype;
    uint32_t i, pi, pid, pval, plen;
    Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_ParseIpf called with %u byte IPF, StringType 0x%02X", lData, stringType);
    res = Ipl_ClrPData(lData, pData);
    if (IPL_RES_OK == res)
    {
        if ((STRING_MIN_LEN <= lData) && (NULL != pData)) /*! \internal Jira UN-373 */
        {
            ipf->ChipID = Ipl_PData(1U, lData, pData);
            Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_ParseIpf IPF ChipID 0x%02X", ipf->ChipID);
            res = IPL_RES_ERR_IPF_WRONGINIC;
            if (Ipl_IplData.ChipID == ipf->ChipID)
            {
                Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_ParseIpf start with Offset %u", offset);
                while ((offset + 9U) < lData)
                {
                    if (Ipl_PData(offset, lData, pData) == stringType)
                    {
                        Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_ParseIpf StringType 0x%02X found at Offset %u", stringType, offset);
                        break;
                    }
                    offset_s = offset;
                    offset += ( ((uint32_t) Ipl_PData(offset_s + 6U, lData, pData) << 24U )
                              + ((uint32_t) Ipl_PData(offset_s + 7U, lData, pData) << 16U )
                              + ((uint32_t) Ipl_PData(offset_s + 8U, lData, pData) <<  8U )
                              + ((uint32_t) Ipl_PData(offset_s + 9U, lData, pData) & 0xFFU) ) + 10U;
                    Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_ParseIpf current Offset %u", offset);
                }
                res = IPL_RES_ERR_IPF_WRONGSTRINGTYPE; /* Offset out of bounds */
                Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_ParseIpf current Offset %u", offset);
                if ((offset + 13U) < lData) /* +9U, 10U */
                {
                    res = IPL_RES_OK;
                    if (Ipl_PData(offset, lData, pData) == stringType)
                    {
                        /* Image is fine and we can continue */
                        Ipl_IpfData.StringType    = stringType;
                        Ipl_IpfData.StringOffset  = offset + 10U; /* Points to first data byte */
                        Ipl_IpfData.StringSize    = (uint32_t) Ipl_PData(offset + 6U, lData, pData) << 24U;
                        Ipl_IpfData.StringSize   += (uint32_t) Ipl_PData(offset + 7U, lData, pData) << 16U;
                        Ipl_IpfData.StringSize   += (uint32_t) Ipl_PData(offset + 8U, lData, pData) <<  8U;
                        Ipl_IpfData.StringSize   += (uint32_t) Ipl_PData(offset + 9U, lData, pData)       ;
                        switch (ipf->StringType)
                        {
                            case STRINGTYPE_FW:
                                ipf->ProgAddr  = (uint32_t) Ipl_PData(offset + 2U, lData, pData) << 24U ;
                                ipf->ProgAddr += (uint32_t) Ipl_PData(offset + 3U, lData, pData) << 16U ;
                                ipf->ProgAddr += (uint32_t) Ipl_PData(offset + 4U, lData, pData) <<  8U ;
                                ipf->ProgAddr += (uint32_t) Ipl_PData(offset + 5U, lData, pData)        ;
                                Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_ParseIpf ProgAddr 0x%X", ipf->ProgAddr);
                                break;
                            case STRINGTYPE_PS:
                                ipf->ProgAddr = Ipl_IpfData.Meta.PatchsStdStartAddr;
                                Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_ParseIpf ProgAddr 0x%X", ipf->ProgAddr);
                                break;
                            case STRINGTYPE_META:
                                ipf->Meta.NumOfItems  = (uint32_t) Ipl_PData(offset + 10U, lData, pData) << 24U ;
                                ipf->Meta.NumOfItems += (uint32_t) Ipl_PData(offset + 11U, lData, pData) << 16U ;
                                ipf->Meta.NumOfItems += (uint32_t) Ipl_PData(offset + 12U, lData, pData) <<  8U ;
                                ipf->Meta.NumOfItems += (uint32_t) Ipl_PData(offset + 13U, lData, pData)        ;
                                Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_ParseIpf %u Meta Items found", ipf->Meta.NumOfItems);
                                if ((offset + 14U + (ipf->Meta.NumOfItems*12U)) < lData)
                                {
                                    for (i=0U; i<ipf->Meta.NumOfItems; i++)
                                    {
                                        pid   = (uint32_t) Ipl_PData(offset + 14U + (i*12U)      , lData, pData) << 24U;
                                        pid  += (uint32_t) Ipl_PData(offset + 14U + (i*12U) +  1U, lData, pData) << 16U;
                                        pid  += (uint32_t) Ipl_PData(offset + 14U + (i*12U) +  2U, lData, pData) <<  8U;
                                        pid  += (uint32_t) Ipl_PData(offset + 14U + (i*12U) +  3U, lData, pData)       ;
                                        ptype =            Ipl_PData(offset + 14U + (i*12U) +  4U, lData, pData)       ;
                                        pval  = (uint32_t) Ipl_PData(offset + 14U + (i*12U) +  8U, lData, pData) << 24U;
                                        pval += (uint32_t) Ipl_PData(offset + 14U + (i*12U) +  9U, lData, pData) << 16U;
                                        pval += (uint32_t) Ipl_PData(offset + 14U + (i*12U) + 10U, lData, pData) <<  8U;
                                        pval += (uint32_t) Ipl_PData(offset + 14U + (i*12U) + 11U, lData, pData)       ;

                                        res = Ipl_SetStdMetaProps(ipf, pid, pval, ptype);
                                        /* Handle Tool Type String */
                                        if (METAID_TOOLTYPE == pid)
                                        {
                                            if (METATYPE_STRING == ptype)
                                            {
                                                plen  = (uint32_t) Ipl_PData(offset + 14U + (i*12U) + 5U, lData, pData)  << 16U ;
                                                plen += (uint32_t) Ipl_PData(offset + 14U + (i*12U) + 6U, lData, pData)  <<  8U ;
                                                plen += (uint32_t) Ipl_PData(offset + 14U + (i*12U) + 7U, lData, pData)         ;
                                                if ((TOOL_MAX_TYPELEN - 1U) < plen)
                                                {
                                                    plen = TOOL_MAX_TYPELEN - 1U;
                                                }
                                                if ((offset + 10U + pval + plen) <= lData)
                                                {
                                                    for (pi=0U; pi<plen; pi++)
                                                    {
                                                        ipf->Meta.ToolType[pi] = (char) Ipl_PData(offset + 10U + pval + pi,
                                                                                                  lData, pData);
                                                    }
                                                    ipf->Meta.ToolType[pi + 1U] = '\0';
                                                    Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_ParseIpf MetaID 0x%08X (ToolType) = '%s'",
                                                              pid, ipf->Meta.ToolType);
                                                }
                                                else
                                                {
                                                    /* res = IPL_RES_ERR_IPF_WRONGSTRINGTYPE; */ /* No error, just ignore it */
                                                    Ipl_Trace(IPL_TRACETAG_ERR,
                                                              "Ipl_ParseIpf MetaID 0x%08X (ToolType) out of bounds", pid);
                                                }
                                            }
                                        }
                                    }
                                }
                                else
                                {
                                    res = IPL_RES_ERR_IPF_WRONGSTRINGTYPE;
                                    Ipl_Trace(IPL_TRACETAG_ERR, "Ipl_ParseIpf Meta data out of bounds");
                                }
                                break;
                            case STRINGTYPE_CONFIG:
                            case STRINGTYPE_CS:
                            case STRINGTYPE_IS:
                                break;
                            default:
                                Ipl_Trace(IPL_TRACETAG_ERR, "Ipl_ParseIpf StringType 0x%02X unknown", ipf->StringType);
                                break;
                        }
                        if (IPL_RES_OK == res)
                        {
                            if (ipf->StringSize < lData)
                            {
                                Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_ParseIpf StringSize %u", ipf->StringSize);
                                Ipl_TraceIpf(ipf->StringSize, pData);
                            }
                            else
                            {
                                res = IPL_RES_ERR_IPF_WRONGSTRINGTYPE;
                                Ipl_Trace(IPL_TRACETAG_ERR, "Ipl_ParseIpf StringSize out of bounds");
                            }
                        }
                        else
                        {
                            Ipl_Trace(IPL_TRACETAG_ERR, "Ipl_ParseIpf Internal size out of bounds");
                        }
                    }
                }
                else
                {
                    res = IPL_RES_ERR_IPF_WRONGSTRINGTYPE; /*! \internal Jira UN-374, UN-386 */
                    Ipl_Trace(IPL_TRACETAG_ERR, "Ipl_ParseIpf StringType 0x%02X not found", stringType);
                }
            }
        }
        else
        {
            Ipl_Trace(IPL_TRACETAG_ERR, "Ipl_ParseIpf IPF contains no valid data");
        }
#ifdef IPL_LEGACY_IPF
        if (STRINGTYPE_META == stringType)
        {
            res = Ipl_SetDefaultMetaProps(ipf);
        }
#endif
    }
    Ipl_Trace(Ipl_TraceTag(res), "Ipl_ParseIpf returned 0x%02X", res);
    return res;
}


/*! \internal Checks all the standard Meta properies and sets them. */
static uint8_t Ipl_SetStdMetaProps(Ipl_IpfData_t *ipf, uint32_t pid, uint32_t pval, uint8_t ptype)
{
	uint8_t res = IPL_RES_OK;
	switch (pid)
    {
        case  METAID_CHIPID:
            res = Ipl_CheckMetaPType(pid, pval, ptype, METATYPE_UINT8);
            if (IPL_RES_OK == res)
            {
                ipf->Meta.ChipID = (uint8_t) pval;
            }
            break;
        case METAID_CHIPPRGMEMSIZE:
            res = Ipl_CheckMetaPType(pid, pval, ptype, METATYPE_UINT32);
            if (IPL_RES_OK == res)
            {
                ipf->Meta.ChipPrgMemSize = (uint32_t) pval;
            }
            break;
        case METAID_CHIPPRGMEMPAGESIZE:
            res = Ipl_CheckMetaPType(pid, pval, ptype, METATYPE_UINT32);
            if (IPL_RES_OK == res)
            {
                ipf->Meta.ChipPrgMemPageSize = (uint32_t) pval;
            }
            break;
        case METAID_CHIPPRGMEMSECTIONSIZE:
            res = Ipl_CheckMetaPType(pid, pval, ptype, METATYPE_UINT16);
            if (IPL_RES_OK == res)
            {
                ipf->Meta.ChipPrgMemSectionSize = (uint16_t) pval;
            }
            break;
        case METAID_CHIPNUMOFINFOMEMSECTIONS:
            res = Ipl_CheckMetaPType(pid, pval, ptype, METATYPE_UINT8);
            if (IPL_RES_OK == res)
            {
                ipf->Meta.ChipNumOfInfoMemSections = (uint8_t) pval;
            }
            break;
        case METAID_CHIPINFOMEMSECTIONSIZE:
            res = Ipl_CheckMetaPType(pid, pval, ptype, METATYPE_UINT16);
            if (IPL_RES_OK == res)
            {
                ipf->Meta.ChipInfoMemSectionSize = (uint16_t) pval;
            }
            break;
        case METAID_CHIPTESTMEMSIZE:
            res = Ipl_CheckMetaPType(pid, pval, ptype, METATYPE_UINT16);
            if (IPL_RES_OK == res)
            {
                ipf->Meta.ChipTestMemSize = (uint16_t) pval;
            }
            break;
        case METAID_CHIPCOMBINEDCFGSTARTADDR:
            res = Ipl_CheckMetaPType(pid, pval, ptype, METATYPE_UINT32);
            if (IPL_RES_OK == res)
            {
                ipf->Meta.ChipCombinedCfgStartAddr = (uint32_t) pval;
            }
            break;
        case METAID_CHIPCOMBINEDCFGSIZE:
            res = Ipl_CheckMetaPType(pid, pval, ptype, METATYPE_UINT16);
            if (IPL_RES_OK == res)
            {
                ipf->Meta.ChipCombinedCfgSize = (uint16_t) pval;
            }
            break;
        case  METAID_FWMAJORVERSION:
            res = Ipl_CheckMetaPType(pid, pval, ptype, METATYPE_UINT8);
            if (IPL_RES_OK == res)
            {
                ipf->Meta.FwMajorVersion = (uint8_t) pval;
                ipf->Meta.FwVersionValid = VERSION_VALID;
            }
            break;
        case  METAID_FWMINORVERSION:
            res = Ipl_CheckMetaPType(pid, pval, ptype, METATYPE_UINT8);
            if (IPL_RES_OK == res)
            {
                ipf->Meta.FwMinorVersion = (uint8_t) pval;
                ipf->Meta.FwVersionValid = VERSION_VALID;
            }
            break;
        case  METAID_FWRELEASEVERSION:
            res = Ipl_CheckMetaPType(pid, pval, ptype, METATYPE_UINT8);
            if (IPL_RES_OK == res)
            {
                ipf->Meta.FwReleaseVersion = (uint8_t) pval;
                ipf->Meta.FwVersionValid = VERSION_VALID;
            }
            break;
        case METAID_FWBUILDVERSION:
            res = Ipl_CheckMetaPType(pid, pval, ptype, METATYPE_UINT32);
            if (IPL_RES_OK == res)
            {
                ipf->Meta.FwBuildVersion = (uint32_t) pval;
                ipf->Meta.FwVersionValid = VERSION_VALID;
            }
            break;
        case METAID_FWSIZE:
            res = Ipl_CheckMetaPType(pid, pval, ptype, METATYPE_UINT32);
            if (IPL_RES_OK == res)
            {
                ipf->Meta.FwSize = (uint32_t) pval;
            }
            break;
        case METAID_FWSTARTADDR:
            res = Ipl_CheckMetaPType(pid, pval, ptype, METATYPE_UINT32);
            if (IPL_RES_OK == res)
            {
                ipf->Meta.FwStartAddr = (uint32_t) pval;
            }
            break;
        case METAID_BMSIZE:
            res = Ipl_CheckMetaPType(pid, pval, ptype, METATYPE_UINT16);
            if (IPL_RES_OK == res)
            {
                ipf->Meta.BmSize = (uint16_t) pval;
            }
            break;
        case METAID_BMMAXDATALENGTH:
            res = Ipl_CheckMetaPType(pid, pval, ptype, METATYPE_UINT16);
            if (IPL_RES_OK == res)
            {
                ipf->Meta.BmMaxDataLength = (uint16_t) pval;
            }
            break;
        case METAID_CFGSFORMATVERSION:
            res = Ipl_CheckMetaPType(pid, pval, ptype, METATYPE_UINT8);
            if (IPL_RES_OK == res)
            {
                ipf->Meta.CfgsFormatVersion = (uint8_t) pval;
            }
            break;
        case METAID_CFGSCUSTMAJORVERSION:
            res = Ipl_CheckMetaPType(pid, pval, ptype, METATYPE_UINT8);
            if (IPL_RES_OK == res)
            {
                ipf->Meta.CfgsCustMajorVersion = (uint8_t) pval;
                ipf->Meta.CfgsVersionValid     = VERSION_VALID;
            }
            break;
        case METAID_CFGSCUSTMINORVERSION:
            res = Ipl_CheckMetaPType(pid, pval, ptype, METATYPE_UINT8);
            if (IPL_RES_OK == res)
            {
                ipf->Meta.CfgsCustMinorVersion = (uint8_t) pval;
                ipf->Meta.CfgsVersionValid     = VERSION_VALID;
            }
            break;
        case METAID_CFGSCUSTRELEASEVERSION:
            res = Ipl_CheckMetaPType(pid, pval, ptype, METATYPE_UINT8);
            if (IPL_RES_OK == res)
            {
                ipf->Meta.CfgsCustReleaseVersion = (uint8_t) pval;
                ipf->Meta.CfgsVersionValid       = VERSION_VALID;
            }
            break;
        case METAID_CFGSSIZE:
            res = Ipl_CheckMetaPType(pid, pval, ptype, METATYPE_UINT16);
            if (IPL_RES_OK == res)
            {
                ipf->Meta.CfgsSize = (uint16_t) pval;
            }
            break;
        case METAID_CFGSDEFSTARTADDR:
            res = Ipl_CheckMetaPType(pid, pval, ptype, METATYPE_UINT32);
            if (IPL_RES_OK == res)
            {
                ipf->Meta.CfgsDefStartAddr = (uint32_t) pval;
            }
            break;
        case METAID_CFGSSTDSTARTADDR:
            res = Ipl_CheckMetaPType(pid, pval, ptype, METATYPE_UINT32);
            if (IPL_RES_OK == res)
            {
                ipf->Meta.CfgsStdStartAddr = (uint32_t) pval;
            }
            break;
        case METAID_CFGSOVRLSTARTADDR:
            res = Ipl_CheckMetaPType(pid, pval, ptype, METATYPE_UINT32);
            if (IPL_RES_OK == res)
            {
                ipf->Meta.CfgsOvrlStartAddr = (uint32_t) pval;
            }
            break;
        case METAID_CFGSTESTSTARTADDR:
            res = Ipl_CheckMetaPType(pid, pval, ptype, METATYPE_UINT32);
            if (IPL_RES_OK == res)
            {
                ipf->Meta.CfgsTestStartAddr = (uint32_t) pval;
            }
            break;
        case METAID_IDENTSFORMATVERSION:
            res = Ipl_CheckMetaPType(pid, pval, ptype, METATYPE_UINT8);
            if (IPL_RES_OK == res)
            {
                ipf->Meta.IdentsFormatVersion = (uint8_t) pval;
            }
            break;
        case METAID_IDENTSSIZE:
            res = Ipl_CheckMetaPType(pid, pval, ptype, METATYPE_UINT16);
            if (IPL_RES_OK == res)
            {
                ipf->Meta.IdentsSize = (uint16_t) pval;
            }
            break;
        case METAID_IDENTSSTDSTARTADDR:
            res = Ipl_CheckMetaPType(pid, pval, ptype, METATYPE_UINT32);
            if (IPL_RES_OK == res)
            {
                ipf->Meta.IdentsStdStartAddr = (uint32_t) pval;
            }
            break;
        case METAID_IDENTSOVRLSTARTADDR:
            res = Ipl_CheckMetaPType(pid, pval, ptype, METATYPE_UINT32);
            if (IPL_RES_OK == res)
            {
                ipf->Meta.IdentsOvrlStartAddr = (uint32_t) pval;
            }
            break;
        case METAID_IDENTSTESTSTARTADDR:
            res = Ipl_CheckMetaPType(pid, pval, ptype, METATYPE_UINT32);
            if (IPL_RES_OK == res)
            {
                ipf->Meta.IdentsTestStartAddr = (uint32_t) pval;
            }
            break;
        case METAID_PATCHSSIZE:
            res = Ipl_CheckMetaPType(pid, pval, ptype, METATYPE_UINT16);
            if (IPL_RES_OK == res)
            {
                ipf->Meta.PatchsSize = (uint16_t) pval;
            }
            break;
        case METAID_PATCHSSTDSTARTADDR:
            res = Ipl_CheckMetaPType(pid, pval, ptype, METATYPE_UINT32);
            if (IPL_RES_OK == res)
            {
                ipf->Meta.PatchsStdStartAddr = (uint32_t) pval;
            }
            break;
        case METAID_PATCHSTESTSTARTADDR:
            res = Ipl_CheckMetaPType(pid, pval, ptype, METATYPE_UINT32);
            if (IPL_RES_OK == res)
            {
                ipf->Meta.PatchsTestStartAddr = (uint32_t) pval;
            }
            break;
        case METAID_TOOLMAJORVERSION:
            res = Ipl_CheckMetaPType(pid, pval, ptype, METATYPE_UINT8);
            if (IPL_RES_OK == res)
            {
                ipf->Meta.ToolMajorVersion = (uint8_t) pval;
            }
            break;
        case METAID_TOOLMINORVERSION:
            res = Ipl_CheckMetaPType(pid, pval, ptype, METATYPE_UINT8);
            if (IPL_RES_OK == res)
            {
                ipf->Meta.ToolMinorVersion = (uint8_t) pval;
            }
            break;
        case METAID_TOOLRELEASEVERSION:
            res = Ipl_CheckMetaPType(pid, pval, ptype, METATYPE_UINT8);
            if (IPL_RES_OK == res)
            {
                ipf->Meta.ToolReleaseVersion = (uint8_t) pval;
            }
            break;
        case METAID_TOOLBUILDVERSION:
            res = Ipl_CheckMetaPType(pid, pval, ptype, METATYPE_UINT32);
            if (IPL_RES_OK == res)
            {
                 ipf->Meta.FwBuildVersion = (uint32_t) pval;
            }
            break;
        default:
            /* ignore all other IDs */
            Ipl_Trace(IPL_TRACETAG_ERR, "Ipl_SetStdMetaProps MetaProp 0x%08X (unknown) ignored", pid);
            break;
    }
	Ipl_Trace(Ipl_TraceTag(res), "Ipl_SetStdMetaProps returned 0x%02X", res);
	return res;
}


#ifdef IPL_LEGACY_IPF
/*! \internal In case the IPF data does not contain Meta information the default values are set. */
static uint8_t Ipl_SetDefaultMetaProps(Ipl_IpfData_t *ipf)
{
    uint8_t res = IPL_RES_OK;
	Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_SetDefaultMetaProps called with ChipID 0x%02X", ipf->ChipID);
	/* If no meta data, the versions are always invalid */
	ipf->Meta.FwVersionValid   = VERSION_INVALID;
	ipf->Meta.CfgsVersionValid = VERSION_INVALID;
    switch (ipf->ChipID)
    {
        case IPL_CHIP_OS81118:
            ipf->Meta.ChipID                    = IPL_CHIP_OS81118;
            ipf->Meta.ChipPrgMemSize            = 0x0030000U;
            ipf->Meta.ChipPrgMemPageSize        = 0x0010000U;
            ipf->Meta.BmMaxDataLength           = 32U;
            ipf->Meta.ChipPrgMemSectionSize     = 0x400U;
            ipf->Meta.ChipNumOfInfoMemSections  = 2U;
            ipf->Meta.ChipInfoMemSectionSize    = 0x0200U;
            ipf->Meta.BmSize                    = 0x1800U;
            ipf->Meta.CfgsStdStartAddr          = 0x0000U;
            ipf->Meta.CfgsSize                  = 0x0080U;
            ipf->Meta.ChipCombinedCfgStartAddr  = 0x0000U;
            ipf->Meta.IdentsStdStartAddr        = 0x0100U;
            ipf->Meta.IdentsSize                = 0x000EU;
            ipf->Meta.FwSize                    = 0x0002E7FFU;
            ipf->Meta.FwStartAddr               = 0x00001800U;
            ipf->Meta.NumOfItems                = 15U;
            break;
        case IPL_CHIP_OS81119:
            ipf->Meta.ChipID                    = IPL_CHIP_OS81119;
            ipf->Meta.ChipPrgMemSize            = 0x0030000U;
            ipf->Meta.ChipPrgMemPageSize        = 0x0010000U;
            ipf->Meta.BmMaxDataLength           = 32U;
            ipf->Meta.ChipPrgMemSectionSize     = 0x400U;
            ipf->Meta.ChipNumOfInfoMemSections  = 2U;
            ipf->Meta.ChipInfoMemSectionSize    = 0x0200U;
            ipf->Meta.BmSize                    = 0x1800U;
            ipf->Meta.CfgsStdStartAddr          = 0x0000U;
            ipf->Meta.CfgsSize                  = 0x0080U;
            ipf->Meta.ChipCombinedCfgStartAddr  = 0x0000U;
            ipf->Meta.IdentsStdStartAddr        = 0x0100U;
            ipf->Meta.IdentsSize                = 0x000EU;
            ipf->Meta.FwSize                    = 0x0002E7FFU;
            ipf->Meta.FwStartAddr               = 0x00001800U;
            ipf->Meta.NumOfItems                = 15U;
            break;
        case IPL_CHIP_OS81210:
            ipf->Meta.ChipID                    = IPL_CHIP_OS81210;
            ipf->Meta.ChipPrgMemSize            = 0x0030000U;
            ipf->Meta.ChipPrgMemPageSize        = 0x0010000U;
            ipf->Meta.BmMaxDataLength           = 32U;
            ipf->Meta.ChipTestMemSize           = 768U;
            ipf->Meta.BmSize                    = 0x2000U;
            ipf->Meta.CfgsDefStartAddr          = 0x0002FF9CU;
            ipf->Meta.CfgsStdStartAddr          = 0x00U;
            ipf->Meta.CfgsOvrlStartAddr         = 0x22U;
            ipf->Meta.CfgsTestStartAddr         = 0x00U;
            ipf->Meta.IdentsStdStartAddr        = 0x44U;
            ipf->Meta.IdentsOvrlStartAddr       = 0x52U;
            ipf->Meta.IdentsTestStartAddr       = 0x22U;
            ipf->Meta.PatchsStdStartAddr        = 0x60U;
            ipf->Meta.PatchsTestStartAddr       = 0x30U;
            ipf->Meta.NumOfItems                = 15U;
            break;
        case IPL_CHIP_OS81212:
            ipf->Meta.ChipID                    = IPL_CHIP_OS81212;
            ipf->Meta.ChipPrgMemSize            = 0x0030000U;
            ipf->Meta.ChipPrgMemPageSize        = 0x0010000U;
            ipf->Meta.BmMaxDataLength           = 32U;
            ipf->Meta.ChipTestMemSize           = 768U;
            ipf->Meta.BmSize                    = 0x2000U;
            ipf->Meta.CfgsDefStartAddr          = 0x0002FF9CU;
            ipf->Meta.CfgsStdStartAddr          = 0x00U;
            ipf->Meta.CfgsOvrlStartAddr         = 0x22U;
            ipf->Meta.CfgsTestStartAddr         = 0x00U;
            ipf->Meta.IdentsStdStartAddr        = 0x44U;
            ipf->Meta.IdentsOvrlStartAddr       = 0x52U;
            ipf->Meta.IdentsTestStartAddr       = 0x22U;
            ipf->Meta.PatchsStdStartAddr        = 0x60U;
            ipf->Meta.PatchsTestStartAddr       = 0x30U;
            ipf->Meta.NumOfItems                = 15U;
            break;
        case IPL_CHIP_OS81214:
            ipf->Meta.ChipID                    = IPL_CHIP_OS81214;
            ipf->Meta.ChipPrgMemSize            = 0x0030000U;
            ipf->Meta.ChipPrgMemPageSize        = 0x0010000U;
            ipf->Meta.BmMaxDataLength           = 32U;
            ipf->Meta.ChipTestMemSize           = 768U;
            ipf->Meta.BmSize                    = 0x2000U;
            ipf->Meta.CfgsDefStartAddr          = 0x0002FF9CU;
            ipf->Meta.CfgsStdStartAddr          = 0x00U;
            ipf->Meta.CfgsOvrlStartAddr         = 0x22U;
            ipf->Meta.CfgsTestStartAddr         = 0x00U;
            ipf->Meta.IdentsStdStartAddr        = 0x44U;
            ipf->Meta.IdentsOvrlStartAddr       = 0x52U;
            ipf->Meta.IdentsTestStartAddr       = 0x22U;
            ipf->Meta.PatchsStdStartAddr        = 0x60U;
            ipf->Meta.PatchsTestStartAddr       = 0x30U;
            ipf->Meta.NumOfItems                = 15U;
            break;
        default:
            Ipl_Trace(IPL_TRACETAG_ERR, "Ipl_SetDefaultMetaProps ChipID unexpected");
			res = IPL_RES_ERR_IPF_WRONGINIC;
            break;
    }
    if (IPL_RES_OK == res)
    {
        Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_SetDefaultMetaProps MetaData is overwritten by legacy default values");
    }
	Ipl_Trace(Ipl_TraceTag(res), "Ipl_SetDefaultMetaProps returned 0x%02X", res);
	return res;
}
#endif


/*! \internal Sets all Meta properties to the default value. */
void Ipl_ClrIpfData(Ipl_IpfData_t *ipf)
{
    Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_ClrIpfData");
    ipf->ProgAddr                          = DEFAULTVAL_UINT32;
    ipf->StringSize                        = DEFAULTVAL_UINT32;
    ipf->StringOffset                      = DEFAULTVAL_UINT32;
    ipf->StringType                        = DEFAULTVAL_UINT8;
    ipf->ChipID                            = DEFAULTVAL_UINT8;
    ipf->Meta.NumOfItems                   = DEFAULTVAL_UINT32;
    ipf->Meta.ChipID                       = DEFAULTVAL_UINT8;
    ipf->Meta.ChipPrgMemSize               = DEFAULTVAL_UINT32;
    ipf->Meta.ChipPrgMemPageSize           = DEFAULTVAL_UINT32;
    ipf->Meta.ChipPrgMemSectionSize        = DEFAULTVAL_UINT16;
    ipf->Meta.ChipNumOfInfoMemSections     = DEFAULTVAL_UINT8;
    ipf->Meta.ChipInfoMemSectionSize       = DEFAULTVAL_UINT16;
    ipf->Meta.ChipTestMemSize              = DEFAULTVAL_UINT16;
    ipf->Meta.ChipCombinedCfgStartAddr     = DEFAULTVAL_UINT32;
    ipf->Meta.ChipCombinedCfgSize          = DEFAULTVAL_UINT16;
    ipf->Meta.FwMajorVersion               = DEFAULTVAL_UINT8;
    ipf->Meta.FwMinorVersion               = DEFAULTVAL_UINT8;
    ipf->Meta.FwReleaseVersion             = DEFAULTVAL_UINT8;
    ipf->Meta.FwBuildVersion               = DEFAULTVAL_UINT32;
    ipf->Meta.FwSize                       = DEFAULTVAL_UINT32;
    ipf->Meta.FwStartAddr                  = DEFAULTVAL_UINT32;
    ipf->Meta.FwVersionValid               = VERSION_INVALID;
    ipf->Meta.BmSize                       = DEFAULTVAL_UINT16;
    ipf->Meta.BmMaxDataLength              = DEFAULTVAL_UINT16;
    ipf->Meta.CfgsFormatVersion            = DEFAULTVAL_UINT8;
    ipf->Meta.CfgsCustMajorVersion         = DEFAULTVAL_UINT8;
    ipf->Meta.CfgsCustMinorVersion         = DEFAULTVAL_UINT8;
    ipf->Meta.CfgsCustReleaseVersion       = DEFAULTVAL_UINT8;
    ipf->Meta.CfgsSize                     = DEFAULTVAL_UINT16;
    ipf->Meta.CfgsDefStartAddr             = DEFAULTVAL_UINT32;
    ipf->Meta.CfgsStdStartAddr             = DEFAULTVAL_UINT32;
    ipf->Meta.CfgsOvrlStartAddr            = DEFAULTVAL_UINT32;
    ipf->Meta.CfgsTestStartAddr            = DEFAULTVAL_UINT32;
    ipf->Meta.CfgsVersionValid             = VERSION_INVALID;
    ipf->Meta.IdentsFormatVersion          = DEFAULTVAL_UINT8;
    ipf->Meta.IdentsSize                   = DEFAULTVAL_UINT16;
    ipf->Meta.IdentsStdStartAddr           = DEFAULTVAL_UINT32;
    ipf->Meta.IdentsOvrlStartAddr          = DEFAULTVAL_UINT32;
    ipf->Meta.IdentsTestStartAddr          = DEFAULTVAL_UINT32;
    ipf->Meta.PatchsSize                   = DEFAULTVAL_UINT16;
    ipf->Meta.PatchsStdStartAddr           = DEFAULTVAL_UINT32;
    ipf->Meta.PatchsTestStartAddr          = DEFAULTVAL_UINT32;
    ipf->Meta.ToolType[0]                  = DEFAULTVAL_STRING[0];
    ipf->Meta.ToolMajorVersion             = DEFAULTVAL_UINT8;
    ipf->Meta.ToolMinorVersion             = DEFAULTVAL_UINT8;
    ipf->Meta.ToolReleaseVersion           = DEFAULTVAL_UINT8;
    ipf->Meta.ToolBuildVersion             = DEFAULTVAL_UINT32;
}


/*! \internal Checks if a Meta property has the correct DataType. */
static uint8_t Ipl_CheckMetaPType(uint32_t pid, uint32_t pval, uint8_t ptype_act, uint8_t ptype_ref)
{
    uint8_t res = IPL_RES_OK;
    if (ptype_act == ptype_ref)
    {
        Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_CheckMetaPType returned 0x%02X - MetaProp 0x%08X = 0x%08X, PType = 0x%02X", res, pid, pval, ptype_act);
    }
    else
    {
        /* res = IPL_RES_ERR_IPF_WRONGSTRINGTYPE; */ /* No error, just ignore it */
        Ipl_Trace(IPL_TRACETAG_ERR, "Ipl_CheckMetaPType returned 0x%02X - MetaProp 0x%08X, PType (0x%02X) unexpected", res, pid, ptype_act);

    }
    return res;
}


/*------------------------------------------------------------------------------------------------*/
/* TRACE LOGGING                                                                                  */
/*------------------------------------------------------------------------------------------------*/

/*! \internal Traces out the IPF data. */
static void Ipl_TraceIpf(uint32_t nOfBytes, uint8_t pData[])
{
#ifdef IPL_TRACETAG_IPF
    uint8_t  i;
    char     line [TRACELINE_MAXLEN];
    uint32_t len;
    uint32_t data   = 0U;
    uint32_t maxBpl = (TRACELINE_MAXLEN - 4U) / 3U;
    do
    {
        if (nOfBytes >= maxBpl)
        {
            len       = maxBpl;
            nOfBytes -= maxBpl;
        }
        else
        {
            len      = nOfBytes;
            nOfBytes = 0U;
        }
        /* Write Ipf line */
        for (i=0U; i<len; i++)
        {
            line[0U+(i*3U)] = Ipl_BcdChr(Ipl_PData(Ipl_IpfData.StringOffset+i+data, nOfBytes, pData), IPL_HIGH);
            line[1U+(i*3U)] = Ipl_BcdChr(Ipl_PData(Ipl_IpfData.StringOffset+i+data, nOfBytes, pData), IPL_LOW);
            line[2U+(i*3U)] = ' ';
        }
        line[0U+(len*3U)] = '\0';
        Ipl_Trace(IPL_TRACETAG_IPF, line);
        if (0U == nOfBytes)
        {
            break;
        }
        data += maxBpl;
    } while (nOfBytes != 0U);
#endif
}
