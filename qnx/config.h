/* config.h.  Generated automatically by configure.  */
/* config.h.in.  Generated automatically from configure.in by autoheader 2.13.  */

/* Define if on AIX 3.
   System headers sometimes define this.
   We just want to avoid a redefinition error message.  */
#ifndef _ALL_SOURCE
/* #undef _ALL_SOURCE */
#endif

/* Define to empty if the keyword does not work.  */
/* #undef const */

/* Define if you have <sys/wait.h> that is POSIX.1 compatible.  */
#define HAVE_SYS_WAIT_H 1

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef pid_t */

/* Define to `unsigned' if <sys/types.h> doesn't define.  */
/* #undef size_t */

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Define if your system supports binary pipes (i.e. Unix) */
/* #undef DRV_PIPE */
/* Define if you want a .aiff file writer driver */
/* #undef DRV_AIFF */
/* Define if the AudioFile driver is compiled */
/* #undef DRV_AF */
/* Define if the AIX audio driver is compiled */
/* #undef DRV_AIX */
/* Define if the Linux ALSA driver is compiled */
/* #undef DRV_ALSA */
/* Define if the QNX ALSA driver is compiled */
#define DRV_QNX 1
/* Define if the driver for no output is compiled */
#define DRV_NOS 1
/* Define if the Enlightened Sound Daemon driver is compiled */
/* #undef DRV_ESD */
/* Define if the HP-UX audio driver is compiled */
/* #undef DRV_HP */
/* Define if the Open Sound System driver is compiled */
/* #undef DRV_OSS */
/* Define if the Linux SAM9407 driver is compiled */
/* #undef DRV_SAM9407 */
/* Define if the SGI audio driver is compiled */
/* #undef DRV_SGI */
/* Define if the Sun audio driver or compatible (NetBSD, OpenBSD)
   is compiled */
/* #undef DRV_SUN */
/* Define if the Linux Ultra driver is compiled */
/* #undef DRV_ULTRA */
/* Define this if you want the MacOS X CoreAudio driver */
/* #undef DRV_OSX */
/* Define this if you want the OS2 driver */
/* #undef DRV_OS2 */
/* Define this if you want the Carbon Mac Audio driver */
/* #undef DRV_MAC */
/* Define this if you want the DOS WSS driver */
/* #undef DRV_WSS */
/* #undef DRV_WAV */
/* #undef DRV_STDIO */

/* Define if you want a debug version of the library */
#define MIKMOD_DEBUG 1
/* Define if you want runtime dynamic linking of ALSA and EsounD drivers */
/* #undef MIKMOD_DYNAMIC */
/* Define if your system provides POSIX.4 threads */
#define HAVE_PTHREAD 1

/* Define if your system is SunOS 4.* */
/* #undef SUNOS */
/* Define if your system is AIX 3.* - might be needed for 4.* too */
/* #undef AIX */
/* Define if your system defines random(3) and srandom(3) in math.h instead
   of stdlib.h */
/* #undef SRANDOM_IN_MATH_H */
/* Define if EsounD driver depends on ALSA */
/* #undef MIKMOD_DYNAMIC_ESD_NEEDS_ALSA */
/* Define if your system has RTLD_GLOBAL defined in <dlfcn.h> */
/* #undef HAVE_RTLD_GLOBAL */
/* Define if your system needs leading underscore to function names in dlsym() calls */
/* #undef DLSYM_NEEDS_UNDERSCORE */

/* Define if you have the setenv function.  */
#define HAVE_SETENV 1

/* Define if you have the snprintf function.  */
#define HAVE_SNPRINTF 1

/* Define if you have the srandom function.  */
#define HAVE_SRANDOM 1

/* Define if you have the strcasecmp function.  */
#define HAVE_STRCASECMP 1

/* Define if you have the strdup function.  */
#define HAVE_STRDUP 1

/* Define if you have the strstr function.  */
#define HAVE_STRSTR 1

/* Define if you have the <AF/AFlib.h> header file.  */
/* #undef HAVE_AF_AFLIB_H */

/* Define if you have the <dl.h> header file.  */
/* #undef HAVE_DL_H */

/* Define if you have the <dlfcn.h> header file.  */
/* #undef HAVE_DLFCN_H */

/* Define if you have the <dmedia/audio.h> header file.  */
/* #undef HAVE_DMEDIA_AUDIO_H */

/* Define if you have the <fcntl.h> header file.  */
#define HAVE_FCNTL_H 1

/* Define if you have the <libgus.h> header file.  */
/* #undef HAVE_LIBGUS_H */

/* Define if you have the <machine/soundcard.h> header file.  */
/* #undef HAVE_MACHINE_SOUNDCARD_H */

/* Define if you have the <malloc.h> header file.  */
#define HAVE_MALLOC_H 1

/* Define if you have the <memory.h> header file.  */
#define HAVE_MEMORY_H 1

/* Define if you have the <strings.h> header file.  */
#define HAVE_STRINGS_H 1

/* Define if you have the <sun/audioio.h> header file.  */
/* #undef HAVE_SUN_AUDIOIO_H */

/* Define if you have the <sys/acpa.h> header file.  */
/* #undef HAVE_SYS_ACPA_H */

/* Define if you have the <sys/asoundlib.h> header file.  */
#define HAVE_SYS_ASOUNDLIB_H 1

/* Define if you have the <sys/audio.h> header file.  */
/* #undef HAVE_SYS_AUDIO_H */

/* Define if you have the <sys/audioio.h> header file.  */
/* #undef HAVE_SYS_AUDIOIO_H */

/* Define if you have the <sys/ioctl.h> header file.  */
#define HAVE_SYS_IOCTL_H 1

/* Define if you have the <sys/sam9407.h> header file.  */
/* #undef HAVE_SYS_SAM9407_H */

/* Define if you have the <sys/soundcard.h> header file.  */
/* #undef HAVE_SYS_SOUNDCARD_H */

/* Define if you have the <unistd.h> header file.  */
#define HAVE_UNISTD_H 1

/* Name of package */
#define PACKAGE "libmikmod"

/* Version number of package */
#define VERSION "3.2.0"

#undef LDR_669
#undef LDR_AMF
#undef LDR_ASY
#undef LDR_DSM
#undef LDR_FAR
#undef LDR_GDM
#undef LDR_GT2
#undef LDR_IT
#undef LDR_IMF
#define LDR_MOD 1
#undef LDR_MED
#undef LDR_MTM
#undef LDR_OKT
#define LDR_S3M 1
#undef LDR_STM
#undef LDR_STX
#undef LDR_ULT
#undef LDR_UNI
#define LDR_XM 1
#undef LDR_M15

#define QNX_MIKMOD_PLAY_THREAD 1
