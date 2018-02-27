compiles under devkitarm r43

howto:
head to /bin folder and copy all contents in SD root, then run dsftp.nds

user/passwords:
read-only
user: anonymous
pass:

read/write
user: nds
pass: nds

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

--

original readme:

			      DSFTP 2.6
			      =========
			      
		   A FTP server for the Nintendo DS
	      Software Component and Stand-Alone Program

	     (c) 2006 Bjoern Giesler <bjoern@giesler.de>

This release contains the following:

     o a stand-alone FTP server for your Nintendo DS (DSFTP.nds
       and DSFTP.gba.ds, whatever is appropriate for your hardware)
       
     o in the subdirectory libDSFTP, headers and a library to
       add an FTP server to your own program

     o in the subdirectory DSFTP, the complete sources for the
       DSFTP program, to serve as an example how to use libDSFTP

     o in the subdirectory doc, documentation for DSFTP and libDSFTP
       in RTF and PDF format.
       