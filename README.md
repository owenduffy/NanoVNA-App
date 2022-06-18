# NanoVNA-App

<div align="center">
<img src="/Image1.png">
</div>

<div align="center">
<img src="/NanoVNA-App_1_1_49.png">
</div>

# About

This PC software is for use with the NanoVNA unit.

It's currently compilable in Windows but should also be the case in MAC-OS and NIX-OS's.

You'll need C++Builder from Embarcadero to edit and compile the source. You can download and freely use the starter/community edition from here ..

https://www.embarcadero.com/products/cbuilder/starter/free-download

### Companion Tools

* [C++Builder software](https://www.embarcadero.com/products/cbuilder/starter/free-download) by Embarcadero

### Contributors

* [@OneOfEleven](https://github.com/OneOfEleven/)
* [@DiSLord](https://github.com/DiSlord)
* [@owenduffy](https://github.com/owenduffy/)

### Firmware upload drivers

NanoVNA-App contains a facility to upload / download NanoVNA flash memory. The appropriate Windows driver filename is STTub30.sys (or later?) from ST. 
If you wish to use it, make sure that you have not replaced the bootloader driver with something else.
The driver is packaged with ST's [DfuseDemo](https://www.st.com/en/development-tools/stsw-stm32080.html).

