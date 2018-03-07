/*
 * string fragments, for libreswan
 *
 * Copyright (C) 1997 Angelos D. Keromytis.
 * Copyright (C) 1998-2001, 2013 D. Hugh Redelmeier <hugh@mimosa.com>
 * Copyright (C) 2012 Paul Wouters <paul@libreswan.org>
 * Copyright (C) 2013 Tuomo Soini <tis@foobar.fi>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.  See <http://www.fsf.org/copyleft/gpl.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */

#ifndef SHUNK_H
#define SHUNK_H

#include <stddef.h>	/* size_t */

/*
 * shunk_t is a rip of of chunk_t, but with a character pointer.  It
 * is intended for string slicing.
 */

struct shunk {
	const char *ptr;
	size_t len;
};

typedef struct shunk shunk_t;

extern const shunk_t empty_shunk;

shunk_t shunk1(const char *ptr); /* strlen() implied */
shunk_t shunk2(const char *ptr, int len);

/*
 * To print, use: printf(PRISHUNK, SHUNKF(shunk));
 */
#define PRISHUNK "%.*s"
#define SHUNKF(SHUNK) (int) (SHUNK).len, (SHUNK).ptr

#endif
