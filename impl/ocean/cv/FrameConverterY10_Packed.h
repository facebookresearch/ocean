/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_CONVERTER_Y_10_PACKED_H
#define META_OCEAN_CV_FRAME_CONVERTER_Y_10_PACKED_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameChannels.h"
#include "ocean/cv/FrameConverter.h"

#include "ocean/base/Memory.h"
#include "ocean/base/Singleton.h"
#include "ocean/base/Worker.h"

#include <unordered_map>

namespace Ocean
{

namespace CV
{

/**
 * This class provides functions to convert frames with Y10_PACKED pixel format.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameConverterY10_Packed : public FrameConverter
{
	public:

		/**
		 * This class implements the manager for lookup tables.
		 */
		class LookupTableManager : public Singleton<LookupTableManager>
		{
			protected:

				/// Definition of a map mapping gamma values to the memory of lookup tables.
				typedef std::unordered_map<float, Memory> LookupTables;

			public:

				/**
				 * Returns the lookup table for a gamma compression/correction function.
				 * The gamma compression/correction is based the following equation
				 * <pre>
				 * Y8 = 255 * (Y10 / 1023) ^ gamma
				 * </pre>
				 * @param gamma The gamma value for which the lookup table will be returned, with range (0, 2)
				 * @return The requested lookup table, will be valid
				 */
				const uint8_t* lookupTable(const float gamma);

			protected:

				/// The lookup tables.
				LookupTables lookupTables_;

				/// The lock of the manager.
				Lock lock_;
		};

	public:

		/**
		 * Converts a Y10_PACKED frame to a Y8 frame.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [4, infinity), must be a multiple of 4
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 */
		static inline void convertY10_PackedToY8Linear(const uint8_t* const source, uint8_t* const target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a Y10_PACKED frame to a Y8 frame by applying gamma compression/correction using a lookup table.
		 * The gamma compression/correction is based the following equation
		 * <pre>
		 * Y8 = 255 * (Y10 / 1023) ^ gamma
		 * </pre>
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [4, infinity), must be a multiple of 4
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param gamma The gamma value to be applied, with range (0, 2)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 */
		static inline void convertY10_PackedToY8GammaLUT(const uint8_t* const source, uint8_t* const target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const float gamma, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a Y10_PACKED frame to a Y8 frame by applying gamma compression/correction using a 3-step linear approximation.
		 * The gamma compression/correction is based the following equation
		 * <pre>
		 * Y8 = 255 * (Y10 / 1023) ^ gamma
		 * </pre>
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [4, infinity), must be a multiple of 4
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param gamma The gamma value to be applied, with range (0, 2)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 */
		static void convertY10_PackedToY8GammaApproximated(const uint8_t* const source, uint8_t* const target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const float gamma, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a Y10_PACKED frame to a Y10 frame, so that this function simply unpacks the 10 bits.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [4, infinity), must be a multiple of 4
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 */
		static inline void convertY10_PackedToY10(const uint8_t* const source, uint16_t* const target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a Y10_PACKED frame to a RGB24 frame.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [4, infinity), must be a multiple of 4
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 */
		static inline void convertY10_PackedToBGR24(const uint8_t* const source, uint8_t* const target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a Y10_PACKED frame to a RGB24 frame.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [4, infinity), must be a multiple of 4
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 */
		static inline void convertY10_PackedToRGB24(const uint8_t* const source, uint8_t* const target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

	protected:

		/**
		 * Converts a Y10_Packed row to a Y8 row.
		 * This function simply applies a linear bit reduction from 10 bits to 8 bits.
		 * @param source The pointer to the source pixels, must be valid
		 * @param target The pointer to the target pixels receiving the converted pixel data, must be valid
		 * @param size The number of source (and target pixels) to convert, with range [4, infinity), must be a multiple of 4
		 * @param unusedParameters Unused parameters, must be nullptr
		 */
		static void convertRowY10_PackedToY8Linear(const uint8_t* source, uint8_t* target, const size_t size, const void* unusedParameters = nullptr);

		/**
		 * Converts a Y10_Packed row to a Y8 row by applying gamma compression/correction with a lookup table.
		 * @param source The pointer to the source pixels, must be valid
		 * @param target The pointer to the target pixels receiving the converted pixel data, must be valid
		 * @param size The number of source (and target pixels) to convert, with range [4, infinity), must be a multiple of 4
		 * @param parameters The pointer to the `uint8_t` lookup table to be used, must be valid
		 */
		static void convertRowY10_PackedToY8GammaLUT(const uint8_t* source, uint8_t* target, const size_t size, const void* parameters);

		/**
		 * Converts a Y10_Packed row to a Y8 row by applying gamma compression/correction with a 3-step linear interpolation.
		 * @param source The pointer to the source pixels, must be valid
		 * @param target The pointer to the target pixels receiving the converted pixel data, must be valid
		 * @param size The number of source (and target pixels) to convert, with range [4, infinity), must be a multiple of 4
		 * @param parameters Three linear slope parameters and two intercept parameters, must be valid
		 */
		template <unsigned int tStep01, unsigned int tStep12>
		static void convertRowY10_PackedToY8GammaApproximated(const uint8_t* source, uint8_t* target, const size_t size, const void* parameters);

		/**
		 * Converts a Y10_Packed row to a Y10 row.
		 * This function simply applies an unpacking of the 10 bits.
		 * @param source The pointer to the source pixels, must be valid
		 * @param target The pointer to the target pixels receiving the converted pixel data, must be valid
		 * @param size The number of source (and target pixels) to convert, with range [4, infinity), must be a multiple of 4
		 * @param unusedParameters Unused parameters, must be nullptr
		 */
		static void convertRowY10_PackedToY10(const uint8_t* source, uint16_t* target, const size_t size, const void* unusedParameters = nullptr);

		/**
		 * Converts a Y10_Packed row to a RGB24 row, or BGR24 row.
		 * This function simply applies a linear bit reduction from 10 bits to 8 bits.
		 * @param source The pointer to the source pixels, must be valid
		 * @param target The pointer to the target pixels receiving the converted pixel data, must be valid
		 * @param size The number of source (and target pixels) to convert, with range [4, infinity), must be a multiple of 4
		 * @param unusedParameters Unused parameters, must be nullptr
		 */
		static void convertRowY10_PackedToYYY24Linear(const uint8_t* source, uint8_t* target, const size_t size, const void* unusedParameters = nullptr);

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

		/**
		 * Converts 16 pixels (20 elements) of a Y10_Packed buffer to 16 Y8 pixels by applying a liner conversion.
		 * @param source The souce buffer with 16 elements, must be valid
		 * @param target The resulting 16 Y8 pixels, must be valid
		 */
		static OCEAN_FORCE_INLINE void convert16PixelY10_PackedToY8LinearNEON(const uint8_t* const source, uint8_t* const target);

		/**
		 * Converts 16 pixels (20 elements) of a Y10_Packed buffer to 16 Y8 pixels by applying gamma compression/correction with a 3-step linear interpolation.
		 * @param source The souce buffer with 16 elements, must be valid
		 * @param m0_256_s_16x4 The slope of the first linear approximation (with multiple of 256), with range (-10 * 256, 10 * 256)
		 * @param m1_256_s_16x4 The slope of the second linear approximation (with multiple of 256), with range (-10 * 256, 10 * 256)
		 * @param m2_256_s_16x4 The slope of the third linear approximation (with multiple of 256), with range (-10 * 256, 10 * 256)
		 * @param c1_s_16x8 The intercept of the second linear approximation, with range (-255, 255)
		 * @param c2_s_16x8 The intercept of the third linear approximation, with range (-255, 255)
		 * @param target The resulting 16 Y8 pixels, must be valid
		 */
		template <unsigned int tStep01, unsigned int tStep12>
		static OCEAN_FORCE_INLINE void convert16PixelY10_PackedToY8ApproximatedNEON(const uint8_t* const source, const int16x4_t& m0_256_s_16x4, const int16x4_t& m1_256_s_16x4, const int16x4_t& m2_256_s_16x4, const int16x8_t& c1_s_16x8, const int16x8_t& c2_s_16x8, uint8_t* const target);

#endif
};

inline void FrameConverterY10_Packed::convertY10_PackedToY8Linear(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 4u && height >= 1u);
	ocean_assert(width % 4u == 0u);

	const unsigned int sourceStrideElements = width * 5u / 4u + sourcePaddingElements;
	const unsigned int targetStrideElements = width * 1u + targetPaddingElements;

	constexpr void* options = nullptr;

	const bool areContinuous = sourcePaddingElements == 0u && targetPaddingElements == 0u;

	FrameConverter::convertGenericPixelFormat(source, target, width, height, sourceStrideElements, targetStrideElements, flag, convertRowY10_PackedToY8Linear, CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 1u>, areContinuous, options, worker);
}

inline void FrameConverterY10_Packed::convertY10_PackedToY8GammaLUT(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const float gamma, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 4u && height >= 1u);
	ocean_assert(width % 4u == 0u);

	ocean_assert(gamma > 0.0f && gamma < 2.0f);

	const unsigned int sourceStrideElements = width * 5u / 4u + sourcePaddingElements;
	const unsigned int targetStrideElements = width * 1u + targetPaddingElements;

	const void* const options = LookupTableManager::get().lookupTable(gamma);

	const bool areContinuous = sourcePaddingElements == 0u && targetPaddingElements == 0u;

	FrameConverter::convertGenericPixelFormat(source, target, width, height, sourceStrideElements, targetStrideElements, flag, convertRowY10_PackedToY8GammaLUT, CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 1u>, areContinuous, options, worker);
}

inline void FrameConverterY10_Packed::convertY10_PackedToY10(const uint8_t* source, uint16_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 4u && height >= 1u);
	ocean_assert(width % 4u == 0u);

	const unsigned int sourceStrideElements = width * 5u / 4u + sourcePaddingElements;
	const unsigned int targetStrideElements = width * 1u + targetPaddingElements;

	constexpr void* options = nullptr;

	const bool areContinuous = sourcePaddingElements == 0u && targetPaddingElements == 0u;

	FrameConverter::convertGenericPixelFormat(source, target, width, height, sourceStrideElements, targetStrideElements, flag, convertRowY10_PackedToY10, CV::FrameChannels::reverseRowPixelOrderInPlace<uint16_t, 1u>, areContinuous, options, worker);
}

inline void FrameConverterY10_Packed::convertY10_PackedToBGR24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	convertY10_PackedToRGB24(source, target, width, height, flag, sourcePaddingElements, targetPaddingElements, worker);
}

inline void FrameConverterY10_Packed::convertY10_PackedToRGB24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 4u && height >= 1u);
	ocean_assert(width % 4u == 0u);

	const unsigned int sourceStrideElements = width * 5u / 4u  + sourcePaddingElements;
	const unsigned int targetStrideElements = width * 3u + targetPaddingElements;

	constexpr void* options = nullptr;

	const bool areContinuous = sourcePaddingElements == 0u && targetPaddingElements == 0u;

	FrameConverter::convertGenericPixelFormat(source, target, width, height, sourceStrideElements, targetStrideElements, flag, convertRowY10_PackedToYYY24Linear, CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>, areContinuous, options, worker);
}

template <unsigned int tStep01, unsigned int tStep12>
void FrameConverterY10_Packed::convertRowY10_PackedToY8GammaApproximated(const uint8_t* source, uint8_t* target, const size_t size, const void* parameters)
{
	static_assert(0u < tStep01 && tStep01 < tStep12 && tStep12 < 1023u, "Invalid steps");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(size >= 4 && size % 4 == 0);
	ocean_assert(parameters != nullptr);

	// applying a 3-step linear approximation
	// https://www.desmos.com/calculator/pezgk5slux

	const int* coefficients = reinterpret_cast<const int*>(parameters);

	const int32_t m0_256 = coefficients[0];
	const int32_t m1_256 = coefficients[1];
	const int32_t m2_256 = coefficients[2];

	const int32_t c1 = coefficients[3];
	const int32_t c2 = coefficients[4];

	size_t blocks4 = size / size_t(4);

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	const size_t blocks16 = size / size_t(16);

	const int16x4_t m0_256_s_16x4 = vdup_n_s16(int16_t(m0_256));
	const int16x4_t m1_256_s_16x4 = vdup_n_s16(int16_t(m1_256));
	const int16x4_t m2_256_s_16x4 = vdup_n_s16(int16_t(m2_256));

	const int16x8_t c1_s_16x8 = vdupq_n_s16(int16_t(c1));
	const int16x8_t c2_s_16x8 = vdupq_n_s16(int16_t(c2));

	for (size_t n = 0; n < blocks16; ++n)
	{
		convert16PixelY10_PackedToY8ApproximatedNEON<tStep01, tStep12>(source, m0_256_s_16x4, m1_256_s_16x4, m2_256_s_16x4, c1_s_16x8, c2_s_16x8, target);

		target += 16;
		source += 20;
	}

	blocks4 = (size - blocks16 * size_t(16)) / size_t(4);
	ocean_assert(blocks4 <= size / size_t(4));

#endif // OCEAN_HARDWARE_NEON_VERSION

	int32_t result256;

	const int32_t c1_256 = c1 * 256;
	const int32_t c2_256 = c2 * 256;

	for (size_t n = 0; n < blocks4; ++n)
	{
		const int32_t x[4] =
		{
			int32_t(uint16_t(source[0]) << uint16_t(2) | (uint16_t(source[4]) & uint16_t(0b00000011))),
			int32_t(uint16_t(source[1]) << uint16_t(2) | ((uint16_t(source[4]) & uint16_t(0b00001100)) >> uint16_t(2))),
			int32_t(uint16_t(source[2]) << uint16_t(2) | ((uint16_t(source[4]) & uint16_t(0b00110000)) >> uint16_t(4))),
			int32_t(uint16_t(source[3]) << uint16_t(2) | (uint16_t(source[4]) >> uint16_t(6)))
		};

		for (unsigned int i = 0u; i < 4u; ++i)
		{
			const uint32_t& xx = x[i];

			if (xx < tStep01)
			{
				result256 = (m0_256 * xx);
			}
			else if (xx <= tStep12)
			{
				result256 = (m1_256 * xx + c1_256);
			}
			else
			{
				result256 = (m2_256 * xx + c2_256);
			}

			ocean_assert(0 <= result256 && result256 <= 255 * 256);

			target[i] = int8_t((uint32_t(result256) + 128u) >> 8u);
		}

		target += 4;
		source += 5;
	}
}

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

OCEAN_FORCE_INLINE void FrameConverterY10_Packed::convert16PixelY10_PackedToY8LinearNEON(const uint8_t* const source, uint8_t* const target)
{
#ifdef __aarch64__

	const uint8x16_t packedA_u_8x16 = vld1q_u8(source);
	const uint8x8_t packedB_u_8x8 = vld1_u8(source + 12);

	// F E D C B A 9 8 7 6 5 4 3 2 1 0
	// D C B A 8 7 6 5 3 2 1 0 X X X X
	constexpr uint8x16_t shuffle_u_8x16 = {16u, 16u, 16u, 16u, 0u, 1u, 2u, 3u, 5u, 6u, 7u, 8u, 10u, 11u, 12u, 13u};
	const uint8x16_t intermediateA_u_8x16 = vqtbl1q_u8(packedA_u_8x16, shuffle_u_8x16);

	const uint8x8_t intermediateB_u_8x8 = vext_u8(packedB_u_8x8, packedB_u_8x8, 3);

	const uint8x16_t target_u_8x16 = vextq_u8(intermediateA_u_8x16, vcombine_u8(intermediateB_u_8x8, intermediateB_u_8x8), 4);

#else

	constexpr uint8x16_t mask_u_8x16 = {0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0xFFu, 0xFFu, 0xFFu};

	const uint8x16_t packedA_u_8x16 = vld1q_u8(source);
	const uint8x8_t packedB_u_8x8 = vld1_u8(source + 11);

	const uint8x8_t packedAA_u_8x8 = vget_low_u8(packedA_u_8x16);
	const uint8x8_t packedAB_u_8x8 = vget_high_u8(packedA_u_8x16);

	constexpr uint8x8_t shuffleA_u_8x8 = {8u, 0u, 1u, 2u, 3u, 5u, 6u, 7u};
	constexpr uint8x8_t shuffleB_u_8x8 = {0u, 2u, 3u, 4u, 5u, 7u, 8u, 8u};
	const uint8x16_t intermediateA_u_8x16 = vextq_u8(vcombine_u8(vtbl1_u8(packedAA_u_8x8, shuffleA_u_8x8), vtbl1_u8(packedAB_u_8x8, shuffleB_u_8x8)), mask_u_8x16, 1); // we use the first zero element of mask_u_8x16

	const uint8x16_t intermediateB_u_8x16 = vcombine_u8(vget_low_u8(mask_u_8x16), vand_u8(packedB_u_8x8, vget_high_u8(mask_u_8x16)));

	const uint8x16_t target_u_8x16 = vorrq_u8(intermediateA_u_8x16, intermediateB_u_8x16);

#endif // __aarch64__

	vst1q_u8(target, target_u_8x16);
}

template <unsigned int tStep01, unsigned int tStep12>
OCEAN_FORCE_INLINE void FrameConverterY10_Packed::convert16PixelY10_PackedToY8ApproximatedNEON(const uint8_t* const source, const int16x4_t& m0_s_16x4, const int16x4_t& m1_s_16x4, const int16x4_t& m2_s_16x4, const int16x8_t& c1_s_16x8, const int16x8_t& c2_s_16x8, uint8_t* const target)
{
	static_assert(0u < tStep01 && tStep01 < tStep12 && tStep12 < 1023u, "Invalid steps");

	constexpr int8x16_t leftShifts_s_8x16 = {6, 0, 4, 0, 2, 0, 0, 0, 6, 0, 4, 0, 2, 0, 0, 0};
	constexpr int16x8_t rightShifts_s_16x8 = {-6, -6, -6, -6, -6, -6, -6, -6};

#ifdef __aarch64__

	const uint8x16_t packedAB_u_8x16 = vld1q_u8(source);
	const uint8x16_t packedCD_u_8x16 = vld1q_u8(source + 4);

	// F E D C B A 9 8 7 6 5 4 3 2 1 0
	// 8 9 7 9 6 9 5 9 3 4 2 4 1 4 0 4
	constexpr uint8x16_t shuffleAB_u_8x16 = {4u, 0u, 4u, 1u, 4u, 2u, 4u, 3u, 9u, 5u, 9u, 6u, 9u, 7u, 9u, 8u};
	const uint8x16_t intermediateAB_u_8x16 = vqtbl1q_u8(packedAB_u_8x16, shuffleAB_u_8x16);

	constexpr uint8x16_t shuffleCD_u_8x16 = {10u, 6u, 10u, 7u, 10u, 8u, 10u, 9u, 15u, 11u, 15u, 12u, 15u, 13u, 15u, 14u};
	const uint8x16_t intermediateCD_u_8x16 = vqtbl1q_u8(packedCD_u_8x16, shuffleCD_u_8x16);

#else

	constexpr uint8x8_t shuffleAB_u_8x8 = {4u, 0u, 4u, 1u, 4u, 2u, 4u, 3u};
	constexpr uint8x8_t shuffleC_u_8x8 = {6u, 2u, 6u, 3u, 6u, 4u, 6u, 5u};
	constexpr uint8x8_t shuffleD_u_8x8 = {7u, 3u, 7u, 4u, 7u, 5u, 7u, 6u};

	const uint8x16_t packedAB_u_8x16 = vld1q_u8(source);
	const uint8x8_t packedForD_u_8x8 = vld1_u8(source + 12);

	const uint8x8_t packedForA_u_8x8 = vget_low_u8(packedAB_u_8x16);
	const uint8x8_t packedForB_u_8x8 = vget_low_u8(vextq_u8(packedAB_u_8x16, packedAB_u_8x16, 5));
	const uint8x8_t packedForC_u_8x8 = vget_high_u8(packedAB_u_8x16);

	const uint8x16_t intermediateAB_u_8x16 = vcombine_u8(vtbl1_u8(packedForA_u_8x8, shuffleAB_u_8x8), vtbl1_u8(packedForB_u_8x8, shuffleAB_u_8x8));
	const uint8x16_t intermediateCD_u_8x16 = vcombine_u8(vtbl1_u8(packedForC_u_8x8, shuffleC_u_8x8), vtbl1_u8(packedForD_u_8x8, shuffleD_u_8x8));

#endif // __aarch64__


	// ... XXXXXX99 33333333 44XXXXXX 22222222 XX44XXXX 11111111 XXXX44XX 00000000 XXXXXX44
	// ... 99------ 33333333 44------ 22222222 44------ 11111111 44------ 00000000 44------
	const uint16x8_t intermediateAB_u_16x8 = vreinterpretq_u16_u8(vshlq_u8(intermediateAB_u_8x16, leftShifts_s_8x16));
	const uint16x8_t intermediateCD_u_16x8 = vreinterpretq_u16_u8(vshlq_u8(intermediateCD_u_8x16, leftShifts_s_8x16));


	// ... 99------ 33333333 44------ 22222222 44------ 11111111 44------ 00000000 44------
	// ... 55555599 ------33 33333344 ------22 22222244 ------11 11111144 ------00 00000044
	const uint16x8_t unpackedAB_u_16x8 = vshlq_u16(intermediateAB_u_16x8, rightShifts_s_16x8);
	const uint16x8_t unpackedCD_u_16x8 = vshlq_u16(intermediateCD_u_16x8, rightShifts_s_16x8);

	// now, we have 16 uin16_t unpacked values for which we will approximate the gamma compression/correction

	// approximation via three linear equations
	// [     0, step01]:  f_0(x) = m_0 * x, with f_0(0) = 0
	// [step01, step12]:  f_1(x) = m_1 * x + c_1
	// [step21, 1     ]:  f_2(x) = m_2 * x + c_2, with f_2(1) = 1

	constexpr int16x8_t step01_s_16x8 = {int32_t(tStep01), int32_t(tStep01), int32_t(tStep01), int32_t(tStep01), int32_t(tStep01), int32_t(tStep01), int32_t(tStep01), int32_t(tStep01)};
	constexpr int16x8_t step12_s_16x8 = {int32_t(tStep12), int32_t(tStep12), int32_t(tStep12), int32_t(tStep12), int32_t(tStep12), int32_t(tStep12), int32_t(tStep12), int32_t(tStep12)};

	// determining masks to switch between one of the tree linear equations

	const uint16x8_t isWithin0AB_u_16x8 = vcleq_s16(vreinterpretq_s16_u16(unpackedAB_u_16x8), step01_s_16x8); // unpackedAB <= step01 ? 0xFFFFFFFF : 0x00000000
	const uint16x8_t isWithin0CD_u_16x8 = vcleq_s16(vreinterpretq_s16_u16(unpackedCD_u_16x8), step01_s_16x8);
	const uint8x16_t isWithin0_u_8x16 = vcombine_u8(vmovn_u16(isWithin0AB_u_16x8), vmovn_u16(isWithin0CD_u_16x8));

	const uint16x8_t isWithin2AB_u_16x8 = vcgtq_s16(vreinterpretq_s16_u16(unpackedAB_u_16x8), step12_s_16x8); // unpackedAB > step12 ? 0xFFFFFFFF : 0x00000000
	const uint16x8_t isWithin2CD_u_16x8 = vcgtq_s16(vreinterpretq_s16_u16(unpackedCD_u_16x8), step12_s_16x8);
	const uint8x16_t isWithin2_u_8x16 = vcombine_u8(vmovn_u16(isWithin2AB_u_16x8), vmovn_u16(isWithin2CD_u_16x8));

	const uint8x16_t isWithin1_u_8x16 = vmvnq_u8(vorrq_u8(isWithin0_u_8x16, isWithin2_u_8x16)); // unpacked > step01 && unpacked <= step02 ? 0xFFFFFFFF : 0x00000000


	const int16x4_t unpackedA_s_16x4 = vreinterpret_s16_u16(vget_low_u8(unpackedAB_u_16x8));
	const int16x4_t unpackedB_s_16x4 = vreinterpret_s16_u16(vget_high_u8(unpackedAB_u_16x8));
	const int16x4_t unpackedC_s_16x4 = vreinterpret_s16_u16(vget_low_u8(unpackedCD_u_16x8));
	const int16x4_t unpackedD_s_16x4 = vreinterpret_s16_u16(vget_high_u8(unpackedCD_u_16x8));

	// result0 = (m0 * x) / 256)
	const uint16x8_t resultAB0_u_16x8 = vcombine_u16(vqrshrun_n_s32(vmull_s16(m0_s_16x4, unpackedA_s_16x4), 8), vqrshrun_n_s32(vmull_s16(m0_s_16x4, unpackedB_s_16x4), 8));
	const uint16x8_t resultCD0_u_16x8 = vcombine_u16(vqrshrun_n_s32(vmull_s16(m0_s_16x4, unpackedC_s_16x4), 8), vqrshrun_n_s32(vmull_s16(m0_s_16x4, unpackedD_s_16x4), 8));

	// result1 = ((m1 * x) / 256 + c1)
	const int16x8_t resultAB1_s_16x8 = vaddq_s16(c1_s_16x8, vcombine_s16(vrshrn_n_s32(vmull_s16(m1_s_16x4, unpackedA_s_16x4), 8), vrshrn_n_s32(vmull_s16(m1_s_16x4, unpackedB_s_16x4), 8)));
	const int16x8_t resultCD1_s_16x8 = vaddq_s16(c1_s_16x8, vcombine_s16(vrshrn_n_s32(vmull_s16(m1_s_16x4, unpackedC_s_16x4), 8), vrshrn_n_s32(vmull_s16(m1_s_16x4, unpackedD_s_16x4), 8)));

	// result2 = ((m2 * x) / 256 + c2)
	const int16x8_t resultAB2_s_16x8 = vaddq_s16(c2_s_16x8, vcombine_s16(vrshrn_n_s32(vmull_s16(m2_s_16x4, unpackedA_s_16x4), 8), vrshrn_n_s32(vmull_s16(m2_s_16x4, unpackedB_s_16x4), 8)));
	const int16x8_t resultCD2_s_16x8 = vaddq_s16(c2_s_16x8, vcombine_s16(vrshrn_n_s32(vmull_s16(m2_s_16x4, unpackedC_s_16x4), 8), vrshrn_n_s32(vmull_s16(m2_s_16x4, unpackedD_s_16x4), 8)));

	const uint8x16_t result0_u_8x16 = vcombine_u8(vqmovn_u16(resultAB0_u_16x8), vqmovn_u16(resultCD0_u_16x8));
	const uint8x16_t result1_u_8x16 = vcombine_u8(vqmovun_s16(resultAB1_s_16x8), vqmovun_s16(resultCD1_s_16x8));
	const uint8x16_t result2_u_8x16 = vcombine_u8(vqmovun_s16(resultAB2_s_16x8), vqmovun_s16(resultCD2_s_16x8));


	// result0 & isWithin0  |  result1 & isWithin1  |  result2 & isWithin2
	const uint8x16_t result_u_8x16 = vorrq_u8(vorrq_u8(vandq_u8(result0_u_8x16, isWithin0_u_8x16), vandq_u8(result1_u_8x16, isWithin1_u_8x16)), vandq_u8(result2_u_8x16, isWithin2_u_8x16));

	vst1q_u8(target, result_u_8x16);
}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

}

}

#endif // META_OCEAN_CV_FRAME_CONVERTER_Y_10_PACKED_H
