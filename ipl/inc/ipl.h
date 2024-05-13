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

/*! \file   ipl.h
 *  \brief  Internal header for general functions for INIC Programming Library
 *  \author Roland Trissl (RTR)
 *  \note   For support related to this code contact http://www.microchip.com/support.
 */

#ifndef IPL_H
#define IPL_H

#include <stdint.h>
#include <stddef.h>
#include "ipl_cfg.h"
#include "ipl_pb.h"


/*------------------------------------------------------------------------------------------------*/
/* SPECIAL DEFINES                                                                                */
/*------------------------------------------------------------------------------------------------*/

/*! \internal #define IPL_LEGACY_IPF              */
/*! \internal #define IPL_TRACETAG_DUMP "IPL DMP" */
/*! \internal #define IPL_USE_OS81050             */
/*! \internal #define IPL_USE_OS81060             */
/*! \internal #define IPL_USE_OS81082             */
/*! \internal #define IPL_USE_OS81092             */
/*! \internal #define IPL_USE_OS81110             */
/*! \internal #define IPL_XTRA                    */


/*------------------------------------------------------------------------------------------------*/
/* CHECKS FOR ipl_cfg.h                                                                           */
/*------------------------------------------------------------------------------------------------*/

#ifndef IPL_TRACETAG_INFO
#define IPL_TRACETAG_INFO 0
#endif

#ifndef IPL_TRACETAG_ERR
#error "ipl_cfg.h: IPL_TRACETAG_ERR needs to be defined."
#endif

#ifndef IPL_DATACHUNK_SIZE
#error "ipl_cfg.h: IPL_DATACHUNK_SIZE needs to be defined."
#endif

#if (IPL_DATACHUNK_SIZE > 0) && (IPL_DATACHUNK_SIZE < 2048)
#error "ipl_cfg.h: IPL_DATACHUNK_SIZE must be at least 2048 (or 0)."
#endif

#ifndef IPL_USE_OS81118
#ifndef IPL_USE_OS81119
#ifndef IPL_USE_OS81210
#ifndef IPL_USE_OS81212
#ifndef IPL_USE_OS81214
#ifndef IPL_USE_OS81216
#ifndef IPL_USE_OS81110
#ifndef IPL_USE_OS81092
#ifndef IPL_USE_OS81082
#ifndef IPL_USE_OS81060
#ifndef IPL_USE_OS81050
#error "ipl_cfg.h: At least one supported INIC needs to be defined (for example IPL_USE_OS81118)."
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif


/*------------------------------------------------------------------------------------------------*/
/* CONSTANTS                                                                                      */
/*------------------------------------------------------------------------------------------------*/

/*! \defgroup version Version
 *  This is the version number of the INIC Programming Library.
 *  Always pass on this number when communicating with Microchip support.
 */
/*!@{*/
#define VERSIONTAG "V1.0.0-8"
/*!@}*/


/*------------------------------------------------------------------------------------------------*/
/* INIC MODES                                                                                     */
/*------------------------------------------------------------------------------------------------*/

#define INIC_MODE_NORMAL                 0x01U
#define INIC_MODE_BOOT                   0x02U

#define INIC_TESTMEM_UNCLEARED           0x00U
#define INIC_TESTMEM_CLEARED             0x01U


/*------------------------------------------------------------------------------------------------*/
/* MAXIMUM LENGTHS                                                                                */
/*------------------------------------------------------------------------------------------------*/

#define INIC_MAX_TELLEN                 36U /* Meta.BmMaxDataLength + 4 */
#define INIC_MAX_PATCHSTRINGSIZE        64U
#define INIC_MAX_TESTMEMSIZE            768U /*  OS8121x */
#define INIC_MAX_OTPMEMSIZE             0x01FFU /* Address range from ReadOTPMemory command */
#define INIC_MAX_INFOMEMSIZE            0x03FFU /* Address range from ReadInfoMemory command */
#define INIC_MAX_PROGMEMSIZE            0xFFFFU /* Address range from ReadProgramMemory command */
#define INIC_MAX_RAMSIZE                0x2000U
#define INIC_MAX_DATABUFFERSIZE         0x5000U
#define INIC_MAX_RTSIZE                 768U
#define INIC_MAX_RFSIZE                 640U
#define INIC_MAX_IOREGNUM               0xFFU
#define INIC_MAX_EXTIOREGNUM            0xFFU
#define INIC_MAX_CPUREGNUM              0x07U
#define TOOL_MAX_TYPELEN                80U


/*------------------------------------------------------------------------------------------------*/
/* INIC COMMANDS AND TX/RX COMMAND LENGTHS                                                        */
/*------------------------------------------------------------------------------------------------*/

#define CMD_PROGSTART                   0x0DU
#define CMD_PROGSTART_TXLEN             4U
#define CMD_PROGSTART_RXLEN             1U
#define CMD_READFWVER                   0xEEU
#define CMD_READFWVER_TXLEN             4U
#define CMD_READFWVER_RXLEN             32U /* Meta.BmMaxDataLength */
#define CMD_ERASEPROGMEM                0x0CU
#define CMD_ERASEPROGMEM_TXLEN          4U
#define CMD_ERASEPROGMEM_RXLEN          1U
#define CMD_SETPROGMEMPAGE              0x06U
#define CMD_SETPROGMEMPAGE_TXLEN        2U
#define CMD_SETPROGMEMPAGE_RXLEN        1U
#define CMD_READPROGMEM                 0x11U
#define CMD_READPROGMEM_TXLEN           4U
#define CMD_READPROGMEM_RXLEN           36U /* Meta.BmMaxDataLength + 4 */
#define CMD_WRITEPROGMEM                0x01U
#define CMD_WRITEPROGMEM_TXLEN          36U /* Meta.BmMaxDataLength + 4 */
#define CMD_WRITEPROGMEM_RXLEN          1U
#define CMD_ERASEINFOMEM                0xCEU
#define CMD_ERASEINFOMEM_TXLEN          3U
#define CMD_ERASEINFOMEM_RXLEN          1U
#define CMD_READINFOMEM                 0xD2U
#define CMD_READINFOMEM_TXLEN           4U
#define CMD_READINFOMEM_RXLEN           36U /* Meta.BmMaxDataLength + 4 */
#define CMD_WRITEINFOMEM                0xC2U
#define CMD_WRITEINFOMEM_TXLEN          36U /* Meta.BmMaxDataLength + 4 */
#define CMD_WRITEINFOMEM_RXLEN          1U
#define CMD_VERIFYINFOMEM               0xCFU
#define CMD_VERIFYINFOMEM_TXLEN         4U
#define CMD_VERIFYINFOMEM_RXLEN         1U
#define CMD_READOTPMEM                  0x90U
#define CMD_READOTPMEM_TXLEN            4U
#define CMD_READOTPMEM_RXLEN            36U /* Meta.BmMaxDataLength + 4 */
#define CMD_WRITEOTPMEM                 0x80U
#define CMD_WRITEOTPMEM_TXLEN           36U /* Meta.BmMaxDataLength + 4 */
#define CMD_WRITEOTPMEM_RXLEN           1U
#define CMD_VERIFYOTPMEM                0x82U
#define CMD_VERIFYOTPMEM_TXLEN          4U
#define CMD_VERIFYOTPMEM_RXLEN          1U
#define CMD_WRITETESTMEM                0x0AU
#define CMD_WRITETESTMEM_TXLEN          36U /* Meta.BmMaxDataLength + 4 */
#define CMD_WRITETESTMEM_RXLEN          1U
#define CMD_READTESTMEM                 0x1AU
#define CMD_READTESTMEM_TXLEN           4U
#define CMD_READTESTMEM_RXLEN           36U /* Meta.BmMaxDataLength + 4 */
#define CMD_CLEARCRC                    0xE5U
#define CMD_CLEARCRC_TXLEN              4U
#define CMD_CLEARCRC_RXLEN              1U
#define CMD_GETCRC                      0xE6U
#define CMD_GETCRC_TXLEN                4U
#define CMD_GETCRC_RXLEN                6U
#define CMD_LEG_READFWVER               0xE4U
#define CMD_LEG_READFWVER_TXLEN         4U
#define CMD_LEG_READFWVER_RXLEN         19U
#define CMD_LEG_ERASEENABLE             0x0FU
#define CMD_LEG_ERASEENABLE_TXLEN       5U
#define CMD_LEG_ERASEENABLE_RXLEN       1U
#define CMD_LEG_ERASECS                 0xCCU
#define CMD_LEG_ERASECS_TXLEN           2U
#define CMD_LEG_ERASECS_RXLEN           1U
#define CMD_LEG_WRITECS                 0xC1U
#define CMD_LEG_WRITECS_TXLEN           32U
#define CMD_LEG_WRITECS_RXLEN           1U
#define CMD_LEG_GETCSINFO               0xD1U
#define CMD_LEG_GETCSINFO_TXLEN         4U
#define CMD_LEG_GETCSINFO_RXLEN         30U
#define CMD_READRAM                     0x10U
#define CMD_READRAM_TXLEN               4U
#define CMD_READRAM_RXLEN               36U
#define CMD_READIOREG                   0x12U
#define CMD_READIOREG_TXLEN             4U
#define CMD_READIOREG_RXLEN             36U
#define CMD_READCPUREG                  0x14U
#define CMD_READCPUREG_TXLEN            4U
#define CMD_READCPUREG_RXLEN            36U
#define CMD_READEXTIOREG                0x1BU
#define CMD_READEXTIOREG_TXLEN          4U
#define CMD_READEXTIOREG_RXLEN          36U
#define CMD_READDATABUF                 0x30U
#define CMD_READDATABUF_TXLEN           4U
#define CMD_READDATABUF_RXLEN           36U
#define CMD_READRT                      0x50U
#define CMD_READRT_TXLEN                4U
#define CMD_READRT_RXLEN                36U
#define CMD_READRF0                     0x51U
#define CMD_READRF0_TXLEN               4U
#define CMD_READRF0_RXLEN               36U
#define CMD_READRF1                     0x52U
#define CMD_READRF1_TXLEN               4U
#define CMD_READRF1_RXLEN               36U
#define CMD_WRITEIOREG                  0x02U
#define CMD_WRITEIOREG_TXLEN            8U
#define CMD_WRITEIOREG_RXLEN            1U


/*------------------------------------------------------------------------------------------------*/
/* CONSTANTS FOR TRACING                                                                          */
/*------------------------------------------------------------------------------------------------*/

#define TRACELINE_MAXLEN                200U
#define DIR_TX                          0U
#define DIR_RX                          1U
#define IPL_LOW                         0U
#define IPL_HIGH                        1U

/*------------------------------------------------------------------------------------------------*/
/* INIC TIMING VALUES (IN MILLISECONDS)                                                           */
/*------------------------------------------------------------------------------------------------*/

#define INIC_PIN_WAIT_TIME              10U   /* Time to wait before continueing after pin has been set */
#define INIC_RESP_WAIT_TIME             1U    /* Time to wait before reading response from INIC */
#define INIC_INT_WAIT_TIMEOUT           200U  /* Timeout for INT pin getting low */
#define INIC_BOOTUP_TIME                12U   /* Time to wait after INIC reset before sending 1st command */
#define INIC_PROGRAM_WAIT_TIME          10U   /* Time to wait after a programming command was sent */
#define INIC_PROGRAM_ADDWAIT_TIME       5U    /* Additional time for legacy INICs */
#define INIC_ERASEPROGMEM_WAIT_TIME     5000U /* Time to wait after an erase prog mem command was sent */
#define INIC_ERASEINFOMEM_WAIT_TIME     1100U /* Time to wait after an erase info mem command was sent */


/*------------------------------------------------------------------------------------------------*/
/* OTHER CONSTANTS                                                                                */
/*------------------------------------------------------------------------------------------------*/

#define TESTMEM_DATA                    0U
#define TESTMEM_CLEAR                   1U

#if defined IPL_USE_OS81110 || defined IPL_USE_OS81092 || defined IPL_USE_OS81082 || defined IPL_USE_OS81060 || defined IPL_USE_OS81050
#define IPL_LEGACY_IPF
#define IPL_LEGACY_INIC
#endif

/*------------------------------------------------------------------------------------------------*/
/* TYPES                                                                                          */
/*------------------------------------------------------------------------------------------------*/

typedef struct Ipl_IplData_
{
    uint8_t  Tel[INIC_MAX_TELLEN]; /*!< \internal Message Buffer for message to (TX) and from (RX) INIC       */
    uint8_t  TelLen;               /*!< \internal Length of the sent/received message                         */
    uint8_t  ChipID;               /*!< \internal ChipID as referred in INIC Programming Guide                */
    uint16_t IntTime;              /*!< \internal Time elapsed until INT pin toggled                          */
    uint8_t  ChipMode;             /*!< \internal Current INIC mode (INICMODE_BOOT or INICMODE_NORMAL         */

    uint32_t ChunkOffset;
    uint8_t* pData;
} Ipl_IplData_t;


typedef struct Ipl_InicData_
{
    uint8_t  ChipID;                  /*!< \internal INIC identifier. */
    uint8_t  FwMajorVersion;          /*!< \internal Firmware major version. */
    uint8_t  FwMinorVersion;          /*!< \internal Firmware minor version. */
    uint8_t  FwReleaseVersion;        /*!< \internal Firmware release version. */
    uint32_t FwBuildVersion;          /*!< \internal Firmware build version. */
    uint8_t  CfgsCustMajorVersion;    /*!< \internal ConfigString customer major version. */
    uint8_t  CfgsCustMinorVersion;    /*!< \internal ConfigString customer minor version. */
    uint8_t  CfgsCustReleaseVersion;  /*!< \internal ConfigString customer release version. */
    uint8_t  FwVersionValid;          /*!< \internal Indicator if firmware version is valid or not. */
    uint16_t FwCrc;                   /*!< \internal CRC value of firmware. */
    uint8_t  CfgsVersionValid;        /*!< \internal Indicator if ConfigString version is valid or not. */
    uint8_t  CfgsActiveConfigPage;    /*!< \internal Active configuration page. */
    uint8_t  TestMemCleared;          /*!< \internal State of the INIC test memory. */
} Ipl_InicData_t;


/*------------------------------------------------------------------------------------------------*/
/* VARIABLES                                                                                      */
/*------------------------------------------------------------------------------------------------*/

extern Ipl_IplData_t Ipl_IplData;
extern Ipl_InicData_t Ipl_InicData;

/*------------------------------------------------------------------------------------------------*/
/* FUNCTION PROTOTYPES                                                                            */
/*------------------------------------------------------------------------------------------------*/

uint8_t Ipl_ExecInicCmd(void);
void    Ipl_ClrTel(void);
void    Ipl_ProgressIndicator(uint32_t val, uint32_t fval);
uint8_t Ipl_CheckChipId(void);
uint8_t Ipl_CheckInicFwVersion(void);
char*   Ipl_TraceTag(uint8_t  result);
char    Ipl_Bcd2Char(uint8_t val, uint8_t lowHigh);
uint8_t Ipl_ClrPData(uint32_t lData, uint8_t pData[]);
uint8_t Ipl_PData(uint32_t index, uint32_t lData, uint8_t pData[]);
void    Ipl_ExportChipInfo(void);


/*------------------------------------------------------------------------------------------------*/
/* DOXYGEN                                                                                        */
/*------------------------------------------------------------------------------------------------*/

/*! \defgroup conf Configuration
 *  All configuration is done inside the ipl_cfg.h file.
 */
/*!@{*/
/*!@}*/


/*! \page introduction INIC Programming Library (IPL)
 *  Microchip Technology Inc. provides a platform independent programming library called IPL.<br>
 *  The IPL enables an application to program Microchip's Intelligent Network Interface Controllers (INICs)
 *  via their I2C interface.<br> <br>
 *  The IPL can handle the following INICs:<br> <br>
 *  OS81118 (INICnet technology 150)<br>
 *  OS81119 (INICnet technology 150)<br>
 *  OS81210 (INICnet technology 50)<br>
 *  OS81212 (INICnet technology 50)<br>
 *  OS81214 (INICnet technology 50)<br>
 *  OS81216 (INICnet technology 50)<br>
 */


#ifdef IPL_LEGACY_INIC
/*! \page introduction INIC Programming Library (IPL)
 *  OS81110 (MOST 150)<br>
 *  OS81092 (MOST 50)<br>
 *  OS81082 (MOST 50)<br>
 *  OS81060 (MOST 25)<br>
 *  OS81050 (MOST 25)<br>
 */
#endif


/*! \page introduction INIC Programming Library (IPL)
 *  Depending on the used INIC, it is possible to program the<br>
 *  - Configuration string (CFGS)
 *  - Identification string (IDENTS)
 *  - Configuration (this includes both CFGS and IDENTS)
 *  - Patch string (PATCHS)
 *  - Firmware
 *
 *  into RAM, OTP or FLASH memory.<br>
 *  The IPL uses the IPF format for the programming data. IPF files can be created by
 *  the Microchip Automotive Target Manager (MATM)<br> or UNICENS System Designer.
 *  If the programming device cannot handle files, you can convert an IPF file to a C-style \ref ipfconvert "data array". <br> <br>
 *  With the IPL, it is possible to read out CFGS and firmware versions and to check if an update with a
 *  certain IPF data file is useful.<br>
 *  Besides the connection via I2C, INIC's RESET pin and ERR/BOOT_ pin needs to be controlled by the IPL.
 *  The use of the INT_ pin is optional.<br>
 *  For systems with low memory it is possible to load portions of the IPF content instead of loading the entire data. <br>
 *  \note For support related to this code contact http://www.microchip.com/support.
 */


/*! \page ipfconvert Conversion of IPF files to data array
 *  If the programming device cannot handle files, you can convert an IPF file to a C-style data array.
 *
 *  For such purpose, the Linux tool
 *
 *  <strong>xxd</strong>
 *
 *  can be used. With the command line option -i a header file can be created.
 *
 *  Example:
 *
 *  <strong>xxd -i cs.ipf >cs.h</strong>
 *
 *  where:
 *
 *  <strong>cs.ipf</strong> is the IPF file created with MATM or UNICENS System Designer.<br>
 *  <strong>cs.h</strong> is the header file that will be created.
 *
 *  <strong>cs.h</strong> then contains the data in the following style:
 *
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 unsigned char cs_ipf[] = {
 0x01, 0x30, 0xff, 0xff, 0xff, 0xff, 0x06, 0x01, 0x00, 0x02, 0xff, 0xec,
 0x00, 0x00, 0x00, 0x10, 0x01, 0x00, 0x08, 0x12, 0x10, 0x01, 0x01, 0x00,
 0x00, 0x00, 0x00, 0x16, 0xff, 0xff, 0x85, 0x13, 0x03, 0x01, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x32, 0x87, 0x22, 0x12, 0x70, 0x01, 0x00,
 0x99, 0x09, 0x08, 0x03, 0xff, 0xff, 0xff, 0xfd, 0xa0, 0x01, 0x50, 0xaa,
 0x00, 0x8d, 0xb4, 0x01, 0x17, 0x18, 0xe0, 0x00, 0x02, 0x04, 0x08, 0x10,
 0x78, 0xf1, 0xe3, 0xc0, 0x00, 0x00, 0x30, 0x9f, 0xff, 0xff, 0xff, 0xf0,
 0x60, 0x42, 0x82, 0x7f, 0xff, 0xff, 0x86, 0x62, 0x07, 0x01, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x01, 0x32, 0x00, 0x00, 0x00, 0x17, 0x00, 0x00,
 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00,
 0x01, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00,
 0x01, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
 0x01, 0x03, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x01, 0x06, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
 0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
 0x02, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x02, 0x03, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00,
 0x02, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00, 0x02, 0xe5, 0xff, 0x00, 0x00,
 0x02, 0x05, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x00, 0x00, 0x00,
 0x04, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,
 0x04, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00,
 0x06, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x87, 0x00, 0x00,
 0x06, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x32, 0x00, 0x00,
 0x06, 0x05, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x9c, 0x00, 0x00,
 0x06, 0x06, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x06, 0x07, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x32, 0x00, 0x00,
 0x06, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x09, 0x00, 0x40, 0x00, 0x00, 0x1a, 0x00, 0x00, 0x01, 0x18, 0x00, 0x00,
 0x09, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
 0x09, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x09, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x75, 0x63,
 0x73, 0x2d, 0x70, 0x72, 0x6f, 0x70, 0x65, 0x72, 0x74, 0x79, 0x2d, 0x63,
 0x6f, 0x6e, 0x66, 0x69, 0x67, 0x75, 0x72, 0x61, 0x74, 0x6f, 0x72, 0x00
 };
 unsigned int cs_ipf_len = 408;
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * and can added to your project.
 * The array can be easily referenced by any IPL job:
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 res = Ipl_Prog(IPL_JOB_PROG_CONFIGSTRING, cs_ipf_len, &cs_ipf);
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * <br>
 */



/*! \page history Release History
 *  # V1.0.0-8 #
 *  <b> Release date: </b>2023-11-03<br>
 *  <table>
 *  <tr><th>Type                  <th>Description
 *  <tr><td>Issue                 <td>Fixed issue: Reviewed and corrected all meta data for OS8121x.
 *  </table>
 *  # V1.0.0-7 #
 *  <b> Release date: </b>2023-03-24<br>
 *  <table>
 *  <tr><th>Type                  <th>Description
 *  <tr><td>Issue                 <td>Fixed issue: IdentString Address for Test Memory on OS8121x was incorrect. Minor adjustments.
 *  </table>
 *  # V1.0.0-6 #
 *  <b> Release date: </b>2022-06-03<br>
 *  <table>
 *  <tr><th>Type                  <th>Description
 *  <tr><td>Issue                 <td>Fixed issue: Added default values to support MATM versions that create IPF files without meta data.
 *  </table>
 *  # V1.0.0-5 #
 *  <b> Release date: </b>2020-04-09<br>
 *  <table>
 *  <tr><th>Type                  <th>Description
 *  <tr><td>Issue                 <td>Fixed issue: Wait time after detection of INT_ pin activity longer than needed.
 *  </table>
 *  # V1.0.0-4 #
 *  <b> Release date: </b>2019-03-15<br>
 *  <table>
 *  <tr><th>Type                  <th>Description
 *  <tr><td>Issue                 <td>Added OS81050, OS81060, OS81082, OS81092, OS81110. Some minor fixes and cleanups.
 *  </table>
 *  # V1.0.0-3 #
 *  <b> Release date: </b>2018-11-28<br>
 *  <table>
 *  <tr><th>Type                  <th>Description
 *  <tr><td>Issue                 <td>Fixed issue: If the firmware was not programmed correctly (no valid firmware version could be read),
 *                                    ::IPL_JOB_CHK_UPDATE_CONFIGSTRING will report this with result ::IPL_RES_UPDATE_DENIED_UNKNOWN
 *                                    and not compare the CFGS version numbers.
 *  </table>
 *  # V1.0.0-1 (Initial version) #
 *  <b> Release date: </b>2018-10-11<br>
 *  <table>
 *  <tr><th>Type                  <th>Description
 *  <tr><td>Info                  <td>First version of IPL.
 *  </table>
 */


/*! \page misra MISRA-C-2004
 *  The library was developed in compliance to MISRA-C-2004.
 *  # Project information #
 *  <b>Total files:</b> 18 (Core IPL) + 12 (Optional components)<br>
 *  <b>Checked files:</b> 18 (Core IPL) + 12 (Optional components)<br>
 *  <b>Total violations:</b> 0<br>
 *  <b>Suppressed violations:</b> 0<br>
 */

#endif
