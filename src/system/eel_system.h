/*(LGPLv2.1)
---------------------------------------------------------------------------
	eel_system.h - EEL system/platform information module
---------------------------------------------------------------------------
 * Copyright (C) 2007, 2009 David Olofson
 *
 * This library is free software;  you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation;  either version 2.1 of the License, or (at
 * your option) any later version.
 *
 * This library  is  distributed  in  the hope that it will be useful,  but
 * WITHOUT   ANY   WARRANTY;   without   even   the   implied  warranty  of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library;  if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#ifndef EEL_SYSTEM_H
#define EEL_SYSTEM_H

#include "EEL.h"

EEL_xno eel_system_init(EEL_vm *vm, int argc, const char *argv[]);

#endif /* EEL_SYSTEM_H */
