/*
---------------------------------------------------------------------------
	eb_sdl.h - EEL SDL Binding
---------------------------------------------------------------------------
 * Copyright 2005, 2007, 2009, 2011, 2013 David Olofson
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

#ifndef EELBOX_SDL_H
#define EELBOX_SDL_H

#include "EEL.h"
#include "SDL.h"
#include "sfifo.h"

EEL_MAKE_CAST(SDL_Rect)


/* Surface */
typedef struct
{
	SDL_Surface	*surface;
} EB_surface;
EEL_MAKE_CAST(EB_surface)


/* SurfaceLock */
typedef struct
{
	EEL_object	*surface;
} EB_surfacelock;
EEL_MAKE_CAST(EB_surfacelock)


/* Joystick */
typedef struct EB_joystick EB_joystick;
struct EB_joystick
{
	EB_joystick	*next;
	int		index;
	EEL_object	*name;
	SDL_Joystick	*joystick;
};
EEL_MAKE_CAST(EB_joystick)


/* Module instance data */
typedef struct
{
	/* Class Type IDs */
	int		rect_cid;
	int		surface_cid;
	int		surfacelock_cid;
	int		joystick_cid;

	/* Current video surface, if any */
	EEL_object	*video_surface;

	/* Linked list of joysticks */
	EB_joystick	*joysticks;

	/* Audio interface */
	int		audio_open;
	int		audio_pos;
	sfifo_t		audiofifo;
} EB_moduledata;

extern EB_moduledata eb_md;

EEL_xno eb_sdl_init(EEL_vm *vm);

#endif /* EELBOX_SDL_H */
