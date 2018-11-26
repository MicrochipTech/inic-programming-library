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

/*! \file   ipf.h
 *  \brief  Internal header for functions to parse IPF data for INIC Programming Library
 *  \author Roland Trissl (RTR)
 *  \note   For support related to this code contact http://www.microchip.com/support.
 */

#ifndef IPF_H
#define IPF_H

#include <stdint.h>
#include "ipl.h"


/*------------------------------------------------------------------------------------------------*/
/* IPF RELATED VALUES                                                                             */
/*------------------------------------------------------------------------------------------------*/

#define STRINGTYPE_FW                   0x01U
#define STRINGTYPE_CONFIG               0x02U /* CS + IS */
#define STRINGTYPE_CS                   0x03U
#define STRINGTYPE_IS                   0x04U
#define STRINGTYPE_PS                   0x05U
#define STRINGTYPE_META                 0x07U
#define STRING_MIN_LEN                  32U

#define METAID_CHIPID                   0x00000100U
#define METAID_CHIPPRGMEMSIZE           0x00000101U
#define METAID_CHIPPRGMEMPAGESIZE       0x00000102U
#define METAID_CHIPPRGMEMSECTIONSIZE    0x00000103U
#define METAID_CHIPNUMOFINFOMEMSECTIONS 0x00000104U
#define METAID_CHIPINFOMEMSECTIONSIZE   0x00000105U
#define METAID_CHIPTESTMEMSIZE          0x00000106U
#define METAID_CHIPCOMBINEDCFGSTARTADDR 0x00000107U
#define METAID_CHIPCOMBINEDCFGSIZE      0x00000108U
#define METAID_FWMAJORVERSION           0x00000200U
#define METAID_FWMINORVERSION           0x00000201U
#define METAID_FWRELEASEVERSION         0x00000202U
#define METAID_FWBUILDVERSION           0x00000203U
#define METAID_FWSIZE                   0x00000204U
#define METAID_FWSTARTADDR              0x00000205U
#define METAID_BMSIZE                   0x00000400U
#define METAID_BMMAXDATALENGTH          0x00000401U
#define METAID_CFGSFORMATVERSION        0x00000600U
#define METAID_CFGSCUSTMAJORVERSION     0x00000601U
#define METAID_CFGSCUSTMINORVERSION     0x00000602U
#define METAID_CFGSCUSTRELEASEVERSION   0x00000603U
#define METAID_CFGSSIZE                 0x00000604U
#define METAID_CFGSDEFSTARTADDR         0x00000605U
#define METAID_CFGSSTDSTARTADDR         0x00000606U
#define METAID_CFGSOVRLSTARTADDR        0x00000607U
#define METAID_CFGSTESTSTARTADDR        0x00000608U
#define METAID_IDENTSFORMATVERSION      0x00000700U
#define METAID_IDENTSSIZE               0x00000701U
#define METAID_IDENTSSTDSTARTADDR       0x00000702U
#define METAID_IDENTSOVRLSTARTADDR      0x00000703U
#define METAID_IDENTSTESTSTARTADDR      0x00000704U
#define METAID_PATCHSSIZE               0x00000800U
#define METAID_PATCHSSTDSTARTADDR       0x00000801U
#define METAID_PATCHSTESTSTARTADDR      0x00000802U
#define METAID_TOOLTYPE                 0x00000900U
#define METAID_TOOLMAJORVERSION         0x00000901U
#define METAID_TOOLMINORVERSION         0x00000902U
#define METAID_TOOLRELEASEVERSION       0x00000903U
#define METAID_TOOLBUILDVERSION         0x00000904U

#define METATYPE_UINT8                  0x01U
#define METATYPE_UINT16                 0x02U
#define METATYPE_UINT32                 0x04U
#define METATYPE_INT8                   0x09U
#define METATYPE_INT16                  0x0AU
#define METATYPE_INT32                  0x0CU
#define METATYPE_STRING                 0x40U
#define METATYPE_ARRAY                  0x80U

#define DEFAULTVAL_UINT8                0xFFU
#define DEFAULTVAL_UINT16               0xFFFFU
#define DEFAULTVAL_UINT32               0xFFFFFFFFU
#define DEFAULTVAL_STRING               "\0"

#define VERSION_VALID                   0x00U
#define VERSION_INVALID                 0xFFU




/*------------------------------------------------------------------------------------------------*/
/* TYPES                                                                                          */
/*------------------------------------------------------------------------------------------------*/

typedef struct Ipl_MetaData_
{
    uint32_t NumOfItems;
    uint8_t  ChipID;
    uint32_t ChipPrgMemSize;
    uint32_t ChipPrgMemPageSize;
    uint32_t ChipPrgMemSectionSize;
    uint8_t  ChipNumOfInfoMemSections;
    uint32_t ChipInfoMemSectionSize;
    uint32_t ChipTestMemSize;
    uint32_t ChipCombinedCfgStartAddr;
    uint32_t ChipCombinedCfgSize;
    uint8_t  FwMajorVersion;
    uint8_t  FwMinorVersion;
    uint8_t  FwReleaseVersion;
    uint32_t FwBuildVersion;
    uint8_t  FwVersionValid;
    uint32_t FwSize;
    uint32_t FwStartAddr;
    uint32_t BmSize;
    uint32_t BmMaxDataLength;
    uint8_t  CfgsFormatVersion;
    uint8_t  CfgsCustMajorVersion;
    uint8_t  CfgsCustMinorVersion;
    uint8_t  CfgsCustReleaseVersion;
    uint8_t  CfgsVersionValid;
    uint32_t CfgsSize;
    uint32_t CfgsDefStartAddr;
    uint32_t CfgsStdStartAddr;
    uint32_t CfgsOvrlStartAddr;
    uint32_t CfgsTestStartAddr;
    uint8_t  IdentsFormatVersion;
    uint32_t IdentsSize;
    uint32_t IdentsStdStartAddr;
    uint32_t IdentsOvrlStartAddr;
    uint32_t IdentsTestStartAddr;
    uint32_t PatchsSize;
    uint32_t PatchsStdStartAddr;
    uint32_t PatchsTestStartAddr;
    char     ToolType[TOOL_MAX_TYPELEN];
    uint8_t  ToolMajorVersion;
    uint8_t  ToolMinorVersion;
    uint8_t  ToolReleaseVersion;
    uint32_t ToolBuildVersion;
} Ipl_MetaData_t;


/* Data derived from IPF file */
typedef struct Ipl_IpfData_
{
    uint32_t       ProgAddr;            /* Address of data as referred in INIC Programming Guide */
    uint32_t       StringSize;          /* Size of data as referred in INIC Programming Guide */
    uint32_t       StringOffset;        /* Offset of data */
    uint8_t        StringType;          /* Type of String */
    uint8_t        ChipID;              /* ChipID from IPF file as referred in INIC Programming Guide */
    Ipl_MetaData_t Meta;
} Ipl_IpfData_t;


/*------------------------------------------------------------------------------------------------*/
/* VARIABLES                                                                                      */
/*------------------------------------------------------------------------------------------------*/

extern Ipl_IpfData_t Ipl_IpfData;


/*------------------------------------------------------------------------------------------------*/
/* FUNCTION PROTOTYPES                                                                            */
/*------------------------------------------------------------------------------------------------*/

uint8_t Ipl_ParseIpf(Ipl_IpfData_t *ipf, uint32_t lData, uint8_t pData[], uint8_t stringType);
void    Ipl_ClrIpfData(Ipl_IpfData_t *ipf);
void    Ipl_ClrMetaData(Ipl_IpfData_t *ipf);


#endif
