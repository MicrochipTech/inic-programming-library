/*------------------------------------------------------------------------------------------------*/
/* (c) 2022 Microchip Technology Inc. and its subsidiaries.                                       */
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

/*! \file   ipl_xtra.c
 *  \brief  Xtra functionality for INIC Programming Library
 *  \author Roland Trissl (RTR)
 *  \note   For support related to this code contact http://www.microchip.com/support.
 */

#include <stdint.h>
#include "ipl_cfg.h"
#include "ipl_pb.h"
#include "ipl.h"
#include "ipf.h"

#ifdef IPL_XTRA

#include "ipl_xtra.h"

/*------------------------------------------------------------------------------------------------*/
/* PROTOTYPES                                                                                     */
/*------------------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------------------*/
/* FUNCTIONS                                                                                      */
/*------------------------------------------------------------------------------------------------*/

/*! \internal Write IO registers. */
uint8_t Ipl_WriteIOReg(uint8_t reg, uint16_t val, uint16_t mask)
{
    uint8_t  res = IPL_RES_ERR_NOT_SUPPORTED;
    Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_WriteIOReg called");
    Ipl_ClrTel();
    Ipl_IplData.Tel[0] = CMD_WRITEIOREG;
    Ipl_IplData.Tel[1] = 0x00U;
    Ipl_IplData.Tel[2] = reg;
    Ipl_IplData.Tel[3] = 4U;
    Ipl_IplData.Tel[4] = (val >> 8) & 0xFFU;
    Ipl_IplData.Tel[5] = val & 0xFFU;
    Ipl_IplData.Tel[6] = (mask >> 8) & 0xFFU;
    Ipl_IplData.Tel[7] = mask & 0xFFU;
    Ipl_IplData.TelLen = CMD_WRITEIOREG_TXLEN;
    res = Ipl_ExecInicCmd();
    Ipl_Trace(Ipl_TraceTag(res), "Ipl_WriteIOReg 0x%02X, Value: 0x%04X, Mask: 0x%04X returned 0x%02X", reg, val, mask, res);
    return res;
}

/*! \internal Read IO registers. */
uint8_t Ipl_ReadIOReg(uint8_t reg, uint16_t *val)
{
    uint8_t res = IPL_RES_ERR_NOT_SUPPORTED;
    Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_ReadIOReg called");
    Ipl_ClrTel();
    Ipl_IplData.Tel[0] = CMD_READIOREG;
    Ipl_IplData.Tel[1] = 0x00U;
    Ipl_IplData.Tel[2] = reg;
    Ipl_IplData.Tel[3] = 1U;
    Ipl_IplData.TelLen = CMD_READIOREG_TXLEN;
    res = Ipl_ExecInicCmd();
    val = (uint16_t) ( (Ipl_IplData.Tel[4] << 8) + Ipl_IplData.Tel[5] );
    Ipl_Trace(Ipl_TraceTag(res), "Ipl_ReadIOReg 0x%02X returned 0x%02X, Value: 0x%04X", reg, res, val);
    return res;
}

/*! \internal Enables and disables RetimedBypass mode. */
uint8_t Ipl_SetRetimedBypass(uint8_t onOff)
{
    uint8_t res = IPL_RES_ERR_NOT_SUPPORTED;
    Ipl_Trace(IPL_TRACETAG_INFO, "Ipl_SetRetimedBypass called");
    //res = Ipl_WriteIOReg(uint8_t reg, uint16_t val, uint16_t mask);
    Ipl_Trace(Ipl_TraceTag(res), "Ipl_SetRetimedBypass %01X returned 0x%02X", onOff, res);
    return res;
}

#endif
