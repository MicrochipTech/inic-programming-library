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

/*! \file ipl.c
 *  \brief Internal general functions for INIC Programming Library
 *  \author Roland Trissl (RTR)
 */

#include <stdint.h>
#include <stddef.h>
#include "ipl_cfg.h"
#include "ipl.h"
#include "ipl_pb.h"
#include "ipf.h"
#include "ipl_81118.h"
#include "ipl_81119.h"
#include "ipl_81210.h"
#include "ipl_81212.h"
#include "ipl_81214.h"


/*------------------------------------------------------------------------------------------------*/
/* CHECKS FOR ipl_cfg.h                                                                           */
/*------------------------------------------------------------------------------------------------*/

#ifndef IPL_DATACHUNK_SIZE
#error "ipl_cfg.h: IPL_DATACHUNK_SIZE needs to be defined."
#endif

#if (IPL_DATACHUNK_SIZE > 0) && (IPL_DATACHUNK_SIZE < 2048)
#error "ipl_cfg.h: IPL_DATACHUNK_SIZE must be at least 2048 (or 0)."
#endif

#ifndef IPL_TRACETAG_INFO
#error "ipl_cfg.h: IPL_TRACETAG_INFO needs to be defined."
#endif

#ifndef IPL_TRACETAG_ERR
#error "ipl_cfg.h: IPL_TRACETAG_ERR needs to be defined."
#endif

#ifndef IPL_USE_OS81118
#ifndef IPL_USE_OS81119
#ifndef IPL_USE_OS81210
#ifndef IPL_USE_OS81212
#ifndef IPL_USE_OS81214
#error "ipl_cfg.h: At least one supported INIC needs to be defined (for example IPL_USE_OS81118)."
#endif
#endif
#endif
#endif
#endif


/*------------------------------------------------------------------------------------------------*/
/* CONSTANTS                                                                                      */
/*------------------------------------------------------------------------------------------------*/

#define IPL_RES_CC_OK 0xFFU /* Completion code is fine */


/*------------------------------------------------------------------------------------------------*/
/* FUNCTION PROTOTYPES                                                                            */
/*------------------------------------------------------------------------------------------------*/

static uint8_t Ipl_ReadFirmwareVersion(void);
static uint8_t Ipl_CheckUpdate(Ipl_IpfData_t *ipf, uint32_t lData, uint8_t pData[], uint8_t stringType);
static uint8_t Ipl_StartupInic(uint8_t chipMode);
static uint8_t Ipl_WaitForResponse(void);
static void    Ipl_TraceTel(uint8_t direction);
#ifdef IPL_USE_INTPIN
static uint8_t Ipl_WaitForInt(uint16_t timeout);
#endif


/*------------------------------------------------------------------------------------------------*/
/* VARIABLES                                                                                      */
/*------------------------------------------------------------------------------------------------*/

Ipl_IplData_t  Ipl_IplData;
Ipl_InicData_t Ipl_InicData;


/*------------------------------------------------------------------------------------------------*/
/* FUNCTIONS                                                                                      */
/*------------------------------------------------------------------------------------------------*/

/*! \internal Sets INIC in programming mode and reads ChipID and firmware version. */
uint8_t Ipl_EnterProgMode(uint8_t chipID)
{
    uint8_t res = IPL_RES_ERR_HW_INIC_COMM;
    uint8_t cc = 0U;
    Ipl_IplData.ChipID = chipID;
#ifdef IPL_INICDRIVER_OPENCLOSE
    cc = Ipl_InicDriverOpen();
#endif
    Ipl_Trace(IPL_TRACETAG_INFO, "INIC Programming Library %s", VERSIONTAG);
    Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_EnterProgMode called with ChipID 0x%02X", chipID);
    Ipl_ClrIpfData(&Ipl_IpfData);
    if (0U == cc)
    {
        res = Ipl_StartupInic(INICMODE_BOOT);
        if (IPL_RES_OK == res)
        {
            Ipl_ClrTel();
            Ipl_IplData.Tel[0] = CMD_PROGSTART;
            switch (chipID)
            {
                case IPL_CHIP_OS81118:
                case IPL_CHIP_OS81119:
                    Ipl_IplData.TelLen = CMD_PROGSTART_TXLEN;
                    break;
                case IPL_CHIP_OS81210:
                case IPL_CHIP_OS81212:
                case IPL_CHIP_OS81214:
                    Ipl_IplData.Tel[4] = 0x28U;
                    Ipl_IplData.Tel[5] = 0x1BU;
                    Ipl_IplData.Tel[6] = 0x6BU;
                    Ipl_IplData.Tel[7] = 0x95U;
                    Ipl_IplData.TelLen = 8U;
                    break;
                default:
                    res = IPL_RES_ERR_NOT_SUPPORTED;
                    break;
             }
             if (IPL_RES_OK == res) /*! \internal Jira UN-369, UN-370 */
             {
                 res = Ipl_ExecInicCmd();
                 if (IPL_RES_OK == res)
                 {
                     res = Ipl_ReadFirmwareVersion(); /* for later crosscheck of the IPF file */
                     res = IPL_RES_OK; /*! \internal Jira UN-376 */
                 }
             }
        }
    }
    Ipl_Trace(Ipl_TraceTag(res), "Ipl_EnterProgMode returned 0x%02X", res);
    return res;
}


/*! \internal Sends INIC back to normal mode. */
uint8_t Ipl_LeaveProgMode(void)
{
    uint8_t res;
    uint8_t cc;
    res = Ipl_StartupInic(INICMODE_NORMAL);
#ifdef IPL_INICDRIVER_OPENCLOSE
    if (IPL_RES_OK == res)
    {
        cc = Ipl_InicDriverClose();
        if (0U != cc)
        {
            res = IPL_RES_ERR_HW_INIC_COMM;
        }
    }
#endif
    Ipl_Trace(Ipl_TraceTag(res), "Ipl_LeaveProgMode returned 0x%02X", res);
    return res;
}


/*! \internal Executes the referred job by using the referred data. */
uint8_t Ipl_Prog(uint8_t job, uint32_t lData, uint8_t* pData)
{
    uint8_t res;
    Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_Prog called with Job 0x%02X", job);
    /* Jobs that can be used for OS81118 only */
    if (IPL_CHIP_OS81118 == Ipl_IplData.ChipID)
    {
        switch (job)
        {
            case IPL_JOB_READ_FIRMWARE_VER:
                res = Ipl_ReadFirmwareVersion();
                break;
            case IPL_JOB_CHK_UPDATE_CONFIGSTRING:
                res = Ipl_CheckUpdate(&Ipl_IpfData, lData, pData, STRINGTYPE_CS);
                break;
            case IPL_JOB_CHK_UPDATE_FIRMWARE:
                res = Ipl_CheckUpdate(&Ipl_IpfData, lData, pData, STRINGTYPE_FW);
                break;
            case IPL_JOB_PROG_CONFIG:
#ifdef IPL_USE_OS81118
                res = OS81118_ProgConfiguration(lData, pData);
#else
                res = IPL_RES_ERR_NOT_SUPPORTED;
#endif
                break;
            case IPL_JOB_PROG_FIRMWARE:
#ifdef IPL_USE_OS81118
                res = OS81118_ProgFirmware(lData, pData);
#else
                res = IPL_RES_ERR_NOT_SUPPORTED;
#endif
                break;
            case IPL_JOB_READ_CONFIGSTRING_VER:
#ifdef IPL_USE_OS81118
                res = OS81118_ReadConfigStringVersion(lData, pData);
#else
                res = IPL_RES_ERR_NOT_SUPPORTED;
#endif
                break;
            case IPL_JOB_PROG_PATCHSTRING:
            case IPL_JOB_PROG_TEST_PATCHSTRING:
            case IPL_JOB_PROG_CONFIGSTRING:
            case IPL_JOB_PROG_TEST_CONFIGSTRING:
            case IPL_JOB_PROG_IDENTSTRING:
            case IPL_JOB_PROG_TEST_IDENTSTRING:
            case IPL_JOB_PROG_TEST_CONFIG:
            default:
                res = IPL_RES_ERR_NOT_SUPPORTED;
                break;
        }
    }
    /* Jobs that can be used for OS81119 only */
    else if (IPL_CHIP_OS81119 == Ipl_IplData.ChipID)
    {
        switch (job)
        {
            case IPL_JOB_READ_FIRMWARE_VER:
                res = Ipl_ReadFirmwareVersion();
                break;
            case IPL_JOB_PROG_CONFIG:
#ifdef IPL_USE_OS81119
                res = OS81119_ProgConfiguration(lData, pData);
#else
                res = IPL_RES_ERR_NOT_SUPPORTED;
#endif
                break;
            case IPL_JOB_PROG_FIRMWARE:
#ifdef IPL_USE_OS81119
                res = OS81119_ProgFirmware(lData, pData);
#else
                res = IPL_RES_ERR_NOT_SUPPORTED;
#endif
                break;
            case IPL_JOB_READ_CONFIGSTRING_VER:
#ifdef IPL_USE_OS81119
                res = OS81119_ReadConfigStringVersion(lData, pData);
#else
                res = IPL_RES_ERR_NOT_SUPPORTED;
#endif
                break;
            case IPL_JOB_PROG_PATCHSTRING:
            case IPL_JOB_PROG_TEST_PATCHSTRING:
            case IPL_JOB_PROG_CONFIGSTRING:
            case IPL_JOB_PROG_TEST_CONFIGSTRING:
            case IPL_JOB_PROG_IDENTSTRING:
            case IPL_JOB_PROG_TEST_IDENTSTRING:
            case IPL_JOB_PROG_TEST_CONFIG:
            default:
                res = IPL_RES_ERR_NOT_SUPPORTED;
                break;
        }
    }
    /* Jobs that can be used for OS81210 only */
    else if (IPL_CHIP_OS81210 == Ipl_IplData.ChipID)
    {
        switch (job)
        {
            case IPL_JOB_READ_FIRMWARE_VER:
                res = Ipl_ReadFirmwareVersion();
                break;
            case IPL_JOB_PROG_CONFIGSTRING:
#ifdef IPL_USE_OS81210
                res = OS81210_ProgConfigString(lData, pData);
#else
                res = IPL_RES_ERR_NOT_SUPPORTED;
#endif
                break;
            case IPL_JOB_PROG_TEST_CONFIGSTRING:
#ifdef IPL_USE_OS81210
                res = OS81210_ProgTestConfigString(lData, pData);
#else
                res = IPL_RES_ERR_NOT_SUPPORTED;
#endif
                break;
            case IPL_JOB_PROG_IDENTSTRING:
#ifdef IPL_USE_OS81210
                res = OS81210_ProgIdentString(lData, pData);
#else
                res = IPL_RES_ERR_NOT_SUPPORTED;
#endif
                break;
            case IPL_JOB_PROG_TEST_IDENTSTRING:
#ifdef IPL_USE_OS81210
                res = OS81210_ProgTestIdentString(lData, pData);
#else
                res = IPL_RES_ERR_NOT_SUPPORTED;
#endif
                break;
            case IPL_JOB_PROG_CONFIG:
#ifdef IPL_USE_OS81210
                res = OS81210_ProgConfiguration(lData, pData);
#else
                res = IPL_RES_ERR_NOT_SUPPORTED;
#endif
                break;
            case IPL_JOB_PROG_TEST_CONFIG:
#ifdef IPL_USE_OS81210
                res = OS81210_ProgTestConfiguration(lData, pData);
#else
                res = IPL_RES_ERR_NOT_SUPPORTED;
#endif
                break;
            case IPL_JOB_PROG_PATCHSTRING:
#ifdef IPL_USE_OS81210
                res = OS81210_ProgPatchString(lData, pData);
#else
                res = IPL_RES_ERR_NOT_SUPPORTED;
#endif
                break;
            case IPL_JOB_PROG_TEST_PATCHSTRING:
#ifdef IPL_USE_OS81210
                res = OS81210_ProgTestPatchString(lData, pData);
#else
                res = IPL_RES_ERR_NOT_SUPPORTED;
#endif
                break;
            case IPL_JOB_READ_CONFIGSTRING_VER:
#ifdef IPL_USE_OS81210
                res = OS81210_ReadConfigStringVersion(lData, pData);
#else
                res = IPL_RES_ERR_NOT_SUPPORTED;
#endif
                break;
            case IPL_JOB_PROG_FIRMWARE:
            default:
                res = IPL_RES_ERR_NOT_SUPPORTED;
                break;
        }
    }
    /* Jobs that can be used for OS81212 only */
    else if (IPL_CHIP_OS81212 == Ipl_IplData.ChipID)
    {
        switch (job)
        {
            case IPL_JOB_READ_FIRMWARE_VER:
                res = Ipl_ReadFirmwareVersion();
                break;
            case IPL_JOB_PROG_CONFIGSTRING:
#ifdef IPL_USE_OS81212
                res = OS81212_ProgConfigString(lData, pData);
#else
                res = IPL_RES_ERR_NOT_SUPPORTED;
#endif
                break;
            case IPL_JOB_PROG_TEST_CONFIGSTRING:
#ifdef IPL_USE_OS81212
                res = OS81212_ProgTestConfigString(lData, pData);
#else
                res = IPL_RES_ERR_NOT_SUPPORTED;
#endif
                break;
            case IPL_JOB_PROG_IDENTSTRING:
#ifdef IPL_USE_OS81212
                res = OS81212_ProgIdentString(lData, pData);
#else
                res = IPL_RES_ERR_NOT_SUPPORTED;
#endif
                break;
            case IPL_JOB_PROG_TEST_IDENTSTRING:
#ifdef IPL_USE_OS81212
                res = OS81212_ProgTestIdentString(lData, pData);
#else
                res = IPL_RES_ERR_NOT_SUPPORTED;
#endif
                break;
            case IPL_JOB_PROG_CONFIG:
#ifdef IPL_USE_OS81212
                res = OS81212_ProgConfiguration(lData, pData);
#else
                res = IPL_RES_ERR_NOT_SUPPORTED;
#endif
                break;
            case IPL_JOB_PROG_TEST_CONFIG:
#ifdef IPL_USE_OS81212
                res = OS81212_ProgTestConfiguration(lData, pData);
#else
                res = IPL_RES_ERR_NOT_SUPPORTED;
#endif
                break;
            case IPL_JOB_PROG_PATCHSTRING:
#ifdef IPL_USE_OS81212
                res = OS81212_ProgPatchString(lData, pData);
#else
                res = IPL_RES_ERR_NOT_SUPPORTED;
#endif
                break;
            case IPL_JOB_PROG_TEST_PATCHSTRING:
#ifdef IPL_USE_OS81212
                res = OS81212_ProgTestPatchString(lData, pData);
#else
                res = IPL_RES_ERR_NOT_SUPPORTED;
#endif
                break;
            case IPL_JOB_READ_CONFIGSTRING_VER:
#ifdef IPL_USE_OS81212
                res = OS81212_ReadConfigStringVersion(lData, pData);
#else
                res = IPL_RES_ERR_NOT_SUPPORTED;
#endif
                break;
            case IPL_JOB_PROG_FIRMWARE:
            default:
                res = IPL_RES_ERR_NOT_SUPPORTED;
                break;
        }
    }
    /* Jobs that can be used for OS81214 only */
    else if (IPL_CHIP_OS81214 == Ipl_IplData.ChipID)
    {
        switch (job)
        {
            case IPL_JOB_READ_FIRMWARE_VER:
                res = Ipl_ReadFirmwareVersion();
                break;
            case IPL_JOB_PROG_CONFIGSTRING:
#ifdef IPL_USE_OS81214
                res = OS81214_ProgConfigString(lData, pData);
#else
                res = IPL_RES_ERR_NOT_SUPPORTED;
#endif
                break;
            case IPL_JOB_PROG_TEST_CONFIGSTRING:
#ifdef IPL_USE_OS81214
                res = OS81214_ProgTestConfigString(lData, pData);
#else
                res = IPL_RES_ERR_NOT_SUPPORTED;
#endif
                break;
            case IPL_JOB_PROG_IDENTSTRING:
#ifdef IPL_USE_OS81214
                res = OS81214_ProgIdentString(lData, pData);
#else
                res = IPL_RES_ERR_NOT_SUPPORTED;
#endif
                break;
            case IPL_JOB_PROG_TEST_IDENTSTRING:
#ifdef IPL_USE_OS81214
                res = OS81214_ProgTestIdentString(lData, pData);
#else
                res = IPL_RES_ERR_NOT_SUPPORTED;
#endif
                break;
            case IPL_JOB_PROG_CONFIG:
#ifdef IPL_USE_OS81214
                res = OS81214_ProgConfiguration(lData, pData);
#else
                res = IPL_RES_ERR_NOT_SUPPORTED;
#endif
                break;
            case IPL_JOB_PROG_TEST_CONFIG:
#ifdef IPL_USE_OS81214
                res = OS81214_ProgTestConfiguration(lData, pData);
#else
                res = IPL_RES_ERR_NOT_SUPPORTED;
#endif
                break;
            case IPL_JOB_PROG_PATCHSTRING:
#ifdef IPL_USE_OS81214
                res = OS81214_ProgPatchString(lData, pData);
#else
                res = IPL_RES_ERR_NOT_SUPPORTED;
#endif
                break;
            case IPL_JOB_PROG_TEST_PATCHSTRING:
#ifdef IPL_USE_OS81214
                res = OS81214_ProgTestPatchString(lData, pData);
#else
                res = IPL_RES_ERR_NOT_SUPPORTED;
#endif
                break;
            case IPL_JOB_READ_CONFIGSTRING_VER:
#ifdef IPL_USE_OS81214
                res = OS81214_ReadConfigStringVersion(lData, pData);
#else
                res = IPL_RES_ERR_NOT_SUPPORTED;
#endif
                break;
            case IPL_JOB_PROG_FIRMWARE:
            default:
                res = IPL_RES_ERR_NOT_SUPPORTED;
                break;
        }
    }
    else
    {
        res = IPL_RES_ERR_NOT_SUPPORTED; /*! \internal Jira UN-371, UN-372 */
    }
    Ipl_Trace(Ipl_TraceTag(res), "Ipl_Prog returned 0x%02X", res);
    return res;
}


/*! \internal Reads the firmware version from INIC. INIC needs to be in programming mode. */
static uint8_t Ipl_ReadFirmwareVersion(void)
{
    uint8_t res;
    Ipl_ClrTel();
    Ipl_IplData.Tel[0] = CMD_READFWVER;
    Ipl_IplData.TelLen = CMD_READFWVER_TXLEN;
    res = Ipl_ExecInicCmd();
    if (IPL_RES_OK == res)
    {
        Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_ReadFirmwareVersion returned 0x%02X - V%u.%u.%u-%u", res,
                Ipl_InicData.FwMajorVersion, Ipl_InicData.FwMinorVersion, Ipl_InicData.FwReleaseVersion, Ipl_InicData.FwBuildVersion);
    }
    else
    {
        Ipl_InicData.FwVersionValid   = VERSION_INVALID;
        Ipl_InicData.ChipID           = DEFAULTVAL_UINT32; /*! \internal Jira UN-376 */
        Ipl_InicData.FwMajorVersion   = DEFAULTVAL_UINT8;  /*! \internal Jira UN-376 */
        Ipl_InicData.FwMinorVersion   = DEFAULTVAL_UINT8;  /*! \internal Jira UN-376 */
        Ipl_InicData.FwReleaseVersion = DEFAULTVAL_UINT8;  /*! \internal Jira UN-376 */
        Ipl_InicData.FwBuildVersion   = DEFAULTVAL_UINT32; /*! \internal Jira UN-376 */
        Ipl_InicData.FwCrc            = DEFAULTVAL_UINT16; /*! \internal Jira UN-376 */
        Ipl_Trace(Ipl_TraceTag(res), "Ipl_ReadFirmwareVersion returned 0x%02X", res);
    }
    return res;
}


/*! \internal Sends a command to INIC and reads back the result. */
uint8_t Ipl_ExecInicCmd(void)
{
    uint8_t rxlen, cmd, cc, rw;
    uint8_t res = IPL_RES_ERR_TXTELLEN_INVALID;
    Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_ExecInicCmd called with Command 0x%02X", Ipl_IplData.Tel[0]);
    /* Send telegram */
    if ((INIC_MAX_TELLEN >= Ipl_IplData.TelLen) && (Ipl_IplData.TelLen != 0U))
    {
        rw = Ipl_InicWrite(Ipl_IplData.TelLen, &Ipl_IplData.Tel[0]);
        if (0U == rw)
        {
            Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_InicWrite returned 0x%02X", rw);
        }
        else
        {
            Ipl_Trace(IPL_TRACETAG_ERR, "Ipl_InicWrite returned 0x%02X", rw);
        }
        res = IPL_RES_ERR_WRITE;
        if (0U == rw)
        {
            res = IPL_RES_OK;
            Ipl_TraceTel(DIR_TX);
            /* Read response, store command and reset Buffer for RX */
            cmd = Ipl_IplData.Tel[0];
            res = Ipl_WaitForResponse();
            if (IPL_RES_OK == res)
            {
                Ipl_ClrTel();
                switch (cmd)
                {
                    case CMD_PROGSTART:
                        rxlen = CMD_PROGSTART_RXLEN;
                        break;
                    case CMD_READFWVER:
                        rxlen = CMD_READFWVER_RXLEN;
                        break;
                    case CMD_READINFOMEM:
                        rxlen = CMD_READINFOMEM_RXLEN;
                        break;
                    case CMD_CLEARCRC:
                        rxlen = CMD_CLEARCRC_RXLEN;
                        break;
                    case CMD_ERASEINFOMEM:
                        rxlen = CMD_ERASEINFOMEM_RXLEN;
                        break;
                    case CMD_ERASEPROGMEM:
                        rxlen = CMD_ERASEPROGMEM_RXLEN;
                        break;
                    case CMD_GETCRC:
                        rxlen = CMD_GETCRC_RXLEN;
                        break;
                    case CMD_READOTPMEM:
                        rxlen = CMD_READOTPMEM_RXLEN;
                        break;
                    case CMD_READPROGMEM:
                        rxlen = CMD_READPROGMEM_RXLEN;
                        break;
                    case CMD_SETPROGMEMPAGE:
                        rxlen = CMD_SETPROGMEMPAGE_RXLEN;
                        break;
                    case CMD_VERIFYINFOMEM:
                        rxlen = CMD_VERIFYINFOMEM_RXLEN;
                        break;
                    case CMD_VERIFYOTPMEM:
                        rxlen = CMD_VERIFYOTPMEM_RXLEN;
                        break;
                    case CMD_WRITEINFOMEM:
                        rxlen = CMD_WRITEINFOMEM_RXLEN;
                        break;
                    case CMD_WRITEOTPMEM:
                        rxlen = CMD_WRITEOTPMEM_RXLEN;
                        break;
                    case CMD_WRITEPROGMEM:
                        rxlen = CMD_WRITEPROGMEM_RXLEN;
                        break;
                    case CMD_WRITETESTMEM:
                        rxlen = CMD_WRITETESTMEM_RXLEN;
                        break;
                    default:
                        res = IPL_RES_ERR_CMD_UNEXPECTED;
                        break;
                }
                if (IPL_RES_OK == res)
                {
                    rw = Ipl_InicRead(rxlen, &Ipl_IplData.Tel[0]);
                    if (0U == rw)
                    {
                        Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_InicRead returned 0x%02X", rw);
                    }
                    else
                    {
                        Ipl_Trace(IPL_TRACETAG_ERR, "Ipl_InicRead returned 0x%02X", rw);
                    }
                    res = IPL_RES_ERR_READ;
                    if (0x00U == rw)
                    {
                        Ipl_IplData.TelLen = rxlen;
                        Ipl_TraceTel(DIR_RX);
                        /* Parse response */
                        cc = Ipl_IplData.Tel[0];
                        Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_ExecInicCmd read CC 0x%02X", cc);
                        if (IPL_RES_CC_OK == cc)
                        {
                            res = IPL_RES_OK;
                            switch (cmd)
                            {
                                case CMD_READFWVER:
                                    Ipl_InicData.ChipID           =  ((uint32_t) Ipl_IplData.Tel[6]) << 24U;
                                    Ipl_InicData.ChipID           += ((uint32_t) Ipl_IplData.Tel[7]) << 16U;
                                    Ipl_InicData.ChipID           += ((uint32_t) Ipl_IplData.Tel[8]) <<  8U;
                                    Ipl_InicData.ChipID           += ((uint32_t) Ipl_IplData.Tel[9]) & 0xFFU;
                                    Ipl_InicData.FwMajorVersion   =  Ipl_IplData.Tel[10];
                                    Ipl_InicData.FwMinorVersion   =  Ipl_IplData.Tel[11];
                                    Ipl_InicData.FwReleaseVersion =  Ipl_IplData.Tel[12];
                                    Ipl_InicData.FwBuildVersion   =  ((uint32_t) Ipl_IplData.Tel[13]) << 24U;
                                    Ipl_InicData.FwBuildVersion   += ((uint32_t) Ipl_IplData.Tel[14]) << 16U;
                                    Ipl_InicData.FwBuildVersion   += ((uint32_t) Ipl_IplData.Tel[15]) <<  8U;
                                    Ipl_InicData.FwBuildVersion   += ((uint32_t) Ipl_IplData.Tel[16]) & 0xFFU;
                                    Ipl_InicData.FwCrc            =  Ipl_IplData.Tel[19];
                                    Ipl_InicData.FwVersionValid   =  VERSION_VALID;
                                    break;
                                case CMD_PROGSTART:
                                case CMD_READINFOMEM:
                                case CMD_CLEARCRC:
                                case CMD_ERASEINFOMEM:
                                case CMD_ERASEPROGMEM:
                                case CMD_GETCRC:
                                case CMD_READOTPMEM:
                                case CMD_READPROGMEM:
                                case CMD_SETPROGMEMPAGE:
                                case CMD_VERIFYINFOMEM:
                                case CMD_VERIFYOTPMEM:
                                case CMD_WRITEINFOMEM:
                                case CMD_WRITEOTPMEM:
                                case CMD_WRITEPROGMEM:
                                case CMD_WRITETESTMEM:
                                    break;
                                default:
                                    res = IPL_RES_ERR_RESP_UNEXPECTED;
                                    break;
                            }
                        }
                        else
                        {
                            switch (cmd)
                            {
                                case CMD_READFWVER:
                                    Ipl_InicData.FwVersionValid = VERSION_INVALID;
                                    res = IPL_RES_ERR_READFWVER;
                                    break;
                                case CMD_PROGSTART:
                                    res = IPL_RES_ERR_PROGSTART;
                                    break;
                                case CMD_READINFOMEM:
                                    res = IPL_RES_ERR_READINFOMEM;
                                    break;
                                case CMD_CLEARCRC:
                                    res = IPL_RES_ERR_CLEARCRC;
                                    break;
                                case CMD_ERASEINFOMEM:
                                    res = IPL_RES_ERR_ERASEINFOMEM;
                                    break;
                                case CMD_ERASEPROGMEM:
                                    res = IPL_RES_ERR_ERASEPROGMEM;
                                    break;
                                case CMD_GETCRC:
                                    res = IPL_RES_ERR_GETCRC;
                                    break;
                                case CMD_READOTPMEM:
                                    res = IPL_RES_ERR_READOTPMEM;
                                    break;
                                case CMD_READPROGMEM:
                                    res = IPL_RES_ERR_READPROGMEM;
                                    break;
                                case CMD_SETPROGMEMPAGE:
                                    res = IPL_RES_ERR_SETPROGMEMPAGE;
                                    break;
                                case CMD_VERIFYINFOMEM:
                                    res = IPL_RES_ERR_VERIFYINFOMEM;
                                    break;
                                case CMD_VERIFYOTPMEM:
                                    res = cc;
                                    break;
                                case CMD_WRITEINFOMEM:
                                    res = IPL_RES_ERR_WRITEINFOMEM;
                                    break;
                                case CMD_WRITEOTPMEM:
                                    res = IPL_RES_ERR_WRITEOTPMEM;
                                    break;
                                case CMD_WRITEPROGMEM:
                                    res = IPL_RES_ERR_WRITEPROGMEM;
                                    break;
                                case CMD_WRITETESTMEM:
                                    res = IPL_RES_ERR_WRITETESTMEM;
                                    break;
                                default:
                                    res = IPL_RES_ERR_RESP_UNEXPECTED;
                                    break;
                            }
                        }
                    }
                }
            }
        }
    }
    Ipl_Trace(Ipl_TraceTag(res), "Ipl_ExecInicCmd returned 0x%02X",res);
    return res;
}

/*! \internal Checks if the ChipID of the IPF data is equal to the connected INIC */
uint8_t Ipl_CheckChipId(void)
{
    uint8_t res = IPL_RES_ERR_IPF_WRONGINIC;
    switch (Ipl_InicData.ChipID)
    {
        case 0x81118U:
            if (IPL_CHIP_OS81118 == Ipl_IpfData.ChipID)
            {
                res = IPL_RES_OK;
            }
            break;
        case 0x81119U:
            if (IPL_CHIP_OS81119 == Ipl_IpfData.ChipID)
            {
                res = IPL_RES_OK;
            }
            break;
        case 0x81210U:
            if (IPL_CHIP_OS81210 == Ipl_IpfData.ChipID)
            {
                res = IPL_RES_OK;
            }
            break;
        case 0x81212U:
            if (IPL_CHIP_OS81212 == Ipl_IpfData.ChipID)
            {
                res = IPL_RES_OK;
            }
            break;
        case 0x81214U:
            if (IPL_CHIP_OS81214 == Ipl_IpfData.ChipID)
            {
                res = IPL_RES_OK;
            }
            break;
        default: /* No valid ChipID could be determined */                  /*! \internal Jira UN-376 */
            switch (Ipl_IpfData.ChipID)                                     /*! \internal Jira UN-376 */
            {                                                               /*! \internal Jira UN-376 */
                case IPL_CHIP_OS81118:                                      /*! \internal Jira UN-376 */
                case IPL_CHIP_OS81119:                                      /*! \internal Jira UN-376 */
                    /* If we assume a Flash Chip, we can try programming */ /*! \internal Jira UN-376 */
                    res = IPL_RES_OK;                                       /*! \internal Jira UN-376 */
                    break;                                                  /*! \internal Jira UN-376 */
                default:                                                    /*! \internal Jira UN-376 */
                    res = IPL_RES_ERR_NOVALIDCHIPID;                        /*! \internal Jira UN-376 */
                    /* If we assume a ROM chip, we do not touch it */       /*! \internal Jira UN-376 */
                    break;                                                  /*! \internal Jira UN-376 */
            }                                                               /*! \internal Jira UN-376 */
            break;                                                          /*! \internal Jira UN-376 */
    }
    Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_CheckChipId IPF ChipID 0x%X", Ipl_IpfData.ChipID);
    Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_CheckChipId connected INIC with ChipID 0x%X", Ipl_InicData.ChipID);
    Ipl_Trace(Ipl_TraceTag(res), "Ipl_CheckChipId returned 0x%02X", res);
    return res;
}


/*! \internal Checks, if the FW Version stored in the IPF data is equal to the FW version in the connected INIC */
uint8_t Ipl_CheckInicFwVersion(void)
{
    uint8_t res;
    res = Ipl_CheckChipId();
    if (IPL_RES_OK == res)
    {
        /* additional version check */
        Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_CheckInicFwVersion FW IPF Meta - V%u.%u.%u-%u",
                Ipl_IpfData.Meta.FwMajorVersion, Ipl_IpfData.Meta.FwMinorVersion, Ipl_IpfData.Meta.FwReleaseVersion, Ipl_IpfData.Meta.FwBuildVersion);
        Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_CheckInicFwVersion FW connected INIC V%u.%u.%u-%u",
                Ipl_InicData.FwMajorVersion, Ipl_InicData.FwMinorVersion, Ipl_InicData.FwReleaseVersion, Ipl_InicData.FwBuildVersion);

        if ((Ipl_IpfData.Meta.FwMajorVersion   != Ipl_InicData.FwMajorVersion)   ||
            (Ipl_IpfData.Meta.FwMinorVersion   != Ipl_InicData.FwMinorVersion)   ||
            (Ipl_IpfData.Meta.FwReleaseVersion != Ipl_InicData.FwReleaseVersion) ||
            (Ipl_IpfData.Meta.FwBuildVersion   != Ipl_InicData.FwBuildVersion))     /* Connected Version is different from Version in IPF Meta Info */
        {

            if ((Ipl_IpfData.Meta.FwMajorVersion   != DEFAULTVAL_UINT8)   ||
                (Ipl_IpfData.Meta.FwMinorVersion   != DEFAULTVAL_UINT8)   ||
                (Ipl_IpfData.Meta.FwReleaseVersion != DEFAULTVAL_UINT8)   ||
                (Ipl_IpfData.Meta.FwBuildVersion   != DEFAULTVAL_UINT32))    /* IPF Meta Version is not default */
            {

                res = IPL_RES_ERR_IPF_WRONGFWVERSION;
            }
        }
    }
    Ipl_Trace(Ipl_TraceTag(res), "Ipl_CheckInicFwVersion returned 0x%02X", res);
    return res;
}


/*! \internal Checks, if update is useful or not */
static uint8_t Ipl_CheckUpdate(Ipl_IpfData_t *ipf, uint32_t lData, uint8_t pData[], uint8_t stringType)
{
    uint8_t  res;
    uint32_t vinic, vipf;

    Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_CheckUpdate called with %u byte IPF, StringType 0x%02X", lData, stringType);
    /* Check if correct string is contained in IPF data */
    res = Ipl_ParseIpf(&Ipl_IpfData, lData, pData, stringType);
    if (IPL_RES_OK == res)
    {
        /* Get IPF versions from metadata */
        res = Ipl_ParseIpf(&Ipl_IpfData, lData, pData, STRINGTYPE_META);
        if ((STRINGTYPE_CONFIG == stringType) || (STRINGTYPE_CS == stringType))
        {
            /* If any version is not valid */
            if ((VERSION_INVALID == Ipl_IpfData.Meta.CfgsVersionValid) || (VERSION_INVALID == Ipl_InicData.CfgsVersionValid))
            {
                res = IPL_RES_UPDATE_DENIED_UNKNOWN;
                /* If version in INIC is not valid */
                if (VERSION_INVALID == Ipl_InicData.CfgsVersionValid)
                {
                    res = IPL_RES_OK; /* Update */
                }
            }
            else /* All versions are valid, we can compare them */
            {
                vipf  =   (uint32_t) ( (uint32_t) Ipl_IpfData.Meta.CfgsCustMajorVersion << 16U )
                        + (uint32_t) ( (uint32_t) Ipl_IpfData.Meta.CfgsCustMinorVersion << 8U )
                        + (uint32_t) Ipl_IpfData.Meta.CfgsCustReleaseVersion;
                vinic =   (uint32_t) ( (uint32_t) Ipl_InicData.CfgsCustMajorVersion << 16U )
                        + (uint32_t) ( (uint32_t) Ipl_InicData.CfgsCustMinorVersion << 8U )
                        + (uint32_t) Ipl_InicData.CfgsCustReleaseVersion;

                if (vipf > vinic)
                {
                    res = IPL_RES_OK;
                }
                else if (vipf == vinic)
                {
                    res = IPL_RES_UPDATE_DENIED_EQUAL;
                }
                else
                {
                    res = IPL_RES_UPDATE_DENIED_NEWER;
                }
                /* Check finally, if CS fits to FW */
                if (IPL_RES_OK == res)
                {
                    res = Ipl_CheckInicFwVersion();
                }
            }
            Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_CheckUpdate ConfigString INIC: V%u.%u.%u IPF: V%u.%u.%u",
                Ipl_InicData.CfgsCustMajorVersion, Ipl_InicData.CfgsCustMinorVersion, Ipl_InicData.CfgsCustReleaseVersion,
                Ipl_IpfData.Meta.CfgsCustMajorVersion, Ipl_IpfData.Meta.CfgsCustMinorVersion, Ipl_IpfData.Meta.CfgsCustReleaseVersion);
        }
        else if  (STRINGTYPE_FW == stringType)
        {
            /* If any version is not valid */
            if ((VERSION_INVALID == Ipl_IpfData.Meta.FwVersionValid) || (VERSION_INVALID == Ipl_InicData.FwVersionValid))
            {
                res = IPL_RES_UPDATE_DENIED_UNKNOWN;
                /* If version in INIC is not valid */
                if (VERSION_INVALID == Ipl_InicData.FwVersionValid)
                {
                    res = IPL_RES_OK; /* Update */
                }
            }
            else /* All versions are valid, we can compare them */
            {
                vipf  =   (uint32_t) ( (uint32_t) Ipl_IpfData.Meta.FwMajorVersion << 16U )
                        + (uint32_t) ( (uint32_t) Ipl_IpfData.Meta.FwMinorVersion << 8U )
                        + (uint32_t)  Ipl_IpfData.Meta.FwReleaseVersion;
                vinic =   (uint32_t) ( (uint32_t) Ipl_InicData.FwMajorVersion << 16U )
                        + (uint32_t) ( (uint32_t) Ipl_InicData.FwMinorVersion << 8U )
                        + (uint32_t) Ipl_InicData.FwReleaseVersion;
                if (vipf > vinic)
                {
                    res = IPL_RES_OK;
                }
                else if (vipf == vinic)
                {
                    if (Ipl_IpfData.Meta.FwBuildVersion > Ipl_InicData.FwBuildVersion)
                    {
                        res = IPL_RES_OK;
                    }
                    else if (Ipl_IpfData.Meta.FwBuildVersion == Ipl_InicData.FwBuildVersion)
                    {
                        res = IPL_RES_UPDATE_DENIED_EQUAL;
                    }
                    else
                    {
                        res = IPL_RES_UPDATE_DENIED_NEWER;
                    }
                }
                else
                {
                    res = IPL_RES_UPDATE_DENIED_NEWER;
                }
            }
            Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_CheckUpdate Firmware INIC: V%u.%u.%u-%u IPF: V%u.%u.%u-%u",
                Ipl_InicData.FwMajorVersion, Ipl_InicData.FwMinorVersion, Ipl_InicData.FwReleaseVersion, Ipl_InicData.FwBuildVersion,
                Ipl_IpfData.Meta.FwMajorVersion, Ipl_IpfData.Meta.FwMinorVersion, Ipl_IpfData.Meta.FwReleaseVersion, Ipl_IpfData.Meta.FwBuildVersion);
        }
    }
    Ipl_Trace(Ipl_TraceTag(res), "Ipl_CheckUpdate returned 0x%02X", res);
    return res;
}


/*! \internal Starts up INIC either in boot mode or normal mode. */
static uint8_t Ipl_StartupInic(uint8_t chipMode)
{
    uint8_t res = IPL_RES_ERR_HW_INIC_PINS;
    uint8_t pin;
    pin = Ipl_SetResetPin(IPL_LOW);
    if (0U == pin)
    {
        Ipl_Sleep(INIC_PIN_WAIT_TIME);
        if (INICMODE_BOOT == chipMode)
        {
            pin = Ipl_SetErrBootPin(IPL_LOW);
        }
        else
        {
            pin = Ipl_SetErrBootPin(IPL_HIGH);
        }
        if (0U == pin)
        {
            Ipl_Sleep(INIC_PIN_WAIT_TIME);
            pin = Ipl_SetResetPin(IPL_HIGH);
            if (0U == pin)
            {
                Ipl_Sleep(INIC_PIN_WAIT_TIME);
                Ipl_Sleep(INIC_BOOTUP_TIME);
                pin = Ipl_SetErrBootPin(IPL_HIGH);
                if (0U == pin)
                {
                    res = IPL_RES_OK;
                }
            }
        }
    }
    if (IPL_RES_OK == res)
    {
        Ipl_IplData.ChipMode = chipMode;
    }
    Ipl_Trace(Ipl_TraceTag(res), "Ipl_StartupInic with mode 0x%02X returned 0x%02X", chipMode, res);
    return res;
}


/*! \internal Clears RX and TX telegram buffer. */
void Ipl_ClrTel(void)
{
    uint8_t i;
    Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_ClrTel");
    for (i=0U; i<INIC_MAX_TELLEN; i++)
    {
        Ipl_IplData.Tel[i] = 0x00U;
    }
}


/*! \internal Waits for some dedicated time or the pulling of the INT pin. */
static uint8_t Ipl_WaitForResponse(void)
{
    uint8_t res = IPL_RES_OK;
    int32_t waittime;
    Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_WaitForResponse called");
    switch (Ipl_IplData.Tel[0])
    {
        case CMD_WRITEPROGMEM:
            waittime = (int32_t) INIC_PROGRAM_WAIT_TIME;
            break;
        case CMD_ERASEPROGMEM:
            waittime = (int32_t) INIC_ERASEPROGMEM_WAIT_TIME;
            break;
        case CMD_ERASEINFOMEM:
            waittime = (int32_t) INIC_ERASEINFOMEM_WAIT_TIME;
            break;
        default:
            waittime = (int32_t) INIC_RESP_WAIT_TIME;
            break;
    }
#ifdef IPL_USE_INTPIN
    if ( (int32_t) INIC_INT_WAIT_TIMEOUT > waittime )
    {
        waittime = (int32_t) INIC_INT_WAIT_TIMEOUT;
    }
    res = Ipl_WaitForInt((uint16_t) waittime);
    waittime -= (int32_t) Ipl_IplData.IntTime;
#endif
    if (0 < waittime)
    {
        Ipl_Sleep((uint16_t) waittime);
        Ipl_Trace(Ipl_TraceTag(res), "Ipl_WaitForResponse returned 0x%02X after sleeping for %u ms", res, waittime);
    }
    else
    {
        Ipl_Trace(Ipl_TraceTag(res), "Ipl_WaitForResponse returned 0x%02X", res);
    }
    return res;
}


#ifdef IPL_USE_INTPIN
/*! \internal Waits until INT pin goes low or timeout. */
uint8_t Ipl_WaitForInt(uint16_t timeout)
{
    uint8_t  pin;
    uint8_t  res = IPL_RES_OK;
    uint16_t wtime = 0U;
    Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_WaitForInt called");
    pin = Ipl_GetIntPin();
    Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_WaitForInt - Ipl_GetIntPin returned 0x%02X ", pin);
    /* Wait until INT goes low or timeout or error */
    while ((1U == pin) && (wtime < timeout))
    {
        Ipl_Sleep(1U);
        wtime++;
        pin = Ipl_GetIntPin();
        Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_WaitForInt - Ipl_GetIntPin returned 0x%02X ", pin);
    }
    switch (pin)
    {
        case 0U:
            res = IPL_RES_OK;
            break;
        case 1U:
            res = IPL_RES_ERR_INT_TIMEOUT;
            break;
        default:
            res = IPL_RES_ERR_INT_READ;
            break;
    }
    Ipl_IplData.IntTime = wtime;
    Ipl_Trace(Ipl_TraceTag(res), "Ipl_WaitForInt returned 0x%02X after %u ms", res, wtime);
    return res;
}
#endif


/*! \internal Calls Progress indicator function when appropriate. */
void Ipl_ProgressIndicator(uint32_t val, uint32_t fval)
{
#ifdef IPL_PROGRESS_INDICATOR
    static uint8_t prg_old = 110U;
    uint8_t        prg, res;
    if ((1U == val) && (1U == fval))
    {
        Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_Progress called with 100%%");
        Ipl_Progress(100U);
    }
    else if ((0U == val) && (0U == fval))
    {
        Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_Progress called with 0%%");
        Ipl_Progress(0U);
    }
    else
    {
        prg = (uint8_t) ((20U*val) / fval);
        if (prg != prg_old)
        {
            prg_old = prg;
            res = (uint8_t) (prg * 5U);
            Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_Progress called with %u%% (%u of %u)", res, val, fval);
            Ipl_Progress(res);
        }
    }
#endif
}


/*------------------------------------------------------------------------------------------------*/
/* DATA CHUNK HANDLING                                                                            */
/*------------------------------------------------------------------------------------------------*/

/*! \internal Function to refer to a dedicated data position by an application buffer. */
uint8_t Ipl_PData(uint32_t index, uint32_t lData, uint8_t pData[])
{
    uint8_t res;
#if IPL_DATACHUNK_SIZE > 0
    if ( ( index >= Ipl_IplData.ChunkOffset ) && ( index < ( Ipl_IplData.ChunkOffset + (uint32_t) IPL_DATACHUNK_SIZE ) ) )
    {
        res = Ipl_IplData.pData[ (index - Ipl_IplData.ChunkOffset) ];
    }
    else
    {
        Ipl_IplData.ChunkOffset = (index / ( (uint32_t) IPL_DATACHUNK_SIZE ) ) * (uint32_t) IPL_DATACHUNK_SIZE;
        Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_PData requested new DataChunk, Index %u, Offset %u, Size %u",
                  index, Ipl_IplData.ChunkOffset, IPL_DATACHUNK_SIZE);
        Ipl_IplData.pData = Ipl_ProvideDataChunk(Ipl_IplData.ChunkOffset, (uint32_t) IPL_DATACHUNK_SIZE);
        res = Ipl_IplData.pData[index-Ipl_IplData.ChunkOffset];
    }
#else
    res = pData[index];
#endif
    return res;
}


/*! \internal Sets buffer back to the first data chunk. */
uint8_t Ipl_ClrPData(uint32_t lData, uint8_t pData[])
{
    uint8_t res = IPL_RES_OK;
#if IPL_DATACHUNK_SIZE > 0
    if (pData != Ipl_IplData.pData)
    {
        Ipl_IplData.ChunkOffset = 0U;
        Ipl_IplData.pData       = Ipl_ProvideDataChunk(Ipl_IplData.ChunkOffset, (uint32_t) IPL_DATACHUNK_SIZE);
    }
    if (Ipl_IplData.pData == NULL)
    {
        res = IPL_RES_ERR_INVALID_DATACHUNK;
    }
#else
    if (pData == NULL)
    {
        res = IPL_RES_ERR_INVALID_DATACHUNK;
    }
#endif
    Ipl_Trace(Ipl_TraceTag(res), "Ipl_ClrPData returned 0x%02X", res);
    return res;
}


/*------------------------------------------------------------------------------------------------*/
/* TRACE LOGGING                                                                                  */
/*------------------------------------------------------------------------------------------------*/

/*! \internal Traces the current telegram buffer. */
static void Ipl_TraceTel(uint8_t direction)
{
#ifdef IPL_TRACETAG_COM
    uint8_t i;
    char    line [TRACELINE_MAXLEN];
    if (DIR_TX == direction)
    {
        line[0U] = 'T';
    }
    else
    {
        line[0U] = 'R';
    }
    line[1U] = 'X';
    line[2U] = ' ';
    for (i=0U; i<Ipl_IplData.TelLen; i++)
    {
        line[3U+(i*3U)] = Ipl_BcdChr(Ipl_IplData.Tel[i], IPL_HIGH);
        line[4U+(i*3U)] = Ipl_BcdChr(Ipl_IplData.Tel[i], IPL_LOW);
        line[5U+(i*3U)] = ' ';
    }
    line[3U+(Ipl_IplData.TelLen*3U)] = '\0';
    Ipl_Trace(IPL_TRACETAG_COM, line);
#endif
}


/*! \internal Returns a nible in hex format. */
char Ipl_BcdChr(uint8_t val, uint8_t lowHigh)
{
    uint8_t nib;
    char    res;
    if (IPL_HIGH == lowHigh)
    {
        nib = (val & 0xF0U) >> 4U;
    }
    else
    {
        nib = val & 0x0FU;
    }
    switch (nib)
    {
        case 0x01U:
            res ='1';
            break;
        case 0x02U:
            res ='2';
            break;
        case 0x03U:
            res ='3';
            break;
        case 0x04U:
            res ='4';
            break;
        case 0x05U:
            res ='5';
            break;
        case 0x06U:
            res ='6';
            break;
        case 0x07U:
            res ='7';
            break;
        case 0x08U:
            res ='8';
            break;
        case 0x09U:
            res ='9';
            break;
        case 0x0AU:
            res ='A';
            break;
        case 0x0BU:
            res ='B';
            break;
        case 0x0CU:
            res ='C';
            break;
        case 0x0DU:
            res ='D';
            break;
        case 0x0EU:
            res ='E';
            break;
        case 0x0FU:
            res ='F';
            break;
        default:
            res ='0';
            break;
    }
    return res;
}


/*! \internal Provides a trace tag depending on the result code. */
char* Ipl_TraceTag(uint8_t result)
{
    char *tag;
    if (IPL_RES_OK == result)
    {
        tag = IPL_TRACETAG_INFO;
    }
    else
    {
        tag = IPL_TRACETAG_ERR;
    }
    return tag;
}
