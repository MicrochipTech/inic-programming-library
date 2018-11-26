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

/*! \file   ipl_cfg.h
 *  \brief  Configuration header for INIC Programming Library
 *  \author Roland Trissl (RTR)
 *  \note   For support related to this code contact http://www.microchip.com/support.
 */

#ifndef IPL_CFG_H
#define IPL_CFG_H

/*! \defgroup conf_inics Supported INICs
 *  \ingroup  conf
 *  Please select all the INICs your programming application should support.
 */
/*!@{*/

/*! \brief Adds the functions needed for OS81118. */
#define IPL_USE_OS81118
/*! \brief Adds the functions needed for OS81119. */
#define IPL_USE_OS81119
/*! \brief Adds the functions needed for OS81210. */
#define IPL_USE_OS81210
/*! \brief Adds the functions needed for OS81212. */
#define IPL_USE_OS81212
/*! \brief Adds the functions needed for OS81214. */
#define IPL_USE_OS81214
/*! \brief Adds the functions needed for OS81216. */
#define IPL_USE_OS81216

/*!@}*/


/*! \defgroup intpin INT_ Pin Support
 *  \ingroup  conf
 *  Please select, if the INT_ pin of INIC should be used in the communication with IPL. If the
 *  macro is defined, application needs to provide the callback function ::Ipl_GetIntPin() that reads INIC's INT_ pin and
 *  returns the result. If the macro is not defined, IPL will use sleep with reasonable timings.
 */
/*!@{*/

/*! Enables usage of the INIC's INT_ pin for RX control.
    If the macro is defined, the additional callback function ::Ipl_GetIntPin() needs to be implemented.
    The function reads the status of INIC's INT_ pin.
    If the macro is not defined, IPL does not check the INIC's INT_ pin and the callback function
    ::Ipl_GetIntPin() is not required.
*/

// #define IPL_USE_INTPIN

/*!@}*/


/*! \defgroup driver_openclose INIC Driver Open/Close Callback Functions
 *  \ingroup  conf
 *  If your application has functions for setting up the INIC (I2C) driver, you can let them call
 *  by the IPL via the callback functions ::Ipl_InicDriverOpen() and ::Ipl_InicDriverClose().
 */
/*!@{*/

/*! Enables additional callbacks to open/close the INIC driver.
 *  Callbacks: ::Ipl_InicDriverOpen(), ::Ipl_InicDriverClose().
 */

#define IPL_INICDRIVER_OPENCLOSE

/*!@}*/

/*! \defgroup progress_indicator Progress Indicator Callback Function
 *  \ingroup  conf
 *  For tasks that take longer the progress indicator callback ::Ipl_Progress() can be enabled.
 */

/*!@{*/

/*! Enables the progress indicator callback ::Ipl_Progress(). */

#define IPL_PROGRESS_INDICATOR

/*!@}*/

/*! \defgroup chunk_handling Data Chunk Handling
 *  \ingroup  conf
 *  Adds a callback function to provide single data chunks instead of the complete IPF data.
 *  Useful if there is not enough RAM to store the complete IPF file.
 */

/*!@{*/

/*! \brief Size of data chunks in bytes. 0 means that the complete IPF file is stored in the memory.
 *
 *  Smallest value (besides 0) is 2048.
 *  If the value is unequal to 0, the additional callback function
 *  ::Ipl_ProvideDataChunk() is used for the application to provide
 *  the respective data chunk.
 *  If the value is 0, the complete IPF data is referred in the ::Ipl_Prog() call (no callback is used).
 */

#define IPL_DATACHUNK_SIZE 2048

/*!@}*/

/*! \defgroup trace_tags Trace Tags
 *  \ingroup  conf
 *  Tags used for trace info and trace error output.
 */
/*!@{*/

/*! \brief Tag used for normal information tracing. */
#define IPL_TRACETAG_INFO    "IPL    "

/*! \brief Tag used for error tracing. */
#define IPL_TRACETAG_ERR     "IPL ERR"

/*! \brief Tag used for IPF data tracing. If tag is not defined IPF data is not traced. */
#define IPL_TRACETAG_IPF     "IPL IPF"

/*! \brief Tag used for INIC communication tracing. If tag is not defined communication with INIC is not traced. */
#define IPL_TRACETAG_COM     "IPL COM"


/*!@}*/

#endif
