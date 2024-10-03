/*
 * Portions Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

/* quirc -- QR-code recognition library
 * Copyright (C) 2010-2012 Daniel Beer <dlbeer@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "ocean/cv/detector/qrcodes/MicroQRCodeDecoder.h"

#include "ocean/cv/detector/qrcodes/MicroQRCodeEncoder.h"

#include "ocean/math/Numeric.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

namespace {

// clang-format off

#ifdef __cplusplus
	extern "C" {
#endif

// Quirc
// Code: https://github.com/dlbeer/quirc
// Commit: 542848dd6b9b0eaa9587bbf25b9bc67bd8a71fca

// quirc.h ---------------------------------------------------------------------

/* quirc -- QR-code recognition library
 * Copyright (C) 2010-2012 Daniel Beer <dlbeer@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef QUIRC_H_
#define QUIRC_H_

//#include <stdint.h>

//#ifdef __cplusplus
//extern "C" {
//#endif

struct quirc;

/* Obtain the library version string. */
const char *quirc_version(void);

/* Construct a new QR-code recognizer. This function will return nullptr
 * if sufficient memory could not be allocated.
 */
struct quirc *quirc_new(void);

/* Destroy a QR-code recognizer. */
void quirc_destroy(struct quirc *q);

/* Resize the QR-code recognizer. The size of an image must be
 * specified before codes can be analyzed.
 *
 * This function returns 0 on success, or -1 if sufficient memory could
 * not be allocated.
 */
int quirc_resize(struct quirc *q, int w, int h);

/* These functions are used to process images for QR-code recognition.
 * quirc_begin() must first be called to obtain access to a buffer into
 * which the input image should be placed. Optionally, the current
 * width and height may be returned.
 *
 * After filling the buffer, quirc_end() should be called to process
 * the image for QR-code recognition. The locations and content of each
 * code may be obtained using accessor functions described below.
 */
uint8_t *quirc_begin(struct quirc *q, int *w, int *h);
void quirc_end(struct quirc *q);

/* This structure describes a location in the input image buffer. */
struct quirc_point {
	int	x;
	int	y;
};

/* This enum describes the various decoder errors which may occur. */
typedef enum {
	QUIRC_SUCCESS = 0,
	QUIRC_ERROR_INVALID_GRID_SIZE,
	QUIRC_ERROR_INVALID_VERSION,
	QUIRC_ERROR_FORMAT_ECC,
	QUIRC_ERROR_DATA_ECC,
	QUIRC_ERROR_UNKNOWN_DATA_TYPE,
	QUIRC_ERROR_DATA_OVERFLOW,
	QUIRC_ERROR_DATA_UNDERFLOW
} quirc_decode_error_t;

/* Return a string error message for an error code. */
const char *quirc_strerror(quirc_decode_error_t err);

/* Limits on the maximum size of QR-codes and their content. */
#define QUIRC_MAX_VERSION	40
#define QUIRC_MAX_GRID_SIZE	(QUIRC_MAX_VERSION * 4 + 17)
#define QUIRC_MAX_BITMAP	(((QUIRC_MAX_GRID_SIZE * QUIRC_MAX_GRID_SIZE) + 7) / 8)
#define QUIRC_MAX_PAYLOAD	8896

/* QR-code ECC types. */
#define QUIRC_ECC_LEVEL_M     0
#define QUIRC_ECC_LEVEL_L     1
#define QUIRC_ECC_LEVEL_H     2
#define QUIRC_ECC_LEVEL_Q     3

/* QR-code data types. */
#define QUIRC_DATA_TYPE_NUMERIC       1
#define QUIRC_DATA_TYPE_ALPHA         2
#define QUIRC_DATA_TYPE_BYTE          4
#define QUIRC_DATA_TYPE_KANJI         8

/* Common character encodings */
#define QUIRC_ECI_ISO_8859_1		1
#define QUIRC_ECI_IBM437		2
#define QUIRC_ECI_ISO_8859_2		4
#define QUIRC_ECI_ISO_8859_3		5
#define QUIRC_ECI_ISO_8859_4		6
#define QUIRC_ECI_ISO_8859_5		7
#define QUIRC_ECI_ISO_8859_6		8
#define QUIRC_ECI_ISO_8859_7		9
#define QUIRC_ECI_ISO_8859_8		10
#define QUIRC_ECI_ISO_8859_9		11
#define QUIRC_ECI_WINDOWS_874		13
#define QUIRC_ECI_ISO_8859_13		15
#define QUIRC_ECI_ISO_8859_15		17
#define QUIRC_ECI_SHIFT_JIS		20
#define QUIRC_ECI_UTF_8			26

/* This structure is used to return information about detected QR codes
 * in the input image.
 */
struct quirc_code {
	/* The four corners of the QR-code, from top left, clockwise */
	struct quirc_point	corners[4];

	/* The number of cells across in the QR-code. The cell bitmap
	 * is a bitmask giving the actual values of cells. If the cell
	 * at (x, y) is black, then the following bit is set:
	 *
	 *     cell_bitmap[i >> 3] & (1 << (i & 7))
	 *
	 * where i = (y * size) + x.
	 */
	int			size;
	uint8_t			cell_bitmap[QUIRC_MAX_BITMAP];
};

/* This structure holds the decoded QR-code data */
struct quirc_data {
	/* Various parameters of the QR-code. These can mostly be
	 * ignored if you only care about the data.
	 */
	int			version;
	int			ecc_level;
	int			mask;

	/* This field is the highest-valued data type found in the QR
	 * code.
	 */
	int			data_type;

	/* Data payload. For the Kanji datatype, payload is encoded as
	 * Shift-JIS. For all other datatypes, payload is ASCII text.
	 */
	uint8_t			payload[QUIRC_MAX_PAYLOAD];
	int			payload_len;

	/* ECI assignment number */
	uint32_t		eci;
};

/* Return the number of QR-codes identified in the last processed
 * image.
 */
int quirc_count(const struct quirc *q);

/* Extract the QR-code specified by the given index. */
void quirc_extract(const struct quirc *q, int index,
		   struct quirc_code *code);

/* Decode a QR-code, returning the payload data. */
quirc_decode_error_t quirc_decode(const struct quirc_code *code,
				  struct quirc_data *data);

/* Flip a QR-code according to optional mirror feature of ISO 18004:2015 */
void quirc_flip(struct quirc_code *code);

//#ifdef __cplusplus
//}
//#endif

#endif

// quirc.h - EOF ---------------------------------------------------------------

// quirc_internal.h ------------------------------------------------------------

/* quirc -- QR-code recognition library
 * Copyright (C) 2010-2012 Daniel Beer <dlbeer@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef QUIRC_INTERNAL_H_
#define QUIRC_INTERNAL_H_

//#include <assert.h>
//#include <stdlib.h>

//#include "quirc.h"

#define QUIRC_ASSERT(a)	ocean_assert(a)

#define QUIRC_PIXEL_WHITE	0
#define QUIRC_PIXEL_BLACK	1
#define QUIRC_PIXEL_REGION	2

#ifndef QUIRC_MAX_REGIONS
#define QUIRC_MAX_REGIONS	254
#endif
#define QUIRC_MAX_CAPSTONES	32
#define QUIRC_MAX_GRIDS		(QUIRC_MAX_CAPSTONES * 2)

#define QUIRC_PERSPECTIVE_PARAMS	8

#if QUIRC_MAX_REGIONS < UINT8_MAX
#define QUIRC_PIXEL_ALIAS_IMAGE	1
typedef uint8_t quirc_pixel_t;
#elif QUIRC_MAX_REGIONS < UINT16_MAX
#define QUIRC_PIXEL_ALIAS_IMAGE	0
typedef uint16_t quirc_pixel_t;
#else
#error "QUIRC_MAX_REGIONS > 65534 is not supported"
#endif

#ifdef QUIRC_FLOAT_TYPE
/* Quirc uses double precision floating point internally by default.
 * On platforms with a single precision FPU but no double precision FPU,
 * this can be changed to float by defining QUIRC_FLOAT_TYPE.
 *
 * When setting QUIRC_FLOAT_TYPE to 'float', consider also defining QUIRC_USE_TGMATH.
 * This will use the type-generic math functions (tgmath.h, C99 or later) instead of the normal ones,
 * which will allow the compiler to use the correct overloaded functions for the type.
 */
typedef QUIRC_FLOAT_TYPE quirc_float_t;
#else
typedef double quirc_float_t;
#endif

struct quirc_region {
	struct quirc_point	seed;
	int			count;
	int			capstone;
};

struct quirc_capstone {
	int			ring;
	int			stone;

	struct quirc_point	corners[4];
	struct quirc_point	center;
	quirc_float_t		c[QUIRC_PERSPECTIVE_PARAMS];

	int			qr_grid;
};

struct quirc_grid {
	/* Capstone indices */
	int			caps[3];

	/* Alignment pattern region and corner */
	int			align_region;
	struct quirc_point	align;

	/* Timing pattern endpoints */
	struct quirc_point	tpep[3];

	/* Grid size and perspective transform */
	int			grid_size;
	quirc_float_t		c[QUIRC_PERSPECTIVE_PARAMS];
};

struct quirc_flood_fill_vars {
	int y;
	int right;
	int left_up;
	int left_down;
};

struct quirc {
	uint8_t			*image;
	quirc_pixel_t		*pixels;
	int			w;
	int			h;

	int			num_regions;
	struct quirc_region	regions[QUIRC_MAX_REGIONS];

	int			num_capstones;
	struct quirc_capstone	capstones[QUIRC_MAX_CAPSTONES];

	int			num_grids;
	struct quirc_grid	grids[QUIRC_MAX_GRIDS];

	size_t      		num_flood_fill_vars;
	struct quirc_flood_fill_vars *flood_fill_vars;
};

/************************************************************************
 * QR-code version information database
 */

#define QUIRC_MAX_VERSION     40
#define QUIRC_MAX_ALIGNMENT   7

struct quirc_rs_params {
	int             bs; /* Small block size */
	int             dw; /* Small data words */
	int		ns; /* Number of small blocks */
};

struct quirc_version_info {
	int				data_bytes;
	int				apat[QUIRC_MAX_ALIGNMENT];
	struct quirc_rs_params          ecc[4];
};

#endif

// quirc_internal.h - EOF ------------------------------------------------------

// decode.c --------------------------------------------------------------------

/* quirc -- QR-code recognition library
 * Copyright (C) 2010-2012 Daniel Beer <dlbeer@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

//#include "quirc_internal.h"
//
//#include <string.h>
//#include <stdlib.h>

#define MAX_POLY       64

/************************************************************************
 * Galois fields
 */

struct galois_field {
	int p;
	const uint8_t *log;
	const uint8_t *exp;
};

static const uint8_t gf16_exp[16] = {
	0x01, 0x02, 0x04, 0x08, 0x03, 0x06, 0x0c, 0x0b,
	0x05, 0x0a, 0x07, 0x0e, 0x0f, 0x0d, 0x09, 0x01
};

static const uint8_t gf16_log[16] = {
	0x00, 0x0f, 0x01, 0x04, 0x02, 0x08, 0x05, 0x0a,
	0x03, 0x0e, 0x09, 0x07, 0x06, 0x0d, 0x0b, 0x0c
};

static const struct galois_field gf16 = {
	.p = 15,
	.log = gf16_log,
	.exp = gf16_exp
};

static const uint8_t gf256_exp[256] = {
	0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,
	0x1d, 0x3a, 0x74, 0xe8, 0xcd, 0x87, 0x13, 0x26,
	0x4c, 0x98, 0x2d, 0x5a, 0xb4, 0x75, 0xea, 0xc9,
	0x8f, 0x03, 0x06, 0x0c, 0x18, 0x30, 0x60, 0xc0,
	0x9d, 0x27, 0x4e, 0x9c, 0x25, 0x4a, 0x94, 0x35,
	0x6a, 0xd4, 0xb5, 0x77, 0xee, 0xc1, 0x9f, 0x23,
	0x46, 0x8c, 0x05, 0x0a, 0x14, 0x28, 0x50, 0xa0,
	0x5d, 0xba, 0x69, 0xd2, 0xb9, 0x6f, 0xde, 0xa1,
	0x5f, 0xbe, 0x61, 0xc2, 0x99, 0x2f, 0x5e, 0xbc,
	0x65, 0xca, 0x89, 0x0f, 0x1e, 0x3c, 0x78, 0xf0,
	0xfd, 0xe7, 0xd3, 0xbb, 0x6b, 0xd6, 0xb1, 0x7f,
	0xfe, 0xe1, 0xdf, 0xa3, 0x5b, 0xb6, 0x71, 0xe2,
	0xd9, 0xaf, 0x43, 0x86, 0x11, 0x22, 0x44, 0x88,
	0x0d, 0x1a, 0x34, 0x68, 0xd0, 0xbd, 0x67, 0xce,
	0x81, 0x1f, 0x3e, 0x7c, 0xf8, 0xed, 0xc7, 0x93,
	0x3b, 0x76, 0xec, 0xc5, 0x97, 0x33, 0x66, 0xcc,
	0x85, 0x17, 0x2e, 0x5c, 0xb8, 0x6d, 0xda, 0xa9,
	0x4f, 0x9e, 0x21, 0x42, 0x84, 0x15, 0x2a, 0x54,
	0xa8, 0x4d, 0x9a, 0x29, 0x52, 0xa4, 0x55, 0xaa,
	0x49, 0x92, 0x39, 0x72, 0xe4, 0xd5, 0xb7, 0x73,
	0xe6, 0xd1, 0xbf, 0x63, 0xc6, 0x91, 0x3f, 0x7e,
	0xfc, 0xe5, 0xd7, 0xb3, 0x7b, 0xf6, 0xf1, 0xff,
	0xe3, 0xdb, 0xab, 0x4b, 0x96, 0x31, 0x62, 0xc4,
	0x95, 0x37, 0x6e, 0xdc, 0xa5, 0x57, 0xae, 0x41,
	0x82, 0x19, 0x32, 0x64, 0xc8, 0x8d, 0x07, 0x0e,
	0x1c, 0x38, 0x70, 0xe0, 0xdd, 0xa7, 0x53, 0xa6,
	0x51, 0xa2, 0x59, 0xb2, 0x79, 0xf2, 0xf9, 0xef,
	0xc3, 0x9b, 0x2b, 0x56, 0xac, 0x45, 0x8a, 0x09,
	0x12, 0x24, 0x48, 0x90, 0x3d, 0x7a, 0xf4, 0xf5,
	0xf7, 0xf3, 0xfb, 0xeb, 0xcb, 0x8b, 0x0b, 0x16,
	0x2c, 0x58, 0xb0, 0x7d, 0xfa, 0xe9, 0xcf, 0x83,
	0x1b, 0x36, 0x6c, 0xd8, 0xad, 0x47, 0x8e, 0x01
};

static const uint8_t gf256_log[256] = {
	0x00, 0xff, 0x01, 0x19, 0x02, 0x32, 0x1a, 0xc6,
	0x03, 0xdf, 0x33, 0xee, 0x1b, 0x68, 0xc7, 0x4b,
	0x04, 0x64, 0xe0, 0x0e, 0x34, 0x8d, 0xef, 0x81,
	0x1c, 0xc1, 0x69, 0xf8, 0xc8, 0x08, 0x4c, 0x71,
	0x05, 0x8a, 0x65, 0x2f, 0xe1, 0x24, 0x0f, 0x21,
	0x35, 0x93, 0x8e, 0xda, 0xf0, 0x12, 0x82, 0x45,
	0x1d, 0xb5, 0xc2, 0x7d, 0x6a, 0x27, 0xf9, 0xb9,
	0xc9, 0x9a, 0x09, 0x78, 0x4d, 0xe4, 0x72, 0xa6,
	0x06, 0xbf, 0x8b, 0x62, 0x66, 0xdd, 0x30, 0xfd,
	0xe2, 0x98, 0x25, 0xb3, 0x10, 0x91, 0x22, 0x88,
	0x36, 0xd0, 0x94, 0xce, 0x8f, 0x96, 0xdb, 0xbd,
	0xf1, 0xd2, 0x13, 0x5c, 0x83, 0x38, 0x46, 0x40,
	0x1e, 0x42, 0xb6, 0xa3, 0xc3, 0x48, 0x7e, 0x6e,
	0x6b, 0x3a, 0x28, 0x54, 0xfa, 0x85, 0xba, 0x3d,
	0xca, 0x5e, 0x9b, 0x9f, 0x0a, 0x15, 0x79, 0x2b,
	0x4e, 0xd4, 0xe5, 0xac, 0x73, 0xf3, 0xa7, 0x57,
	0x07, 0x70, 0xc0, 0xf7, 0x8c, 0x80, 0x63, 0x0d,
	0x67, 0x4a, 0xde, 0xed, 0x31, 0xc5, 0xfe, 0x18,
	0xe3, 0xa5, 0x99, 0x77, 0x26, 0xb8, 0xb4, 0x7c,
	0x11, 0x44, 0x92, 0xd9, 0x23, 0x20, 0x89, 0x2e,
	0x37, 0x3f, 0xd1, 0x5b, 0x95, 0xbc, 0xcf, 0xcd,
	0x90, 0x87, 0x97, 0xb2, 0xdc, 0xfc, 0xbe, 0x61,
	0xf2, 0x56, 0xd3, 0xab, 0x14, 0x2a, 0x5d, 0x9e,
	0x84, 0x3c, 0x39, 0x53, 0x47, 0x6d, 0x41, 0xa2,
	0x1f, 0x2d, 0x43, 0xd8, 0xb7, 0x7b, 0xa4, 0x76,
	0xc4, 0x17, 0x49, 0xec, 0x7f, 0x0c, 0x6f, 0xf6,
	0x6c, 0xa1, 0x3b, 0x52, 0x29, 0x9d, 0x55, 0xaa,
	0xfb, 0x60, 0x86, 0xb1, 0xbb, 0xcc, 0x3e, 0x5a,
	0xcb, 0x59, 0x5f, 0xb0, 0x9c, 0xa9, 0xa0, 0x51,
	0x0b, 0xf5, 0x16, 0xeb, 0x7a, 0x75, 0x2c, 0xd7,
	0x4f, 0xae, 0xd5, 0xe9, 0xe6, 0xe7, 0xad, 0xe8,
	0x74, 0xd6, 0xf4, 0xea, 0xa8, 0x50, 0x58, 0xaf
};

static const struct galois_field gf256 = {
	.p = 255,
	.log = gf256_log,
	.exp = gf256_exp
};

/************************************************************************
 * Polynomial operations
 */

static void poly_add(uint8_t *dst, const uint8_t *src, uint8_t c,
		     int shift, const struct galois_field *gf)
{
	int i;
	int log_c = gf->log[c];

	if (!c)
		return;

	for (i = 0; i < MAX_POLY; i++) {
		int p = i + shift;
		uint8_t v = src[i];

		if (p < 0 || p >= MAX_POLY)
			continue;
		if (!v)
			continue;

		dst[p] ^= gf->exp[(gf->log[v] + log_c) % gf->p];
	}
}

static uint8_t poly_eval(const uint8_t *s, uint8_t x,
			 const struct galois_field *gf)
{
	int i;
	uint8_t sum = 0;
	uint8_t log_x = gf->log[x];

	if (!x)
		return s[0];

	for (i = 0; i < MAX_POLY; i++) {
		uint8_t c = s[i];

		if (!c)
			continue;

		sum ^= gf->exp[(gf->log[c] + log_x * i) % gf->p];
	}

	return sum;
}

/************************************************************************
 * Berlekamp-Massey algorithm for finding error locator polynomials.
 */

static void berlekamp_massey(const uint8_t *s, int N,
			     const struct galois_field *gf,
			     uint8_t *sigma)
{
	uint8_t C[MAX_POLY];
	uint8_t B[MAX_POLY];
	int L = 0;
	int m = 1;
	uint8_t b = 1;
	int n;

	memset(B, 0, sizeof(B));
	memset(C, 0, sizeof(C));
	B[0] = 1;
	C[0] = 1;

	for (n = 0; n < N; n++) {
		uint8_t d = s[n];
		uint8_t mult;
		int i;

		for (i = 1; i <= L; i++) {
			if (!(C[i] && s[n - i]))
				continue;

			d ^= gf->exp[(gf->log[C[i]] +
				      gf->log[s[n - i]]) %
				     gf->p];
		}

		mult = gf->exp[(gf->p - gf->log[b] + gf->log[d]) % gf->p];

		if (!d) {
			m++;
		} else if (L * 2 <= n) {
			uint8_t T[MAX_POLY];

			memcpy(T, C, sizeof(T));
			poly_add(C, B, mult, m, gf);
			memcpy(B, T, sizeof(B));
			L = n + 1 - L;
			b = d;
			m = 1;
		} else {
			poly_add(C, B, mult, m, gf);
			m++;
		}
	}

	memcpy(sigma, C, MAX_POLY);
}

/************************************************************************
 * Code stream error correction
 *
 * Generator polynomial for GF(2^8) is x^8 + x^4 + x^3 + x^2 + 1
 */

static int block_syndromes(const uint8_t *data, int bs, int npar, uint8_t *s)
{
	int nonzero = 0;
	int i;

	memset(s, 0, MAX_POLY);

	for (i = 0; i < npar; i++) {
		int j;

		for (j = 0; j < bs; j++) {
			uint8_t c = data[bs - j - 1];

			if (!c)
				continue;

			s[i] ^= gf256_exp[((int)gf256_log[c] +
				    i * j) % 255];
		}

		if (s[i])
			nonzero = 1;
	}

	return nonzero;
}

static void eloc_poly(uint8_t *omega,
		      const uint8_t *s, const uint8_t *sigma,
		      int npar)
{
	int i;

	memset(omega, 0, MAX_POLY);

	for (i = 0; i < npar; i++) {
		const uint8_t a = sigma[i];
		const uint8_t log_a = gf256_log[a];
		int j;

		if (!a)
			continue;

		for (j = 0; j + 1 < MAX_POLY; j++) {
			const uint8_t b = s[j + 1];

			if (i + j >= npar)
				break;

			if (!b)
				continue;

			omega[i + j] ^=
			    gf256_exp[(log_a + gf256_log[b]) % 255];
		}
	}
}

static quirc_decode_error_t correct_block(uint8_t *data,
					  const struct quirc_rs_params *ecc)
{
	int npar = ecc->bs - ecc->dw;
	uint8_t s[MAX_POLY];
	uint8_t sigma[MAX_POLY];
	uint8_t sigma_deriv[MAX_POLY];
	uint8_t omega[MAX_POLY];
	int i;

	/* Compute syndrome vector */
	if (!block_syndromes(data, ecc->bs, npar, s))
		return QUIRC_SUCCESS;

	berlekamp_massey(s, npar, &gf256, sigma);

	/* Compute derivative of sigma */
	memset(sigma_deriv, 0, MAX_POLY);
	for (i = 0; i + 1 < MAX_POLY; i += 2)
		sigma_deriv[i] = sigma[i + 1];

	/* Compute error evaluator polynomial */
	eloc_poly(omega, s, sigma, npar - 1);

	/* Find error locations and magnitudes */
	for (i = 0; i < ecc->bs; i++) {
		uint8_t xinv = gf256_exp[255 - i];

		if (!poly_eval(sigma, xinv, &gf256)) {
			uint8_t sd_x = poly_eval(sigma_deriv, xinv, &gf256);
			uint8_t omega_x = poly_eval(omega, xinv, &gf256);
			uint8_t error = gf256_exp[(255 - gf256_log[sd_x] +
						   gf256_log[omega_x]) % 255];

			data[ecc->bs - i - 1] ^= error;
		}
	}

	if (block_syndromes(data, ecc->bs, npar, s))
		return QUIRC_ERROR_DATA_ECC;

	return QUIRC_SUCCESS;
}

/************************************************************************
 * Format value error correction
 *
 * Generator polynomial for GF(2^4) is x^4 + x + 1
 */

#define FORMAT_MAX_ERROR        3
#define FORMAT_SYNDROMES        (FORMAT_MAX_ERROR * 2)
#define FORMAT_BITS             15

static int format_syndromes(uint16_t u, uint8_t *s)
{
	int i;
	int nonzero = 0;

	memset(s, 0, MAX_POLY);

	for (i = 0; i < FORMAT_SYNDROMES; i++) {
		int j;

		s[i] = 0;
		for (j = 0; j < FORMAT_BITS; j++)
			if (u & (1 << j))
				s[i] ^= gf16_exp[((i + 1) * j) % 15];

		if (s[i])
			nonzero = 1;
	}

	return nonzero;
}

static quirc_decode_error_t correct_format(uint16_t *f_ret)
{
	uint16_t u = *f_ret;
	int i;
	uint8_t s[MAX_POLY];
	uint8_t sigma[MAX_POLY];

	/* Evaluate U (received codeword) at each of alpha_1 .. alpha_6
	 * to get S_1 .. S_6 (but we index them from 0).
	 */
	if (!format_syndromes(u, s))
		return QUIRC_SUCCESS;

	berlekamp_massey(s, FORMAT_SYNDROMES, &gf16, sigma);

	/* Now, find the roots of the polynomial */
	for (i = 0; i < 15; i++)
		if (!poly_eval(sigma, gf16_exp[15 - i], &gf16))
			u ^= (1 << i);

	if (format_syndromes(u, s))
		return QUIRC_ERROR_FORMAT_ECC;

	*f_ret = u;
	return QUIRC_SUCCESS;
}

/// File Truncated

// decode.c - EOF --------------------------------------------------------------

#ifdef __cplusplus
}
#endif

// clang-format on

} // namespace

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace QRCodes
{

static uint16_t readFormatBits(const std::vector<uint8_t>& modules, const unsigned int modulesPerSide)
{
	ocean_assert(modules.size() == modulesPerSide * modulesPerSide);

	uint16_t format = 0;

	static const std::array<int, 15> xs = {
		8, 8, 8, 8, 8, 8, 8, 8, 7, 6, 5, 4, 3, 2, 1
	};
	static const std::array<int, 15> ys = {
		1, 2, 3, 4, 5, 6, 7, 8, 8, 8, 8, 8, 8, 8, 8
	};
	static_assert(xs.size() == ys.size());
	static_assert(xs.size() < 8u * sizeof(format));

	for (size_t i = xs.size() - 1u; i < xs.size(); i--) {
		format = (format << 1) | (modules[xs[i] + ys[i] * modulesPerSide] == 0u ? 0u : 1u);
	}

	return format;
}

static bool decodeFormatBits(const uint16_t formatBits, uint32_t& version, MicroQRCode::ErrorCorrectionCapacity& errorCorrectionCapacity, MicroQRCodeEncoder::MaskingPattern& maskingPattern, unsigned int& symbolNumber)
{
	ocean_assert(formatBits == (formatBits & 0x7fff));
	uint16_t format = formatBits ^ 0x4445;

	quirc_decode_error_t err = correct_format(&format);
	if (err) 
	{
		return false;
	}

	uint8_t fdata = format >> 10;

	symbolNumber = fdata >> 2;

	switch(symbolNumber) {
		case 0u:
			version = 1u;
			errorCorrectionCapacity = MicroQRCode::ECC_DETECTION_ONLY;
			break;
		case 7u:
			version = 4u;
			errorCorrectionCapacity = MicroQRCode::ECC_25;
			break;
		default:
			version = (symbolNumber + 3u) >> 1u;
			errorCorrectionCapacity = (symbolNumber & 1) ? MicroQRCode::ECC_07 : MicroQRCode::ECC_15;
			break;
	}

	maskingPattern = MicroQRCodeEncoder::MaskingPattern(fdata & 3);

	return true;
}

static bool dataMask(const MicroQRCodeEncoder::MaskingPattern mask, const unsigned int x, const unsigned int y)
{
	switch(mask)
	{
		case MicroQRCodeEncoder::MP_PATTERN_0:
			return y % 2u == 0u;
		case MicroQRCodeEncoder::MP_PATTERN_1:
			return (x / 3u + y / 2u) % 2u == 0u;
		case MicroQRCodeEncoder::MP_PATTERN_2:
			return (x * y % 2u + x * y % 3u) % 2u == 0u;
		case MicroQRCodeEncoder::MP_PATTERN_3:
			return ((x + y) % 2u + x * y % 3u) % 2u == 0u;
		default:
			ocean_assert(false && "This should never happen");
			return 0u;
	}
}

static void getCodewords(const std::vector<uint8_t>& modules, const unsigned int version, const MicroQRCode::ErrorCorrectionCapacity errorCorrectionCapacity, const MicroQRCodeEncoder::MaskingPattern mask, std::vector<uint8_t>& codewords)
{
	ocean_assert(version >= 1u && version <= MicroQRCode::MAX_VERSION);

	static const std::array<int, 4> codewordCounts = {5u, 10u, 17u, 24u};

	codewords.resize(codewordCounts[version - 1u]);

	// All codewords are 8 bits in length, except in versions M1 and M3 where the final data codeword is 4 bits in length
	// Because the codewords are stored as bytes, the final data codeword of M1 and M3 is padded with 4 zero bits
	unsigned int startOfHalfByteToSkip = (unsigned int)(-1);
	if (version == 1u)
	{
		startOfHalfByteToSkip = 20u;
	}
	else if (version == 3u)
	{
		switch(errorCorrectionCapacity)
		{
			case QRCodeBase::ECC_07:
				startOfHalfByteToSkip = 84u;
				break;
			case QRCodeBase::ECC_15:
				startOfHalfByteToSkip = 68u;
				break;
			default:
				ocean_assert(false && "Invalid format");
				break;
		}
	}

	const unsigned int size = MicroQRCode::modulesPerSide(version);

	size_t i = 0; // Bit index into the data

	// Do the funny zigzag scan
	for (unsigned int right = size - 1u; right > 0; right -= 2u) // Index of right column in each column pair
	{
		for (unsigned int vert = 0u; vert < size; vert++) // Vertical counter
		{
			for (unsigned int j = 0u; j < 2u; j++)
			{
				unsigned int x = right - j; // Actual x coordinate
				bool upward = ((size - right) & 2u) == 0u;
				int y = upward ? size - 1u - vert : vert; // Actual y coordinate

				unsigned int index = y * size + x;
				ocean_assert(index < size * size);

				// Skip timing patterns
				if (x == 0 || y == 0) {
					continue;
				}

				// Skip finder pattern
				if (x <= 8 && y <= 8) {
					continue;
				}

				if (i < codewords.size() * 8u)
				{
					if (i == startOfHalfByteToSkip)
					{
						i += 4u;
					}

					if (modules[index] ^ dataMask(mask, x, y))
					{
						uint8_t& codeword = codewords[i >> 3];
						const unsigned int bit = 7u - (i & 7u);
						codeword |= (1u << bit);
					}

					i++;
				}
			}
		}
	}

	ocean_assert(i == codewords.size() * 8);
}

static bool applyErrorCorrection(const unsigned int symbolNumber, std::vector<uint8_t>& codewords)
{
	static const struct quirc_rs_params ecc_params[8] = {
		{.bs = 5, .dw = 3, .ns = 1},
		{.bs = 10, .dw = 5, .ns = 1},
		{.bs = 10, .dw = 4, .ns = 1},
		{.bs = 17, .dw = 11, .ns = 1},
		{.bs = 17, .dw = 9, .ns = 1},
		{.bs = 24, .dw = 16, .ns = 1},
		{.bs = 24, .dw = 14, .ns = 1},
		{.bs = 24, .dw = 10, .ns = 1}};

	const struct quirc_rs_params *ecc = &ecc_params[symbolNumber];

	uint8_t *dst = codewords.data();
	quirc_decode_error_t err = correct_block(dst, ecc);
	return err == QUIRC_SUCCESS;
}

MicroQRCodeDecoder::BitStream::BitStream(std::vector<uint8_t>&& buffer, const unsigned int numberOfBits) : buffer_(std::move(buffer)), bitsConsumed_(0u), bytesConsumed_(0u), bitsRemaining_(numberOfBits)
{
	ocean_assert(buffer_.size() * 8u >= numberOfBits);
}

bool MicroQRCodeDecoder::BitStream::consumeBit()
{
	ocean_assert(bitsRemaining_ > 0u);

	const bool bit = (buffer_[bytesConsumed_] >> (7u - bitsConsumed_)) & 1u;

	bitsRemaining_--;
	bitsConsumed_++;
	if (bitsConsumed_ == 8u)
	{
		bitsConsumed_ = 0u;
		bytesConsumed_++;
	}

	return bit;
}

uint32_t MicroQRCodeDecoder::BitStream::consumeBits(const unsigned int numberOfBits)
{
	ocean_assert(numberOfBits <= 32u);

	const unsigned int bitsToRead = std::min(numberOfBits, bitsRemaining_);
	unsigned int bitsTaken = 0u;

	uint32_t result = 0u;

	while (bitsTaken < bitsToRead)
	{
		result = (result << 1u) | consumeBit();
		bitsTaken++;
	}

	// Pad with zeros if we ran out of bits
	result <<= (numberOfBits - bitsTaken);
	
	return result;
}

bool MicroQRCodeDecoder::BitStream::peekNonzeroBits(const unsigned int numberOfBits) const
{
	const unsigned int bitsToRead = std::min(numberOfBits, bitsRemaining_);
	unsigned int bitIndex = bitsConsumed_;
	unsigned int byteIndex = bytesConsumed_;

	for (unsigned int i = 0u; i < bitsToRead; ++i)
	{
		const uint8_t byte = buffer_[byteIndex];
		if ((0x80u >> bitIndex) & byte)
		{
			return true;
		}

		bitIndex++;
		if (bitIndex == 8u)
		{
			bitIndex = 0u;
			byteIndex++;
		}
	}

	return false;
}

static bool decodeNumericSegment(const unsigned int version, MicroQRCodeDecoder::BitStream& bitstream, std::vector<uint8_t>& data)
{
	ocean_assert(version >= MicroQRCode::MIN_VERSION && version <= MicroQRCode::MAX_VERSION);

	const unsigned int characterCountBits = version + 2u;

	if (bitstream.bitsRemaining() < characterCountBits)
	{
		return false;
	}

	unsigned int characterCount = bitstream.consumeBits(characterCountBits);

	data.reserve(data.size() + characterCount);
	
	while (characterCount >= 3u)
	{
		if (bitstream.bitsRemaining() < 10u)
		{
			return false;
		}

		const unsigned int digits = bitstream.consumeBits(10u);

		if (digits >= 1000u)
		{
			return false;
		}

		data.push_back((digits / 100u) + '0');
		data.push_back(((digits / 10u) % 10u) + '0');
		data.push_back((digits % 10u) + '0');

		characterCount -= 3u;
	}
	
	if (characterCount == 2u)
	{
		if (bitstream.bitsRemaining() < 7u)
		{
			return false;
		}

		const unsigned int digits = bitstream.consumeBits(7u);

		if (digits >= 100u)
		{
			return false;
		}

		data.push_back(digits / 10u + '0');
		data.push_back(digits % 10u + '0');

		characterCount -= 2u;
	}

	if (characterCount == 1u)
	{
		if (bitstream.bitsRemaining() < 4u)
		{
			return false;
		}

		const unsigned int digit = bitstream.consumeBits(4u);

		if (digit >= 10u)
		{
			return false;
		}

		data.push_back(digit + '0');

		characterCount -= 1u;
	}

	ocean_assert(characterCount == 0u);

	return true;
}

static bool decodeAlphanumericSegment(const unsigned int version, MicroQRCodeDecoder::BitStream& bitstream, std::vector<uint8_t>& data)
{
	ocean_assert(version >= 2u && version <= MicroQRCode::MAX_VERSION);

	const unsigned int characterCountBits = version + 1u;

	if (bitstream.bitsRemaining() < characterCountBits)
	{
		return false;
	}

	unsigned int characterCount = bitstream.consumeBits(characterCountBits);

	data.reserve(data.size() + characterCount);

	const std::string& alphaChars = QRCodeEncoderBase::Segment::getAlphanumericCharset();

	while (characterCount >= 2u)
	{
		if (bitstream.bitsRemaining() < 11u)
		{
			return false;
		}

		const unsigned int codepair = bitstream.consumeBits(11u);

		if (codepair >= 45u * 45u)
		{
			return false;
		}

		data.push_back(alphaChars[codepair / 45u]);
		data.push_back(alphaChars[codepair % 45u]);

		characterCount -= 2u;
	}

	if (characterCount == 1u)
	{
		if (bitstream.bitsRemaining() < 6u)
		{
			return false;
		}

		const unsigned int code = bitstream.consumeBits(6u);

		if (code >= 45u)
		{
			return false;
		}

		data.push_back(alphaChars[code]);

		characterCount -= 1u;
	}

	ocean_assert(characterCount == 0u);

	return true;
}

static bool decodeByteSegment(const unsigned int version, MicroQRCodeDecoder::BitStream& bitstream, std::vector<uint8_t>& data)
{
	ocean_assert(version >= 3u && version <= MicroQRCode::MAX_VERSION);

	const unsigned int characterCountBits = version + 1u;

	if (bitstream.bitsRemaining() < characterCountBits)
	{
		return false;
	}

	const unsigned int characterCount = bitstream.consumeBits(characterCountBits);

	data.reserve(data.size() + characterCount);

	if (bitstream.bitsRemaining() < characterCount * 8u)
	{
		return false;
	}

	for (unsigned int i = 0u; i < characterCount; ++i)
	{
		data.push_back(bitstream.consumeBits(8u));
	}

	return true;
}

static bool decodeBitStream(const unsigned int version, MicroQRCodeDecoder::BitStream& bitstream, MicroQRCode::EncodingMode& encodingMode, std::vector<uint8_t>& data)
{
	const unsigned int modeIndicatorLength = version - 1u;
	const unsigned int terminatorLength = version * 2u + 1u;

	MicroQRCode::EncodingMode highestMode = MicroQRCode::EM_INVALID_ENCODING_MODE;

	while (bitstream.bitsRemaining() >= modeIndicatorLength && bitstream.peekNonzeroBits(terminatorLength))
	{
		const MicroQRCode::EncodingMode mode = MicroQRCode::EncodingMode(bitstream.consumeBits(modeIndicatorLength));

		switch(mode)
		{
			case MicroQRCode::EM_NUMERIC:
				if (!decodeNumericSegment(version, bitstream, data))
				{
					return false;
				}
				break;
			case MicroQRCode::EM_ALPHANUMERIC:
				if (!decodeAlphanumericSegment(version, bitstream, data))
				{
					return false;
				}
				break;
			case MicroQRCode::EM_BYTE:
				if (!decodeByteSegment(version, bitstream, data))
				{
					return false;
				}
				break;
			case MicroQRCode::EM_KANJI:
				Log::warning() << "Kanji encoding mode is not supported";
				return false;
			default:
				ocean_assert(false && "This should never happen");
				return false;
		}

		if (highestMode == MicroQRCode::EM_INVALID_ENCODING_MODE || highestMode < mode)
		{
			highestMode = mode;
		}
	}

	encodingMode = highestMode;

	return true;
}

static bool decodeModules(const std::vector<uint8_t>& modules, unsigned int& version, MicroQRCode::EncodingMode& encodingMode, MicroQRCode::ErrorCorrectionCapacity& errorCorrectionCapacity, std::vector<uint8_t>& data, std::vector<uint8_t>& finalModules)
{
	unsigned int modulesPerSide;
	unsigned int provisionalVersion;
	switch(modules.size())
	{
		case 11u * 11u:
			modulesPerSide = 11;
			provisionalVersion = 1;
			break;
		case 13u * 13u:
			modulesPerSide = 13;
			provisionalVersion = 2;
			break;
		case 15u * 15u:
			modulesPerSide = 15;
			provisionalVersion = 3;
			break;
		case 17u * 17u:
			modulesPerSide = 17;
			provisionalVersion = 4;
			break;
		default:
			ocean_assert(false && "Invalid number of modules");
			return false;
	}
	
	// Read format information
	const uint16_t formatBits = readFormatBits(modules, modulesPerSide);
	MicroQRCodeEncoder::MaskingPattern maskingPattern;
	unsigned int symbolNumber;
	if (decodeFormatBits(formatBits, version, errorCorrectionCapacity, maskingPattern, symbolNumber) && version == provisionalVersion)
	{
		finalModules = modules;
	}
	else
	{
		// Try reversing the format bits. If that works, then all of the modules need to be mirrored.
		uint16_t reversedFormatBits = formatBits & (1 << 7u);
		for (unsigned int i = 0; i < 7; ++i)
		{
			reversedFormatBits |= (((formatBits >> i) & 1u) << (14u - i));
		}
		if (decodeFormatBits(reversedFormatBits, version, errorCorrectionCapacity, maskingPattern, symbolNumber) && version == provisionalVersion)
		{
			for (unsigned int x = 0u; x < modulesPerSide; x++)
			{
				for (unsigned int y = 0u; y < modulesPerSide; y++)
				{
					const unsigned int i = x + modulesPerSide * y;
					const unsigned int j = y + modulesPerSide * x;
					finalModules[i] = modules[j];
					finalModules[j] = modules[i];
				}
			}
		}
		else
		{
			return false;
		}
	}

	std::vector<uint8_t> codewords;
	getCodewords(finalModules, version, errorCorrectionCapacity, maskingPattern, codewords);
	if (!applyErrorCorrection(symbolNumber, codewords))
	{
		return false;
	}

	static const std::array<unsigned int, 8> dataCapacityBits = {20, 40, 32, 84, 68, 128, 112, 80};

	MicroQRCodeDecoder::BitStream bitStream(std::move(codewords), dataCapacityBits[symbolNumber]);
	return decodeBitStream(version, bitStream, encodingMode, data);
}

bool MicroQRCodeDecoder::decodeMicroQRCode(const std::vector<uint8_t>& modules, MicroQRCode& code)
{
	// Decode the modules
	MicroQRCode::ErrorCorrectionCapacity errorCorrectionCapacity = MicroQRCode::ECC_INVALID;
	MicroQRCode::EncodingMode encodingMode = MicroQRCode::EM_INVALID_ENCODING_MODE;
	std::vector<uint8_t> decodedData;
	unsigned int version = 0u;

	std::vector<uint8_t> finalModules;

	if (!decodeModules(modules, version, encodingMode, errorCorrectionCapacity, decodedData, finalModules))
	{
		return false;
	}

	ocean_assert(version >= MicroQRCode::MIN_VERSION && version <= MicroQRCode::MAX_VERSION);
	ocean_assert(errorCorrectionCapacity != MicroQRCode::ECC_INVALID);
	ocean_assert(encodingMode != MicroQRCode::EM_INVALID_ENCODING_MODE);

	if (decodedData.empty())
	{
		return false;
	}

	code = MicroQRCode(std::move(decodedData), encodingMode, errorCorrectionCapacity, std::move(finalModules), version);
	ocean_assert(code.isValid());

	return code.isValid();
}

} // namespace QRCodes

} // namespace Detector

} // namespace CV

} // namespace Ocean
