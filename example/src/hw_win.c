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

/*! \file   hw_win.c
 *  \brief  HW Abstraction for Aardvark I2C Host and Windows PC (polling mode).
 *  \author Roland Trissl (RTR)
 *  \note   For support related to this code contact http://www.microchip.com/support.
 *
 *  Used Pins:
 *  PHY+ Board Connector    Chip        Aardvark
 *  GND                                 Red (GND)
 *  Pin 33 (SCL)                        Brown (SCL)
 *  Pin 35 (SDA)                        Orange (SDA)
 *  Pin 25 (Reset)                      Green (MISO)
 *  Pin 27 (Err/Boot)                   Gray (MOSI)
 *
 *  Pin 34 (Int)                        White (SS) - Optional
 *  Pin 32 (DSDA/TDI)       Pin 13      Orange (SDA) - Debug
 *  Pin 29 (DSCL/TCK)       Pin 14      Brown  (SCL) - Debug
 */


#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <conio.h>
#include <Windows.h>
#include "aardvark.h"
#include "ipl_cfg.h"


/*------------------------------------------------------------------------------------------------*/
/* CONSTANTS                                                                                      */
/*------------------------------------------------------------------------------------------------*/

#define ENABLE_TIMESTAMP

#define HW_INIC_I2C_ADDR         0x20
#define HW_TRACEFILE             "IPL_Log.txt"

#define AARD_MAX_DATA_LEN        72 /* port messages of type control are limited to 72 bytes */

#define AARD_PORT                0
#define AARD_BITRATE             400
#define AARD_DEV                 0x20
#define AARD_TGT                 0x20

#define RESETCOLOR               "\033[0m"
#define GREEN                    "\033[0;32m"
#define RED                      "\033[0;31m"
#define YELLOW                   "\033[1;33m"
#define BLUE                     "\033[0;34m"


/*------------------------------------------------------------------------------------------------*/
/* FUNCTION PROTOTYPES                                                                            */
/*------------------------------------------------------------------------------------------------*/

uint16_t Hw_GetTime(void);
char     Hw_GetKey(void);


/*------------------------------------------------------------------------------------------------*/
/* VARIABLES                                                                                      */
/*------------------------------------------------------------------------------------------------*/

static          FILE* tracefile = NULL;
Aardvark        aard_handle;
uint16_t        aard_device;
uint16_t        aard_tgt;
uint16_t        aard_port;
static uint16_t aard_gpio;
uint8_t         tx_buffer[AARD_MAX_DATA_LEN];
uint8_t         rx_buffer[AARD_MAX_DATA_LEN];


/*------------------------------------------------------------------------------------------------*/
/* IPL CALLBACK FUNCTIONS                                                                         */
/*------------------------------------------------------------------------------------------------*/

uint8_t Ipl_InicDriverOpen(void)
{
    //FILE *hfile = NULL;
    aard_device = AARD_DEV;
    aard_tgt    = AARD_TGT;
    aard_port   = AARD_PORT;
    //hfile = fopen(HW_TRACEFILE, "w"); /* Create empty trace file */
    //if (hfile != NULL)
    //{
      //  tracefile = hfile;
    //}
    aard_handle = aa_open(aard_port); /* Open the device */
    if (aard_handle <= 0)
    {
        printf("Unable to open Aardvark device on port %d\n", aard_port);
        printf("Error code = %d\n", aard_handle);
        return 1U;
    }
    aa_configure(aard_handle, AA_CONFIG_GPIO_I2C); /* Ensure that the I2C subsystem is enabled  */
    aa_i2c_pullup(aard_handle, AA_I2C_PULLUP_BOTH); /* Enable the I2C bus pullup resistors (2.2k) */
    aa_i2c_bitrate(aard_handle, AARD_BITRATE); /* Set the bitrate */
    aard_gpio = AA_GPIO_MISO + AA_GPIO_MOSI;
    aa_gpio_direction(aard_handle, aard_gpio); /* Now configure GPIO output pins */
    aa_gpio_pullup(aard_handle, 0x00U); /* GPIO disable pullups */
    aa_gpio_set(aard_handle, aard_gpio); /* Set GPIO outputs to HIGH */
    return 0U;
}


uint8_t Ipl_InicDriverClose(void)
{
    if (tracefile == NULL)
    {
        return 1U;
    }
    (void)fflush(tracefile);

    if (aard_handle <= 0)
    {
        fclose(tracefile); // Close only if no error to keep logfile
    }
    aa_close(aard_handle);
    return 0U;
}


uint8_t Ipl_SetResetPin(uint8_t lowHigh)
{
    if (0 == lowHigh)
    {
        aard_gpio = ((aard_gpio | AA_GPIO_MISO) - AA_GPIO_MISO);
    }
    else
    {
        aard_gpio = (aard_gpio | AA_GPIO_MISO);
    }
    aa_gpio_set(aard_handle, aard_gpio);
    return 0U;
}


uint8_t Ipl_SetErrBootPin(uint8_t lowHigh)
{
    if (0U == lowHigh)
    {
        aard_gpio = ((aard_gpio | AA_GPIO_MOSI) - AA_GPIO_MOSI);
    }
    else
    {
        aard_gpio = (aard_gpio | AA_GPIO_MOSI);
    }
    aa_gpio_set(aard_handle, aard_gpio);
    return 0U;
}


uint8_t Ipl_InicWrite(uint8_t lData, uint8_t* pData)
{
    memcpy(&tx_buffer[0], pData, lData);
    aa_i2c_write(aard_handle, HW_INIC_I2C_ADDR, AA_I2C_NO_FLAGS, lData, tx_buffer);
    return 0U;
}


uint8_t Ipl_InicRead(uint8_t lData, uint8_t* pData)
{
    aa_i2c_read(aard_handle, HW_INIC_I2C_ADDR, AA_I2C_NO_FLAGS, lData, rx_buffer);
    memcpy(pData, rx_buffer, lData);
    return 0U;
}


void Ipl_Sleep(uint16_t TimeMs)
{
    Sleep(TimeMs);
}


uint8_t Ipl_GetIntPin(void)
{
    uint8_t val, res;
    val = aa_gpio_get(aard_handle);
    res = 0U;
    if (val && AA_GPIO_SS)
    {
        res = 1U;
    }
    return res;
}


void Ipl_Trace(const char *tag, const char* fmt, ...)
{
    va_list args;

    if ( NULL == tracefile)
    {
        FILE *hfile = NULL;
        hfile = fopen(HW_TRACEFILE, "w"); /* Create empty trace file */
        if (hfile != NULL)
        {
            tracefile = hfile;
        }
    }

    if ( NULL != tracefile)
    {
        if ( NULL != tag )
        {
#ifdef ENABLE_TIMESTAMP
            fprintf(tracefile,"%s %06d ", tag, Hw_GetTime());
#else
            fprintf(tracefile,"%s ", tag);
#endif
            va_start(args, fmt);
            vfprintf(tracefile, fmt, args);
            va_end(args);
            fprintf(tracefile, "\n");
            fflush(tracefile);
        }
    }
}


/*------------------------------------------------------------------------------------------------*/
/* FUNCTIONS                                                                                      */
/*------------------------------------------------------------------------------------------------*/

uint16_t Hw_GetTime(void)
{
    return timeGetTime();
}


char Hw_GetKey(void)
{
    char ch = ' ';
    while ( 0U != kbhit() )
    {
        ch = getch();
    }
    return ch;
}
