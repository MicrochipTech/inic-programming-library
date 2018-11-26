# INIC Programming Library (IPL)

## Introduction

Microchip Technology Inc. provides a platform independent programming library called IPL.
The IPL enables an application to program Microchip's Intelligent Network Interface Controllers (INICs) via their I2C interface.

The IPL can handle the following INICs:

* OS81118 (INICnetTM 150)
* OS81119 (INICnetTM 150)
* OS81210 (INICnetTM 50)
* OS81212 (INICnetTM 50)
* OS81214 (INICnetTM 50)
* OS81216 (INICnetTM 50)

Depending on the used INIC, it is possible to program the

    * Configuration string (CFGS)
    * Identification string (IDENTS)
    * Configuration (this includes both CFGS and IDENTS)
    * Patch string (PATCHS)
    * Firmware

into RAM, OTP or FLASH memory.
The IPL uses the IPF format for the programming data, which can be created by the Microchip Automotive Target Manager (MATM) or UNICENSTM System Designer.
It is also possible to read out CFGS and firmware versions and to check if an update with a certain IPF data file is useful.

Besides the connection via I2C, INIC's RESET pin and ERR/BOOT_ pin needs to be controlled by the IPL. The use of the INT_ pin is optional.

For systems with low memory it is possible to load portions of the IPF content instead of loading the entire data.

> Note
>    For support related to this code contact http://www.microchip.com/support. 



## License
UNICENS source code is released under the [BSD License](https://github.com/MicrochipTech/unicens/blob/master/LICENSE).