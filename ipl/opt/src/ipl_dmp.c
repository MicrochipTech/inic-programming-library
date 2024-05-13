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

/*! \file   ipl_dmp.c
 *  \brief  Dump functionality for INIC Programming Library
 *  \author Roland Trissl (RTR)
 *  \note   For support related to this code contact http://www.microchip.com/support.
 */


#include <stdint.h>
#include "ipl_cfg.h"
#include "ipl_pb.h"
#include "ipl.h"
#include "ipf.h"


#ifdef IPL_TRACETAG_DUMP

#include "ipl_dmp.h"


/*------------------------------------------------------------------------------------------------*/
/* PROTOTYPES                                                                                     */
/*------------------------------------------------------------------------------------------------*/

static void Ipl_TraceDump(uint32_t nOfBytes, uint8_t pData[]);


/*------------------------------------------------------------------------------------------------*/
/* FUNCTIONS                                                                                      */
/*------------------------------------------------------------------------------------------------*/


/*! \internal Traces out dump data. */
static void Ipl_TraceDump(uint32_t nOfBytes, uint8_t pData[])
{
    char     line [TRACELINE_MAXLEN];
    uint32_t len, i;
    uint32_t addr   = 0U;
    uint32_t maxBpl = 32U;
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
        /* Write Dump line */
        line[0U] = Ipl_Bcd2Char( (addr >> 24U) & 0xFFU, IPL_HIGH);
        line[1U] = Ipl_Bcd2Char( (addr >> 24U) & 0xFFU, IPL_LOW);
        line[2U] = Ipl_Bcd2Char( (addr >> 16U) & 0xFFU, IPL_HIGH);
        line[3U] = Ipl_Bcd2Char( (addr >> 16U) & 0xFFU, IPL_LOW);
        line[4U] = Ipl_Bcd2Char( (addr >> 8U)  & 0xFFU, IPL_HIGH);
        line[5U] = Ipl_Bcd2Char( (addr >> 8U)  & 0xFFU, IPL_LOW);
        line[6U] = Ipl_Bcd2Char(  addr & 0xFFU,         IPL_HIGH);
        line[7U] = Ipl_Bcd2Char(  addr & 0xFFU,         IPL_LOW);
        line[8U] = ' ';
        for (i=0U; i<len; i++)
        {
            line[9U+(i*3U)]  = Ipl_Bcd2Char(pData[i+addr], IPL_HIGH);
            line[10U+(i*3U)] = Ipl_Bcd2Char(pData[i+addr], IPL_LOW);
            line[11U+(i*3U)] = ' ';
        }
        line[9U+(len*3U)] = '\0';
        Ipl_Trace(IPL_TRACETAG_DUMP, line);
        if (0U == nOfBytes)
        {
            break;
        }
        addr += maxBpl;
    } while (nOfBytes != 0U);
}



/*! \internal Dumps the OTP memory */
uint8_t Ipl_DumpOtpMem(uint32_t lData, uint8_t pData[])
{
    uint8_t  res = IPL_RES_ERR_NOT_SUPPORTED;
    uint32_t len, i;
    uint32_t nOfBytes = INIC_MAX_OTPMEMSIZE;
    uint32_t size = nOfBytes;
    uint32_t addr = 0U;
    uint8_t  data_read[INIC_MAX_OTPMEMSIZE];
    Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_DumpOtpMem called");
    /* Get addresses and sizes from metadata */
    res = Ipl_ParseIpf(&Ipl_IpfData, lData, pData, STRINGTYPE_META);
    if (IPL_RES_OK == res)
    {
        /* Check if IPF fits to INIC */
        res = Ipl_CheckChipId();
        if (IPL_RES_OK == res)
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
                    data_read[i + addr] = Ipl_IplData.Tel[4U+i];
                }
                if (0U == nOfBytes)
                {
                    break;
                }
                addr += Ipl_IpfData.Meta.BmMaxDataLength;
            } while ((nOfBytes != 0U) && (IPL_RES_OK == res));
            Ipl_ProgressIndicator(1U, 1U); /* Set Progress Indicator to 100 */
        }
    }
    if (IPL_RES_OK == res)
    {
        Ipl_Trace(IPL_TRACETAG_DUMP, "[OTP.Memory]");
        Ipl_TraceDump(size, data_read);
    }
    Ipl_Trace(Ipl_TraceTag(res), "Ipl_DumpOtpMem returned 0x%02X", res);
    return res;
}


/*! \internal Dumps the Info memory */
uint8_t Ipl_DumpInfoMem(uint32_t lData, uint8_t pData[])
{
    uint8_t  res = IPL_RES_ERR_NOT_SUPPORTED;
    uint32_t len, i;
    uint32_t nOfBytes = INIC_MAX_INFOMEMSIZE;
    uint32_t size = nOfBytes;
    uint32_t addr = 0U;
    uint8_t  data_read[INIC_MAX_INFOMEMSIZE];
    Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_DumpInfoMem called");
    /* Get addresses and sizes from metadata */
    res = Ipl_ParseIpf(&Ipl_IpfData, lData, pData, STRINGTYPE_META);
    if (IPL_RES_OK == res)
    {
        /* Check if IPF fits to INIC */
        res = Ipl_CheckChipId();
        if (IPL_RES_OK == res)
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
                /* Read Info Memory */
                Ipl_ClrTel();
                Ipl_IplData.Tel[0] = CMD_READINFOMEM;
                Ipl_IplData.Tel[1] = (addr >> 8) & 0xFFU;
                Ipl_IplData.Tel[2] = addr & 0xFFU;
                Ipl_IplData.Tel[3] = len & 0xFFU;
                Ipl_IplData.TelLen = 4U;
                res = Ipl_ExecInicCmd();
                for (i=0U; i<len; i++)
                {
                    data_read[i + addr] = Ipl_IplData.Tel[4U+i];
                }
                if (0U == nOfBytes)
                {
                    break;
                }
                addr += Ipl_IpfData.Meta.BmMaxDataLength;
            } while ((nOfBytes != 0U) && (IPL_RES_OK == res));
            Ipl_ProgressIndicator(1U, 1U); /* Set Progress Indicator to 100 */
        }
    }
    if (IPL_RES_OK == res)
    {
        Ipl_Trace(IPL_TRACETAG_DUMP, "[Cougar.InfoMemory]");
        Ipl_TraceDump(size, data_read);
    }
    Ipl_Trace(Ipl_TraceTag(res), "Ipl_DumpInfoMem returned 0x%02X", res);
    return res;
}


/*! \internal Dumps the Program memory */
uint8_t Ipl_DumpProgMem(uint32_t lData, uint8_t pData[])
{
    uint8_t  res = IPL_RES_ERR_NOT_SUPPORTED;
    uint32_t len, i;
    uint32_t nOfBytes = INIC_MAX_PROGMEMSIZE;
    uint32_t size = nOfBytes;
    uint32_t addr = 0U;
    uint8_t  data_read[INIC_MAX_PROGMEMSIZE];
    Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_DumpProgMem called");
    /* Get addresses and sizes from metadata */
    res = Ipl_ParseIpf(&Ipl_IpfData, lData, pData, STRINGTYPE_META);
    if (IPL_RES_OK == res)
    {
        /* Check if IPF fits to INIC */
        res = Ipl_CheckChipId();
        if (IPL_RES_OK == res)
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
                /* Read Program Memory */
                Ipl_ClrTel();
                Ipl_IplData.Tel[0] = CMD_READPROGMEM;
                Ipl_IplData.Tel[1] = (addr >> 8) & 0xFFU;
                Ipl_IplData.Tel[2] = addr & 0xFFU;
                Ipl_IplData.Tel[3] = len & 0xFFU;
                Ipl_IplData.TelLen = 4U;
                res = Ipl_ExecInicCmd();
                for (i=0U; i<len; i++)
                {
                    data_read[i + addr] = Ipl_IplData.Tel[4U+i];
                }
                if (0U == nOfBytes)
                {
                    break;
                }
                addr += Ipl_IpfData.Meta.BmMaxDataLength;
            } while ((nOfBytes != 0U) && (IPL_RES_OK == res));
            Ipl_ProgressIndicator(1U, 1U); /* Set Progress Indicator to 100 */
        }
    }
    if (IPL_RES_OK == res)
    {
        Ipl_Trace(IPL_TRACETAG_DUMP, "[Cougar.ProgramMem]");
        Ipl_TraceDump(size, data_read);
    }
    Ipl_Trace(Ipl_TraceTag(res), "Ipl_DumpProgMem returned 0x%02X", res);
    return res;
}


/*! \internal Dumps the Test memory */
uint8_t Ipl_DumpTestMem(uint32_t lData, uint8_t pData[])
{
    uint8_t  res = IPL_RES_ERR_NOT_SUPPORTED;
    uint32_t len, i;
    uint32_t nOfBytes = INIC_MAX_TESTMEMSIZE;
    uint32_t size = nOfBytes;
    uint32_t addr = 0U;
    uint8_t  data_read[INIC_MAX_TESTMEMSIZE];
    Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_DumpTestMem called");
    /* Get addresses and sizes from metadata */
    res = Ipl_ParseIpf(&Ipl_IpfData, lData, pData, STRINGTYPE_META);
    if (IPL_RES_OK == res)
    {
        /* Check if IPF fits to INIC */
        res = Ipl_CheckChipId();
        if (IPL_RES_OK == res)
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
                /* Read Info Memory */
                Ipl_ClrTel();
                Ipl_IplData.Tel[0] = CMD_READTESTMEM;
                Ipl_IplData.Tel[1] = (addr >> 8) & 0xFFU;
                Ipl_IplData.Tel[2] = addr & 0xFFU;
                Ipl_IplData.Tel[3] = len & 0xFFU;
                Ipl_IplData.TelLen = 4U;
                res = Ipl_ExecInicCmd();
                for (i=0U; i<len; i++)
                {
                    data_read[i + addr] = Ipl_IplData.Tel[4U+i];
                }
                if (0U == nOfBytes)
                {
                    break;
                }
                addr += Ipl_IpfData.Meta.BmMaxDataLength;
            } while ((nOfBytes != 0U) && (IPL_RES_OK == res));
            Ipl_ProgressIndicator(1U, 1U); /* Set Progress Indicator to 100 */
        }
    }
    if (IPL_RES_OK == res)
    {
        Ipl_Trace(IPL_TRACETAG_DUMP, "[OTP.TestMemory]");
        Ipl_TraceDump(size, data_read);
    }
    Ipl_Trace(Ipl_TraceTag(res), "Ipl_DumpTestMem returned 0x%02X", res);
    return res;
}


/*! \internal Dumps RAM */
uint8_t Ipl_DumpRam(uint32_t lData, uint8_t pData[])
{
    uint8_t  res = IPL_RES_ERR_NOT_SUPPORTED;
    uint32_t len, i;
    uint32_t nOfBytes = INIC_MAX_RAMSIZE;
    uint32_t size = nOfBytes;
    uint32_t addr = 0U;
    uint8_t  data_read[INIC_MAX_RAMSIZE];
    Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_DumpRam called");
    /* Get addresses and sizes from metadata */
    res = Ipl_ParseIpf(&Ipl_IpfData, lData, pData, STRINGTYPE_META);
    if (IPL_RES_OK == res)
    {
        /* Check if IPF fits to INIC */
        res = Ipl_CheckChipId();
        if (IPL_RES_OK == res)
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
                /* Read Info Memory */
                Ipl_ClrTel();
                Ipl_IplData.Tel[0] = CMD_READRAM;
                Ipl_IplData.Tel[1] = (addr >> 8) & 0xFFU;
                Ipl_IplData.Tel[2] = addr & 0xFFU;
                Ipl_IplData.Tel[3] = len & 0xFFU;
                Ipl_IplData.TelLen = 4U;
                res = Ipl_ExecInicCmd();
                for (i=0U; i<len; i++)
                {
                    data_read[i + addr] = Ipl_IplData.Tel[4U+i];
                }
                if (0U == nOfBytes)
                {
                    break;
                }
                addr += Ipl_IpfData.Meta.BmMaxDataLength;
            } while ((nOfBytes != 0U) && (IPL_RES_OK == res));
            Ipl_ProgressIndicator(1U, 1U); /* Set Progress Indicator to 100 */
        }
    }
    if (IPL_RES_OK == res)
    {
        Ipl_Trace(IPL_TRACETAG_DUMP, "[Cougar.RAM]");
        Ipl_TraceDump(size, data_read);
    }
    Ipl_Trace(Ipl_TraceTag(res), "Ipl_DumpRam returned 0x%02X", res);
    return res;
}


/*! \internal Dumps IO Registers */
uint8_t Ipl_DumpIOReg(uint32_t lData, uint8_t pData[])
{
    uint8_t  res = IPL_RES_ERR_NOT_SUPPORTED;
    uint32_t len, i;
    uint32_t nOfBytes = INIC_MAX_IOREGNUM;
    uint32_t size = nOfBytes;
    uint32_t addr = 0U;
    uint8_t  data_read[INIC_MAX_IOREGNUM*2U];
    Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_DumpIOReg called");
    /* Get addresses and sizes from metadata */
    res = Ipl_ParseIpf(&Ipl_IpfData, lData, pData, STRINGTYPE_META);
    if (IPL_RES_OK == res)
    {
        /* Check if IPF fits to INIC */
        res = Ipl_CheckChipId();
        if (IPL_RES_OK == res)
        {
            do
            {
                Ipl_ProgressIndicator(size-nOfBytes, size); /* Update Progress Indicator */
                if (nOfBytes >= 16U) /* Ipl_IpfData.Meta.BmMaxDataLength */
                {
                    len       = 16U; /* Ipl_IpfData.Meta.BmMaxDataLength */
                    nOfBytes -= 16U; /* Ipl_IpfData.Meta.BmMaxDataLength */
                }
                else
                {
                    len      = nOfBytes;
                    nOfBytes = 0U;
                }
                /* Read Info Memory */
                Ipl_ClrTel();
                Ipl_IplData.Tel[0] = CMD_READIOREG;
                Ipl_IplData.Tel[1] = 0U;
                Ipl_IplData.Tel[2] = addr & 0xFFU;
                Ipl_IplData.Tel[3] = len & 0xFFU;
                Ipl_IplData.TelLen = 4U;
                res = Ipl_ExecInicCmd();
                for (i=0U; i<(len*2U); i++)
                {
                    data_read[i + (addr*2U)] = Ipl_IplData.Tel[4U+i];
                }
                if (0U == nOfBytes)
                {
                    break;
                }
                addr += 16U; /* Ipl_IpfData.Meta.BmMaxDataLength */
            } while ((nOfBytes != 0U) && (IPL_RES_OK == res));
            Ipl_ProgressIndicator(1U, 1U); /* Set Progress Indicator to 100 */
        }
    }
    if (IPL_RES_OK == res)
    {
        Ipl_Trace(IPL_TRACETAG_DUMP, "[Cougar.IORegisters]");
        Ipl_TraceDump(size, data_read);
    }
    Ipl_Trace(Ipl_TraceTag(res), "Ipl_DumpIOReg returned 0x%02X", res);
    return res;
}


/*! \internal Dumps CPU Registers */
uint8_t Ipl_DumpCPUReg(uint32_t lData, uint8_t pData[])
{
    uint8_t  res = IPL_RES_ERR_NOT_SUPPORTED;
    uint32_t len, i;
    uint32_t nOfBytes = INIC_MAX_CPUREGNUM;
    uint32_t size = nOfBytes;
    uint32_t addr = 0U;
    uint8_t  data_read[INIC_MAX_CPUREGNUM*2U];
    Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_DumpCPUReg called");
    /* Get addresses and sizes from metadata */
    res = Ipl_ParseIpf(&Ipl_IpfData, lData, pData, STRINGTYPE_META);
    if (IPL_RES_OK == res)
    {
        /* Check if IPF fits to INIC */
        res = Ipl_CheckChipId();
        if (IPL_RES_OK == res)
        {
            do
            {
                Ipl_ProgressIndicator(size-nOfBytes, size); /* Update Progress Indicator */
                if (nOfBytes >= 16U) /* Ipl_IpfData.Meta.BmMaxDataLength */
                {
                    len       = 16U; /* Ipl_IpfData.Meta.BmMaxDataLength */
                    nOfBytes -= 16U; /* Ipl_IpfData.Meta.BmMaxDataLength */
                }
                else
                {
                    len      = nOfBytes;
                    nOfBytes = 0U;
                }
                /* Read Info Memory */
                Ipl_ClrTel();
                Ipl_IplData.Tel[0] = CMD_READCPUREG;
                Ipl_IplData.Tel[1] = 0U;
                Ipl_IplData.Tel[2] = addr & 0xFFU;
                Ipl_IplData.Tel[3] = len & 0xFFU;
                Ipl_IplData.TelLen = 4U;
                res = Ipl_ExecInicCmd();
                for (i=0U; i<(len*2U); i++)
                {
                    data_read[i + (addr*2U)] = Ipl_IplData.Tel[4U+i];
                }
                if (0U == nOfBytes)
                {
                    break;
                }
                addr += 16U; /* Ipl_IpfData.Meta.BmMaxDataLength */
            } while ((nOfBytes != 0U) && (IPL_RES_OK == res));
            Ipl_ProgressIndicator(1U, 1U); /* Set Progress Indicator to 100 */
        }
    }
    if (IPL_RES_OK == res)
    {
        Ipl_Trace(IPL_TRACETAG_DUMP, "[Cougar.CPURegisters]");
        Ipl_TraceDump(size, data_read);
    }
    Ipl_Trace(Ipl_TraceTag(res), "Ipl_DumpCPUReg returned 0x%02X", res);
    return res;
}


/*! \internal Dumps Extended IO Registers */
uint8_t Ipl_DumpExtIOReg(uint32_t lData, uint8_t pData[])
{
    uint8_t  res = IPL_RES_ERR_NOT_SUPPORTED;
    uint32_t len, i;
    uint32_t nOfBytes = INIC_MAX_EXTIOREGNUM;
    uint32_t size = nOfBytes;
    uint32_t addr = 0U;
    uint8_t  data_read[INIC_MAX_EXTIOREGNUM*2U];
    Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_DumpExtIOReg called");
    /* Get addresses and sizes from metadata */
    res = Ipl_ParseIpf(&Ipl_IpfData, lData, pData, STRINGTYPE_META);
    if (IPL_RES_OK == res)
    {
        /* Check if IPF fits to INIC */
        res = Ipl_CheckChipId();
        if (IPL_RES_OK == res)
        {
            do
            {
                Ipl_ProgressIndicator(size-nOfBytes, size); /* Update Progress Indicator */
                if (nOfBytes >= 16U) /* Ipl_IpfData.Meta.BmMaxDataLength */
                {
                    len       = 16U; /* Ipl_IpfData.Meta.BmMaxDataLength */
                    nOfBytes -= 16U; /* Ipl_IpfData.Meta.BmMaxDataLength */
                }
                else
                {
                    len      = nOfBytes;
                    nOfBytes = 0U;
                }
                /* Read Info Memory */
                Ipl_ClrTel();
                Ipl_IplData.Tel[0] = CMD_READEXTIOREG;
                Ipl_IplData.Tel[1] = 0U;
                Ipl_IplData.Tel[2] = addr & 0xFFU;
                Ipl_IplData.Tel[3] = len & 0xFFU;
                Ipl_IplData.TelLen = 4U;
                res = Ipl_ExecInicCmd();
                for (i=0U; i<(len*2U); i++)
                {
                    data_read[i + (addr*2U)] = Ipl_IplData.Tel[4U+i];
                }
                if (0U == nOfBytes)
                {
                    break;
                }
                addr += 16U; /* Ipl_IpfData.Meta.BmMaxDataLength */
            } while ((nOfBytes != 0U) && (IPL_RES_OK == res));
            Ipl_ProgressIndicator(1U, 1U); /* Set Progress Indicator to 100 */
        }
    }
    if (IPL_RES_OK == res)
    {
        Ipl_Trace(IPL_TRACETAG_DUMP, "[Cougar.ExtendedIO]");
        Ipl_TraceDump(size, data_read);
    }
    Ipl_Trace(Ipl_TraceTag(res), "Ipl_DumpExtIOReg returned 0x%02X", res);
    return res;
}


/*! \internal Dumps Data Buffer */
uint8_t Ipl_DumpDataBuf(uint32_t lData, uint8_t pData[])
{
    uint8_t  res = IPL_RES_ERR_NOT_SUPPORTED;
    uint32_t len, i;
    uint32_t nOfBytes = INIC_MAX_DATABUFFERSIZE;
    uint32_t size = nOfBytes;
    uint32_t addr = 0U;
    uint8_t  data_read[INIC_MAX_DATABUFFERSIZE];
    Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_DumpDataBuf called");
    /* Get addresses and sizes from metadata */
    res = Ipl_ParseIpf(&Ipl_IpfData, lData, pData, STRINGTYPE_META);
    if (IPL_RES_OK == res)
    {
        /* Check if IPF fits to INIC */
        res = Ipl_CheckChipId();
        if (IPL_RES_OK == res)
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
                /* Read Info Memory */
                Ipl_ClrTel();
                Ipl_IplData.Tel[0] = CMD_READDATABUF;
                Ipl_IplData.Tel[1] = (addr >> 8) & 0xFFU;
                Ipl_IplData.Tel[2] = addr & 0xFFU;
                Ipl_IplData.Tel[3] = len & 0xFFU;
                Ipl_IplData.TelLen = 4U;
                res = Ipl_ExecInicCmd();
                for (i=0U; i<len; i++)
                {
                    data_read[i + addr] = Ipl_IplData.Tel[4U+i];
                }
                if (0U == nOfBytes)
                {
                    break;
                }
                addr += Ipl_IpfData.Meta.BmMaxDataLength;
            } while ((nOfBytes != 0U) && (IPL_RES_OK == res));
            Ipl_ProgressIndicator(1U, 1U); /* Set Progress Indicator to 100 */
        }
    }
    if (IPL_RES_OK == res)
    {
        Ipl_Trace(IPL_TRACETAG_DUMP, "[DataBuffer.RAM]");
        Ipl_TraceDump(size, data_read);
    }
    Ipl_Trace(Ipl_TraceTag(res), "Ipl_DumpDataBuf returned 0x%02X", res);
    return res;
}


/*! \internal Dumps Routing Table */
uint8_t Ipl_DumpRT(uint32_t lData, uint8_t pData[])
{
    uint8_t  res = IPL_RES_ERR_NOT_SUPPORTED;
    uint32_t len, i;
    uint32_t nOfBytes = INIC_MAX_RTSIZE;
    uint32_t size = nOfBytes;
    uint32_t addr = 0U;
    uint8_t  data_read[INIC_MAX_RTSIZE];
    Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_DumpRT called");
    /* Get addresses and sizes from metadata */
    res = Ipl_ParseIpf(&Ipl_IpfData, lData, pData, STRINGTYPE_META);
    if (IPL_RES_OK == res)
    {
        /* Check if IPF fits to INIC */
        res = Ipl_CheckChipId();
        if (IPL_RES_OK == res)
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
                /* Read Info Memory */
                Ipl_ClrTel();
                Ipl_IplData.Tel[0] = CMD_READRT;
                Ipl_IplData.Tel[1] = (addr >> 8) & 0xFFU;
                Ipl_IplData.Tel[2] = addr & 0xFFU;
                Ipl_IplData.Tel[3] = len & 0xFFU;
                Ipl_IplData.TelLen = 4U;
                res = Ipl_ExecInicCmd();
                for (i=0U; i<len; i++)
                {
                    data_read[i + addr] = Ipl_IplData.Tel[4U+i];
                }
                if (0U == nOfBytes)
                {
                    break;
                }
                addr += Ipl_IpfData.Meta.BmMaxDataLength;
            } while ((nOfBytes != 0U) && (IPL_RES_OK == res));
            Ipl_ProgressIndicator(1U, 1U); /* Set Progress Indicator to 100 */
        }
    }
    if (IPL_RES_OK == res)
    {
        Ipl_Trace(IPL_TRACETAG_DUMP, "[RoutingEngine.Table]");
        Ipl_TraceDump(size, data_read);
    }
    Ipl_Trace(Ipl_TraceTag(res), "Ipl_DumpRT returned 0x%02X", res);
    return res;
}


/*! \internal Dumps Register File 0 */
uint8_t Ipl_DumpRF0(uint32_t lData, uint8_t pData[])
{
    uint8_t  res = IPL_RES_ERR_NOT_SUPPORTED;
    uint32_t len, i;
    uint32_t nOfBytes = INIC_MAX_RFSIZE;
    uint32_t size = nOfBytes;
    uint32_t addr = 0U;
    uint8_t  data_read[INIC_MAX_RFSIZE];
    Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_DumpRF0 called");
    /* Get addresses and sizes from metadata */
    res = Ipl_ParseIpf(&Ipl_IpfData, lData, pData, STRINGTYPE_META);
    if (IPL_RES_OK == res)
    {
        /* Check if IPF fits to INIC */
        res = Ipl_CheckChipId();
        if (IPL_RES_OK == res)
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
                /* Read Info Memory */
                Ipl_ClrTel();
                Ipl_IplData.Tel[0] = CMD_READRF0;
                Ipl_IplData.Tel[1] = (addr >> 8) & 0xFFU;
                Ipl_IplData.Tel[2] = addr & 0xFFU;
                Ipl_IplData.Tel[3] = len & 0xFFU;
                Ipl_IplData.TelLen = 4U;
                res = Ipl_ExecInicCmd();
                for (i=0U; i<len; i++)
                {
                    data_read[i + addr] = Ipl_IplData.Tel[4U+i];
                }
                if (0U == nOfBytes)
                {
                    break;
                }
                addr += Ipl_IpfData.Meta.BmMaxDataLength;
            } while ((nOfBytes != 0U) && (IPL_RES_OK == res));
            Ipl_ProgressIndicator(1U, 1U); /* Set Progress Indicator to 100 */
        }
    }
    if (IPL_RES_OK == res)
    {
        Ipl_Trace(IPL_TRACETAG_DUMP, "[RoutingEngine.File0]");
        Ipl_TraceDump(size, data_read);
    }
    Ipl_Trace(Ipl_TraceTag(res), "Ipl_DumpRF0 returned 0x%02X", res);
    return res;
}


/*! \internal Dumps Register File 1 */
uint8_t Ipl_DumpRF1(uint32_t lData, uint8_t pData[])
{
    uint8_t  res = IPL_RES_ERR_NOT_SUPPORTED;
    uint32_t len, i;
    uint32_t nOfBytes = INIC_MAX_RFSIZE;
    uint32_t size = nOfBytes;
    uint32_t addr = 0U;
    uint8_t  data_read[INIC_MAX_RFSIZE];
    Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_DumpRF1 called");
    /* Get addresses and sizes from metadata */
    res = Ipl_ParseIpf(&Ipl_IpfData, lData, pData, STRINGTYPE_META);
    if (IPL_RES_OK == res)
    {
        /* Check if IPF fits to INIC */
        res = Ipl_CheckChipId();
        if (IPL_RES_OK == res)
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
                /* Read Info Memory */
                Ipl_ClrTel();
                Ipl_IplData.Tel[0] = CMD_READRF1;
                Ipl_IplData.Tel[1] = (addr >> 8) & 0xFFU;
                Ipl_IplData.Tel[2] = addr & 0xFFU;
                Ipl_IplData.Tel[3] = len & 0xFFU;
                Ipl_IplData.TelLen = 4U;
                res = Ipl_ExecInicCmd();
                for (i=0U; i<len; i++)
                {
                    data_read[i + addr] = Ipl_IplData.Tel[4U+i];
                }
                if (0U == nOfBytes)
                {
                    break;
                }
                addr += Ipl_IpfData.Meta.BmMaxDataLength;
            } while ((nOfBytes != 0U) && (IPL_RES_OK == res));
            Ipl_ProgressIndicator(1U, 1U); /* Set Progress Indicator to 100 */
        }
    }
    if (IPL_RES_OK == res)
    {
        Ipl_Trace(IPL_TRACETAG_DUMP, "[RoutingEngine.File1]");
        Ipl_TraceDump(size, data_read);
    }
    Ipl_Trace(Ipl_TraceTag(res), "Ipl_DumpRF1 returned 0x%02X", res);
    return res;
}


/*! \internal Dumps all available information */
uint8_t Ipl_DumpAll(uint32_t lData, uint8_t pData[])
{
    uint8_t res_d;
    uint8_t res = IPL_RES_ERR_NOT_SUPPORTED;

    res_d = Ipl_DumpRam(lData, pData);
    if (IPL_RES_OK == res_d)
    {
        res = IPL_RES_OK;
    }
    res_d = Ipl_DumpExtIOReg(lData, pData);
    if (IPL_RES_OK == res_d)
    {
        res = IPL_RES_OK;
    }
    res_d = Ipl_DumpIOReg(lData, pData);
    if (IPL_RES_OK == res_d)
    {
        res = IPL_RES_OK;
    }
    res_d = Ipl_DumpCPUReg(lData, pData);
    if (IPL_RES_OK == res_d)
    {
        res = IPL_RES_OK;
    }
    res_d = Ipl_DumpDataBuf(lData, pData);
    if (IPL_RES_OK == res_d)
    {
        res = IPL_RES_OK;
    }
    res_d = Ipl_DumpRT(lData, pData);
    if (IPL_RES_OK == res_d)
    {
        res = IPL_RES_OK;
    }
    res_d = Ipl_DumpRF0(lData, pData);
    if (IPL_RES_OK == res_d)
    {
        res = IPL_RES_OK;
    }
    res_d = Ipl_DumpRF1(lData, pData);
    if (IPL_RES_OK == res_d)
    {
        res = IPL_RES_OK;
    }
    res_d = Ipl_DumpOtpMem(lData, pData);
    if (IPL_RES_OK == res_d)
    {
        res = IPL_RES_OK;
    }
    res_d = Ipl_DumpTestMem(lData, pData);
    if (IPL_RES_OK == res_d)
    {
        res = IPL_RES_OK;
    }
    res_d = Ipl_DumpInfoMem(lData, pData);
    if (IPL_RES_OK == res_d)
    {
        res = IPL_RES_OK;
    }
    res_d = Ipl_DumpProgMem(lData, pData);
    if (IPL_RES_OK == res_d)
    {
        res = IPL_RES_OK;
    }
    return res;
}


#endif
