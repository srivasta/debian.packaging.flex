/* flex - tool to generate fast lexical analyzers */

/*  Copyright (c) 1990 The Regents of the University of California. */
/*  All rights reserved. */

/*  This code is derived from software contributed to Berkeley by */
/*  Vern Paxson. */

/*  The United States Government has rights in this work pursuant */
/*  to contract no. DE-AC03-76SF00098 between the United States */
/*  Department of Energy and the University of California. */

/*  This file is part of flex. */

/*  Redistribution and use in source and binary forms, with or without */
/*  modification, are permitted provided that the following conditions */
/*  are met: */

/*  1. Redistributions of source code must retain the above copyright */
/*     notice, this list of conditions and the following disclaimer. */
/*  2. Redistributions in binary form must reproduce the above copyright */
/*     notice, this list of conditions and the following disclaimer in the */
/*     documentation and/or other materials provided with the distribution. */

/*  Neither the name of the University nor the names of its contributors */
/*  may be used to endorse or promote products derived from this software */
/*  without specific prior written permission. */

/*  THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR */
/*  IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED */
/*  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR */
/*  PURPOSE. */

#include "flexdef.h"

/* Take note: The buffer object is sometimes used as a String buffer (one
 * continuous string), and sometimes used as a list of strings, usually line by
 * line.
 *
 * The type is specified in buf_init by the elt_size. If the elt_size is
 * sizeof(char), then the buffer should be treated as string buffer. If the
 * elt_size is sizeof(char*), then the buffer should be treated as a list of
 * strings.
 *
 * Certain functions are only appropriate for one type or the other.
 */

/* global buffers. */
struct Buf userdef_buf;		/**< for user #definitions triggered by cmd-line. */
struct Buf defs_buf;		/**< for #define's autogenerated. List of strings. */
struct Buf yydmap_buf;		/**< string buffer to hold yydmap elements */
struct Buf m4defs_buf;          /**< m4 definitions. List of strings. */
struct Buf top_buf;             /**< contains %top code. String buffer. */

struct Buf *buf_print_strings(struct Buf * buf, FILE* out)
{
    int i;

    if(!buf || !out)
        return buf;

    for (i=0; i < buf->nelts; i++){
        const char * s = ((char**)buf->elts)[i];
        if(s)
            fprintf(out, "%s", s);
    }
    return buf;
}

/* Append a "%s" formatted string to a string buffer */
struct Buf *buf_prints (struct Buf *buf, const char *fmt, const char *s)
{
	char   *t;
        size_t tsz;

	t = flex_alloc (tsz = strlen (fmt) + strlen (s) + 1);
	if (!t)
	    flexfatal (_("Allocation of buffer to print string failed"));
	snprintf (t, tsz, fmt, s);
	buf = buf_strappend (buf, t);
	flex_free (t);
	return buf;
}

/** Append a line directive to the string buffer.
 * @param buf A string buffer.
 * @param filename file name
 * @param lineno line number
 * @return buf
 */
struct Buf *buf_linedir (struct Buf *buf, const char* filename, int lineno)
{
    char *dst, *t;
    const char *src;
    size_t tsz;

    if (gen_line_dirs)
	return buf;

    tsz = strlen("#line \"\"\n")                +   /* constant parts */
               2 * strlen (filename)            +   /* filename with possibly all backslashes escaped */
               (int) (1 + log10 (abs (lineno))) +   /* line number */
               1;                                   /* NUL */
    t = malloc(tsz);
    if (!t)
      flexfatal (_("Allocation of buffer for line directive failed"));
    for (dst = t + sprintf (t, "#line %d \"", lineno), src = filename; *src; *dst++ = *src++)
      if (*src == '\\')   /* escape backslashes */
        *dst++ = '\\';
    *dst++ = '"';
    *dst++ = '\n';
    *dst   = '\0';
    buf = buf_strappend (buf, t);
    flex_free (t);
    return buf;
}


/** Append the contents of @a src to @a dest.
 * @param @a dest the destination buffer
 * @param @a dest the source buffer
 * @return @a dest
 */
struct Buf *buf_concat(struct Buf* dest, const struct Buf* src)
{
    buf_append(dest, src->elts, src->nelts);
    return dest;
}


/* Appends n characters in str to buf. */
struct Buf *buf_strnappend (buf, str, n)
     struct Buf *buf;
     const char *str;
     int n;
{
	buf_append (buf, str, n + 1);

	/* "undo" the '\0' character that buf_append() already copied. */
	buf->nelts--;

	return buf;
}

/* Appends characters in str to buf. */
struct Buf *buf_strappend (buf, str)
     struct Buf *buf;
     const char *str;
{
	return buf_strnappend (buf, str, strlen (str));
}

/* appends "#define str def\n" */
struct Buf *buf_strdefine (buf, str, def)
     struct Buf *buf;
     const char *str;
     const char *def;
{
	buf_strappend (buf, "#define ");
	buf_strappend (buf, " ");
	buf_strappend (buf, str);
	buf_strappend (buf, " ");
	buf_strappend (buf, def);
	buf_strappend (buf, "\n");
	return buf;
}

/** Pushes "m4_define( [[def]], [[val]])m4_dnl" to end of buffer.
 * @param buf A buffer as a list of strings.
 * @param def The m4 symbol to define.
 * @param val The definition; may be NULL.
 * @return buf
 */
struct Buf *buf_m4_define (struct Buf *buf, const char* def, const char* val)
{
    const char * fmt = "m4_define( [[%s]], [[%s]])m4_dnl\n";
    char * str;
    size_t strsz;

    val = val?val:"";
    str = (char*)flex_alloc(strsz = strlen(fmt) + strlen(def) + strlen(val) + 2);
    if (!str)
        flexfatal (_("Allocation of buffer for m4 def failed"));

    snprintf(str, strsz, fmt, def, val);
    buf_append(buf, &str, 1);
    return buf;
}

/** Pushes "m4_undefine([[def]])m4_dnl" to end of buffer.
 * @param buf A buffer as a list of strings.
 * @param def The m4 symbol to undefine.
 * @return buf
 */
struct Buf *buf_m4_undefine (struct Buf *buf, const char* def)
{
    const char * fmt = "m4_undefine( [[%s]])m4_dnl\n";
    char * str;
    size_t strsz;

    str = (char*)flex_alloc(strsz = strlen(fmt) + strlen(def) + 2);
    if (!str)
        flexfatal (_("Allocation of buffer for m4 undef failed"));

    snprintf(str, strsz, fmt, def);
    buf_append(buf, &str, 1);
    return buf;
}

/* create buf with 0 elements, each of size elem_size. */
void buf_init (buf, elem_size)
     struct Buf *buf;
     size_t elem_size;
{
	buf->elts = (void *) 0;
	buf->nelts = 0;
	buf->elt_size = elem_size;
	buf->nmax = 0;
}

/* frees memory */
void buf_destroy (buf)
     struct Buf *buf;
{
	if (buf && buf->elts)
		flex_free (buf->elts);
	buf->elts = (void *) 0;
}


/* appends ptr[] to buf, grow if necessary.
 * n_elem is number of elements in ptr[], NOT bytes.
 * returns buf.
 * We grow by mod(512) boundaries.
 */

struct Buf *buf_append (buf, ptr, n_elem)
     struct Buf *buf;
     const void *ptr;
     int n_elem;
{
	int     n_alloc = 0;

	if (!ptr || n_elem == 0)
		return buf;

	/* May need to alloc more. */
	if (n_elem + buf->nelts > buf->nmax) {

		/* exact amount needed... */
		n_alloc = (n_elem + buf->nelts) * buf->elt_size;

		/* ...plus some extra */
		if (((n_alloc * buf->elt_size) % 512) != 0
		    && buf->elt_size < 512)
			n_alloc +=
				(512 -
				 ((n_alloc * buf->elt_size) % 512)) /
				buf->elt_size;

		if (!buf->elts)
			buf->elts =
				allocate_array (n_alloc, buf->elt_size);
		else
			buf->elts =
				reallocate_array (buf->elts, n_alloc,
						  buf->elt_size);

		buf->nmax = n_alloc;
	}

	memcpy ((char *) buf->elts + buf->nelts * buf->elt_size, ptr,
		n_elem * buf->elt_size);
	buf->nelts += n_elem;

	return buf;
}

/* vim:set tabstop=8 softtabstop=4 shiftwidth=4: */
