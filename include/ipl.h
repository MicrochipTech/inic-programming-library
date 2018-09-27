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

/*! \file ipl.h
 *  \brief Internal header for general functions for INIC Programming Library
 *  \author Roland Trissl (RTR)
 */

#ifndef IPL_H
#define IPL_H

#include <stdint.h>
#include "ipl_cfg.h"
#include "ipl_pb.h"

/*------------------------------------------------------------------------------------------------*/
/* DOCUMENTATION OF POTENTIALLY UNDEFINED MACROS (ipl_cfg.h) AND INTRODUCTION PAGE                */
/*------------------------------------------------------------------------------------------------*/

#ifdef _IPL_DOXYGEN

#ifndef IPL_USE_OS81118
/** \ingroup conf_inics */
/*@{*/
/*! Adds the functions needed for OS81118. */
#define IPL_USE_OS81118
/*@}*/
#endif

#ifndef IPL_USE_OS81119
/** \ingroup conf_inics */
/*@{*/
/*! Adds the functions needed for OS81119. */
#define IPL_USE_OS81119
/*@}*/
#endif

#ifndef IPL_USE_OS81210
/** \ingroup conf_inics */
/*@{*/
/*! Adds the functions needed for OS81210. */
#define IPL_USE_OS81210
/*@}*/
#endif

#ifndef IPL_USE_OS81212
/** \ingroup conf_inics */
/*@{*/
/*! Adds the functions needed for OS81212. */
#define IPL_USE_OS81212
/*@}*/
#endif

#ifndef IPL_USE_OS81214
/** \ingroup conf_inics */
/*@{*/
/*! Adds the functions needed for OS81214. */
#define IPL_USE_OS81214
/*@}*/
#endif

#ifndef IPL_USE_INTPIN
/** \ingroup intpin */
/*@{*/
/*!< \brief Enables usage of INICs INT pin for RX control.
 *
 * If not defined, IPL does not check INICs INT pin.
 */
#define IPL_USE_INTPIN
/*@}*/
#endif

#ifndef IPL_INICDRIVER_OPENCLOSE
/** \ingroup driver_openclose */
/*@{*/
/*! \brief Enables additional callbacks to open/close INIC driver
 *
 * Callbacks: ::Ipl_InicDriverOpen(), ::Ipl_InicDriverClose().
 */
#define IPL_INICDRIVER_OPENCLOSE
/*@}*/
#endif

#ifndef IPL_PROGRESS_INDICATOR
/** \ingroup progress_indicator */
/*@{*/
/*! \brief Enables progress indicator callback ::Ipl_Progress(). */
#define IPL_PROGRESS_INDICATOR
/*@}*/
#endif

#ifndef IPL_TRACETAG_IPF
/** \ingroup trace_tags */
/*@{*/
/*! \brief Tag used for IPF data tracing.
 *
 * If tag is not defined IPF data is not traced.
 */
#define IPL_TRACETAG_IPF     "IPL IPF"
/*@}*/
#endif

#ifndef IPL_DECODE_RESULTS
/** \ingroup decode_res */
/*@{*/
 /*!< \brief The macro enables the API function Ipl_DecodeResult to decode the result values of all functions. */
#define IPL_DECODE_RESULTS
/*@}*/
#endif

#ifndef IPL_TRACETAG_COM
/** \ingroup trace_tags */
/*@{*/
/*!< \brief Tag used for INIC communication tracing.
 *
 * If tag is not defined communication with INIC is not traced.
 */
#define IPL_TRACETAG_COM     "IPL COM"
/*@}*/
#endif

#endif

/**
 * \page introduction INIC Programming Library (IPL)
 * The IPL is a platform independent programming library that enables an application to program
 * Microchip INIC network controllers via the I2C interface.<br> <br>
 * The library can handle the following INICs:<br>
 * OS81118<br>
 * OS81119<br>
 * OS81210<br>
 * OS81212<br>
 * OS81214<br> <br>
 * Depending on the used INIC, it is possible to program ConfigString, IdentString, PatchString and Firmware
 * into RAM, OTP or FLASH.<br>
 * The library is using the IPF format for the programming data which can be created by
 * Microchip Automotive Target Manager (MATM) or UNICENS System Designer.<br>
 * It is also possible to read out ConfigString and Firmware versions and to check, if an update with a
 * certain IPF data file is useful.<br> <br>
 * Besides the connection via I2C, INICs RESET pin and ERR/BOOT pin needs to be controlled by the library.
 * The use of the INT pin is optional.<br> <br>
 * On systems with low memory, it is possible to load only portions of the IPF content instead of the complete data.<br>
 */


/*------------------------------------------------------------------------------------------------*/
/* CONSTANTS                                                                                      */
/*------------------------------------------------------------------------------------------------*/

/**
 * \defgroup version ***** Version ***** */
/*@{*/
/*! This is the version number of the INIC Programming Library.
 *  Please always pass on this number when communicating with MCHP support.
 */
#define VERSIONTAG "V0.0.7-0"
/*@}*/


/*------------------------------------------------------------------------------------------------*/
/* INIC MODES                                                                                     */
/*------------------------------------------------------------------------------------------------*/

#define INICMODE_NORMAL                 0x01U
#define INICMODE_BOOT                   0x02U


/*------------------------------------------------------------------------------------------------*/
/* MAXIMUM LENGTHS                                                                                */
/*------------------------------------------------------------------------------------------------*/

#define INIC_MAX_TELLEN                 36U /* Meta.BmMaxDataLength + 4 */
#define INIC_MAX_PATCHSTRINGLEN         64U
#define INIC_MAX_TESTMEMSIZE            768U /* OS8121x */
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
#define CMD_CLEARCRC                    0xE5U
#define CMD_CLEARCRC_TXLEN              4U
#define CMD_CLEARCRC_RXLEN              1U
#define CMD_GETCRC                      0xE6U
#define CMD_GETCRC_TXLEN                4U
#define CMD_GETCRC_RXLEN                6U


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
#define INIC_ERASEPROGMEM_WAIT_TIME     5000U /* Time to wait after an erase prog mem command was sent */
#define INIC_ERASEINFOMEM_WAIT_TIME     1100U /* Time to wait after an erase info mem command was sent */


/*------------------------------------------------------------------------------------------------*/
/* OTHER CONSTANTS                                                                                */
/*------------------------------------------------------------------------------------------------*/

#define TESTMEM_DATA                    0U
#define TESTMEM_CLEAR                   1U


/*------------------------------------------------------------------------------------------------*/
/* TYPES                                                                                          */
/*------------------------------------------------------------------------------------------------*/

/* IPL internal Data */
typedef struct Ipl_IplData_
{
    uint8_t  Tel[INIC_MAX_TELLEN]; /* Message Buffer for message to (TX) and from (RX) INIC       */
    uint8_t  TelLen;               /* Length of the sent/received message                         */
    uint8_t  ChipID;               /* ChipID as referred in INIC Programming Guide                */
    uint16_t IntTime;              /* Time elapsed until INT pin toggled                          */
    uint8_t  ChipMode;             /* Current INIC mode (INICMODE_BOOT or INICMODE_NORMAL         */

    uint32_t ChunkOffset;
    uint8_t* pData;
} Ipl_IplData_t;


/*------------------------------------------------------------------------------------------------*/
/* VARIABLES                                                                                      */
/*------------------------------------------------------------------------------------------------*/

extern Ipl_IplData_t Ipl_IplData;


/*------------------------------------------------------------------------------------------------*/
/* FUNCTION PROTOTYPES                                                                            */
/*------------------------------------------------------------------------------------------------*/

uint8_t Ipl_ExecInicCmd(void);
void    Ipl_ClrTel(void);
void    Ipl_ProgressIndicator(uint32_t val, uint32_t fval);
uint8_t Ipl_CheckChipId(void);
uint8_t Ipl_CheckInicFwVersion(void);
char*   Ipl_TraceTag(uint8_t  result);
char    Ipl_BcdChr(uint8_t val, uint8_t lowHigh);
uint8_t Ipl_ClrPData(uint32_t lData, uint8_t pData[]);
uint8_t Ipl_PData(uint32_t index, uint32_t lData, uint8_t pData[]);


#endif
