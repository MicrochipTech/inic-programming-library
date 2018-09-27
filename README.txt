INIC Programming Library (IPL)
==============================

The IPL is a platform independent programming library that enables an application to program Microchip INIC network controllers via the I2C interface.

 The library can handle the following INICs:

 * OS81118
 * OS81119
 * OS81210
 * OS81212
 * OS81214

Depending on the used INIC, it is possible to program ConfigString, IdentString, PatchString and Firmware into RAM, OTP or FLASH.
The library is using the IPF format for the programming data which can be created by Microchip Automotive Target Manager (MATM) or UNICENS System Designer.
It is also possible to read out ConfigString and Firmware versions and to check, if an update with a certain IPF data file is useful.

Besides the connection via I2C, INICs RESET pin and ERR/BOOT pin needs to be controlled by the library. The use of the INT pin is optional.

On systems with low memory, it is possible to load only portions of the IPF content instead of the complete data.

© 2018 Microchip Technology Inc. All rights reserved. www.microchip.com
