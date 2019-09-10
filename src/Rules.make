#Architecture
ARCH=ARM7EJ-S

#Cross compiler prefix
export CROSS_COMPILE=D:\Progra~1\ARM\RVCT\Programs\3.1\569\win_32-pentium
#Default CC value to be used when cross compiling.  This is so that the
#GNU Make default of "cc" is not used to point to  the host compiler
export CC=$(CROSS_COMPILE)\armcc
export AR=$(CROSS_COMPILE)\armar

