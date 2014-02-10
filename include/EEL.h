/*
---------------------------------------------------------------------------
	EEL.h - The "Extensible Embeddable Language"
---------------------------------------------------------------------------
 * Copyright 2002-2004, 2006, 2009, 2011 David Olofson
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from the
 * use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

/*
---------------------------------------------------------------------------
   IMPORTANT   IMPORTANT   IMPORTANT   IMPORTANT   IMPORTANT   IMPORTANT
---------------------------------------------------------------------------
	Strings returned from EEL API calls are owned by the EEL
	state they came from, and MAY HAVE A LIMITED LIFE TIME;

		* The strings are no longer valid after the
		  EEL state they came from has been closed.

		* A string must be considered invalid after
		  more than FIVE (5) further calls to ANY EEL
		  functions have been made.

	The strings MUST NOT EVER be free()d by the application.

	In general, the safest bet is to just analyze and/or copy
	any string you get from EEL first thing after you get it,
	and then forget about it.
---------------------------------------------------------------------------
   IMPORTANT   IMPORTANT   IMPORTANT   IMPORTANT   IMPORTANT   IMPORTANT
---------------------------------------------------------------------------
 */

#ifndef EEL_H
#define EEL_H

#include <stdarg.h>
#include "EEL_export.h"
#include "EEL_module.h"
#include "EEL_object.h"
#include "EEL_register.h"
#include "EEL_types.h"
#include "EEL_value.h"
#include "EEL_version.h"
#include "EEL_platform.h"
#include "EEL_vm.h"
#include "EEL_xno.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Get the version of the library linked. (Should match
 * EEL_COMPILED_VERSION short of the snapshot field, if
 * you're linking statically, but dynamic linking or
 * loading as a module (dlopen()) gives you the version
 * of the library actually loaded.
 */
EELAPI(EEL_uint32)eel_lib_version(void);


/*----------------------------------------------------------
	EEL String API
----------------------------------------------------------*/
/*
 * Find a string in the pool, or add it.
 * Returns a shared EEL_string object, along with one
 * reference ownership. (ie "++refcount".)
 */
EELAPI(EEL_object *)eel_ps_new(EEL_vm *vm, const char *s);

/* Pascal string version; length instead of null terminator. */
EELAPI(EEL_object *)eel_ps_nnew(EEL_vm *vm, const char *s, int len);

/*
 * Create a dynamic string from C or Pascal string 's'.
 * Returns a new EEL_dstring object, along with one
 * reference ownership. (ie "++refcount".)
 */
EELAPI(EEL_object *)eel_ds_new(EEL_vm *vm, const char *s);
EELAPI(EEL_object *)eel_ds_nnew(EEL_vm *vm, const char *s, int len);

/*
 * Find a string identical to 's' in the pool.
 * Returns NULL if no such string exists.
 */
EELAPI(EEL_object *)eel_ps_find(EEL_vm *vm, const char *s);

/* Convenience inline for generating a string reference value */
static inline void eel_s2v(EEL_vm *vm, EEL_value *v, const char *s)
{
	v->objref.v = eel_ps_new(vm, s);
	if(v->objref.v)
		v->type = EEL_TOBJREF;
	else
		v->type = EEL_TNIL;
}

/*
 * Verify that 'v' refers to an object that has something
 * that's compatible with a C string, and return that string.
 * If nothing sensible can be extracted from the value, NULL
 * is returned.
 *
 * The returned string is only valid for the life time of
 * the object referred to by 'v'!
 *
 * NOTE:
 *	This will *not* cast non-string values to strings,
 *	except in special (simple) cases like nil, boolean
 *	values and enum values.
 */
EELAPI(const char *)eel_v2s(EEL_value *v);


/*----------------------------------------------------------
	Indexable Object API
----------------------------------------------------------*/

/*
 * Create an indexable object with 'length' elements.
 * Elements will be initialized to the default "filler"
 * value used when writing to an index > length.
 *
 * SUCCESS:	Returns the new object.
 * FAILURE:	Returns NULL.
 */
EELAPI(EEL_object *)eel_new_indexable(EEL_vm *vm, EEL_types itype, int length);

#if 0
/*
 * Read 'count' elements from vector 'v', starting at 'start',
 * into 'buf'. 'vstride' and 'bstride' is the step size used
 * when reading the vector and writing the buffer, respectively.
 * Returns 0 if the operation is successful, otherwise an EEL
 * exception code describing the problem.
 */
EELAPI(EEL_xno)eel_iread_i(EEL_object *v, int start, int vstride,
		int *buf, int count, int bstride);
EELAPI(EEL_xno)eel_iread_f(EEL_object *v, int start, int vstride,
		float *buf, int count, int bstride);
EELAPI(EEL_xno)eel_iread_d(EEL_object *v, int start, int vstride,
		double *buf, int count, int bstride);

/*
 * Write 'count' elements from 'buf' into vector 'v', starting
 * at 'start'. 'vstride' and 'bstride' is the step size used
 * when writing the vector and reading the buffer, respectively.
 * Returns 0 if the operation is successful, otherwise an EEL
 * exception code describing the problem.
 */
EELAPI(EEL_xno)eel_iwrite_i(EEL_object *v, int start, int vstride,
		const int *buf, int count, int bstride);
EELAPI(EEL_xno)eel_iwrite_f(EEL_object *v, int start, int vstride,
		const float *buf, int count, int bstride);
EELAPI(EEL_xno)eel_iwrite_d(EEL_object *v, int start, int vstride,
		const double *buf, int count, int bstride);
#endif

#ifdef __cplusplus
};
#endif

#endif /* EEL_H */