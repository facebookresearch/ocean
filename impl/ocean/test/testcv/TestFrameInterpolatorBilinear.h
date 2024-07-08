/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_FRAME_INTERPOLATOR_BILINEAR_H
#define META_OCEAN_TEST_TESTCV_TEST_FRAME_INTERPOLATOR_BILINEAR_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/PixelPosition.h"

#include "ocean/math/Lookup2.h"
#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/Vector2.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements a bilinear frame interpolation test.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameInterpolatorBilinear
{
	public:

		/**
		 * Tests all bilinear interpolation filter functions.
		 * @param width The width of the source frame in pixel
		 * @param height The height of the source frame in pixel
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the pixel interpolation function for frames with 8 bit per channel.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testInterpolatePixel8BitPerChannel(const double testDuration);

		/**
		 * Tests the pixel interpolation function for frames with 8 bit per channel.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param pixelCenter The pixel center to be used
		 * @return True, if succeeded
		 * @tparam TScalar The data type of the scalar to be used, either 'float' or 'double'
		 */
		template <typename TScalar>
		static bool testInterpolatePixel8BitPerChannel(const CV::PixelCenter pixelCenter, const double testDuration);

		/**
		 * Tests the pixel interpolation function for frames with arbitrary data type.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testInterpolatePixel(const double testDuration);

		/**
		 * Tests the pixel interpolation function for frames arbitrary data type.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param pixelCenter The pixel center to be used
		 * @return True, if succeeded
		 * @tparam TSource The data type of the source to be used
		 * @tparam TTarget The data type of the target to be used
		 * @tparam TScalar The data type of the scalar to be used, either 'float' or 'double'
		 */
		template <typename TSource, typename TTarget, typename TScalar>
		static bool testInterpolatePixel(const CV::PixelCenter pixelCenter, const double testDuration);

		/**
		 * Tests the affine transformation function using a constant color for unknown image content.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testAffine(const double testDuration, Worker& worker);

		/**
		 * Tests the homography transformation function supporting arbitrary pixel formats using a constant color for unknown image content.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 * @tparam T The data type of each pixel channel, e.g., 'float', 'double', 'int', ...
		 */
		template <typename T>
		static bool testHomography(const double testDuration, Worker& worker);

		/**
		 * Tests the homography transformation function defining a binary mask for known and unknown image content.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testHomographyMask(const double testDuration, Worker& worker);

		/**
		 * Tests the bilinear resize function for extreme image resolutions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testResizeExtremeResolutions(const double testDuration, Worker& worker);

		/**
		 * Tests the bilinear resize function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testResize(const double testDuration, Worker& worker);

		/**
		 * Tests the bilinear resize function supporting arbitrary data types.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 * @tparam T The data type of each pixel channel, e.g., `uint8_t`, 'float', 'double', ...
		 */
		template <typename T>
		static bool testResize(const double testDuration, Worker& worker);

		/**
		 * Tests the frame transformation function applying a lookup table.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testLookup(const double testDuration, Worker& worker);

		/**
		 * Tests the frame transformation function applying a lookup table.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 * @tparam T The data type of each pixel channel, e.g., `uint8_t`, 'float', 'double', ...
		 */
		template <typename T>
		static bool testLookup(const double testDuration, Worker& worker);

		/**
		 * Tests the frame mask transformation function applying a lookup table.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testLookupMask(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the frame rotate function.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testRotateFrame(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the intensity sum of an image patch with sub-pixel location in a 1-channel frame using an integral image.
		 * @param width The width of the frame in pixel, with range [64, infinity)
		 * @param height The height of the frame in pixel, with range [64, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPatchIntensitySum1Channel(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the function for affine transformations (with constant color for unknown image content) for a given frame dimension and channel number.
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param channels The number of frame channels, with range [1, 4]
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testAffine(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker);

		/**
		 * Tests the homography transformation function (with constant color for unknown image content) for arbitrary data types and for a given frame dimension and channel number.
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param channels The number of frame channels, with range [1, 4]
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 * @tparam T The data type of each pixel channel, e.g., 'float', 'double', 'int', ...
		 */
		template <typename T>
		static bool testHomography(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker);

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
		 * Tests the bilinear resize function for a given frame dimension and channel number.
		 * @param sourceWidth Width of the source frame in pixel
		 * @param sourceHeight Height of the source frame in pixel
		 * @param sourceChannels Number of the data channels of the source frame, with range [1, 4]
		 * @param targetWidth Width of the target frame in pixel
		 * @param targetHeight Height of the target frame in pixel
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testResize(const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int sourceChannels, const unsigned int targetWidth, const unsigned int targetHeight, const double testDuration, Worker& worker);

		/**
		 * Tests the bilinear resize function for arbitrary data types and for a given frame dimension and channel number.
		 * @param sourceWidth Width of the source frame in pixel
		 * @param sourceHeight Height of the source frame in pixel
		 * @param sourceChannels Number of the data channels of the source frame
		 * @param targetWidth Width of the target frame in pixel
		 * @param targetHeight Height of the target frame in pixel
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 * @tparam T The data type of each pixel channel, e.g., 'float', 'double', 'int', ...
		 */
		template <typename T>
		static bool testResize(const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int sourceChannels, const unsigned int targetWidth, const unsigned int targetHeight, const double testDuration, Worker& worker);

		/**
		 * Tests the special case resize function for image resolutions from 400x400 to 256x256.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSpecialCasesResize400x400To256x256_8BitPerChannel(const double testDuration);

		/**
		 * Tests the special case resize function for image resolutions from 400x400 to 224x224.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSpecialCasesResize400x400To224x224_8BitPerChannel(const double testDuration);

		/**
		 * Tests the frame transformation function applying a lookup table.
		 * @param width The width of the test frame in pixel, with range [20, infinity)
		 * @param height The height of the test frame in pixel, with range [20, infinity)
		 * @param channels The number of frame channels, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 * @tparam T The data type of each pixel channel, e.g., 'float', 'double', 'int', ...
		 */
		template <typename T>
		static bool testLookup(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker);

		/**
		 * Tests the function to re-sample a camera image.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testResampleCameraImage(const double testDuration, Worker& worker);

		/**
		 * Tests the function to re-sample a camera image.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 * @tparam T The data type of each pixel channel, e.g., 'float', 'double', 'int', ...
		 */
		template <typename T>
		static bool testResampleCameraImage(const double testDuration, Worker& worker);

		/**
		 * Validates the bilinear frame resize function.
		 * @param source The source frame that has been resized, must be valid
		 * @param sourceWidth The width of the source frame in pixel, with range [1, infinity)
		 * @param sourceHeight The height of the source frame in pixel, with range [1, infinity)
		 * @param channels Number of the data channels of the source (and target) frame, with range [1, 4]
		 * @param target The resized target frame to be verified, must be valid
		 * @param targetWidth The width of the target frame in pixel, with range [1, infinity)
		 * @param targetHeight The height of the target frame in pixel, with range [1, infinity)
		 * @param xTargetToSource The horizontal scale factor transforming a location in the target frame to a location in the source frame, with range (0, infinity)
		 * @param yTargetToSource The vertical scale factor transforming a location in the target frame to a location in the source frame, with range (0, infinity)
		 * @param sourcePaddingElements Optional padding at the end of each source row in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional padding at the end of each target row in elements, with range [0, infinity)
		 * @param averageAbsErrorToInteger Optional resulting average absolute error between the converted result and the ground truth integer result (rounded result), with range [0, 256)
		 * @param maximalAbsErrorToInteger Optional resulting maximal absolute error between the converted result and the ground truth integer result (rounded result), with range [0, 256)
		 * @param groundTruth Optional resulting ground truth data (the resized image content determined with floating point accuracy), nullptr otherwise
		 */
		static void validateScaleFrame(const unsigned char* source, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int channels, const unsigned char* target, const unsigned int targetWidth, const unsigned int targetHeight, const double xTargetToSource, const double yTargetToSource, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, double* averageAbsErrorToInteger, unsigned int* maximalAbsErrorToInteger, unsigned char* groundTruth = nullptr);

		/**
		 * Validates the bilinear frame resize function for arbitrary data types.
		 * @param source The source frame that has been resized, must be valid
		 * @param sourceWidth The width of the source frame in pixel, with range [1, infinity)
		 * @param sourceHeight The height of the source frame in pixel, with range [1, infinity)
		 * @param channels Number of the data channels of the source (and target) frame, with range [1, 4]
		 * @param target The resized target frame to be verified, must be valid
		 * @param targetWidth The width of the target frame in pixel, with range [1, infinity)
		 * @param targetHeight The height of the target frame in pixel, with range [1, infinity)
		 * @param xSource_s_xTarget The horizontal scale factor transforming a location in the target frame to a location in the source frame, with range (0, infinity)
		 * @param ySource_s_yTarget The vertical scale factor transforming a location in the target frame to a location in the source frame, with range (0, infinity)
		 * @param sourcePaddingElements Optional padding at the end of each source row in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional padding at the end of each target row in elements, with range [0, infinity)
		 * @param averageAbsError Optional resulting average absolute error between the converted result and the ground truth result, with range (-infinity, infinity)
		 * @param maximalAbsError Optional resulting maximal absolute error between the converted result and the ground truth result, with range (-infinity, infinity)
		 * @param groundTruth Optional resulting ground truth data (the resized image content determined with floating point accuracy), nullptr otherwise
		 * @tparam T The data type of each pixel channel, e.g., 'float', 'double', 'int', ...
		 */
		template <typename T>
		static void validateScaleFrame(const T* source, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int channels, const T* target, const unsigned int targetWidth, const unsigned int targetHeight, const double xSource_s_xTarget, const double ySource_s_yTarget, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, double* averageAbsError, double* maximalAbsError, T* groundTruth = nullptr);

		/**
		 * Validates the homography interpolation function for (almost) arbitrary pixel formats (using a constant background color for unknown image content).
		 * @param input The input frame which has been used for the interpolation, must be valid
		 * @param output The output frame holding the interpolated image information of the input frame, must be valid
		 * @param input_H_output The homography that has been used to interpolate/warp the frame, transforming points int he output frame to points in the input frame, must not be singular
		 * @param backgroundColor The background color for all pixels for which no valid source pixel exists, one for each frame channel, must be valid
		 * @param interpolatedFrameOrigin The origin of the interpolated frame defining the global position of the interpolated frame's pixel coordinate (0, 0), with range (-infinity, infinity)x(-infinity, infinity)
		 * @param averageAbsError Optional resulting average absolute error between the converted result and the ground truth result, with range (-infinity, infinity)
		 * @param maximalAbsError Optional resulting maximal absolute error between the converted result and the ground truth result, with range (-infinity, infinity)
		 * @param groundTruth Optional resulting ground truth frame (the resized image content determined with floating point accuracy), nullptr if not of interest
		 * @tparam T The data type of each pixel channel, e.g., 'float', 'double', 'int', ...
		 */
		template <typename T>
		static void validateHomography(const Frame& input, const Frame& output, const SquareMatrix3& input_H_output, const T* backgroundColor, const CV::PixelPositionI& interpolatedFrameOrigin, double* averageAbsError, double* maximalAbsError, Frame* groundTruth = nullptr);

	protected:

		/**
		 * Tests the intensity sum of an image patch with sub-pixel location in a 1-channel frame using an integral image.
		 * @param width The width of the frame, in pixel, with range [patchWidth + 1u, infinity)
		 * @param height The height of the frame, in pixel, with range [patchHeight + 1u, infinity)
		 * @param patchWidth Width of the patch, in pixel, with range [1, infinity)
		 * @param patchHeight Height of the patch, in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPatchIntensitySum1Channel(const unsigned int width, const unsigned int height, const unsigned int patchWidth, const unsigned int patchHeight, const double testDuration);

		/**
		 * Validates a pixel interpolation result for frame with 8 bit per channel.
		 * @param frame The frame in pixel the pixel interpolation was applied, must be valid
		 * @param position The position for which the interpolated pixel will be determined, with ranges [0, frame.width() - 1]x[0, frame.height() - 1] for PC_TOP_LEFT, [0, frame.width()]x[0, frame.height()] for PC_CENTER
		 * @param pixelCenter The pixel center to be used during interpolation
		 * @param interpolationResult The interpolation result to be verified, one for each frame channel
		 * @param threshold The maximal distance between interpolated result and ground-truth result so that the result is valid, with range [0, 255)
		 * @return True, if the interpolation is correct
		 * @tparam TScalar The data type of a scalar, either 'float' or 'double'
		 */
		template <typename TScalar>
		static bool validateInterpolatePixel8BitPerChannel(const Frame& frame, const VectorT2<TScalar>& position, const CV::PixelCenter pixelCenter, const uint8_t* const interpolationResult, const TScalar threshold);

		/**
		 * Validates a pixel interpolation result for frame with arbitrary data type.
		 * @param frame The frame in pixel the pixel interpolation was applied, must be valid
		 * @param position The position for which the interpolated pixel will be determined, with ranges [0, frame.width() - 1]x[0, frame.height() - 1] for PC_TOP_LEFT, [0, frame.width()]x[0, frame.height()] for PC_CENTER
		 * @param pixelCenter The pixel center to be used during interpolation
		 * @param interpolationResult The interpolation result to be verified, one for each frame channel
		 * @param threshold The maximal distance between interpolated result and ground-truth result so that the result is valid, with range [0, 255)
		 * @return True, if the interpolation is correct
		 * @tparam TSource The data type of the source to be used
		 * @tparam TTarget The data type of the target to be used
		 * @tparam TScalar The data type of a scalar, either 'float' or 'double'
		 */
		template <typename TSource, typename TTarget, typename TScalar>
		static bool validateInterpolatePixel(const Frame& frame, const VectorT2<TScalar>& position, const CV::PixelCenter pixelCenter, const TTarget* const interpolationResult, const TScalar threshold);

		/**
		 * Validation function for the bilinear interpolation of 2D homogeneous image transformations (+ constant background color for unknown image content).
		 * @param source The frame which will be interpolated based on the homography, must be valid
		 * @param validationTarget The interpolated frame that will be validated, must the same pixel format and origin as 'source' and must be valid
		 * @param source_H_target The 2D homogeneous image transformation that has been used to interpolate/warp the frame, transforming points defined in the interpolatedFrame to the source frame, i.e. pointFrame = transformation * pointInterpolatedFrame, must not be singular
		 * @param backgroundColor The background color for all pixels for which no valid source pixel exists, one for each frame channel, must be valid
		 * @param interpolatedFrameOrigin The origin of the interpolated frame defining the global position of the interpolated frame's pixel coordinate (0, 0), with range (-infinity, infinity)x(-infinity, infinity)
		 * @return True, if the interpolation is correct
		 */
		static bool validateTransformation8BitPerChannel(const Frame& source, const Frame& validationTarget, const SquareMatrix3& source_H_target, const uint8_t* backgroundColor, const CV::PixelPositionI& interpolatedFrameOrigin);

		/**
		 * Validates the homography interpolation function (using a binary mask to define known and unknown image content).
		 * @param frame The frame which will be interpolated based on the homography, must be valid
		 * @param interpolatedFrame The interpolated/warped frame to be validated, width same pixel format and pixel origin as 'frame, must be valid
		 * @param interpolatedMask The binary mask corresponding to the interpolated/warped frame specifying known and unknown image content, a mask value of 0xFF defined known image content, 0x00 defines unknown image content, with same pixel origin and frame resolution as 'interpolatedFrame'
		 * @param input_H_output The homography that has been used to interpolate/warp the frame, transforming points in the output frame to points in the input frame, must not be singular
		 * @param interpolatedFrameOrigin The origin of the interpolated frame defining the global position of the interpolated frame's pixel coordinate (0, 0), with range (-infinity, infinity)x(-infinity, infinity)
		 * @return True, if the interpolation is correct
		 */
		static bool validateHomographyMask8BitPerChannel(const Frame& frame, const Frame& interpolatedFrame, const Frame& interpolatedMask, const SquareMatrix3& input_H_output, const CV::PixelPositionI& interpolatedFrameOrigin);

		/**
		 * Validates the frame transformation function applying a lookup table.
		 * @param sourceFrame The source frame which has been transformed, must be valid
		 * @param targetFrame The target frame which holds the transformed input frame, must be valid
		 * @param lookupTable The lookup table which has been used for the transformation, must be valid
		 * @param offset True, to interpret the lookup values as offset values; False, to interpret the lookup values as absolute locations
		 * @param backgroundColor The background color to be used, must be valid
		 * @return True, if succeeded
		 * @tparam T The data type of each pixel channel, e.g., 'float', 'double', 'int', ...
		 */
		template <typename T>
		static bool validateLookup(const Frame& sourceFrame, const Frame& targetFrame, const LookupCorner2<Vector2>& lookupTable, const bool offset, const T* backgroundColor);

		/**
		 * Validates the frame mask transformation function applying a lookup table.
		 * @param sourceFrame The source frame which has been transformed, must be valid
		 * @param targetFrame The target frame which holds the transformed input frame, must be valid
		 * @param targetMask The mask associated with the target frame, must be valid
		 * @param lookupTable The lookup table which has been used for the transformation, must be valid
		 * @param offset True, to interpret the lookup values as offset values; False, to interpret the lookup values as absolute locations
		 * @return True, if succeeded
		 */
		static bool validateLookupMask(const Frame& sourceFrame, const Frame& targetFrame, const Frame& targetMask, const LookupCorner2<Vector2>& lookupTable, const bool offset);

		/**
		 * Validates the rotation of a frame using a bilinear interpolation.
		 * @param source The source frame to be rotated, must be valid
		 * @param target The target frame which will receive the rotated image, with same frame type as the source frame, must be valid
		 * @param anchorX Position of the rotation anchor in the horizontal direction, with range (-infinity, infinity)
		 * @param anchorY Position of the rotation anchor in the vertical direction, with range (-infinity, infinity)
		 * @param angle The counter clockwise rotation angle in radian, with range [0, 2PI)
		 */
		static bool validateRotatedFrame(const Frame& source, const Frame& target, const Scalar anchorX, const Scalar anchorY, const Scalar angle);

		/**
		 * Validate the intensity sum of an image patch with sub-pixel location in a 1-channel frame.
		 * @param yFrame The frame in which the intensity sum is determined, with pixel format FORMAT_Y8, must be valid
		 * @param patchWidth The width of the patch, in pixel, with range [1, infinity)
		 * @param patchHeight The height of the patch, in pixel, with range [1, infinity)
		 * @param center The center of the image patch to be used
		 * @param pixelCenter The pixel center to be used
		 * @param intensity The intensity sum to verify
		 * @return True, if succeeded
		 */
		static bool validatePatchIntensitySum1Channel(const Frame& yFrame, const unsigned int patchWidth, const unsigned int patchHeight, const Vector2& center, const CV::PixelCenter pixelCenter, const Scalar intensity);

		/**
		 * Validate the bilinear extraction of frame patches
		 * @param source Pointer to the data of the source frame, must be valid
		 * @param validationTarget Pointer to the data of the target frame that will validated, must be valid
		 * @param sourceWidth Width of the source frame in pixels, range: [targetWidth, infinity)
		 * @param sourceHeight Height of the source frame in pixels, range: [targetWidth, infinity)
		 * @param x Horizontal center position of the patch, range: [targetWidth/2, sourceWidth - targetWidth/2)
		 * @param y Vertical center position of the patch, range: [targetHeight/2, sourceHeight - targetHeight/2)
		 * @param validationTargetWidth Width of the target frame, range: all odd values in range [1, sourceWidth]
		 * @param validationTargetHeight Height of the target frame, range: all odd values in range [1, sourceHeight]
		 * @param sourcePaddingElements Optional number of padding elements of the source frame
		 * @param validationTargetPaddingElements Optional number of padding elements of the target frame
		 * @return True if the validation was successful, otherwise false.
		 * @tparam tChannels Number of data channels of the given source frame, range: [1, infinity)
		 */
		template <uint32_t tChannels>
		static bool validatePatchFrame8BitPerChannel(const uint8_t* const source, const uint8_t* const validationTarget, const uint32_t sourceWidth, const uint32_t sourceHeight, const Scalar x, const Scalar y, const uint32_t validationTargetWidth, const uint32_t validationTargetHeight, const uint32_t sourcePaddingElements, const uint32_t validationTargetPaddingElements);

		/**
		 * Validates the bilinear frame resize function for uint8_t data types using 7-bit integer precision.
		 * This function first interpolates two rows and stores the interpolated result as uint8_t values, followed by an interpolation within the row.
		 * @param source The source frame that has been resized, must be valid
		 * @param sourceWidth The width of the source frame in pixel, with range [1, infinity)
		 * @param sourceHeight The height of the source frame in pixel, with range [1, infinity)
		 * @param channels Number of the data channels of the source (and target) frame, with range [1, 4]
		 * @param target The resized target frame to be verified, must be valid
		 * @param targetWidth The width of the target frame in pixel, with range [1, infinity)
		 * @param targetHeight The height of the target frame in pixel, with range [1, infinity)
		 * @param xSource_s_xTarget The horizontal scale factor transforming a location in the target frame to a location in the source frame, with range (0, infinity)
		 * @param ySource_s_yTarget The vertical scale factor transforming a location in the target frame to a location in the source frame, with range (0, infinity)
		 * @param sourcePaddingElements Optional padding at the end of each source row in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional padding at the end of each target row in elements, with range [0, infinity)
		 * @param averageAbsError Optional resulting average absolute error between the converted result and the ground truth result, with range (-infinity, infinity)
		 * @param maximalAbsError Optional resulting maximal absolute error between the converted result and the ground truth result, with range (-infinity, infinity)
		 * @param groundTruth Optional resulting ground truth data (the resized image content determined with floating point accuracy), nullptr otherwise
		 */
		static void validateScaleFramePrecision7Bit(const uint8_t* source, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int channels, const uint8_t* target, const unsigned int targetWidth, const unsigned int targetHeight, const double xSource_s_xTarget, const double ySource_s_yTarget, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, double* averageAbsError, double* maximalAbsError, uint8_t* groundTruth = nullptr);
};

template <typename T>
void TestFrameInterpolatorBilinear::validateScaleFrame(const T* source, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int channels, const T* target, const unsigned int targetWidth, const unsigned int targetHeight, const double xSource_s_xTarget, const double ySource_s_yTarget, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, double* averageAbsError, double* maximalAbsError, T* groundTruth)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(sourceWidth != 0u && sourceHeight != 0u);
	ocean_assert(targetWidth != 0u && targetHeight != 0u);
	ocean_assert(channels >= 1u);
	ocean_assert(xSource_s_xTarget > 0.0 && ySource_s_yTarget > 0.0);

	const unsigned int sourceStrideElements = sourceWidth * channels + sourcePaddingElements;
	const unsigned int targetStrideElements = targetWidth * channels + targetPaddingElements;

	std::vector<T> result(channels, T(0));

	if (averageAbsError)
	{
		*averageAbsError = NumericD::maxValue();
	}

	if (maximalAbsError)
	{
		*maximalAbsError = NumericD::maxValue();
	}

	double sumAbsError = 0.0;
	double maxAbsError = 0.0;

	for (unsigned int y = 0u; y < targetHeight; ++y)
	{
		for (unsigned int x = 0u; x < targetWidth; ++x)
		{
			const double sx = minmax(0.0, (double(x) + 0.5) * xSource_s_xTarget - 0.5, double(sourceWidth - 1u));
			const double sy = minmax(0.0, (double(y) + 0.5) * ySource_s_yTarget - 0.5, double(sourceHeight - 1u));

			const unsigned int leftPixel = (unsigned int)sx;
			const unsigned int rightPixel = min(leftPixel + 1u, sourceWidth - 1u);
			ocean_assert(leftPixel < sourceWidth && rightPixel < sourceWidth);

			const unsigned int topPixel = (unsigned int)sy;
			const unsigned int bottomPixel = min(topPixel + 1u, sourceHeight - 1u);
			ocean_assert(topPixel < sourceHeight && bottomPixel < sourceHeight);

			const double rightFactor = sx - double(leftPixel);
			const double bottomFactor = sy - double(topPixel);

			ocean_assert(rightFactor >= 0.0 && rightFactor <= 1.0);
			ocean_assert(bottomFactor >= 0.0 && bottomFactor <= 1.0);

			const double leftFactor = 1.0 - rightFactor;
			const double topFactor = 1.0 - bottomFactor;

			const T* sourceTopLeft = source + sourceStrideElements * topPixel + leftPixel * channels;
			const T* sourceTopRight = source + sourceStrideElements * topPixel + rightPixel * channels;

			const T* sourceBottomLeft = source + sourceStrideElements * bottomPixel + leftPixel * channels;
			const T* sourceBottomRight = source + sourceStrideElements * bottomPixel + rightPixel * channels;

			for (unsigned int n = 0u; n < channels; ++n)
			{
				const double top = double(sourceTopLeft[n]) * leftFactor + double(sourceTopRight[n]) * rightFactor;
				const double bottom = double(sourceBottomLeft[n]) * leftFactor + double(sourceBottomRight[n]) * rightFactor;

				const double interpolated = top * topFactor + bottom * bottomFactor;

				result[n] = T(interpolated);
			}

			const T* const targetValue = target + targetStrideElements * y + x * channels;

			for (unsigned int n = 0u; n < channels; ++n)
			{
				const double absError = NumericD::abs(double(result[n]) - double(targetValue[n]));

				sumAbsError += absError;
				maxAbsError = max(maxAbsError, absError);
			}

			if (groundTruth)
			{
				memcpy(groundTruth + (y * targetWidth + x) * channels, result.data(), sizeof(T) * channels);
			}
		}
	}

	if (averageAbsError)
	{
		*averageAbsError = sumAbsError / double(targetWidth * targetHeight * channels);
	}

	if (maximalAbsError)
	{
		*maximalAbsError = maxAbsError;
	}
}

template <typename T>
void TestFrameInterpolatorBilinear::validateHomography(const Frame& input, const Frame& output, const SquareMatrix3& input_H_output, const T* backgroundColor, const CV::PixelPositionI& interpolatedFrameOrigin, double* averageAbsError, double* maximalAbsError, Frame* groundTruth)
{
	ocean_assert(input.isValid() && output.isValid());
	ocean_assert(input.isPixelFormatCompatible(output.pixelFormat()));

	ocean_assert(!input_H_output.isSingular());
	ocean_assert(backgroundColor != nullptr);

	const Scalar frameBorderEps = Scalar(0.5);

	ocean_assert(input.numberPlanes() == 1u);

	const unsigned int channels = input.channels();
	ocean_assert(channels >= 1u);

	std::vector<T> result(channels, T(0));

	if (averageAbsError != nullptr)
	{
		*averageAbsError = NumericD::maxValue();
	}

	if (maximalAbsError != nullptr)
	{
		*maximalAbsError = NumericD::maxValue();
	}

	if (groundTruth != nullptr)
	{
		groundTruth->set(output.frameType(), false /*forceOwner*/, true /*forceWritable*/);
	}

	double sumAbsError = 0.0;
	double maxAbsError = 0.0;
	unsigned long long measurements = 0ull;

	for (unsigned int yOutput = 0u; yOutput < output.height(); ++yOutput)
	{
		for (unsigned int xOutput = 0u; xOutput < output.width(); ++xOutput)
		{
			const T* const outputPixel = output.constpixel<T>(xOutput, yOutput);

			const Vector2 outputPosition = Vector2(Scalar(xOutput) + Scalar(interpolatedFrameOrigin.x()), Scalar(yOutput) + Scalar(interpolatedFrameOrigin.y()));
			const Vector2 inputPosition = input_H_output * outputPosition;

			if (inputPosition.x() >= Scalar(0) && inputPosition.y() >= Scalar(0) && inputPosition.x() <= Scalar(input.width() - 1u) && inputPosition.y() <= Scalar(input.height() - 1u))
			{
				const unsigned int inputLeftPixel = (unsigned int)(inputPosition.x());
				const unsigned int inputRightPixel = min(inputLeftPixel + 1u, input.width() - 1u);

				const unsigned int inputTopPixel = (unsigned int)(inputPosition.y());
				const unsigned int inputBottomPixel = min(inputTopPixel + 1u, input.height() - 1u);

				const double rightFactor = double(inputPosition.x()) - double(inputLeftPixel);
				const double bottomFactor = double(inputPosition.y()) - double(inputTopPixel);

				ocean_assert(rightFactor >= 0.0 && rightFactor <= 1.0);
				ocean_assert(bottomFactor >= 0.0 && bottomFactor <= 1.0);

				const double leftFactor = 1.0 - rightFactor;
				const double topFactor = 1.0 - bottomFactor;

				const T* const inputTopLeft = input.constpixel<T>(inputLeftPixel, inputTopPixel);
				const T* const inputTopRight = input.constpixel<T>(inputRightPixel, inputTopPixel);

				const T* const inputBottomLeft = input.constpixel<T>(inputLeftPixel, inputBottomPixel);
				const T* const inputBottomRight = input.constpixel<T>(inputRightPixel, inputBottomPixel);

				for (unsigned int n = 0u; n < channels; ++n)
				{
					const double top = double(inputTopLeft[n]) * leftFactor + double(inputTopRight[n]) * rightFactor;
					const double bottom = double(inputBottomLeft[n]) * leftFactor + double(inputBottomRight[n]) * rightFactor;

					const double interpolated = top * topFactor + bottom * bottomFactor;

					result[n] = T(interpolated);
				}

				// we do not check the result if we are very close to the frame boundaries
				if (Numeric::isNotEqual(inputPosition.x(), Scalar(0), frameBorderEps) && Numeric::isNotEqual(inputPosition.x(), Scalar(input.width() - 1u), frameBorderEps)
						&& Numeric::isNotEqual(inputPosition.y(), Scalar(0), frameBorderEps) && Numeric::isNotEqual(inputPosition.y(), Scalar(input.height() - 1u), frameBorderEps))
				{
					for (unsigned int n = 0u; n < channels; ++n)
					{
						const double absError = NumericD::abs(double(result[n]) - double(outputPixel[n]));

						sumAbsError += absError;
						maxAbsError = max(maxAbsError, absError);

						measurements++;
					}
				}

				if (groundTruth != nullptr)
				{
					memcpy(groundTruth->pixel<T>(xOutput, yOutput), result.data(), sizeof(T) * channels);
				}
			}
			else
			{
				// we do not check the result if we are very close to the frame boundaries
				if (Numeric::isNotEqual(inputPosition.x(), Scalar(0), frameBorderEps) && Numeric::isNotEqual(inputPosition.x(), Scalar(input.width() - 1u), frameBorderEps)
						&& Numeric::isNotEqual(inputPosition.y(), Scalar(0), frameBorderEps) && Numeric::isNotEqual(inputPosition.y(), Scalar(input.height() - 1u), frameBorderEps))
				{
					for (unsigned int n = 0u; n < channels; ++n)
					{
						const double absError = NumericD::abs(double(backgroundColor[n]) - double(outputPixel[n]));

						sumAbsError += absError;
						maxAbsError = max(maxAbsError, absError);

						measurements++;
					}
				}

				if (groundTruth != nullptr)
				{
					memcpy(groundTruth->pixel<T>(xOutput, yOutput), backgroundColor, sizeof(T) * channels);
				}
			}
		}
	}

	ocean_assert(measurements != 0ull || input.width() <= 2u || input.height() <= 2u || output.width() <= 2u || output.height() <= 2u);

	if (averageAbsError && measurements != 0ull)
	{
		*averageAbsError = sumAbsError / double(measurements);
	}

	if (maximalAbsError)
	{
		*maximalAbsError = maxAbsError;
	}
}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_FRAME_INTERPOLATOR_BILINEAR_H
