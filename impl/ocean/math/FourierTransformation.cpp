/*
 * Portions Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

/*
 * This file contains derived work from
 *
 * https://github.com/opencv/opencv/blob/7713c84465b1bbfea112d87ad61312b190a1505d/modules/core/include/opencv2/core/base.hpp
 * https://github.com/opencv/opencv/blob/7713c84465b1bbfea112d87ad61312b190a1505d/modules/core/include/opencv2/core/types.hpp
 * https://github.com/opencv/opencv/blob/7713c84465b1bbfea112d87ad61312b190a1505d/modules/core/include/opencv2/core/utility.hpp
 * https://github.com/opencv/opencv/blob/7713c84465b1bbfea112d87ad61312b190a1505d/modules/core/src/dxt.cpp
 *
 * Copyright (C) 2000, Intel Corporation, all rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * https://github.com/opencv/opencv/blob/7713c84465b1bbfea112d87ad61312b190a1505d/LICENSE
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ocean/base/Frame.h"

#include "ocean/math/FourierTransformation.h"
#include "ocean/math/Numeric.h"

#include <cmath>
#include <complex>
#include <map>

namespace
{
// clang-format off

/**
 * Within this namespace we have OpenCV's implementation for the DFT.
 * We mainly replaced the usage of cv::Mat by Ocean"s data structures.
 * However, this is mainly a workaround only.
 * We need a nice and clean DFT implementation realized on our own.
 */

template <typename T>
class Complex
{
	public:

		Complex() :
			re(0),
			im(0)
		{
			// nothing to do here
		}

		Complex(const T& real, const T& imag) :
			re(real),
			im(imag)
		{
			// nothing to do here
		}

		T re;
		T im;
};

typedef union Cv32suf
{
	int i;
	unsigned u;
	float f;
}
Cv32suf;


template<typename _Tp, size_t fixed_size = 1024/sizeof(_Tp)+8> class AutoBuffer
{
public:
	typedef _Tp value_type;

	//! the default constructor
	AutoBuffer();
	//! constructor taking the real buffer size
	AutoBuffer(size_t _size);

	//! the copy constructor
	AutoBuffer(const AutoBuffer<_Tp, fixed_size>& buf);
	//! the assignment operator
	AutoBuffer<_Tp, fixed_size>& operator = (const AutoBuffer<_Tp, fixed_size>& buf);

	//! destructor. calls deallocate()
	~AutoBuffer();

	//! allocates the new buffer of size _size. if the _size is small enough, stack-allocated buffer is used
	void allocate(size_t _size);
	//! deallocates the buffer if it was dynamically allocated
	void deallocate();
	//! resizes the buffer and preserves the content
	void resize(size_t _size);
	//! returns the current buffer size
	size_t size() const;
	//! returns pointer to the real buffer, stack-allocated or head-allocated
	operator _Tp* ();
	//! returns read-only pointer to the real buffer, stack-allocated or head-allocated
	operator const _Tp* () const;

protected:
	//! pointer to the real buffer, can point to buf if the buffer is small enough
	_Tp* ptr;
	//! size of the real buffer
	size_t sz;
	//! pre-allocated buffer. At least 1 element to confirm C++ standard reqirements
	_Tp buf[(fixed_size > 0) ? fixed_size : 1];
};

template<typename _Tp, size_t fixed_size> inline
AutoBuffer<_Tp, fixed_size>::AutoBuffer()
{
	ptr = buf;
	sz = fixed_size;
}

template<typename _Tp, size_t fixed_size> inline
AutoBuffer<_Tp, fixed_size>::AutoBuffer(size_t _size)
{
	ptr = buf;
	sz = fixed_size;
	allocate(_size);
}

template<typename _Tp, size_t fixed_size> inline
AutoBuffer<_Tp, fixed_size>::AutoBuffer(const AutoBuffer<_Tp, fixed_size>& abuf )
{
	ptr = buf;
	sz = fixed_size;
	allocate(abuf.size());
	for( size_t i = 0; i < sz; i++ )
		ptr[i] = abuf.ptr[i];
}

template<typename _Tp, size_t fixed_size> inline AutoBuffer<_Tp, fixed_size>&
AutoBuffer<_Tp, fixed_size>::operator = (const AutoBuffer<_Tp, fixed_size>& abuf)
{
	if( this != &abuf )
	{
		deallocate();
		allocate(abuf.size());
		for( size_t i = 0; i < sz; i++ )
			ptr[i] = abuf.ptr[i];
	}
	return *this;
}

template<typename _Tp, size_t fixed_size> inline
AutoBuffer<_Tp, fixed_size>::~AutoBuffer()
{ deallocate(); }

template<typename _Tp, size_t fixed_size> inline void
AutoBuffer<_Tp, fixed_size>::allocate(size_t _size)
{
	if(_size <= sz)
	{
		sz = _size;
		return;
	}
	deallocate();
	if(_size > fixed_size)
	{
		ptr = new _Tp[_size];
		sz = _size;
	}
}

template<typename _Tp, size_t fixed_size> inline void
AutoBuffer<_Tp, fixed_size>::deallocate()
{
	if( ptr != buf )
	{
		delete[] ptr;
		ptr = buf;
		sz = fixed_size;
	}
}

template<typename _Tp, size_t fixed_size> inline void
AutoBuffer<_Tp, fixed_size>::resize(size_t _size)
{
	if(_size <= sz)
	{
		sz = _size;
		return;
	}
	size_t i, prevsize = sz, minsize = std::min(prevsize, _size);
	_Tp* prevptr = ptr;

	ptr = _size > fixed_size ? new _Tp[_size] : buf;
	sz = _size;

	if( ptr != prevptr )
		for( i = 0; i < minsize; i++ )
			ptr[i] = prevptr[i];
	for( i = prevsize; i < _size; i++ )
		ptr[i] = _Tp();

	if( prevptr != buf )
		delete[] prevptr;
}

template<typename _Tp, size_t fixed_size> inline size_t
AutoBuffer<_Tp, fixed_size>::size() const
{ return sz; }

template<typename _Tp, size_t fixed_size> inline
AutoBuffer<_Tp, fixed_size>::operator _Tp* ()
{ return ptr; }

template<typename _Tp, size_t fixed_size> inline
AutoBuffer<_Tp, fixed_size>::operator const _Tp* () const
{ return ptr; }

typedef Complex<float> Complexf;
typedef Complex<double> Complexd;

enum DftFlags {
	/** performs an inverse 1D or 2D transform instead of the default forward
		transform. */
	DFT_INVERSE		= 1,
	/** scales the result: divide it by the number of array elements. Normally, it is
		combined with DFT_INVERSE. */
	DFT_SCALE		  = 2,
	/** performs a forward or inverse transform of every individual row of the input
		matrix; this flag enables you to transform multiple vectors simultaneously and can be used to
		decrease the overhead (which is sometimes several times larger than the processing itself) to
		perform 3D and higher-dimensional transformations and so forth.*/
	DFT_ROWS			= 4,
	/** performs a forward transformation of 1D or 2D real array; the result,
		though being a complex array, has complex-conjugate symmetry (*CCS*, see the function
		description below for details), and such an array can be packed into a real array of the same
		size as input, which is the fastest option and which is what the function does by default;
		however, you may wish to get a full complex array (for simpler spectrum analysis, and so on) -
		pass the flag to enable the function to produce a full-size complex output array. */
	DFT_COMPLEX_OUTPUT = 16,
	/** performs an inverse transformation of a 1D or 2D complex array; the
		result is normally a complex array of the same size, however, if the input array has
		conjugate-complex symmetry (for example, it is a result of forward transformation with
		DFT_COMPLEX_OUTPUT flag), the output is a real array; while the function itself does not
		check whether the input is symmetrical or not, you can pass the flag and then the function
		will assume the symmetry and produce the real output array (note that when the input is packed
		into a real array and inverse transformation is executed, the function treats the input as a
		packed complex-conjugate symmetrical array, and the output will also be a real array). */
	DFT_REAL_OUTPUT	= 32,
	/** performs an inverse 1D or 2D transform instead of the default forward transform. */
	DCT_INVERSE		= DFT_INVERSE,
	/** performs a forward or inverse transform of every individual row of the input
		matrix. This flag enables you to transform multiple vectors simultaneously and can be used to
		decrease the overhead (which is sometimes several times larger than the processing itself) to
		perform 3D and higher-dimensional transforms and so forth.*/
	DCT_ROWS			= DFT_ROWS
};

/** @anchor core_c_DftFlags
  @name Flags for cvDFT, cvDCT and cvMulSpectrums
  @{
  */
#define CV_DXT_FORWARD  0
#define CV_DXT_INVERSE  1
#define CV_DXT_SCALE	2 /**< divide result by size of array */
#define CV_DXT_INV_SCALE (CV_DXT_INVERSE + CV_DXT_SCALE)
#define CV_DXT_INVERSE_SCALE CV_DXT_INV_SCALE
#define CV_DXT_ROWS	 4 /**< transform each row individually */
#define CV_DXT_MUL_CONJ 8 /**< conjugate the second argument of cvMulSpectrums */
/** @} */

static inline void* alignPtr( const void* ptr, int align = 32 )
{
	assert ( (align & (align-1)) == 0 );
	return (void*)( ((size_t)ptr + align - 1) & ~(size_t)(align-1) );
}

/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//						Intel License Agreement
//				For Open Source Computer Vision Library
//
// Copyright (C) 2000, Intel Corporation, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//	* Redistribution's of source code must retain the above copyright notice,
//	 this list of conditions and the following disclaimer.
//
//	* Redistribution's in binary form must reproduce the above copyright notice,
//	 this list of conditions and the following disclaimer in the documentation
//	 and/or other materials provided with the distribution.
//
//	* The name of Intel Corporation may not be used to endorse or promote products
//	 derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

/****************************************************************************************\
								Discrete Fourier Transform
\****************************************************************************************/

#define CV_MAX_LOCAL_DFT_SIZE  (1 << 15)

static unsigned char bitrevTab[] =
{
  0x00,0x80,0x40,0xc0,0x20,0xa0,0x60,0xe0,0x10,0x90,0x50,0xd0,0x30,0xb0,0x70,0xf0,
  0x08,0x88,0x48,0xc8,0x28,0xa8,0x68,0xe8,0x18,0x98,0x58,0xd8,0x38,0xb8,0x78,0xf8,
  0x04,0x84,0x44,0xc4,0x24,0xa4,0x64,0xe4,0x14,0x94,0x54,0xd4,0x34,0xb4,0x74,0xf4,
  0x0c,0x8c,0x4c,0xcc,0x2c,0xac,0x6c,0xec,0x1c,0x9c,0x5c,0xdc,0x3c,0xbc,0x7c,0xfc,
  0x02,0x82,0x42,0xc2,0x22,0xa2,0x62,0xe2,0x12,0x92,0x52,0xd2,0x32,0xb2,0x72,0xf2,
  0x0a,0x8a,0x4a,0xca,0x2a,0xaa,0x6a,0xea,0x1a,0x9a,0x5a,0xda,0x3a,0xba,0x7a,0xfa,
  0x06,0x86,0x46,0xc6,0x26,0xa6,0x66,0xe6,0x16,0x96,0x56,0xd6,0x36,0xb6,0x76,0xf6,
  0x0e,0x8e,0x4e,0xce,0x2e,0xae,0x6e,0xee,0x1e,0x9e,0x5e,0xde,0x3e,0xbe,0x7e,0xfe,
  0x01,0x81,0x41,0xc1,0x21,0xa1,0x61,0xe1,0x11,0x91,0x51,0xd1,0x31,0xb1,0x71,0xf1,
  0x09,0x89,0x49,0xc9,0x29,0xa9,0x69,0xe9,0x19,0x99,0x59,0xd9,0x39,0xb9,0x79,0xf9,
  0x05,0x85,0x45,0xc5,0x25,0xa5,0x65,0xe5,0x15,0x95,0x55,0xd5,0x35,0xb5,0x75,0xf5,
  0x0d,0x8d,0x4d,0xcd,0x2d,0xad,0x6d,0xed,0x1d,0x9d,0x5d,0xdd,0x3d,0xbd,0x7d,0xfd,
  0x03,0x83,0x43,0xc3,0x23,0xa3,0x63,0xe3,0x13,0x93,0x53,0xd3,0x33,0xb3,0x73,0xf3,
  0x0b,0x8b,0x4b,0xcb,0x2b,0xab,0x6b,0xeb,0x1b,0x9b,0x5b,0xdb,0x3b,0xbb,0x7b,0xfb,
  0x07,0x87,0x47,0xc7,0x27,0xa7,0x67,0xe7,0x17,0x97,0x57,0xd7,0x37,0xb7,0x77,0xf7,
  0x0f,0x8f,0x4f,0xcf,0x2f,0xaf,0x6f,0xef,0x1f,0x9f,0x5f,0xdf,0x3f,0xbf,0x7f,0xff
};

static const double DFTTab[][2] =
{
{ 1.00000000000000000, 0.00000000000000000 },
{-1.00000000000000000, 0.00000000000000000 },
{ 0.00000000000000000, 1.00000000000000000 },
{ 0.70710678118654757, 0.70710678118654746 },
{ 0.92387953251128674, 0.38268343236508978 },
{ 0.98078528040323043, 0.19509032201612825 },
{ 0.99518472667219693, 0.09801714032956060 },
{ 0.99879545620517241, 0.04906767432741802 },
{ 0.99969881869620425, 0.02454122852291229 },
{ 0.99992470183914450, 0.01227153828571993 },
{ 0.99998117528260111, 0.00613588464915448 },
{ 0.99999529380957619, 0.00306795676296598 },
{ 0.99999882345170188, 0.00153398018628477 },
{ 0.99999970586288223, 0.00076699031874270 },
{ 0.99999992646571789, 0.00038349518757140 },
{ 0.99999998161642933, 0.00019174759731070 },
{ 0.99999999540410733, 0.00009587379909598 },
{ 0.99999999885102686, 0.00004793689960307 },
{ 0.99999999971275666, 0.00002396844980842 },
{ 0.99999999992818922, 0.00001198422490507 },
{ 0.99999999998204725, 0.00000599211245264 },
{ 0.99999999999551181, 0.00000299605622633 },
{ 0.99999999999887801, 0.00000149802811317 },
{ 0.99999999999971945, 0.00000074901405658 },
{ 0.99999999999992983, 0.00000037450702829 },
{ 0.99999999999998246, 0.00000018725351415 },
{ 0.99999999999999567, 0.00000009362675707 },
{ 0.99999999999999889, 0.00000004681337854 },
{ 0.99999999999999978, 0.00000002340668927 },
{ 0.99999999999999989, 0.00000001170334463 },
{ 1.00000000000000000, 0.00000000585167232 },
{ 1.00000000000000000, 0.00000000292583616 }
};

#define BitRev(i,shift) \
	((int)((((unsigned)bitrevTab[(i)&255] << 24)+ \
			((unsigned)bitrevTab[((i)>> 8)&255] << 16)+ \
			((unsigned)bitrevTab[((i)>>16)&255] <<  8)+ \
			((unsigned)bitrevTab[((i)>>24)])) >> (shift)))

static int
DFTFactorize( int n, int* factors )
{
	int nf = 0, f, i;

	if( n <= 5 )
	{
		factors[0] = n;
		return 1;
	}

	f = (((n - 1)^n)+1) >> 1;
	if( f > 1 )
	{
		factors[nf++] = f;
		n = f == n ? 1 : n/f;
	}

	for( f = 3; n > 1; )
	{
		int d = n/f;
		if( d*f == n )
		{
			factors[nf++] = f;
			n = d;
		}
		else
		{
			f += 2;
			if( f*f > n )
				break;
		}
	}

	if( n > 1 )
		factors[nf++] = n;

	f = (factors[0] & 1) == 0;
	for( i = f; i < (nf+f)/2; i++ )
		std::swap( factors[i], factors[nf-i-1+f] );

	return nf;
}

static void
DFTInit( int n0, int nf, int* factors, int* itab, int elem_size, void* _wave, int inv_itab )
{
	int digits[34], radix[34];
	int n = factors[0], m = 0;
	int* itab0 = itab;
	int i, j, k;
	Complex<double> w, w1;
	double t;

	if( n0 <= 5 )
	{
		itab[0] = 0;
		itab[n0-1] = n0-1;

		if( n0 != 4 )
		{
			for( i = 1; i < n0-1; i++ )
				itab[i] = i;
		}
		else
		{
			itab[1] = 2;
			itab[2] = 1;
		}
		if( n0 == 5 )
		{
			if( elem_size == sizeof(Complex<double>) )
				((Complex<double>*)_wave)[0] = Complex<double>(1.,0.);
			else
				((Complex<float>*)_wave)[0] = Complex<float>(1.f,0.f);
		}
		if( n0 != 4 )
			return;
		m = 2;
	}
	else
	{
		// radix[] is initialized from index 'nf' down to zero
		assert (nf < 34);
		radix[nf] = 1;
		digits[nf] = 0;
		for( i = 0; i < nf; i++ )
		{
			digits[i] = 0;
			radix[nf-i-1] = radix[nf-i]*factors[nf-i-1];
		}

		if( inv_itab && factors[0] != factors[nf-1] )
			itab = (int*)_wave;

		if( (n & 1) == 0 )
		{
			int a = radix[1], na2 = n*a>>1, na4 = na2 >> 1;
			for( m = 0; (unsigned)(1 << m) < (unsigned)n; m++ )
				;
			if( n <= 2  )
			{
				itab[0] = 0;
				itab[1] = na2;
			}
			else if( n <= 256 )
			{
				int shift = 10 - m;
				for( i = 0; i <= n - 4; i += 4 )
				{
					j = (bitrevTab[i>>2]>>shift)*a;
					itab[i] = j;
					itab[i+1] = j + na2;
					itab[i+2] = j + na4;
					itab[i+3] = j + na2 + na4;
				}
			}
			else
			{
				int shift = 34 - m;
				for( i = 0; i < n; i += 4 )
				{
					int i4 = i >> 2;
					j = BitRev(i4,shift)*a;
					itab[i] = j;
					itab[i+1] = j + na2;
					itab[i+2] = j + na4;
					itab[i+3] = j + na2 + na4;
				}
			}

			digits[1]++;

			if( nf >= 2 )
			{
				for( i = n, j = radix[2]; i < n0; )
				{
					for( k = 0; k < n; k++ )
						itab[i+k] = itab[k] + j;
					if( (i += n) >= n0 )
						break;
					j += radix[2];
					for( k = 1; ++digits[k] >= factors[k]; k++ )
					{
						digits[k] = 0;
						j += radix[k+2] - radix[k];
					}
				}
			}
		}
		else
		{
			for( i = 0, j = 0;; )
			{
				itab[i] = j;
				if( ++i >= n0 )
					break;
				j += radix[1];
				for( k = 0; ++digits[k] >= factors[k]; k++ )
				{
					digits[k] = 0;
					j += radix[k+2] - radix[k];
				}
			}
		}

		if( itab != itab0 )
		{
			itab0[0] = 0;
			for( i = n0 & 1; i < n0; i += 2 )
			{
				int k0 = itab[i];
				int k1 = itab[i+1];
				itab0[k0] = i;
				itab0[k1] = i+1;
			}
		}
	}

	if( (n0 & (n0-1)) == 0 )
	{
		w.re = w1.re = DFTTab[m][0];
		w.im = w1.im = -DFTTab[m][1];
	}
	else
	{
		t = -Ocean::NumericD::pi2()/double(n0);
		w.im = w1.im = sin(t);
		w.re = w1.re = std::sqrt(1. - w1.im*w1.im);
	}
	n = (n0+1)/2;

	if( elem_size == sizeof(Complex<double>) )
	{
		Complex<double>* wave = (Complex<double>*)_wave;

		wave[0].re = 1.;
		wave[0].im = 0.;

		if( (n0 & 1) == 0 )
		{
			wave[n].re = -1.;
			wave[n].im = 0;
		}

		for( i = 1; i < n; i++ )
		{
			wave[i] = w;
			wave[n0-i].re = w.re;
			wave[n0-i].im = -w.im;

			t = w.re*w1.re - w.im*w1.im;
			w.im = w.re*w1.im + w.im*w1.re;
			w.re = t;
		}
	}
	else
	{
		Complex<float>* wave = (Complex<float>*)_wave;
		ocean_assert( elem_size == sizeof(Complex<float>) );

		wave[0].re = 1.f;
		wave[0].im = 0.f;

		if( (n0 & 1) == 0 )
		{
			wave[n].re = -1.f;
			wave[n].im = 0.f;
		}

		for( i = 1; i < n; i++ )
		{
			wave[i].re = (float)w.re;
			wave[i].im = (float)w.im;
			wave[n0-i].re = (float)w.re;
			wave[n0-i].im = (float)-w.im;

			t = w.re*w1.re - w.im*w1.im;
			w.im = w.re*w1.im + w.im*w1.re;
			w.re = t;
		}
	}
}

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 30

template<typename T> struct DFT_VecR4
{
	int operator()(Complex<T>*, int, int, int&, const Complex<T>*) const { return 1; }
};

// optimized radix-4 transform
template<> struct DFT_VecR4<float>
{
	int operator()(Complex<float>* dst, int N, int n0, int& _dw0, const Complex<float>* wave) const
	{
		int n = 1, i, j, nx, dw, dw0 = _dw0;
		__m128 z = _mm_setzero_ps(), x02=z, x13=z, w01=z, w23=z, y01, y23, t0, t1;
		Cv32suf t; t.i = 0x80000000;
		__m128 neg0_mask = _mm_load_ss(&t.f);
		__m128 neg3_mask = _mm_shuffle_ps(neg0_mask, neg0_mask, _MM_SHUFFLE(0,1,2,3));

		for( ; n*4 <= N; )
		{
			nx = n;
			n *= 4;
			dw0 /= 4;

			for( i = 0; i < n0; i += n )
			{
				Complexf *v0, *v1;

				v0 = dst + i;
				v1 = v0 + nx*2;

				x02 = _mm_loadl_pi(x02, (const __m64*)&v0[0]);
				x13 = _mm_loadl_pi(x13, (const __m64*)&v0[nx]);
				x02 = _mm_loadh_pi(x02, (const __m64*)&v1[0]);
				x13 = _mm_loadh_pi(x13, (const __m64*)&v1[nx]);

				y01 = _mm_add_ps(x02, x13);
				y23 = _mm_sub_ps(x02, x13);
				t1 = _mm_xor_ps(_mm_shuffle_ps(y01, y23, _MM_SHUFFLE(2,3,3,2)), neg3_mask);
				t0 = _mm_movelh_ps(y01, y23);
				y01 = _mm_add_ps(t0, t1);
				y23 = _mm_sub_ps(t0, t1);

				_mm_storel_pi((__m64*)&v0[0], y01);
				_mm_storeh_pi((__m64*)&v0[nx], y01);
				_mm_storel_pi((__m64*)&v1[0], y23);
				_mm_storeh_pi((__m64*)&v1[nx], y23);

				for( j = 1, dw = dw0; j < nx; j++, dw += dw0 )
				{
					v0 = dst + i + j;
					v1 = v0 + nx*2;

					x13 = _mm_loadl_pi(x13, (const __m64*)&v0[nx]);
					w23 = _mm_loadl_pi(w23, (const __m64*)&wave[dw*2]);
					x13 = _mm_loadh_pi(x13, (const __m64*)&v1[nx]); // x1, x3 = r1 i1 r3 i3
					w23 = _mm_loadh_pi(w23, (const __m64*)&wave[dw*3]); // w2, w3 = wr2 wi2 wr3 wi3

					t0 = _mm_mul_ps(_mm_moveldup_ps(x13), w23);
					t1 = _mm_mul_ps(_mm_movehdup_ps(x13), _mm_shuffle_ps(w23, w23, _MM_SHUFFLE(2,3,0,1)));
					x13 = _mm_addsub_ps(t0, t1);
					// re(x1*w2), im(x1*w2), re(x3*w3), im(x3*w3)
					x02 = _mm_loadl_pi(x02, (const __m64*)&v1[0]); // x2 = r2 i2
					w01 = _mm_loadl_pi(w01, (const __m64*)&wave[dw]); // w1 = wr1 wi1
					x02 = _mm_shuffle_ps(x02, x02, _MM_SHUFFLE(0,0,1,1));
					w01 = _mm_shuffle_ps(w01, w01, _MM_SHUFFLE(1,0,0,1));
					x02 = _mm_mul_ps(x02, w01);
					x02 = _mm_addsub_ps(x02, _mm_movelh_ps(x02, x02));
					// re(x0) im(x0) re(x2*w1), im(x2*w1)
					x02 = _mm_loadl_pi(x02, (const __m64*)&v0[0]);

					y01 = _mm_add_ps(x02, x13);
					y23 = _mm_sub_ps(x02, x13);
					t1 = _mm_xor_ps(_mm_shuffle_ps(y01, y23, _MM_SHUFFLE(2,3,3,2)), neg3_mask);
					t0 = _mm_movelh_ps(y01, y23);
					y01 = _mm_add_ps(t0, t1);
					y23 = _mm_sub_ps(t0, t1);

					_mm_storel_pi((__m64*)&v0[0], y01);
					_mm_storeh_pi((__m64*)&v0[nx], y01);
					_mm_storel_pi((__m64*)&v1[0], y23);
					_mm_storeh_pi((__m64*)&v1[nx], y23);
				}
			}
		}

		_dw0 = dw0;
		return n;
	}
};

#endif // OCEAN_HARDWARE_SSE_VERSION >= 30



enum { DFT_NO_PERMUTE=256, DFT_COMPLEX_INPUT_OR_OUTPUT=512 };

// mixed-radix complex discrete Fourier transform: double-precision version
template<typename T> static void
DFT( const Complex<T>* src, Complex<T>* dst, int n,
	 int nf, const int* factors, const int* itab,
	 const Complex<T>* wave, int tab_size,
	 const void*, Complex<T>* buf,
	 int flags, double _scale )
{
	static const T sin_120 = (T)0.86602540378443864676372317075294;
	static const T fft5_2 = (T)0.559016994374947424102293417182819;
	static const T fft5_3 = (T)-0.951056516295153572116439333379382;
	static const T fft5_4 = (T)-1.538841768587626701285145288018455;
	static const T fft5_5 = (T)0.363271264002680442947733378740309;

	int n0 = n, f_idx, nx;
	int inv = flags & DFT_INVERSE;
	int dw0 = tab_size, dw;
	int i, j, k;
	Complex<T> t;
	T scale = (T)_scale;
	int tab_step;

	tab_step = tab_size == n ? 1 : tab_size == n*2 ? 2 : tab_size/n;

	// 0. shuffle data
	if( dst != src )
	{
		ocean_assert( (flags & DFT_NO_PERMUTE) == 0 );
		if( !inv )
		{
			for( i = 0; i <= n - 2; i += 2, itab += 2*tab_step )
			{
				int k0 = itab[0], k1 = itab[tab_step];
				ocean_assert( (unsigned)k0 < (unsigned)n && (unsigned)k1 < (unsigned)n );
				dst[i] = src[k0]; dst[i+1] = src[k1];
			}

			if( i < n )
				dst[n-1] = src[n-1];
		}
		else
		{
			for( i = 0; i <= n - 2; i += 2, itab += 2*tab_step )
			{
				int k0 = itab[0], k1 = itab[tab_step];
				ocean_assert( (unsigned)k0 < (unsigned)n && (unsigned)k1 < (unsigned)n );
				t.re = src[k0].re; t.im = -src[k0].im;
				dst[i] = t;
				t.re = src[k1].re; t.im = -src[k1].im;
				dst[i+1] = t;
			}

			if( i < n )
			{
				t.re = src[n-1].re; t.im = -src[n-1].im;
				dst[i] = t;
			}
		}
	}
	else
	{
		if( (flags & DFT_NO_PERMUTE) == 0 )
		{
			ocean_assert( factors[0] == factors[nf-1] );
			if( nf == 1 )
			{
				if( (n & 3) == 0 )
				{
					int n2 = n/2;
					Complex<T>* dsth = dst + n2;

					for( i = 0; i < n2; i += 2, itab += tab_step*2 )
					{
						j = itab[0];
						ocean_assert( (unsigned)j < (unsigned)n2 );

						std::swap(dst[i+1], dsth[j]);
						if( j > i )
						{
							std::swap(dst[i], dst[j]);
							std::swap(dsth[i+1], dsth[j+1]);
						}
					}
				}
				// else do nothing
			}
			else
			{
				for( i = 0; i < n; i++, itab += tab_step )
				{
					j = itab[0];
					ocean_assert( (unsigned)j < (unsigned)n );
					if( j > i )
						std::swap(dst[i], dst[j]);
				}
			}
		}

		if( inv )
		{
			for( i = 0; i <= n - 2; i += 2 )
			{
				T t0 = -dst[i].im;
				T t1 = -dst[i+1].im;
				dst[i].im = t0; dst[i+1].im = t1;
			}

			if( i < n )
				dst[n-1].im = -dst[n-1].im;
		}
	}

	n = 1;
	// 1. power-2 transforms
	if( (factors[0] & 1) == 0 )
	{
#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 30
		if( factors[0] >= 4)
		{
			DFT_VecR4<T> vr4;
			n = vr4(dst, factors[0], n0, dw0, wave);
		}
#endif

		// radix-4 transform
		for( ; n*4 <= factors[0]; )
		{
			nx = n;
			n *= 4;
			dw0 /= 4;

			for( i = 0; i < n0; i += n )
			{
				Complex<T> *v0, *v1;
				T r0, i0, r1, i1, r2, i2, r3, i3, r4, i4;

				v0 = dst + i;
				v1 = v0 + nx*2;

				r0 = v1[0].re; i0 = v1[0].im;
				r4 = v1[nx].re; i4 = v1[nx].im;

				r1 = r0 + r4; i1 = i0 + i4;
				r3 = i0 - i4; i3 = r4 - r0;

				r2 = v0[0].re; i2 = v0[0].im;
				r4 = v0[nx].re; i4 = v0[nx].im;

				r0 = r2 + r4; i0 = i2 + i4;
				r2 -= r4; i2 -= i4;

				v0[0].re = r0 + r1; v0[0].im = i0 + i1;
				v1[0].re = r0 - r1; v1[0].im = i0 - i1;
				v0[nx].re = r2 + r3; v0[nx].im = i2 + i3;
				v1[nx].re = r2 - r3; v1[nx].im = i2 - i3;

				for( j = 1, dw = dw0; j < nx; j++, dw += dw0 )
				{
					v0 = dst + i + j;
					v1 = v0 + nx*2;

					r2 = v0[nx].re*wave[dw*2].re - v0[nx].im*wave[dw*2].im;
					i2 = v0[nx].re*wave[dw*2].im + v0[nx].im*wave[dw*2].re;
					r0 = v1[0].re*wave[dw].im + v1[0].im*wave[dw].re;
					i0 = v1[0].re*wave[dw].re - v1[0].im*wave[dw].im;
					r3 = v1[nx].re*wave[dw*3].im + v1[nx].im*wave[dw*3].re;
					i3 = v1[nx].re*wave[dw*3].re - v1[nx].im*wave[dw*3].im;

					r1 = i0 + i3; i1 = r0 + r3;
					r3 = r0 - r3; i3 = i3 - i0;
					r4 = v0[0].re; i4 = v0[0].im;

					r0 = r4 + r2; i0 = i4 + i2;
					r2 = r4 - r2; i2 = i4 - i2;

					v0[0].re = r0 + r1; v0[0].im = i0 + i1;
					v1[0].re = r0 - r1; v1[0].im = i0 - i1;
					v0[nx].re = r2 + r3; v0[nx].im = i2 + i3;
					v1[nx].re = r2 - r3; v1[nx].im = i2 - i3;
				}
			}
		}

		for( ; n < factors[0]; )
		{
			// do the remaining radix-2 transform
			nx = n;
			n *= 2;
			dw0 /= 2;

			for( i = 0; i < n0; i += n )
			{
				Complex<T>* v = dst + i;
				T r0 = v[0].re + v[nx].re;
				T i0 = v[0].im + v[nx].im;
				T r1 = v[0].re - v[nx].re;
				T i1 = v[0].im - v[nx].im;
				v[0].re = r0; v[0].im = i0;
				v[nx].re = r1; v[nx].im = i1;

				for( j = 1, dw = dw0; j < nx; j++, dw += dw0 )
				{
					v = dst + i + j;
					r1 = v[nx].re*wave[dw].re - v[nx].im*wave[dw].im;
					i1 = v[nx].im*wave[dw].re + v[nx].re*wave[dw].im;
					r0 = v[0].re; i0 = v[0].im;

					v[0].re = r0 + r1; v[0].im = i0 + i1;
					v[nx].re = r0 - r1; v[nx].im = i0 - i1;
				}
			}
		}
	}

	// 2. all the other transforms
	for( f_idx = (factors[0]&1) ? 0 : 1; f_idx < nf; f_idx++ )
	{
		int factor = factors[f_idx];
		nx = n;
		n *= factor;
		dw0 /= factor;

		if( factor == 3 )
		{
			// radix-3
			for( i = 0; i < n0; i += n )
			{
				Complex<T>* v = dst + i;

				T r1 = v[nx].re + v[nx*2].re;
				T i1 = v[nx].im + v[nx*2].im;
				T r0 = v[0].re;
				T i0 = v[0].im;
				T r2 = sin_120*(v[nx].im - v[nx*2].im);
				T i2 = sin_120*(v[nx*2].re - v[nx].re);
				v[0].re = r0 + r1; v[0].im = i0 + i1;
				r0 -= (T)0.5*r1; i0 -= (T)0.5*i1;
				v[nx].re = r0 + r2; v[nx].im = i0 + i2;
				v[nx*2].re = r0 - r2; v[nx*2].im = i0 - i2;

				for( j = 1, dw = dw0; j < nx; j++, dw += dw0 )
				{
					v = dst + i + j;
					r0 = v[nx].re*wave[dw].re - v[nx].im*wave[dw].im;
					i0 = v[nx].re*wave[dw].im + v[nx].im*wave[dw].re;
					i2 = v[nx*2].re*wave[dw*2].re - v[nx*2].im*wave[dw*2].im;
					r2 = v[nx*2].re*wave[dw*2].im + v[nx*2].im*wave[dw*2].re;
					r1 = r0 + i2; i1 = i0 + r2;

					r2 = sin_120*(i0 - r2); i2 = sin_120*(i2 - r0);
					r0 = v[0].re; i0 = v[0].im;
					v[0].re = r0 + r1; v[0].im = i0 + i1;
					r0 -= (T)0.5*r1; i0 -= (T)0.5*i1;
					v[nx].re = r0 + r2; v[nx].im = i0 + i2;
					v[nx*2].re = r0 - r2; v[nx*2].im = i0 - i2;
				}
			}
		}
		else if( factor == 5 )
		{
			// radix-5
			for( i = 0; i < n0; i += n )
			{
				for( j = 0, dw = 0; j < nx; j++, dw += dw0 )
				{
					Complex<T>* v0 = dst + i + j;
					Complex<T>* v1 = v0 + nx*2;
					Complex<T>* v2 = v1 + nx*2;

					T r0, i0, r1, i1, r2, i2, r3, i3, r4, i4, r5, i5;

					r3 = v0[nx].re*wave[dw].re - v0[nx].im*wave[dw].im;
					i3 = v0[nx].re*wave[dw].im + v0[nx].im*wave[dw].re;
					r2 = v2[0].re*wave[dw*4].re - v2[0].im*wave[dw*4].im;
					i2 = v2[0].re*wave[dw*4].im + v2[0].im*wave[dw*4].re;

					r1 = r3 + r2; i1 = i3 + i2;
					r3 -= r2; i3 -= i2;

					r4 = v1[nx].re*wave[dw*3].re - v1[nx].im*wave[dw*3].im;
					i4 = v1[nx].re*wave[dw*3].im + v1[nx].im*wave[dw*3].re;
					r0 = v1[0].re*wave[dw*2].re - v1[0].im*wave[dw*2].im;
					i0 = v1[0].re*wave[dw*2].im + v1[0].im*wave[dw*2].re;

					r2 = r4 + r0; i2 = i4 + i0;
					r4 -= r0; i4 -= i0;

					r0 = v0[0].re; i0 = v0[0].im;
					r5 = r1 + r2; i5 = i1 + i2;

					v0[0].re = r0 + r5; v0[0].im = i0 + i5;

					r0 -= (T)0.25*r5; i0 -= (T)0.25*i5;
					r1 = fft5_2*(r1 - r2); i1 = fft5_2*(i1 - i2);
					r2 = -fft5_3*(i3 + i4); i2 = fft5_3*(r3 + r4);

					i3 *= -fft5_5; r3 *= fft5_5;
					i4 *= -fft5_4; r4 *= fft5_4;

					r5 = r2 + i3; i5 = i2 + r3;
					r2 -= i4; i2 -= r4;

					r3 = r0 + r1; i3 = i0 + i1;
					r0 -= r1; i0 -= i1;

					v0[nx].re = r3 + r2; v0[nx].im = i3 + i2;
					v2[0].re = r3 - r2; v2[0].im = i3 - i2;

					v1[0].re = r0 + r5; v1[0].im = i0 + i5;
					v1[nx].re = r0 - r5; v1[nx].im = i0 - i5;
				}
			}
		}
		else
		{
			// radix-"factor" - an odd number
			int p, q, factor2 = (factor - 1)/2;
			int d, dd, dw_f = tab_size/factor;
			Complex<T>* a = buf;
			Complex<T>* b = buf + factor2;

			for( i = 0; i < n0; i += n )
			{
				for( j = 0, dw = 0; j < nx; j++, dw += dw0 )
				{
					Complex<T>* v = dst + i + j;
					Complex<T> v_0 = v[0];
					Complex<T> vn_0 = v_0;

					if( j == 0 )
					{
						for( p = 1, k = nx; p <= factor2; p++, k += nx )
						{
							T r0 = v[k].re + v[n-k].re;
							T i0 = v[k].im - v[n-k].im;
							T r1 = v[k].re - v[n-k].re;
							T i1 = v[k].im + v[n-k].im;

							vn_0.re += r0; vn_0.im += i1;
							a[p-1].re = r0; a[p-1].im = i0;
							b[p-1].re = r1; b[p-1].im = i1;
						}
					}
					else
					{
						const Complex<T>* wave_ = wave + dw*factor;
						d = dw;

						for( p = 1, k = nx; p <= factor2; p++, k += nx, d += dw )
						{
							T r2 = v[k].re*wave[d].re - v[k].im*wave[d].im;
							T i2 = v[k].re*wave[d].im + v[k].im*wave[d].re;

							T r1 = v[n-k].re*wave_[-d].re - v[n-k].im*wave_[-d].im;
							T i1 = v[n-k].re*wave_[-d].im + v[n-k].im*wave_[-d].re;

							T r0 = r2 + r1;
							T i0 = i2 - i1;
							r1 = r2 - r1;
							i1 = i2 + i1;

							vn_0.re += r0; vn_0.im += i1;
							a[p-1].re = r0; a[p-1].im = i0;
							b[p-1].re = r1; b[p-1].im = i1;
						}
					}

					v[0] = vn_0;

					for( p = 1, k = nx; p <= factor2; p++, k += nx )
					{
						Complex<T> s0 = v_0, s1 = v_0;
						d = dd = dw_f*p;

						for( q = 0; q < factor2; q++ )
						{
							T r0 = wave[d].re * a[q].re;
							T i0 = wave[d].im * a[q].im;
							T r1 = wave[d].re * b[q].im;
							T i1 = wave[d].im * b[q].re;

							s1.re += r0 + i0; s0.re += r0 - i0;
							s1.im += r1 - i1; s0.im += r1 + i1;

							d += dd;
							d -= -(d >= tab_size) & tab_size;
						}

						v[k] = s0;
						v[n-k] = s1;
					}
				}
			}
		}
	}

	if( scale != 1 )
	{
		T re_scale = scale, im_scale = scale;
		if( inv )
			im_scale = -im_scale;

		for( i = 0; i < n0; i++ )
		{
			T t0 = dst[i].re*re_scale;
			T t1 = dst[i].im*im_scale;
			dst[i].re = t0;
			dst[i].im = t1;
		}
	}
	else if( inv )
	{
		for( i = 0; i <= n0 - 2; i += 2 )
		{
			T t0 = -dst[i].im;
			T t1 = -dst[i+1].im;
			dst[i].im = t0;
			dst[i+1].im = t1;
		}

		if( i < n0 )
			dst[n0-1].im = -dst[n0-1].im;
	}
}


/* FFT of real vector
	output vector format:
	 re(0), re(1), im(1), ... , re(n/2-1), im((n+1)/2-1) [, re((n+1)/2)] OR ...
	 re(0), 0, re(1), im(1), ..., re(n/2-1), im((n+1)/2-1) [, re((n+1)/2), 0] */
template<typename T> static void
RealDFT( const T* src, T* dst, int n, int nf, int* factors, const int* itab,
		 const Complex<T>* wave, int tab_size, const void*,
		 Complex<T>* buf, int flags, double _scale )
{
	int complex_output = (flags & DFT_COMPLEX_INPUT_OR_OUTPUT) != 0;
	T scale = (T)_scale;
	int j, n2 = n >> 1;
	dst += complex_output;

	ocean_assert( tab_size == n );

	if( n == 1 )
	{
		dst[0] = src[0]*scale;
	}
	else if( n == 2 )
	{
		T t = (src[0] + src[1])*scale;
		dst[1] = (src[0] - src[1])*scale;
		dst[0] = t;
	}
	else if( n & 1 )
	{
		dst -= complex_output;
		Complex<T>* _dst = (Complex<T>*)dst;
		_dst[0].re = src[0]*scale;
		_dst[0].im = 0;
		for( j = 1; j < n; j += 2 )
		{
			T t0 = src[itab[j]]*scale;
			T t1 = src[itab[j+1]]*scale;
			_dst[j].re = t0;
			_dst[j].im = 0;
			_dst[j+1].re = t1;
			_dst[j+1].im = 0;
		}
		DFT( _dst, _dst, n, nf, factors, itab, wave,
			 tab_size, 0, buf, DFT_NO_PERMUTE, 1 );
		if( !complex_output )
			dst[1] = dst[0];
	}
	else
	{
		T t0, t;
		T h1_re, h1_im, h2_re, h2_im;
		T scale2 = scale*(T)0.5;
		factors[0] >>= 1;

		DFT( (Complex<T>*)src, (Complex<T>*)dst, n2, nf - (factors[0] == 1),
			 factors + (factors[0] == 1),
			 itab, wave, tab_size, 0, buf, 0, 1 );
		factors[0] <<= 1;

		t = dst[0] - dst[1];
		dst[0] = (dst[0] + dst[1])*scale;
		dst[1] = t*scale;

		t0 = dst[n2];
		t = dst[n-1];
		dst[n-1] = dst[1];

		for( j = 2, wave++; j < n2; j += 2, wave++ )
		{
			/* calc odd */
			h2_re = scale2*(dst[j+1] + t);
			h2_im = scale2*(dst[n-j] - dst[j]);

			/* calc even */
			h1_re = scale2*(dst[j] + dst[n-j]);
			h1_im = scale2*(dst[j+1] - t);

			/* rotate */
			t = h2_re*wave->re - h2_im*wave->im;
			h2_im = h2_re*wave->im + h2_im*wave->re;
			h2_re = t;
			t = dst[n-j-1];

			dst[j-1] = h1_re + h2_re;
			dst[n-j-1] = h1_re - h2_re;
			dst[j] = h1_im + h2_im;
			dst[n-j] = h2_im - h1_im;
		}

		if( j <= n2 )
		{
			dst[n2-1] = t0*scale;
			dst[n2] = -t*scale;
		}
	}

	if( complex_output && ((n & 1) == 0 || n == 1))
	{
		dst[-1] = dst[0];
		dst[0] = 0;
		if( n > 1 )
			dst[n] = 0;
	}
}

/* Inverse FFT of complex conjugate-symmetric vector
	input vector format:
	  re[0], re[1], im[1], ... , re[n/2-1], im[n/2-1], re[n/2] OR
	  re(0), 0, re(1), im(1), ..., re(n/2-1), im((n+1)/2-1) [, re((n+1)/2), 0] */
template<typename T> static void
CCSIDFT( const T* src, T* dst, int n, int nf, int* factors, const int* itab,
		 const Complex<T>* wave, int tab_size,
		 const void*, Complex<T>* buf,
		 int flags, double _scale )
{
	int complex_input = (flags & DFT_COMPLEX_INPUT_OR_OUTPUT) != 0;
	int j, k, n2 = (n+1) >> 1;
	T scale = (T)_scale;
	T save_s1 = 0.;
	T t0, t1, t2, t3, t;

	ocean_assert( tab_size == n );

	if( complex_input )
	{
		ocean_assert( src != dst );
		save_s1 = src[1];
		((T*)src)[1] = src[0];
		src++;
	}

	if( n == 1 )
	{
		dst[0] = (T)(src[0]*scale);
	}
	else if( n == 2 )
	{
		t = (src[0] + src[1])*scale;
		dst[1] = (src[0] - src[1])*scale;
		dst[0] = t;
	}
	else if( n & 1 )
	{
		Complex<T>* _src = (Complex<T>*)(src-1);
		Complex<T>* _dst = (Complex<T>*)dst;

		_dst[0].re = src[0];
		_dst[0].im = 0;
		for( j = 1; j < n2; j++ )
		{
			int k0 = itab[j], k1 = itab[n-j];
			t0 = _src[j].re; t1 = _src[j].im;
			_dst[k0].re = t0; _dst[k0].im = -t1;
			_dst[k1].re = t0; _dst[k1].im = t1;
		}

		DFT( _dst, _dst, n, nf, factors, itab, wave,
			 tab_size, 0, buf, DFT_NO_PERMUTE, 1. );
		dst[0] *= scale;
		for( j = 1; j < n; j += 2 )
		{
			t0 = dst[j*2]*scale;
			t1 = dst[j*2+2]*scale;
			dst[j] = t0;
			dst[j+1] = t1;
		}
	}
	else
	{
		int inplace = src == dst;
		const Complex<T>* w = wave;

		t = src[1];
		t0 = (src[0] + src[n-1]);
		t1 = (src[n-1] - src[0]);
		dst[0] = t0;
		dst[1] = t1;

		for( j = 2, w++; j < n2; j += 2, w++ )
		{
			T h1_re, h1_im, h2_re, h2_im;

			h1_re = (t + src[n-j-1]);
			h1_im = (src[j] - src[n-j]);

			h2_re = (t - src[n-j-1]);
			h2_im = (src[j] + src[n-j]);

			t = h2_re*w->re + h2_im*w->im;
			h2_im = h2_im*w->re - h2_re*w->im;
			h2_re = t;

			t = src[j+1];
			t0 = h1_re - h2_im;
			t1 = -h1_im - h2_re;
			t2 = h1_re + h2_im;
			t3 = h1_im - h2_re;

			if( inplace )
			{
				dst[j] = t0;
				dst[j+1] = t1;
				dst[n-j] = t2;
				dst[n-j+1]= t3;
			}
			else
			{
				int j2 = j >> 1;
				k = itab[j2];
				dst[k] = t0;
				dst[k+1] = t1;
				k = itab[n2-j2];
				dst[k] = t2;
				dst[k+1]= t3;
			}
		}

		if( j <= n2 )
		{
			t0 = t*2;
			t1 = src[n2]*2;

			if( inplace )
			{
				dst[n2] = t0;
				dst[n2+1] = t1;
			}
			else
			{
				k = itab[n2];
				dst[k*2] = t0;
				dst[k*2+1] = t1;
			}
		}

		factors[0] >>= 1;
		DFT( (Complex<T>*)dst, (Complex<T>*)dst, n2,
			 nf - (factors[0] == 1),
			 factors + (factors[0] == 1), itab,
			 wave, tab_size, 0, buf,
			 inplace ? 0 : DFT_NO_PERMUTE, 1. );
		factors[0] <<= 1;

		for( j = 0; j < n; j += 2 )
		{
			t0 = dst[j]*scale;
			t1 = dst[j+1]*(-scale);
			dst[j] = t0;
			dst[j+1] = t1;
		}
	}
	if( complex_input )
		((T*)src)[0] = (T)save_s1;
}

static void
CopyColumn( const unsigned char* _src, size_t src_step,
			unsigned char* _dst, size_t dst_step,
			int len, size_t elem_size )
{
	int i, t0, t1;
	const int* src = (const int*)_src;
	int* dst = (int*)_dst;
	src_step /= sizeof(src[0]);
	dst_step /= sizeof(dst[0]);

	if( elem_size == sizeof(int) )
	{
		for( i = 0; i < len; i++, src += src_step, dst += dst_step )
			dst[0] = src[0];
	}
	else if( elem_size == sizeof(int)*2 )
	{
		for( i = 0; i < len; i++, src += src_step, dst += dst_step )
		{
			t0 = src[0]; t1 = src[1];
			dst[0] = t0; dst[1] = t1;
		}
	}
	else if( elem_size == sizeof(int)*4 )
	{
		for( i = 0; i < len; i++, src += src_step, dst += dst_step )
		{
			t0 = src[0]; t1 = src[1];
			dst[0] = t0; dst[1] = t1;
			t0 = src[2]; t1 = src[3];
			dst[2] = t0; dst[3] = t1;
		}
	}
}


static void
CopyFrom2Columns( const unsigned char* _src, size_t src_step,
				  unsigned char* _dst0, unsigned char* _dst1,
				  int len, size_t elem_size )
{
	int i, t0, t1;
	const int* src = (const int*)_src;
	int* dst0 = (int*)_dst0;
	int* dst1 = (int*)_dst1;
	src_step /= sizeof(src[0]);

	if( elem_size == sizeof(int) )
	{
		for( i = 0; i < len; i++, src += src_step )
		{
			t0 = src[0]; t1 = src[1];
			dst0[i] = t0; dst1[i] = t1;
		}
	}
	else if( elem_size == sizeof(int)*2 )
	{
		for( i = 0; i < len*2; i += 2, src += src_step )
		{
			t0 = src[0]; t1 = src[1];
			dst0[i] = t0; dst0[i+1] = t1;
			t0 = src[2]; t1 = src[3];
			dst1[i] = t0; dst1[i+1] = t1;
		}
	}
	else if( elem_size == sizeof(int)*4 )
	{
		for( i = 0; i < len*4; i += 4, src += src_step )
		{
			t0 = src[0]; t1 = src[1];
			dst0[i] = t0; dst0[i+1] = t1;
			t0 = src[2]; t1 = src[3];
			dst0[i+2] = t0; dst0[i+3] = t1;
			t0 = src[4]; t1 = src[5];
			dst1[i] = t0; dst1[i+1] = t1;
			t0 = src[6]; t1 = src[7];
			dst1[i+2] = t0; dst1[i+3] = t1;
		}
	}
}


static void
CopyTo2Columns( const unsigned char* _src0, const unsigned char* _src1,
				unsigned char* _dst, size_t dst_step,
				int len, size_t elem_size )
{
	int i, t0, t1;
	const int* src0 = (const int*)_src0;
	const int* src1 = (const int*)_src1;
	int* dst = (int*)_dst;
	dst_step /= sizeof(dst[0]);

	if( elem_size == sizeof(int) )
	{
		for( i = 0; i < len; i++, dst += dst_step )
		{
			t0 = src0[i]; t1 = src1[i];
			dst[0] = t0; dst[1] = t1;
		}
	}
	else if( elem_size == sizeof(int)*2 )
	{
		for( i = 0; i < len*2; i += 2, dst += dst_step )
		{
			t0 = src0[i]; t1 = src0[i+1];
			dst[0] = t0; dst[1] = t1;
			t0 = src1[i]; t1 = src1[i+1];
			dst[2] = t0; dst[3] = t1;
		}
	}
	else if( elem_size == sizeof(int)*4 )
	{
		for( i = 0; i < len*4; i += 4, dst += dst_step )
		{
			t0 = src0[i]; t1 = src0[i+1];
			dst[0] = t0; dst[1] = t1;
			t0 = src0[i+2]; t1 = src0[i+3];
			dst[2] = t0; dst[3] = t1;
			t0 = src1[i]; t1 = src1[i+1];
			dst[4] = t0; dst[5] = t1;
			t0 = src1[i+2]; t1 = src1[i+3];
			dst[6] = t0; dst[7] = t1;
		}
	}
}


static void
ExpandCCS( unsigned char* _ptr, int n, int elem_size )
{
	int i;
	if( elem_size == (int)sizeof(float) )
	{
		float* p = (float*)_ptr;
		for( i = 1; i < (n+1)/2; i++ )
		{
			p[(n-i)*2] = p[i*2-1];
			p[(n-i)*2+1] = -p[i*2];
		}
		if( (n & 1) == 0 )
		{
			p[n] = p[n-1];
			p[n+1] = 0.f;
			n--;
		}
		for( i = n-1; i > 0; i-- )
			p[i+1] = p[i];
		p[1] = 0.f;
	}
	else
	{
		double* p = (double*)_ptr;
		for( i = 1; i < (n+1)/2; i++ )
		{
			p[(n-i)*2] = p[i*2-1];
			p[(n-i)*2+1] = -p[i*2];
		}
		if( (n & 1) == 0 )
		{
			p[n] = p[n-1];
			p[n+1] = 0.f;
			n--;
		}
		for( i = n-1; i > 0; i-- )
			p[i+1] = p[i];
		p[1] = 0.f;
	}
}


typedef void (*DFTFunc)(
	 const void* src, void* dst, int n, int nf, int* factors,
	 const int* itab, const void* wave, int tab_size,
	 const void* spec, void* buf, int inv, double scale );

static void DFT_32f( const Complexf* src, Complexf* dst, int n,
	int nf, const int* factors, const int* itab,
	const Complexf* wave, int tab_size,
	const void* spec, Complexf* buf,
	int flags, double scale )
{
	DFT(src, dst, n, nf, factors, itab, wave, tab_size, spec, buf, flags, scale);
}

static void DFT_64f( const Complexd* src, Complexd* dst, int n,
	int nf, const int* factors, const int* itab,
	const Complexd* wave, int tab_size,
	const void* spec, Complexd* buf,
	int flags, double scale )
{
	DFT(src, dst, n, nf, factors, itab, wave, tab_size, spec, buf, flags, scale);
}

static void RealDFT_32f( const float* src, float* dst, int n, int nf, int* factors,
		const int* itab,  const Complexf* wave, int tab_size, const void* spec,
		Complexf* buf, int flags, double scale )
{
	RealDFT( src, dst, n, nf, factors, itab, wave, tab_size, spec, buf, flags, scale);
}

static void RealDFT_64f( const double* src, double* dst, int n, int nf, int* factors,
		const int* itab,  const Complexd* wave, int tab_size, const void* spec,
		Complexd* buf, int flags, double scale )
{
	RealDFT( src, dst, n, nf, factors, itab, wave, tab_size, spec, buf, flags, scale);
}

static void CCSIDFT_32f( const float* src, float* dst, int n, int nf, int* factors,
						 const int* itab,  const Complexf* wave, int tab_size, const void* spec,
						 Complexf* buf, int flags, double scale )
{
	CCSIDFT( src, dst, n, nf, factors, itab, wave, tab_size, spec, buf, flags, scale);
}

static void CCSIDFT_64f( const double* src, double* dst, int n, int nf, int* factors,
						 const int* itab,  const Complexd* wave, int tab_size, const void* spec,
						 Complexd* buf, int flags, double scale )
{
	CCSIDFT( src, dst, n, nf, factors, itab, wave, tab_size, spec, buf, flags, scale);
}

static void complementComplexOutput(void* dst, const unsigned int width, const unsigned int height, const unsigned int channels, Ocean::FrameType::DataType dataType, int len, int dft_dims, const unsigned dstPaddingElements = 0u)
{
	ocean_assert(dst != nullptr);
	ocean_assert_and_suppress_unused(width != 0u && height != 0u, height);
	ocean_assert(channels != 0u);
	ocean_assert(dataType == Ocean::FrameType::DT_SIGNED_FLOAT_32 || dataType == Ocean::FrameType::DT_SIGNED_FLOAT_64);

	int i, n = (int)width;

	if( dataType == Ocean::FrameType::DT_SIGNED_FLOAT_32 )
	{
		float* p0 = (float*)dst;
		size_t dstep = width * channels + dstPaddingElements;
		for( i = 0; i < len; i++ )
		{
			float* p = p0 + dstep*i;
			float* q = dft_dims == 1 || i == 0 || i*2 == len ? p : p0 + dstep*(len-i);

			for( int j = 1; j < (n+1)/2; j++ )
			{
				p[(n-j)*2] = q[j*2];
				p[(n-j)*2+1] = -q[j*2+1];
			}
		}
	}
	else
	{
		double* p0 = (double*)dst;
		size_t dstep = width * channels + dstPaddingElements;
		for( i = 0; i < len; i++ )
		{
			double* p = p0 + dstep*i;
			double* q = dft_dims == 1 || i == 0 || i*2 == len ? p : p0 + dstep*(len-i);

			for( int j = 1; j < (n+1)/2; j++ )
			{
				p[(n-j)*2] = q[j*2];
				p[(n-j)*2+1] = -q[j*2+1];
			}
		}
	}
}

static bool dft(const void* source, const unsigned int width, const unsigned int height, const unsigned int sourceChannels, void* target, const unsigned int targetChannels, const Ocean::FrameType::DataType dataType, int flags, int nonzero_rows, const unsigned int sourcePaddingElements = 0u, const unsigned int targetPaddingElements = 0u)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(sourceChannels != 0u);
	ocean_assert(targetChannels != 0u);

	static DFTFunc dft_tbl[6] =
	{
		(DFTFunc)DFT_32f,
		(DFTFunc)RealDFT_32f,
		(DFTFunc)CCSIDFT_32f,
		(DFTFunc)DFT_64f,
		(DFTFunc)RealDFT_64f,
		(DFTFunc)CCSIDFT_64f
	};
	AutoBuffer<unsigned char> buf;

	int prev_len = 0, stage = 0;
	bool inv = (flags & DFT_INVERSE) != 0;
	int nf = 0, real_transform = sourceChannels == 1 || (inv && (flags & DFT_REAL_OUTPUT) != 0);

	if (dataType != Ocean::FrameType::DT_SIGNED_FLOAT_32 && dataType != Ocean::FrameType::DT_SIGNED_FLOAT_64)
	{
		ocean_assert(false && "Invalid input data!");
		return  false;
	}

	if (sourceChannels != 1u && sourceChannels != 2u)
	{
		ocean_assert(false && "Invalid input data!");
		return  false;
	}

	static_assert(sizeof(float) == 4 && sizeof(double) == 8, "Unexpected size of float or double");
	int elem_size = dataType == Ocean::FrameType::DT_SIGNED_FLOAT_32 ? sizeof(float) : sizeof(double);
	int complex_elem_size = elem_size * 2;
	int factors[34];
	bool inplace_transform = false;

	if( !inv && sourceChannels == 1 && (flags & DFT_COMPLEX_OUTPUT) && targetChannels != 2u)
	{
		ocean_assert(false && "Invalid output data!");
		return false;
	}
	else if( inv && sourceChannels == 2 && (flags & DFT_REAL_OUTPUT) && targetChannels != 1u)
	{
		ocean_assert(false && "Invalid output data!");
		return false;
	}

	if( !real_transform )
		elem_size = complex_elem_size;

	if( width == 1 && nonzero_rows > 0 )
	{
		ocean_assert(false && "This mode (using nonzero_rows with a single-column matrix) breaks the function's logic, so it is prohibited.");
		// "For fast convolution/correlation use 2-column matrix or single-row matrix instead" );
		return false;
	}

	// determine, which transform to do first - row-wise
	// (stage 0) or column-wise (stage 1) transform
	if( !(flags & DFT_ROWS) && height > 1 &&
		(width > 1 && inv && real_transform) )
		stage = 1;

	const unsigned int dataTypeBytes = dataType == Ocean::FrameType::DT_SIGNED_FLOAT_32 ? sizeof(float) : sizeof(double);
	const unsigned int sourceStrideBytes = (width * sourceChannels + sourcePaddingElements) * dataTypeBytes;
	const unsigned int targetStrideBytes = (width * targetChannels + targetPaddingElements) * dataTypeBytes;

	const unsigned char* src = (const unsigned char*)source;
	int srcStrideBytes = (int)sourceStrideBytes;
	int srcWidth = (int)width;
	int srcHeight = (int)height;
	int srcChannels = (int)sourceChannels;

	unsigned char* dst = (unsigned char*)target;
	int dstWidth = (int)width;
	int dstHeight = (int)height;
	int dstChannels = (int)targetChannels;

	for(;;)
	{
		const int srcStep = srcStrideBytes;
		const int dstStep = (int)targetStrideBytes;

		double scale = 1;
		unsigned char* wave = 0;
		int* itab = 0;
		unsigned char* ptr;
		int i, len, count, sz = 0;
		int use_buf = 0, odd_real = 0;
		DFTFunc dft_func;

		if( stage == 0 ) // row-wise transform
		{
			len = !inv ? srcWidth : dstWidth;
			count = srcHeight;
			if( len == 1 && !(flags & DFT_ROWS) )
			{
				len = !inv ? srcHeight : dstHeight;
				count = 1;
			}
			odd_real = real_transform && (len & 1);
		}
		else
		{
			len = dstHeight;
			count = !inv ? srcWidth : dstWidth;
			sz = 2 * len * complex_elem_size;
		}

		void *spec = 0;

		{
			if( len != prev_len )
				nf = DFTFactorize( len, factors );

			inplace_transform = factors[0] == factors[nf-1];
			sz += len*(complex_elem_size + sizeof(int));
			i = nf > 1 && (factors[0] & 1) == 0;
			if( (factors[i] & 1) != 0 && factors[i] > 5 )
				sz += (factors[i]+1)*complex_elem_size;

			if( (stage == 0 && ((src == dst && !inplace_transform) || odd_real)) ||
				(stage == 1 && !inplace_transform) )
			{
				use_buf = 1;
				sz += len*complex_elem_size;
			}
		}

		ptr = (unsigned char*)buf;
		buf.allocate( sz + 32 );
		if( ptr != (unsigned char*)buf )
			prev_len = 0; // because we release the buffer,
						  // force recalculation of
						  // twiddle factors and permutation table
		ptr = (unsigned char*)buf;
		if( !spec )
		{
			wave = ptr;
			ptr += len*complex_elem_size;
			itab = (int*)ptr;
			ptr = (unsigned char*)alignPtr( ptr + len*sizeof(int), 16 );

			if( len != prev_len || (!inplace_transform && inv && real_transform))
				DFTInit( len, nf, factors, itab, complex_elem_size,
							wave, stage == 0 && inv && real_transform );
			// otherwise reuse the tables calculated on the previous stage
		}

		if( stage == 0 )
		{
			unsigned char* tmp_buf = 0;
			int dptr_offset = 0;
			int dst_full_len = len*elem_size;
			int _flags = (int)inv + (srcChannels != dstChannels ?
						 DFT_COMPLEX_INPUT_OR_OUTPUT : 0);
			if( use_buf )
			{
				tmp_buf = ptr;
				ptr += len*complex_elem_size;
				if( odd_real && !inv && len > 1 &&
					!(_flags & DFT_COMPLEX_INPUT_OR_OUTPUT))
					dptr_offset = elem_size;
			}

			if( !inv && (_flags & DFT_COMPLEX_INPUT_OR_OUTPUT) )
				dst_full_len += (len & 1) ? elem_size : complex_elem_size;

			dft_func = dft_tbl[(!real_transform ? 0 : !inv ? 1 : 2) + (dataType == Ocean::FrameType::DT_SIGNED_FLOAT_64)*3];

			if( count > 1 && !(flags & DFT_ROWS) && (!inv || !real_transform) )
				stage = 1;
			else if( flags & CV_DXT_SCALE )
				scale = 1./(len * (flags & DFT_ROWS ? 1 : count));

			if( nonzero_rows <= 0 || nonzero_rows > count )
				nonzero_rows = count;

			for( i = 0; i < nonzero_rows; i++ )
			{
				const unsigned char* sptr = src + i * srcStep;
				unsigned char* dptr0 = dst + i * dstStep;
				unsigned char* dptr = dptr0;

				if( tmp_buf )
					dptr = tmp_buf;

				dft_func( sptr, dptr, len, nf, factors, itab, wave, len, spec, ptr, _flags, scale );
				if( dptr != dptr0 )
					memcpy( dptr0, dptr + dptr_offset, dst_full_len );
			}

			for( ; i < count; i++ )
			{
				unsigned char* dptr0 = dst + i * dstStep;
				memset( dptr0, 0, dst_full_len );
			}

			if( stage != 1 )
			{
				if( !inv && real_transform && dstChannels == 2 )
				{
					complementComplexOutput((void*)dst, (unsigned int)dstWidth, (unsigned int)dstHeight, (unsigned int)dstChannels, dataType, nonzero_rows, 1, targetPaddingElements);
				}
				break;
			}

			src = (const unsigned char*)dst;
			srcStrideBytes = (int)targetStrideBytes;
			srcWidth = dstWidth;
			srcHeight = dstHeight;
			srcChannels = dstChannels;
		}
		else
		{
			int a = 0, b = count;
			unsigned char *buf0, *buf1, *dbuf0, *dbuf1;
			const unsigned char* sptr0 = src;
			unsigned char* dptr0 = dst;
			buf0 = ptr;
			ptr += len*complex_elem_size;
			buf1 = ptr;
			ptr += len*complex_elem_size;
			dbuf0 = buf0;
			dbuf1 = buf1;

			if( use_buf )
			{
				dbuf1 = ptr;
				dbuf0 = buf1;
				ptr += len*complex_elem_size;
			}

			dft_func = dft_tbl[(dataType == Ocean::FrameType::DT_SIGNED_FLOAT_64)*3];

			if( real_transform && inv && srcWidth > 1 )
				stage = 0;
			else if( flags & CV_DXT_SCALE )
				scale = 1./(len * count);

			if( real_transform )
			{
				int even;
				a = 1;
				even = (count & 1) == 0;
				b = (count+1)/2;
				if( !inv )
				{
					memset( buf0, 0, len*complex_elem_size );
					CopyColumn( sptr0, srcStep, buf0, complex_elem_size, len, elem_size );
					sptr0 += dstChannels * elem_size;
					if( even )
					{
						memset( buf1, 0, len*complex_elem_size );
						CopyColumn( sptr0 + (count-2)*elem_size, srcStep,
									buf1, complex_elem_size, len, elem_size );
					}
				}
				else if( srcChannels == 1 )
				{
					CopyColumn( sptr0, srcStep, buf0, elem_size, len, elem_size );
					ExpandCCS( buf0, len, elem_size );
					if( even )
					{
						CopyColumn( sptr0 + (count-1)*elem_size, srcStep,
									buf1, elem_size, len, elem_size );
						ExpandCCS( buf1, len, elem_size );
					}
					sptr0 += elem_size;
				}
				else
				{
					CopyColumn( sptr0, srcStep, buf0, complex_elem_size, len, complex_elem_size );
					if( even )
					{
						CopyColumn( sptr0 + b*complex_elem_size, srcStep,
										buf1, complex_elem_size, len, complex_elem_size );
					}
					sptr0 += complex_elem_size;
				}

				if( even )
					dft_func( buf1, dbuf1, len, nf, factors, itab,
							  wave, len, spec, ptr, inv, scale );
				dft_func( buf0, dbuf0, len, nf, factors, itab,
						  wave, len, spec, ptr, inv, scale );

				if( dstChannels == 1 )
				{
					if( !inv )
					{
						// copy the half of output vector to the first/last column.
						// before doing that, defgragment the vector
						memcpy( dbuf0 + elem_size, dbuf0, elem_size );
						CopyColumn( dbuf0 + elem_size, elem_size, dptr0,
										dstStep, len, elem_size );
						if( even )
						{
							memcpy( dbuf1 + elem_size, dbuf1, elem_size );
							CopyColumn( dbuf1 + elem_size, elem_size,
											dptr0 + (count-1)*elem_size,
											dstStep, len, elem_size );
						}
						dptr0 += elem_size;
					}
					else
					{
						// copy the real part of the complex vector to the first/last column
						CopyColumn( dbuf0, complex_elem_size, dptr0, dstStep, len, elem_size );
						if( even )
							CopyColumn( dbuf1, complex_elem_size, dptr0 + (count-1)*elem_size,
											dstStep, len, elem_size );
						dptr0 += elem_size;
					}
				}
				else
				{
					ocean_assert( !inv );
					CopyColumn( dbuf0, complex_elem_size, dptr0,
									dstStep, len, complex_elem_size );
					if( even )
						CopyColumn( dbuf1, complex_elem_size,
										dptr0 + b*complex_elem_size,
										dstStep, len, complex_elem_size );
					dptr0 += complex_elem_size;
				}
			}

			for( i = a; i < b; i += 2 )
			{
				if( i+1 < b )
				{
					CopyFrom2Columns( sptr0, srcStep, buf0, buf1, len, complex_elem_size );
					dft_func( buf1, dbuf1, len, nf, factors, itab,
							  wave, len, spec, ptr, inv, scale );
				}
				else
					CopyColumn( sptr0, srcStep, buf0, complex_elem_size, len, complex_elem_size );

				dft_func( buf0, dbuf0, len, nf, factors, itab,
						  wave, len, spec, ptr, inv, scale );

				if( i+1 < b )
					CopyTo2Columns( dbuf0, dbuf1, dptr0, dstStep, len, complex_elem_size );
				else
					CopyColumn( dbuf0, complex_elem_size, dptr0, dstStep, len, complex_elem_size );
				sptr0 += 2*complex_elem_size;
				dptr0 += 2*complex_elem_size;
			}

			if( stage != 0 )
			{
				if( !inv && real_transform && dstChannels == 2 && len > 1 )
				{
					complementComplexOutput((void*)dst, (unsigned int)dstWidth, (unsigned int)dstHeight, (unsigned int)dstChannels, dataType, len, 2, targetPaddingElements);
				}
				break;
			}

			src = (const unsigned char*)dst;
			srcStrideBytes = (int)targetStrideBytes;
			srcWidth = dstWidth;
			srcHeight = dstHeight;
			srcChannels = dstChannels;
		}
	}

	return true;
}

static bool dft(const Ocean::Frame& src0, Ocean::Frame& dst, int flags, int nonzero_rows)
{
	ocean_assert(src0.isValid() && dst.isValid());
	ocean_assert(src0.numberPlanes() == 1u && dst.numberPlanes() == 1u);

	return dft(src0.constdata<void>(), src0.width(), src0.height(), src0.channels(), dst.data<void>(), dst.channels(), src0.dataType(), flags, nonzero_rows, src0.paddingElements(), dst.paddingElements());
}

static const int optimalDFTSizeTab[] = {
1, 2, 3, 4, 5, 6, 8, 9, 10, 12, 15, 16, 18, 20, 24, 25, 27, 30, 32, 36, 40, 45, 48,
50, 54, 60, 64, 72, 75, 80, 81, 90, 96, 100, 108, 120, 125, 128, 135, 144, 150, 160,
162, 180, 192, 200, 216, 225, 240, 243, 250, 256, 270, 288, 300, 320, 324, 360, 375,
384, 400, 405, 432, 450, 480, 486, 500, 512, 540, 576, 600, 625, 640, 648, 675, 720,
729, 750, 768, 800, 810, 864, 900, 960, 972, 1000, 1024, 1080, 1125, 1152, 1200,
1215, 1250, 1280, 1296, 1350, 1440, 1458, 1500, 1536, 1600, 1620, 1728, 1800, 1875,
1920, 1944, 2000, 2025, 2048, 2160, 2187, 2250, 2304, 2400, 2430, 2500, 2560, 2592,
2700, 2880, 2916, 3000, 3072, 3125, 3200, 3240, 3375, 3456, 3600, 3645, 3750, 3840,
3888, 4000, 4050, 4096, 4320, 4374, 4500, 4608, 4800, 4860, 5000, 5120, 5184, 5400,
5625, 5760, 5832, 6000, 6075, 6144, 6250, 6400, 6480, 6561, 6750, 6912, 7200, 7290,
7500, 7680, 7776, 8000, 8100, 8192, 8640, 8748, 9000, 9216, 9375, 9600, 9720, 10000,
10125, 10240, 10368, 10800, 10935, 11250, 11520, 11664, 12000, 12150, 12288, 12500,
12800, 12960, 13122, 13500, 13824, 14400, 14580, 15000, 15360, 15552, 15625, 16000,
16200, 16384, 16875, 17280, 17496, 18000, 18225, 18432, 18750, 19200, 19440, 19683,
20000, 20250, 20480, 20736, 21600, 21870, 22500, 23040, 23328, 24000, 24300, 24576,
25000, 25600, 25920, 26244, 27000, 27648, 28125, 28800, 29160, 30000, 30375, 30720,
31104, 31250, 32000, 32400, 32768, 32805, 33750, 34560, 34992, 36000, 36450, 36864,
37500, 38400, 38880, 39366, 40000, 40500, 40960, 41472, 43200, 43740, 45000, 46080,
46656, 46875, 48000, 48600, 49152, 50000, 50625, 51200, 51840, 52488, 54000, 54675,
55296, 56250, 57600, 58320, 59049, 60000, 60750, 61440, 62208, 62500, 64000, 64800,
65536, 65610, 67500, 69120, 69984, 72000, 72900, 73728, 75000, 76800, 77760, 78125,
78732, 80000, 81000, 81920, 82944, 84375, 86400, 87480, 90000, 91125, 92160, 93312,
93750, 96000, 97200, 98304, 98415, 100000, 101250, 102400, 103680, 104976, 108000,
109350, 110592, 112500, 115200, 116640, 118098, 120000, 121500, 122880, 124416, 125000,
128000, 129600, 131072, 131220, 135000, 138240, 139968, 140625, 144000, 145800, 147456,
150000, 151875, 153600, 155520, 156250, 157464, 160000, 162000, 163840, 164025, 165888,
168750, 172800, 174960, 177147, 180000, 182250, 184320, 186624, 187500, 192000, 194400,
196608, 196830, 200000, 202500, 204800, 207360, 209952, 216000, 218700, 221184, 225000,
230400, 233280, 234375, 236196, 240000, 243000, 245760, 248832, 250000, 253125, 256000,
259200, 262144, 262440, 270000, 273375, 276480, 279936, 281250, 288000, 291600, 294912,
295245, 300000, 303750, 307200, 311040, 312500, 314928, 320000, 324000, 327680, 328050,
331776, 337500, 345600, 349920, 354294, 360000, 364500, 368640, 373248, 375000, 384000,
388800, 390625, 393216, 393660, 400000, 405000, 409600, 414720, 419904, 421875, 432000,
437400, 442368, 450000, 455625, 460800, 466560, 468750, 472392, 480000, 486000, 491520,
492075, 497664, 500000, 506250, 512000, 518400, 524288, 524880, 531441, 540000, 546750,
552960, 559872, 562500, 576000, 583200, 589824, 590490, 600000, 607500, 614400, 622080,
625000, 629856, 640000, 648000, 655360, 656100, 663552, 675000, 691200, 699840, 703125,
708588, 720000, 729000, 737280, 746496, 750000, 759375, 768000, 777600, 781250, 786432,
787320, 800000, 810000, 819200, 820125, 829440, 839808, 843750, 864000, 874800, 884736,
885735, 900000, 911250, 921600, 933120, 937500, 944784, 960000, 972000, 983040, 984150,
995328, 1000000, 1012500, 1024000, 1036800, 1048576, 1049760, 1062882, 1080000, 1093500,
1105920, 1119744, 1125000, 1152000, 1166400, 1171875, 1179648, 1180980, 1200000,
1215000, 1228800, 1244160, 1250000, 1259712, 1265625, 1280000, 1296000, 1310720,
1312200, 1327104, 1350000, 1366875, 1382400, 1399680, 1406250, 1417176, 1440000,
1458000, 1474560, 1476225, 1492992, 1500000, 1518750, 1536000, 1555200, 1562500,
1572864, 1574640, 1594323, 1600000, 1620000, 1638400, 1640250, 1658880, 1679616,
1687500, 1728000, 1749600, 1769472, 1771470, 1800000, 1822500, 1843200, 1866240,
1875000, 1889568, 1920000, 1944000, 1953125, 1966080, 1968300, 1990656, 2000000,
2025000, 2048000, 2073600, 2097152, 2099520, 2109375, 2125764, 2160000, 2187000,
2211840, 2239488, 2250000, 2278125, 2304000, 2332800, 2343750, 2359296, 2361960,
2400000, 2430000, 2457600, 2460375, 2488320, 2500000, 2519424, 2531250, 2560000,
2592000, 2621440, 2624400, 2654208, 2657205, 2700000, 2733750, 2764800, 2799360,
2812500, 2834352, 2880000, 2916000, 2949120, 2952450, 2985984, 3000000, 3037500,
3072000, 3110400, 3125000, 3145728, 3149280, 3188646, 3200000, 3240000, 3276800,
3280500, 3317760, 3359232, 3375000, 3456000, 3499200, 3515625, 3538944, 3542940,
3600000, 3645000, 3686400, 3732480, 3750000, 3779136, 3796875, 3840000, 3888000,
3906250, 3932160, 3936600, 3981312, 4000000, 4050000, 4096000, 4100625, 4147200,
4194304, 4199040, 4218750, 4251528, 4320000, 4374000, 4423680, 4428675, 4478976,
4500000, 4556250, 4608000, 4665600, 4687500, 4718592, 4723920, 4782969, 4800000,
4860000, 4915200, 4920750, 4976640, 5000000, 5038848, 5062500, 5120000, 5184000,
5242880, 5248800, 5308416, 5314410, 5400000, 5467500, 5529600, 5598720, 5625000,
5668704, 5760000, 5832000, 5859375, 5898240, 5904900, 5971968, 6000000, 6075000,
6144000, 6220800, 6250000, 6291456, 6298560, 6328125, 6377292, 6400000, 6480000,
6553600, 6561000, 6635520, 6718464, 6750000, 6834375, 6912000, 6998400, 7031250,
7077888, 7085880, 7200000, 7290000, 7372800, 7381125, 7464960, 7500000, 7558272,
7593750, 7680000, 7776000, 7812500, 7864320, 7873200, 7962624, 7971615, 8000000,
8100000, 8192000, 8201250, 8294400, 8388608, 8398080, 8437500, 8503056, 8640000,
8748000, 8847360, 8857350, 8957952, 9000000, 9112500, 9216000, 9331200, 9375000,
9437184, 9447840, 9565938, 9600000, 9720000, 9765625, 9830400, 9841500, 9953280,
10000000, 10077696, 10125000, 10240000, 10368000, 10485760, 10497600, 10546875, 10616832,
10628820, 10800000, 10935000, 11059200, 11197440, 11250000, 11337408, 11390625, 11520000,
11664000, 11718750, 11796480, 11809800, 11943936, 12000000, 12150000, 12288000, 12301875,
12441600, 12500000, 12582912, 12597120, 12656250, 12754584, 12800000, 12960000, 13107200,
13122000, 13271040, 13286025, 13436928, 13500000, 13668750, 13824000, 13996800, 14062500,
14155776, 14171760, 14400000, 14580000, 14745600, 14762250, 14929920, 15000000, 15116544,
15187500, 15360000, 15552000, 15625000, 15728640, 15746400, 15925248, 15943230, 16000000,
16200000, 16384000, 16402500, 16588800, 16777216, 16796160, 16875000, 17006112, 17280000,
17496000, 17578125, 17694720, 17714700, 17915904, 18000000, 18225000, 18432000, 18662400,
18750000, 18874368, 18895680, 18984375, 19131876, 19200000, 19440000, 19531250, 19660800,
19683000, 19906560, 20000000, 20155392, 20250000, 20480000, 20503125, 20736000, 20971520,
20995200, 21093750, 21233664, 21257640, 21600000, 21870000, 22118400, 22143375, 22394880,
22500000, 22674816, 22781250, 23040000, 23328000, 23437500, 23592960, 23619600, 23887872,
23914845, 24000000, 24300000, 24576000, 24603750, 24883200, 25000000, 25165824, 25194240,
25312500, 25509168, 25600000, 25920000, 26214400, 26244000, 26542080, 26572050, 26873856,
27000000, 27337500, 27648000, 27993600, 28125000, 28311552, 28343520, 28800000, 29160000,
29296875, 29491200, 29524500, 29859840, 30000000, 30233088, 30375000, 30720000, 31104000,
31250000, 31457280, 31492800, 31640625, 31850496, 31886460, 32000000, 32400000, 32768000,
32805000, 33177600, 33554432, 33592320, 33750000, 34012224, 34171875, 34560000, 34992000,
35156250, 35389440, 35429400, 35831808, 36000000, 36450000, 36864000, 36905625, 37324800,
37500000, 37748736, 37791360, 37968750, 38263752, 38400000, 38880000, 39062500, 39321600,
39366000, 39813120, 39858075, 40000000, 40310784, 40500000, 40960000, 41006250, 41472000,
41943040, 41990400, 42187500, 42467328, 42515280, 43200000, 43740000, 44236800, 44286750,
44789760, 45000000, 45349632, 45562500, 46080000, 46656000, 46875000, 47185920, 47239200,
47775744, 47829690, 48000000, 48600000, 48828125, 49152000, 49207500, 49766400, 50000000,
50331648, 50388480, 50625000, 51018336, 51200000, 51840000, 52428800, 52488000, 52734375,
53084160, 53144100, 53747712, 54000000, 54675000, 55296000, 55987200, 56250000, 56623104,
56687040, 56953125, 57600000, 58320000, 58593750, 58982400, 59049000, 59719680, 60000000,
60466176, 60750000, 61440000, 61509375, 62208000, 62500000, 62914560, 62985600, 63281250,
63700992, 63772920, 64000000, 64800000, 65536000, 65610000, 66355200, 66430125, 67108864,
67184640, 67500000, 68024448, 68343750, 69120000, 69984000, 70312500, 70778880, 70858800,
71663616, 72000000, 72900000, 73728000, 73811250, 74649600, 75000000, 75497472, 75582720,
75937500, 76527504, 76800000, 77760000, 78125000, 78643200, 78732000, 79626240, 79716150,
80000000, 80621568, 81000000, 81920000, 82012500, 82944000, 83886080, 83980800, 84375000,
84934656, 85030560, 86400000, 87480000, 87890625, 88473600, 88573500, 89579520, 90000000,
90699264, 91125000, 92160000, 93312000, 93750000, 94371840, 94478400, 94921875, 95551488,
95659380, 96000000, 97200000, 97656250, 98304000, 98415000, 99532800, 100000000,
100663296, 100776960, 101250000, 102036672, 102400000, 102515625, 103680000, 104857600,
104976000, 105468750, 106168320, 106288200, 107495424, 108000000, 109350000, 110592000,
110716875, 111974400, 112500000, 113246208, 113374080, 113906250, 115200000, 116640000,
117187500, 117964800, 118098000, 119439360, 119574225, 120000000, 120932352, 121500000,
122880000, 123018750, 124416000, 125000000, 125829120, 125971200, 126562500, 127401984,
127545840, 128000000, 129600000, 131072000, 131220000, 132710400, 132860250, 134217728,
134369280, 135000000, 136048896, 136687500, 138240000, 139968000, 140625000, 141557760,
141717600, 143327232, 144000000, 145800000, 146484375, 147456000, 147622500, 149299200,
150000000, 150994944, 151165440, 151875000, 153055008, 153600000, 155520000, 156250000,
157286400, 157464000, 158203125, 159252480, 159432300, 160000000, 161243136, 162000000,
163840000, 164025000, 165888000, 167772160, 167961600, 168750000, 169869312, 170061120,
170859375, 172800000, 174960000, 175781250, 176947200, 177147000, 179159040, 180000000,
181398528, 182250000, 184320000, 184528125, 186624000, 187500000, 188743680, 188956800,
189843750, 191102976, 191318760, 192000000, 194400000, 195312500, 196608000, 196830000,
199065600, 199290375, 200000000, 201326592, 201553920, 202500000, 204073344, 204800000,
205031250, 207360000, 209715200, 209952000, 210937500, 212336640, 212576400, 214990848,
216000000, 218700000, 221184000, 221433750, 223948800, 225000000, 226492416, 226748160,
227812500, 230400000, 233280000, 234375000, 235929600, 236196000, 238878720, 239148450,
240000000, 241864704, 243000000, 244140625, 245760000, 246037500, 248832000, 250000000,
251658240, 251942400, 253125000, 254803968, 255091680, 256000000, 259200000, 262144000,
262440000, 263671875, 265420800, 265720500, 268435456, 268738560, 270000000, 272097792,
273375000, 276480000, 279936000, 281250000, 283115520, 283435200, 284765625, 286654464,
288000000, 291600000, 292968750, 294912000, 295245000, 298598400, 300000000, 301989888,
302330880, 303750000, 306110016, 307200000, 307546875, 311040000, 312500000, 314572800,
314928000, 316406250, 318504960, 318864600, 320000000, 322486272, 324000000, 327680000,
328050000, 331776000, 332150625, 335544320, 335923200, 337500000, 339738624, 340122240,
341718750, 345600000, 349920000, 351562500, 353894400, 354294000, 358318080, 360000000,
362797056, 364500000, 368640000, 369056250, 373248000, 375000000, 377487360, 377913600,
379687500, 382205952, 382637520, 384000000, 388800000, 390625000, 393216000, 393660000,
398131200, 398580750, 400000000, 402653184, 403107840, 405000000, 408146688, 409600000,
410062500, 414720000, 419430400, 419904000, 421875000, 424673280, 425152800, 429981696,
432000000, 437400000, 439453125, 442368000, 442867500, 447897600, 450000000, 452984832,
453496320, 455625000, 460800000, 466560000, 468750000, 471859200, 472392000, 474609375,
477757440, 478296900, 480000000, 483729408, 486000000, 488281250, 491520000, 492075000,
497664000, 500000000, 503316480, 503884800, 506250000, 509607936, 510183360, 512000000,
512578125, 518400000, 524288000, 524880000, 527343750, 530841600, 531441000, 536870912,
537477120, 540000000, 544195584, 546750000, 552960000, 553584375, 559872000, 562500000,
566231040, 566870400, 569531250, 573308928, 576000000, 583200000, 585937500, 589824000,
590490000, 597196800, 597871125, 600000000, 603979776, 604661760, 607500000, 612220032,
614400000, 615093750, 622080000, 625000000, 629145600, 629856000, 632812500, 637009920,
637729200, 640000000, 644972544, 648000000, 655360000, 656100000, 663552000, 664301250,
671088640, 671846400, 675000000, 679477248, 680244480, 683437500, 691200000, 699840000,
703125000, 707788800, 708588000, 716636160, 720000000, 725594112, 729000000, 732421875,
737280000, 738112500, 746496000, 750000000, 754974720, 755827200, 759375000, 764411904,
765275040, 768000000, 777600000, 781250000, 786432000, 787320000, 791015625, 796262400,
797161500, 800000000, 805306368, 806215680, 810000000, 816293376, 819200000, 820125000,
829440000, 838860800, 839808000, 843750000, 849346560, 850305600, 854296875, 859963392,
864000000, 874800000, 878906250, 884736000, 885735000, 895795200, 900000000, 905969664,
906992640, 911250000, 921600000, 922640625, 933120000, 937500000, 943718400, 944784000,
949218750, 955514880, 956593800, 960000000, 967458816, 972000000, 976562500, 983040000,
984150000, 995328000, 996451875, 1000000000, 1006632960, 1007769600, 1012500000,
1019215872, 1020366720, 1024000000, 1025156250, 1036800000, 1048576000, 1049760000,
1054687500, 1061683200, 1062882000, 1073741824, 1074954240, 1080000000, 1088391168,
1093500000, 1105920000, 1107168750, 1119744000, 1125000000, 1132462080, 1133740800,
1139062500, 1146617856, 1152000000, 1166400000, 1171875000, 1179648000, 1180980000,
1194393600, 1195742250, 1200000000, 1207959552, 1209323520, 1215000000, 1220703125,
1224440064, 1228800000, 1230187500, 1244160000, 1250000000, 1258291200, 1259712000,
1265625000, 1274019840, 1275458400, 1280000000, 1289945088, 1296000000, 1310720000,
1312200000, 1318359375, 1327104000, 1328602500, 1342177280, 1343692800, 1350000000,
1358954496, 1360488960, 1366875000, 1382400000, 1399680000, 1406250000, 1415577600,
1417176000, 1423828125, 1433272320, 1440000000, 1451188224, 1458000000, 1464843750,
1474560000, 1476225000, 1492992000, 1500000000, 1509949440, 1511654400, 1518750000,
1528823808, 1530550080, 1536000000, 1537734375, 1555200000, 1562500000, 1572864000,
1574640000, 1582031250, 1592524800, 1594323000, 1600000000, 1610612736, 1612431360,
1620000000, 1632586752, 1638400000, 1640250000, 1658880000, 1660753125, 1677721600,
1679616000, 1687500000, 1698693120, 1700611200, 1708593750, 1719926784, 1728000000,
1749600000, 1757812500, 1769472000, 1771470000, 1791590400, 1800000000, 1811939328,
1813985280, 1822500000, 1843200000, 1845281250, 1866240000, 1875000000, 1887436800,
1889568000, 1898437500, 1911029760, 1913187600, 1920000000, 1934917632, 1944000000,
1953125000, 1966080000, 1968300000, 1990656000, 1992903750, 2000000000, 2013265920,
2015539200, 2025000000, 2038431744, 2040733440, 2048000000, 2050312500, 2073600000,
2097152000, 2099520000, 2109375000, 2123366400, 2125764000
};

int getOptimalDFTSize( int size0 )
{
    int a = 0;
    int b = sizeof(optimalDFTSizeTab)/sizeof(optimalDFTSizeTab[0]) - 1;
    if( (unsigned)size0 >= (unsigned)optimalDFTSizeTab[b] )
    {
        return -1;
	}

    while( a < b )
    {
        int c = (a + b) >> 1;
        if( size0 <= optimalDFTSizeTab[c] )
            b = c;
        else
            a = c+1;
    }

    return optimalDFTSizeTab[b];
}

// clang-format on
} // namespace

namespace Ocean
{

bool FourierTransformation::dft0(const Frame& source, Frame& target, int flags, int nonzero_rows)
{
	ocean_assert(source.isValid() && target.isValid());
	const bool inv = (flags & DFT_INVERSE) != 0;

	if (!inv && source.channels() == 1 && (flags & DFT_COMPLEX_OUTPUT))
	{
		if (!target.set(FrameType(source, FrameType::genericPixelFormat(source.dataType(), 2u)), false /*forceOwner*/, true /*forceWritable*/))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}
	}
	else if (inv && source.channels() == 2 && (flags & DFT_REAL_OUTPUT))
	{
		if (!target.set(FrameType(source, FrameType::genericPixelFormat(source.dataType(), 1u)), false /*forceOwner*/, true /*forceWritable*/))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}
	}
	else
	{
		if (!target.set(source.frameType(), false /*forceOwner*/, true /*forceWritable*/))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}
	}

	return dft(source, target, flags, nonzero_rows);
}

bool FourierTransformation::dft0(const void* source, const unsigned int width, const unsigned int height, const unsigned int sourceChannels, void* target, const unsigned int targetChannels, const Ocean::FrameType::DataType dataType, int flags, int nonzero_rows, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements)
{
	return dft(source, width, height, sourceChannels, target, targetChannels, dataType, flags, nonzero_rows, sourcePaddingElements, targetPaddingElements);
}

int FourierTransformation::getOptimalDFTSize0(int size)
{
	return getOptimalDFTSize(size);
}

template <typename T>
void FourierTransformation::spatialToFrequency2(const T* spatial, const unsigned int width, const unsigned int height, T* complexFrequency, const unsigned int spatialPaddingElements, const unsigned int frequencyPaddingElements)
{
	static_assert(std::is_same<T, float>::value || std::is_same<T, double>::value, "Invalid data type!");

	const Frame sourceFrame(FrameType(width, height, FrameType::genericPixelFormat<T, 1u>(), FrameType::ORIGIN_UPPER_LEFT), (const void*)spatial, Frame::CM_USE_KEEP_LAYOUT, spatialPaddingElements);
	Frame targetFrame(FrameType(width, height, FrameType::genericPixelFormat<T, 2u>(), FrameType::ORIGIN_UPPER_LEFT), (void*)complexFrequency, Frame::CM_USE_KEEP_LAYOUT, frequencyPaddingElements);

	dft(sourceFrame, targetFrame, DFT_COMPLEX_OUTPUT, 0);
}

template <typename T>
void FourierTransformation::complexSpatialToFrequency2(const T* complexSpatial, const unsigned int width, const unsigned int height, T* complexFrequency, const unsigned int spatialPaddingElements, const unsigned int frequencyPaddingElements)
{
	static_assert(std::is_same<T, float>::value || std::is_same<T, double>::value, "Invalid data type!");

	const Frame sourceFrame(FrameType(width, height, FrameType::genericPixelFormat<T, 2u>(), FrameType::ORIGIN_UPPER_LEFT), (const void*)complexSpatial, Frame::CM_USE_KEEP_LAYOUT, spatialPaddingElements);
	Frame targetFrame(FrameType(width, height, FrameType::genericPixelFormat<T, 2u>(), FrameType::ORIGIN_UPPER_LEFT), (void*)complexFrequency, Frame::CM_USE_KEEP_LAYOUT, frequencyPaddingElements);

	dft(sourceFrame, targetFrame, DFT_COMPLEX_OUTPUT, 0);
}

template <typename T>
void FourierTransformation::frequencyToSpatial2(const T* complexFrequency, const unsigned int width, const unsigned int height, T* spatial, const unsigned int frequencyPaddingElements, const unsigned int spatialPaddingElements)
{
	static_assert(std::is_same<T, float>::value || std::is_same<T, double>::value, "Invalid data type!");

	const Frame sourceFrame(FrameType(width, height, FrameType::genericPixelFormat<T, 2u>(), FrameType::ORIGIN_UPPER_LEFT), (const void*)complexFrequency, Frame::CM_USE_KEEP_LAYOUT, frequencyPaddingElements);
	Frame targetFrame(FrameType(width, height, FrameType::genericPixelFormat<T, 1u>(), FrameType::ORIGIN_UPPER_LEFT), (void*)spatial, Frame::CM_USE_KEEP_LAYOUT, spatialPaddingElements);

	dft(sourceFrame, targetFrame, DFT_SCALE | DFT_REAL_OUTPUT | DFT_INVERSE, 0);
}

template <typename T>
void FourierTransformation::frequencyToComplexSpatial2(const T* complexFrequency, const unsigned int width, const unsigned int height, T* complexSpatial, const unsigned int frequencyPaddingElements, const unsigned int spatialPaddingElements)
{
	static_assert(std::is_same<T, float>::value || std::is_same<T, double>::value, "Invalid data type!");

	const Frame sourceFrame(FrameType(width, height, FrameType::genericPixelFormat<T, 2u>(), FrameType::ORIGIN_UPPER_LEFT), (const void*)complexFrequency, Frame::CM_USE_KEEP_LAYOUT, frequencyPaddingElements);
	Frame targetFrame(FrameType(width, height, FrameType::genericPixelFormat<T, 2u>(), FrameType::ORIGIN_UPPER_LEFT), (void*)complexSpatial, Frame::CM_USE_KEEP_LAYOUT, spatialPaddingElements);

	dft(sourceFrame, targetFrame, DFT_SCALE | DFT_COMPLEX_OUTPUT | DFT_INVERSE, 0);
}

template void OCEAN_MATH_EXPORT FourierTransformation::spatialToFrequency2<double>(const double*, const unsigned int, const unsigned int, double*, const unsigned int, const unsigned int);
template void OCEAN_MATH_EXPORT FourierTransformation::spatialToFrequency2<float>(const float*, const unsigned int, const unsigned int, float*, const unsigned int, const unsigned int);
template void OCEAN_MATH_EXPORT FourierTransformation::complexSpatialToFrequency2<double>(const double*, const unsigned int, const unsigned int, double*, const unsigned int, const unsigned int);
template void OCEAN_MATH_EXPORT FourierTransformation::complexSpatialToFrequency2<float>(const float*, const unsigned int, const unsigned int, float*, const unsigned int, const unsigned int);

template void OCEAN_MATH_EXPORT FourierTransformation::frequencyToSpatial2<double>(const double*, const unsigned int, const unsigned int, double*, const unsigned int, const unsigned int);
template void OCEAN_MATH_EXPORT FourierTransformation::frequencyToSpatial2<float>(const float*, const unsigned int, const unsigned int, float*, const unsigned int, const unsigned int);
template void OCEAN_MATH_EXPORT FourierTransformation::frequencyToComplexSpatial2<double>(const double*, const unsigned int, const unsigned int, double*, const unsigned int, const unsigned int);
template void OCEAN_MATH_EXPORT FourierTransformation::frequencyToComplexSpatial2<float>(const float*, const unsigned int, const unsigned int, float*, const unsigned int, const unsigned int);

}
