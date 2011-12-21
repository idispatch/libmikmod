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

  Dynamic memory routines

  On failure will set the _mm_errno = MMERR_OUT_OF_MEMORY and call
  the error handler if it was setup

==============================================================================*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "mikmod_internals.h"

#if defined __MACH__ || defined __QNXNTO__
#else
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

static void *get_pointer(void *data)
{
	unsigned char *_pptr = (unsigned char*)data - sizeof(void*);
	size_t _ptr = *(size_t*)_pptr;
	return (void*)_ptr;
}
#endif

void* MikMod_realloc(void *data, size_t size)
{
	if (data)
	{
#if defined __MACH__ || defined __QNXNTO__
		void *d = realloc(data, size);
		if(!d) {
			_mm_errno = MMERR_OUT_OF_MEMORY;
			if(_mm_errorhandler) {
				_mm_errorhandler();
			}
		}
		return d;
#elif (defined _WIN32 || defined _WIN64) && !defined(_WIN32_WCE)
		return _aligned_realloc(data, size, ALIGN_STRIDE);
#else
		unsigned char *newPtr = (unsigned char *)realloc(get_pointer(data), size + ALIGN_STRIDE + sizeof(void*));
		return align_pointer((char*)newPtr, ALIGN_STRIDE);
#endif
	}
	return MikMod_malloc(size);
}

/* Same as malloc, but sets error variable _mm_error when fails. Returns a 16-byte aligned pointer */
void* MikMod_malloc(size_t size)
{
#if defined __MACH__ || defined __QNXNTO__
	void *d = malloc(size);
	if(!d) {
		_mm_errno = MMERR_OUT_OF_MEMORY;
		if(_mm_errorhandler) {
			_mm_errorhandler();
		}
	}
	return d;
#elif (defined _WIN32 || defined _WIN64) && !defined(_WIN32_WCE)
	void * d = _aligned_malloc(size, ALIGN_STRIDE);
	if (d) {
		ZeroMemory(d, size);
		return d;
	} else {
		_mm_errno = MMERR_OUT_OF_MEMORY;
		if(_mm_errorhandler) {
			_mm_errorhandler();
		}
	}
	return 0;
#else
	void *d = calloc(1, size + ALIGN_STRIDE + sizeof(void*));
	if(!d) {
		_mm_errno = MMERR_OUT_OF_MEMORY;
		if(_mm_errorhandler) _mm_errorhandler();
	}
	return align_pointer(d, ALIGN_STRIDE);
#endif
}

/* Same as calloc, but sets error variable _mm_error when fails */
void* MikMod_calloc(size_t nitems,size_t size)
{
#if defined __MACH__ || defined __QNXNTO__
	void * d = calloc(nitems, size);
	if(!d) {
		_mm_errno = MMERR_OUT_OF_MEMORY;
		if(_mm_errorhandler) {
			_mm_errorhandler();
		}
	}
	return d;
#elif (defined _WIN32 || defined _WIN64) && !defined(_WIN32_WCE)
	void * d = _aligned_malloc(size * nitems, ALIGN_STRIDE);
	if (d) {
		ZeroMemory(d, size * nitems);
		return d;
	} else {
		_mm_errno = MMERR_OUT_OF_MEMORY;
		if(_mm_errorhandler) {
			_mm_errorhandler();
		}
	}
	return 0;
#else
	void *d = calloc(nitems, size + ALIGN_STRIDE + sizeof(void*));
	if(!d) {
		_mm_errno = MMERR_OUT_OF_MEMORY;
		if(_mm_errorhandler) _mm_errorhandler();
	}
	return align_pointer(d, ALIGN_STRIDE);
#endif
}

void MikMod_free(void *data)
{
#if defined __MACH__ || defined __QNXNTO__
	free(data);
#elif (defined _WIN32 || defined _WIN64) && !defined(_WIN32_WCE)
	if (data) {
		_aligned_free(data);
	}
#else
	if (data) {
		free(get_pointer(data));
	}
#endif
}

