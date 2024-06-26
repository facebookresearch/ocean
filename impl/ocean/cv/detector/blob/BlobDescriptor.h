/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_DETECTOR_BLOB_DESCRIPTOR_H
#define META_OCEAN_CV_DETECTOR_BLOB_DESCRIPTOR_H

#include "ocean/cv/detector/blob/Blob.h"

#include "ocean/base/DataType.h"

#include "ocean/cv/SSE.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace Blob
{

/*#ifdef OCEAN_HARDWARE_REDUCED_PERFORMANCE
	#ifndef OCEAN_CV_DETECTOR_BLOB_USE_DESCRIPTOR_36
		#define OCEAN_CV_DETECTOR_BLOB_USE_DESCRIPTOR_36
	#endif
	#ifndef OCEAN_CV_DETECTOR_BLOB_USE_INTEGER_DESCRIPTOR
		#define OCEAN_CV_DETECTOR_BLOB_USE_INTEGER_DESCRIPTOR
	#endif
#endif*/

// We set the descriptor dimension hard coded to 36 for any platform
// as we need to reduce the binary size of descriptors for TargetAR.
// We should create a more flexible solution to support features
// with arbitrary dimensions...
#ifndef OCEAN_CV_DETECTOR_BLOB_USE_DESCRIPTOR_36
	#define OCEAN_CV_DETECTOR_BLOB_USE_DESCRIPTOR_36
#endif

/**
 * This class implements a Blob descriptor.
 * @ingroup cvdetectorblob
 */
class OCEAN_CV_DETECTOR_BLOB_EXPORT BlobDescriptor
{
	public:

#ifdef OCEAN_CV_DETECTOR_BLOB_USE_INTEGER_DESCRIPTOR

		/**
		 * Definition of a descriptor element.
		 */
		typedef int DescriptorElement;

		/**
		 * Definition of a descriptor ssd value.
		 */
		typedef unsigned int DescriptorSSD;

#else

		/**
		 * Definition of a descriptor element.
		 */
		typedef float DescriptorElement;

		/**
		 * Definition of a descriptor ssd value.
		 */
		typedef float DescriptorSSD;

#endif // OCEAN_CV_DETECTOR_BLOB_USE_INTEGER_DESCRIPTOR

	private:

#ifdef OCEAN_CV_DETECTOR_BLOB_USE_DESCRIPTOR_36

	#define definedDescriptorElements 36

#else

	#define definedDescriptorElements 64

#endif // OCEAN_CV_DETECTOR_BLOB_USE_DESCRIPTOR_36

	protected:

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

		/**
		 * This class implements a simple helper class for the calculation of SSDs using SSE instructions.
		 * @tparam TElement The data type of one descriptor element, can be 'int', 'float' or 'double'.
		 */
		template <typename TElement>
		class SummedSquareDifferencesSSE
		{
			public:

				/**
				 * Returns the summed square distance (SSD) between two descriptors.
				 * @param firstDescriptorElements The 'tElements' elements of the first descriptor to be used for the SSD calculation, must be valid
				 * @param secondDescriptorElements The 'tElements' elements of the second descriptor to be used for the SSD calculation, must be valid
				 * @return SSD between both feature descriptors, with range [0, infinity)
				 * @tparam tElements The number of elements the descriptor has, possible values are 36 or 64
				 */
				template <unsigned int tElements>
				static OCEAN_FORCE_INLINE typename SquareValueTyper<TElement>::Type ssd(const TElement* firstDescriptorElements, const TElement* secondDescriptorElements);
		};

#endif // OCEAN_HARDWARE_SSE_VERSION >= 41

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

		/**
		 * This class implements a simple helper class for the calculation of SSDs using NEON instructions.
		 * @tparam TElement The data type of one descriptor element, can be 'int', 'float' or 'double'.
		 */
		template <typename TElement>
		class SummedSquareDifferencesNEON
		{
			public:

				/**
				 * Returns the summed square distance (SSD) between two descriptors.
				 * @param firstDescriptorElements The 'tElements' elements of the first descriptor to be used for the SSD calculation, must be valid
				 * @param secondDescriptorElements The 'tElements' elements of the second descriptor to be used for the SSD calculation, must be valid
				 * @return SSD between both feature descriptors, with range [0, infinity)
				 * @tparam tElements The number of elements the descriptor has, possible values are 36 or 64
				 */
				template <unsigned int tElements>
				static OCEAN_FORCE_INLINE typename SquareValueTyper<TElement>::Type ssd(const TElement* firstDescriptorElements, const TElement* secondDescriptorElements);
		};

#endif // OCEAN_HARDWARE_SSE_VERSION >= 41

	public:

		/**
		 * Creates a new descriptor object.
		 * The descriptor data will be initialized with zero.
		 */
		inline BlobDescriptor();

		/**
		 * Returns the summed square distance (SSD) between two descriptors.
		 * @param secondDescriptor Descriptor of second feature
		 * @return SSD between both feature descriptors, with range [0, infinity)
		 */
		OCEAN_FORCE_INLINE DescriptorSSD ssd(const BlobDescriptor& secondDescriptor) const;

		/**
		 * Returns whether the summed square distance (SSD) between two descriptors is below a specified threshold.
		 * @param secondDescriptor Descriptor of second feature
		 * @param threshold Upper SSD boundary, with range [0, infinity)
		 * @param ssd Resulting SSD between two descriptors, with range [0, infinity)
		 * @return True, if so
		 */
		inline bool isDescriptorEqual(const BlobDescriptor& secondDescriptor, const DescriptorSSD threshold, DescriptorSSD& ssd) const;

		/**
		 * Returns whether the summed square distance (SSD) between two descriptors is below a specified threshold.
		 * This test checks the inner kernel first and stops if a large portion of the allowed threshold is reached in the internal kernel already.<br>
		 * Actually, this function is a wrapper for platform specific functions only.
		 * @param secondDescriptor Descriptor of second feature
		 * @param threshold Upper SSD boundary, with range [0, infinity)
		 * @param ssd Resulting SSD between two descriptors, with range [0, infinity)
		 * @return True, if so
		 * @see isDescriptorEqualEarlyRejectFallback(), isDescriptorEqualEarlyRejectNEON().
		 */
		inline bool isDescriptorEqualEarlyReject(const BlobDescriptor& secondDescriptor, const DescriptorSSD threshold, DescriptorSSD& ssd) const;

		/**
		 * Returns the data of the descriptor.
		 * @return Descriptor data
		 */
		inline const DescriptorElement* operator()() const;

		/**
		 * Returns the data of the descriptor.
		 * @return Descriptor data
		 */
		inline DescriptorElement* operator()();

		/**
		 * Returns one element of the descriptor specified by a defined index.
		 * @param index Element index to return, with range [0, elements())
		 * @return Descriptor element
		 */
		inline DescriptorElement operator[](const unsigned int index) const;

		/**
		 * Returns one element of the descriptor specified by a defined index.
		 * @param index Element index to return, with range [0, elements())
		 * @return Descriptor element
		 */
		inline DescriptorElement& operator[](const unsigned int index);

		/**
		 * Returns the number of elements sorted for this descriptor.
		 * @return Element number
		 */
		static constexpr unsigned int elements();

		/**
		 * Returns the explicit descriptor normalization value.
		 * @return Normalization value
		 */
		static constexpr Scalar descriptorNormalization();

		/**
		 * Returns the explicit descriptor normalization value in relation to a specified element type.
		 * @return Normalization value
		 * @tparam TElement The data type of all descriptor elements
		 */
		template <typename TElement>
		static constexpr TElement descriptorNormalization();

		/**
		 * Returns the explicit squared descriptor normalization value.
		 * @return Squared normalization value
		 */
		static constexpr Scalar descriptorNormalizationSqr();

		/**
		 * Returns a maximal descriptor ssd value.
		 * @return Maximal ssd value
		 */
		static constexpr DescriptorSSD descriptorMaximalSSD();

	protected:

		/**
		 * Returns the summed square distance (SSD) between two descriptors.
		 * This function does not apply SIMD instructions.
		 * @param firstDescriptorElements The 'tElements' elements of the first descriptor to be used for the SSD calculation, must be valid
		 * @param secondDescriptorElements The 'tElements' elements of the second descriptor to be used for the SSD calculation, must be valid
		 * @return SSD between both feature descriptors, with range [0, infinity)
		 * @tparam TElement The data type of one descriptor element, can be 'int', 'float' or 'double'.
		 * @tparam tElements The number of elements the descriptor has, possible values are 36 or 64
		 */
		template <typename TElement, unsigned int tElements>
		static OCEAN_FORCE_INLINE typename SquareValueTyper<TElement>::Type ssdFallback(const TElement* firstDescriptorElements, const TElement* secondDescriptorElements);

		/**
		 * Returns whether the summed square distance (SSD) between two descriptors is below a specified threshold.
		 * This test checks the inner kernel first and stops if a large portion of the allowed threshold is reached in the internal kernel already.<br>
		 * This function does not apply SIMD instructions.
		 * @param firstDescriptorElements The 'tElements' elements of the first descriptor to be used for the SSD calculation, must be valid
		 * @param secondDescriptorElements The 'tElements' elements of the second descriptor to be used for the SSD calculation, must be valid
		 * @param threshold Upper SSD boundary, with range [0, infinity)
		 * @param ssd Resulting SSD between two descriptors, with range [0, infinity)
		 * @return True, if so
		 * @tparam TElement The data type of one descriptor element, can be 'int', 'float' or 'double'.
		 * @tparam tElements The number of elements the descriptor has, possible values are 36 or 64
		 * @see isDescriptorEqualEarlyRejectt().
		 */
		template <typename TElement, unsigned int tElements>
		static OCEAN_FORCE_INLINE bool isDescriptorEqualEarlyRejectFallback(const TElement* firstDescriptorElements, const TElement* secondDescriptorElements, const typename SquareValueTyper<TElement>::Type threshold, typename SquareValueTyper<TElement>::Type& ssd);

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

		/**
		 * Returns whether the summed square distance (SSD) between two descriptors is below a specified threshold.
		 * This test checks the inner kernel first and stops if a large portion of the allowed threshold is reached in the internal kernel already.<br>
		 * This functions applies SSE (needing 4.1) instructions for the computations.
		 * @param firstDescriptorElements The 'tElements' elements of the first descriptor to be used for the SSD calculation, must be valid
		 * @param secondDescriptorElements The 'tElements'; elements of the second descriptor to be used for the SSD calculation, must be valid
		 * @param threshold Upper SSD boundary, with range [0, infinity)
		 * @param ssd Resulting SSD between two descriptors, with range [0, infinity)
		 * @tparam TElement The data type of one descriptor element, can be 'int', 'float' or 'double'.
		 * @tparam tElements The number of elements the descriptor has, possible values are 36 or 64
		 * @return True, if so
		 */
		template <typename TElement, unsigned int tElements>
		static OCEAN_FORCE_INLINE bool isDescriptorEqualEarlyRejectSSE(const TElement* firstDescriptorElements, const TElement* secondDescriptorElements, const typename SquareValueTyper<TElement>::Type threshold, typename SquareValueTyper<TElement>::Type& ssd);

#endif

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

		/**
		 * Returns whether the summed square distance (SSD) between two descriptors is below a specified threshold.
		 * This test checks the inner kernel first and stops if a large portion of the allowed threshold is reached in the internal kernel already.<br>
		 * This functions applies NEON instructions for the computations.
		 * @param firstDescriptorElements The 'tElements' elements of the first descriptor to be used for the SSD calculation, must be valid
		 * @param secondDescriptorElements The 'tElements' elements of the second descriptor to be used for the SSD calculation, must be valid
		 * @param threshold Upper SSD boundary, with range [0, infinity)
		 * @param ssd Resulting SSD between two descriptors, with range [0, infinity)
		 * @return True, if so
		 * @tparam TElement The data type of one descriptor element, can be 'int', 'float' or 'double'.
		 * @tparam tElements The number of elements the descriptor has, possible values are 36 or 64
		 */
		template <typename TElement, unsigned int tElements>
		static OCEAN_FORCE_INLINE bool isDescriptorEqualEarlyRejectNEON(const TElement* firstDescriptorElements, const TElement* secondDescriptorElements, const typename SquareValueTyper<TElement>::Type threshold, typename SquareValueTyper<TElement>::Type& ssd);

#endif

		/**
		 * Definition of a function returning the square value.
		 * @param value The value to be squared
		 * @return Squared value
		 * @tparam T The data type of the value to be squared, and the data type of the result
		 */
		template <typename T>
		static OCEAN_FORCE_INLINE T sqr(const T value);

	private:

		/// Descriptor elements.
		DescriptorElement featureDescriptor[definedDescriptorElements];
};

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

template <>
template <unsigned int tElements>
OCEAN_FORCE_INLINE unsigned int BlobDescriptor::SummedSquareDifferencesSSE<int>::ssd(const int* firstDescriptorElements, const int* secondDescriptorElements)
{
	static_assert(tElements == 36u || tElements == 64u, "Invalid element number!");

	ocean_assert(firstDescriptorElements != nullptr && secondDescriptorElements != nullptr);

	// the following code uses the following SSE instructions, and thus needs SSE4.1 or higher

	// SSE2:
	// _mm_sub_epi32
	// _mm_add_epi32

	// SSE3:
	// _mm_lddqu_si128

	// SSE4.1:
	// _mm_mullo_epi32

	// the individual descriptor values come in the range [-4000, 4000]
	// thus, we simply can multiply signed integers as the result will be within a signed 32 bit integer:
	// 4000^2 * 64 = 1,024,000,000

	__m128i first = _mm_lddqu_si128((const __m128i*)firstDescriptorElements + 0);
	__m128i second = _mm_lddqu_si128((const __m128i*)secondDescriptorElements + 0);
	__m128i difference = _mm_sub_epi32(first, second);
	__m128i result = _mm_mullo_epi32(difference, difference);

	for (unsigned int n = 1u; n < tElements / 4u; ++n)
	{
		first = _mm_lddqu_si128((const __m128i*)firstDescriptorElements + n);
		second = _mm_lddqu_si128((const __m128i*)secondDescriptorElements + n);
		difference = _mm_sub_epi32(first, second);
		result = _mm_add_epi32(result, _mm_mullo_epi32(difference, difference));
	}

	return SSE::sum_u32_4(result);
}

template <>
template <unsigned int tElements>
OCEAN_FORCE_INLINE float BlobDescriptor::SummedSquareDifferencesSSE<float>::ssd(const float* firstDescriptorElements, const float* secondDescriptorElements)
{
	static_assert(tElements == 36u || tElements == 64u, "Invalid element number!");

	ocean_assert(firstDescriptorElements != nullptr && secondDescriptorElements != nullptr);

	// the following code uses the following SSE instructions, and thus needs SSE1 or higher

	// SSE1:
	// _mm_loadu_ps
	// _mm_sub_ps
	// _mm_mul_ps
	// _mm_add_ps

	__m128 first = _mm_loadu_ps(firstDescriptorElements + 0);
	__m128 second = _mm_loadu_ps(secondDescriptorElements + 0);
	__m128 difference = _mm_sub_ps(first, second);
	__m128 result = _mm_mul_ps(difference, difference);

	for (unsigned int n = 4u; n < tElements; n += 4u)
	{
		first = _mm_loadu_ps(firstDescriptorElements + n);
		second = _mm_loadu_ps(secondDescriptorElements + n);
		difference = _mm_sub_ps(first, second);
		result = _mm_add_ps(result, _mm_mul_ps(difference, difference));
	}

	return SSE::sum_f32_4(result);
}

template <>
template <unsigned int tElements>
OCEAN_FORCE_INLINE double BlobDescriptor::SummedSquareDifferencesSSE<double>::ssd(const double* firstDescriptorElements, const double* secondDescriptorElements)
{
	static_assert(tElements == 36u || tElements == 64u, "Invalid element number!");

	ocean_assert(firstDescriptorElements != nullptr && secondDescriptorElements != nullptr);

	// the following code uses the following SSE2 instructions, and thus needs SSE2 or higher

	// SSE2:
	// _mm_loadu_pd
	// _mm_sub_pd
	// _mm_mul_pd
	// _mm_add_pd

	__m128d first = _mm_loadu_pd(firstDescriptorElements + 0);
	__m128d second = _mm_loadu_pd(secondDescriptorElements + 0);
	__m128d difference = _mm_sub_pd(first, second);
	__m128d result = _mm_mul_pd(difference, difference);

	for (unsigned int n = 2u; n < tElements; n += 2u)
	{
		first = _mm_loadu_pd(firstDescriptorElements + n);
		second = _mm_loadu_pd(secondDescriptorElements + n);
		difference = _mm_sub_pd(first, second);
		result = _mm_add_pd(result, _mm_mul_pd(difference, difference));
	}

	return SSE::sum_f64_2(result);
};

#endif // OCEAN_HARDWARE_SSE_VERSION

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

template <>
template <unsigned int tElements>
OCEAN_FORCE_INLINE unsigned int BlobDescriptor::SummedSquareDifferencesNEON<int>::ssd(const int* firstDescriptorElements, const int* secondDescriptorElements)
{
	static_assert(tElements == 36u || tElements == 64u, "Invalid element number!");

	ocean_assert(firstDescriptorElements != nullptr && secondDescriptorElements != nullptr);

	// we determine the square distance for four 32 bit integers concurrently
	// and keep the result in NEON registers until we reach the last element
	// (as summing up the four 32 bit integers is very expensive)

	// loading the first pair of four 32 bit integers
	int32x4_t first = vld1q_s32(firstDescriptorElements + 0);              // [first3,  first2,  first1,  first0 ]
	int32x4_t second = vld1q_s32(secondDescriptorElements + 0);  // [second3, second2, second1, second0]
	// determining the individual differences
	int32x4_t difference = vsubq_s32(first, second);                 // [first3 - second3, first2 - second2, ... ]
	// calculating the square differences
	uint32x4_t result = vreinterpretq_u32_s32(vmulq_s32(difference, difference)); // [(first3 - second3)^2, (first2 - second2)^2, ... ]

	// loading the next pair of four 32 bit floats, and so on

	for (unsigned int n = 4u; n < tElements; n += 4u)
	{
		first = vld1q_s32(firstDescriptorElements + n);
		second = vld1q_s32(secondDescriptorElements + n);
		difference = vsubq_s32(first, second);
		// calculating the square differences and adding them directly to the existing onces
		result = vaddq_u32(result, vreinterpretq_u32_s32(vmulq_s32(difference, difference)));
	}

	return vgetq_lane_u32(result, 0) + vgetq_lane_u32(result, 1) + vgetq_lane_u32(result, 2) + vgetq_lane_u32(result, 3);
}

template <>
template <unsigned int tElements>
OCEAN_FORCE_INLINE float BlobDescriptor::SummedSquareDifferencesNEON<float>::ssd(const float* firstDescriptorElements, const float* secondDescriptorElements)
{
	static_assert(tElements == 36u || tElements == 64u, "Invalid element number!");

	ocean_assert(firstDescriptorElements != nullptr && secondDescriptorElements != nullptr);

	float32x4_t first = vld1q_f32(firstDescriptorElements + 0);
	float32x4_t second = vld1q_f32(secondDescriptorElements + 0);
	float32x4_t difference = vsubq_f32(first, second);
	float32x4_t result = vmulq_f32(difference, difference);

	for (unsigned int n = 4u; n < tElements; n += 4u)
	{
		first = vld1q_f32(firstDescriptorElements + n);
		second = vld1q_f32(secondDescriptorElements + n);
		difference = vsubq_f32(first, second);
		result = vmlaq_f32(result, difference, difference);
	}

	return vgetq_lane_f32(result, 0) + vgetq_lane_f32(result, 1) + vgetq_lane_f32(result, 2) + vgetq_lane_f32(result, 3);
}

template <>
template <unsigned int tElements>
OCEAN_FORCE_INLINE double BlobDescriptor::SummedSquareDifferencesNEON<double>::ssd(const double* firstDescriptorElements, const double* secondDescriptorElements)
{
	static_assert(tElements == 36u || tElements == 64u, "Invalid element number!");

	ocean_assert(firstDescriptorElements != nullptr && secondDescriptorElements != nullptr);

	// Not optimized NEON version as NEON 1.0 does not support 64 bit floating point values
	return ssdFallback<double, tElements>(firstDescriptorElements, secondDescriptorElements);
}

#endif // OCEAN_HARDWARE_NEON_VERSION

inline BlobDescriptor::BlobDescriptor()
{
	ocean_assert(sizeof(featureDescriptor) == sizeof(DescriptorElement) * elements());
	memset(featureDescriptor, 0, sizeof(featureDescriptor));
}

constexpr unsigned int BlobDescriptor::elements()
{
	return definedDescriptorElements;
}

inline BlobDescriptor::DescriptorSSD BlobDescriptor::ssd(const BlobDescriptor& secondDescriptor) const
{
#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

	return SummedSquareDifferencesSSE<DescriptorElement>::ssd<definedDescriptorElements>(featureDescriptor, secondDescriptor.featureDescriptor);

#elif defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	return SummedSquareDifferencesNEON<DescriptorElement>::ssd<definedDescriptorElements>(featureDescriptor, secondDescriptor.featureDescriptor);

#else

	return ssdFallback<DescriptorElement, definedDescriptorElements>(featureDescriptor, secondDescriptor.featureDescriptor);

#endif
}

inline bool BlobDescriptor::isDescriptorEqual(const BlobDescriptor& secondDescriptor, const DescriptorSSD threshold, DescriptorSSD& ssd) const
{
	ssd = BlobDescriptor::ssd(secondDescriptor);

	return ssd <= threshold;
}

constexpr Scalar BlobDescriptor::descriptorNormalization()
{

#ifdef OCEAN_CV_DETECTOR_BLOB_USE_INTEGER_DESCRIPTOR

	/// 4000 << 4096!
	return 4000;

#else

	return 1;

#endif // OCEAN_CV_DETECTOR_BLOB_USE_INTEGER_DESCRIPTOR

}

template <>
constexpr int BlobDescriptor::descriptorNormalization<int>()
{
	/// 4000 << 4096!
	return 4000;
}

template <>
constexpr float BlobDescriptor::descriptorNormalization<float>()
{
	return 1.0f;
}

template <>
constexpr double BlobDescriptor::descriptorNormalization<double>()
{
	return 1.0f;
}

constexpr Scalar BlobDescriptor::descriptorNormalizationSqr()
{
	return descriptorNormalization() * descriptorNormalization();
}

constexpr BlobDescriptor::DescriptorSSD BlobDescriptor::descriptorMaximalSSD()
{

#ifdef OCEAN_CV_DETECTOR_BLOB_USE_INTEGER_DESCRIPTOR

	return DescriptorSSD(-1);

#else

	return NumericT<BlobDescriptor::DescriptorSSD>::maxValue();

#endif // OCEAN_CV_DETECTOR_BLOB_USE_INTEGER_DESCRIPTOR

}

inline const BlobDescriptor::DescriptorElement* BlobDescriptor::operator()() const
{
	return featureDescriptor;
}

inline BlobDescriptor::DescriptorElement* BlobDescriptor::operator()()
{
	return featureDescriptor;
}

inline BlobDescriptor::DescriptorElement BlobDescriptor::operator[](const unsigned int index) const
{
	ocean_assert(index < elements());
	return featureDescriptor[index];
}

inline BlobDescriptor::DescriptorElement& BlobDescriptor::operator[](const unsigned int index)
{
	ocean_assert(index < elements());
	return featureDescriptor[index];
}

template <typename TElement, unsigned int tElements>
OCEAN_FORCE_INLINE typename SquareValueTyper<TElement>::Type BlobDescriptor::ssdFallback(const TElement* firstDescriptorElements, const TElement* secondDescriptorElements)
{
	static_assert(std::is_same<TElement, int>::value || std::is_same<TElement, float>::value || std::is_same<TElement, double>::value, "Invalid element data type!");
	static_assert(tElements == 36u || tElements == 64u, "Invalid element number!");

	ocean_assert(firstDescriptorElements != nullptr && secondDescriptorElements != nullptr);

	// the following code is quite simple that normally modern compiler should be able to apply SIMD instructions on it's own
	// however, at least for x86 platforms some compilers still seem to avoid the usage of SEE instructions
	// therefore, this implementation should be a pure backup implementation only

	typename SquareValueTyper<TElement>::Type ssd = sqr(firstDescriptorElements[0] - secondDescriptorElements[0]);

	for (size_t n = 1; n < tElements; ++n)
		ssd += sqr(firstDescriptorElements[n] - secondDescriptorElements[n]);

	return ssd;
}

template <typename TElement, unsigned int tElements>
OCEAN_FORCE_INLINE bool BlobDescriptor::isDescriptorEqualEarlyRejectFallback(const TElement* firstDescriptorElements, const TElement* secondDescriptorElements, const typename SquareValueTyper<TElement>::Type threshold, typename SquareValueTyper<TElement>::Type& ssd)
{
	static_assert(std::is_same<TElement, int>::value || std::is_same<TElement, float>::value || std::is_same<TElement, double>::value, "Invalid element data type!");
	static_assert(tElements == 36u || tElements == 64u, "Invalid element number!");

	ocean_assert(firstDescriptorElements != nullptr && secondDescriptorElements != nullptr);

	typedef typename SquareValueTyper<TElement>::Type SSDType;

	if constexpr (tElements == 36u)
	{
		// first the inner kernel

		SSDType middleSSD = sqr(firstDescriptorElements[16] - secondDescriptorElements[16]) + sqr(firstDescriptorElements[17] - secondDescriptorElements[17])
												+ sqr(firstDescriptorElements[18] - secondDescriptorElements[18]) + sqr(firstDescriptorElements[19] - secondDescriptorElements[19]);

		// the inner descriptor kernel holds about 65% of the descriptor energy, the outer ring holds about 35%
		// thus we add additional 5% to increase the probability for a correct decision and check for 70%

		if constexpr (std::is_same<SSDType, float>::value || std::is_same<SSDType, double>::value)
		{
			if (middleSSD > SSDType(0.70) * threshold)
			{
				return false;
			}
		}
		else
		{
			ocean_assert((std::is_same<SSDType, unsigned int>::value));

			if (middleSSD > threshold * 90u / 128u) // 90/128 = 0.703125
			{
				return false;
			}
		}

		// now the remaining information outside the inner kernel

		for (unsigned int n = 0u; n < 16u; ++n)
		{
			middleSSD += sqr(firstDescriptorElements[n] - secondDescriptorElements[n]);
		}

		for (unsigned int n = 20u; n < 36u; ++n)
		{
			middleSSD += sqr(firstDescriptorElements[n] - secondDescriptorElements[n]);
		}

		ssd = middleSSD;

		return ssd <= threshold;
	}
	else
	{
		// first the inner kernel

		SSDType middleSSD = SSDType(0);

		for (unsigned int n = 20u; n < 28u; ++n)
		{
			middleSSD += sqr(firstDescriptorElements[n] - secondDescriptorElements[n]);
		}

		for (unsigned int n = 36u; n < 44u; ++n)
		{
			middleSSD += sqr(firstDescriptorElements[n] - secondDescriptorElements[n]);
		}

		// the inner descriptor kernel holds about 81% of the descriptor energy, the outer ring holds about 19%
		// thus we add additional 5% to increase the probability for a correct decision and check for 86%

		if constexpr (std::is_same<SSDType, float>::value || std::is_same<SSDType, double>::value)
		{
			if (middleSSD > SSDType(0.86) * threshold)
			{
				return false;
			}
		}
		else
		{
			ocean_assert((std::is_same<SSDType, unsigned int>::value));

			if (middleSSD > threshold * 110u / 128u) // 110/128 = 0.859375
			{
				return false;
			}
		}

		// now the remaining information outside the inner kernel

		for (unsigned int n = 0u; n < 20u; ++n)
		{
			middleSSD += sqr(firstDescriptorElements[n] - secondDescriptorElements[n]);
		}

		for (unsigned int n = 28u; n < 36u; ++n)
		{
			middleSSD += sqr(firstDescriptorElements[n] - secondDescriptorElements[n]);
		}

		for (unsigned int n = 44u; n < 64u; ++n)
		{
			middleSSD += sqr(firstDescriptorElements[n] - secondDescriptorElements[n]);
		}

		ssd = middleSSD;

		return ssd <= threshold;
	}
}

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

template <>
OCEAN_FORCE_INLINE bool BlobDescriptor::isDescriptorEqualEarlyRejectSSE<int, 36u>(const int* firstDescriptorElements, const int* secondDescriptorElements, const unsigned int threshold, unsigned int& ssd)
{
	ocean_assert(firstDescriptorElements != nullptr && secondDescriptorElements != nullptr);

	// the following code uses the following SSE instructions, and thus needs SSE4.1 or higher

	// SSE2:
	// _mm_sub_epi32
	// _mm_add_epi32

	// SSE3:
	// _mm_lddqu_si128

	// SSE4.1:
	// _mm_mullo_epi32

	// the individual descriptor values come in the range [-4000, 4000]
	// thus, we simply can multiply signed integers as the result will be within a signed 32 bit integer:
	// 4000^2 * 36 = 576,000,000

	const unsigned int middleSSD = sqr(firstDescriptorElements[16] - secondDescriptorElements[16]) + sqr(firstDescriptorElements[17] - secondDescriptorElements[17])
										+ sqr(firstDescriptorElements[18] - secondDescriptorElements[18]) + sqr(firstDescriptorElements[19] - secondDescriptorElements[19]);

	// the inner descriptor kernel holds about 65% of the descriptor energy, the outer ring holds about 35%
	// thus we add additional 5% to increase the probability for a correct decision and check for 70%
	if (middleSSD > threshold * 90u / 128u) // 90/128 = 0.703125
		return false;

	__m128i first = _mm_lddqu_si128((const __m128i*)firstDescriptorElements + 0);
	__m128i second = _mm_lddqu_si128((const __m128i*)secondDescriptorElements + 0);
	__m128i difference = _mm_sub_epi32(first, second);
	__m128i result = _mm_mullo_epi32(difference, difference);

	for (unsigned int n = 1u; n <= 3u; ++n)
	{
		first = _mm_lddqu_si128((const __m128i*)firstDescriptorElements + n);
		second = _mm_lddqu_si128((const __m128i*)secondDescriptorElements + n);
		difference = _mm_sub_epi32(first, second);
		result = _mm_add_epi32(result, _mm_mullo_epi32(difference, difference));
	}

	for (unsigned int n = 5u; n <= 8u; ++n)
	{
		first = _mm_lddqu_si128((const __m128i*)firstDescriptorElements + n);
		second = _mm_lddqu_si128((const __m128i*)secondDescriptorElements + n);
		difference = _mm_sub_epi32(first, second);
		result = _mm_add_epi32(result, _mm_mullo_epi32(difference, difference));
	}

	ssd = middleSSD + SSE::sum_u32_4(result);

	return ssd <= threshold;
}

template <>
OCEAN_FORCE_INLINE bool BlobDescriptor::isDescriptorEqualEarlyRejectSSE<int, 64u>(const int* firstDescriptorElements, const int* secondDescriptorElements, const unsigned int threshold, unsigned int& ssd)
{
	ocean_assert(firstDescriptorElements != nullptr && secondDescriptorElements != nullptr);

	// the following code uses the following SSE instructions, and thus needs SSE4.1 or higher

	// SSE2:
	// _mm_sub_epi32
	// _mm_add_epi32

	// SSE3:
	// _mm_lddqu_si128

	// SSE4.1:
	// _mm_mullo_epi32

	// the individual descriptor values come in the range [-4000, 4000]
	// thus, we simply can multiply signed integers as the result will be within a signed 32 bit integer:
	// 4000^2 * 64 = 1,024,000,000

	__m128i first = _mm_lddqu_si128((const __m128i*)firstDescriptorElements + 5);
	__m128i second = _mm_lddqu_si128((const __m128i*)secondDescriptorElements + 5);
	__m128i difference = _mm_sub_epi32(first, second);
	__m128i result = _mm_mullo_epi32(difference, difference);

	first = _mm_lddqu_si128((const __m128i*)firstDescriptorElements + 6);
	second = _mm_lddqu_si128((const __m128i*)secondDescriptorElements + 6);
	difference = _mm_sub_epi32(first, second);
	result = _mm_add_epi32(result, _mm_mullo_epi32(difference, difference));

	first = _mm_lddqu_si128((const __m128i*)firstDescriptorElements + 9);
	second = _mm_lddqu_si128((const __m128i*)secondDescriptorElements + 9);
	difference = _mm_sub_epi32(first, second);
	result = _mm_add_epi32(result, _mm_mullo_epi32(difference, difference));

	first = _mm_lddqu_si128((const __m128i*)firstDescriptorElements + 10);
	second = _mm_lddqu_si128((const __m128i*)secondDescriptorElements + 10);
	difference = _mm_sub_epi32(first, second);
	result = _mm_add_epi32(result, _mm_mullo_epi32(difference, difference));

	// the inner descriptor kernel holds about 81% of the descriptor energy, the outer ring holds about 19%
	// thus we add additional 5% to increase the probability for a correct decision and check for 86%
	if (SSE::sum_u32_4(result) > threshold * 110u / 128u) // 110/128 = 0.859375
		return false;

	for (unsigned int n = 0u; n <= 4u; ++n)
	{
		first = _mm_lddqu_si128((const __m128i*)firstDescriptorElements + n);
		second = _mm_lddqu_si128((const __m128i*)secondDescriptorElements + n);
		difference = _mm_sub_epi32(first, second);
		result = _mm_add_epi32(result, _mm_mullo_epi32(difference, difference));
	}

	for (unsigned int n = 7u; n <= 8u; ++n)
	{
		first = _mm_lddqu_si128((const __m128i*)firstDescriptorElements + n);
		second = _mm_lddqu_si128((const __m128i*)secondDescriptorElements + n);
		difference = _mm_sub_epi32(first, second);
		result = _mm_add_epi32(result, _mm_mullo_epi32(difference, difference));
	}

	for (unsigned int n = 11u; n <= 15u; ++n)
	{
		first = _mm_lddqu_si128((const __m128i*)firstDescriptorElements + n);
		second = _mm_lddqu_si128((const __m128i*)secondDescriptorElements + n);
		difference = _mm_sub_epi32(first, second);
		result = _mm_add_epi32(result, _mm_mullo_epi32(difference, difference));
	}

	ssd = SSE::sum_u32_4(result);

	return ssd <= threshold;
}

template <>
OCEAN_FORCE_INLINE bool BlobDescriptor::isDescriptorEqualEarlyRejectSSE<float, 36u>(const float* firstDescriptorElements, const float* secondDescriptorElements, const float threshold, float& ssd)
{
	ocean_assert(firstDescriptorElements != nullptr && secondDescriptorElements != nullptr);

	const float middleSSD = sqr(firstDescriptorElements[16] - secondDescriptorElements[16]) + sqr(firstDescriptorElements[17] - secondDescriptorElements[17])
										+ sqr(firstDescriptorElements[18] - secondDescriptorElements[18]) + sqr(firstDescriptorElements[19] - secondDescriptorElements[19]);

	// the inner descriptor kernel holds about 65% of the descriptor energy, the outer ring holds about 35%
	// thus we add additional 5% to increase the probability for a correct decision and check for 70%
	if (middleSSD > threshold * 0.70f)
		return false;

	__m128 first = _mm_loadu_ps(firstDescriptorElements + 0);
	__m128 second = _mm_loadu_ps(secondDescriptorElements + 0);
	__m128 difference = _mm_sub_ps(first, second);
	__m128 result = _mm_mul_ps(difference, difference);

	for (unsigned int n = 4u; n <= 12u; n += 4)
	{
		first = _mm_loadu_ps(firstDescriptorElements + n);
		second = _mm_loadu_ps(secondDescriptorElements + n);
		difference = _mm_sub_ps(first, second);
		result = _mm_add_ps(result, _mm_mul_ps(difference, difference));
	}

	for (unsigned int n = 20u; n <= 32u; n += 4)
	{
		first = _mm_loadu_ps(firstDescriptorElements + n);
		second = _mm_loadu_ps(secondDescriptorElements + n);
		difference = _mm_sub_ps(first, second);
		result = _mm_add_ps(result, _mm_mul_ps(difference, difference));
	}

	ssd = middleSSD + SSE::sum_f32_4(result);

	return ssd <= threshold;
}

template <>
OCEAN_FORCE_INLINE bool BlobDescriptor::isDescriptorEqualEarlyRejectSSE<double, 36u>(const double* firstDescriptorElements, const double* secondDescriptorElements, const double threshold, double& ssd)
{
	ocean_assert(firstDescriptorElements != nullptr && secondDescriptorElements != nullptr);

	const double middleSSD = sqr(firstDescriptorElements[16] - secondDescriptorElements[16]) + sqr(firstDescriptorElements[17] - secondDescriptorElements[17])
										+ sqr(firstDescriptorElements[18] - secondDescriptorElements[18]) + sqr(firstDescriptorElements[19] - secondDescriptorElements[19]);

	// the inner descriptor kernel holds about 65% of the descriptor energy, the outer ring holds about 35%
	// thus we add additional 5% to increase the probability for a correct decision and check for 70%
	if (middleSSD > threshold * 0.70)
		return false;

	__m128d first = _mm_loadu_pd(firstDescriptorElements + 0);
	__m128d second = _mm_loadu_pd(secondDescriptorElements + 0);
	__m128d difference = _mm_sub_pd(first, second);
	__m128d result = _mm_mul_pd(difference, difference);

	for (unsigned int n = 2u; n <= 14u; n += 2u)
	{
		first = _mm_loadu_pd(firstDescriptorElements + n);
		second = _mm_loadu_pd(secondDescriptorElements + n);
		difference = _mm_sub_pd(first, second);
		result = _mm_add_pd(result, _mm_mul_pd(difference, difference));
	}

	for (unsigned int n = 20u; n <= 34u; n += 2u)
	{
		first = _mm_loadu_pd(firstDescriptorElements + n);
		second = _mm_loadu_pd(secondDescriptorElements + n);
		difference = _mm_sub_pd(first, second);
		result = _mm_add_pd(result, _mm_mul_pd(difference, difference));
	}

	ssd = middleSSD + SSE::sum_f64_2(result);

	return ssd <= threshold;
}

template <>
OCEAN_FORCE_INLINE bool BlobDescriptor::isDescriptorEqualEarlyRejectSSE<float, 64u>(const float* firstDescriptorElements, const float* secondDescriptorElements, const float threshold, float& ssd)
{
	ocean_assert(firstDescriptorElements != nullptr && secondDescriptorElements != nullptr);

	__m128 first = _mm_loadu_ps(firstDescriptorElements + 20);
	__m128 second = _mm_loadu_ps(secondDescriptorElements + 20);
	__m128 difference = _mm_sub_ps(first, second);
	__m128 result = _mm_mul_ps(difference, difference);

	first = _mm_loadu_ps(firstDescriptorElements + 24);
	second = _mm_loadu_ps(secondDescriptorElements + 24);
	difference = _mm_sub_ps(first, second);
	result = _mm_add_ps(result, _mm_mul_ps(difference, difference));

	first = _mm_loadu_ps(firstDescriptorElements + 36);
	second = _mm_loadu_ps(secondDescriptorElements + 36);
	difference = _mm_sub_ps(first, second);
	result = _mm_add_ps(result, _mm_mul_ps(difference, difference));

	first = _mm_loadu_ps(firstDescriptorElements + 40);
	second = _mm_loadu_ps(secondDescriptorElements + 40);
	difference = _mm_sub_ps(first, second);
	result = _mm_add_ps(result, _mm_mul_ps(difference, difference));

	// the inner descriptor kernel holds about 81% of the descriptor energy, the outer ring holds about 19%
	// thus we add additional 5% to increase the probability for a correct decision and check for 86%
	if (SSE::sum_f32_4(result) > 0.86f * threshold)
		return false;

	for (unsigned int n = 0u; n <= 16u; n += 4)
	{
		first = _mm_loadu_ps(firstDescriptorElements + n);
		second = _mm_loadu_ps(secondDescriptorElements + n);
		difference = _mm_sub_ps(first, second);
		result = _mm_add_ps(result, _mm_mul_ps(difference, difference));
	}

	for (unsigned int n = 28u; n <= 32u; n += 4)
	{
		first = _mm_loadu_ps(firstDescriptorElements + n);
		second = _mm_loadu_ps(secondDescriptorElements + n);
		difference = _mm_sub_ps(first, second);
		result = _mm_add_ps(result, _mm_mul_ps(difference, difference));
	}

	for (unsigned int n = 44u; n <= 60u; n += 4)
	{
		first = _mm_loadu_ps(firstDescriptorElements + n);
		second = _mm_loadu_ps(secondDescriptorElements + n);
		difference = _mm_sub_ps(first, second);
		result = _mm_add_ps(result, _mm_mul_ps(difference, difference));
	}

	ssd = SSE::sum_f32_4(result);

	return ssd <= threshold;
}

template <>
OCEAN_FORCE_INLINE bool BlobDescriptor::isDescriptorEqualEarlyRejectSSE<double, 64u>(const double* firstDescriptorElements, const double* secondDescriptorElements, const double threshold, double& ssd)
{
	ocean_assert(firstDescriptorElements != nullptr && secondDescriptorElements != nullptr);

	__m128d first = _mm_loadu_pd(firstDescriptorElements + 20);
	__m128d second = _mm_loadu_pd(secondDescriptorElements + 20);
	__m128d difference = _mm_sub_pd(first, second);
	__m128d result = _mm_mul_pd(difference, difference);

	for (unsigned int n = 22u; n <= 26u; n += 2u)
	{
		first = _mm_loadu_pd(firstDescriptorElements + n);
		second = _mm_loadu_pd(secondDescriptorElements + n);
		difference = _mm_sub_pd(first, second);
		result = _mm_add_pd(result, _mm_mul_pd(difference, difference));
	}

	for (unsigned int n = 36u; n <= 42u; n += 2u)
	{
		first = _mm_loadu_pd(firstDescriptorElements + n);
		second = _mm_loadu_pd(secondDescriptorElements + n);
		difference = _mm_sub_pd(first, second);
		result = _mm_add_pd(result, _mm_mul_pd(difference, difference));
	}

	// the inner descriptor kernel holds about 81% of the descriptor energy, the outer ring holds about 19%
	// thus we add additional 5% to increase the probability for a correct decision and check for 86%
	if (SSE::sum_f64_2(result) > 0.86 * threshold)
		return false;

	for (unsigned int n = 0u; n <= 18u; n += 2u)
	{
		first = _mm_loadu_pd(firstDescriptorElements + n);
		second = _mm_loadu_pd(secondDescriptorElements + n);
		difference = _mm_sub_pd(first, second);
		result = _mm_add_pd(result, _mm_mul_pd(difference, difference));
	}

	for (unsigned int n = 28u; n <= 34u; n += 2u)
	{
		first = _mm_loadu_pd(firstDescriptorElements + n);
		second = _mm_loadu_pd(secondDescriptorElements + n);
		difference = _mm_sub_pd(first, second);
		result = _mm_add_pd(result, _mm_mul_pd(difference, difference));
	}

	for (unsigned int n = 44u; n <= 62u; n += 2u)
	{
		first = _mm_loadu_pd(firstDescriptorElements + n);
		second = _mm_loadu_pd(secondDescriptorElements + n);
		difference = _mm_sub_pd(first, second);
		result = _mm_add_pd(result, _mm_mul_pd(difference, difference));
	}

	ssd = SSE::sum_f64_2(result);

	return ssd <= threshold;
}

#endif // OCEAN_HARDWARE_SSE_VERSION >= 41

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

template <>
OCEAN_FORCE_INLINE bool BlobDescriptor::isDescriptorEqualEarlyRejectNEON<int, 36u>(const int* firstDescriptorElements, const int* secondDescriptorElements, const unsigned int threshold, unsigned int& ssd)
{
	ocean_assert(firstDescriptorElements != nullptr && secondDescriptorElements != nullptr);

	// we do not have a performance boost for 36 integer elements applying the early reject strategy, thus we simply determine the entire SSD instead

	return isDescriptorEqualEarlyRejectFallback<int, 36u>(firstDescriptorElements, secondDescriptorElements, threshold, ssd);
}

template <>
OCEAN_FORCE_INLINE bool BlobDescriptor::isDescriptorEqualEarlyRejectNEON<float, 36u>(const float* firstDescriptorElements, const float* secondDescriptorElements, const float threshold, float& ssd)
{
	ocean_assert(firstDescriptorElements != nullptr && secondDescriptorElements != nullptr);

	// we do not have a significant performance boost for 36 elements using NEON
	// thus we apply a compromise - we first determine the first half of the SSD and check whether we can stop early
	// if not we simply determine the remaining half

	float32x4_t first = vld1q_f32(firstDescriptorElements + 0);
	float32x4_t second = vld1q_f32(secondDescriptorElements + 0);
	float32x4_t difference = vsubq_f32(first, second);
	float32x4_t result = vmulq_f32(difference, difference);

	for (unsigned int n = 4u; n <= 16u; n += 4u)
	{
		first = vld1q_f32(firstDescriptorElements + n);
		second = vld1q_f32(secondDescriptorElements + n);
		difference = vsubq_f32(first, second);
		result = vmlaq_f32(result, difference, difference);
	}

	// the inner descriptor kernel holds about 65% of the descriptor energy, the outer ring holds about 35%
	// as we have determined half of the outer ring and the inner kernel we have seen 82.5%
	if (vgetq_lane_f32(result, 0) + vgetq_lane_f32(result, 1) + vgetq_lane_f32(result, 2) + vgetq_lane_f32(result, 3) > 0.875 * threshold)
		return false;

	for (unsigned int n = 20u; n <= 32u; n += 4u)
	{
		first = vld1q_f32(firstDescriptorElements + n);
		second = vld1q_f32(secondDescriptorElements + n);
		difference = vsubq_f32(first, second);
		result = vmlaq_f32(result, difference, difference);
	}

	ssd = vgetq_lane_f32(result, 0) + vgetq_lane_f32(result, 1) + vgetq_lane_f32(result, 2) + vgetq_lane_f32(result, 3);

	return ssd <= threshold;
}

template <>
OCEAN_FORCE_INLINE bool BlobDescriptor::isDescriptorEqualEarlyRejectNEON<double, 36u>(const double* firstDescriptorElements, const double* secondDescriptorElements, const double threshold, double& ssd)
{
	ocean_assert(firstDescriptorElements != nullptr && secondDescriptorElements != nullptr);

	// Not optimized NEON version as NEON 1.0 does not support 64 bit floating point values
	return isDescriptorEqualEarlyRejectFallback<double, 36u>(firstDescriptorElements, secondDescriptorElements, threshold, ssd);
}

template <>
OCEAN_FORCE_INLINE bool BlobDescriptor::isDescriptorEqualEarlyRejectNEON<int, 64u>(const int* firstDescriptorElements, const int* secondDescriptorElements, const unsigned int threshold, unsigned int& ssd)
{
	ocean_assert(firstDescriptorElements != nullptr && secondDescriptorElements != nullptr);

	// first the inner kernel

	// we determine the square distance for four 32 bit integers concurrently
	// and keep the result in NEON registers until we reach the last element
	// (as summing up the four 32 bit integers is very expensive)

	// loading the first pair of four 32 bit integers
	int32x4_t first = vld1q_s32(firstDescriptorElements + 20);             // [first3,  first2,  first1,  first0 ]
	int32x4_t second = vld1q_s32(secondDescriptorElements + 20); // [second3, second2, second1, second0]
	// determining the individual differences
	int32x4_t difference = vsubq_s32(first, second);                 // [first3 - second3, first2 - second2, ... ]
	// calculating the square differences
	uint32x4_t result = vreinterpretq_u32_s32(vmulq_s32(difference, difference)); // [(first3 - second3)^2, (first2 - second2)^2, ... ]

	// loading the next pair of four 32 bit floats, and so on
	first = vld1q_s32(firstDescriptorElements + 24);
	second = vld1q_s32(secondDescriptorElements + 24);
	difference = vsubq_s32(first, second);
	// calculating the square differences and adding them directly to the existing onces
	result = vaddq_u32(result, vreinterpretq_u32_s32(vmulq_s32(difference, difference)));

	first = vld1q_s32(firstDescriptorElements + 36);
	second = vld1q_s32(secondDescriptorElements + 36);
	difference = vsubq_s32(first, second);
	result = vaddq_u32(result, vreinterpretq_u32_s32(vmulq_s32(difference, difference)));

	first = vld1q_s32(firstDescriptorElements + 40);
	second = vld1q_s32(secondDescriptorElements + 40);
	difference = vsubq_s32(first, second);
	result = vaddq_u32(result, vreinterpretq_u32_s32(vmulq_s32(difference, difference)));

	const unsigned int middleSSD = vgetq_lane_u32(result, 0) + vgetq_lane_u32(result, 1) + vgetq_lane_u32(result, 2) + vgetq_lane_u32(result, 3);

	// the inner descriptor kernel holds about 81% of the descriptor energy, the outer ring holds about 19%
	if (middleSSD > threshold * 110u / 128u) // 0.86
		return false;

	first = vld1q_s32(firstDescriptorElements + 0);
	second = vld1q_s32(secondDescriptorElements + 0);
	difference = vsubq_s32(first, second);
	result = vreinterpretq_u32_s32(vmulq_s32(difference, difference));

	for (unsigned int n = 4u; n <= 16u; n += 4u)
	{
		first = vld1q_s32(firstDescriptorElements + n);
		second = vld1q_s32(secondDescriptorElements + n);
		difference = vsubq_s32(first, second);
		result = vaddq_u32(result, vreinterpretq_u32_s32(vmulq_s32(difference, difference)));
	}

	first = vld1q_s32(firstDescriptorElements + 28);
	second = vld1q_s32(secondDescriptorElements + 28);
	difference = vsubq_s32(first, second);
	result = vaddq_u32(result, vreinterpretq_u32_s32(vmulq_s32(difference, difference)));

	first = vld1q_s32(firstDescriptorElements + 32);
	second = vld1q_s32(secondDescriptorElements + 32);
	difference = vsubq_s32(first, second);
	result = vaddq_u32(result, vreinterpretq_u32_s32(vmulq_s32(difference, difference)));

	for (unsigned int n = 44u; n <= 60u; n += 4u)
	{
		first = vld1q_s32(firstDescriptorElements + n);
		second = vld1q_s32(secondDescriptorElements + n);
		difference = vsubq_s32(first, second);
		result = vaddq_u32(result, vreinterpretq_u32_s32(vmulq_s32(difference, difference)));
	}

	ssd = middleSSD + vgetq_lane_u32(result, 0) + vgetq_lane_u32(result, 1) + vgetq_lane_u32(result, 2) + vgetq_lane_u32(result, 3);

	return ssd <= threshold;
}

template <>
OCEAN_FORCE_INLINE bool BlobDescriptor::isDescriptorEqualEarlyRejectNEON<float, 64u>(const float* firstDescriptorElements, const float* secondDescriptorElements, const float threshold, float& ssd)
{
	ocean_assert(firstDescriptorElements != nullptr && secondDescriptorElements != nullptr);

	// first the inner kernel

	// we determine the square distance for four 32 bit floats concurrently
	// and keep the result in NEON registers until we reach the last element
	// (as summing up the four 32 bit floats is very expensive)

	// loading the first pair of four 32 bit floats
	float32x4_t first = vld1q_f32(firstDescriptorElements + 20);             // [first3,  first2,  first1,  first0 ]
	float32x4_t second = vld1q_f32(secondDescriptorElements + 20); // [second3, second2, second1, second0]
	// determining the individual differences
	float32x4_t difference = vsubq_f32(first, second);                 // [first3 - second3, first2 - second2, ... ]
	// calculating the square differences
	float32x4_t result = vmulq_f32(difference, difference);            // [(first3 - second3)^2, (first2 - second2)^2, ... ]

	// loading the next pair of four 32 bit floats, and so on
	first = vld1q_f32(firstDescriptorElements + 24);
	second = vld1q_f32(secondDescriptorElements + 24);
	difference = vsubq_f32(first, second);
	// calculating the square differences and adding them directly to the existing onces
	result = vmlaq_f32(result, difference, difference);

	first = vld1q_f32(firstDescriptorElements + 36);
	second = vld1q_f32(secondDescriptorElements + 36);
	difference = vsubq_f32(first, second);
	result = vmlaq_f32(result, difference, difference);

	first = vld1q_f32(firstDescriptorElements + 40);
	second = vld1q_f32(secondDescriptorElements + 40);
	difference = vsubq_f32(first, second);
	result = vmlaq_f32(result, difference, difference);

	// the inner descriptor kernel holds about 81% of the descriptor energy, the outer ring holds about 19%
	if (vgetq_lane_f32(result, 0) + vgetq_lane_f32(result, 1) + vgetq_lane_f32(result, 2) + vgetq_lane_f32(result, 3) > 0.86 * threshold)
		return false;

	first = vld1q_f32(firstDescriptorElements + 0);
	second = vld1q_f32(secondDescriptorElements + 0);
	difference = vsubq_f32(first, second);
	result = vmlaq_f32(result, difference, difference);

	for (unsigned int n = 4u; n <= 16u; n += 4u)
	{
		first = vld1q_f32(firstDescriptorElements + n);
		second = vld1q_f32(secondDescriptorElements + n);
		difference = vsubq_f32(first, second);
		result = vmlaq_f32(result, difference, difference);
	}

	first = vld1q_f32(firstDescriptorElements + 28);
	second = vld1q_f32(secondDescriptorElements + 28);
	difference = vsubq_f32(first, second);
	result = vmlaq_f32(result, difference, difference);

	first = vld1q_f32(firstDescriptorElements + 32);
	second = vld1q_f32(secondDescriptorElements + 32);
	difference = vsubq_f32(first, second);
	result = vmlaq_f32(result, difference, difference);

	for (unsigned int n = 44u; n <= 60u; n += 4u)
	{
		first = vld1q_f32(firstDescriptorElements + n);
		second = vld1q_f32(secondDescriptorElements + n);
		difference = vsubq_f32(first, second);
		result = vmlaq_f32(result, difference, difference);
	}

	ssd = vgetq_lane_f32(result, 0) + vgetq_lane_f32(result, 1) + vgetq_lane_f32(result, 2) + vgetq_lane_f32(result, 3);

	return ssd <= threshold;
}

template <>
OCEAN_FORCE_INLINE bool BlobDescriptor::isDescriptorEqualEarlyRejectNEON<double, 64u>(const double* firstDescriptorElements, const double* secondDescriptorElements, const double threshold, double& ssd)
{
	ocean_assert(firstDescriptorElements != nullptr && secondDescriptorElements != nullptr);

	// Not optimized NEON version as NEON 1.0 does not support 64 bit floating point values
	return isDescriptorEqualEarlyRejectFallback<double, 64u>(firstDescriptorElements, secondDescriptorElements, threshold, ssd);
}

#endif // OCEAN_HARDWARE_NEON_VERSION

inline bool BlobDescriptor::isDescriptorEqualEarlyReject(const BlobDescriptor& secondDescriptor, const DescriptorSSD threshold, DescriptorSSD& ssd) const
{
#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

	return isDescriptorEqualEarlyRejectSSE<DescriptorElement, definedDescriptorElements>(featureDescriptor, secondDescriptor.featureDescriptor, threshold, ssd);

#elif defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	return isDescriptorEqualEarlyRejectNEON<DescriptorElement, definedDescriptorElements>(featureDescriptor, secondDescriptor.featureDescriptor, threshold, ssd);

#else

	return isDescriptorEqualEarlyRejectFallback<DescriptorElement, definedDescriptorElements>(featureDescriptor, secondDescriptor.featureDescriptor, threshold, ssd);

#endif
}

template <typename T>
OCEAN_FORCE_INLINE T BlobDescriptor::sqr(const T value)
{
	return value * value;
}

}

}

}

}

#endif // META_OCEAN_CV_DETECTOR_BLOB_DESCRIPTOR_H
