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

/*! \file   main.c
 *  \brief  Example Application for INIC Programming Library
 *  \author Roland Trissl (RTR)
 *  \note   For support related to this code contact http://www.microchip.com/support.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include "ipl_cfg.h"
#include "ipl_pb.h"


/*------------------------------------------------------------------------------------------------*/
/* EXTERNAL FUNCTIONS                                                                             */
/*------------------------------------------------------------------------------------------------*/

extern char Hw_GetKey(void);


/*------------------------------------------------------------------------------------------------*/
/* CONSTANTS                                                                                      */
/*------------------------------------------------------------------------------------------------*/

#define IMAGE_MAXLEN    200000U
#define FILENAME_MAXLEN 1024U

#ifndef STDIN_FILENO
#define STDIN_FILENO    0
#endif


/*------------------------------------------------------------------------------------------------*/
/* VARIABLES                                                                                      */
/*------------------------------------------------------------------------------------------------*/

uint8_t  image[IMAGE_MAXLEN];
int32_t  imageLen;
uint8_t  chipid = IPL_CHIP_OS81210;
char     ipffile[FILENAME_MAXLEN];


/*------------------------------------------------------------------------------------------------*/
/* FUNCTION PROTOTYPES                                                                            */
/*------------------------------------------------------------------------------------------------*/

uint32_t load_ipf(char* fileName, uint32_t posData, uint32_t lData, uint8_t* pData);
uint8_t  exec_job(uint8_t job);
void     print_menu(void);


/*------------------------------------------------------------------------------------------------*/
/* IPL CALLBACK FUNCTIONS                                                                         */
/*------------------------------------------------------------------------------------------------*/

void Ipl_Progress(uint8_t percent)
{
    printf("\b\b\b\b\b%3d%%]", percent);
    fflush(stdout);
}

uint8_t * Ipl_ProvideDataChunk(uint32_t sIndex, uint32_t lData)
{
    imageLen = load_ipf(ipffile, sIndex, lData, image);
    return image;
}


/*------------------------------------------------------------------------------------------------*/
/* FUNCTIONS                                                                                      */
/*------------------------------------------------------------------------------------------------*/

/* Loads an IPF file or a portion of it into an array. */
uint32_t load_ipf(char* fileName, uint32_t posData, uint32_t lData, uint8_t* pData)
{
    int32_t rd, ls;
    int32_t tl = -1;
    FILE *fp;

    fp = fopen(fileName, "rb");
    if (fp != NULL)
    {
        fseek(fp, 0, SEEK_END); /* File length */
        tl = ftell(fp);
        if (lData == 0)
        {
            lData = tl;
        }
        fseek(fp, posData, SEEK_SET);
        ls = ftell(fp);
        if (ls == posData)
        {
            rd = fread(pData, lData, 1, fp);
            if (rd >= 0)
            {
                Ipl_Trace("EXAMPLE", "File %s loaded from pos %u, len %u bytes, total %u bytes", fileName, posData, lData, tl);
            }
            else
            {
                tl = -1;
                Ipl_Trace("EXAMPLE", "IPF file read error");
            }
        }
        else
        {
            tl = -1;
            Ipl_Trace("EXAMPLE", "IPF file position invalid");
        }
    }
    else
    {
        tl = -1;
        Ipl_Trace("EXAMPLE", "IPF file %s does not exist", fileName);
    }
    fclose(fp);
    return tl;
}


/* Prints the main menu of the example application (interactive mode). */
void print_menu(void)
{
    printf("\nINIC Programming Library Example - Chip: ");
    switch (chipid)
    {
        case IPL_CHIP_OS81118: printf("OS81118, "); break;
        case IPL_CHIP_OS81119: printf("OS81119, "); break;
        case IPL_CHIP_OS81210: printf("OS81210, "); break;
        case IPL_CHIP_OS81212: printf("OS81212, "); break;
        case IPL_CHIP_OS81214: printf("OS81214, "); break;
        case IPL_CHIP_OS81216: printf("OS81216, "); break;
        case IPL_CHIP_OS81050: printf("OS81050, "); break;
        case IPL_CHIP_OS81060: printf("OS81060, "); break;
        case IPL_CHIP_OS81082: printf("OS81082, "); break;
        case IPL_CHIP_OS81092: printf("OS81092, "); break;
        case IPL_CHIP_OS81110: printf("OS81110, "); break;
    }
#ifndef IPL_USE_INTPIN
    printf("no ");
#endif
    printf("INT pin used.\n");
    printf("------------------------------------------------------------------------\n");
    printf("[0] - Change Chip \n");
    printf("[1] - EnterProgMode \n");
    printf("[2] - LoadIpf \n");
    printf("[u] - CheckUpdateConfig / CheckUpdateFirmware\n");
    printf("[9] - LeaveProgMode \n");
    printf("------------------------------------------------------------------------\n");
    printf("[3] - ReadFirmwareVersion \n");
    printf("[4] - ReadConfigStringVersion \n");
    printf("------------------------------------------------------------------------\n");
    printf("[5] - ProgramConfiguration             [a] - ProgramTestConfiguration   \n");
    printf("[6] - ProgramFirmware \n");
    printf("------------------------------------------------------------------------\n");
    printf("[c] - ProgramConfigString              [d] - ProgramTestConfigString    \n");
    printf("[e] - ProgramIdentString               [f] - ProgramTestIdentString     \n");
    printf("[7] - ProgramPatchString               [b] - ProgramTestPatchString     \n");
    printf("------------------------------------------------------------------------\n");
    printf("[x] - Exit \n\n");
}


/* Changes the current Chip ID. */
void change_chipid(void)
{
    switch (chipid)
    {
        case IPL_CHIP_OS81118:
            chipid = IPL_CHIP_OS81119;
            break;
        case IPL_CHIP_OS81119:
            chipid = IPL_CHIP_OS81210;
            break;
        case IPL_CHIP_OS81210:
            chipid = IPL_CHIP_OS81212;
            break;
        case IPL_CHIP_OS81212:
            chipid = IPL_CHIP_OS81214;
            break;
        case IPL_CHIP_OS81214:
            chipid = IPL_CHIP_OS81216;
            break;
        case IPL_CHIP_OS81216:
            chipid = IPL_CHIP_OS81050;
            break;
        case IPL_CHIP_OS81050:
            chipid = IPL_CHIP_OS81060;
            break;
        case IPL_CHIP_OS81060:
            chipid = IPL_CHIP_OS81082;
            break;
        case IPL_CHIP_OS81082:
            chipid = IPL_CHIP_OS81092;
            break;
        case IPL_CHIP_OS81092:
            chipid = IPL_CHIP_OS81110;
            break;
        case IPL_CHIP_OS81110:
        default:
            chipid = IPL_CHIP_OS81118;
            break;
    }
    print_menu();
}


/* Performs a Job and displays the result. */
uint8_t exec_job(uint8_t job)
{
    uint8_t res;
    switch (job)
    {
        case IPL_JOB_READ_CONFIGSTRING_VER:
            res = Ipl_Prog(job, imageLen, image);
            printf("ReadConfigStringVersion 0x%02X ", res);
            fflush(stdout);
            if (IPL_RES_OK == res)
            {
                printf("- V%02u.%02u.%02u\n", Ipl_Inic.CfgsCustMajorVersion, Ipl_Inic.CfgsCustMinorVersion,
                       Ipl_Inic.CfgsCustReleaseVersion);
            }
            else
            {
                printf("\n");
            }
            break;
        case IPL_JOB_PROG_FIRMWARE:
            printf("ProgramFirmware [  0%%]");
            fflush(stdout);
            res = Ipl_Prog(job, imageLen, image);
            printf("\b\b\b\b\b\b0x%02X     \n", res);
            break;
        case IPL_JOB_PROG_CONFIG:
            printf("ProgramConfiguration [  0%%]");
            fflush(stdout);
            res = Ipl_Prog(job, imageLen, image);
            printf("\b\b\b\b\b\b0x%02X     \n", res);
            break;
        case IPL_JOB_PROG_PATCHSTRING:
            printf("ProgramPatchString [  0%%]");
            fflush(stdout);
            res = Ipl_Prog(job, imageLen, image);
            printf("\b\b\b\b\b\b0x%02X     \n", res);
            break;
        case IPL_JOB_PROG_TEST_CONFIG:
            printf("ProgramTestConfiguration [  0%%]");
            fflush(stdout);
            res = Ipl_Prog(job, imageLen, image);
            printf("\b\b\b\b\b\b0x%02X     \n", res);
            break;
        case IPL_JOB_PROG_TEST_PATCHSTRING:
            printf("ProgramTestPatchString [  0%%]");
            fflush(stdout);
            res = Ipl_Prog(job, imageLen, image);
            printf("\b\b\b\b\b\b0x%02X     \n", res);
            break;
        case IPL_JOB_PROG_CONFIGSTRING:
            printf("ProgramConfigString [  0%%]");
            fflush(stdout);
            res = Ipl_Prog(job, imageLen, image);
            printf("\b\b\b\b\b\b0x%02X     \n", res);
            break;
        case IPL_JOB_PROG_TEST_CONFIGSTRING:
            printf("ProgramTestConfigString [  0%%]");
            fflush(stdout);
            res = Ipl_Prog(job, imageLen, image);
            printf("\b\b\b\b\b\b0x%02X     \n", res);
            break;
        case IPL_JOB_PROG_IDENTSTRING:
            printf("ProgramIdentString [  0%%]");
            fflush(stdout);
            res = Ipl_Prog(job, imageLen, image);
            printf("\b\b\b\b\b\b0x%02X     \n", res);
            break;
        case IPL_JOB_PROG_TEST_IDENTSTRING:
            printf("ProgramTestIdentString [  0%%]");
            fflush(stdout);
            res = Ipl_Prog(job, imageLen, image);
            printf("\b\b\b\b\b\b0x%02X     \n", res);
            break;
        case IPL_JOB_READ_FIRMWARE_VER:
            res = Ipl_Prog(job, imageLen, image);
            printf("ReadFirmwareVersion 0x%02X ", res);
            fflush(stdout);
            if (IPL_RES_OK == res)
            {
                printf("- ChipID: 0x%2X - V%02u.%02u.%02u-%02u\n", Ipl_Inic.ChipID, Ipl_Inic.FwMajorVersion,
                       Ipl_Inic.FwMinorVersion, Ipl_Inic.FwReleaseVersion, Ipl_Inic.FwBuildVersion);
            }
            else
            {
                printf("\n");
            }
            break;
        case IPL_JOB_CHK_UPDATE_CONFIGSTRING:
            res = Ipl_Prog(job, imageLen, image);
            printf("CheckUpdateConfigString 0x%02X \n", res);
            fflush(stdout);
            break;
        case IPL_JOB_CHK_UPDATE_FIRMWARE:
            res = Ipl_Prog(job, imageLen, image);
            printf("CheckUpdateFirmware 0x%02X \n", res);
            fflush(stdout);
            break;
        default:
            printf("Job [0x%02X] [  0%%]", job);
            fflush(stdout);
            res = Ipl_Prog(job, imageLen, image);
            printf("\b\b\b\b\b\b0x%02X     \n", res);
            break;
    }
    return res;
}


/*------------------------------------------------------------------------------------------------*/
/* EXAMPLE APPLICATION                                                                            */
/*------------------------------------------------------------------------------------------------*/

int main(int argc, char** argv)
{
    uint8_t  ch;
    uint8_t  res;
    uint8_t  jobid = 0x00;
    uint32_t i;
    bool     err_syntax = false;
    bool     app_stop   = false;
    for (i=0U; i<IMAGE_MAXLEN; i++)
    {
        image[i] = 0x00;
    }
    if ( argc == 5 || argc == 7 ) /* Parse arguments */
    {
        if ( 0 == strcmp(argv[1], "-INIC") )
        {
            if      ( 0 == strcmp(argv[2], "OS81118") ) chipid = IPL_CHIP_OS81118;
            else if ( 0 == strcmp(argv[2], "OS81119") ) chipid = IPL_CHIP_OS81119;
            else if ( 0 == strcmp(argv[2], "OS81210") ) chipid = IPL_CHIP_OS81210;
            else if ( 0 == strcmp(argv[2], "OS81212") ) chipid = IPL_CHIP_OS81212;
            else if ( 0 == strcmp(argv[2], "OS81214") ) chipid = IPL_CHIP_OS81214;
            else if ( 0 == strcmp(argv[2], "OS81216") ) chipid = IPL_CHIP_OS81216;
            else if ( 0 == strcmp(argv[2], "OS81050") ) chipid = IPL_CHIP_OS81050;
            else if ( 0 == strcmp(argv[2], "OS81060") ) chipid = IPL_CHIP_OS81060;
            else if ( 0 == strcmp(argv[2], "OS81082") ) chipid = IPL_CHIP_OS81082;
            else if ( 0 == strcmp(argv[2], "OS81092") ) chipid = IPL_CHIP_OS81092;
            else if ( 0 == strcmp(argv[2], "OS81110") ) chipid = IPL_CHIP_OS81110;
            else err_syntax = true;
        }
        else err_syntax = true;
        if ( argc == 7U )
        {
            if ( 0 == strcmp(argv[5], "-IPF") )
            {
                strcpy(ipffile, argv[6]);
            }
            else err_syntax = true;
        }
        if ( 0 == strcmp(argv[3], "-JOB") )
        {
            if      ( 0 == strcmp(argv[4], "READ_FIRMWARE_VER" ) )         jobid = IPL_JOB_READ_FIRMWARE_VER;
            else if ( 0 == strcmp(argv[4], "READ_CONFIGSTRING_VER" ) )     jobid = IPL_JOB_READ_CONFIGSTRING_VER;
            else if ( 0 == strcmp(argv[4], "PROG_FIRMWARE" ) )             jobid = IPL_JOB_PROG_FIRMWARE;
            else if ( 0 == strcmp(argv[4], "PROG_CONFIG" ) )               jobid = IPL_JOB_PROG_CONFIG;
            else if ( 0 == strcmp(argv[4], "PROG_PATCHSTRING" ) )          jobid = IPL_JOB_PROG_PATCHSTRING;
            else if ( 0 == strcmp(argv[4], "PROG_TEST_CONFIG" ) )          jobid = IPL_JOB_PROG_TEST_CONFIG;
            else if ( 0 == strcmp(argv[4], "PROG_TEST_PATCHSTRING" ) )     jobid = IPL_JOB_PROG_TEST_PATCHSTRING;
            else if ( 0 == strcmp(argv[4], "PROG_CONFIGSTRING" ) )         jobid = IPL_JOB_PROG_CONFIGSTRING;
            else if ( 0 == strcmp(argv[4], "PROG_TEST_CONFIGSTRING" ) )    jobid = IPL_JOB_PROG_TEST_CONFIGSTRING;
            else if ( 0 == strcmp(argv[4], "PROG_IDENTSTRING" ) )          jobid = IPL_JOB_PROG_IDENTSTRING;
            else if ( 0 == strcmp(argv[4], "PROG_TEST_IDENTSTRING" ) )     jobid = IPL_JOB_PROG_TEST_IDENTSTRING;
            else if ( 0 == strcmp(argv[4], "CHK_UPDATE_CONFIGSTRING" ) )   jobid = IPL_JOB_CHK_UPDATE_CONFIGSTRING;
            else if ( 0 == strcmp(argv[4], "CHK_UPDATE_FIRMWARE" ) )       jobid = IPL_JOB_CHK_UPDATE_FIRMWARE;
            else err_syntax = true;
        }
        else err_syntax = true;
    }
    else if ( argc > 1U )
    {
        err_syntax = true;
    }
    printf("\n");
    for (i=0U; i<argc; i++)
    {
        printf("%s ", argv[i]);
    }
    if ( (!err_syntax) && (argc > 1) )
    {
        res = Ipl_EnterProgMode(chipid);
        printf("\n\nEnterProgMode 0x%02X\n", res);
        if (argc == 7)
        {
             imageLen = load_ipf(ipffile, 0, 0, image);
             if (imageLen > 0)
             {
                 printf("File %s loaded, total %u bytes\n", ipffile, imageLen);
             }
             else
             {
                 printf("File %s could not be loaded\n", ipffile);
             }
        }
        res = exec_job(jobid);
        res = Ipl_LeaveProgMode();
        printf("LeaveProgMode 0x%02X\n\n", res);
        return 0;
    }
    else if (err_syntax)
    {
        printf("\n\nINIC Programming Library Example\r\n");
        printf("Programming of INIC Configuration and Firmware\r\n");
        printf("\r\n");
        printf("  -INIC Inic\r\n");
        printf("    select connected INIC\r\n");
        printf("\r\n");
        printf("  -JOB JobID\r\n");
        printf("    selects job that should be performed\r\n");
        printf("\r\n");
        printf("  [-IPF Filename]\r\n");
        printf("    data file used for programming (IPF Format)\r\n");
        printf("\r\n");
        printf("  Examples:\r\n");
        printf("    %s -INIC OS81118 -JOB READ_FIRMWARE_VER\r\n", argv[0]);
        printf("    %s -INIC OS81210 -JOB READ_CONFIGSTRING_VER -IPF myFile.ipf\r\n", argv[0]);
        printf("    %s -INIC OS81119 -JOB PROG_FIRMWARE -IPF myFile.ipf\r\n", argv[0]);
        printf("    %s -INIC OS81212 -JOB PROG_CONFIG -IPF myFile.ipf\r\n", argv[0]);
        printf("    %s -INIC OS81210 -JOB PROG_PATCHSTRING -IPF myFile.ipf\r\n", argv[0]);
        printf("    %s -INIC OS81214 -JOB PROG_TEST_CONFIG -IPF myFile.ipf\r\n", argv[0]);
        printf("    %s -INIC OS81214 -JOB PROG_TEST_PATCHSTRING -IPF myFile.ipf\r\n", argv[0]);
        printf("    %s -INIC OS81214 -JOB PROG_CONFIGSTRING -IPF myFile.ipf\r\n", argv[0]);
        printf("    %s -INIC OS81214 -JOB PROG_TEST_CONFIGSTRING -IPF myFile.ipf\r\n", argv[0]);
        printf("    %s -INIC OS81214 -JOB PROG_IDENTSTRING -IPF myFile.ipf\r\n", argv[0]);
        printf("    %s -INIC OS81214 -JOB PROG_TEST_IDENTSTRING -IPF myFile.ipf\r\n", argv[0]);
        printf("    %s -INIC OS81214 -JOB CHK_UPDATE_CONFIG -IPF myFile.ipf\r\n", argv[0]);
        printf("    %s -INIC OS81118 -JOB CHK_UPDATE_FIRMWARE -IPF myFile.ipf\r\n\n", argv[0]);
        return 0;
    }
    print_menu();
    while ( !app_stop )
    {
        ch = Hw_GetKey();
        switch (ch)
        {
            case '0':
                change_chipid();
                break;
            case '1':
                res = Ipl_EnterProgMode(chipid);
                printf("EnterProgMode 0x%02X\n", res);
                break;
            case '2':
                printf("Filename: ");
                fflush(stdout);
                res = read(STDIN_FILENO, ipffile, FILENAME_MAXLEN);
                if (res > 0)
                {
                    ipffile[--res] = '\0';
                }
                imageLen = load_ipf(ipffile, 0, IPL_DATACHUNK_SIZE, image);
                if (imageLen > 0)
                {
                    printf("File %s loaded, total %u bytes\n", ipffile, imageLen);
                }
                else
                {
                       printf("File %s could not be loaded\n", ipffile);
                }
                break;
            case '3':
                exec_job(IPL_JOB_READ_FIRMWARE_VER);
                break;
            case '4':
                exec_job(IPL_JOB_READ_CONFIGSTRING_VER);
                break;
            case '5':
                exec_job(IPL_JOB_PROG_CONFIG);
                break;
            case '6':
                exec_job(IPL_JOB_PROG_FIRMWARE);
                break;
            case '7':
                exec_job(IPL_JOB_PROG_PATCHSTRING);
                break;
            case '9':
                res = Ipl_LeaveProgMode();
                printf("LeaveProgMode 0x%02X\n", res);
                break;
            case 'a':
                exec_job(IPL_JOB_PROG_TEST_CONFIG);
                break;
            case 'b':
                exec_job(IPL_JOB_PROG_TEST_PATCHSTRING);
                break;
            case 'c':
                exec_job(IPL_JOB_PROG_CONFIGSTRING);
                break;
            case 'd':
                exec_job(IPL_JOB_PROG_TEST_CONFIGSTRING);
                break;
            case 'e':
                exec_job(IPL_JOB_PROG_IDENTSTRING);
                break;
            case 'f':
                exec_job(IPL_JOB_PROG_TEST_IDENTSTRING);
                break;
            case 'u':
                exec_job(IPL_JOB_CHK_UPDATE_CONFIGSTRING);
                exec_job(IPL_JOB_CHK_UPDATE_FIRMWARE);
                break;
            case 'm':
                print_menu();
                break;
            case 'z':
                exec_job(0xFEU);
                break;
            case 'x':
            case  27:
                printf("\nExit\n");
                app_stop = true;
                break;
            default :
                break;
        }
    }
    return 0;
}
