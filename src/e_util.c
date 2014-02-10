/*
---------------------------------------------------------------------------
	e_util.c - EEL engine utilities
---------------------------------------------------------------------------
 * Copyright 2002-2007, 2009-2013 David Olofson
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

#include <stdio.h>

#include "ec_symtab.h"
#include "ec_coder.h"
#include "e_util.h"
#include "e_state.h"
#include "e_class.h"
#include "e_object.h"
#include "e_string.h"
#include "e_dstring.h"
#include "e_function.h"
#include "e_vector.h"
#include "e_array.h"
#include "e_table.h"


/*----------------------------------------------------------
	"Alloc and forget" string handling
----------------------------------------------------------*/

/* Auto-recycled result string buffers */
struct EEL_sbuffer
{
	char		buffer[EEL_SBUFSIZE];
	EEL_sbuffer	*prev, *next;
#ifdef DEBUG
	int		inuse;
#endif
};

/*
 * Pool of sbuffers.
 * NOTE:
 *	There has to be exactly EEL_SBUFFERS sbuffers, and they all need to be
 *	allocated from this single memory block, for the "is this an sbuffer?"
 *	test in eel_sfree()!
 */
static EEL_sbuffer *sbuffers = NULL;


EEL_DLLIST(asb_, EEL_state, firstasb, lastasb, EEL_sbuffer, prev, next)
EEL_DLLIST(fsb_, EEL_state, firstfsb, lastfsb, EEL_sbuffer, prev, next)

int eel_sbuffer_open(EEL_state *es)
{
	int i;
	sbuffers = (EEL_sbuffer *)calloc(EEL_SBUFFERS, sizeof(EEL_sbuffer));
	if(!sbuffers)
		return -1;
	for(i = 0; i < EEL_SBUFFERS; ++i)
		fsb_link(es, sbuffers + i);
	return 0;
}


void eel_sbuffer_close(EEL_state *es)
{
	es->firstasb = es->firstfsb = NULL;
	free(sbuffers);
	sbuffers = NULL;
}


char *eel_salloc(EEL_state *es)
{
	EEL_sbuffer *sb = es->lastfsb;
	if(sb)
	{
		/* Grab newest free sbuffer (cache efficiency) */
		fsb_unlink(es, sb);
	}
	else
	{
		/* Steal oldest allocated sbuffer */
		sb = es->firstasb;
		asb_unlink(es, sb);
		SDBG(printf("NOTE: Old string \"%s\" stolen.\n", sb->buffer);)
	}
	asb_link(es, sb);
#ifdef DEBUG
	sb->inuse = 1;
#endif
	return &sb->buffer[0];
}


void eel_sfree(EEL_state *es, const char *s)
{
	if(s < (const char *)sbuffers)
	{
		SDBG(fprintf(stderr, "WARNING: Someone tried to eel_sfree() "
				"something located before the pool!\n");)
		return;
	}
	if(s >= (const char *)sbuffers + sizeof(EEL_sbuffer) * EEL_SBUFFERS)
	{
		SDBG(fprintf(stderr, "WARNING: Someone tried to eel_sfree() "
				"something located after the pool!\n");)
		return;
	}
	if((s - (const char *)sbuffers) % sizeof(EEL_sbuffer))
	{
		fprintf(stderr, "INTERNAL ERROR: Someone tried to eel_sfree() "
				"part of an sbuffer!\n");
		return;
	}
#ifdef DEBUG
	if(!((EEL_sbuffer *)s)->inuse)
	{
		fprintf(stderr, "INTERNAL ERROR: Someone tried to eel_sfree() "
				"an sbuffer that is already free!\n");
		return;
	}
	((EEL_sbuffer *)s)->inuse = 0;
#endif
	asb_unlink(es, (EEL_sbuffer *)s);
	fsb_link(es, (EEL_sbuffer *)s);
}


#define	XN(n)	case EEL_##n:	return #n;
const char *eel_x_name(EEL_vm *vm, EEL_xno x)
{
	switch(x)
	{
	  XN(XNONE)

	  XN(XYIELD)
	  XN(XCOUNTER)
	  XN(XEND)
	  XN(XRETURN)
	  XN(XREFUSE)
	  XN(XOTHER)
	  XN(XVMCHECK)
	  XN(XBADEXCEPTION)

	  XN(XINTERNAL)
	  XN(XVMERROR)

	  XN(XILLEGAL)
	  XN(XNOTIMPLEMENTED)
	  XN(XCANTREAD)
	  XN(XCANTWRITE)
	  XN(XCANTINDEX)
	  XN(XCANTINPLACE)
	  XN(XUPVALUE)
	  XN(XCASTFAILED)
	  XN(XNOMETAMETHOD)
	  XN(XNOCONSTRUCTOR)
	  XN(XCONSTRUCTOR)
	  XN(XBADCONTEXT)

	  XN(XARGUMENTS)
	  XN(XFEWARGS)
	  XN(XMANYARGS)
	  XN(XTUPLEARGS)
	  XN(XNORESULT)
	  XN(XNEEDREAL)
	  XN(XNEEDINTEGER)
	  XN(XNEEDBOOLEAN)
	  XN(XNEEDTYPEID)
	  XN(XNEEDOBJECT)
	  XN(XNEEDLIST)
	  XN(XNEEDSTRING)
	  XN(XNEEDDSTRING)
	  XN(XNEEDARRAY)
	  XN(XNEEDTABLE)
	  XN(XNEEDCALLABLE)
	  XN(XNEEDEVEN)
	  XN(XWRONGTYPE)
	  XN(XBADTYPE)
	  XN(XBADCLASS)
	  XN(XLOWINDEX)
	  XN(XHIGHINDEX)
	  XN(XWRONGINDEX)

	  XN(XHIGHVALUE)
	  XN(XLOWVALUE)
	  XN(XBADVALUE)
	  XN(XDIVBYZERO)
	  XN(XOVERFLOW)
	  XN(XUNDERFLOW)
	  XN(XDOMAIN)
	  XN(XMATHERROR)
	  XN(XILLEGALOPERATION)

	  XN(XMEMORY)
	  XN(XEOF)
	  XN(XFILEOPEN)
	  XN(XFILESEEK)
	  XN(XFILEREAD)
	  XN(XFILEWRITE)
	  XN(XFILELOAD)
	  XN(XFILESAVE)
	  XN(XFILEOPENED)
	  XN(XFILECLOSED)
	  XN(XFILEERROR)
	  XN(XDEVICEOPEN)
	  XN(XDEVICEREAD)
	  XN(XDEVICEWRITE)
	  XN(XDEVICECONTROL)
	  XN(XDEVICEOPENED)
	  XN(XDEVICECLOSED)
	  XN(XDEVICEERROR)
	  XN(XTHREADCREATE)
	  XN(XBUFOVERFLOW)

	  XN(XNONUMBER)
	  XN(XBADBASE)
	  XN(XBIGBASE)
	  XN(XBADINTEGER)
	  XN(XBADFRACTION)
	  XN(XBADEXPONENT)
	  XN(XREALNUMBER)

	  XN(XCOMPILE)
	  XN(XSYNTAX)
	  XN(XNOTFOUND)
	  XN(XMODULEINIT)
	  XN(XCANTSETMETHOD)

	  case EEL__XCOUNT:
		break;
	}
	return "<unknown>";
}
#undef XN


#define	MMN(n)	case EEL_MM_##n:	return #n;
const char *eel_mm_name(EEL_mmindex mm)
{
	switch(mm)
	{
	  MMN(GETINDEX)
	  MMN(SETINDEX)
	  MMN(COPY)
	  MMN(INSERT)
	  MMN(DELETE)
	  MMN(LENGTH)
	  MMN(COMPARE)
	  MMN(EQ)
	  MMN(CAST)
	  MMN(SERIALIZE)

	  MMN(POWER)
	  MMN(IPPOWER)
	  MMN(MOD)
	  MMN(IPMOD)
	  MMN(DIV)
	  MMN(IPDIV)
	  MMN(MUL)
	  MMN(IPMUL)
	  MMN(SUB)
	  MMN(IPSUB)
	  MMN(ADD)
	  MMN(IPADD)

	  MMN(RPOWER)
	  MMN(IPRPOWER)
	  MMN(RMOD)
	  MMN(IPRMOD)
	  MMN(RDIV)
	  MMN(IPRDIV)
	  MMN(RSUB)
	  MMN(IPRSUB)
	  MMN(RMUL)
	  MMN(IPRMUL)
	  MMN(RADD)
	  MMN(IPRADD)

	  MMN(VPOWER)
	  MMN(IPVPOWER)
	  MMN(VMOD)
	  MMN(IPVMOD)
	  MMN(VDIV)
	  MMN(IPVDIV)
	  MMN(VMUL)
	  MMN(IPVMUL)
	  MMN(VSUB)
	  MMN(IPVSUB)
	  MMN(VADD)
	  MMN(IPVADD)

	  MMN(VRPOWER)
	  MMN(IPVRPOWER)
	  MMN(VRMOD)
	  MMN(IPVRMOD)
	  MMN(VRDIV)
	  MMN(IPVRDIV)
	  MMN(VRSUB)
	  MMN(IPVRSUB)
	  MMN(VRMUL)
	  MMN(IPVRMUL)
	  MMN(VRADD)
	  MMN(IPVRADD)

	  case EEL_MM__COUNT:
		break;
	}
	return "<unknown>";
}
#undef MMN


/*----------------------------------------------------------
	Data access tools
----------------------------------------------------------*/
long int eel__v2l(EEL_value *v)
{
/*
TODO: Try to cast to EEL_TINTEGER or something.
*/
	return 0;
}


double eel__v2d(EEL_value *v)
{
/*
TODO: Try to cast to EEL_TREAL or something.
*/
	return 0.0f;
}


const char *eel_v2s(EEL_value *v)
{
	switch(v->type)
	{
	  case EEL_TNIL:
		return "nil";
	  case EEL_TREAL:
	  case EEL_TINTEGER:
		return NULL;
	  case EEL_TBOOLEAN:
		return v->integer.v ? "true" : "false";
	  case EEL_TTYPEID:
		return NULL;
	  case EEL_TOBJREF:
	  case EEL_TWEAKREF:
		switch((EEL_classes)v->objref.v->type)
		{
		  case EEL_CSTRING:
			return eel_o2s(v->objref.v);
		  case EEL_CDSTRING:
			return o2EEL_dstring(v->objref.v)->buffer;
		  default:
			return NULL;
		}
	}
	return "<BROKEN VALUE>";
}


/*----------------------------------------------------------
	Indexable Object API
----------------------------------------------------------*/

EEL_object *eel_new_indexable(EEL_vm *vm, EEL_types itype, int length)
{
	EEL_value v, iv;
	EEL_object *o;
	switch((EEL_classes)itype)
	{
	  case EEL_CVECTOR:
	  case EEL_CVECTOR_U8:
	  case EEL_CVECTOR_S8:
	  case EEL_CVECTOR_U16:
	  case EEL_CVECTOR_S16:
	  case EEL_CVECTOR_U32:
	  case EEL_CVECTOR_S32:
	  case EEL_CVECTOR_F:
	  case EEL_CVECTOR_D:
	  case EEL_CDSTRING:
		eel_l2v(&v, 0);
		break;
	  case EEL_CARRAY:
		eel_nil2v(&v);
		break;
	  default:
		return NULL;
	}

	/* Try to construct empty object */
	if(eel_o__construct(vm, itype, NULL, 0, &iv))
		return NULL;
	o = iv.objref.v;

	/* Set last index, to reallocate and initialize */
	eel_l2v(&iv, length - 1);
	if(eel_o__metamethod(o, EEL_MM_SETINDEX, &iv, &v))
	{
		eel_o_disown_nz(o);
		return NULL;
	}

	/* Verify that we actually got something with
	 * 'length' elements!
	 */
	if(eel_o__metamethod(o, EEL_MM_LENGTH, NULL, &v) ||
			eel_v2l(&v) != length)
	{
		eel_o_disown_nz(o);
		return NULL;
	}

	return o;
}

#if 0
EEL_xno eel_iread_i(EEL_object *v, int start, int vstride,
		int *buf, int count, int bstride)
{
	return EEL_XNOTIMPLEMENTED;
}


EEL_xno eel_iread_f(EEL_object *v, int start, int vstride,
		float *buf, int count, int bstride)
{
	return EEL_XNOTIMPLEMENTED;
}


EEL_xno eel_iread_d(EEL_object *v, int start, int vstride,
		double *buf, int count, int bstride)
{
	return EEL_XNOTIMPLEMENTED;
}


EEL_xno eel_iwrite_i(EEL_object *v, int start, int vstride,
		const int *buf, int count, int bstride)
{
	return EEL_XNOTIMPLEMENTED;
}


EEL_xno eel_iwrite_f(EEL_object *v, int start, int vstride,
		const float *buf, int count, int bstride)
{
	return EEL_XNOTIMPLEMENTED;
}


EEL_xno eel_iwrite_d(EEL_object *v, int start, int vstride,
		const double *buf, int count, int bstride)
{
	return EEL_XNOTIMPLEMENTED;
}
#endif


/*----------------------------------------------------------
	Symbol table related utilities
----------------------------------------------------------*/

static void strnquote(char *dst, const char *src, int maxlen, int srclen)
{
	int i = 0, j = 0;
	--maxlen;	/* For the null terminator */
	dst[j++] = '\"';
	while(1)
	{
		const char *s;
		int len;
		if(i >= srclen)
			break;
		switch(src[i])
		{
		  case 0:
			s = NULL;
			break;
		  case '\a':
			s = "\\a";
			break;
		  case '\b':
			s = "\\b";
			break;
		  case '\t':
			s = "\\t";
			break;
		  case '\n':
			s = "\\n";
			break;
		  case '\v':
			s = "\\v";
			break;
		  case '\f':
			s = "\\f";
			break;
		  case '\r':
			s = "\\r";
			break;
		  case '\\':
			s = "\\\\";
			break;
		  default:
			if(src[i] < 32)
			{
				s = "\\?";
				break;
			}
			else if(j < maxlen)
			{
				dst[j++] = src[i++];
				continue;
			}
			else
				s = NULL;
		}
		if(!s)
			break;
		len = strlen(s);
		if(j + len - 1 < maxlen)
			while(len--)
				dst[j++] = *s++;
		else
			break;
		++i;
	}
	if(j < maxlen)
		dst[j++] = '\"';
	dst[j] = 0;
}


/* NOTE: Uses eel_salloc()! */
const char *eel_o_stringrep(EEL_object *o)
{
	EEL_vm *vm = o->vm;
	EEL_state *es = VMP->state;
	EEL_object *n;
	char *buf = eel_salloc(es);
	switch((EEL_classes)o->type)
	{
	  case EEL_CVALUE:
	  case EEL_COBJECT:
	  case EEL_CVECTOR:
		snprintf(buf, EEL_SBUFSIZE,
				"<virtual base class '%s' (%p; rc: %d)>",
				eel_typename(vm, o->type),
				o, o->refcount);
		return buf;
	  case EEL_CCLASS:
		n = o2EEL_classdef(o)->name;
		snprintf(buf, EEL_SBUFSIZE, "<class '%s' (%p; rc: %d)>",
				n ? eel_o2s(n) : "<unnamed>",
				o, o->refcount);
		return buf;
	  case EEL_CFUNCTION:
		n = o2EEL_function(o)->common.name;
		snprintf(buf, EEL_SBUFSIZE, "<function '%s' (%p; rc: %d)>",
				n ? eel_o2s(n) : "<unnamed>",
				o, o->refcount);
		return buf;
	  case EEL_CMODULE:
	  {
	  	const char *s;
	  	if(o2EEL_module(o)->exports && VMP->strings)
			s = eel_module_modname(o);
		else
			s = "<cannot get name>";
		snprintf(buf, EEL_SBUFSIZE, "<module '%s' (%p; rc: %d)>",
				s ? s : "<unnamed>", o, o->refcount);
		return buf;
	  }
	  case EEL_CSTRING:
	  {
		EEL_string *ps = o2EEL_string(o);
		char *s = eel_salloc(es);
		if(ps->buffer)
			strnquote(s, ps->buffer, EEL_SBUFSIZE, ps->length);
		else
			snprintf(s, EEL_SBUFSIZE, "<no buffer!>");
		snprintf(buf, EEL_SBUFSIZE, "<string %s (%p; %x; rc: %d)>",
				s, o, ps->hash, o->refcount);
		eel_sfree(es, s);
		return buf;
	  }
	  case EEL_CDSTRING:
	  {
		EEL_dstring *ds = o2EEL_dstring(o);
		char *s = eel_salloc(es);
		if(ds->buffer)
			strnquote(s, ds->buffer, EEL_SBUFSIZE, ds->length);
		else
			snprintf(s, EEL_SBUFSIZE, "<no buffer!>");
		snprintf(buf, EEL_SBUFSIZE, "<dstring %s (%p; rc: %d)>",
				s, o, o->refcount);
		eel_sfree(es, s);
		return buf;
	  }
	  case EEL_CVECTOR_U8:
	  case EEL_CVECTOR_S8:
	  case EEL_CVECTOR_U16:
	  case EEL_CVECTOR_S16:
	  case EEL_CVECTOR_U32:
	  case EEL_CVECTOR_S32:
	  case EEL_CVECTOR_F:
	  case EEL_CVECTOR_D:
		snprintf(buf, EEL_SBUFSIZE, "<%s of %d elements"
				" (%p; rc: %d)>",
				eel_typename(vm, o->type),
				o2EEL_vector(o)->length,
				o, o->refcount);
		return buf;
	  case EEL_CARRAY:
		snprintf(buf, EEL_SBUFSIZE, "<array of %d values (%p; rc: %d)>",
				o2EEL_array(o)->length, o, o->refcount);
		return buf;
	  case EEL_CTABLE:
		snprintf(buf, EEL_SBUFSIZE, "<table of %d entries (%p; rc: %d)>",
				o2EEL_table(o)->length, o, o->refcount);
		return buf;
	  case EEL__CUSER:
	  default:
		break;
	}
	if(es->classes)
	{
		n = o2EEL_classdef(es->classes[o->type])->name;
		snprintf(buf, EEL_SBUFSIZE, "<object %p of class %s (rc: %d)>",
			 	o, eel_o2s(n), o->refcount);
	}
	else
		snprintf(buf, EEL_SBUFSIZE, "<object %p of class #%d (rc: %d)>",
			 	o, o->type, o->refcount);
	return buf;
}


/* NOTE: Uses eel_salloc()! */
const char *eel_v_stringrep(EEL_vm *vm, const EEL_value *value)
{
	char *buf;
	switch(value->type)
	{
	  case EEL_TNIL:
		return "<nil>";
	  case EEL_TREAL:
		buf = eel_salloc(VMP->state);
		snprintf(buf, EEL_SBUFSIZE, "%.10g", value->real.v);
		return buf;
	  case EEL_TINTEGER:
		buf = eel_salloc(VMP->state);
		snprintf(buf, EEL_SBUFSIZE, "%d (0x%x)",
				value->integer.v, value->integer.v);
		return buf;
	  case EEL_TBOOLEAN:
		if(value->integer.v)
			return "true";
		else
			return "false";
	  case EEL_TTYPEID:
		buf = eel_salloc(VMP->state);
		snprintf(buf, EEL_SBUFSIZE, "<typeid %s>",
				eel_o2s(o2EEL_classdef(
				VMP->state->classes[value->integer.v])->name));
		return buf;
	  case EEL_TOBJREF:
	  case EEL_TWEAKREF:
		return eel_o_stringrep(value->objref.v);
	}
	return "<internal error: undef value type>";
}


const char *eel_symbol_is(EEL_symbol *s)
{
	switch(s->type)
	{
	  case EEL_SUNDEFINED:	return "an undedined symbol";
	  case EEL_SKEYWORD:	return "a keyword";
	  case EEL_SCLASS:	return "a class";
	  case EEL_SCONSTANT:	return "a constant";
	  case EEL_SVARIABLE:
		switch((EEL_varkinds)s->v.var.kind)
		{
		  case EVK_STACK:	return "a stack variable";
		  case EVK_STATIC:	return "a static variable";
		  case EVK_ARGUMENT:	return "an argument";
		  case EVK_OPTARG:	return "an optional argument";
		  case EVK_TUPARG:	return "a tuple argument";
		  default:		return "a variable of illegal type";
		}
	  case EEL_SUPVALUE:
		switch((EEL_varkinds)s->v.var.kind)
		{
		  case EVK_STACK:	return "a stack upvalue";
		  case EVK_STATIC:	return "a static upvalue";
		  case EVK_ARGUMENT:	return "an argument upvalue";
		  case EVK_OPTARG:	return "an optional argument upvalue";
		  case EVK_TUPARG:	return "a tuple argument upvalue";
		  default:		return "a upvalue of illegal type";
		}
	  case EEL_SOPERATOR:	return "an operator";
	  case EEL_SBODY:	return "a body";
	  case EEL_SFUNCTION:	return "a function";
	  case EEL_SMODULE:	return "a module";
	  case EEL_SNAMESPACE:	return "a namespace";
	}
	/*
	 * Should be here, and not under default:, to get
	 * a warning if we forget to add new types. :-)
	 */
	return "a broken symbol! (illegal type)";
}


/*
 * This one tries to work for all built-in types
 * relying only on the value, but obviously, it
 * can't do much about user classes for which
 * classdefs or name strings are not available.
 */
const char *eel_typename(EEL_vm *vm, EEL_types type)
{
	switch(type)
	{
	  case EEL_TNIL:	return "nil";
	  case EEL_TREAL:	return "real";
	  case EEL_TINTEGER:	return "integer";
	  case EEL_TBOOLEAN:	return "boolean";
	  case EEL_TTYPEID:	return "typeid";
	  case EEL_TOBJREF:	return "objref";
	  case EEL_TWEAKREF:	return "weakref";
	}
	switch((EEL_classes)type)
	{
	  case EEL_CVALUE:	return "value";
	  case EEL_COBJECT:	return "object";
	  case EEL_CCLASS:	return "class";
	  case EEL_CSTRING:	return "string";
	  case EEL_CDSTRING:	return "dstring";
	  case EEL_CFUNCTION:	return "function";
	  case EEL_CMODULE:	return "module";
	  case EEL_CARRAY:	return "array";
	  case EEL_CTABLE:	return "table";
	  case EEL_CVECTOR:	return "vector";
	  case EEL_CVECTOR_U8:	return "vector_u8";
	  case EEL_CVECTOR_S8:	return "vector_s8";
	  case EEL_CVECTOR_U16:	return "vector_u16";
	  case EEL_CVECTOR_S16:	return "vector_s16";
	  case EEL_CVECTOR_U32:	return "vector_u32";
	  case EEL_CVECTOR_S32:	return "vector_s32";
	  case EEL_CVECTOR_F:	return "vector_f";
	  case EEL_CVECTOR_D:	return "vector_d";
	  case EEL__CUSER:
	  default:
		break;
	}
  	if((type < 0) || (type >= VMP->state->nclasses))
		return "<ILLEGAL TYPE>";
	if(!VMP->state->classes[type])
		return "<UNDEFINED TYPE>";
	if(!o2EEL_classdef(VMP->state->classes[type])->name)
		return "<UNNAMED TYPE>";
	return eel_o2s(o2EEL_classdef(VMP->state->classes[type])->name);
}


/* NOTE: Output string only valid until next call. */
const char *eel_s_stringrep(EEL_vm *vm, EEL_symbol *s)
{
	char *buf = eel_salloc(VMP->state);
	const char *n = s->name ? eel_o2s(s->name) : "<unnamed>";
	switch(s->type)
	{
	  case EEL_SUNDEFINED:
		snprintf(buf, EEL_SBUFSIZE, "<undef symbol '%s'>", n);
		return buf;
	  case EEL_SKEYWORD:
		snprintf(buf, EEL_SBUFSIZE, "keyword '%s'", n);
		return buf;
	  case EEL_SCLASS:
		snprintf(buf, EEL_SBUFSIZE, "class '%s'", n);
		return buf;
	  case EEL_SCONSTANT:
		eel_sfree(VMP->state, buf);
		return eel_v_stringrep(vm, &s->v.value);
	  case EEL_SVARIABLE:
		snprintf(buf, EEL_SBUFSIZE, "variable '%s'", n);
		return buf;
	  case EEL_SUPVALUE:
		snprintf(buf, EEL_SBUFSIZE, "upvalue '%s'", n);
		return buf;
	  case EEL_SOPERATOR:
		snprintf(buf, EEL_SBUFSIZE, "operator '%s'", n);
		return buf;
	  case EEL_SBODY:
		snprintf(buf, EEL_SBUFSIZE, "body '%s'", n);
		return buf;
	  case EEL_SFUNCTION:
		snprintf(buf, EEL_SBUFSIZE, "function '%s'", n);
		return buf;
	  case EEL_SMODULE:
		snprintf(buf, EEL_SBUFSIZE, "module '%s'", n);
		return buf;
	  case EEL_SNAMESPACE:
		snprintf(buf, EEL_SBUFSIZE, "namespace '%s'", n);
		return buf;
	}
	eel_sfree(VMP->state, buf);
	return "<internal error: undef symbol type>";
}


static const char *symstring(EEL_state *es, EEL_symbol *s, int w)
{
	char *buf = eel_salloc(es);
	const char *n = s->name ? eel_o2s(s->name) : "<unnamed>";
	snprintf(buf, EEL_SBUFSIZE, "%-*s %s", w, n, eel_symbol_is(s));
	return buf;
}


#define	S_TAB	4

static void dump_constants(EEL_state *es, EEL_function *f, int indent)
{
	int i;
	const char *tmp;
	char *inds = malloc(indent+2);
	memset(inds, ' ', indent);
	inds[indent+1] = 0;
	for(i = 0; i <= indent; i += S_TAB)
		inds[i] = '|';
	for(i = 0; i < f->e.nconstants; ++i)
	{
		tmp = eel_v_stringrep(es->vm, &f->e.constants[i]);
		printf("%s   C%d = %s\n", inds, i, tmp);
		eel_sfree(es, tmp);
	}
	free(inds);
}

static void dump_code(EEL_state *es, EEL_object *fo, int indent)
{
	EEL_function *f = o2EEL_function(fo);
	int i;
	char *inds = malloc(indent+2);
	memset(inds, ' ', indent);
	inds[indent+1] = 0;
	for(i = 0; i <= indent; i += S_TAB)
		inds[i] = '|';
	if(f->e.codesize)
	{
		int pc = 0;
		while(pc < f->e.codesize)
		{
			const char *tmp = eel_i_stringrep(es, fo, pc);
			printf("%s%6.1d: %s\n", inds, pc, tmp);
			eel_sfree(es, tmp);
			pc += eel_i_size(f->e.code[pc]);
		}
	}
	else
		printf("%s       <no code>\n", inds);
	free(inds);
}

static void st_dump(EEL_state *es, EEL_symbol *st, int code, int indent)
{
	int i, wrap;
	const char *tmp;
	char *inds = malloc(indent + 2);
	memset(inds, ' ', indent + 2);
	for(i = 0; i <= indent; i += S_TAB)
		inds[i] = '|';
	inds[indent + 1] = 0;
	wrap = st->symbols || !st->parent || (EEL_SFUNCTION == st->type) ||
			(st->parent && (st->uvlevel != st->parent->uvlevel));
	if(wrap)
	{
		inds[indent] = '.';
		printf("%s---------------------------------------\n", inds);
		inds[indent] = '|';
	}
	else
		inds[indent] = ' ';

	switch(st->type)
	{
	  case EEL_SFUNCTION:
	  {
		EEL_function *f;
		if(!st->v.object)
		{
			printf("%s function %s\n", inds,
					st->name ? eel_o2s(st->name) : "<unnamed>");
			printf("%s Object missing!\n", inds);
			break;
		}
		f = o2EEL_function(st->v.object);
		if(f->common.results)
		{
			printf("%s function ", inds);
			printf("(%d)", f->common.results);
		}
		else
			printf("%s procedure ", inds);
		fputs(st->name ? eel_o2s(st->name) : "<unnamed>", stdout);
		if(f->common.reqargs)
			printf("(%d)", f->common.reqargs);
		if(255 == f->common.optargs)
			printf("[...]");
		else if(f->common.optargs > 0)
			printf("[%d]", f->common.optargs);
		if(f->common.tupargs)
			printf("<%d>", f->common.tupargs);
		printf("\n");
		if(!(f->common.flags & EEL_FF_CFUNC))
			dump_constants(es, f, indent);
		break;
	  }
	  case EEL_SMODULE:
	  {
		if(!st->v.object)
		{
			printf("%s module %s\n", inds,
					st->name ? eel_o2s(st->name) : "<unnamed>");
			printf("%s Object missing!\n", inds);
			break;
		}
		tmp = symstring(es, st, 16);
		printf("%s %s\n", inds, tmp);
		eel_sfree(es, tmp);
		printf("%s source: \"%s\"\n", inds,
				eel_module_filename(st->v.object));
		break;
	  }
	  case EEL_SVARIABLE:
	  case EEL_SUPVALUE:
		tmp = symstring(es, st, 16);
		printf("%s %s", inds, tmp);
		eel_sfree(es, tmp);
		switch(st->v.var.kind)
		{
		  case EVK_STACK:
			printf(", R%d\n", st->v.var.location);
			break;
		  case EVK_STATIC:
			printf(", SV%d\n", st->v.var.location);
			break;
		  case EVK_ARGUMENT:
		  case EVK_OPTARG:
			printf(", args[%d]\n", st->v.var.location);
			break;
		  case EVK_TUPARG:
			printf(", index %d\n", st->v.var.location);
			break;
		}
		break;
	  default:
		tmp = symstring(es, st, 16);
		printf("%s %s\n", inds, tmp);
		eel_sfree(es, tmp);
		break;
	}

	if(!st->parent)
		printf("%s uvlevel: 0\n", inds);
	else if(st->uvlevel != st->parent->uvlevel)
		printf("%s uvlevel: %d\n", inds, st->uvlevel);

	if((EEL_SFUNCTION == st->type) && code && st->v.object)
		if(!(o2EEL_function(st->v.object)->common.flags & EEL_FF_CFUNC))
			printf("%s framesize: %d\n", inds,
					(o2EEL_function(st->v.object)->e.framesize));

	if(wrap && st->symbols)
	{
		EEL_symbol *s = st->symbols;
		printf("%s---------------------------------------\n", inds);
		while(s)
		{
			st_dump(es, s, code, indent + S_TAB);
			s = s->next;
		}
	}

	if((EEL_SFUNCTION == st->type) && code && st->v.object)
	{
		printf("%s---------------------------------------\n", inds);
		if((o2EEL_function(st->v.object)->common.flags & EEL_FF_CFUNC))
			printf("%s native code: %p\n", inds,
					o2EEL_function(st->v.object)->c.cb);
		else
			dump_code(es, st->v.object, indent);
	}

	if(wrap)
	{
		inds[indent] = '\'';
		printf("%s---------------------------------------\n", inds);
	}
	free(inds);
}


void eel_s_dump(EEL_vm *vm, EEL_symbol *st, int code)
{
	if(!st)
		st = VMP->state->root_symtab;
	while(st)
	{
		st_dump(VMP->state, st, code, 0);
		st = st->next;
	}
}


EEL_symbol *eel_find_symbol(EEL_vm *vm, EEL_symbol *scope, const char *name)
{
	EEL_state *es = VMP->state;
	char buf[128];
	EEL_finder f;
	EEL_symbol *s = NULL;
	int i = 0;
	DBGJ(printf("Trying to find '%s'...\n", name);)
	if(!scope)
		scope = es->root_symtab;
	while(1)
		switch(*name)
		{
		  case 0:
		  case '.':
			DBGJ(printf("  %p:\n", scope);)
			eel_finder_init(es, &f, scope, ESTF_NAME | ESTF_TYPES);
			buf[i] = 0;
			i = 0;
			f.name = eel_ps_new(es->vm, buf);
			f.types =	EEL_SMKEYWORD |
					EEL_SMCLASS |
					EEL_SMCONSTANT |
					EEL_SMVARIABLE |
					EEL_SMFUNCTION |
					EEL_SMMODULE |
					EEL_SMOPERATOR;
			scope = s = eel_finder_go(&f);
			eel_o_disown_nz(f.name);
			DBGJ(printf("'%s' ==> %p\n", buf, s);)
			if(*name++)
				break;
			else
				return s;
		  default:
			if(i >= sizeof(buf))
				eel_ierror(es, "Too long object or field name!");
			buf[i++] = *name++;
			break;
		}
}


/*----------------------------------------------------------
	Run-time Version control
----------------------------------------------------------*/

EEL_uint32 eel_lib_version(void)
{
	/*
	 * Note that this compiles into the version
	 * of the *library*... obviously! :-)
	 */
	return EEL_COMPILED_VERSION | EEL_SNAPSHOT;
}


/*---------------------------------------------------------
	Low level interfaces
---------------------------------------------------------*/

void *eel_rawdata(EEL_object *o)
{
	switch((EEL_classes)o->type)
	{
	  case EEL_CSTRING:
		return o2EEL_string(o)->buffer;
	  case EEL_CDSTRING:
		return o2EEL_dstring(o)->buffer;
	  case EEL_CVECTOR_U8:
	  case EEL_CVECTOR_S8:
	  case EEL_CVECTOR_U16:
	  case EEL_CVECTOR_S16:
	  case EEL_CVECTOR_U32:
	  case EEL_CVECTOR_S32:
	  case EEL_CVECTOR_F:
	  case EEL_CVECTOR_D:
		return o2EEL_vector(o)->buffer.u8;
	  case EEL_CARRAY:
		return o2EEL_array(o)->values;
	  default:
		return NULL;
	}
}