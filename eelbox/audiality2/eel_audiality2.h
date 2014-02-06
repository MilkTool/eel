/*
---------------------------------------------------------------------------
	eel_audiality2.h - EEL Audiality 2 binding
---------------------------------------------------------------------------
 * Copyright 2011-2012 David Olofson
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

#ifndef EEL_AUDIALITY2_H
#define EEL_AUDIALITY2_H

#include "EEL.h"
#include "Audiality2/audiality2.h"
#include "Audiality2/waves.h"

/* state */
typedef struct
{
	A2_state	*state;
	EEL_object	*table;
} EA2_state;
EEL_MAKE_CAST(EA2_state)

EEL_xno eel_audiality2_init(EEL_vm *vm);

#endif /* EEL_AUDIALITY2_H */
