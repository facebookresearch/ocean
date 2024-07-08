/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_NORMALIZER_H
#define META_OCEAN_CV_FRAME_NORMALIZER_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameMinMax.h"

#include "ocean/base/DataType.h"
#include "ocean/base/Frame.h"
#include "ocean/base/Utilities.h"
#include "ocean/base/Worker.h"

#include "ocean/math/Numeric.h"

#include <limits>

namespace Ocean
{

namespace CV
{

/**
 * This class implements functions normalizing frames.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameNormalizer
{
	private:

		/**
		 * Helper struct allowing to determine the necessary float type for a given data type.
		 * @tparam T The data type for which the matching float type has to be found
		 */
		template <typename T>
		struct FloatType
		{
			/// The matching float type for `T`, is `double` if `T` is double, otherwise always `float`.
			typedef float Type;
		};

		/**
		 * Helper struct allowing to determine the type able to store data multiplied by 255
		 * @tparam T The data type for which the matching type has to be found
		 */
		template <typename T>
		struct DataTypeMultiple255
		{
			/// The matching datatype which is able to store all values in T multiplied by 255
			typedef typename NextLargerTyper<T>::Type Type;
		};

	public:

		/**
		 * Normalizes a given 1-channel frame linearly to a uint8 image.
		 * The normalization is based on the following equation for each pixel individually:
		 * <pre>
		 * normalizedPixel = (pixel - minimalPixelValue) / (maximalPixelValue - minimalPixelValue) * 255
		 *
		 * with `minimalPixelValue` and `maximalPixelValue` the minimal (or maximal) pixel value within the entire image
		 * </pre>
		 * @param source The source frame to be normalized, must have one channel, must be valid
		 * @param target The resulting normalized frame with pixel format FORMAT_Y8
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool normalizeToUint8(const Frame& source, Frame& target, Worker* worker = nullptr);

		/**
		 * Normalizes each channel of a frame linearly to a float32 image to the range [0, 1].
		 * The normalization is based on the following equation for each pixel individually:
		 * <pre>
		 * normalizedPixel[channel] = (pixel[channel] - minimalPixelValues[channel]) / (maximalPixelValues[channel] - minimalPixelValues[channel])
		 * </pre>
		 * with `minimalPixelValues[channel]` and `maximalPixelValues[channel]` being the minimum and maximum pixel values of a channel of the image.
		 * @param source The source frame to be normalized, must have one channel, must be valid
		 * @param target The resulting normalized frame; will have the same frame type as `source` but with data type `FrameType::DT_SIGNED_FLOAT_32`
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool normalizeToFloat32(const Frame& source, Frame& target, Worker* worker = nullptr);

		/**
		 * Normalizes a given frame linearly to a float32 image.
		 * If both, `bias` and `scale` are valid, the normalization is based on the following equation for each pixel individually:
		 * <pre>
		 * normalizedPixel[channel] = (pixel[channel] + bias[channel]) * scale[channel]
		 * </pre>
		 * if either or both are `nullptr`, the normalization is as follows:
		 * <pre>
		 * normalizedPixel[channel] = (pixel[channel] - minimalPixelValues[channel]) / (maximalPixelValues[channel] - minimalPixelValues[channel])
		 * </pre>
		 * with `minimalPixelValues[channel]` and `maximalPixelValues[channel]` being the minimum and maximum pixel values of each channel of the image.
		 * @param source The source frame to be normalized, must have one channel, must be valid
		 * @param target The resulting normalized frame; will have the same frame type as `source` but with data type `FrameType::DT_SIGNED_FLOAT_32`
		 * @param bias The bias values, if valid it must have `source.channels()` elements
		 * @param scale The scale value, if valid it must have `source.channels()` elements
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool normalizeToFloat32(const Frame& source, Frame& target, const float* bias, const float* scale, Worker* worker = nullptr);

		/**
		 * Normalizes a given frame logarithmically to a uint8 image.
		 * @param source The source frame to be normalized, must be valid
		 * @param target The target frame receiving the normalized values, will be adjusted to the correct frame type if it does not have the correct pixel format and pixel origin
		 * @param octaves Scalar that specifies the logarithmic display range, with range (0, 10]
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool normalizeLogarithmToUint8(const Frame& source, Frame& target, const Scalar octaves = 5.0f, Worker* worker = nullptr);

		/**
		 * Normalizes a given frame logarithmically to a float32 image.
		 * @param source The source frame to be normalized, must be valid
		 * @param target The target frame receiving the normalized values, will be adjusted to the correct frame type if it does not have the correct pixel format and pixel origin
		 * @param octaves Scalar that specifies the logarithmic display range, with range (0, 10]
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool normalizeLogarithmToFloat32(const Frame& source, Frame& target, const Scalar octaves = 5.0f, Worker* worker = nullptr);

		/**
		 * Normalizes a given 1-channel frame linearly to a uint8 image.
		  * The normalization is based on the following equation for each pixel individually:
		 * <pre>
		 * normalizedPixel = (pixel - minimalPixelValue) / (maximalPixelValue - minimalPixelValue) * 255
		 *
		 * with `minimalPixelValue` and `maximalPixelValue` the minimal (or maximal) pixel value within the entire image
		 * </pre>
		 * @param frame The frame to be normalized, must have one channel, must be valid
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool normalizeToUint8(Frame& frame, Worker* worker = nullptr);

		/**
		 * Normalizes a given 1-channel frame linearly to a float32 image.
		  * The normalization is based on the following equation for each pixel individually:
		 * <pre>
		 * normalizedPixel = (pixel - minimalPixelValue) / (maximalPixelValue - minimalPixelValue) * 255
		 *
		 * with `minimalPixelValue` and `maximalPixelValue` the minimal (or maximal) pixel value within the entire image
		 * </pre>
		 * @param frame The frame to be normalized, must have one channel, must be valid
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool normalizeToFloat32(Frame& frame, Worker* worker = nullptr);

		/**
		 * Normalizes a given frame linearly to a uint8 image.
		 * @param frame The frame to be normalized
		 * @param octaves Scalar that specifies the logarithmic display range, with range (0, 10]
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool normalizeLogarithmToUint8(Frame& frame, const Scalar octaves = 5.0f, Worker* worker = nullptr);

		/**
		 * Normalizes a given frame linearly to a float32 image.
		 * @param frame The frame to be normalized
		 * @param octaves Scalar that specifies the logarithmic display range, with range (0, 10]
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool normalizeLogarithmToFloat32(Frame& frame, const Scalar octaves = 5.0f, Worker* worker = nullptr);

		/**
		 * Normalizes a given 1 channel frame to the 8 bit value range [0, 255] linearly.
		 * The normalization is based on the following equation for each pixel individually:
		 * <pre>
		 * normalizedPixel = (pixel - minimalPixelValue) / (maximalPixelValue - minimalPixelValue) * 255
		 *
		 * with `minimalPixelValue` and `maximalPixelValue` the minimal (or maximal) pixel value within the entire image
		 * </pre>
		 * @param source The source frame to be normalized, must be valid
		 * @param target The normalized frame, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param sourcePaddingElements Optional number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam T The data type of each source pixel
		 * @see normalizeLogarithm1ChannelToUint8(), normalizeToFloat().
		 */
		template <typename T>
		static void normalize1ChannelToUint8(const T* source, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Normalizes a frame to float values by using predefined per-channel bias and scaling values.
		 * If both, `bias` and `scale` are valid, the normalization is based on the following equation for each pixel individually:
		 * <pre>
		 * normalizedPixel[channel] = (pixel[channel] + bias[channel]) * scale[channel]
		 * </pre>
		 * if either or both are `nullptr`, the normalization is as follows:
		 * <pre>
		 * normalizedPixel[channel] = (pixel[channel] - minimalPixelValues[channel]) / (maximalPixelValues[channel] - minimalPixelValues[channel])
		 * </pre>
		 * with `minimalPixelValues[channel]` and `maximalPixelValues[channel]` being the minimum and maximum pixel values of each channel of the image.
		 * @param source The source frame to be normalized, must be valid
		 * @param target The normalized frame, must be valid
		 * @param width The width of the frame in pixels, with range [1, infinity)
		 * @param height The height of the frame in pixels, with range [1, infinity)
		 * @param bias The bias values, must be valid and have `tChannels` elements
		 * @param scale The scale value, must be valid and have `tChannels` elements
		 * @param sourcePaddingElements Optional number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam TSource The data type of each source pixel, can be an integer or floating point data type
		 * @tparam TTarget The data type of each target pixel, must be a floating point data type
		 * @tparam tChannels Number of channels of the frame, with range [1, infinity)
		 */
		template <typename TSource, typename TTarget, unsigned int tChannels>
		static void normalizeToFloat(const TSource* source, TTarget* target, const unsigned int width, const unsigned int height, const TTarget* bias, const TTarget* scale, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Normalizes a given 1 channel frame to the 8 bit value range [0, 255] logarithmically.
		 * @param source The source frame to be normalized, must be valid
		 * @param target The target frame receiving the normalized frame, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param octaves Scalar that specifies the logarithmic display range, with range (0, 10]
		 * @param sourcePaddingElements Optional number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam T Data type of each pixel
		 * @see normalize1ChannelToUint8().
		 */
		template <typename T>
		static void normalizeLogarithm1ChannelToUint8(const T* source, uint8_t* target, const unsigned int width, const unsigned int height, const Scalar octaves, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Normalizes a given 1 channel frame to float values with value range [0, 1] logarithmically.
		 * @param source The source frame to be normalized, must be valid
		 * @param target The target frame with floating point precision receiving the normalized frame, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param octaves Scalar that specifies the logarithmic display range, with range (0, 10]
		 * @param sourcePaddingElements Optional number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam TSource The data type of each source pixel, can be an integer or floating point data type
		 * @tparam TTarget The data type of each target pixel, must be a floating point data type
		 */
		template <typename TSource, typename TTarget>
		static void normalizeLogarithm1ChannelToFloat(const TSource* source, TTarget* target, const unsigned int width, const unsigned int height, const Scalar octaves, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

	private:

		/**
		 * Normalizes a given 1 channel integer frame to the 8 bit value range [0, 255] linearly.
		 * @param source The source frame to be normalized, must be valid
		 * @param target The normalized frame, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param sourcePaddingElements Optional number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam T The data type of each source pixel, must be an integer type
		 */
		template <typename T>
		static inline void normalize1ChannelIntegerToUint8(const T* source, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Normalizes a given 1 channel float frame to the 8 bit value range [0, 255] linearly.
		 * @param source The source frame to be normalized, must be valid
		 * @param target The normalized frame, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param sourcePaddingElements Optional number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam T The data type of each source pixel, must be a floating point type
		 */
		template <typename T>
		static inline void normalize1ChannelFloatToUint8(const T* source, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Normalizes a frame to float values by using predefined per-channel bias and scaling values.
		 * If both, `bias` and `scale` are valid, the normalization is based on the following equation for each pixel individually:
		 * <pre>
		 * normalizedPixel[channel] = (pixel[channel] + bias[channel]) * scale[channel]
		 * </pre>
		 * if either or both are `nullptr`, the normalization is as follows:
		 * <pre>
		 * normalizedPixel[channel] = (pixel[channel] - minimalPixelValues[channel]) / (maximalPixelValues[channel] - minimalPixelValues[channel])
		 * </pre>
		 * with `minimalPixelValues[channel]` and `maximalPixelValues[channel]` being the minimum and maximum pixel values of each channel of the image.
		 * @param source The source frame to be normalized, must be valid
		 * @param target The normalized frame, must be valid
		 * @param width The width of the frame in pixels, with range [1, infinity)
		 * @param height The height of the frame in pixels, with range [1, infinity)
		 * @param channels Number of channels of the frame, with range [1, infinity)
		 * @param bias The bias values, must be valid and have `channels` elements
		 * @param scale The scale value, must be valid and have `channels` elements
		 * @param sourcePaddingElements Optional number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return True if the normalization was successful, otherwise false
		 * @tparam TSource The data type of each source pixel, can be an integer or floating point data type
		 * @tparam TTarget The data type of each target pixel, must be a floating point data type
		 */
		template <typename TSource, typename TTarget>
		static bool normalizeToFloat(const TSource* source, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int channels, const TTarget* bias, const TTarget* scale, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Normalizes a subset of a given 1 channel frame to the 8 bit value range [0, 255] linearly.
		 * @param source The source frame to be normalized, must be valid
		 * @param target The normalized frame, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param minimalValue Minimal value that have been determined already
		 * @param range The range between maximal and minimal value [maximalValue - minimalValue]
		 * @param sourcePaddingElements Optional number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param firstRow First row to be handled, with range [0, height - 1]
		 * @param numberRows Number rows to be handled, with range [1, height - firstRow]
		 * @tparam T Data type of each pixel, must be an integer type
		 */
		template <typename T>
		static void normalize1ChannelIntegerToUint8Subset(const T* source, uint8_t* target, const unsigned int width, const unsigned int height, const T minimalValue, const typename UnsignedTyper<T>::Type range, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Normalizes a subset of a given 1 channel frame to the 8 bit value range [0, 255] linearly.
		 * @param source The source frame to be normalized, must be valid
		 * @param target The normalized frame, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param minimalValue Minimal value that have been determined already
		 * @param range The range between maximal and minimal value [maximalValue - minimalValue]
		 * @param sourcePaddingElements Optional number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param firstRow First row to be handled, with range [0, height - 1]
		 * @param numberRows Number rows to be handled, with range [1, height - firstRow]
		 * @tparam T Data type of each pixel, must be a floating point type
		 * @tparam TRange The data type of the range value, either 'float' or 'double'
		 */
		template <typename T, typename TRange>
		static void normalize1ChannelFloatToUint8Subset(const T* source, uint8_t* target, const unsigned int width, const unsigned int height, const T minimalValue, const TRange range, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Normalizes a subset of rows of a frame to float values by using predefined per-channel bias and scaling values.
		 * @param source The source frame to be normalized, must be valid
		 * @param target The normalized frame with floating point precision, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param bias The bias values, must be valid and have `tChannels` elements
		 * @param scale The scale value, must be valid and have `tChannels` elements
		 * @param sourcePaddingElements Optional number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param firstRow First row to be handled, with range [0, height - 1]
		 * @param numberRows Number rows to be handled, with range [1, height - firstRow]
		 * @tparam TSource The data type of each source pixel, can be an integer or floating point data type
		 * @tparam TTarget The data type of each target pixel, must be a floating point data type
		 * @tparam tChannels Number of channels of the frame, with range [1, infinity)
		 */
		template <typename TSource, typename TTarget, unsigned int tChannels>
		static void normalizeToFloatSubset(const TSource* source, TTarget* target, const unsigned int width, const unsigned int height, const TTarget* bias, const TTarget* scale, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Normalizes a subset of a given 1 channel frame to the 8 bit value range [0, 255] logarithmically.
		 * @param source The source frame to be normalizes, must be valid
		 * @param target The target frame receiving the normalized frame, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param octaves Scalar that specifies the logarithmic display range, with range (0, 10]
		 * @param minimalValue Minimal value that have been determined already
		 * @param range The range between maximal and minimal value [maximal - minimal]
		 * @param sourcePaddingElements Optional number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param firstRow First row to be handled, with range [0, height - 1]
		 * @param numberRows Number rows to be handled, with range [1, height - firstRow]
		 * @tparam T Data type of each pixel
		 * @tparam TRange The data type of the range value, either 'float' or 'double'
		 */
		template <typename T, typename TRange>
		static void normalizeLogarithm1ChannelToUint8Subset(const T* source, uint8_t* target, const unsigned int width, const unsigned int height, const Scalar octaves, const T minimalValue, const TRange range, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Normalizes a subset of a given 1 channel frame to the value range [0, 1] logarithmically.
		 * @param source The source frame to be normalizes, must be valid
		 * @param target The target frame with floating point precision receiving the normalized frame, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param octaves Scalar that specifies the logarithmic display range, with range (0, 10]
		 * @param minimalValue Minimal value that have been determined already
		 * @param range The range between maximal and minimal value [maximal - minimal]
		 * @param sourcePaddingElements Optional number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param firstRow First row to be handled, with range [0, height - 1]
		 * @param numberRows Number rows to be handled, with range [1, height - firstRow]
		 * @tparam TSource The data type of each source pixel, can be an integer or floating point data type
		 * @tparam TTarget The data type of each target pixel, must be a floating point data type
		 */
		template <typename TSource, typename TTarget>
		static void normalizeLogarithm1ChannelToFloatSubset(const TSource* source, TTarget* target, const unsigned int width, const unsigned int height, const Scalar octaves, const TTarget minimalValue, const TTarget range, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Returns the smallest epsilon larger than zero for a specific data type.
		 * @return Requested epsilon
		 * @tparam T Data type for that the epsilon is requested
		 */
		template <typename T>
		static inline T epsilon();
};

/**
 * Specialization of the helper struct.
 */
template <>
struct FrameNormalizer::FloatType<double>
{
	typedef double Type;
};

template <>
struct FrameNormalizer::DataTypeMultiple255<uint64_t>
{
	typedef double Type;
};

template <>
struct FrameNormalizer::DataTypeMultiple255<float>
{
	typedef float Type;
};

template <typename T>
void FrameNormalizer::normalize1ChannelToUint8(const T* source, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	normalize1ChannelIntegerToUint8<T>(source, target, width, height, sourcePaddingElements, targetPaddingElements, worker);
}

template <typename TSource, typename TTarget, unsigned int tChannels>
void FrameNormalizer::normalizeToFloat(const TSource* source, TTarget* target, const unsigned int width, const unsigned int height, const TTarget* bias, const TTarget* scale, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(std::is_floating_point<TTarget>::value, "Invalid data type!");
	static_assert(tChannels != 0u, "Invalid number of channels");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	TTarget localBias[tChannels];
	TTarget localScale[tChannels];

	const TTarget* biasPtr = nullptr;
	const TTarget* scalePtr = nullptr;

	if (bias != nullptr && scale != nullptr)
	{
		biasPtr = bias;
		scalePtr = scale;
	}
	else
	{
		TSource minimalValues[tChannels];
		TSource maximalValues[tChannels];

		FrameMinMax::determineMinMaxValues<TSource, tChannels, true /*tIgnoreInfinity*/>(source, width, height, sourcePaddingElements, minimalValues, maximalValues, worker);

		for (unsigned int c = 0u; c < tChannels; ++c)
		{
			localBias[c] = -TTarget(minimalValues[c]);
			localScale[c] = TTarget(1) / max(epsilon<TTarget>(), TTarget(maximalValues[c]) - TTarget(minimalValues[c]));
		}

		biasPtr = localBias;
		scalePtr = localScale;
	}

	ocean_assert(biasPtr != nullptr && scalePtr != nullptr);

	if (worker != nullptr)
	{
		worker->executeFunction(Worker::Function::createStatic(normalizeToFloatSubset<TSource, TTarget, tChannels>, source, target, width, height, biasPtr, scalePtr, sourcePaddingElements, targetPaddingElements, 0u, 0u), 0u, height, 8u, 9u, 20u);
	}
	else
	{
		normalizeToFloatSubset<TSource, TTarget, tChannels>(source, target, width, height, biasPtr, scalePtr, sourcePaddingElements, targetPaddingElements, 0u, height);
	}
}

template <>
inline void FrameNormalizer::normalize1ChannelToUint8<float>(const float* source, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	normalize1ChannelFloatToUint8<float>(source, target, width, height, sourcePaddingElements, targetPaddingElements, worker);
}

template <>
inline void FrameNormalizer::normalize1ChannelToUint8<double>(const double* source, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	normalize1ChannelFloatToUint8<double>(source, target, width, height, sourcePaddingElements, targetPaddingElements, worker);
}

template <typename T>
void FrameNormalizer::normalizeLogarithm1ChannelToUint8(const T* source, uint8_t* target, const unsigned int width, const unsigned int height, const Scalar octaves, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(octaves > 0 && octaves <= 10);

	T minimalValue = NumericT<T>::maxValue();
	T maximalValue = NumericT<T>::minValue();

	FrameMinMax::determineMinMaxValues<T, 1u, true /*tIgnoreInfinity*/>(source, width, height, sourcePaddingElements, &minimalValue, &maximalValue, worker);

	if (std::is_same<T, double>::value || double(maximalValue) - double(minimalValue) >= 1.0e6)
	{
		const double range = max(epsilon<double>(), double(maximalValue) - double(minimalValue));

		if (worker != nullptr)
		{
			worker->executeFunction(Worker::Function::createStatic(normalizeLogarithm1ChannelToUint8Subset<T, double>, source, target, width, height, minmax(Numeric::eps(), octaves, Scalar(10)), minimalValue, range, sourcePaddingElements, targetPaddingElements, 0u, 0u), 0u, height, 9u, 10u, 20u);
		}
		else
		{
			normalizeLogarithm1ChannelToUint8Subset<T, double>(source, target, width, height, minmax(Numeric::eps(), octaves, Scalar(10)), minimalValue, range, sourcePaddingElements, targetPaddingElements, 0u, height);
		}
	}
	else
	{
		const T range = max(epsilon<T>(), T(maximalValue - minimalValue));

		if (worker != nullptr)
		{
			worker->executeFunction(Worker::Function::createStatic(normalizeLogarithm1ChannelToUint8Subset<T, T>, source, target, width, height, minmax(Numeric::eps(), octaves, Scalar(10)), minimalValue, range, sourcePaddingElements, targetPaddingElements, 0u, 0u), 0u, height, 9u, 10u, 20u);
		}
		else
		{
			normalizeLogarithm1ChannelToUint8Subset<T, T>(source, target, width, height, minmax(Numeric::eps(), octaves, Scalar(10)), minimalValue, range, sourcePaddingElements, targetPaddingElements, 0u, height);
		}
	}
}

template <typename TSource, typename TTarget>
void FrameNormalizer::normalizeLogarithm1ChannelToFloat(const TSource* source, TTarget* target, const unsigned int width, const unsigned int height, const Scalar octaves, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(std::is_floating_point<TTarget>::value, "Invalid data type!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(octaves > 0 && octaves <= 10);

	TSource minimalValue = NumericT<TSource>::maxValue();
	TSource maximalValue = NumericT<TSource>::minValue();

	FrameMinMax::determineMinMaxValues<TSource, 1u, true /*tIgnoreInfinity*/>(source, width, height, sourcePaddingElements, &minimalValue, &maximalValue, worker);

	const TTarget range = max(epsilon<TTarget>(), TTarget(maximalValue) - TTarget(minimalValue));

	if (worker != nullptr)
	{
		worker->executeFunction(Worker::Function::createStatic(normalizeLogarithm1ChannelToFloatSubset<TSource, TTarget>, source, target, width, height, minmax(Numeric::eps(), octaves, Scalar(10)), TTarget(minimalValue), range, sourcePaddingElements, targetPaddingElements, 0u, 0u), 0u, height, 9u, 10u, 20u);
	}
	else
	{
		normalizeLogarithm1ChannelToFloatSubset<TSource, TTarget>(source, target, width, height, minmax(Numeric::eps(), octaves, Scalar(10)), TTarget(minimalValue), range, sourcePaddingElements, targetPaddingElements, 0u, height);
	}
}

template <typename T>
inline void FrameNormalizer::normalize1ChannelIntegerToUint8(const T* source, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(!std::is_floating_point<T>::value, "Invalid data type!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	T minimalValue = NumericT<T>::maxValue();
	T maximalValue = NumericT<T>::minValue();

	FrameMinMax::determineMinMaxValues<T, 1u>(source, width, height, sourcePaddingElements, &minimalValue, &maximalValue, worker);

	if (NumericT<T>::isEqual(minimalValue, maximalValue))
	{
		Frame targetFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), target, Frame::CM_USE_KEEP_LAYOUT, targetPaddingElements);
		targetFrame.setValue(0x00);
	}
	else
	{
		const typename UnsignedTyper<T>::Type range = typename UnsignedTyper<T>::Type(typename NextLargerTyper<T>::Type(maximalValue) - typename NextLargerTyper<T>::Type(minimalValue));

		if (worker != nullptr)
		{
			worker->executeFunction(Worker::Function::createStatic(normalize1ChannelIntegerToUint8Subset<T>, source, target, width, height, minimalValue, range, sourcePaddingElements, targetPaddingElements, 0u, 0u), 0u, height, 8u, 9u, 20u);
		}
		else
		{
			normalize1ChannelIntegerToUint8Subset<T>(source, target, width, height, minimalValue, range, sourcePaddingElements, targetPaddingElements, 0u, height);
		}
	}
}

template <typename T>
inline void FrameNormalizer::normalize1ChannelFloatToUint8(const T* source, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(std::is_floating_point<T>::value, "Invalid data type!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	T minimalValue = NumericT<T>::maxValue();
	T maximalValue = NumericT<T>::minValue();

	FrameMinMax::determineMinMaxValues<T, 1u, true /*tIgnoreInfinity*/>(source, width, height, sourcePaddingElements, &minimalValue, &maximalValue, worker);

	if (std::is_same<T, double>::value || double(maximalValue) - double(minimalValue) >= 1.0e6)
	{
		const double range = max(epsilon<double>(), double(maximalValue) - double(minimalValue));

		if (worker != nullptr)
		{
			worker->executeFunction(Worker::Function::createStatic(normalize1ChannelFloatToUint8Subset<T, double>, source, target, width, height, minimalValue, range, sourcePaddingElements, targetPaddingElements, 0u, 0u), 0u, height, 8u, 9u, 20u);
		}
		else
		{
			normalize1ChannelFloatToUint8Subset<T, double>(source, target, width, height, minimalValue, range, sourcePaddingElements, targetPaddingElements, 0u, height);
		}
	}
	else
	{
		const T range = max(epsilon<T>(), T(maximalValue - minimalValue));

		if (worker != nullptr)
		{
			worker->executeFunction(Worker::Function::createStatic(normalize1ChannelFloatToUint8Subset<T, T>, source, target, width, height, minimalValue, range, sourcePaddingElements, targetPaddingElements, 0u, 0u), 0u, height, 8u, 9u, 20u);
		}
		else
		{
			normalize1ChannelFloatToUint8Subset<T, T>(source, target, width, height, minimalValue, range, sourcePaddingElements, targetPaddingElements, 0u, height);
		}
	}
}

template <typename TSource, typename TTarget>
bool FrameNormalizer::normalizeToFloat(const TSource* source, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int channels, const TTarget* bias, const TTarget* scale, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(channels != 0u && channels <= 4u);

	switch (channels)
	{
		case 1u:
			normalizeToFloat<TSource, TTarget, 1u>(source, target, width, height, bias, scale, sourcePaddingElements, targetPaddingElements, worker);
			return true;

		case 2u:
			normalizeToFloat<TSource, TTarget, 2u>(source, target, width, height, bias, scale, sourcePaddingElements, targetPaddingElements, worker);
			return true;

		case 3u:
			normalizeToFloat<TSource, TTarget, 3u>(source, target, width, height, bias, scale, sourcePaddingElements, targetPaddingElements, worker);
			return true;

		case 4u:
			normalizeToFloat<TSource, TTarget, 4u>(source, target, width, height, bias, scale, sourcePaddingElements, targetPaddingElements, worker);
			return true;

		default:
			ocean_assert(false && "Invalid number of channels!");
			return false;
	}

	ocean_assert(false && "Should never be here");
	return false;
}

template <typename T>
void FrameNormalizer::normalize1ChannelIntegerToUint8Subset(const T* source, uint8_t* target, const unsigned int width, const unsigned int height, const T minimalValue, const typename UnsignedTyper<T>::Type range, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows)
#if defined(OCEAN_CLANG_VERSION) && OCEAN_CLANG_VERSION > 0
	__attribute__((no_sanitize("signed-integer-overflow")))
#endif
{
	static_assert(!std::is_floating_point<T>::value, "Invalid data type!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert_and_suppress_unused(firstRow + numberRows <= height, height);

	const unsigned int sourceStrideElements = width + sourcePaddingElements;
	const unsigned int targetStrideElements = width + targetPaddingElements;

	typedef typename UnsignedTyper<T>::Type UnsignedType;
	typedef typename DataTypeMultiple255<UnsignedType>::Type NextLargerUnsignedType;

	const NextLargerUnsignedType range_2 = NextLargerUnsignedType(range) / NextLargerUnsignedType(2);

	source += firstRow * sourceStrideElements;
	target += firstRow * targetStrideElements;

	for (unsigned int nRow = firstRow; nRow < firstRow + numberRows; ++nRow)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			ocean_assert((std::is_same<T, uint64_t>::value || std::is_same<T, int64_t>::value) || (double(*source) >= double(minimalValue) && double(*source) <= double(minimalValue) + double(range)));
			ocean_assert(double(NextLargerUnsignedType(UnsignedType(*source - minimalValue)) * NextLargerUnsignedType(0xFF) + range_2) / double(NextLargerUnsignedType(range)) >= 0.0);
			ocean_assert(double(NextLargerUnsignedType(UnsignedType(*source - minimalValue)) * NextLargerUnsignedType(0xFF) + range_2) / double(NextLargerUnsignedType(range)) < 256.0);

			*target++ = uint8_t((NextLargerUnsignedType(UnsignedType(*source++ - minimalValue)) * NextLargerUnsignedType(0xFF) + range_2) / NextLargerUnsignedType(range));
		}

		source += sourcePaddingElements;
		target += targetPaddingElements;
	}
}

template <typename T, typename TRange>
void FrameNormalizer::normalize1ChannelFloatToUint8Subset(const T* source, uint8_t* target, const unsigned int width, const unsigned int height, const T minimalValue, const TRange range, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(std::is_floating_point<TRange>::value, "Invalid data type!");
	static_assert(sizeof(T) <= sizeof(TRange), "TRange range must not be smaller than T");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert_and_suppress_unused(firstRow + numberRows <= height, height);

	const unsigned int sourceStrideElements = width + sourcePaddingElements;
	const unsigned int targetStrideElements = width + targetPaddingElements;

	/*
	 * Value range mapping:
	 *
	 * [minValue,                                                         maxValue], maxValue = minValue + range
	 * [   0   |   1   |   2   | ...                    ...   |   254   |   255   ]
	 */

	const TRange invRange = TRange(255.999) / range; // generously staying below 256 to avoid that we run out of the value range [0, 256) below

	const TRange maxValueThreshold = TRange(minimalValue) + range + range * TRange(0.0001);

	source += firstRow * sourceStrideElements;
	target += firstRow * targetStrideElements;

	for (unsigned int nRow = firstRow; nRow < firstRow + numberRows; ++nRow)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			if (std::is_floating_point<T>::value && NumericT<T>::isInf(*source)) // +/- inf is ignored, so mapping -inf to 0, and +inf to 255
			{
				if (*source > T(0))
				{
					*target = uint8_t(255);
				}
				else
				{
					*target = uint8_t(0);
				}
			}
			else
			{
				ocean_assert_and_suppress_unused(*source >= minimalValue && *source <= maxValueThreshold, maxValueThreshold);

				const TRange normalizedValue = TRange(*source - minimalValue) * invRange;

				ocean_assert(normalizedValue >= TRange(0));
				ocean_assert(normalizedValue < TRange(256));

				*target = uint8_t(normalizedValue);
			}

			++target;
			++source;
		}

		source += sourcePaddingElements;
		target += targetPaddingElements;
	}
}

template <typename TSource, typename TTarget, unsigned int tChannels>
void FrameNormalizer::normalizeToFloatSubset(const TSource* source, TTarget* target, const unsigned int width, const unsigned int height, const TTarget* bias, const TTarget* scale, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(std::is_floating_point<TTarget>::value, "Invalid data type!");
	static_assert(tChannels != 0u, "Invalid number of channels");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(bias != nullptr && scale != nullptr);
	ocean_assert_and_suppress_unused(firstRow + numberRows <= height, height);

	const unsigned int sourceStrideElements = width * tChannels + sourcePaddingElements;
	const unsigned int targetStrideElements = width * tChannels + targetPaddingElements;

	source += firstRow * sourceStrideElements;
	target += firstRow * targetStrideElements;

	for (unsigned int nRow = firstRow; nRow < firstRow + numberRows; ++nRow)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			for (unsigned int c = 0u; c < tChannels; ++c)
			{
				if (std::is_floating_point<TSource>::value && NumericT<TSource>::isInf(*source)) // +/- inf is ignored, so we preserve the value
				{
					*target = TTarget(*source);
				}
				else
				{
					*target = (TTarget(*source) + bias[c]) * scale[c];
				}

				++target;
				++source;
			}
		}

		source += sourcePaddingElements;
		target += targetPaddingElements;
	}
}

template <typename T, typename TRange>
void FrameNormalizer::normalizeLogarithm1ChannelToUint8Subset(const T* source, uint8_t* target, const unsigned int width, const unsigned int height, const Scalar octaves, const T minimalValue, const TRange range, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(octaves > 0 && octaves <= 10);
	ocean_assert(source && target);
	ocean_assert_and_suppress_unused(firstRow + numberRows <= height, height);

	typedef typename FloatType<TRange>::Type FloatType;

	const unsigned int sourceStrideElements = width + sourcePaddingElements;
	const unsigned int targetStrideElements = width + targetPaddingElements;

	const FloatType factor = FloatType(Numeric::pow(10, octaves) - 1) / FloatType(range);
	const FloatType normalization = FloatType(255.999) / FloatType(octaves); // generously staying below 256 to avoid that we run out of the value range [0, 256) below, see normalize1ChannelFloatToUint8Subset()

	source += firstRow * sourceStrideElements;
	target += firstRow * targetStrideElements;

	for (unsigned int nRow = firstRow; nRow < firstRow + numberRows; ++nRow)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			if (std::is_floating_point<T>::value && NumericT<T>::isInf(*source)) // +/- inf is ignored, so mapping -inf to 0, and +inf to 255
			{
				if (*source > T(0))
				{
					*target = uint8_t(255);
				}
				else
				{
					*target = uint8_t(0);
				}
			}
			else
			{
				ocean_assert(*source >= minimalValue && FloatType(*source) <= FloatType(minimalValue) + FloatType(range) + NumericT<FloatType>::eps());
				ocean_assert(NumericT<FloatType>::log10(FloatType(1 + FloatType(*source - minimalValue) * factor)) * normalization >= FloatType(0));
				ocean_assert(NumericT<FloatType>::log10(FloatType(1 + FloatType(*source - minimalValue) * factor)) * normalization < FloatType(256));

				*target = (uint8_t)(NumericT<FloatType>::log10(FloatType(1 + FloatType(*source - minimalValue) * factor)) * normalization);
			}

			++target;
			++source;
		}

		source += sourcePaddingElements;
		target += targetPaddingElements;
	}
}

template <typename TSource, typename TTarget>
void FrameNormalizer::normalizeLogarithm1ChannelToFloatSubset(const TSource* source, TTarget* target, const unsigned int width, const unsigned int height, const Scalar octaves, const TTarget minimalValue, const TTarget range, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(std::is_floating_point<TTarget>::value, "Invalid data type!");

	ocean_assert(octaves > 0 && octaves <= 10);
	ocean_assert(source && target);
	ocean_assert_and_suppress_unused(firstRow + numberRows <= height, height);

	const unsigned int sourceStrideElements = width + sourcePaddingElements;
	const unsigned int targetStrideElements = width + targetPaddingElements;

	const TTarget factor = TTarget(Numeric::pow(10, octaves) - 1) / TTarget(range);
	const TTarget normalization = TTarget(1) / TTarget(octaves);

	source += firstRow * sourceStrideElements;
	target += firstRow * targetStrideElements;

	for (unsigned int nRow = firstRow; nRow < firstRow + numberRows; ++nRow)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			if (std::is_floating_point<TSource>::value && NumericT<TSource>::isInf(*source)) // +/- inf is ignored, so we preserve the value
			{
				*target = TTarget(*source);
			}
			else
			{
				ocean_assert(TTarget(*source) >= minimalValue && TTarget(*source) <= minimalValue + range + NumericT<TTarget>::eps());
				ocean_assert(NumericT<TTarget>::log10(TTarget(1) + (TTarget(*source) - minimalValue) * factor) * normalization >= TTarget(0));
				ocean_assert(NumericT<TTarget>::log10(TTarget(1) + (TTarget(*source) - minimalValue) * factor) * normalization <= TTarget(1) + NumericT<TTarget>::eps());

				*target = NumericT<TTarget>::log10(TTarget(1) + (TTarget(*source) - minimalValue) * factor) * normalization;
			}

			++target;
			++source;
		}

		source += sourcePaddingElements;
		target += targetPaddingElements;
	}
}

template <typename T>
inline T FrameNormalizer::epsilon()
{
	return T(1);
}

template <>
inline float FrameNormalizer::epsilon<float>()
{
	return NumericT<float>::eps();
}

template <>
inline double FrameNormalizer::epsilon<double>()
{
	return NumericT<double>::eps();
}

}

}

#endif // META_OCEAN_CV_FRAME_NORMALIZER_H
