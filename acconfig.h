/* ========== Package information */

/* Package name (libmikmod) */
#undef PACKAGE
/* Package version */
#undef VERSION

/* ========== Features selected */

/* Define if your system supports binary pipes (i.e. Unix) */
#undef DRV_PIPE

/* Define if you want a .aiff file writer driver */
#undef DRV_AIFF

/* Define if the AudioFile driver is compiled */
#undef DRV_AF
/* Define if the AIX audio driver is compiled */
#undef DRV_AIX
/* Define if the Linux ALSA driver is compiled */
#undef DRV_ALSA
/* Define if the QNX sound driver is compiled */
#undef DRV_QNX
/* Define if the Enlightened Sound Daemon driver is compiled */
#undef DRV_ESD
/* Define if the HP-UX audio driver is compiled */
#undef DRV_HP
/* Define if the Network Audio System driver is compiled */
#undef DRV_NAS
/* Define if the Open Sound System driver is compiled */
#undef DRV_OSS
/* Define if the OS/2 driver is compiled */
#undef DRV_OS2
/* Define if the Linux SAM9407 driver is compiled */
#undef DRV_SAM9407
/* Define if the SGI audio driver is compiled */
#undef DRV_SGI
/* Define if the Sun audio driver or compatible (NetBSD, OpenBSD)
   is compiled */
#undef DRV_SUN
/* Define if the Linux Ultra driver is compiled */
#undef DRV_ULTRA
/* Define this if you want the MacOS X CoreAudio driver */
#undef DRV_OSX
/* Define this if you want the Carbon Mac Audio driver */
#undef DRV_MAC
/* Define this if you want the DOS WSS driver */
#undef DRV_WSS
/* Define this if you want the no-sound (null) driver */
#undef DRV_NOS
/* Define if you want a debug version of the library */
#undef MIKMOD_DEBUG
/* Define if you want runtime dynamic linking of ALSA and EsounD drivers */
#undef MIKMOD_DYNAMIC
/* Define if your system provides POSIX.4 threads */
#undef HAVE_PTHREAD

/* ========== Build environment information */

/* Define if your system is SunOS 4.* */
#undef SUNOS
/* Define if your system is AIX 3.* - might be needed for 4.* too */
#undef AIX
/* Define if your system defines random(3) and srandom(3) in math.h instead
   of stdlib.h */
#undef SRANDOM_IN_MATH_H
/* Define if EsounD driver depends on ALSA */
#undef MIKMOD_DYNAMIC_ESD_NEEDS_ALSA
/* Define if your system has RTLD_GLOBAL defined in <dlfcn.h> */
#undef HAVE_RTLD_GLOBAL
/* Define if your system needs leading underscore to function names in dlsym() calls */
#undef DLSYM_NEEDS_UNDERSCORE

/* define this if you are running a bigendian system (motorola, sparc, etc) */
#undef WORDS_BIGENDIAN

/* Module Loaders*/
#undef LDR_669

#undef LDR_AMF

#undef LDR_ASY

#undef LDR_DSM

#undef LDR_FAR

#undef LDR_GDM

#undef LDR_GT2

#undef LDR_IT

#undef LDR_IMF

#undef LDR_MOD

#undef LDR_MED

#undef LDR_MTM

#undef LDR_OKT

#undef LDR_S3M

#undef LDR_STM

#undef LDR_STX

#undef LDR_ULT

#undef LDR_UNI

#undef LDR_XM

#undef LDR_M15

