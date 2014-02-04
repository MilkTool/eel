/*(LGPLv2.1)
---------------------------------------------------------------------------
	EEL_xno.h - EEL Exception/Error codes (API)
---------------------------------------------------------------------------
 * Copyright (C) 2005-2006, 2009 David Olofson
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

#ifndef	EEL_XNO_H
#define	EEL_XNO_H

#include "EEL_export.h"
#include "EEL_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * NOTE:
 *	Add new codes LAST, to preserve binary compatibility.
 *	When bumping the interface version to indicate broken
 *	binary compatibility, take the opportunity to put any
 *	new codes where they belong in the list.
 */
typedef enum
{
	EEL_XNONE =	0,	/* Nothing; everything's fine */

	/* VM exceptions */
	EEL_XYIELD,		/* Give up VM if there is other work */
	EEL_XCOUNTER,		/* Runlimit instruction counter exhausted */
	EEL_XEND,		/* Thread returns from top level */
	EEL_XRETURN,		/* Return from the "real" function. */
	EEL_XREFUSE,		/* Object refused to destruct, and/or
				 * wants EEL to stay away. */
	EEL_XOTHER,		/* Other - check vm->exception! */
	EEL_XVMCHECK,		/* Error detected due to EEL_VM_CHECKING */
	EEL_XBADEXCEPTION,	/* Bad exception value type */

	/* Internal/VM/native errors */
	EEL_XINTERNAL,		/* Internal error in EEL */
	EEL_XVMERROR,		/* Unspecified VM error */

	/* General VM and operator exceptions */
	EEL_XILLEGAL,		/* Illegal opcode */
	EEL_XNOTIMPLEMENTED,	/* Feature not implemented */
	EEL_XCANTREAD,		/* Object cannot be read */
	EEL_XCANTWRITE,		/* Object cannot be written */
	EEL_XCANTINDEX,		/* Object cannot be indexed */
	EEL_XCANTINPLACE,	/* Inplace operation not possible */
	EEL_XUPVALUE,		/* Upvalue(s) not accessible */
	EEL_XCASTFAILED,	/* Type cast failed */
	EEL_XNOMETAMETHOD,	/* Metamethod not implemented */
	EEL_XNOCONSTRUCTOR,	/* No constructor! */
	EEL_XCONSTRUCTOR,	/* Constructor failed */
	EEL_XBADCONTEXT,	/* Not possible in this context */

	/* Argument and operand errors */
	EEL_XARGUMENTS,		/* Incorrect argument list */
	EEL_XFEWARGS,		/* Too few arguments */
	EEL_XMANYARGS,		/* Too manp arguments */
	EEL_XTUPLEARGS,		/* Incomplete argument tuple */
	EEL_XNORESULT,		/* No result available */
	EEL_XNEEDREAL,		/* Argument must be real type */
	EEL_XNEEDINTEGER,	/* Argument must be integer type */
	EEL_XNEEDBOOLEAN,	/* Argument must be boolean type */
	EEL_XNEEDTYPEID,	/* Argument must be typeid type */
	EEL_XNEEDOBJECT,	/* Argument must be object type */
	EEL_XNEEDLIST,		/* Argument must be LIST type */
	EEL_XNEEDSTRING,	/* Argument must be string type */
	EEL_XNEEDDSTRING,	/* Argument must be dstring type */
	EEL_XNEEDARRAY,		/* Argument must be array type */
	EEL_XNEEDTABLE,		/* Argument must be table type */
	EEL_XNEEDCALLABLE,	/* Argument must be callable object */
	EEL_XNEEDEVEN,		/* Needs even number of items */
	EEL_XWRONGTYPE,		/* Wrong type */
	EEL_XBADTYPE,		/* Illegal type ID */
	EEL_XBADCLASS,		/* Illegal class type ID */
	EEL_XLOWINDEX,		/* Index out of range; too low */
	EEL_XHIGHINDEX,		/* Index out of range; too high */
	EEL_XWRONGINDEX,	/* Nonexistent index (index-by-name) */

	/* Math and other operation errors */
	EEL_XLOWVALUE,		/* Value out of range; too low */
	EEL_XHIGHVALUE,		/* Value out of range; too high */
	EEL_XBADVALUE,		/* Incorrect value */
	EEL_XDIVBYZERO,		/* Division by zero */
	EEL_XOVERFLOW,		/* Too large value */
	EEL_XUNDERFLOW,		/* Too small value */
	EEL_XDOMAIN,		/* Math domain error */
	EEL_XMATHERROR,		/* Other math errors */
	EEL_XILLEGALOPERATION,	/* Illegal operation */

	/* System errors */
	EEL_XMEMORY,		/* Out of memory */
	EEL_XEOF,		/* End of file */
	EEL_XFILEOPEN,		/* Error opening file */
	EEL_XFILESEEK,		/* Error seeking in file */
	EEL_XFILEREAD,		/* Error reading file */
	EEL_XFILEWRITE,		/* Error writing file */
	EEL_XFILELOAD,		/* Unspecified load error */
	EEL_XFILESAVE,		/* Unspecified save error */
	EEL_XFILEOPENED,	/* File is already open */
	EEL_XFILECLOSED,	/* File is closed */
	EEL_XFILEERROR,		/* Unspecified file I/O error */
	EEL_XDEVICEOPEN,	/* Error opening device */
	EEL_XDEVICEREAD,	/* Error reading from device */
	EEL_XDEVICEWRITE,	/* Error writing to device */
	EEL_XDEVICECONTROL,	/* Error controlling device */
	EEL_XDEVICEOPENED,	/* Device is already open */
	EEL_XDEVICECLOSED,	/* Device is closed */
	EEL_XDEVICEERROR,	/* Unspecified device I/O error */
	EEL_XTHREADCREATE,	/* Could not create thread */
	EEL_XBUFOVERFLOW,	/* Buffer overflow */

	/* Lexer and parser exceptions */
	EEL_XNONUMBER,		/* Not a valid number */
	EEL_XBADBASE,		/* Bad base syntax */
	EEL_XBIGBASE,		/* Too big base */
	EEL_XBADINTEGER,	/* Bad integer part format */
	EEL_XBADFRACTION,	/* Bad fraction part format */
	EEL_XBADEXPONENT,	/* Bad exponent format */
	EEL_XREALNUMBER,	/* Enforce real value (lexer) */

	/* Compiler and API errors */
	EEL_XCOMPILE,		/* Compile error */
	EEL_XSYNTAX,		/* Unspecified syntax error */
	EEL_XNOTFOUND,		/* Object not found (call by name) */
	EEL_XMODULEINIT,	/* Module initialization failed */
	EEL_XCANTSETMETHOD,	/* Could not set (meta)method */

	EEL__XCOUNT
} EEL_xno;

/*
 * Exception name strings.
 */
EELAPI(const char *)eel_x_name(EEL_xno x);

#ifdef __cplusplus
};
#endif

#endif /*  EEL_XNO_H */
