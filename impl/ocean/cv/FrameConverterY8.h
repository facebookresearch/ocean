/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_CONVERTER_Y_8_H
#define META_OCEAN_CV_FRAME_CONVERTER_Y_8_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameChannels.h"
#include "ocean/cv/FrameConverter.h"

#include "ocean/base/Memory.h"
#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

/**
 * This class provides functions to convert frames with Y8 pixel format.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameConverterY8 : public FrameConverter
{
	protected:

		/**
		 * This class implements the manager for lookup tables.
		 */
		class LookupTableManager : public Singleton<LookupTableManager>
		{
			protected:

				/// Definition of a map mapping gamma values to the memory of lookup tables.
				using LookupTables = std::unordered_map<float, Memory>;

			public:

				/**
				 * Returns the lookup table for a gamma compression/correction function.
				 * The gamma compression/correction is based the following equation
				 * <pre>
				 * Y8 = 255 * (Y8 / 255) ^ gamma
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
		 * Converts a Y 8 bit full range [0, 255] frame to a BGR 24 bit frame.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 */
		static inline void convertY8FullRangeToBGR24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a Y 8 bit full range [0, 255] frame to a RGB 24 bit frame.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 */
		static inline void convertY8FullRangeToRGB24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a Y 8 bit full range [0, 255] frame to a RGBA 32 bit frame.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param alphaValue The value of the alpha channel to be set, with range [0, 255]
		 * @param worker Optional worker object to distribute the computational load
		 */
		static inline void convertY8FullRangeToRGBA32(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const uint8_t alphaValue = 0xFF, Worker* worker = nullptr);

		/**
		 * Converts a Y8 limited range frame [16, 235] to an RGB24 (full range) frame [0, 255].
		 * This function first converts limited range to full range, then expands grayscale to RGB.
		 * @param source The source frame buffer with limited range values, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 * @tparam tPrecision The precision to use for range conversion: 6 for 6-bit (faster) or 10 for 10-bit (slower, higher quality)
		 */
		template <unsigned int tPrecision = 10u>
		static inline void convertY8LimitedRangeToRGB24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a Y8 limited range frame [16, 235] to an RGBA32 (full range) frame [0, 255].
		 * This function first converts limited range to full range, then expands grayscale to RGBA.
		 * @param source The source frame buffer with limited range values, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param alphaValue The value of the alpha channel to be set, with range [0, 255]
		 * @param worker Optional worker object to distribute the computational load
		 * @tparam tPrecision The precision to use for range conversion: 6 for 6-bit (faster) or 10 for 10-bit (slower, higher quality)
		 */
		template <unsigned int tPrecision = 10u>
		static inline void convertY8LimitedRangeToRGBA32(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const uint8_t alphaValue = 0xFF, Worker* worker = nullptr);

		/**
		 * Converts a Y 8 bit frame to a Y 8 bit frame.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 */
		static inline void convertY8ToY8(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a Y8 frame to a Y8 frame by applying gamma compression/correction using a lookup table.
		 * The gamma compression/correction is based the following equation
		 * <pre>
		 * Y8 = 255 * (Y8 / 255) ^ gamma
		 * </pre>
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param gamma The gamma value to be applied, with range (0, 2)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 */
		static inline void convertY8ToY8GammaLUT(const uint8_t* const source, uint8_t* const target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const float gamma, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a Y8 limited range frame [16, 235] to a Y8 full range frame [0, 255].
		 * @param source The source frame buffer with limited range values, must be valid
		 * @param target The target frame buffer with full range values, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 * @tparam tPrecision The precision to use: 6 for 6-bit precision (faster) or 10 for 10-bit precision (slower)
		 */
		template <unsigned int tPrecision = 10u>
		static inline void convertY8LimitedRangeToY8FullRange(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a Y8 full range frame [0, 255] to a Y8 limited range frame [16, 235].
		 * @param source The source frame buffer with full range values, must be valid
		 * @param target The target frame buffer with limited range values, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 * @tparam tPrecision The precision to use: 6 for 6-bit precision (faster) or 10 for 10-bit precision (slower)
		 */
		template <unsigned int tPrecision = 10u>
		static inline void convertY8FullRangeToY8LimitedRange(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

	protected:

		/**
		 * Converts a Y8 row to a Y8 row by applying gamma compression/correction with a lookup table.
		 * @param source The pointer to the source pixels, must be valid
		 * @param target The pointer to the target pixels receiving the converted pixel data, must be valid
		 * @param size The number of source (and target pixels) to convert, with range [1, infinity)
		 * @param parameters The pointer to the `uint8_t` lookup table to be used, must be valid
		 */
		static void convertRowY8ToY8GammaLUT(const uint8_t* source, uint8_t* target, const size_t size, const void* parameters);
};

inline void FrameConverterY8::convertY8FullRangeToBGR24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	// source frame Y
	//              0
	// target frame B G R
	// pattern      0 0 0
	constexpr unsigned int shufflePattern = 0x000u;

	FrameChannels::shuffleChannels<uint8_t, 1u, 3u, shufflePattern>(source, target, width, height, flag, sourcePaddingElements, targetPaddingElements, worker);
}

inline void FrameConverterY8::convertY8FullRangeToRGB24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	// source frame Y
	//              0
	// target frame R G B
	// pattern      0 0 0
	constexpr unsigned int shufflePattern = 0x000u;

	FrameChannels::shuffleChannels<uint8_t, 1u, 3u, shufflePattern>(source, target, width, height, flag, sourcePaddingElements, targetPaddingElements, worker);
}

inline void FrameConverterY8::convertY8FullRangeToRGBA32(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const uint8_t alphaValue, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	// source frame Y
	//              0
	// target frame R G B A
	// pattern      0 0 0
	constexpr unsigned int shufflePattern = 0x000u;

	FrameChannels::shuffleChannelsAndSetLastChannelValue<uint8_t, 1u, 4u, shufflePattern>(source, alphaValue, target, width, height, flag, sourcePaddingElements, targetPaddingElements, worker);
}

inline void FrameConverterY8::convertY8ToY8(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	FrameChannels::transformGeneric<uint8_t, 1u>(source, target, width, height, flag, sourcePaddingElements, targetPaddingElements, worker);
}

inline void FrameConverterY8::convertY8ToY8GammaLUT(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const float gamma, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	ocean_assert(gamma > 0.0f && gamma < 2.0f);

	const unsigned int sourceStrideElements = width + sourcePaddingElements;
	const unsigned int targetStrideElements = width + targetPaddingElements;

	const void* const options = LookupTableManager::get().lookupTable(gamma);

	const bool areContinuous = sourcePaddingElements == 0u && targetPaddingElements == 0u;

	FrameConverter::convertGenericPixelFormat(source, target, width, height, sourceStrideElements, targetStrideElements, flag, convertRowY8ToY8GammaLUT, CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 1u>, areContinuous, options, worker);
}

template <unsigned int tPrecision>
inline void FrameConverterY8::convertY8LimitedRangeToY8FullRange(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(tPrecision == 6u || tPrecision == 10u, "Precision must be 6 or 10");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	// Y_full = (Y_limited - 16) * 255 / 219
	const int32_t options[5] =
	{
		int32_t(sourcePaddingElements),
		int32_t(targetPaddingElements),

		tPrecision == 6u ? int32_t(75) : int32_t(1192),  // 6-bit: 75 == (255/219 * 64); 10-bit: 1192 == (255/219 * 1024)
		16,
		0
	};

	const void* sources[1] = {source};
	void* targets[1] = {target};

	if constexpr (tPrecision == 6u)
	{
		FrameConverter::convertArbitraryPixelFormat(sources, targets, width, height, flag, 1u, FrameConverter::convertOneRow_1Plane1Channel_To_1Plane1Channel_8BitPerChannel_Precision6Bit, &options, worker);
	}
	else
	{
		FrameConverter::convertArbitraryPixelFormat(sources, targets, width, height, flag, 1u, FrameConverter::convertOneRow_1Plane1Channel_To_1Plane1Channel_8BitPerChannel_Precision10Bit, &options, worker);
	}
}

template <unsigned int tPrecision>
inline void FrameConverterY8::convertY8FullRangeToY8LimitedRange(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(tPrecision == 6u || tPrecision == 10u, "Precision must be 6 or 10");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	// Y_limited = Y_full * 219 / 255 + 16
	const int32_t options[5] =
	{
		int32_t(sourcePaddingElements),
		int32_t(targetPaddingElements),

		tPrecision == 6u ? int32_t(55) : int32_t(879), // 6-bit: 55 == (219/255 * 64); 10-bit: 879 == (219/255 * 1024)
		0,
		16
	};

	const void* sources[1] = {source};
	void* targets[1] = {target};

	if constexpr (tPrecision == 6u)
	{
		FrameConverter::convertArbitraryPixelFormat(sources, targets, width, height, flag, 1u, FrameConverter::convertOneRow_1Plane1Channel_To_1Plane1Channel_8BitPerChannel_Precision6Bit, &options, worker);
	}
	else
	{
		FrameConverter::convertArbitraryPixelFormat(sources, targets, width, height, flag, 1u, FrameConverter::convertOneRow_1Plane1Channel_To_1Plane1Channel_8BitPerChannel_Precision10Bit, &options, worker);
	}
}

template <unsigned int tPrecision>
inline void FrameConverterY8::convertY8LimitedRangeToRGB24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(tPrecision == 6u || tPrecision == 10u, "Precision must be 6 or 10");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	// Y_full = (Y_limited - 16) * 255 / 219
	const int32_t options[5] =
	{
		int32_t(sourcePaddingElements),
		int32_t(targetPaddingElements),

		tPrecision == 6u ? int32_t(75) : int32_t(1192), // 6-bit: 75 == (255/219 * 64); 10-bit: 1192 == (255/219 * 1024)
		16,
		0
	};

	const void* sources[1] = {source};
	void* targets[1] = {target};

	if constexpr (tPrecision == 6u)
	{
		FrameConverter::convertArbitraryPixelFormat(sources, targets, width, height, flag, 1u, FrameConverter::convertOneRow_1Plane1Channel_To_1Plane3Channels_8BitPerChannel_Precision6Bit, &options, worker);
	}
	else
	{
		FrameConverter::convertArbitraryPixelFormat(sources, targets, width, height, flag, 1u, FrameConverter::convertOneRow_1Plane1Channel_To_1Plane3Channels_8BitPerChannel_Precision10Bit, &options, worker);
	}
}

template <unsigned int tPrecision>
inline void FrameConverterY8::convertY8LimitedRangeToRGBA32(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const uint8_t alphaValue, Worker* worker)
{
	static_assert(tPrecision == 6u || tPrecision == 10u, "Precision must be 6 or 10");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	// Y_full = (Y_limited - 16) * 255 / 219
	const int32_t options[6] =
	{
		int32_t(sourcePaddingElements),
		int32_t(targetPaddingElements),

		tPrecision == 6u ? int32_t(75) : int32_t(1192), // 6-bit: 75 == (255/219 * 64); 10-bit: 1192 == (255/219 * 1024)
		16,
		0,
		int32_t(alphaValue)
	};

	const void* sources[1] = {source};
	void* targets[1] = {target};

	if constexpr (tPrecision == 6u)
	{
		FrameConverter::convertArbitraryPixelFormat(sources, targets, width, height, flag, 1u, FrameConverter::convertOneRow_1Plane1Channel_To_1Plane4Channels_8BitPerChannel_Precision6Bit, &options, worker);
	}
	else
	{
		FrameConverter::convertArbitraryPixelFormat(sources, targets, width, height, flag, 1u, FrameConverter::convertOneRow_1Plane1Channel_To_1Plane4Channels_8BitPerChannel_Precision10Bit, &options, worker);
	}
}

}

}

#endif // META_OCEAN_CV_FRAME_CONVERTER_Y_8_H
