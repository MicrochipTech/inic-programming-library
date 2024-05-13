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

/*! \file   ipl_pb.h
 *  \brief  Public API function header for INIC Programming Library
 *  \author Roland Trissl (RTR)
 *  \note   For support related to this code contact http://www.microchip.com/support.
 */

#ifndef IPL_PB_H
#define IPL_PB_H

#include <stdint.h>
#include "ipl_cfg.h"


/*------------------------------------------------------------------------------------------------*/
/* CHECKS FOR ipl_cfg.h                                                                           */
/*------------------------------------------------------------------------------------------------*/

#ifndef IPL_DATACHUNK_SIZE
#error "ipl_cfg.h: IPL_DATACHUNK_SIZE needs to be defined."
#endif


/*------------------------------------------------------------------------------------------------*/
/* CONSTANTS                                                                                      */
/*------------------------------------------------------------------------------------------------*/

/*!
 * \defgroup chip_ids Chip IDs
 * These Chip IDs are to be used with the API function ::Ipl_EnterProgMode().
 */
/*!@{*/

/*! \brief OS81118 (INICnet technology 150). */
#define IPL_CHIP_OS81118                    0x18U
/*! \brief OS81119 (INICnet technology 150). */
#define IPL_CHIP_OS81119                    0x19U
/*! \brief OS81210 (INICnet technology 50). */
#define IPL_CHIP_OS81210                    0x30U
/*! \brief OS81212 (INICnet technology 50). */
#define IPL_CHIP_OS81212                    0x32U
/*! \brief OS81214 (INICnet technology 50). */
#define IPL_CHIP_OS81214                    0x34U
/*! \brief OS81216 (INICnet technology 50). */
#define IPL_CHIP_OS81216                    0x33U
/*! \brief OS81110 (MOST 150). */
#define IPL_CHIP_OS81110                    0x10U
/*! \brief OS81092 (MOST 50). */
#define IPL_CHIP_OS81092                    0x82U
/*! \brief OS81082 (MOST 50). */
#define IPL_CHIP_OS81082                    0x92U
/*! \brief OS81060 (MOST 25). */
#define IPL_CHIP_OS81060                    0x60U
/*! \brief OS81050 (MOST25). */
#define IPL_CHIP_OS81050                    0x0FU

/*!@}*/


/*!
 * \defgroup result_codes Result Values
 * These result values are returned by all IPL API functions and by internal functions that are traced.
 * Some result values are passed on from INIC, some are generated within the IPL.
 */
/*!@{*/

/*! \brief Success. No error occured.
 *
 *  Parameter is returned by IPL.
 */
#define IPL_RES_OK                          0x00U
/*! \brief Error. Identification string invalid.
 *
 *  Error on verifying the Identification string.
 *  Parameter is returned by IPL.
 */
#define IPL_RES_ERR_IS_INVALID              0x60U

/*! \brief Error. Programming Info memory failed.
 *
 *  Parameter is returned by IPL.
 */
#define IPL_RES_ERR_PROG_INFOMEM            0x61U

/*! \brief Error. Function is not supported for this INIC.
 *
 *  Parameter is returned by IPL.
 */
#define IPL_RES_ERR_NOT_SUPPORTED           0x62U

/*! \brief Error. Error on opening/closing INIC driver.
 *
 *  Please check implementation of callback functions ::Ipl_InicDriverOpen() and ::Ipl_InicDriverClose().
 *  Parameter is returned by IPL.
 */
#define IPL_RES_ERR_HW_INIC_COM             0x63U

/*! \brief Error. Error on reading INIC's INT_ pin.
 *
 *  Please check hardware and implementation of callback function ::Ipl_GetIntPin().
 *  Parameter is returned by IPL.
 */
#define IPL_RES_ERR_INT_READ                0x64U

/*! \brief Error. Timeout while waiting for INIC's INT_ pin.
 *
 *  Please check hardware and implementation of callback function ::Ipl_GetIntPin().
 *  Parameter is returned by IPL.
 */
#define IPL_RES_ERR_INT_TIMEOUT             0x65U

/*! \brief Error. Patch string invalid.
 *
 *  Error on verifying the Patch string.
 *  Parameter is returned by IPL.
 */
#define IPL_RES_ERR_PS_INCORRECT            0x66U

/*! \brief Error. No more OTP space available.
 *
 *  All OTP sections in INIC have been already written.<br>
 *  Parameter is returned by IPL.
 */
#define IPL_RES_ERR_NO_OTP_SPACE            0x67U

/*! \brief Error. Accessing Test memory failed.
 *
 * Parameter is returned by IPL.
 */
#define IPL_RES_ERR_ACCESS_TESTMEM          0x68U

/*! \brief Error. Reading Info memory failed.
 *
 * Parameter is returned by IPL.
 */
#define IPL_RES_ERR_READINFOMEM             0x69U


/*! \brief Error. Reading firmware version failed.
 *
 *  Maybe INIC does not contain proper firmware.<br>
 *  Parameter is returned by IPL.
 */
#define IPL_RES_ERR_READFWVER               0x6AU

/*! \brief Error. ProgrammingStart failed.
 *
 *  Parameter is returned by IPL.
 */
#define IPL_RES_ERR_PROGSTART               0x6BU

/*! \brief Error. Reading OTP memory failed.
 *
 *  Parameter is returned by IPL.
 */
#define IPL_RES_ERR_READOTPMEM              0x6CU

/*! \brief Error. Read Program memory failed.
 *
 *  Parameter is returned by IPL.
 */
#define IPL_RES_ERR_READPROGMEM             0x6DU

/*! \brief Error. Verify OTP memory failed.
 *
 *  Parameter is returned by IPL.
 */
#define IPL_RES_ERR_VERIFYOTPMEM            0x6EU

/*! \brief Error. Write OTP memory failed.
 *
 *  Parameter is returned by IPL.
 */
#define IPL_RES_ERR_WRITEOTPMEM             0x6FU

/*! \brief Error. Write Info memory failed.
 *
 *  Parameter is returned by IPL.
 */
#define IPL_RES_ERR_WRITEINFOMEM            0x70U

/*! \brief Error. Erase Program memory failed.
 *
 *  Parameter is returned by IPL.
 */
#define IPL_RES_ERR_ERASEPROGMEM            0x71U

/*! \brief Error. Clear CRC failed.
 *
 *  Parameter is returned by IPL.
 */
#define IPL_RES_ERR_CLEARCRC                0x72U

/*! \brief Error. Set Program memory page failed.
 *
 *  Parameter is returned by IPL.
 */
#define IPL_RES_ERR_SETPROGMEMPAGE          0x73U

/*! \brief Error. Write Program memory failed.
 *
 *  Parameter is returned by IPL.
 */
#define IPL_RES_ERR_WRITEPROGMEM            0x74U

/*! \brief Error. Get CRC value failed.
 *
 *  Parameter is returned by IPL.
 */
#define IPL_RES_ERR_GETCRC                  0x75U

/*! \brief Error. Erase Info memory failed.
 *
 *  Parameter is returned by IPL.
 */
#define IPL_RES_ERR_ERASEINFOMEM            0x76U

/*! \brief Error. Sent TX telegramm is longer than ::INIC_MAX_TELLEN.
 *
 *  Parameter is returned by IPL.
 */
#define IPL_RES_ERR_TXTELLEN_INVALID        0x77U

/*! \brief Error. Error on reading data from INIC.
 *
 *  Please check hardware and implementation of ::Ipl_InicRead().
 *  Parameter is returned by IPL.
 */
#define IPL_RES_ERR_READ                    0x78U

/*! \brief Error. Error on writing data to INIC.
 *
 *  Please check hardware and implementation of ::Ipl_InicWrite().
 *  Parameter is returned by IPL.
 */
#define IPL_RES_ERR_WRITE                   0x79U

/*! \brief Error. Unexpected response from INIC.
 *
 *  Parameter is returned by IPL.
 */
#define IPL_RES_ERR_RESP_UNEXPECTED         0x7AU

/*! \brief Error. Unexpected command sent to INIC.
 *
 *  Parameter is returned by IPL.
 */
#define IPL_RES_ERR_CMD_UNEXPECTED          0x7BU

/*! \brief Error. Referred stringtype could not be found in IPF data.
 *
 *  Please check the used IPF data.
 *  Parameter is returned by IPL.
 */
#define IPL_RES_ERR_IPF_WRONGSTRINGTYPE     0x7CU

/*! \brief Error. IPF data does not fit the connected INIC chip.
 *
 *  Please check the used IPF data.
 *  Parameter is returned by IPL.
 */
#define IPL_RES_ERR_IPF_WRONGINIC           0x7DU

/*! \brief Error. Wrong CRC is returned.
 *
 *  Parameter is returned by IPL.
 */
#define IPL_RES_ERR_WRONG_CRC               0x7EU

/*! \brief Error. Configuration string invalid.
 *
 *  Error on verifying the configuration string.
 *  Parameter is returned by IPL.
 */
#define IPL_RES_ERR_CS_INVALID              0x7FU

/*! \brief Error. Programming OTP memory failed.
 *
 *  Parameter is returned by IPL.
 */
#define IPL_RES_ERR_PROG_OTP                0x80U

/*! \brief Error. IPF data does not fit the connected INIC firmware version.
 *
 *  Please check the used IPF data or update INIC firmware.
 *  Parameter is returned by IPL.
 */
#define IPL_RES_ERR_IPF_WRONGFWVERSION      0x81U

/*! \brief Error. IPF data seems to be invalid.
 *
 *  Please check the used IPF data.
 *  Parameter is returned by IPL.
 */
#define IPL_RES_ERR_IPF_INVALID             0x82U

/*! \brief Error. Patch string is longer than ::INIC_MAX_PATCHSTRINGSIZE.
 *
 *  Parameter is returned by IPL.
 */
#define IPL_RES_ERR_MAX_PATCHSTRINGLEN      0x83U

/*! \brief Error. No valid chip ID could be read from INIC.
 *
 *  Maybe INIC does not contain proper firmware.
 *  Parameter is returned by IPL.
 */
#define IPL_RES_ERR_NOVALIDCHIPID           0x84U

/*! \brief Error. INIC HW pins could not be set correctly.
 *
 *  Function ::Ipl_SetResetPin() or ::Ipl_SetErrBootPin() returned parameter
 *  unequal to 0 (indicating an error with HW interface).
 *  Parameter is returned by IPL.
 */
#define IPL_RES_ERR_HW_INIC_PINS            0x85U

/*! \brief Firmware or configuration string update is not recommended.
 *
 * INIC already contains the same version of firmware or configuration string than
 * the version in the IPF data.
 * Parameter is returned by IPL.
 */
#define IPL_RES_UPDATE_DENIED_EQUAL         0x86U

/*! \brief Firmware or configuration string update is not recommended.
 *
 *  INIC already contains a newer version of firmware or configuration string than
 *  the version in the IPF data.
 *  Parameter is returned by IPL.
 */
#define IPL_RES_UPDATE_DENIED_NEWER         0x87U

/*! \brief Firmware or configuration string update is not recommended.
 *
 *  The version of firmware or configuration string in the IPF data could not be determined.
 *  Parameter is returned by IPL.
 */
#define IPL_RES_UPDATE_DENIED_UNKNOWN       0x88U

/*! \brief Error. Application did not provide a valid IPF data chunk or buffer.
 *
 *  Please check implementation of callback function ::Ipl_ProvideDataChunk().
 *  Parameter is returned by IPL.
 */
#define IPL_RES_ERR_INVALID_DATACHUNK       0x89U

/*! \brief Error. Connected INIC does not fit the parameter given in ::Ipl_EnterProgMode().
 *
 *  Please check connected INIC.
 *  Parameter is returned by IPL.
 */
#define IPL_RES_ERR_WRONG_INIC              0x90U

/*! \brief Error. Accessing RAM failed.
 *
 * Parameter is returned by IPL.
 */
#define IPL_RES_ERR_ACCESS_RAM              0x68U

/*! \brief Error. Invalid parameters.
 *
 *  Invalid start partition and/or invalid number of partitions.
 *  Parameter is returned by INIC.
 */
#define IPL_RES_ERR_PARTITION_INVALID       0x01U

/*! \brief Error. Verification error.
 *
 *  Read back after write does not match the data sent or
 *  INIC firmware is invalid.
 *  Parameter is returned by INIC.
 */
#define IPL_RES_ERR_VERIFYINFOMEM           0x10U

/*! \brief Error. Invalid location.
 *
 *  Tried to program the boot monitor memory section instead of
 *  the current firmware area of INIC.
 *  Parameter is returned by INIC.
 */
#define IPL_RES_ERR_LOCATION_INVALID        0x11U

/*! \brief Error. Invalid address.
 *
 *  Parameter is returned by INIC.
 */
#define IPL_RES_ERR_ADDRESS_INVALID         0x12U

/*! \brief Error. Invalid length.
 *
 * Parameter is returned by INIC.
 */
#define IPL_RES_ERR_LENGTH_INVALID          0x13U

/*! \brief Error. Completion code is bad.
 *
 *  Parameter is returned by INIC.
 */
#define IPL_RES_ERR_CMD_CC_BAD              0x14U

/*! \brief Error. INIC is not locked to crystal oscillator.
 *
 *  INIC is not connected properly to crystal oscillator or wrong frequency is set.
 *  Parameter is returned by INIC.
 */
#define IPL_RES_ERR_NOT_LOCKED              0x20U

/*! \brief Error. No initialization done.
 *
 *  ProgrammingStart command was not sent or successful.
 *  Parameter is returned by INIC.
 */
#define IPL_RES_ERR_MISSING_PROGSTART       0x21U

/*! \brief Error. Bad clock speed.
 *
 * (Programming Start)<br>
 *  Parameter is returned by INIC.
 */
#define IPL_RES_ERR_BAD_CLK                 0x22U

/*! \brief Error. Bad divide.
 *
 *  Parameter is returned by INIC.
 */
#define IPL_RES_ERR_BAD_DIV                 0x23U

/*! \brief Error. Bad key.
 *
 *  (Safe Erase Info Block)<br>
 *  Parameter is returned by INIC.
 */
#define IPL_RES_ERR_BAD_KEY                 0x24U

/*! \brief Error. Wrong data sequence.
 *
 *  Parameter is returned by INIC.
 */
#define IPL_RES_ERR_WRONG_SEQUENCE          0x25U

/*! \brief Error. OTP write access is not possible.
 *
 *  Parameter is returned by INIC.
 */
#define IPL_RES_ERR_NO_OTP_ACCESS           0x26U

/*! \brief Error. All OTP bytes are read zero.
 *
 *  This occures also if OTP has never been written.
 *  Parameter is returned by INIC.
 */
#define IPL_RES_ERR_OTP_IS_ZERO             0x27U

/*! \brief Error. The command was not recognized by INIC.
 *
 *  Parameter is returned by INIC.
 */
#define IPL_RES_ERR_CMD_NOT_RECOGNIZED      0x40U


/*! \brief Error. OTP hardware interface is in use by application.
 *
 * Command cannot be executed.
 * Parameter is returned by INIC.
 */
#define IPL_RES_ERR_IF_USED                 0x41U

/*! \brief Error. Pipe not ready.
 *
 *  Parameter is returned by INIC.
 */
#define IPL_RES_ERR_PIPE_NOT_READY          0x50U

/*! \brief Error. Wrong pipe number.
 *
 *  Parameter is returned by INIC.
 */
#define IPL_RES_ERR_WRONG_PIPENUMBER        0x51U

/*! \brief Error. Wrong pipe mode.
 *
 * Parameter is returned by INIC.
 */
#define IPL_RES_ERR_WRONG_PIPEMODE          0x52U

/*! \brief Error. Pipe buffer not available.
 *
 *  Parameter is returned by INIC.
 */
#define IPL_RES_ERR_PIPE_BUF                0x53U

/*! \brief Error. Pipe data not available.
 *
 *  Parameter is returned by INIC.
 */
#define IPL_RES_ERR_PIPE_DATA               0x54U

/*!@}*/


/*!
 * \defgroup jobs ***** Jobs *****
 * The following jobs are to be used with the ::Ipl_Prog() function.
 * If the respective job is not supported for a specific INIC, an error ::IPL_RES_ERR_NOT_SUPPORTED will be reported.
 */
/*!@{*/

/*! \brief Reads the firmware version. Available for FLASH and ROM INICs.
 *
 *  For reading the firmware version, no IPF data is required.
 */
#define IPL_JOB_READ_FIRMWARE_VER           0x01U

/*! \brief Reads the config string version according to the referred IPF data.
 *  \note  The configuration string version is read from OTP, not from test memory.
 */
#define IPL_JOB_READ_CONFIGSTRING_VER       0x02U

/*! \brief Programs the INIC firmware from the referred IPF data.
 */
#define IPL_JOB_PROG_FIRMWARE               0x03U

/*! \brief Programs the INIC configuration from the referred IPF data.
 *
 * If a ROM INIC is connected and this is the first job to be performed after ::Ipl_EnterProgMode(),
 * the INIC's test memory is cleared before programming.
 */
#define IPL_JOB_PROG_CONFIG                 0x04U

/*! \brief Programs the INIC patch string from the referred IPF data.
*
*   If a ROM INIC is connected and this is the first job to be performed after ::Ipl_EnterProgMode(),
*   the INIC's test memory is cleared before programming.
*/
#define IPL_JOB_PROG_PATCHSTRING            0x05U

/*! \brief Programs the INIC test config from the referred IPF data.
 *
 *  If a ROM INIC is connected and this is the first job to be performed after ::Ipl_EnterProgMode(),
 *  the INIC's test memory is cleared before programming.
 */
#define IPL_JOB_PROG_TEST_CONFIG            0x06U

/*! \brief Programs the INIC test patch string from the referred IPF data.
 *
 *  If a ROM INIC is connected and this is the first job to be performed after ::Ipl_EnterProgMode(),
 *  the INIC's test memory is cleared before programming.
 */
#define IPL_JOB_PROG_TEST_PATCHSTRING       0x07U

/*! \brief Programs the INIC config string from the referred IPF data.
 *
 *  If a ROM INIC is connected and this is the first job to be performed after ::Ipl_EnterProgMode(),
 *  the INIC's test memory is cleared before programming.
 */
#define IPL_JOB_PROG_CONFIGSTRING           0x08U

/*! \brief Programs the INIC test config string from the referred IPF data.
 *
 *  If a ROM INIC is connected and this is the first job to be performed after ::Ipl_EnterProgMode(),
 *  the INIC's test memory is cleared before programming.
 */
#define IPL_JOB_PROG_TEST_CONFIGSTRING      0x09U

/*! \brief Programs the INIC ident string from the referred IPF data.
 *
 *  If a ROM INIC is connected and this is the first job to be performed after ::Ipl_EnterProgMode(),
 *  the INIC's test memory is cleared before programming.
 */
#define IPL_JOB_PROG_IDENTSTRING            0x0AU

/*! \brief Programs the INIC test ident string from the referred IPF data.
 *
 *  If a ROM INIC is connected and this is the first job to be performed after ::Ipl_EnterProgMode(),
 *  the INIC's test memory is cleared before programming.
 */
#define IPL_JOB_PROG_TEST_IDENTSTRING       0x0BU

/*! \brief Checks if an update of configuration string is useful or not.
 *
 *  In general an update of the configuration string (CFGS) is considered as useful if the IPF data contains a Configuration
 *  string version that is higher than the configuration string version of the connected chip.
 *  \note The configuration string version is read from OTP, not from test memory.
 *
 *  CFGS version in IPF data | CFGS version in OTP of connected INIC | Result
 *  -------------------------|---------------------------------------|-------------------------------------------------------
 *  Invalid                  | Any                                   | Update is not useful (::IPL_RES_UPDATE_DENIED_UNKNOWN)
 *  Any                      | Invalid                               | Update is useful (::IPL_RES_OK)
 *  Invalid                  | Invalid                               | Update is useful (::IPL_RES_OK)
 *  Any                      | Higher than version in IPF data       | Update is not useful (::IPL_RES_UPDATE_DENIED_NEWER)
 *  Any                      | Equal to version in IPF data          | Update is not useful (::IPL_RES_UPDATE_DENIED_EQUAL)
 *  Any                      | Lower than version in IPF data        | Update is useful (::IPL_RES_OK)
 *
 *  If the configuration string does not match to the connected INIC's firmware, of course an update is also not considered as useful:
 *  Firmware version of CFGS in IPF data | Firmware version in connected INIC  | Result
 *  -------------------------------------|-------------------------------------|--------------------------------------------------------
 *  Invalid                              | Any                                 | Update is not useful (::IPL_RES_UPDATE_DENIED_UNKNOWN)
 *  Any                                  | Invalid                             | Update is not useful (::IPL_RES_UPDATE_DENIED_UNKNOWN)
 *  Invalid                              | Invalid                             | Update is not useful (::IPL_RES_UPDATE_DENIED_UNKNOWN)
 *  Any                                  | Any (different to CFGS in IPF data) | Update is not useful (::IPL_RES_ERR_IPF_WRONGFWVERSION)
 *  Any                                  | Any (same as CFGS in IPF data)      | Update is useful (::IPL_RES_OK)
 */
#define IPL_JOB_CHK_UPDATE_CONFIGSTRING     0x0CU

/*! \brief Checks if an update of firmware is useful or not.
 *
 *  In general, an update of firmware is considered as useful, if the IPF data contains a firmware version which
 *  is bigger than the firmware version of the connected chip.
 *  Firmware version in IPF data | Firmware version in connected INIC | Result
 *  -----------------------------|------------------------------------|----------------------
 *  Invalid                      | Any                                | Update is not useful (::IPL_RES_UPDATE_DENIED_UNKNOWN)
 *  Any                          | Invalid                            | Update is useful (::IPL_RES_OK)
 *  Invalid                      | Invalid                            | Update is useful (::IPL_RES_OK)
 *  Any                          | Bigger than version in IPF data    | Update is not useful (::IPL_RES_UPDATE_DENIED_NEWER)
 *  Any                          | Equal to version in IPF data       | Update is not useful (::IPL_RES_UPDATE_DENIED_EQUAL)
 *  Any                          | Smaller than version in IPF data   | Update is useful (::IPL_RES_OK)
 */
#define IPL_JOB_CHK_UPDATE_FIRMWARE        0x0DU

/*! \brief This option adds 3 jobs to only check the IPF content. */
#ifdef IPL_CHK_IPF_JOBS
/*! \brief Checks if the IPF data contains config string. */
#define IPL_JOB_CHK_IPF_CONFIGSTRING       0xE0U
/*! \brief Checks if the IPF data contains ident string. */
#define IPL_JOB_CHK_IPF_IDENTSTRING        0xE1U
/*! \brief Checks if the IPF data contains firmware. */
#define IPL_JOB_CHK_IPF_FIRMWARE           0xE2U
#endif

#ifdef IPL_TRACETAG_DUMP
/*! \brief Dumps the OTP memory to trace log. Available for ROM INICs (INICnet technology only). */
#define IPL_JOB_DUMP_OTPMEM                0xF0U
/*! \brief Dumps the Info memory to trace log. Available for FLASH INICs (INICnet technology only). */
#define IPL_JOB_DUMP_INFOMEM               0xF1U
/*! \brief Dumps the Program memory to trace log. Available for FLASH and ROM INICs (INICnet technology only). */
#define IPL_JOB_DUMP_PROGMEM               0xF2U
/*! \brief Dumps the Test memory to trace log. Available for ROM INICs (INICnet technology only). */
#define IPL_JOB_DUMP_TESTMEM               0xF3U
/*! \brief Dumps all available INIC information to trace log (INICnet technology only). */
#define IPL_JOB_DUMP_ALL                   0xFEU
#endif


/*!@}*/


/*------------------------------------------------------------------------------------------------*/
/* TYPES                                                                                          */
/*------------------------------------------------------------------------------------------------*/

typedef struct Ipl_Inic_
{
    uint8_t  ChipID;                  /*!< \brief INIC identifier. All supported ChipIDs are listed here: \ref chip_ids */
    uint8_t  FwMajorVersion;          /*!< \brief Firmware major version. */
    uint8_t  FwMinorVersion;          /*!< \brief Firmware minor version. */
    uint8_t  FwReleaseVersion;        /*!< \brief Firmware release version. */
    uint32_t FwBuildVersion;          /*!< \brief Firmware build version. */
    uint8_t  CfgsCustMajorVersion;    /*!< \brief ConfigString customer major version. */
    uint8_t  CfgsCustMinorVersion;    /*!< \brief ConfigString customer minor version. */
    uint8_t  CfgsCustReleaseVersion;  /*!< \brief ConfigString customer release version. */
} Ipl_Inic_t;


/*!
 * \defgroup bm Variables
 The data fields contain data that is derived from INIC's boot loader.
 The reading happens when the function ::Ipl_EnterProgMode() has been called successfully.
 */
/*!@{*/
extern Ipl_Inic_t Ipl_Inic;
/*!@}*/


/*------------------------------------------------------------------------------------------------*/
/* FUNCTION PROTOTYPES                                                                            */
/*------------------------------------------------------------------------------------------------*/

/*!
 * \defgroup callback Callback Functions
 * The following callback functions need to be implemented in an application and are used by the IPL to interface with the INIC hardware.
 */
/*!@{*/

/*! \brief Hardware abstraction. Callback function to set INIC's RESET_ pin.
 *  \param lowHigh       Pin setting of RESET_
 *  lowHigh | Description
 *  --------|--------------------------
 *  0       | Pull the RESET_ pin to GND
 *  1       | Release the RESET_ pin
 *  \return Possible result values:
 *  Value   | Description
 *  --------|-------------------------------
 *  0       | No error occured
 *  1...255 | Error occured when pin was set
 */
extern uint8_t  Ipl_SetResetPin(uint8_t lowHigh);

/*! \brief Hardware abstraction. Callback function to set INIC's ERR/BOOT_ pin.
 *  \param lowHigh        Pin setting of ERR/BOOT_
 *  lowHigh | Description
 *  --------|-----------------------------
 *  0       | Pull the ERR/BOOT_ pin to GND
 *  1       | Release the ERR/BOOT_ pin
 *  \return Possible result values:
 *  Value   | Description
 *  --------|-------------------------------
 *  0       | No error occured
 *  1...255 | Error occured when pin was set
 */
extern uint8_t  Ipl_SetErrBootPin(uint8_t lowHigh);

/*! \brief Hardware abstraction. Callback function to read telegram from INIC.
 *  \param lData         Length of the telegram (in bytes) to be read
 *  \param pData         Pointer to data array used for storing the telegram
 *  \return Possible result values:
 *  Value   | Description
 *  --------| --------------------------------
 *  0       | No error occured
 *  1...255 | Error occured when data was read
 */
extern uint8_t  Ipl_InicRead(uint8_t lData, uint8_t* pData);

/*! \brief Hardware abstraction. Callback function to send telegram to INIC.
 *  \param lData         Length of the telegram (in bytes) to be sent
 *  \param pData         Pointer to data array containing the telegram
 *  \return Possible result values:
 *   Value   | Description
 *   --------| --------------------------------
 *   0       | No error occured
 *   1...255 | Error occured when data was sent
 */
extern uint8_t  Ipl_InicWrite(uint8_t lData, uint8_t* pData);


/*! \brief Hardware abstraction. Callback function to sleep some milliseconds.
 *  \param timeMs        Time (in ms) to sleep
 */
extern void     Ipl_Sleep(uint16_t timeMs);
#ifdef IPL_USE_INTPIN
/*! \brief Hardware abstraction. Callback function to read INIC's INT_ pin status.
 *
 *  Optional. Only required if INT_ pin is connected and should be used.
 *  Enabled by ::IPL_USE_INTPIN.
 *  \return Possible result values:
 *  Value   | Description
 *  --------|------------------------------------
 *  0       | INT_ pin is LOW
 *  1       | INT_ pin is HIGH
 *  2...255 | Error occured when INT_ pin was read
 */
extern uint8_t  Ipl_GetIntPin(void);
#endif
/*! \brief Callback function to deliver trace information.
 *
 *  Can be left empty if no tracing is needed.
 *  \param tag           Pointer to tag indicating kind of trace information
 *  \param fmt           Pointer to trace information
 *  \param ...           Pointer to trace information
 */
extern void     Ipl_Trace(const char* tag, const char* fmt, ...);
#ifdef IPL_PROGRESS_INDICATOR
/*! \brief Callback function to deliver progress indication for long running jobs.
 *
 *  Optional.
 *  Enabled by ::IPL_PROGRESS_INDICATOR.
 *  \param percent       Percentage of job fullfillment (0-100)
 *  percent | Description
 *  --------|----------------------------------
 *  0       | Long running job has just started
 *  1..99   | Long running job is still running
 *  100     | Long running job has finished
 */
extern void     Ipl_Progress(uint8_t percent);
#endif
#ifdef IPL_INICDRIVER_OPENCLOSE
/*! \brief Hardware abstraction. Callback function to open INIC driver.
 *
 *  Optional.
 *  Enabled by ::IPL_INICDRIVER_OPENCLOSE.
 *  \return Possible result values:
 *  Value  | Description
 *  -------|--------------------------------------
 *  0      | No error occured
 *  1..255 | Error occured when driver was opened.
 */
extern uint8_t  Ipl_InicDriverOpen(void);
/*! \brief Hardware abstraction. Callback function to close INIC driver.
 *
 *  Optional.
 *  Enabled by ::IPL_INICDRIVER_OPENCLOSE.
 *  \return Possible result values:
 *  Value  | Description
 *  -------|--------------------------------------
 *  0      | No error occured
 *  1..255 | Error occured when driver was closed.
 */
extern uint8_t  Ipl_InicDriverClose(void);
#endif
#if IPL_DATACHUNK_SIZE > 0
/*! \brief Callback function to provide a portion (chunk) of the IPF data.
 *
 *  Optional. The Callback function is only needed if the data chunk feature is used.
 *  Enabled by (::IPL_DATACHUNK_SIZE > 0).
 *  \param sIndex        Start index of the byte array to be provided
 *  \param lData         Length of the byte array to be provided
 *  \return Pointer to byte array where the IPF chunk is stored
 */
extern uint8_t* Ipl_ProvideDataChunk(uint32_t sIndex, uint32_t lData);
#endif
/*!@}*/


/*!
 * \defgroup api API Functions
 * The following API functions are used to access the IPL functionality.
 * To use ::Ipl_Prog(), the function ::Ipl_EnterProgMode() needs to be called first to set INIC in programmming mode.
 * After programming, calling the function ::Ipl_LeaveProgMode() will reset the INIC and restart it in
 * normal mode.
 */
/*!@{*/

/*! \brief Resets INIC, starts up in programming mode and checks Bootloader communication.
 *
 *  Needs to be called before ::Ipl_Prog() is called.
 *  \param chipID ID of the used INIC. All supported chipIDs are listed here: \ref chip_ids
 *  \return Possible result values:
 *  Value           | Description
 *  ----------------|-----------------
 *  ::IPL_RES_OK      | No error occured
 *  IPL_RES_ERR_... | Error occured
 *  <br>
 *  All possible result values are listed here:
 *  \ref result_codes.
 *  When the function has returned ::IPL_RES_OK, the structure #Ipl_Inic
 *  contains valid values.
 */
uint8_t Ipl_EnterProgMode(uint8_t chipID);

/*! \brief Resets INIC and starts up in normal mode.
 *  Needs to be called when programming is finished.
 *  \return Possible result values:
 *  Value           | Description
 *  ----------------|-----------------
 *  ::IPL_RES_OK      | No error occured
 *  IPL_RES_ERR_... | Error occured
 *
 *  All possible result values are listed here:
 *  \ref result_codes
 */
uint8_t Ipl_LeaveProgMode(void);

/*! \brief Performs job by using the referred IPF data.
 *  INIC needs to be set in programming mode first (by calling ::Ipl_EnterProgMode()).
 *  \param job   Job to be performed. All possible jobs are listed here: \ref jobs
 *  \param lData Length of complete IPF data (file size in bytes).<br>
 *               For the job ::IPL_JOB_READ_FIRMWARE_VER, the parameter can be random (no IPF data required).
 *  \param pData Pointer to byte array where the IPF data or the first data chunk of the IPF data is stored.<br>
 *               For the job ::IPL_JOB_READ_FIRMWARE_VER, the pointer can be NULL (no IPF data required).
 *  \return Possible result values:
 *  Value           | Description
 *  ----------------|-----------------
 *  ::IPL_RES_OK    | No error occured
 *  IPL_RES_ERR_... | Error occured
 *
 *  All possible result values are listed here:
 *  \ref result_codes
 */
uint8_t Ipl_Prog(uint8_t job, uint32_t lData, uint8_t* pData);

/*!@}*/

#endif
