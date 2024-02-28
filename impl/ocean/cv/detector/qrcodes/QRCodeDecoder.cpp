// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/cv/detector/qrcodes/QRCodeDecoder.h"

#include "ocean/math/Numeric.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

namespace {

// clang-format off

#ifdef __cplusplus
	extern "C" {
#endif

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

/* Construct a new QR-code recognizer. This function will return NULL
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
#define QUIRC_MAX_BITMAP	3917
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

//#include "quirc.h"

#define QUIRC_PIXEL_WHITE	0
#define QUIRC_PIXEL_BLACK	1
#define QUIRC_PIXEL_REGION	2

#ifndef QUIRC_MAX_REGIONS
#define QUIRC_MAX_REGIONS	254
#endif
#define QUIRC_MAX_CAPSTONES	32
#define QUIRC_MAX_GRIDS		8

#define QUIRC_PERSPECTIVE_PARAMS	8

#if QUIRC_MAX_REGIONS < UINT8_MAX
typedef uint8_t quirc_pixel_t;
#elif QUIRC_MAX_REGIONS < UINT16_MAX
typedef uint16_t quirc_pixel_t;
#else
#error "QUIRC_MAX_REGIONS > 65534 is not supported"
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
	double			c[QUIRC_PERSPECTIVE_PARAMS];

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
	int			hscan;
	int			vscan;

	/* Grid size and perspective transform */
	int			grid_size;
	double			c[QUIRC_PERSPECTIVE_PARAMS];
};

struct quirc {
	uint8_t			*image;
	quirc_pixel_t		*pixels;
	int			*row_average; /* used by threshold() */
	int			w;
	int			h;

	int			num_regions;
	struct quirc_region	regions[QUIRC_MAX_REGIONS];

	int			num_capstones;
	struct quirc_capstone	capstones[QUIRC_MAX_CAPSTONES];

	int			num_grids;
	struct quirc_grid	grids[QUIRC_MAX_GRIDS];
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

extern const struct quirc_version_info quirc_version_db[QUIRC_MAX_VERSION + 1];

#endif

// quirc_internal.h - EOF ------------------------------------------------------

// version_db.c ----------------------------------------------------------------

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


const struct quirc_version_info quirc_version_db[QUIRC_MAX_VERSION + 1] = {
	{ },
	{ /* Version 1 */
		26, // .data_bytes
		{ 0 }, // .apat
		{ // .ecc
			{ 26, 16, 1 }, // {.bs, .dw, .ns}
			{ 26, 19, 1 }, // {.bs, .dw, .ns}
			{ 26, 9, 1 }, // {.bs, .dw, .ns}
			{ 26, 13, 1 } // {.bs, .dw, .ns}
		}
	},
	{ /* Version 2 */
		44, // .data_bytes
		{ 6, 18, 0 }, // .apat
		{ // .ecc
			{ 44, 28, 1 }, // {.bs, .dw, .ns}
			{ 44, 34, 1 }, // {.bs, .dw, .ns}
			{ 44, 16, 1 }, // {.bs, .dw, .ns}
			{ 44, 22, 1 } // {.bs, .dw, .ns}
		}
	},
	{ /* Version 3 */
		70, // .data_bytes
		{ 6, 22, 0 }, // .apat
		{ // .ecc
			{ 70, 44, 1 }, // {.bs, .dw, .ns}
			{ 70, 55, 1 }, // {.bs, .dw, .ns}
			{ 35, 13, 2 }, // {.bs, .dw, .ns}
			{ 35, 17, 2 } // {.bs, .dw, .ns}
		}
	},
	{ /* Version 4 */
		100, // .data_bytes
		{ 6, 26, 0 }, // .apat
		{ // .ecc
			{ 50, 32, 2 }, // {.bs, .dw, .ns}
			{ 100, 80, 1 }, // {.bs, .dw, .ns}
			{ 25, 9, 4 }, // {.bs, .dw, .ns}
			{ 50, 24, 2 } // {.bs, .dw, .ns}
		}
	},
	{ /* Version 5 */
		134, // .data_bytes
		{ 6, 30, 0 }, // .apat
		{ // .ecc
			{ 67, 43, 2 }, // {.bs, .dw, .ns}
			{ 134, 108, 1 }, // {.bs, .dw, .ns}
			{ 33, 11, 2 }, // {.bs, .dw, .ns}
			{ 33, 15, 2 } // {.bs, .dw, .ns}
		}
	},
	{ /* Version 6 */
		172, // .data_bytes
		{ 6, 34, 0 }, // .apat
		{ // .ecc
			{ 43, 27, 4 }, // {.bs, .dw, .ns}
			{ 86, 68, 2 }, // {.bs, .dw, .ns}
			{ 43, 15, 4 }, // {.bs, .dw, .ns}
			{ 43, 19, 4 } // {.bs, .dw, .ns}
		}
	},
	{ /* Version 7 */
		196, // .data_bytes
		{ 6, 22, 38, 0 }, // .apat
		{ // .ecc
			{ 49, 31, 4 }, // {.bs, .dw, .ns}
			{ 98, 78, 2 }, // {.bs, .dw, .ns}
			{ 39, 13, 4 }, // {.bs, .dw, .ns}
			{ 32, 14, 2 } // {.bs, .dw, .ns}
		}
	},
	{ /* Version 8 */
		242, // .data_bytes
		{ 6, 24, 42, 0 }, // .apat
		{ // .ecc
			{ 60, 38, 2 }, // {.bs, .dw, .ns}
			{ 121, 97, 2 }, // {.bs, .dw, .ns}
			{ 40, 14, 4 }, // {.bs, .dw, .ns}
			{ 40, 18, 4 } // {.bs, .dw, .ns}
		}
	},
	{ /* Version 9 */
		292, // .data_bytes
		{ 6, 26, 46, 0 }, // .apat
		{ // .ecc
			{ 58, 36, 3 }, // {.bs, .dw, .ns}
			{ 146, 116, 2 }, // {.bs, .dw, .ns}
			{ 36, 12, 4 }, // {.bs, .dw, .ns}
			{ 36, 16, 4 } // {.bs, .dw, .ns}
		}
	},
	{ /* Version 10 */
		346, // .data_bytes
		{ 6, 28, 50, 0 }, // .apat
		{ // .ecc
			{ 69, 43, 4 }, // {.bs, .dw, .ns}
			{ 86, 68, 2 }, // {.bs, .dw, .ns}
			{ 43, 15, 6 }, // {.bs, .dw, .ns}
			{ 43, 19, 6 } // {.bs, .dw, .ns}
		}
	},
	{ /* Version 11 */
		404, // .data_bytes
		{ 6, 30, 54, 0 }, // .apat
		{ // .ecc
			{ 80, 50, 1 }, // {.bs, .dw, .ns}
			{ 101, 81, 4 }, // {.bs, .dw, .ns}
			{ 36, 12, 3 }, // {.bs, .dw, .ns}
			{ 50, 22, 4 } // {.bs, .dw, .ns}
		}
	},
	{ /* Version 12 */
		466, // .data_bytes
		{ 6, 32, 58, 0 }, // .apat
		{ // .ecc
			{ 58, 36, 6 }, // {.bs, .dw, .ns}
			{ 116, 92, 2 }, // {.bs, .dw, .ns}
			{ 42, 14, 7 }, // {.bs, .dw, .ns}
			{ 46, 20, 4 } // {.bs, .dw, .ns}
		}
	},
	{ /* Version 13 */
		532, // .data_bytes
		{ 6, 34, 62, 0 }, // .apat
		{ // .ecc
			{ 59, 37, 8 }, // {.bs, .dw, .ns}
			{ 133, 107, 4 }, // {.bs, .dw, .ns}
			{ 33, 11, 12 }, // {.bs, .dw, .ns}
			{ 44, 20, 8 } // {.bs, .dw, .ns}
		}
	},
	{ /* Version 14 */
		581, // .data_bytes
		{ 6, 26, 46, 66, 0 }, // .apat
		{ // .ecc
			{ 64, 40, 4 }, // {.bs, .dw, .ns}
			{ 145, 115, 3 }, // {.bs, .dw, .ns}
			{ 36, 12, 11 }, // {.bs, .dw, .ns}
			{ 36, 16, 11 } // {.bs, .dw, .ns}
		}
	},
	{ /* Version 15 */
		655, // .data_bytes
		{ 6, 26, 48, 70, 0 }, // .apat
		{ // .ecc
			{ 65, 41, 5 }, // {.bs, .dw, .ns}
			{ 109, 87, 5 }, // {.bs, .dw, .ns}
			{ 36, 12, 11 }, // {.bs, .dw, .ns}
			{ 54, 24, 5 } // {.bs, .dw, .ns}
		}
	},
	{ /* Version 16 */
		733, // .data_bytes
		{ 6, 26, 50, 74, 0 }, // .apat
		{ // .ecc
			{ 73, 45, 7 }, // {.bs, .dw, .ns}
			{ 122, 98, 5 }, // {.bs, .dw, .ns}
			{ 45, 15, 3 }, // {.bs, .dw, .ns}
			{ 43, 19, 15 } // {.bs, .dw, .ns}
		}
	},
	{ /* Version 17 */
		815, // .data_bytes
		{ 6, 30, 54, 78, 0 }, // .apat
		{ // .ecc
			{ 74, 46, 10 }, // {.bs, .dw, .ns}
			{ 135, 107, 1 }, // {.bs, .dw, .ns}
			{ 42, 14, 2 }, // {.bs, .dw, .ns}
			{ 50, 22, 1 } // {.bs, .dw, .ns}
		}
	},
	{ /* Version 18 */
		901, // .data_bytes
		{ 6, 30, 56, 82, 0 }, // .apat
		{ // .ecc
			{ 69, 43, 9 }, // {.bs, .dw, .ns}
			{ 150, 120, 5 }, // {.bs, .dw, .ns}
			{ 42, 14, 2 }, // {.bs, .dw, .ns}
			{ 50, 22, 17 } // {.bs, .dw, .ns}
		}
	},
	{ /* Version 19 */
		991, // .data_bytes
		{ 6, 30, 58, 86, 0 }, // .apat
		{ // .ecc
			{ 70, 44, 3 }, // {.bs, .dw, .ns}
			{ 141, 113, 3 }, // {.bs, .dw, .ns}
			{ 39, 13, 9 }, // {.bs, .dw, .ns}
			{ 47, 21, 17 } // {.bs, .dw, .ns}
		}
	},
	{ /* Version 20 */
		1085, // .data_bytes
		{ 6, 34, 62, 90, 0 }, // .apat
		{ // .ecc
			{ 67, 41, 3 }, // {.bs, .dw, .ns}
			{ 135, 107, 3 }, // {.bs, .dw, .ns}
			{ 43, 15, 15 }, // {.bs, .dw, .ns}
			{ 54, 24, 15 } // {.bs, .dw, .ns}
		}
	},
	{ /* Version 21 */
		1156, // .data_bytes
		{ 6, 28, 50, 72, 92, 0 }, // .apat
		{ // .ecc
			{ 68, 42, 17 }, // {.bs, .dw, .ns}
			{ 144, 116, 4 }, // {.bs, .dw, .ns}
			{ 46, 16, 19 }, // {.bs, .dw, .ns}
			{ 50, 22, 17 } // {.bs, .dw, .ns}
		}
	},
	{ /* Version 22 */
		1258, // .data_bytes
		{ 6, 26, 50, 74, 98, 0 }, // .apat
		{ // .ecc
			{ 74, 46, 17 }, // {.bs, .dw, .ns}
			{ 139, 111, 2 }, // {.bs, .dw, .ns}
			{ 37, 13, 34 }, // {.bs, .dw, .ns}
			{ 54, 24, 7 } // {.bs, .dw, .ns}
		}
	},
	{ /* Version 23 */
		1364, // .data_bytes
		{ 6, 30, 54, 78, 102, 0 }, // .apat
		{ // .ecc
			{ 75, 47, 4 }, // {.bs, .dw, .ns}
			{ 151, 121, 4 }, // {.bs, .dw, .ns}
			{ 45, 15, 16 }, // {.bs, .dw, .ns}
			{ 54, 24, 11 } // {.bs, .dw, .ns}
		}
	},
	{ /* Version 24 */
		1474, // .data_bytes
		{ 6, 28, 54, 80, 106, 0 }, // .apat
		{ // .ecc
			{ 73, 45, 6 }, // {.bs, .dw, .ns}
			{ 147, 117, 6 }, // {.bs, .dw, .ns}
			{ 46, 16, 30 }, // {.bs, .dw, .ns}
			{ 54, 24, 11 } // {.bs, .dw, .ns}
		}
	},
	{ /* Version 25 */
		1588, // .data_bytes
		{ 6, 32, 58, 84, 110, 0 }, // .apat
		{ // .ecc
			{ 75, 47, 8 }, // {.bs, .dw, .ns}
			{ 132, 106, 8 }, // {.bs, .dw, .ns}
			{ 45, 15, 22 }, // {.bs, .dw, .ns}
			{ 54, 24, 7 } // {.bs, .dw, .ns}
		}
	},
	{ /* Version 26 */
		1706, // .data_bytes
		{ 6, 30, 58, 86, 114, 0 }, // .apat
		{ // .ecc
			{ 74, 46, 19 }, // {.bs, .dw, .ns}
			{ 142, 114, 10 }, // {.bs, .dw, .ns}
			{ 46, 16, 33 }, // {.bs, .dw, .ns}
			{ 50, 22, 28 } // {.bs, .dw, .ns}
		}
	},
	{ /* Version 27 */
		1828, // .data_bytes
		{ 6, 34, 62, 90, 118, 0 }, // .apat
		{ // .ecc
			{ 73, 45, 22 }, // {.bs, .dw, .ns}
			{ 152, 122, 8 }, // {.bs, .dw, .ns}
			{ 45, 15, 12 }, // {.bs, .dw, .ns}
			{ 53, 23, 8 } // {.bs, .dw, .ns}
		}
	},
	{ /* Version 28 */
		1921, // .data_bytes
		{ 6, 26, 50, 74, 98, 122, 0 }, // .apat
		{ // .ecc
			{ 73, 45, 3 }, // {.bs, .dw, .ns}
			{ 147, 117, 3 }, // {.bs, .dw, .ns}
			{ 45, 15, 11 }, // {.bs, .dw, .ns}
			{ 54, 24, 4 } // {.bs, .dw, .ns}
		}
	},
	{ /* Version 29 */
		2051, // .data_bytes
		{ 6, 30, 54, 78, 102, 126, 0 }, // .apat
		{ // .ecc
			{ 73, 45, 21 }, // {.bs, .dw, .ns}
			{ 146, 116, 7 }, // {.bs, .dw, .ns}
			{ 45, 15, 19 }, // {.bs, .dw, .ns}
			{ 53, 23, 1 } // {.bs, .dw, .ns}
		}
	},
	{ /* Version 30 */
		2185, // .data_bytes
		{ 6, 26, 52, 78, 104, 130, 0 }, // .apat
		{ // .ecc
			{ 75, 47, 19 }, // {.bs, .dw, .ns}
			{ 145, 115, 5 }, // {.bs, .dw, .ns}
			{ 45, 15, 23 }, // {.bs, .dw, .ns}
			{ 54, 24, 15 } // {.bs, .dw, .ns}
		}
	},
	{ /* Version 31 */
		2323, // .data_bytes
		{ 6, 30, 56, 82, 108, 134, 0 }, // .apat
		{ // .ecc
			{ 74, 46, 2 }, // {.bs, .dw, .ns}
			{ 145, 115, 13 }, // {.bs, .dw, .ns}
			{ 45, 15, 23 }, // {.bs, .dw, .ns}
			{ 54, 24, 42 } // {.bs, .dw, .ns}
		}
	},
	{ /* Version 32 */
		2465, // .data_bytes
		{ 6, 34, 60, 86, 112, 138, 0 }, // .apat
		{ // .ecc
			{ 74, 46, 10 }, // {.bs, .dw, .ns}
			{ 145, 115, 17 }, // {.bs, .dw, .ns}
			{ 45, 15, 19 }, // {.bs, .dw, .ns}
			{ 54, 24, 10 } // {.bs, .dw, .ns}
		}
	},
	{ /* Version 33 */
		2611, // .data_bytes
		{ 6, 30, 58, 86, 114, 142, 0 }, // .apat
		{ // .ecc
			{ 74, 46, 14 }, // {.bs, .dw, .ns}
			{ 145, 115, 17 }, // {.bs, .dw, .ns}
			{ 45, 15, 11 }, // {.bs, .dw, .ns}
			{ 54, 24, 29 } // {.bs, .dw, .ns}
		}
	},
	{ /* Version 34 */
		2761, // .data_bytes
		{ 6, 34, 62, 90, 118, 146, 0 }, // .apat
		{ // .ecc
			{ 74, 46, 14 }, // {.bs, .dw, .ns}
			{ 145, 115, 13 }, // {.bs, .dw, .ns}
			{ 46, 16, 59 }, // {.bs, .dw, .ns}
			{ 54, 24, 44 } // {.bs, .dw, .ns}
		}
	},
	{ /* Version 35 */
		2876, // .data_bytes
		{ 6, 30, 54, 78, 102, 126, 150 }, // .apat
		{ // .ecc
			{ 75, 47, 12 }, // {.bs, .dw, .ns}
			{ 151, 121, 12 }, // {.bs, .dw, .ns}
			{ 45, 15, 22 }, // {.bs, .dw, .ns}
			{ 54, 24, 39 } // {.bs, .dw, .ns}
		}
	},
	{ /* Version 36 */
		3034, // .data_bytes
		{ 6, 24, 50, 76, 102, 128, 154 }, // .apat
		{ // .ecc
			{ 75, 47, 6 }, // {.bs, .dw, .ns}
			{ 151, 121, 6 }, // {.bs, .dw, .ns}
			{ 45, 15, 2 }, // {.bs, .dw, .ns}
			{ 54, 24, 46 } // {.bs, .dw, .ns}
		}
	},
	{ /* Version 37 */
		3196, // .data_bytes
		{ 6, 28, 54, 80, 106, 132, 158 }, // .apat
		{ // .ecc
			{ 74, 46, 29 }, // {.bs, .dw, .ns}
			{ 152, 122, 17 }, // {.bs, .dw, .ns}
			{ 45, 15, 24 }, // {.bs, .dw, .ns}
			{ 54, 24, 49 } // {.bs, .dw, .ns}
		}
	},
	{ /* Version 38 */
		3362, // .data_bytes
		{ 6, 32, 58, 84, 110, 136, 162 }, // .apat
		{ // .ecc
			{ 74, 46, 13 }, // {.bs, .dw, .ns}
			{ 152, 122, 4 }, // {.bs, .dw, .ns}
			{ 45, 15, 42 }, // {.bs, .dw, .ns}
			{ 54, 24, 48 } // {.bs, .dw, .ns}
		}
	},
	{ /* Version 39 */
		3532, // .data_bytes
		{ 6, 26, 54, 82, 110, 138, 166 }, // .apat
		{ // .ecc
			{ 75, 47, 40 }, // {.bs, .dw, .ns}
			{ 147, 117, 20 }, // {.bs, .dw, .ns}
			{ 45, 15, 10 }, // {.bs, .dw, .ns}
			{ 54, 24, 43 } // {.bs, .dw, .ns}
		}
	},
	{ /* Version 40 */
		3706, // .data_bytes
		{ 6, 30, 58, 86, 114, 142, 170 }, // .apat
		{ // .ecc
			{ 75, 47, 18 }, // {.bs, .dw, .ns}
			{ 148, 118, 19 }, // {.bs, .dw, .ns}
			{ 45, 15, 20 }, // {.bs, .dw, .ns}
			{ 54, 24, 34 } // {.bs, .dw, .ns}
		}
	}
};

// version_db.c - EOF ----------------------------------------------------------

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
	15, // .p
	gf16_log, // .log
	gf16_exp // .exp
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
	255, // .p
	gf256_log, // .log
	gf256_exp // .exp
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

/************************************************************************
 * Decoder algorithm
 */

struct datastream {
	uint8_t		raw[QUIRC_MAX_PAYLOAD];
	int		data_bits;
	int		ptr;

	uint8_t         data[QUIRC_MAX_PAYLOAD];
};

static inline int grid_bit(const struct quirc_code *code, int x, int y)
{
	int p = y * code->size + x;

	return (code->cell_bitmap[p >> 3] >> (p & 7)) & 1;
}

static quirc_decode_error_t read_format(const struct quirc_code *code,
					struct quirc_data *data, int which)
{
	int i;
	uint16_t format = 0;
	uint16_t fdata;
	quirc_decode_error_t err;

	if (which) {
		for (i = 0; i < 7; i++)
			format = uint16_t((format << 1) |
				grid_bit(code, 8, code->size - 1 - i));
		for (i = 0; i < 8; i++)
			format = uint16_t((format << 1) |
				grid_bit(code, code->size - 8 + i, 8));
	} else {
		static const int xs[15] = {
			8, 8, 8, 8, 8, 8, 8, 8, 7, 5, 4, 3, 2, 1, 0
		};
		static const int ys[15] = {
			0, 1, 2, 3, 4, 5, 7, 8, 8, 8, 8, 8, 8, 8, 8
		};

		for (i = 14; i >= 0; i--)
			format = uint16_t((format << 1) | grid_bit(code, xs[i], ys[i]));
	}

	format ^= 0x5412;

	err = correct_format(&format);
	if (err)
		return err;

	fdata = format >> 10;
	data->ecc_level = fdata >> 3;
	data->mask = fdata & 7;

	return QUIRC_SUCCESS;
}

static int mask_bit(int mask, int i, int j)
{
	switch (mask) {
	case 0: return !((i + j) % 2);
	case 1: return !(i % 2);
	case 2: return !(j % 3);
	case 3: return !((i + j) % 3);
	case 4: return !(((i / 2) + (j / 3)) % 2);
	case 5: return !((i * j) % 2 + (i * j) % 3);
	case 6: return !(((i * j) % 2 + (i * j) % 3) % 2);
	case 7: return !(((i * j) % 3 + (i + j) % 2) % 2);
	}

	return 0;
}

static int reserved_cell(int version, int i, int j)
{
	const struct quirc_version_info *ver = &quirc_version_db[version];
	int size = version * 4 + 17;
	int ai = -1, aj = -1, a;

	/* Finder + format: top left */
	if (i < 9 && j < 9)
		return 1;

	/* Finder + format: bottom left */
	if (i + 8 >= size && j < 9)
		return 1;

	/* Finder + format: top right */
	if (i < 9 && j + 8 >= size)
		return 1;

	/* Exclude timing patterns */
	if (i == 6 || j == 6)
		return 1;

	/* Exclude version info, if it exists. Version info sits adjacent to
	 * the top-right and bottom-left finders in three rows, bounded by
	 * the timing pattern.
	 */
	if (version >= 7) {
		if (i < 6 && j + 11 >= size)
			return 1;
		if (i + 11 >= size && j < 6)
			return 1;
	}

	/* Exclude alignment patterns */
	for (a = 0; a < QUIRC_MAX_ALIGNMENT && ver->apat[a]; a++) {
		int p = ver->apat[a];

		if (abs(p - i) < 3)
			ai = a;
		if (abs(p - j) < 3)
			aj = a;
	}

	if (ai >= 0 && aj >= 0) {
		a--;
		if (ai > 0 && ai < a)
			return 1;
		if (aj > 0 && aj < a)
			return 1;
		if (aj == a && ai == a)
			return 1;
	}

	return 0;
}

static void read_bit(const struct quirc_code *code,
		     struct quirc_data *data,
		     struct datastream *ds, int i, int j)
{
	int bitpos = ds->data_bits & 7;
	int bytepos = ds->data_bits >> 3;
	int v = grid_bit(code, j, i);

	if (mask_bit(data->mask, i, j))
		v ^= 1;

	if (v)
		ds->raw[bytepos] |= (0x80 >> bitpos);

	ds->data_bits++;
}

static void read_data(const struct quirc_code *code,
		      struct quirc_data *data,
		      struct datastream *ds)
{
	int y = code->size - 1;
	int x = code->size - 1;
	int dir = -1;

	while (x > 0) {
		if (x == 6)
			x--;

		if (!reserved_cell(data->version, y, x))
			read_bit(code, data, ds, y, x);

		if (!reserved_cell(data->version, y, x - 1))
			read_bit(code, data, ds, y, x - 1);

		y += dir;
		if (y < 0 || y >= code->size) {
			dir = -dir;
			x -= 2;
			y += dir;
		}
	}
}

static quirc_decode_error_t codestream_ecc(struct quirc_data *data,
					   struct datastream *ds)
{
	const struct quirc_version_info *ver =
		&quirc_version_db[data->version];
	const struct quirc_rs_params *sb_ecc = &ver->ecc[data->ecc_level];
	struct quirc_rs_params lb_ecc;
	const int lb_count =
	    (ver->data_bytes - sb_ecc->bs * sb_ecc->ns) / (sb_ecc->bs + 1);
	const int bc = lb_count + sb_ecc->ns;
	const int ecc_offset = sb_ecc->dw * bc + lb_count;
	int dst_offset = 0;
	int i;

	memcpy(&lb_ecc, sb_ecc, sizeof(lb_ecc));
	lb_ecc.dw++;
	lb_ecc.bs++;

	for (i = 0; i < bc; i++) {
		uint8_t *dst = ds->data + dst_offset;
		const struct quirc_rs_params *ecc =
		    (i < sb_ecc->ns) ? sb_ecc : &lb_ecc;
		const int num_ec = ecc->bs - ecc->dw;
		quirc_decode_error_t err;
		int j;

		for (j = 0; j < ecc->dw; j++)
			dst[j] = ds->raw[j * bc + i];
		for (j = 0; j < num_ec; j++)
			dst[ecc->dw + j] = ds->raw[ecc_offset + j * bc + i];

		err = correct_block(dst, ecc);
		if (err)
			return err;

		dst_offset += ecc->dw;
	}

	ds->data_bits = dst_offset * 8;

	return QUIRC_SUCCESS;
}

static inline int bits_remaining(const struct datastream *ds)
{
	return ds->data_bits - ds->ptr;
}

static int take_bits(struct datastream *ds, int len)
{
	int ret = 0;

	while (len && (ds->ptr < ds->data_bits)) {
		uint8_t b = ds->data[ds->ptr >> 3];
		int bitpos = ds->ptr & 7;

		ret <<= 1;
		if ((b << bitpos) & 0x80)
			ret |= 1;

		ds->ptr++;
		len--;
	}

	return ret;
}

static int numeric_tuple(struct quirc_data *data,
			 struct datastream *ds,
			 int bits, int digits)
{
	int tuple;
	int i;

	if (bits_remaining(ds) < bits)
		return -1;

	tuple = take_bits(ds, bits);

	for (i = digits - 1; i >= 0; i--) {
		data->payload[data->payload_len + i] = tuple % 10 + '0';
		tuple /= 10;
	}

	data->payload_len += digits;
	return 0;
}

static quirc_decode_error_t decode_numeric(struct quirc_data *data,
					   struct datastream *ds)
{
	int bits = 14;
	int count;

	if (data->version < 10)
		bits = 10;
	else if (data->version < 27)
		bits = 12;

	count = take_bits(ds, bits);
	if (data->payload_len + count + 1 > QUIRC_MAX_PAYLOAD)
		return QUIRC_ERROR_DATA_OVERFLOW;

	while (count >= 3) {
		if (numeric_tuple(data, ds, 10, 3) < 0)
			return QUIRC_ERROR_DATA_UNDERFLOW;
		count -= 3;
	}

	if (count >= 2) {
		if (numeric_tuple(data, ds, 7, 2) < 0)
			return QUIRC_ERROR_DATA_UNDERFLOW;
		count -= 2;
	}

	if (count) {
		if (numeric_tuple(data, ds, 4, 1) < 0)
			return QUIRC_ERROR_DATA_UNDERFLOW;
		count--;
	}

	return QUIRC_SUCCESS;
}

static int alpha_tuple(struct quirc_data *data,
		       struct datastream *ds,
		       int bits, int digits)
{
	int tuple;
	int i;

	if (bits_remaining(ds) < bits)
		return -1;

	tuple = take_bits(ds, bits);

	for (i = 0; i < digits; i++) {
		static const char *alpha_map =
			"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:";

		data->payload[data->payload_len + digits - i - 1] =
			alpha_map[tuple % 45];
		tuple /= 45;
	}

	data->payload_len += digits;
	return 0;
}

static quirc_decode_error_t decode_alpha(struct quirc_data *data,
					 struct datastream *ds)
{
	int bits = 13;
	int count;

	if (data->version < 10)
		bits = 9;
	else if (data->version < 27)
		bits = 11;

	count = take_bits(ds, bits);
	if (data->payload_len + count + 1 > QUIRC_MAX_PAYLOAD)
		return QUIRC_ERROR_DATA_OVERFLOW;

	while (count >= 2) {
		if (alpha_tuple(data, ds, 11, 2) < 0)
			return QUIRC_ERROR_DATA_UNDERFLOW;
		count -= 2;
	}

	if (count) {
		if (alpha_tuple(data, ds, 6, 1) < 0)
			return QUIRC_ERROR_DATA_UNDERFLOW;
		count--;
	}

	return QUIRC_SUCCESS;
}

static quirc_decode_error_t decode_byte(struct quirc_data *data,
					struct datastream *ds)
{
	int bits = 16;
	int count;
	int i;

	if (data->version < 10)
		bits = 8;

	count = take_bits(ds, bits);
	if (data->payload_len + count + 1 > QUIRC_MAX_PAYLOAD)
		return QUIRC_ERROR_DATA_OVERFLOW;
	if (bits_remaining(ds) < count * 8)
		return QUIRC_ERROR_DATA_UNDERFLOW;

	for (i = 0; i < count; i++)
		data->payload[data->payload_len++] = uint8_t(take_bits(ds, 8));

	return QUIRC_SUCCESS;
}

static quirc_decode_error_t decode_kanji(struct quirc_data *data,
					 struct datastream *ds)
{
	int bits = 12;
	int count;
	int i;

	if (data->version < 10)
		bits = 8;
	else if (data->version < 27)
		bits = 10;

	count = take_bits(ds, bits);
	if (data->payload_len + count * 2 + 1 > QUIRC_MAX_PAYLOAD)
		return QUIRC_ERROR_DATA_OVERFLOW;
	if (bits_remaining(ds) < count * 13)
		return QUIRC_ERROR_DATA_UNDERFLOW;

	for (i = 0; i < count; i++) {
		int d = take_bits(ds, 13);
		int msB = d / 0xc0;
		int lsB = d % 0xc0;
		int intermediate = (msB << 8) | lsB;
		uint16_t sjw;

		if (intermediate + 0x8140 <= 0x9ffc) {
			/* bytes are in the range 0x8140 to 0x9FFC */
			sjw = uint16_t(intermediate + 0x8140);
		} else {
			/* bytes are in the range 0xE040 to 0xEBBF */
			sjw = uint16_t(intermediate + 0xc140);
		}

		data->payload[data->payload_len++] = sjw >> 8;
		data->payload[data->payload_len++] = sjw & 0xff;
	}

	return QUIRC_SUCCESS;
}

static quirc_decode_error_t decode_eci(struct quirc_data *data,
				       struct datastream *ds)
{
	if (bits_remaining(ds) < 8)
		return QUIRC_ERROR_DATA_UNDERFLOW;

	data->eci = take_bits(ds, 8);

	if ((data->eci & 0xc0) == 0x80) {
		if (bits_remaining(ds) < 8)
			return QUIRC_ERROR_DATA_UNDERFLOW;

		data->eci = (data->eci << 8) | take_bits(ds, 8);
	} else if ((data->eci & 0xe0) == 0xc0) {
		if (bits_remaining(ds) < 16)
			return QUIRC_ERROR_DATA_UNDERFLOW;

		data->eci = (data->eci << 16) | take_bits(ds, 16);
	}

	return QUIRC_SUCCESS;
}

static quirc_decode_error_t decode_payload(struct quirc_data *data,
					   struct datastream *ds)
{
	while (bits_remaining(ds) >= 4) {
		quirc_decode_error_t err = QUIRC_SUCCESS;
		int type = take_bits(ds, 4);

		switch (type) {
		case QUIRC_DATA_TYPE_NUMERIC:
			err = decode_numeric(data, ds);
			break;

		case QUIRC_DATA_TYPE_ALPHA:
			err = decode_alpha(data, ds);
			break;

		case QUIRC_DATA_TYPE_BYTE:
			err = decode_byte(data, ds);
			break;

		case QUIRC_DATA_TYPE_KANJI:
			err = decode_kanji(data, ds);
			break;

		case 7:
			err = decode_eci(data, ds);
			break;

		default:
			goto done;
		}

		if (err)
			return err;

		if (!(type & (type - 1)) && (type > data->data_type))
			data->data_type = type;
	}
done:

	/* Add nul terminator to all payloads */
	if (data->payload_len >= int(sizeof(data->payload)))
		data->payload_len--;
	data->payload[data->payload_len] = 0;

	return QUIRC_SUCCESS;
}

quirc_decode_error_t quirc_decode(const struct quirc_code *code,
				  struct quirc_data *data)
{
	quirc_decode_error_t err;
	struct datastream ds;

	if ((code->size - 17) % 4)
		return QUIRC_ERROR_INVALID_GRID_SIZE;

	memset(data, 0, sizeof(*data));
	memset(&ds, 0, sizeof(ds));

	data->version = (code->size - 17) / 4;

	if (data->version < 1 ||
	    data->version > QUIRC_MAX_VERSION)
		return QUIRC_ERROR_INVALID_VERSION;

	/* Read format information -- try both locations */
	err = read_format(code, data, 0);
	if (err)
		err = read_format(code, data, 1);
	if (err)
		return err;

	read_data(code, data, &ds);
	err = codestream_ecc(data, &ds);
	if (err)
		return err;

	err = decode_payload(data, &ds);
	if (err)
		return err;

	return QUIRC_SUCCESS;
}

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

bool QRCodeDecoder::decodeQRCode(const std::vector<uint8_t>& modules, QRCode& code)
{
	// Determine the version of QR code from the total number of modules
	//
	// Given the version of a QR code, the modules per side are computed as
	//
	//     modulesSideCount = 4 * version + 17
	// <=> version = (modulesSideCount - 17) / 4
	//
	// where version must be in the range: [1, 40]
	//
	// Since a QR code is always a square:
	//
	//   modules.size() = modulesSideCount * modulesSideCount
	//   <=> modulesSideCount = sqrt(modules.size())
	//
	// where modulesSideCount must be in the range: [21, 177].

	const size_t modulesSideCount = (unsigned int)(Numeric::sqrt(Scalar(modules.size())) + Scalar(0.5));

	if (modulesSideCount < 21 || modulesSideCount > 177 || (modulesSideCount * modulesSideCount != modules.size()) || ((modulesSideCount - 17) % 4 != 0))
	{
		// Invalid number of modules
		return false;
	}

	const unsigned int version = (unsigned int)(modulesSideCount - 17) / 4;

	if (version == 0u || version > 40u)
	{
		// Invalid version
		return false;
	}

	// Decode the modules

	struct quirc_code quircCode;
	memset(&quircCode, 0, sizeof(quircCode));

	quircCode.size = int(modulesSideCount);

	for (size_t i = 0; i < modules.size(); ++i)
	{
		if (modules[i] != 0u)
		{
			// Explanation of assignment in definition of struct quirc_code above
			quircCode.cell_bitmap[i >> 3] |= uint8_t(1u << (i & 0b0111u));
		}
	}

	struct quirc_data quircData;
	const quirc_decode_error_t quircStatus = quirc_decode(&quircCode, &quircData);

	if (quircStatus != QUIRC_SUCCESS)
	{
		return false;
	}

	if (version != (unsigned int)quircData.version)
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	QRCode::ErrorCorrectionCapacity errorCorrectionCapacity = QRCode::ECC_INVALID;
	switch (quircData.ecc_level)
	{
		case QUIRC_ECC_LEVEL_L:
			errorCorrectionCapacity = QRCode::ECC_07;
			break;

		case QUIRC_ECC_LEVEL_M:
			errorCorrectionCapacity = QRCode::ECC_15;
			break;

		case QUIRC_ECC_LEVEL_Q:
			errorCorrectionCapacity = QRCode::ECC_25;
			break;

		case QUIRC_ECC_LEVEL_H:
			errorCorrectionCapacity = QRCode::ECC_30;
			break;

		default:
			ocean_assert(false && "Never be here");
			return false;
	}

	ocean_assert(errorCorrectionCapacity != QRCode::ECC_INVALID);

	QRCode::EncodingMode encodingMode = QRCode::EM_INVALID_ENCODING_MODE;
	switch (quircData.data_type)
	{
		case QUIRC_DATA_TYPE_NUMERIC:
			encodingMode = QRCode::EM_NUMERIC;
			break;

		case QUIRC_DATA_TYPE_ALPHA:
			encodingMode = QRCode::EM_ALPHANUMERIC;
			break;

		case QUIRC_DATA_TYPE_BYTE:
			encodingMode = QRCode::EM_BYTE;
			break;

		case QUIRC_DATA_TYPE_KANJI:
			encodingMode = QRCode::EM_KANJI;
			break;

		default:
			ocean_assert(false && "Unsupported encoding mode");
			return false;
	}

	ocean_assert(encodingMode != QRCode::EM_INVALID_ENCODING_MODE);

	std::vector<uint8_t> decodedData(quircData.payload, quircData.payload + quircData.payload_len);
	std::vector<uint8_t> finalModules = modules;

	code = QRCode(std::move(decodedData), encodingMode, errorCorrectionCapacity, std::move(finalModules), version);
	ocean_assert(code.isValid());

	return code.isValid();
}

} // namespace QRCodes

} // namespace Detector

} // namespace CV

} // namespace Ocean
