/*	MikMod sound library
	(c) 1998, 1999 Miodrag Vallat and others - see file AUTHORS for
	complete list.

	This library is free software; you can redistribute it and/or modify
	it under the terms of the GNU Library General Public License as
	published by the Free Software Foundation; either version 2 of
	the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Library General Public License for more details.

	You should have received a copy of the GNU Library General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
	02111-1307, USA.
*/

/*==============================================================================


  Routine for registering all drivers in libmikmod for the current platform.

==============================================================================*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "mikmod_internals.h"

static void _mm_registeralldrivers(void)
{
	/* Register network drivers */
#if DRV_AF
	_mm_registerdriver(&drv_AF);
#endif
#if DRV_ESD
	_mm_registerdriver(&drv_esd);
#endif
#if DRV_NAS
	_mm_registerdriver(&drv_nas);
#endif

	/* Register hardware drivers - hardware mixing */
#if DRV_ULTRA
	_mm_registerdriver(&drv_ultra);
#endif

	/* Register hardware drivers - software mixing */
#if DRV_AIX
	_mm_registerdriver(&drv_aix);
#endif
#if DRV_ALSA
	_mm_registerdriver(&drv_alsa);
#endif
#if DRV_QNX
	_mm_registerdriver(&drv_qnx);
#endif
#if DRV_HP
	_mm_registerdriver(&drv_hp);
#endif
#if DRV_OSS
	_mm_registerdriver(&drv_oss);
#endif
#if DRV_SGI
	_mm_registerdriver(&drv_sgi);
#endif
#if DRV_SUN
	_mm_registerdriver(&drv_sun);
#endif
#if DRV_DART
	_mm_registerdriver(&drv_dart);
#endif
#if DRV_OS2
	_mm_registerdriver(&drv_os2);
#endif
#if DRV_DS
	_mm_registerdriver(&drv_ds);
#endif
#if DRV_WIN
	_mm_registerdriver(&drv_win);
#endif
#if DRV_MAC
	_mm_registerdriver(&drv_mac);
#endif
#if DRV_OSX
	_mm_registerdriver(&drv_osx);
#endif
#if DRV_GP32
	_mm_registerdriver(&drv_gp32);
#endif
	/* dos drivers */
#if DRV_WSS
	/* wss first, since some cards emulate sb */
	_mm_registerdriver(&drv_wss);
#endif
#if DRV_SB
	_mm_registerdriver(&drv_sb);
#endif
#if DRV_RAW
	/* Register disk writers */
	_mm_registerdriver(&drv_raw);
#endif
#if DRV_WAV
	_mm_registerdriver(&drv_wav);
#endif
#if DRV_AIFF
	_mm_registerdriver(&drv_aiff);
#endif
    /* Register other drivers */
#if DRV_PIPE
	_mm_registerdriver(&drv_pipe);
#endif
#if defined macintosh || defined __QNXNTO__
#else
	_mm_registerdriver(&drv_stdout);
#endif
#if DRV_NOS
	/* Must be the last (least preferred) */
	_mm_registerdriver(&drv_nos);
#endif
}

void MikMod_RegisterAllDrivers(void)
{
	MUTEX_LOCK(lists);
	_mm_registeralldrivers();
	MUTEX_UNLOCK(lists);
}

