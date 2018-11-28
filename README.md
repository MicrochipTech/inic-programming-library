# INIC Programming Library (IPL)

## Introduction

Microchip Technology Inc. provides a platform-independent programming library called IPL.
The IPL enables an application to program Microchip's Intelligent Network Interface Controllers (INICs) via their I2C interface.

The IPL can handle the following INICs:

* OS81118 (INICnet technology 150)
* OS81119 (INICnet technology 150)
* OS81210 (INICnet technology 50)
* OS81212 (INICnet technology 50)
* OS81214 (INICnet technology 50)
* OS81216 (INICnet technology 50)

Depending on the used INIC, it is possible to program the

* Configuration string (CFGS)
* Identification string (IDENTS)
* Configuration (this includes both CFGS and IDENTS)
* Patch string (PATCHS)
* Firmware

into RAM, OTP or FLASH memory.
The IPL uses the IPF format for the programming data, which can be created by the Microchip Automotive Target Manager (MATM) or UNICENS System Designer.
It is also possible to read out CFGS and firmware versions and to check if an update with a certain IPF data file is useful.

Besides the connection via I2C, INIC's RESET pin and ERR/BOOT_ pin need to be controlled by the IPL. The use of the INT_ pin is optional.

For systems with low memory it is possible to load portions of the IPF content instead of loading the entire data.

> Notes:
> * Documenation is provided in the ipl/doc/ folder
> * For support, related to this code, go to: http://www.microchip.com/support. 



## License
IPL source code is released under the [BSD License](https://github.com/MicrochipTech/unicens/blob/master/LICENSE).
