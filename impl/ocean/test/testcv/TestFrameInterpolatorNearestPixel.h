/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_FRAME_INTERPOLATOR_NEAREST_PIXEL_H
#define META_OCEAN_TEST_TESTCV_TEST_FRAME_INTERPOLATOR_NEAREST_PIXEL_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/base/Worker.h"

#include "ocean/cv/FrameInterpolatorNearestPixel.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements a nearest pixel frame interpolator test.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameInterpolatorNearestPixel
{
	public:

		/**
		 * Tests all nearest pixel interpolation filter functions.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Test for affine image transformations.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testAffine(const double testDuration, Worker& worker);

		/**
		 * Test for affine image transformations for varying frame dimensions and channel numbers.
		 * @param width The width of the frame in pixel, if this value is 0 (or `height`) it will be randomly selected in the range [1, 1920], range: [0, infinity)
		 * @param height The height of the frame in pixel, if the value is 0 (or `width`) it will be randomly selected in the range [1, 1080], range: [0, infinity)
		 * @param channels The number of frame channels, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testAffine(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker);

		/**
		 * Tests the homography transformation function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 * @tparam T The data type of each pixel element, e.g., 'uint8_t', 'float'
		 */
		template <typename T>
		static bool testHomography(const double testDuration, Worker& worker);

		/**
		 * Tests the homography transformation function for a given frame dimension and channel number.
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param channels The number of frame channels, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 * @tparam T The data type of each pixel element, e.g., 'uint8_t', 'float'
		 */
		template <typename T>
		static bool testHomography(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker);

		/**
		 * Tests the homography transformation function defining a binary mask for known and unknown image content.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testHomographyMask(const double testDuration, Worker& worker);

		/**
		 * Tests the homography transformation function (with binary mask defining known and unknown image content) for a given frame dimension and channel number.
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param channels The number of frame channels, with range [1, 4]
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testHomographyMask(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker);

		/**
		 * Tests the resize function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testResize(const double testDuration, Worker& worker);

		/**
		 * Tests the resize function for a given frame dimension and channel number.
		 * @param sourceWidth Width of the source frame in pixel, with range [1, infinity)
		 * @param sourceHeight Height of the source frame in pixel, with range [1, infinity)
		 * @param targetWidth Width of the target frame in pixel, with range [1, infinity)
		 * @param targetHeight Height of the target frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 * @tparam T The data type of each elements, e.g., 'unsigned char', 'float'
		 * @tparam tChannels The number of channels the frames have, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static bool testResize(const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const double testDuration, Worker& worker);

		/**
		 * Tests the special case resize function for image resolutions from 400x400 to 224x224.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSpecialCasesResize400x400To224x224_8BitPerChannel(const double testDuration);

		/**
		 * Tests the frame transformation function applying a lookup table.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 * @tparam tOffset True, to use an offset transformation; False, to use an absolute transformation
		 */
		template <bool tOffset>
		static bool testTransform(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the frame mask transformation function applying a lookup table.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 * @tparam tOffset True, to use an offset transformation; False, to use an absolute transformation
		 */
		template <bool tOffset>
		static bool testTransformMask(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the 90 degree rotate function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testRotate90(const double testDuration, Worker& worker);

		/**
		 * Tests the 90 degree rotate function for a specific frame size and number of channels
		 * @param width The width of the test frame in pixels, range: [1, infinity)
		 * @param height The height of the test frame in pixels, range: [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 * @tparam TElementType The data type of each elements, e.g., 'unsigned char', 'float'
		 * @tparam tChannels The number of channels the frames have, with range [1, infinity)
		 */
		template <typename TElementType, unsigned int tChannels>
		static bool testRotate90(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Validation function for the nearest-neighbor interpolation of 2D homogeneous image transformations (+ constant background color for unknown image content).
		 * @param frame Pointer to the source frame, must be valid
		 * @param frameWidth Width of the source frame, range: [0, infinity)
		 * @param frameHeight Height of the source frame, range: [0, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each source frame row, in elements, with range [0, infinity)
		 * @param interpolatedFrame Pointer to the target frame, must be valid
		 * @param interpolatedFrameWidth Width of the target frame, range: [0, infinity)
		 * @param interpolatedFrameHeight Height of the target frame, range: [0, infinity)
		 * @param interpolatedFramePaddingElements The number of padding elements at the end of each target frame row, in elements, with range [0, infinity)
		 * @param channels Number of channels in both, the source and the target frame, range: [1, infinity)		 * @param homography The 2D homogeneous image transformation that has been used to interpolate/warp the frame, transforming points defined in the interpolatedFrame to the source frame, i.e. pointFrame = transformation * pointInterpolatedFrame, must not be singular
		 * @param backgroundColor The background color for all pixels for which no valid source pixel exists, one for each frame channel, must be valid
		 * @param interpolatedFrameOrigin The origin of the interpolated frame defining the global position of the interpolated frame's pixel coordinate (0, 0), with range (-infinity, infinity)x(-infinity, infinity)
		 * @param maximalAbsError If specified, will hold the maximum absolute error measured
		 * @param averageAbsError If specified, will hold the average of all absolute pixel errors
		 * @param groundtruth If specified, the result of the validation (which is used as ground truth) will be stored at the specified pointer location, note: memory must be allocated (same size and frame type as the input frames)
		 * @tparam T The data type of each pixel element
		 */
		template <typename T>
		static void validateHomography(const T* frame, const unsigned int frameWidth, const unsigned int frameHeight, const unsigned int framePaddingElements, const T* interpolatedFrame, const unsigned int interpolatedFrameWidth, const unsigned int interpolatedFrameHeight, const unsigned int interpolatedFramePaddingElements, const unsigned int channels, const SquareMatrix3& homography, const T* backgroundColor, const CV::PixelPositionI& interpolatedFrameOrigin, double* maximalAbsError = nullptr, double* averageAbsError = nullptr, T* groundtruth = nullptr);

		/**
		 * Validates a resized frame.
		 * @param source The source frame which has been resized, must be valid
		 * @param target The target frame to validate, must be valid
		 * @param sourceWidth Width of the source frame in pixel, with range [1, infinity)
		 * @param sourceHeight Height of the source frame in pixel, with range [1, infinity)
		 * @param targetWidth Width of the target frame in pixel, with range [1, infinity)
		 * @param targetHeight Height of the target frame in pixel, with range [1, infinity)
		 * @param channels The number of frame channels, with range [1, infinity)
		 * @param sourcePaddingElements The optional number of padding elements at the end of each row of the source frame, in elements, with range [0, infinity)
		 * @param targetPaddingElements The optional number of padding elements at the end of each row of the target frame, in elements, with range [0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of each element e.g., 'unsigned char', 'float'
		 */
		template <typename T>
		static bool validateResizedFrame(const T* source, const T* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int channels, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements);

		/**
		 * Validates the frame transformation function applying a lookup table.
		 * @param frame The frame which will be transformed, must be valid
		 * @param target The target which has received the transformed frame, must be valid
		 * @param lookupTable The lookup table which has been used to transform the image
		 * @param borderColor The border color which has been used, must be valid
		 * @return True, if succeeded
		 * @tparam tOffset True, to use an offset transformation; False, to use an absolute transformation
		 */
		template <bool tOffset>
		static bool validateTransformation(const Frame& frame, const Frame& target, const CV::FrameInterpolatorNearestPixel::LookupTable& lookupTable, const uint8_t* borderColor);

		/**
		 * Validates the frame mask transformation function applying a lookup table.
		 * @param frame The frame which will be transformed, must be valid
		 * @param target The target which has received the transformed frame, must be valid
		 * @param targetMask The target mask which has received the mask of the transformed frame, must be valid
		 * @param lookupTable The lookup table which has been used to transform the image
		 * @param maskValue The pixel value of a mask pixel with corresponding source pixel, with range [0, 255]
		 * @return True, if succeeded
		 * @tparam tOffset True, to use an offset transformation; False, to use an absolute transformation
		 */
		template <bool tOffset>
		static bool validateTransformationMask(const Frame& frame, const Frame& target, const Frame& targetMask, const CV::FrameInterpolatorNearestPixel::LookupTable& lookupTable, const uint8_t maskValue);

		/**
		 * Validates the 90 degree rotate function.
		 * @param frame The original frame, must be valid
		 * @param channels The number of data channels the frame has, with range [1, infinity)
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param clockwise The clockwise rotated frame to be checked, must be valid
		 * @param counterClockwise The counter clockwise frame to be checked, must be valid
		 * @param framePaddingElements Optional number of padding elements at the end of each frame row, in elements, range: [0, infinity)
		 * @param clockwisePaddingElements Optional number of padding elements at the end of each clockwise row, in elements, range: [0, infinity)
		 * @param counterClockwisePaddingElements Optional number of padding elements at the end of each clockwise row, in elements, range: [0, infinity)
		 * @return True, if succeeded
		 * @tparam TElementType The data type of each channel data
		 */
		template <typename TElementType>
		static bool validateRotate90(const TElementType* frame, const unsigned int channels, const unsigned int width, const unsigned int height, const TElementType* clockwise, const TElementType* counterClockwise, const unsigned int framePaddingElements, const unsigned int clockwisePaddingElements, const unsigned int counterClockwisePaddingElements);

	protected:

		/**
		 * Validates the homography interpolation function (using a binary mask to define known and unknown image content).
		 * @param inputFrame The frame which will be interpolated based on the homography, must be valid
		 * @param outputFrame The interpolated/warped frame to be validated, width same pixel format and pixel origin as 'frame, must be valid
		 * @param outputMask The binary mask corresponding to the interpolated/warped frame specifying known and unknown image content, a mask value of 0xFF defined known image content, 0x00 defines unknown image content, with same pixel origin and frame resolution as 'outputFrame'
		 * @param input_H_output The homography transforming output to input points, must be valid
		 * @param outputFrameOrigin The origin of the interpolated frame defining the global position of the interpolated frame's pixel coordinate (0, 0), with range (-infinity, infinity)x(-infinity, infinity)
		 * @return True, if the interpolation is correct
		 */
		static bool validateHomographyMask8BitPerChannel(const Frame& inputFrame, const Frame& outputFrame, const Frame& outputMask, const SquareMatrix3& input_H_output, const CV::PixelPositionI& outputFrameOrigin);
};

template <typename T>
void TestFrameInterpolatorNearestPixel::validateHomography(const T* frame, const unsigned int frameWidth, const unsigned int frameHeight, const unsigned int framePaddingElements, const T* interpolatedFrame, const unsigned int interpolatedFrameWidth, const unsigned int interpolatedFrameHeight, const unsigned int interpolatedFramePaddingElements, const unsigned int channels, const SquareMatrix3& homography, const T* backgroundColor, const CV::PixelPositionI& interpolatedFrameOrigin, double* maximalAbsError, double* averageAbsError, T* groundtruth)
{
	ocean_assert(frame != nullptr && interpolatedFrame != nullptr);
	ocean_assert(frameWidth != 0u && frameHeight != 0u);
	ocean_assert(interpolatedFrameWidth != 0u && interpolatedFrameHeight != 0u);
	ocean_assert(channels != 0u);
	ocean_assert(!homography.isSingular());
	ocean_assert(backgroundColor != nullptr);

	double sumAbsError = 0.0;
	double maxAbsError = 0.0;
	unsigned long long measurements = 0ull;

	for (unsigned int y = 0u; y < interpolatedFrameHeight; ++y)
	{
		for (unsigned int x = 0u; x < interpolatedFrameWidth; ++x)
		{
			const T* const interpolatedPixel = interpolatedFrame + (interpolatedFrameWidth * y + x) * channels + y * interpolatedFramePaddingElements;

			const Vector2 outputPosition = Vector2(Scalar(x) + Scalar(interpolatedFrameOrigin.x()), Scalar(y) + Scalar(interpolatedFrameOrigin.y()));
			const Vector2 inputPosition = homography * outputPosition;

			const int inputXI = Numeric::round32(inputPosition.x());
			const int inputYI = Numeric::round32(inputPosition.y());

			// Ignore the border pixels because background and foreground can be mixed differently here
			if (inputXI == -1 || inputXI == 0 || inputXI == (int)(frameWidth - 1u) || inputXI == (int)frameWidth || inputYI == -1 || inputYI == 0 || inputYI == (int)(frameHeight - 1u) || inputYI == (int)(frameHeight))
			{
				continue;
			}

			const unsigned int inputX = (unsigned int)inputXI;
			const unsigned int inputY = (unsigned int)inputYI;

			if (inputX < frameWidth && inputY < frameHeight)
			{
				const unsigned int nearestNeighborIndex = (inputY * frameWidth + inputX) * channels + inputY * framePaddingElements;

				for (unsigned int n = 0u; n < channels; ++n)
				{
					if (groundtruth)
					{
						*groundtruth = frame[nearestNeighborIndex + n];
						groundtruth++;
					}

					const double interpolatedPixelD = double(interpolatedPixel[n]);
					const double framePixelD = double(frame[nearestNeighborIndex + n]);
					const double absError = NumericD::abs(interpolatedPixelD - framePixelD);

					sumAbsError += absError;

					if (absError > maxAbsError)
					{
						maxAbsError = absError;
					}

					measurements++;
				}
			}
			else
			{
				for (unsigned int n = 0u; n < channels; ++n)
				{
					if (groundtruth)
					{
						*groundtruth = backgroundColor[n];
						groundtruth++;
					}

					const double absError = NumericD::abs(double(interpolatedPixel[n]) - double(backgroundColor[n]));

					sumAbsError += absError;

					if (absError > maxAbsError)
					{
						maxAbsError = absError;
					}

					measurements++;
				}
			}
		}
	}

	if (averageAbsError)
	{
		ocean_assert(measurements != 0ull);
		*averageAbsError = sumAbsError / double(measurements);
	}

	if (maximalAbsError)
	{
		*maximalAbsError = maxAbsError;
	}
}

} // namespace TestCV

} // namespace Test

} // namespace Ocean

#endif // META_OCEAN_TEST_TESTCV_TEST_FRAME_INTERPOLATOR_NEAREST_PIXEL_H
