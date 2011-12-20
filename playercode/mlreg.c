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


  Routine for registering all loaders in libmikmod for the current platform.

==============================================================================*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "mikmod_internals.h"

static void MikMod_RegisterAllLoaders_internal(void)
{
#ifdef LDR_669
	_mm_registerloader(&load_669);
#endif
#ifdef LDR_AMF
	_mm_registerloader(&load_amf);
#endif
#ifdef LDR_ASY
	_mm_registerloader(&load_asy);
#endif
#ifdef LDR_DSM
	_mm_registerloader(&load_dsm);
#endif
#ifdef LDR_FAR
	_mm_registerloader(&load_far);
#endif
#ifdef LDR_GDM
	_mm_registerloader(&load_gdm);
#endif
#ifdef LDR_GT2
	_mm_registerloader(&load_gt2);
#endif
#ifdef LDR_IT
	_mm_registerloader(&load_it);
#endif
#ifdef LDR_IMF
	_mm_registerloader(&load_imf);
#endif
#ifdef LDR_MOD
	_mm_registerloader(&load_mod);
#endif
#ifdef LDR_MED
	_mm_registerloader(&load_med);
#endif
#ifdef LDR_MTM
	_mm_registerloader(&load_mtm);
#endif
#ifdef LDR_OKT
	_mm_registerloader(&load_okt);
#endif
#ifdef LDR_S3M
	_mm_registerloader(&load_s3m);
#endif
#ifdef LDR_STM
	_mm_registerloader(&load_stm);
#endif
#ifdef LDR_STX
	_mm_registerloader(&load_stx);
#endif
#ifdef LDR_ULT
	_mm_registerloader(&load_ult);
#endif
#ifdef LDR_UNI
	_mm_registerloader(&load_uni);
#endif
#ifdef LDR_XM
	_mm_registerloader(&load_xm);
#endif
#ifdef LDR_M15
	_mm_registerloader(&load_m15);
#endif
}

void MikMod_RegisterAllLoaders(void)
{
	MUTEX_LOCK(lists);
	MikMod_RegisterAllLoaders_internal();
	MUTEX_UNLOCK(lists);
}
