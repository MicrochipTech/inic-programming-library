# GNU CC Cross Toolchain Install Instructions

This guild will cover how to install the GCC Cross Compile Toolchain for ARM chips (e.g Raspbery Pi).
We will use the ```arm-linux-gnueabihf``` version of the GCC compiler.

## Windows

1. Download the toolchain from http://releases.linaro.org/components/toolchain/binaries/ . The version you want is the ```gcc-linaro-X.Y.Z-Year.Month-i686-mingw32_arm-linux-gnueabihf.tar.xz``` . This can be found in the ```arm-linux-gueabihf``` folder of the respective version. X, Y, Z, Year and Month should correspond to the targeted GCC compiler version.
2. After downloading the file extract it with e.g 7-Zip to the folder where the compiler should be installed.
3. Now you should have the following structure ```${InstallLocation}\gcc-linaro-X.Y.Z-Year.Month-i686-mingw32_arm-linux-gnueabihf\...```.
4. To make the cross compiler available for all IDEs you should add its bin folder to your USER PATH. This can be done by:
  1. Pressing Win+S and typing Environment, now it should show the option:
  ```Edit the environment variables for your account``` . Click on this option.
  2. In the opened window click on the ```Environment Variables``` button in the lower right corner.
  3. In the new window search for the PATH variable in the upper combobox, if it doesn't exist click the ```New``` button, else select the entry and click the ```Edit``` button.
  4. If you clicked the ```New``` button in the previous step, name the variable ```Path``` and click the browse directory button. Now navigate to the ```${InstallLocation}\gcc-linaro-X.Y.Z-Year.Month-i686-mingw32_arm-linux-gnueabihf\bin``` folder. If you clicked the Edit button in the previous step you click the ```New``` button and paste the path ```${InstallLocation}\gcc-linaro-X.Y.Z-Year.Month-i686-mingw32_arm-linux-gnueabihf\bin```.
5. Now the compiler is fully installed and in your path.

## Linux

### Ubuntu & Debian

1. Run ```sudo apt-get install gcc-X-arm-linux-gnueabihf``` where X is the major version of the compiler.
