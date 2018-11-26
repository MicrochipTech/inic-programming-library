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

/*! \file   hw_raspi.c
 *  \brief  HW Abstraction for Raspberry Pi
 *  \author Roland Trissl (RTR)
 *  \note   For support related to this code contact http://www.microchip.com/support.
 *
 *  Used Pins:
 *  PHY+ Board          RasPI
 *  GND               - Pin 9
 *  Pin 33 (SCL)      - Pin 5 - GPIO 3 (SCL)
 *  Pin 35 (SDA)      - Pin 3 - GPIO 2 (SDA)
 *  Pin 25 (Reset)    - Pin 29 - GPIO 5
 *  Pin 27 (Err/Boot) - Pin 12 - GPIO 18
 *  Pin 34 (Int)      - Pin 31 - GPIO 6
 */


#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <linux/limits.h>


/*------------------------------------------------------------------------------------------------*/
/* USER ADJUSTABLE DEFINES                                                                        */
/*------------------------------------------------------------------------------------------------*/

/* Board specific */
#define RESET_PIN           "5"    /* GPIO 5  */
#define BOOT_PIN            "18"   /* GPIO 18 */
#define INT_PIN             "6"    /* GPIO 6  */
#define I2C_CDEV            "/dev/i2c-1"

/* HW specific */
#define HW_INIC_I2C_ADDR    (0x40>>1)      /* 0x20 */
#define HW_TRACEFILE        "IPL_Log.txt"
#define HW_TRACELINE_MAXLEN 200

/* Should be valid for all kind of Linux */
#define GPIO_FOLDER         "/sys/class/gpio/"
#define DIRECTION           "direction"
#define DIRECTION_IN        "in"
#define DIRECTION_OUT       "out"
#define VALUE               "value"
#define VALUE_0             "0"
#define VALUE_1             "1"

#define RESETCOLOR          "\033[0m"
#define GREEN               "\033[0;32m"
#define RED                 "\033[0;31m"
#define YELLOW              "\033[1;33m"
#define BLUE                "\033[0;34m"


/*------------------------------------------------------------------------------------------------*/
/* AUTOMATIC DEFINED (DO NOT CHANGE)                                                              */
/*------------------------------------------------------------------------------------------------*/

#define GPIO_EXPORT         GPIO_FOLDER "export"
#define RESET_PIN_FOLDER    GPIO_FOLDER "gpio" RESET_PIN "/"
#define BOOT_PIN_FOLDER     GPIO_FOLDER "gpio" BOOT_PIN "/"
#define INT_PIN_FOLDER      GPIO_FOLDER "gpio" INT_PIN "/"


/*------------------------------------------------------------------------------------------------*/
/* FUNCTION PROTOTYPES                                                                            */
/*------------------------------------------------------------------------------------------------*/

uint16_t Hw_GetTime(void);
char     Hw_GetKey(void);

static bool WriteCharactersToFile( const char *pFileName, const char *pString );
static bool ReadFromFile( const char *pFileName, char *pString, uint16_t bufferLen );
static bool ExistsDevice( const char *pDeviceName );
static bool WaitForDevice( const char *pDeviceName );
static void SetI2CAddress(uint8_t addr);
static const char *GetErrnoString();

int getch();
int kbhit(void);


/*------------------------------------------------------------------------------------------------*/
/* VARIABLES                                                                                      */
/*------------------------------------------------------------------------------------------------*/

static int m_fh = -1;
static uint8_t m_addr = 0xFF;
static FILE* tracefile = NULL;


/*------------------------------------------------------------------------------------------------*/
/* IPL CALLBACK FUNCTIONS                                                                         */
/*------------------------------------------------------------------------------------------------*/

uint8_t Ipl_InicDriverOpen(void)
{
    FILE *hfile = NULL;
    char tracepath[PATH_MAX];
    /* Create Logfile */
    strcpy(tracepath, getenv("HOME"));
    strcat(tracepath, "/");
    strcat(tracepath, HW_TRACEFILE);
    hfile = fopen(tracepath, "w");
    if (hfile == NULL) return 11U;
    tracefile = hfile;

    if ((m_fh = open(I2C_CDEV, O_RDWR)) < 0)
    {
        printf("Failed to open the i2c bus, error=%s\n", GetErrnoString());
        return 1U;
    }

    if (!WriteCharactersToFile(GPIO_EXPORT, RESET_PIN)) return 2U;
    if (!WriteCharactersToFile(GPIO_EXPORT, BOOT_PIN)) return 3U;
    if (!WriteCharactersToFile(GPIO_EXPORT, INT_PIN)) return 4U;

    if (!WaitForDevice(RESET_PIN_FOLDER)) return 5U;
    if (!WriteCharactersToFile(RESET_PIN_FOLDER DIRECTION, DIRECTION_OUT)) return 6U;

    if (!WaitForDevice(BOOT_PIN_FOLDER)) return 7U;
    if (!WriteCharactersToFile(BOOT_PIN_FOLDER DIRECTION, DIRECTION_OUT)) return 8U;

    if (!WaitForDevice(INT_PIN_FOLDER)) return 9U;
    if (!WriteCharactersToFile(INT_PIN_FOLDER DIRECTION, DIRECTION_IN)) return 10U;

    return 0U;
}


uint8_t Ipl_InicDriverClose(void)
{
    if (-1 == m_fh) return 1U;
    close(m_fh);
    m_fh = -1;
    m_addr = 0xFF;

    /* Close Logfile */
    if (tracefile == NULL) return 2U;
    (void)fflush(tracefile);
    fclose(tracefile);
    return 0U;
}


uint8_t Ipl_SetResetPin(uint8_t lowHigh)
{
    WriteCharactersToFile(RESET_PIN_FOLDER VALUE, lowHigh ? VALUE_1 : VALUE_0);
    return 0U;
}


uint8_t Ipl_SetErrBootPin(uint8_t lowHigh)
{
    WriteCharactersToFile(BOOT_PIN_FOLDER VALUE, lowHigh ? VALUE_1 : VALUE_0);
    return 0U;
}


uint8_t Ipl_GetIntPin(void)
{
    char buffer[4];
    if (ReadFromFile(INT_PIN_FOLDER VALUE, buffer, sizeof(buffer)))
    {
        if (0 == strcmp(VALUE_1, buffer))
        {
            return 1U;
        }
        else
        {
            return 0U;
        }
    }
    return 2U;
}


uint8_t Ipl_InicWrite(uint8_t lData, uint8_t* pData)
{
    if (-1 == m_fh) return 1U;
    SetI2CAddress(HW_INIC_I2C_ADDR);
    if (write(m_fh, pData, lData) != lData)
    {
        printf("LldInic_I2cWrite failed, error=%s\n", GetErrnoString());
        return 2U;
    }
    return 0U;
}


uint8_t Ipl_InicRead(uint8_t lData, uint8_t* pData)
{
    if (-1 == m_fh) return 1U;
    SetI2CAddress(HW_INIC_I2C_ADDR);
    if (read(m_fh, pData, lData) != lData)
    {
        printf("LldInic_I2cWrite failed, error=%s\n", GetErrnoString());
        return 2U;
    }
    return 0U;
}


void Ipl_Sleep(uint32_t TimeMs)
{
    usleep(1000 * TimeMs);
}


void Ipl_Trace(const char *tag, const char* fmt, ...)
{
    va_list args;
    if ( NULL != tracefile)
    {
        if ( NULL != tag )
        {
            fprintf(tracefile,"%s %06d ", tag, Hw_GetTime());
            va_start(args, fmt);
            vfprintf(tracefile, fmt, args);
            va_end(args);
            fprintf(tracefile, "\n");
            fflush(tracefile);
        }
    }
    /* Print on stderr
    if ( NULL != tag )
    {
        fprintf(stderr,"%s %06d ", tag, Hw_GetTime());
        va_list args;
        va_start(args, fmt);
        vfprintf(stderr, fmt, args);
        va_end(args);
        fprintf(stderr, "\r\n");
    } */
}


uint16_t Hw_GetTime(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint16_t)((uint64_t)(ts.tv_sec) * 1000) + (uint16_t)(ts.tv_nsec / 1000000);
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


/*------------------------------------------------------------------------------------------------*/
/* PRIVATE FUNCTION IMPLEMENTATIONS                                                               */
/*------------------------------------------------------------------------------------------------*/

static bool WriteCharactersToFile( const char *pFileName, const char *pString )
{
    bool success = false;
    FILE *fh = fopen( pFileName, "a" );
    if( NULL != fh )
    {
        int result = fputs( pString, fh );
        if( result >= 0 )
            fputc( '\n', fh );
        if( result >= 0 )
            success = true;
        fclose( fh );
    }
    if (!success)
    {
        printf( "WriteCharactersToFile failed for file '%s', errno:'%s'\n", pFileName, GetErrnoString() );
    }
    return success;
}


static bool ReadFromFile( const char *pFileName, char *pString, uint16_t bufferLen )
{
    FILE *fh;
    bool success = false;
    if( NULL == pString || 0 == bufferLen )
        return success;
    fh = fopen( pFileName, "r" );
    if( NULL != fh )
    {
        success = ( NULL != fgets( pString, bufferLen, fh ) );
        fclose( fh );
    }
    if( !success )
    {
         printf( "ReadFromFile failed for file '%s', errno:'%s'\n", pFileName, GetErrnoString() );
    }
    return success;
}


static bool ExistsDevice( const char *pDeviceName )
{
    struct stat buffer;
    return ( stat( pDeviceName, &buffer ) == 0 );
}


static bool WaitForDevice( const char *pDeviceName )
{
    int timeout;
    bool deviceExists = false;
    for( timeout = 0; timeout < 40; timeout++ )
    {
        deviceExists = ExistsDevice( pDeviceName );
        if( deviceExists )
        {
            break;
        }
        else
        {
            usleep( 2500 );
        }
    }
    if( !deviceExists )
    {
        printf( "Waiting for device '%s' to appear, timed out\n", pDeviceName );
    }
    return deviceExists;
}


static void SetI2CAddress(uint8_t addr)
{
    if (addr == m_addr) return;
    if (ioctl(m_fh, I2C_SLAVE, addr) < 0)
    {
        printf("Could not find slave address of INIC=0x%X.\n", addr);
        exit(1);
    }
    m_addr = addr;
}


static const char *GetErrnoString()
{
    switch( errno )
    {
    case 0:
        return "Nothing stored in errno";
    case 1:
        return "Operation not permitted";
    case 2:
        return "No such file or directory";
    case 3:
        return "No such process";
    case 4:
        return "Interrupted system call";
    case 5:
        return "I/O error";
    case 6:
        return "No such device or address";
    case 7:
        return "Argument list too long";
    case 8:
        return "Exec format error";
    case 9:
        return "Bad file number";
    case 10:
        return "No child processes";
    case 11:
        return "Try again";
    case 12:
        return "Out of memory";
    case 13:
        return "Permission denied";
    case 14:
        return "Bad address";
    case 15:
        return "Block device required";
    case 16:
        return "Device or resource busy";
    case 17:
        return "File exists";
    case 18:
        return "Cross-device link";
    case 19:
        return "No such device";
    case 20:
        return "Not a directory";
    case 21:
        return "Is a directory";
    case 22:
        return "Invalid argument";
    case 23:
        return "File table overflow";
    case 24:
        return "Too many open files";
    case 25:
        return "Not a typewriter";
    case 26:
        return "Text file busy";
    case 27:
        return "File too large";
    case 28:
        return "No space left on device";
    case 29:
        return "Illegal seek";
    case 30:
        return "Read-only file system";
    case 31:
        return "Too many links";
    case 32:
        return "Broken pipe";
    case 33:
        return "Math argument out of domain of func";
    case 34:
        return "Math result not representable";
    default:
        break;
    }
    return "Unknown";
}


int kbhit(void)
{
    struct termios term, oterm;
    int fd = 0;
    int c = 0;
    tcgetattr(fd, &oterm);
    memcpy(&term, &oterm, sizeof(term));
    term.c_lflag = term.c_lflag & (!ICANON);
    term.c_cc[VMIN] = 0;
    term.c_cc[VTIME] = 1;
    tcsetattr(fd, TCSANOW, &term);
    c = getchar();
    tcsetattr(fd, TCSANOW, &oterm);
    if (c != -1)
    ungetc(c, stdin);
    return ((c != -1) ? 1 : 0);
}


int getch()
{
    static int ch = -1, fd = 0;
    struct termios neu, alt;
    fd = fileno(stdin);
    tcgetattr(fd, &alt);
    neu = alt;
    neu.c_lflag &= ~(ICANON|ECHO);
    tcsetattr(fd, TCSANOW, &neu);
    ch = getchar();
    tcsetattr(fd, TCSANOW, &alt);
    return ch;
}
