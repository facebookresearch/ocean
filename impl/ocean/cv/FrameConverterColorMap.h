/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_CONVERTER_COLOR_MAP_H
#define META_OCEAN_CV_FRAME_CONVERTER_COLOR_MAP_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameChannels.h"
#include "ocean/cv/FrameMinMax.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

/**
 * This class provides functions to convert frames based on a color map.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameConverterColorMap : public FrameConverter
{
	public:

		/**
		 * Definition of individual color maps.
		 */
		enum ColorMap : uint32_t
		{
			/// An invalid color map
			CM_INVALID = 0u,
			/// The Jet color map, as implemented in convertJetFloat1ChannelToRGB24().
			CM_JET,
			/// The Turbo color map, as implemented in convertTurboFloat1ChannelToRGB24().
			CM_TURBO,
			/// The Inferno color map, as implemented in convertInfernoFloat1ChannelToRGB24().
			CM_INFERNO,
			/// The Spiral color map with 10 spiral rounds, as implemented in convertSpiralFloat1ChannelToRGB24().
			CM_SPIRAL_10,
			/// The Spiral color map with 20 spiral rounds, as implemented in convertSpiralFloat1ChannelToRGB24().
			CM_SPIRAL_20,
			/// A linear normalization of the input creating an RGB image with values between black and white.
			CM_LINEAR
		};

		/**
		 * Definition of a vector holding color map values.
		 */
		using ColorMaps = std::vector<ColorMap>;

		/**
		 * The following comfort class provides comfortable functions simplifying prototyping applications but also increasing binary size of the resulting applications.
		 * Best practice is to avoid using these functions if binary size matters,<br>
		 * as for every comfort function a corresponding function exists with specialized functionality not increasing binary size significantly.<br>
		 */
		class OCEAN_CV_EXPORT Comfort
		{
			public:

				/**
				 * Converts a 1-channel frame to a RGB24 frame using a specified color map.
				 * This function automatically determines the minimum and maximum value range of the source frame before applying the color map, infinity and NaN values are ignored and set to black.
				 * The source frame can hold 32-bit or 64 bit floating point elements, or 8-bit, 16-bit, 32-bit or 64-bit integer elements.
				 * @param source The source frame, must be valid
				 * @param target The target frame, will be modified in case the target frame type does not fit
				 * @param colorMap The color map to be used when converting the source frame
				 * @param worker Optional worker object to distribute the computational load
				 * @return True, if succeeded
				 */
				static bool convert1ChannelToRGB24(const Frame& source, Frame& target, const ColorMap colorMap, Worker* worker = nullptr);

				/**
				 * Converts a 1-channel float frame to a RGB24 frame using a specified color map.
				 * If minValue and/or maxValue are not specified, this function automatically determines the minimum and maximum value range of the source frame before applying the color map, infinity and NaN values are ignored and set to black.
				 * The source frame can hold 32-bit or 64-bit floating point elements.
				 * @param source The source frame, must be valid
				 * @param target The target frame, will be modified in case the target frame type does not fit
				 * @param colorMap The color map to be used when converting the source frame
				 * @param worker Optional worker object to distribute the computational load
				 * @param minValue Optional minimum value used to apply the color map, with range (-infinity, infinity), NumericF::maxValue() to let the function determine the value
				 * @param maxValue Optional maximum value used to apply the color map, with range (minValue, infinity), NumericF::minValue() to let the function determine the value
				 * @return True, if succeeded
				 */
				static bool convertFloat1ChannelToRGB24(const Frame& source, Frame& target, const ColorMap colorMap, Worker* worker = nullptr, float minValue = NumericF::maxValue(), float maxValue = NumericF::minValue());

				/**
				 * Converts a 1-channel integer frame to a RGB24 frame using a specified color map.
				 * This function automatically determines the minimum and maximum value range of the source frame before applying the color map, infinity and NaN values are ignored and set to black.
				 * The source frame can hold 8-bit, 16-bit, 32-bit or 64-bit integer elements.
				 * @param source The source frame, must be valid
				 * @param target The target frame, will be modified in case the target frame type does not fit
				 * @param colorMap The color map to be used when converting the source frame
				 * @param worker Optional worker object to distribute the computational load
				 * @return True, if succeeded
				 */
				static bool convertInteger1ChannelToRGB24(const Frame& source, Frame& target, const ColorMap colorMap, Worker* worker = nullptr);
		};

	protected:

		/**
		 * Helper class for converters with lookup table.
		 */
		class LookupData
		{
			public:

				/**
				 * Creates a new lookup object.
				 * @param minValue The minimal source value, with range (-infinity, infinity)
				 * @param maxValue The minimal source value, with range (minValue, infinity)
				 * @param lookupTable The actual lookup table, must be valid
				 */
				inline LookupData(const float minValue, const float maxValue, const uint8_t* const lookupTable);

				/**
				 * Return the minimal source value.
				 * @return The minimal source value
				 */
				inline float minValue() const;

				/**
				 * Return the maximal source value.
				 * @return The maximal source value
				 */
				inline float maxValue() const;

				/**
				 * Returns the actual lookup table.
				 * @return The lookup table
				 */
				inline const uint8_t* lookupTable() const;

			protected:

				/// The minimal source value, with range (-infinity, infinity).
				const float minValue_;

				/// The minimal source value, with range (minValue, infinity).
				const float maxValue_;

				/// The actual lookup table.
				const uint8_t* const lookupTable_;
		};

	public:

		/**
		 * Converts a row of a 1-channel float frame to a row of a RGB24 frame using the Jet color map.
		 * Infinity and NaN values are ignored and set to black.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range (0, infinity)
		 * @param height The height of the frame in pixel, with range (0, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in (uint16_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param minSource The minimal source value, with range (-infinity, infinity)
		 * @param maxSource The minimal source value, with range (minValue, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 */
		static inline void convertJetFloat1ChannelToRGB24(const float* const source, uint8_t* const target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const float minSource = 0.0f, const float maxSource = 1.0f, Worker* worker = nullptr);

		/**
		 * Converts a row of a 1-channel float frame to a row of a RGB24 frame using the Turbo color map.
		 * Infinity and NaN values are ignored and set to black.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range (0, infinity)
		 * @param height The height of the frame in pixel, with range (0, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in (uint16_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param minSource The minimal source value, with range (-infinity, infinity)
		 * @param maxSource The minimal source value, with range (minValue, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 */
		static void convertTurboFloat1ChannelToRGB24(const float* const source, uint8_t* const target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const float minSource = 0.0f, const float maxSource = 1.0f, Worker* worker = nullptr);

		/**
		 * Converts a row of a 1-channel float frame to a row of a RGB24 frame using the Inferno color map.
		 * Infinity and NaN values are ignored and set to black.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range (0, infinity)
		 * @param height The height of the frame in pixel, with range (0, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in (uint16_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param minSource The minimal source value, with range (-infinity, infinity)
		 * @param maxSource The minimal source value, with range (minValue, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 */
		static void convertInfernoFloat1ChannelToRGB24(const float* const source, uint8_t* const target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const float minSource = 0.0f, const float maxSource = 1.0f, Worker* worker = nullptr);

		/**
		 * Converts a row of a 1-channel float frame to a row of a RGB24 frame using the Spiral color map.
		 * Infinity and NaN values are ignored and set to black.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range (0, infinity)
		 * @param height The height of the frame in pixel, with range (0, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in (uint16_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param minSource The minimal source value, with range (-infinity, infinity)
		 * @param maxSource The minimal source value, with range (minValue, infinity)
		 * @param period The width of one spiral round/period in defined in the domain of `minValue` and `maxValue`, with range (0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 */
		static inline void convertSpiralFloat1ChannelToRGB24(const float* const source, uint8_t* const target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const float minSource = 0.0f, const float maxSource = 1.0f, const float period = 0.1f, Worker* worker = nullptr);

		/**
		 * Translates the name of a color map to it's value.
		 * @param colorMap The name of the color map, possible values are 'jet', 'turbo', 'inferno', 'spiral10', 'spiral20', 'linear'
		 * @return The resulting value of the color map, CM_INVALID if unknown
		 */
		static ColorMap translateColorMap(const std::string& colorMap);
		/**
		 * Translates the values of a color map to a readable string.
		 * @param colorMap The value of the color map
		 * @return The resulting readable string, empty if unknown
		 */
		static std::string translateColorMap(const ColorMap colorMap);

		/**
		 * Returns all defined color maps.
		 * @return The definition of all color maps
		 */
		static const ColorMaps& definedColorMaps();

	protected:

		/**
		 * Converts a row of a 1-channel float frame to a row of a RGB24 frame using the Jet color map.
		 * @param source The source row, must be valid
		 * @param target The target row, must be valid
		 * @param width The width of the row in pixel, with range [1, infinity)
		 * @param options The options parameter with two float values, must be valid
		 */
		static void convertRowJetFloat1ChannelToRGB24(const float* source, uint8_t* target, const size_t width, const void* options);

		/**
		 * Converts a row of a 1-channel float frame to a row of a RGB24 frame using the Spiral color map.
		 * @param source The source row, must be valid
		 * @param target The target row, must be valid
		 * @param width The width of the row in pixel, with range [1, infinity)
		 * @param options The options parameter with three float values, must be valid
		 */
		static void convertRowSpiralFloat1ChannelToRGB24(const float* source, uint8_t* target, const size_t width, const void* options);

		/**
		 * Converts a row of a 1-channel float frame to a row of a RGB24 frame using a lookup table.
		 * @param source The source row, must be valid
		 * @param target The target row, must be valid
		 * @param width The width of the row in pixel, with range [1, infinity)
		 * @param options The lookup data object, must be valid
		 */
		static void convertRowLookupFloat1ChannelToRGB24(const float* source, uint8_t* target, const size_t width, const void* options);
};

inline FrameConverterColorMap::LookupData::LookupData(const float minValue, const float maxValue, const uint8_t* const lookupTable) :
	minValue_(minValue),
	maxValue_(maxValue),
	lookupTable_(lookupTable)
{
	// nothing to do here
}

inline float FrameConverterColorMap::LookupData::minValue() const
{
	return minValue_;
}

inline float FrameConverterColorMap::LookupData::maxValue() const
{
	return maxValue_;
}

inline const uint8_t* FrameConverterColorMap::LookupData::lookupTable() const
{
	return lookupTable_;
}

inline void FrameConverterColorMap::convertJetFloat1ChannelToRGB24(const float* const source, uint8_t* const target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const float minSource, const float maxSource, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(minSource < maxSource);

	const unsigned int sourceStrideElements = width + sourcePaddingElements;
	const unsigned int targetStrideElements = width * 3u + targetPaddingElements;

	const float options[2] =
	{
		minSource,
		maxSource
	};

	const bool areContinuous = sourcePaddingElements == 0u && targetPaddingElements == 0u;

	FrameConverter::convertGenericPixelFormat<float, uint8_t>(source, target, width, height, sourceStrideElements, targetStrideElements, flag, CV::FrameConverterColorMap::convertRowJetFloat1ChannelToRGB24, CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>, areContinuous, options, worker);
}

inline void FrameConverterColorMap::convertSpiralFloat1ChannelToRGB24(const float* const source, uint8_t* const target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const float minSource, const float maxSource, const float period, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(minSource < maxSource);
	ocean_assert(period > 0.0f);

	const unsigned int sourceStrideElements = width + sourcePaddingElements;
	const unsigned int targetStrideElements = width * 3u + targetPaddingElements;

	const float options[3] =
	{
		minSource,
		maxSource,
		period
	};

	const bool areContinuous = sourcePaddingElements == 0u && targetPaddingElements == 0u;

	FrameConverter::convertGenericPixelFormat<float, uint8_t>(source, target, width, height, sourceStrideElements, targetStrideElements, flag, CV::FrameConverterColorMap::convertRowSpiralFloat1ChannelToRGB24, CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>, areContinuous, options, worker);
}

}

}

#endif // META_OCEAN_CV_FRAME_CONVERTER_COLOR_MAP_H
