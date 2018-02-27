compiles under devkitarm r43

includes:
DSFTP sources
precompiled shared libraries
DSGUI (a DSFTP library)


changes:
force the GCC compiler to use standard C++ and not C++11, otherwise code can´t link against shared libraries.


bug:
revise fat driver, since the FTP directory listing will throw random folders instead actual files from SD card. 
But the heavy-lifting work is done at least hehe

notes:
IIRC the author said the code could not be hosted anywhere else than his website. But it´s been like 12 years, and his site his gone.

Coto