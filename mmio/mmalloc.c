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

  $Id: mmalloc.c,v 1.1 2005/03/30 01:31:01 raphassenat Exp $

  Dynamic memory routines

==============================================================================*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "mikmod_internals.h"

#define ALIGN_STRIDE 16

static void * align_pointer(char *ptr, size_t stride)
{
	char *pptr = ptr + sizeof(void*);
	char *fptr;
	size_t err = ((size_t)pptr)&(stride-1);
	if (err)
		fptr = pptr + (stride - err);
	else
		fptr = pptr;
	*(size_t*)(fptr - sizeof(void*)) = (size_t)ptr;
	return fptr;
}


void* MikMod_realloc(void *data, size_t size)
{
	if (data)
	{
		size_t stride = ALIGN_STRIDE;
		unsigned char *_pptr = (unsigned char*)data - sizeof(void*);
		size_t _ptr = *(size_t*)_pptr;

		unsigned char *newPtr = (unsigned char *)realloc((void*)_ptr, size + stride + sizeof(void*));
		return align_pointer(newPtr, stride);
	}
	return MikMod_malloc(size);
}


/* Same as malloc, but sets error variable _mm_error when fails. Returns a 16-byte aligned pointer */
void* MikMod_malloc(size_t size)
{
	void *d = calloc(1, size + ALIGN_STRIDE + sizeof(void*));

	if(!d) {
		_mm_errno = MMERR_OUT_OF_MEMORY;
		if(_mm_errorhandler) _mm_errorhandler();
	}
	return align_pointer(d, ALIGN_STRIDE);
}

/* Same as calloc, but sets error variable _mm_error when fails */
void* MikMod_calloc(size_t nitems,size_t size)
{
	void *d = calloc(nitems, size + ALIGN_STRIDE + sizeof(void*));
   
	if(!d) {
		_mm_errno = MMERR_OUT_OF_MEMORY;
		if(_mm_errorhandler) _mm_errorhandler();
	}
	return align_pointer(d, ALIGN_STRIDE);
}

void MikMod_free(void *ptr)
{
	if (ptr)
	{
		unsigned char *_pptr = (unsigned char*)ptr - sizeof(void*);
		size_t _ptr = *(size_t*)_pptr;
		free((void*)_ptr);
	}
}

/* ex:set ts=4: */
