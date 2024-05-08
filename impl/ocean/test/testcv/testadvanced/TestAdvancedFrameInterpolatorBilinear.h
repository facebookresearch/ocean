/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTADVANCED_TEST_ADVANCED_FRAME_INTERPOLATOR_BILINEAR_H
#define META_OCEAN_TEST_TESTCV_TESTADVANCED_TEST_ADVANCED_FRAME_INTERPOLATOR_BILINEAR_H

#include "ocean/test/testcv/testadvanced/TestCVAdvanced.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/PixelBoundingBox.h"

#include "ocean/math/Vector2.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestAdvanced
{

/**
 * This class implements a bilinear frame interpolator test.
 * @ingroup testcvadvanced
 */
class OCEAN_TEST_CV_ADVANCED_EXPORT TestAdvancedFrameInterpolatorBilinear
{
	protected:

		/**
		 * Definition of individual types of implementation.
		 */
		enum ImplementationType : uint32_t
		{
			/// The naive implementation.
			IT_NAIVE,
			/// The template-based implementation.
			IT_TEMPLATE,
			/// The SSE-based implementation.
			IT_SSE,
			/// The NEON-based implementation.
			IT_NEON,
			/// The default implementation (which is actually used by default).
			IT_DEFAULT
		};

	public:

		/**
		 * Tests all advanced bilinear interpolation filter functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Tests the pixel interpolation function for frames with 8 bit per channel and mask.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testInterpolatePixelWithMask8BitPerChannel(const double testDuration);

		/**
		 * Tests the pixel interpolation function for frames with 8 bit per channel and mask.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param pixelCenter The pixel center to be used
		 * @return True, if succeeded
		 * @tparam TScalar The data type of the scalar to be used, either 'float' or 'double'
		 */
		template <typename TScalar>
		static bool testInterpolatePixelWithMask8BitPerChannel(const CV::PixelCenter pixelCenter, const double testDuration);

		/**
		 * Tests the bilinear square interpolation.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testInterpolateSquare(const double testDuration);

		/**
		 * Tests the bilinear interpolation of an image patch with mask.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testInterpolatePatchWithMask(const double testDuration);

		/**
		 * Tests the bilinear square interpolation mirrored at the frame borders.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testInterpolateSquareMirroredBorder(const double testDuration);

		/**
		 * Tests the bilinear square interpolation.
		 * @param width The width of the test image, in pixel, with range [tSize, infinity)
		 * @param height The height of the test image, in pixel, with range [tSize, infinity)
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 * @tparam tChannels The number of data channels, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 * @tparam tPixelCenter The pixel center to be used during interpolation, either 'PC_TOP_LEFT' or 'PC_CENTER'
		 */
		template <unsigned int tChannels, unsigned int tPatchSize, CV::PixelCenter tPixelCenter>
		static bool testInterpolateSquare(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the bilinear interpolation of an image patch with mask.
		 * @param width The width of the test image, in pixel, with range [tSize, infinity)
		 * @param height The height of the test image, in pixel, with range [tSize, infinity)
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 * @tparam tChannels The number of data channels, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity)
		 * @tparam tPixelCenter The pixel center to be used during interpolation, either 'PC_TOP_LEFT' or 'PC_CENTER'
		 */
		template <unsigned int tChannels, unsigned int tPatchSize, CV::PixelCenter tPixelCenter>
		static bool testInterpolatePatchWithMask(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the bilinear square mirrored interpolation.
		 * Pixels inside the blocks mapping outside the frames are mirrored back into the frame.
		 * @param width The width of the test image, in pixel, with range [tSize, infinity)
		 * @param height The height of the test image, in pixel, with range [tSize, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam tChannels The number of data channels, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static bool testInterpolateSquareMirroredBorder(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Interpolates a square image patch with sub-pixel accuracy.
		 * @param frame The frame in which the image patch is located, must be valid
		 * @param patchWidth The width of the square patch in pixel, with range [1, infinity), must be odd
		 * @param patchHeight The height of the square patch in pixel, with range [1, infinity), must be odd
		 * @param position The center position of the square image patch with range [patchSize/2, width - patchSize/2 - 1)x[patchSize/2, height - patchSize/2 - 1)
		 * @param pixelCenter The definition of the pixel center when applying the interpolation
		 * @param buffer The resulting (continuous) buffer of the interpolated image patch, must be valid
		 * @return True, if succeeded
		 */
		static bool interpolatePatch8BitPerChannel(const Frame& frame, const unsigned int patchWidth, const unsigned int patchHeight, const Vector2& position, const CV::PixelCenter pixelCenter, uint8_t* buffer);

		/**
		 * Interpolates an image patch and mask with sub-pixel accuracy.
		 * @param frame The frame in which the image patch is located, must be valid
		 * @param mask The mask defining valid and invalid image pixels, must be valid
		 * @param patchWidth The width of the square patch in pixel, with range [1, infinity)
		 * @param patchHeight The height of the square patch in pixel, with range [1, infinity)
		 * @param position The center position of the square image patch with range [patchSize/2, width - patchSize/2 - 1)x[patchSize/2, height - patchSize/2 - 1)
		 * @param pixelCenter The definition of the pixel center when applying the interpolation
		 * @param patchBuffer The resulting (continuous) buffer of the interpolated image patch, must be valid
		 * @param patchMaskBuffer The resulting (continuous) buffer of the mask, must be valid
		 * @param validMaskValue The mask value of a valid pixel, with range [0, 255]
		 * @return True, if succeeded
		 */
		static bool interpolatePatchWithMask8BitPerChannel(const Frame& frame, const Frame& mask, const unsigned int patchWidth, const unsigned int patchHeight, const Vector2& position, const CV::PixelCenter pixelCenter, uint8_t* patchBuffer, uint8_t* patchMaskBuffer, const uint8_t validMaskValue);

		/**
		 * Interpolates a square image patch with sub-pixel accuracy (mirrored at the image borders).
		 * The center of a pixel is expected to be located at the top-left corner of a pixel.
		 * @param frame The frame in which the image patch is located, must be valid
		 * @param patchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 * @param position The center position of the square image patch with range [patchSize/2, width - patchSize/2 - 1)x[patchSize/2, height - patchSize/2 - 1)
		 * @param buffer The resulting (continuous) buffer of the interpolated image patch, must be valid
		 */
		static void interpolateSquarePatchMirroredBorder8BitPerChannel(const Frame& frame, const unsigned int patchSize, const Vector2& position, uint8_t* buffer);

		/**
		 * Tests the homography transformation function defining a binary mask for known and unknown image content.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testHomographyFilterMask(const double testDuration, Worker& worker);

		/**
		 * Tests the homography transformation function (with binary mask defining known and unknown image content) for a given frame dimension and channel number.
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param channels The number of frame channels, with range [1, 4]
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testHomographyFilterMask(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker);

	protected:

		/**
		 * Validates a pixel interpolation result for frame with 8 bit per channel with mask.
		 * @param frame The frame in pixel the pixel interpolation was applied, must be valid
		 * @param mask The mask defining valid and invalid pixels, must be valid
		 * @param position The position for which the interpolated pixel will be determined, with ranges [0, frame.width() - 1]x[0, frame.height() - 1] for PC_TOP_LEFT, [0, frame.width()]x[0, frame.height()] for PC_CENTER
		 * @param pixelCenter The pixel center to be used during interpolation
		 * @param maskValue The mask pixel value defining a valid pixel, an invalid pixel is defined by 0xFF - maskValue
		 * @param interpolationResult The interpolation result to be verified, one for each frame channel
		 * @param maskResult The mask result to be verified
		 * @param threshold The maximal distance between interpolated result and ground-truth result so that the result is valid, with range [0, 255)
		 * @return True, if the interpolation is correct
		 * @tparam TScalar The data type of a scalar, either 'float' or 'double'
		 */
		template <typename TScalar>
		static bool validateInterpolatePixel8BitPerChannel(const Frame& frame, const Frame& mask, const VectorT2<TScalar>& position, const CV::PixelCenter pixelCenter, const uint8_t maskValue, const uint8_t* const interpolationResult, const uint8_t maskResult, const TScalar threshold);

		/**
		 * Validates the homography interpolation function (using a binary mask to define output pixels which will be interpolated).
		 * @param inputFrame The frame which will be interpolated based on the homography, must be valid
		 * @param outputFilterMask The filter mask defining output pixel which will be interpolated, must be valid
		 * @param outputFrame The original output frame before the interpolation was applied, must be valid
		 * @param interpolatedOutputFrame The interpolated frame to verify, must be valid
		 * @param input_H_output The homography that has been used to interpolate/warp the frame, transforming points in the output frame to points in the input frame, must not be singular
		 * @param boundingBox Optional bounding box to apply the interpolation to a subset of the output frame only, invalid to handle the entire output frame
		 * @return True, if the interpolation is correct
		 */
		static bool validateHomographyFilterMask8BitPerChannel(const Frame& inputFrame, const Frame& outputFilterMask, const Frame& outputFrame, const Frame& interpolatedOutputFrame, const SquareMatrix3& input_H_output, const CV::PixelBoundingBox& boundingBox);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTADVANCED_TEST_ADVANCED_FRAME_INTERPOLATOR_BILINEAR_H
