/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_INTERPOLATOR_BILINEAR_H
#define META_OCEAN_CV_FRAME_INTERPOLATOR_BILINEAR_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameBlender.h"
#include "ocean/cv/PixelPosition.h"
#include "ocean/cv/SSE.h"

#include "ocean/base/DataType.h"
#include "ocean/base/Frame.h"
#include "ocean/base/Memory.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/FisheyeCamera.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Lookup2.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/SquareMatrix2.h"
#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/Vector2.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements bilinear frame interpolator functions.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameInterpolatorBilinear
{
	public:

		/**
		 * Definition of a lookup table for 2D vectors.
		 */
		typedef LookupCorner2<Vector2> LookupTable;

	public:

		/**
		 * The following comfort class provides comfortable functions simplifying prototyping applications but also increasing binary size of the resulting applications.
		 * Best practice is to avoid using these functions if binary size matters,<br>
		 * as for every comfort function a corresponding function exists with specialized functionality not increasing binary size significantly.<br>
		 */
		class OCEAN_CV_EXPORT Comfort
		{
			public:

				/**
				 * Resizes/rescales a given frame by application of a bilinear interpolation.
				 * @param source The source frame to resize, must be valid
				 * @param target Resulting target frame with identical frame pixel format and pixel origin as the source frame, must be valid
				 * @param worker Optional worker object used for load distribution
				 * @return True, if the frame could be resized
				 */
				static bool resize(const Frame& source, Frame& target, Worker* worker = nullptr);

				/**
				 * Resizes/rescales a given frame by application of a bilinear interpolation.
				 * @param frame The frame to resize, must be valid
				 * @param width The width of the resized frame in pixel, with range [1, infinity)
				 * @param height The height of the resized frame in pixel, with range [1, infinity)
				 * @param worker Optional worker object used for load distribution
				 * @return True, if the frame could be resized
				 */
				static inline bool resize(Frame& frame, const unsigned int width, const unsigned int height, Worker* worker = nullptr);

				/**
				 * Zooms into a given input frame (or zooms out) and stores the zoomed image content in an output frame.
				 * The resulting zoomed image will have the same frame type (frame resolution, pixel format, pixel origin) as the input image.<br>
				 * The frame must have a 1-plane pixel format with DT_UNSIGNED_INTEGER_8 as data type (e.g., FORMAT_Y8, FORMAT_RGB24, FORMAT_RGBA32, ...).
				 * @param source The source frame for which the zoomed image content will be created, must be valid
				 * @param target The resulting target frame which will receive the zoomed image, will be set to the same frame type as the source frame, can be invalid
				 * @param zoomFactor The zoom factor to be applied, a factor < 1 will zoom out, a factor > 1 will zoom in, with range (0, infinity)
				 * @param worker Optional worker object to distribute the computation to several CPU cores
				 * @return True, if succeeded
				 */
				static bool zoom(const Frame& source, Frame& target, const Scalar zoomFactor, Worker* worker = nullptr);

				/**
				 * Transforms a given input frame into an output frame (with arbitrary frame dimension) by application of a homography.
				 * The output frame must have the same pixel format and pixel origin as the input frame, however the dimension (and position) of the output frame can be arbitrary.<br>
				 * This function allows the creation of an output frame fully covering the input frame (if the position and dimension of the output frame covers the transformation of the homography.<br>
				 * The 'outputOrigin' parameter simply applies an additional translation onto the provided homography i.e., homography * create_translation_matrix3x3(outputOrigin.x(), outputOrigin.y()).<br>
				 * Information: This function is the equivalent to OpenCV's cv::warpPerspective().
				 * The frame must have a 1-plane pixel format with DT_UNSIGNED_INTEGER_8 as data type (e.g., FORMAT_Y8, FORMAT_RGB24, FORMAT_RGBA32, ...).
				 * @param input The input frame that will be transformed, must be valid
				 * @param output The output frame resulting by application of the given homography, with same pixel format and pixel origin as the input frame, must be valid
				 * @param input_H_output Homography used to transform the given input frame by following equation: inputPoint = input_H_output * outputPoint, must be valid
				 * @param borderColor Color of undefined pixel positions, the size of the buffer must match to the number of channels and the data type of the pixel elements, nullptr to assign 0 to each channel
				 * @param worker Optional worker object to distribute the computational load
				 * @param outputOrigin The origin of the output frame defining the global position of the output frame's pixel coordinate (0, 0), with range (-infinity, infinity)x(-infinity, infinity)
				 * @return True, if succeeded
				 */
				static bool homography(const Frame& input, Frame& output, const SquareMatrix3& input_H_output, const void* borderColor = nullptr, Worker* worker = nullptr, const PixelPositionI& outputOrigin = PixelPositionI(0, 0));

				/**
				 * Transforms a given input frame into an output frame (with arbitrary frame dimension) by application of four homographies.
				 * For each quadrant of the output frame an individual homography is applied while the final result is interpolated between the four homographies.<br>
				 * The quadrant order of the homographies is as follows: top left, top right, bottom left, bottom right.<br>
				 * The output frame must have the same pixel format and pixel origin as the input frame, however the dimension (and position) of the output frame can be arbitrary.<br>
				 * This function allows the creation of an output frame fully covering the input frame (if the position and dimension of the output frame covers the transformation of the homography.<br>
				 * The frame must have a 1-plane pixel format with DT_UNSIGNED_INTEGER_8 as data type (e.g., FORMAT_Y8, FORMAT_RGB24, FORMAT_RGBA32, ...).
				 * @param input The input frame that will be transformed
				 * @param output The output frame resulting by application of the given homography, with same pixel format and pixel origin as the input frame, must have a valid dimension
				 * @param homographies Four homographies used to transform the given input frame, transforming points defined in the output frame into points defined in the input frame
				 * @param outputQuadrantCenter The center position of the four quadrants in the output frame (the local center not respecting the optional outputOrigin parameter), with range [0, output.width())x[0, output.height())
				 * @param borderColor Color of undefined pixel positions, the size of the buffer must match to the number of channels, nullptr to assign 0x00 to each channel
				 * @param worker Optional worker object to distribute the computational load
				 * @param outputOrigin The origin of the output frame defining the global position of the output frame's pixel coordinate (0, 0), with range (-infinity, infinity)x(-infinity, infinity)
				 * @return True, if succeeded
				 */
				static bool homographies(const Frame& input, Frame& output, const SquareMatrix3 homographies[4], const Vector2& outputQuadrantCenter, const uint8_t* borderColor = nullptr, Worker* worker = nullptr, const PixelPositionI& outputOrigin = PixelPositionI(0, 0));

				/**
				 * Transforms a given input frame into an output frame (with arbitrary frame dimension) by application of a homography.
				 * The output frame must have the same pixel format and pixel origin as the input frame, however the dimension (and position) of the output frame can be arbitrary.<br>
				 * This function allows the creation of an output frame fully covering the input frame (if the position and dimension of the output frame covers the transformation of the homography).<br>
				 * Input frame pixels lying outside the frame will be masked in the resulting output mask frame, further theses pixels are untouched in the output frame.<br>
				 * The 'outputOrigin' parameter simply applies an additional translation onto the provided homography i.e., homography * create_translation_matrix3x3(outputOrigin.x(), outputOrigin.y()).
				 * The frame must have a 1-plane pixel format with DT_UNSIGNED_INTEGER_8 as data type (e.g., FORMAT_Y8, FORMAT_RGB24, FORMAT_RGBA32, ...).
				 * @param input The input frame that will be transformed, must be valid
				 * @param output The Output frame resulting by application of the given homography, with same pixel format and pixel origin as the input frame, must have a valid dimension
				 * @param outputMask Resulting mask frame with 8 bit per pixel defining whether an output frame pixel has a valid corresponding pixel in the input frame, must be valid and must have the same frame dimension as the output frame
				 * @param input_H_output Homography used to transform the given input frame by following equation: inputPoint = input_H_output * outputPoint, must be valid
				 * @param worker Optional worker object to distribute the computational load
				 * @param maskValue 8 bit mask values for reference pixels lying inside the given camera frame, reference pixels lying outside the camera frame will be assigned with (0xFF - maskValue)
				 * @param outputOrigin The origin of the output frame defining the global position of the output frame's pixel coordinate (0, 0), with range (-infinity, infinity)x(-infinity, infinity)
				 * @return True, if succeeded
				 * @see coversHomographyInputFrame().
				 */
				static bool homographyMask(const Frame& input, Frame& output, Frame& outputMask, const SquareMatrix3& input_H_output, Worker* worker = nullptr, const uint8_t maskValue = 0xFF, const PixelPositionI& outputOrigin = PixelPositionI(0, 0));

				/**
				 * Transforms a given input frame into an output frame (with arbitrary frame dimension) by application of four homographies.
				 * For each quadrant of the output frame an individual homography is applied while the final result is interpolated between the four homographies.<br>
				 * The quadrant order of the homographies is as follows: top left, top right, bottom left, bottom right.<br>
				 * The output frame must have the same pixel format and pixel origin as the input frame, however the dimension (and position) of the output frame can be arbitrary.<br>
				 * This function allows the creation of an output frame fully covering the input frame (if the position and dimension of the output frame covers the transformation of the homography.<br>
				 * Input frame pixels lying outside the frame will be masked in the resulting output mask frame, further theses pixels are untouched in the output frame.<br>
				 * The frame must have a 1-plane pixel format with DT_UNSIGNED_INTEGER_8 as data type (e.g., FORMAT_Y8, FORMAT_RGB24, FORMAT_RGBA32, ...).
				 * @param input The input frame that will be transformed, must be valid
				 * @param output The output frame resulting by application of the given homography, with same pixel format and pixel origin as the input frame, must have a valid dimension
				 * @param outputMask Mask frame with 8 bit per pixel defining whether an output frame pixel has a valid corresponding pixel in the input frame
				 * @param homographies Four homographies used to transform the given input frame, transforming points defined in the output frame into points defined in the input frame
				 * @param outputQuadrantCenter The center position of the four quadrants in the output frame (the local center not respecting the optional outputOrigin parameter), with range [0, output.width())x[0, output.height())
				 * @param worker Optional worker object to distribute the computational load
				 * @param maskValue 8 bit mask values for reference pixels lying inside the given camera frame, reference pixels lying outside the camera frame will be assigned with (0xFF - maskValue)
				 * @param outputOrigin The origin of the output frame defining the global position of the output frame's pixel coordinate (0, 0), with range (-infinity, infinity)x(-infinity, infinity)
				 * @return True, if succeeded
				 * @see coversHomographyInputFrame().
				 */
				static bool homographiesMask(const Frame& input, Frame& output, Frame& outputMask, const SquareMatrix3* homographies, const Vector2& outputQuadrantCenter, Worker* worker = nullptr, const uint8_t maskValue = 0xFF, const PixelPositionI& outputOrigin = PixelPositionI(0, 0));

				/**
				 * Transforms a given input frame into an output frame by application of a homography.
				 * This function also uses a camera profile to improve the interpolation accuracy.<br>
				 * The given homography is transformed into a homography for normalized image coordinates.<br>
				 * Thus, also distortion parameters of the camera profile can be applied.<br>
				 * The frame must have a 1-plane pixel format with DT_UNSIGNED_INTEGER_8 as data type (e.g., FORMAT_Y8, FORMAT_RGB24, FORMAT_RGBA32, ...).
				 * @param inputCamera The pinhole camera profile to be applied for the input frame
				 * @param outputCamera The pinhole camera profile to be applied for the output frame
				 * @param input The input frame that will be transformed, the frame dimension must match the dimension of the left camera
				 * @param output The output frame resulting by application of the given homography, the frame dimension must match the dimension of the right camera
				 * @param homography The homography used to transform the given input frame, with includes both camera profiles: H = Ki * H' * Ko^-1
				 * @param useDistortionParameters True, to apply the distortion parameters of the camera profile
				 * @param borderColor Color of undefined pixel positions, the size of the buffer must match to the number of channels, nullptr to assign 0x00 to each channel
				 * @param worker Optional worker object to distribute the computational load
				 * @return True, if succeeded
				 * @see homographyWithCameraMask(), homography().
				 */
				static bool homographyWithCamera(const PinholeCamera& inputCamera, const PinholeCamera& outputCamera, const Frame& input, Frame& output, const SquareMatrix3& homography, const bool useDistortionParameters, const uint8_t* borderColor = nullptr, Worker* worker = nullptr);

				/**
				 * Transforms a given input frame into an output frame by application of a homography.
				 * Input frame pixels lying outside the frame will be masked in the resulting output mask frame, further theses pixels are untouched in the output frame.<br>
				 * This function also uses a camera profile to improve the interpolation accuracy.<br>
				 * The given homography is transformed into a homography for normalized image coordinates.<br>
				 * The frame must have a 1-plane pixel format with DT_UNSIGNED_INTEGER_8 as data type (e.g., FORMAT_Y8, FORMAT_RGB24, FORMAT_RGBA32, ...).
				 * Thus, also distortion parameters of the camera profile can be applied.<br>
				 * @param inputCamera The pinhole camera profile to be applied for the input frame
				 * @param outputCamera The pinhole camera profile to be applied for the output frame
				 * @param input The input frame that will be transformed, the frame dimension must match the dimension of the left camera
				 * @param output The output frame resulting by application of the given homography, the frame dimension must match the dimension of the right camera
				 * @param outputMask Mask frame with 8 bit per pixel defining whether an output frame pixel has a valid corresponding pixel in the input frame
				 * @param homography The homography used to transform the given input frame, with includes both camera profiles: H = Ki * H' * Ko^-1
				 * @param worker Optional worker object to distribute the computational load
				 * @param maskValue 8 bit mask values for reference pixels lying inside the given camera frame, reference pixels lying outside the camera frame will be assigned with (0xFF - maskValue)
				 * @return True, if succeeded
				 * @see homographyWithCamera(), homography().
				 */
				static bool homographyWithCameraMask(const AnyCamera& inputCamera, const AnyCamera& outputCamera, const Frame& input, Frame& output, Frame& outputMask, const SquareMatrix3& homography, Worker* worker = nullptr, const uint8_t maskValue = 0xFFu);

				/**
				 * Transforms a given input frame into an output frame by application of an interpolation lookup table.
				 * The output frame must have the same pixel format and pixel origin as the input frame.<br>
				 * Information: This function is the equivalent to OpenCV's cv::remap().
				 * The frame must have a 1-plane pixel format with DT_UNSIGNED_INTEGER_8 as data type (e.g., FORMAT_Y8, FORMAT_RGB24, FORMAT_RGBA32, ...).
				 * @param input The input frame that will be transformed
				 * @param output Resulting output frame, the dimension will be set to match the size of the lookup table, pixel format and pixel origin will be set to match the given input frame
				 * @param input_LT_output The lookup table which defines the transformation from locations defined in the output frame to locations defined in the input frame (the lookup table stores the corresponding locations in the input frame), must be valid
				 * @param offset True, if the lookup table store local offsets; False, if the lookup table stores absolute positions
				 * @param borderColor Color of undefined pixel positions, the size of the buffer must match to the number of channels, nullptr to assign 0x00 to each channel
				 * @param worker Optional worker object to distribute the computation
				 * @return True, if succeeded
				 */
				static bool lookup(const Frame& input, Frame& output, const LookupTable& input_LT_output, const bool offset, const void* borderColor, Worker* worker = nullptr);

				/**
				 * Transforms a given input frame into an output frame by application of an interpolation lookup table and creates and additional mask as output.
				 * The output frame must have the same pixel format and pixel origin as the input frame.<br>
				 * Input frame pixels lying outside the frame will be masked in the resulting output mask frame, further theses pixels are untouched in the output frame.<br>
				 * The frame must have a 1-plane pixel format with DT_UNSIGNED_INTEGER_8 as data type (e.g., FORMAT_Y8, FORMAT_RGB24, FORMAT_RGBA32, ...).
				 * @param input The input frame which will be transformed
				 * @param output Resulting output frame, the frame dimension will be set to match the size of the lookup table, pixel format and pixel origin will be set to match the given input frame
				 * @param outputMask Resulting mask frame with 8 bit per pixel defining whether an output frame pixel has a valid corresponding pixel in the input frame
				 * @param input_LT_output The lookup table which defines the transformation from locations defined in the output frame to locations defined in the input frame (the lookup table stores the corresponding locations in the input frame), must be valid
				 * @param offset True, if the lookup table store local offsets; False, if the lookup table stores absolute positions
				 * @param worker Optional worker object to distribute the computation
				 * @param maskValue 8 bit mask values for pixels lying inside the input frame, pixels lying outside the input frame will be assigned with (0xFF - maskValue)
				 * @return True, if succeeded
				 */
				static bool lookupMask(const Frame& input, Frame& output, Frame& outputMask, const LookupTable& input_LT_output, const bool offset, Worker* worker = nullptr, const uint8_t maskValue = 0xFF);

				/**
				* Applies an affine transformation to an image.
				 * The target frame must have the same pixel format and pixel origin as the source frame, however the dimension (and position) of the target frame can be arbitrary.
				 * This function allows the creation of an target frame fully covering the source frame (if the position and dimension of the target frame covers the transformation of the affine transformation.
				 * The multiplication of the affine transformation with pixel location in the target image yield their location in the source image, i.e., sourcePoint = source_A_target * targetPoint.
				 * The parameter 'targetOrigin' applies an additional translation to the provided affine transformation i.e., source_A_target * create_translation_matrix3x3(targetOrigin.x(), targetOrigin.y()).
				 * Please note that here the affine transformation is specified as a 3-by-3 matrix (in contrast to the more commonly used 2-by-3 matrix) and should take of the form:
				 * <pre>
				 * a c e
				 * b d f
				 * 0 0 1
				 * </pre>
				 * However, this function disregards the last row completely and only uses the top two rows, i.e., the elements a through f.
				 * Information: This function is the equivalent to OpenCV's cv::warpAffine().
				 * Note: For applications running on mobile devices, in order to keep the impact on binary size to a minimum please prefer a specialized transformation function (those that work on image pointers instead of Frame instances).
				 * The frame must have a 1-plane pixel format with DT_UNSIGNED_INTEGER_8 as data type (e.g., FORMAT_Y8, FORMAT_RGB24, FORMAT_RGBA32, ...).
				 * @param source The source frame that will be transformed, must be valid
				 * @param target The resulting frame after applying the affine transformation to the source frame; pixel format and pixel origin must be identical to source frame; memory of target frame must be allocated by the caller
				 * @param source_A_target Affine transform used to transform the given source frame, transforming points defined in the target frame into points defined in the source frame
				 * @param borderColor Color of undefined pixel positions, the size of the buffer must match to the number of channels, nullptr to assign 0x00 to each channel
				 * @param worker Optional worker object to distribute the computational load
				 * @param targetOrigin The origin of the target frame defining the global position of the target frame's pixel coordinate (0, 0), with range (-infinity, infinity)x(-infinity, infinity)
				 * @return True, if succeeded
				 */
				static bool affine(const Frame& source, Frame& target, const SquareMatrix3& source_A_target, const uint8_t* borderColor = nullptr, Worker* worker = nullptr, const PixelPositionI& targetOrigin = PixelPositionI(0, 0));

				/**
				 * Rotates a given frame by a bilinear interpolation.
				 * The frame will be rotated around a specified anchor position (inside or outside the frame).<br>
				 * The frame must have a 1-plane pixel format with DT_UNSIGNED_INTEGER_8 as data type (e.g., FORMAT_Y8, FORMAT_RGB24, FORMAT_RGBA32, ...).
				 * @param source The source frame to be rotated, must be valid
				 * @param target The target frame which will receive the rotated image, will be set to the same frame type as the source frame, can be invalid
				 * @param horizontalAnchorPosition Position of the rotation anchor in the horizontal direction, with range (-infinity, infinity)
				 * @param verticalAnchorPosition Position of the rotation anchor in the vertical direction, with range (-infinity, infinity)
				 * @param angle The counter clockwise rotation angle in radian, with range [0, 2PI)
				 * @param worker Optional worker object to distribute the computation to several CPU cores
				 * @param borderColor The color of border pixels for which now visual content exists, provide one value for each channel, nullptr to use 0x00 for each channel
				 * @return True, if succeeded
				 */
				static bool rotate(const Frame& source, Frame& target, const Scalar horizontalAnchorPosition, const Scalar verticalAnchorPosition, const Scalar angle, Worker* worker = nullptr, const uint8_t* borderColor = nullptr);

				/**
				 * Re-samples a camera image which has been captured with a camera profile as if the image would have been captured with a second camera profile.
				 * The function can be used e.g., to rectify a fisheye camera image into a pinhole camera image.
				 * @param sourceFrame The source image captured with the source camera profile, must be valid
				 * @param sourceCamera The source camera profile which has been used to capture the source image, with resolution sourceFrame.width() x sourceFrame.height(), must be valid
				 * @param source_R_target The rotation transforming 3D points defined in the coordinate system of the target camera image to 3D points defined in the coordinate system of the source camera image, must be valid
				 * @param targetCamera The camera profile of the target frame, must be valid
				 * @param targetFrame The resulting target image, with resolution targetCamera.width() x targetCamera.height(), must be valid
				 * @param source_OLT_target Optional resulting offset lookup table between target image points and source image points
				 * @param worker Optional worker object to distribute the computational load
				 * @param binSizeInPixel The size in pixel of the interpolation bins used for building the lookup table, with range [1, infinity)
				 * @param borderColor The color of border pixels for which now visual content exists, provide one value for each channel, nullptr to use ElementType(0) for each channel
				 * @return True, if succeeded
				 * @see resampleCameraImageImage8BitPerChannel().
				 */
				static bool resampleCameraImage(const Frame& sourceFrame, const AnyCamera& sourceCamera, const SquareMatrix3& source_R_target, const AnyCamera& targetCamera, Frame& targetFrame, LookupCorner2<Vector2>* source_OLT_target = nullptr, Worker* worker = nullptr, const unsigned int binSizeInPixel = 8u, const void* borderColor = nullptr);

				/**
				 * Determines the interpolated pixel values for a given pixel position in an 8 bit per channel frame.
				 * This function uses an integer interpolation with a precision of 1/128.
				 * @param frame The frame to determine the pixel values from, must be valid
				 * @param channels Number of channels of the given frame, with range [1, 8]
				 * @param width The width of the frame in pixel, with range [1, infinity)
				 * @param height The height of the frame in pixel, with range [1, infinity)
				 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
				 * @param pixelCenter The pixel center to be used during interpolation, either 'PC_TOP_LEFT' or 'PC_CENTER'
				 * @param position The position for which the interpolated pixel will be determined, with ranges [0, width - 1]x[0, height - 1] for PC_TOP_LEFT, [0, width]x[0, height] for PC_CENTER
				 * @param result Resulting pixel values, must be valid, must be valid
				 * @return True, if succeeded
				 * @tparam TScalar The scalar data type of the sub-pixel position
				 */
				template <typename TScalar = Scalar>
				static bool interpolatePixel8BitPerChannel(const uint8_t* frame, const unsigned int channels, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const PixelCenter pixelCenter, const VectorT2<TScalar>& position, uint8_t* result);

				/**
				 * Determines the interpolated pixel values for a given pixel position in a frame with arbitrary data type.
				 * This function uses floating point precision during interpolation.
				 * @param frame The frame to determine the pixel values from, must be valid
				 * @param channels Number of channels of the given frame, with range [1, 8]
				 * @param width The width of the frame in pixel, with range [1, infinity)
				 * @param height The height of the frame in pixel, with range [1, infinity)
				 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
				 * @param pixelCenter The pixel center to be used during interpolation, either 'PC_TOP_LEFT' or 'PC_CENTER'
				 * @param position The position to determine the interpolated pixel values for, with range [0, width)x[0, height)
				 * @param result Resulting interpolated pixel value(s), must be valid
				 * @param resultBias Optional bias value which will be added to the interpolation result e.g. to handle rounding, with range (-infinity, infinity), default is zero
				 * @return True, if succeeded
				 * @tparam TSource The data type of the provided pixel values in the (source) frame
				 * @tparam TTarget The data type of the resulting interpolated value(s)
				 * @tparam TScalar The data type of each coordinate of the provided interpolation location, should be either Scalar, float, or double
				 * @tparam TIntermediate The data type of the intermediate interpolation result before assigning the result
				 */
				template <typename TSource, typename TTarget, typename TScalar = Scalar, typename TIntermediate = TScalar>
				static bool interpolatePixel(const TSource* frame, const unsigned int channels, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const PixelCenter pixelCenter, const VectorT2<TScalar>& position, TTarget* result, const TIntermediate& resultBias = TIntermediate(0));
		};

		/**
		 * This class implements highly optimized interpolation functions with fixed properties.
		 * The functions can be significantly faster as these functions are tailored to the specific properties.
		 */
		class OCEAN_CV_EXPORT SpecialCases
		{
			public:

				/**
				 * Resizes a given FORMAT_Y8 frame with resolution 400x400 to a FORMAT_Y8 frame with resolution 224x224 by using a bilinear interpolation.
				 * This function exploits the fact that lookup locations and interpolation factors repeat after 25 pixels (16 pixels in the target resolution).
				 * @param source The source frame buffer with resolution 400x400, must be valid
				 * @param target The target frame buffer receiving the resized image information, with resolution 224x224, must be valid
				 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
				 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
				 * @see FrameInterpolatorBilinear::resize<T, tChannels>().
				 */
				static void resize400x400To224x224_8BitPerChannel(const uint8_t* const source, uint8_t* const target, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements);

				/**
				 * Resizes a given FORMAT_Y8 frame with resolution 400x400 to a FORMAT_Y8 frame with resolution 256x256 by using a bilinear interpolation.
				 * This function exploits the fact that lookup locations and interpolation factors repeat after 25 pixels (16 pixels in the target resolution).
				 * @param source The source frame buffer with resolution 400x400, must be valid
				 * @param target The target frame buffer receiving the resized image information, with resolution 256x256, must be valid
				 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
				 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
				 * @see FrameInterpolatorBilinear::resize<T, tChannels>().
				 */
				static void resize400x400To256x256_8BitPerChannel(const uint8_t* const source, uint8_t* const target, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements);
		};

		/**
		 * Resizes a given frame with (almost) arbitrary data type (e.g., float, double, int) by using a bilinear interpolation.
		 * This function is actually a wrapper for scale().
		 * @param source The source frame buffer providing the image information to be resized, must be valid
		 * @param target The target frame buffer receiving the resized image information, must be valid
		 * @param sourceWidth Width of the source frame in pixel, with range [1, infinity)
		 * @param sourceHeight Height of the source frame in pixel, with range [1, infinity)
		 * @param targetWidth Width of the target frame in pixel, with range [1, infinity)
		 * @param targetHeight Height of the target frame in pixel, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation to several CPU cores
		 * @tparam T Data type of each pixel channel, e.g., float, double, int
		 * @tparam tChannels Number of channels of the frame, with range [1, infinity)
		 * @see scale<T, tChannels>().
		 */
		template <typename T, unsigned int tChannels>
		static inline void resize(const T* source, T* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Rescales a given frame with arbitrary data type (e.g., float, double, int) by using a bilinear interpolation with user-defined scaling factors.
		 * Beware: This function is not optimized for performance but supports arbitrary data types.<br>
		 * Try to use scale8BitPerChannel() if possible.
		 * @param source The source frame buffer providing the image information to be resized, must be valid
		 * @param target The target frame buffer receiving the rescaled image information, must be valid
		 * @param sourceWidth Width of the source frame in pixel, with range [1, infinity)
		 * @param sourceHeight Height of the source frame in pixel, with range [1, infinity)
		 * @param targetWidth Width of the target frame in pixel, with range [1, infinity)
		 * @param targetHeight Height of the target frame in pixel, with range [1, infinity)
		 * @param sourceX_s_targetX The horizontal scale factor converting a location in the target frame to a location in the source frame (xSource = sourceX_s_targetX * xTarget), with range (0, sourceWidth/targetWidth]
		 * @param sourceY_s_targetY The vertical scale factor converting a location in the target frame to a location in the source frame (ySource = sourceY_s_targetY * yTarget), with range (0, sourceHeight/targetHeight]
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation to several CPU cores
		 * @tparam T Data type of each pixel channel, e.g., float, double, int
		 * @tparam tChannels Number of channels of the frame, with range [1, infinity)
		 * @see resize<T, tChannels>().
		 */
		template <typename T, unsigned int tChannels>
		static inline void scale(const T* source, T* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const double sourceX_s_targetX, const double sourceY_s_targetY, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Rotates a given frame by a bilinear interpolation.
		 * The frame will be rotated around a specified anchor position (inside or outside the frame).
		 * @param source The source frame to be rotated, must be valid
		 * @param target The target frame which will receive the rotated image, with same frame type as the source frame, must be valid
		 * @param width The width of the source and target frame in pixel, with range [1, infinity)
		 * @param height The height of the source and target frame in pixel, with range [1, infinity)
		 * @param horizontalAnchorPosition Position of the rotation anchor in the horizontal direction, with range (-infinity, infinity)
		 * @param verticalAnchorPosition Position of the rotation anchor in the vertical direction, with range (-infinity, infinity)
		 * @param angle The counter clockwise rotation angle in radian, with range [0, 2PI)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation to several CPU cores
		 * @param borderColor The color of border pixels for which now visual content exists, provide one value for each channel, nullptr to use 0x00 for each channel
		 * @tparam tChannels The number of channels both frames have, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static inline void rotate8BitPerChannel(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const Scalar horizontalAnchorPosition, const Scalar verticalAnchorPosition, const Scalar angle, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr, const uint8_t* borderColor = nullptr);

		/**
		 * Apply an affine transforms to a N-channel, 8-bit frame
		 * The target frame must have the same pixel format and pixel origin as the source frame, however the dimension (and position) of the target frame can be arbitrary.<br>
		 * This function allows the creation of an target frame fully covering the source frame (if the position and dimension of the target frame covers the transformation of the affine transformation).<br>
		 * The 'targetOrigin' parameter simply applies an additional translation onto the provided affine transformation i.e., affine * create_translation_matrix3x3(targetOrigin.x(), targetOrigin.y()).
		 * Please note that here the affine transformation is specified as a 3-by-3 matrix (in contrast to the more commonly used 2-by-3 matrix) and should take of the form:
		 * <pre>
		 * a c e
		 * b d f
		 * 0 0 1
		 * </pre>
		 * However, this function disregards the last row completely and only uses the top two rows, i.e., the elements a through f.
		 * @param source Input frame that will be transformed, must be valid
		 * @param sourceWidth Width of both images in pixel, with range [1, infinity)
		 * @param sourceHeight Height of both images pixel, with range [1, infinity)
		 * @param source_A_target Affine transformation, such that: sourcePoint = source_A_target * targetPoint
		 * @param borderColor Color of undefined pixel positions, the size of the buffer must match to the number of channels, nullptr to assign 0x00 to each channel
		 * @param target The target frame using the given affine transform, must be valid
		 * @param targetOrigin The origin of the target frame defining the global position of the target frame's pixel coordinate (0, 0), with range (-infinity, infinity)x(-infinity, infinity)
		 * @param targetWidth The width of the target image in pixel, with range [1, infinity)
		 * @param targetHeight The height of the target image in pixel, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source frame row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target frame row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 * @tparam tChannels Number of channels of the frame
		 * @see homographyMask8BitPerChannel(), homographyWithCamera8BitPerChannel(), homography().
		 */
		template <unsigned int tChannels>
		static inline void affine8BitPerChannel(const uint8_t* source, const unsigned int sourceWidth, const unsigned int sourceHeight, const SquareMatrix3& source_A_target, const uint8_t* borderColor, uint8_t* target, const PixelPositionI& targetOrigin, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Transforms a input frame with (almost) arbitrary pixel format into an output frame by application of a homography.
		 * The output frame must have the same pixel format and pixel origin as the input frame, however the dimension (and position) of the output frame can be arbitrary.<br>
		 * This function allows the creation of an output frame fully covering the input frame (if the position and dimension of the output frame covers the transformation of the homography.<br>
		 * The 'outputOrigin' parameter simply applies an additional translation onto the provided homography i.e., homography * create_translation_matrix3x3(outputOrigin.x(), outputOrigin.y()).
		 * @param input The input frame that will be transformed, must be valid
		 * @param inputWidth Width of both images in pixel, with range [1, infinity)
		 * @param inputHeight Height of both images pixel, with range [1, infinity)
		 * @param input_H_output Homography used to transform the given input frame by following equation: inputPoint = input_H_output * outputPoint, must be valid
		 * @param borderColor Color of undefined pixel positions, the size of the buffer must match to the number of channels, nullptr to assign 0x00 to each channel
		 * @param output The output frame using the given homography, must be valid
		 * @param outputOrigin The origin of the output frame defining the global position of the output frame's pixel coordinate (0, 0), with range (-infinity, infinity)x(-infinity, infinity)
		 * @param outputWidth The width of the output image in pixel, with range [1, infinity)
		 * @param outputHeight The height of the output image in pixel, with range [1, infinity)
		 * @param inputPaddingElements The number of padding elements at the end of each input frame row, in elements, with range [0, infinity)
		 * @param outputPaddingElements The number of padding elements at the end of each output frame row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 * @tparam T Data type of each pixel channel, e.g., float, double, int
		 * @tparam tChannels Number of channels of the frame, with range [1, infinity)
		 * @see homographyMask8BitPerChannel(), homographyWithCamera8BitPerChannel().
		 */
		template <typename T, unsigned int tChannels>
		static inline void homography(const T* input, const unsigned int inputWidth, const unsigned int inputHeight, const SquareMatrix3& input_H_output, const T* borderColor, T* output, const PixelPositionI& outputOrigin, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, Worker* worker = nullptr);

		/**
		 * Transforms a given 8 bit per channel input frame into an output frame by application of four homographies.
		 * For each quadrant of the output frame an individual homography is applied while the final result is interpolated between the four homographies.<br>
		 * The quadrant order of the homographies is as follows: top left, top right, bottom left, bottom right.<br>
		 * The output frame must have the same pixel format and pixel origin as the input frame, however the dimension (and position) of the output frame can be arbitrary.<br>
		 * This function allows the creation of an output frame fully covering the input frame (if the position and dimension of the output frame covers the transformation of the homography.<br>
		 * @param input The input frame that will be transformed
		 * @param inputWidth Width of both images in pixel, with range [1, infinity)
		 * @param inputHeight Height of both images pixel, with range [1, infinity)
		 * @param homographies Four homographies used to transform the given input frame, transforming points defined in the output frame into points defined in the input frame
		 * @param borderColor Color of undefined pixel positions, the size of the buffer must match to the number of channels, nullptr to assign 0x00 to each channel
		 * @param output The output frame using the given homography
		 * @param outputQuadrantCenter The center position of the four quadrants in the output frame (the local center not respecting the optional outputOrigin parameter), with range [0, outputWidth)x[0, outputHeight)
		 * @param outputOrigin The origin of the output frame defining the global position of the output frame's pixel coordinate (0, 0), with range (-infinity, infinity)x(-infinity, infinity)
		 * @param outputWidth The width of the output image in pixel, with range [1, infinity)
		 * @param outputHeight The height of the output image in pixel, with range [1, infinity)
		 * @param inputPaddingElements The number of padding elements at the end of each input frame row, in elements, with range [0, infinity)
		 * @param outputPaddingElements The number of padding elements at the end of each output frame row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 * @tparam tChannels Number of channels of the frame
		 * @see homographyMask8BitPerChannel(), homographyWithCamera8BitPerChannel().
		 */
		template <unsigned int tChannels>
		static inline void homographies8BitPerChannel(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const SquareMatrix3 homographies[4], const uint8_t* borderColor, uint8_t* output, const Vector2& outputQuadrantCenter, const PixelPositionI& outputOrigin, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, Worker* worker = nullptr);

		/**
		 * Transforms a given 8 bit per channel input frame into an output frame by application of a homography.
		 * The output frame must have the same pixel format and pixel origin as the input frame, however the dimension (and position) of the output frame can be arbitrary.<br>
		 * This function allows the creation of an output frame fully covering the input frame (if the position and dimension of the output frame covers the transformation of the homography.<br>
		 * The 'outputOrigin' parameter simply applies an additional translation onto the provided homography i.e., homography * create_translation_matrix3x3(outputOrigin.x(), outputOrigin.y()).
		 * @param input The input frame that will be transformed, must be valid
		 * @param inputWidth Width of both images in pixel, with range [1, infinity)
		 * @param inputHeight Height of both images pixel, with range [1, infinity)
		 * @param input_H_output Homography used to transform the given input frame by following equation: inputPoint = input_H_output * outputPoint, must be valid
		 * @param output The output frame using the given homography, must be valid
		 * @param outputMask Mask frame with 8 bit per pixel defining whether an output frame pixel has a valid corresponding pixel in the input frame, must be valid
		 * @param outputOrigin The origin of the output frame defining the global position of the output frame's pixel coordinate (0, 0), with range (-infinity, infinity)x(-infinity, infinity)
		 * @param outputWidth The width of the output image in pixel, with range [1, infinity)
		 * @param outputHeight The height of the output image in pixel, with range [1, infinity)
		 * @param maskValue 8 bit mask values for reference pixels lying inside the given camera frame, reference pixels lying outside the camera frame will be assigned with (0xFF - maskValue)
		 * @param inputPaddingElements The number of padding elements at the end of each input row, in elements, with range [0, infinity)
		 * @param outputPaddingElements The number of padding elements at the end of each output row, in elements, with range [0, infinity)
		 * @param outputMaskPaddingElements The number of padding elements at the end of output mask input row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 * @tparam tChannels Number of channels of the frame, with range [1, infinity)
		 * @see homography(), homographyWithCamera8BitPerChannel().
		 */
		template <unsigned int tChannels>
		static inline void homographyMask8BitPerChannel(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const SquareMatrix3& input_H_output, uint8_t* output, uint8_t* outputMask, const PixelPositionI& outputOrigin, const unsigned int outputWidth, const unsigned int outputHeight, const uint8_t maskValue /* = 0xFF*/, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, const unsigned int outputMaskPaddingElements, Worker* worker = nullptr);

		/**
		 * Transforms a given 8 bit per channel input frame into an output frame by application of four homographies.
		 * For each quadrant of the output frame an individual homography is applied while the final result is interpolated between the four homographies.<br>
		 * The quadrant order of the homographies is as follows: top left, top right, bottom left, bottom right.<br>
		 * The output frame must have the same pixel format and pixel origin as the input frame, however the dimension (and position) of the output frame can be arbitrary.<br>
		 * This function allows the creation of an output frame fully covering the input frame (if the position and dimension of the output frame covers the transformation of the homography.<br>
		 * @param input The input frame that will be transformed
		 * @param inputWidth Width of both images in pixel, with range [1, infinity)
		 * @param inputHeight Height of both images pixel, with range [1, infinity)
		 * @param homographies Four homographies used to transform the given input frame, transforming points defined in the output frame into points defined in the input frame
		 * @param output The output frame using the given homography
		 * @param outputMask Mask frame with 8 bit per pixel defining whether an output frame pixel has a valid corresponding pixel in the input frame
		 * @param outputQuadrantCenter The center position of the four quadrants in the output frame (the local center not respecting the optional outputOrigin parameter), with range [0, outputWidth)x[0, outputHeight)
		 * @param outputOrigin The origin of the output frame defining the global position of the output frame's pixel coordinate (0, 0), with range (-infinity, infinity)x(-infinity, infinity)
		 * @param outputWidth The width of the output image in pixel, with range [1, infinity)
		 * @param outputHeight The height of the output image in pixel, with range [1, infinity)
		 * @param inputPaddingElements The number of padding elements at the end of each input row, in elements, with range [0, infinity)
		 * @param outputPaddingElements The number of padding elements at the end of each output row, in elements, with range [0, infinity)
		 * @param outputMaskPaddingElements The number of padding elements at the end of each row of the output mask, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 * @param maskValue 8 bit mask values for reference pixels lying inside the given camera frame, reference pixels lying outside the camera frame will be assigned with (0xFF - maskValue)
		 * @tparam tChannels Number of channels of the frame
		 * @see homography(), homographyWithCamera8BitPerChannel().
		 */
		template <unsigned int tChannels>
		static inline void homographiesMask8BitPerChannel(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const SquareMatrix3 homographies[4], uint8_t* output, uint8_t* outputMask, const Vector2& outputQuadrantCenter, const PixelPositionI& outputOrigin, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, const unsigned int outputMaskPaddingElements, Worker* worker = nullptr, const uint8_t maskValue = 0xFF);

		/**
		 * Transforms a given 8 bit per channel input frame into an output frame by application of a homography.
		 * This function also uses a camera profile to improve the interpolation accuracy.<br>
		 * The given homography is transformed into a homography for normalized image coordinates.<br>
		 * Thus, also distortion parameters of the camera profile can be applied.<br>
		 * @param inputCamera The pinhole camera profile to be applied for the input frame
		 * @param outputCamera The pinhole camera profile to be applied for the output frame
		 * @param input The input frame that will be transformed
		 * @param homography The homography used to transform the given input frame by following equation: inputPoint = homography * outputPoint
		 * @param useDistortionParameters True, to apply the distortion parameters of the camera profile
		 * @param borderColor Color of undefined pixel positions, the size of the buffer must match to the number of channels, nullptr to assign 0x00 to each channel
		 * @param output The output frame using the given homography
		 * @param inputPaddingElements The number of padding elements at the end of each input row, in elements, with range [0, infinity)
		 * @param outputPaddingElements The number of padding elements at the end of each output row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 * @tparam tChannels Number of channels of the frame
		 * @see homography().
		 */
		template <unsigned int tChannels>
		static inline void homographyWithCamera8BitPerChannel(const PinholeCamera& inputCamera, const PinholeCamera& outputCamera, const uint8_t* input, const SquareMatrix3& homography, const bool useDistortionParameters, const uint8_t* borderColor, uint8_t* output, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, Worker* worker = nullptr);

		/**
		 * Transforms a given 8 bit per channel input frame into an output frame by application of a homography.
		 * Input frame pixels lying outside the frame will be masked in the resulting output mask frame.<br>
		 * This function also uses a camera profile to improve the interpolation accuracy.<br>
		 * The given homography is transformed into a homography for normalized image coordinates.<br>
		 * Thus, also distortion parameters of the camera profile can be applied.
		 * @param inputCamera The pinhole camera profile to be applied for the input frame, must be valid
		 * @param outputCamera The pinhole camera profile to be applied for the output frame, must be valid
		 * @param input The input frame that will be transformed, must be valid
		 * @param inputPaddingElements The number of padding elements at the end of each input row, in elements, with range [0, infinity)
		 * @param homography The homography used to transform the given input frame by following equation: inputPoint = homography * outputPoint
		 * @param output The output frame using the given homography
		 * @param outputMask Mask frame with 8 bit per pixel defining whether an output frame pixel has a valid corresponding pixel in the input frame
		 * @param outputPaddingElements The number of padding elements at the end of each output row, in elements, with range [0, infinity)
		 * @param outputMaskPaddingElements The number of padding elements at the end of each output mask row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 * @param maskValue 8 bit mask values for reference pixels lying inside the given camera frame, reference pixels lying outside the camera frame will be assigned with (0xFF - maskValue)
		 * @tparam tChannels Number of channels of the frame
		 */
		template <unsigned int tChannels>
		static inline void homographyWithCameraMask8BitPerChannel(const PinholeCamera& inputCamera, const PinholeCamera& outputCamera, const uint8_t* input, const unsigned int inputPaddingElements, const SquareMatrix3& homography, uint8_t* output, uint8_t* outputMask, const unsigned int outputPaddingElements, const unsigned int outputMaskPaddingElements, Worker* worker = nullptr, const uint8_t maskValue = 0xFF);

		/**
		 * Transforms a given input frame into an output frame by application of an interpolation lookup table.
		 * The frame must have a 1-plane pixel format.<br>
		 * The output frame must have the same pixel format and pixel origin as the input frame.
		 * @param input The input frame which will be transformed, must be valid
		 * @param inputWidth The width of the given input frame in pixel, with range [1, infinity)
		 * @param inputHeight The height of the given input frame in pixel, with range [1, infinity)
		 * @param input_LT_output The lookup table which defines the transformation from locations defined in the output frame to locations defined in the input frame (the lookup table stores the corresponding locations in the input frame), must be valid
		 * @param offset True, if the lookup table store local offsets; False, if the lookup table stores absolute positions
		 * @param borderColor Color of undefined pixel positions, the size of the buffer must match to the number of channels, nullptr to assign T(0) to each channel
		 * @param output Resulting output frame with frame dimension equal to the size of the given lookup table, must be valid
		 * @param inputPaddingElements The number of padding elements at the end of each input row, in elements, with range [0, infinity)
		 * @param outputPaddingElements The number of padding elements at the end of each output row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam T Data type of each pixel channel, e.g., float, double, int
		 * @tparam tChannels Number of channels of the frame, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static inline void lookup(const T* input, const unsigned int inputWidth, const unsigned int inputHeight, const LookupTable& input_LT_output, const bool offset, const T* borderColor, T* output, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, Worker* worker = nullptr);

		/**
		 * Transforms a given input frame into an output frame by application of an interpolation lookup table.
		 * The frame must have a 1-plane pixel format with DT_UNSIGNED_INTEGER_8 as data type (e.g., FORMAT_Y8, FORMAT_RGB24, FORMAT_RGBA32, ...).<br>
		 * The output frame must have the same pixel format and pixel origin as the input frame.<br>
		 * Input frame pixels lying outside the frame will be masked in the resulting output mask frame, further theses pixels are untouched in the output frame.<br>
		 * @param input The input frame which will be transformed
		 * @param inputWidth The width of the given input frame in pixel, with range [1, infinity)
		 * @param inputHeight The height of the given input frame in pixel, with range [1, infinity)
		 * @param input_LT_output The lookup table which defines the transformation from locations defined in the output frame to locations defined in the input frame (the lookup table stores the corresponding locations in the input frame), must be valid
		 * @param offset True, if the lookup table store local offsets; False, if the lookup table stores absolute positions
		 * @param output Resulting output frame with frame dimension equal to the size of the given lookup table
		 * @param outputMask Resulting mask frame with 8 bits per pixel defining whether an output frame pixel has a valid corresponding pixel in the input frame
		 * @param inputPaddingElements The number of padding elements at the end of each row of `input`, in elements, with range [0, infinity)
		 * @param outputPaddingElements The number of padding elements at the end of each row of `output`, in elements, with range [0, infinity)
		 * @param outputMaskPaddingElements The number of padding elements at the end of each row of `outputMask`, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @param maskValue 8 bit mask values for pixels lying inside the input frame, pixels lying outside the input frame will be assigned with (0xFF - maskValue)
		 * @tparam tChannels Number of channels of the frame
		 */
		template <unsigned int tChannels>
		static inline void lookupMask8BitPerChannel(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const LookupTable& input_LT_output, const bool offset, uint8_t* output, uint8_t* outputMask, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, const unsigned int outputMaskPaddingElements, Worker* worker = nullptr, const uint8_t maskValue = 0xFF);

		/**
		 * Re-samples a camera image which has been captured with a camera profile as if the image would have been captured with a second camera profile.
		 * The function can be used e.g., to rectify a fisheye camera image into a pinhole camera image.
		 * @param sourceFrame The source image captured with the source camera profile, must be valid
		 * @param sourceCamera The source camera profile which has been used to capture the source image, with resolution sourceFrame.width() x sourceFrame.height(), must be valid
		 * @param source_R_target The rotation transforming 3D points defined in the coordinate system of the target camera image to 3D points defined in the coordinate system of the source camera image, must be valid
		 * @param targetCamera The camera profile of the target frame, must be valid
		 * @param targetFrame The resulting target image, with resolution targetCamera.width() x targetCamera.height(), must be valid
		 * @param sourceFramePaddingElements The number of padding elements at the end of each source frame row, in elements, with range [0, infinity)
		 * @param targetFramePaddingElements The number of padding elements at the end of each target frame row, in elements, with range [0, infinity)
		 * @param source_OLT_target Optional resulting offset lookup table between target image points and source image points
		 * @param worker Optional worker object to distribute the computational load
		 * @param binSizeInPixel The size in pixel of the interpolation bins used for building the lookup table, with range [1, infinity)
		 * @param borderColor The color of border pixels for which now visual content exists, provide one value for each channel, nullptr to use T(0) for each channel
		 * @tparam T Data type of each pixel channel, e.g., uint8_t, int16_t, float, double
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @see Comfort::resampleCameraImage().
		 */
		template <typename T, unsigned int tChannels>
		static void resampleCameraImage(const T* sourceFrame, const AnyCamera& sourceCamera, const SquareMatrix3& source_R_target, const AnyCamera& targetCamera, T* targetFrame, const unsigned int sourceFramePaddingElements, const unsigned int targetFramePaddingElements, LookupCorner2<Vector2>* source_OLT_target = nullptr, Worker* worker = nullptr, const unsigned int binSizeInPixel = 8u, const T* borderColor = nullptr);

		/**
		 * Determines the interpolated pixel values for a given pixel position in an 8 bit per channel frame.
		 * This function uses an integer interpolation with a precision of 1/128.
		 * @param frame The frame to determine the pixel values from, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param position The position for which the interpolated pixel will be determined, with ranges [0, width - 1]x[0, height - 1] for PC_TOP_LEFT, [0, width]x[0, height] for PC_CENTER
		 * @param result Resulting pixel values, must be valid, must be valid
		 * @tparam tChannels Number of channels of the given frame, with range [1, infinity)
		 * @tparam tPixelCenter The pixel center to be used during interpolation, either 'PC_TOP_LEFT' or 'PC_CENTER'
		 * @tparam TScalar The scalar data type of the sub-pixel position
		 * @see interpolatePixel().
		 */
		template <unsigned int tChannels, PixelCenter tPixelCenter = PC_TOP_LEFT, typename TScalar = Scalar>
		static inline void interpolatePixel8BitPerChannel(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const VectorT2<TScalar>& position, uint8_t* result);

		/**
		 * Determines the interpolated pixel values for a given pixel position in a frame with arbitrary data type.
		 * This function uses floating point precision during interpolation.
		 * @param frame The frame to determine the pixel values from, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param position The position for which the interpolated pixel will be determined, with ranges [0, width - 1]x[0, height - 1] for PC_TOP_LEFT, [0, width]x[0, height] for PC_CENTER
		 * @param result Resulting interpolated pixel value(s), must be valid
		 * @param resultBias Optional bias value which will be added to the interpolation result e.g. to handle rounding, with range (-infinity, infinity), default is zero
		 * @tparam TSource The data type of the provided pixel values in the (source) frame
		 * @tparam TTarget The data type of the resulting interpolated value(s)
		 * @tparam tChannels Number of channels of the given frame, with range [1, infinity)
		 * @tparam tPixelCenter The pixel center to be used during interpolation, either 'PC_TOP_LEFT' or 'PC_CENTER'
		 * @tparam TScalar The data type of each coordinate of the provided interpolation location, should be either Scalar, float, or double
		 * @tparam TIntermediate The data type of the intermediate interpolation result before assigning the result
		 * @see interpolatePixel8BitPerChannel().
		 */
		template <typename TSource, typename TTarget, unsigned int tChannels, PixelCenter tPixelCenter = PC_TOP_LEFT, typename TScalar = Scalar, typename TIntermediate = TScalar>
		static inline void interpolatePixel(const TSource* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const VectorT2<TScalar>& position, TTarget* result, const TIntermediate& resultBias = TIntermediate(0));

		/**
		 * Determines the interpolated pixel values for a given pixel position in an 8 bit per channel frame with alpha channel.
		 * The center of each pixel is located with an offset of (0.5 x 0.5) in relation to the real pixel position.<br>
		 * The given frame is virtually extended by a fully transparent border so that this functions supports arbitrary interpolation positions.<br>
		 * If the given position lies inside the frame area of (-0.5, -0.5) -> (width + 0.5, height + 0.5) the resulting interpolation result will contain color information of the frame, otherwise a fully transparent interpolation result is provided.<br>
		 * @param frame The frame to determine the pixel values from, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param position The position to determine the interpolated pixel values for, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param result Resulting pixel values, must be valid
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @tparam tChannels Number of channels of the given frame, with range [1, infinity)
		 * @tparam tAlphaAtFront True, if the alpha channel is in the front of the data channels
		 * @tparam tTransparentIs0xFF True, if 0xFF is interpreted as fully transparent
		 */
		template <unsigned int tChannels, bool tAlphaAtFront, bool tTransparentIs0xFF>
		static inline void interpolate1PixelFullAlphaBorder8BitPerChannel(const uint8_t* frame, const unsigned int width, const unsigned int height, const Vector2& position, uint8_t* result, const unsigned int framePaddingElements);

		/**
		 * Interpolate the sum of intensity values of an image patch in a frame, while the frame is provided as lined integral frame.
		 * @param linedIntegralFrame The lined integral image created from the actual gray-scale image for which the patch intensity sum will be determined, must be valid
		 * @param frameWidth Width of the original frame in pixel (not the width of the lined-integral frame), with range [1, infinity)
		 * @param frameHeight Height of the original frame in pixel (not the height of the lined-integral frame), with range [1, infinity)
		 * @param lineIntegralFramePaddingElements The number of padding elements at the end of each integral image row, in elements, with range [0, infinity)
		 * @param center 2D coordinates of the center point of the patch, with range [patchWidth/2, frameWidth - patchWidth/2)x[patchHeight/2, frameHeight - patchHeight/2) for PC_CENTER
		 * @param pixelCenter The pixel center to be used during interpolation, either 'PC_TOP_LEFT' or 'PC_CENTER'
		 * @param patchWidth Width of the calculated patch in pixel with range [1, frameWidth - 1]
		 * @param patchHeight Height of the calculated patch in pixel with range [1, frameHeight - 1]
		 * @return The resulting sum of the pixel intensities
		 */
		static Scalar patchIntensitySum1Channel(const uint32_t* linedIntegralFrame, const unsigned int frameWidth, const unsigned int frameHeight, const unsigned int lineIntegralFramePaddingElements, const Vector2& center, const CV::PixelCenter pixelCenter, const unsigned int patchWidth, const unsigned int patchHeight);

		/**
		 * Checks whether the application of a given homography for a specified input frame and output frame covers the entire image information (which is necessary for the bilinear interpolation) or whether the homography relies on missing image information.
		 * @param inputWidth The width of the input frame in pixel, with range [1, infinity)
		 * @param inputHeight The height of the input frame in pixel, with range [1, infinity)
		 * @param outputWidth The width of the output frame in pixel, with range [1, infinity)
		 * @param outputHeight The height of the output frame in pixel, with range [1, infinity)
		 * @param input_H_output Homography used to transform the given input frame by following equation: inputPoint = input_H_output * outputPoint, must be valid
		 * @param outputOriginX The horizontal origin of the output frame defining the global position of the output frame's pixel coordinate (0, 0), with range (-infinity, infinity)
		 * @param outputOriginY The vertical origin of the output frame defining the global position of the output frame's pixel coordinate (0, 0), with range (-infinity, infinity)
		 * @return True, if the homography covers the entire input image information (if all output pixels will receive valid data from the input frame); False, otherwise
		 */
		static bool coversHomographyInputFrame(const unsigned int inputWidth, const unsigned int inputHeight, const unsigned int outputWidth, const unsigned int outputHeight, const SquareMatrix3& input_H_output, const int outputOriginX = 0, const int outputOriginY = 0);

	private:

		/**
		 * Transforms a given 8 bit per channel input frame into an output frame by application of a homography.
		 * The output frame must have the same pixel format and pixel origin as the input frame, however the dimension (and position) of the output frame can be arbitrary.<br>
		 * This function allows the creation of an output frame fully covering the input frame (if the position and dimension of the output frame covers the transformation of the homography).<br>
		 * The 'outputOrigin' parameter simply applies an additional translation onto the provided homography i.e., homography * create_translation_matrix3x3(outputOrigin.x(), outputOrigin.y()).
		 * @param input The input frame that will be transformed, must be valid
		 * @param inputWidth Width of both images in pixel, with range [1, infinity)
		 * @param inputHeight Height of both images pixel, with range [1, infinity)
		 * @param input_H_output Homography used to transform the given input frame by following equation: inputPoint = input_H_output * outputPoint, must be valid
		 * @param borderColor Color of undefined pixel positions, the size of the buffer must match to the number of channels, nullptr to assign 0x00 to each channel
		 * @param output The output frame using the given homography, must be valid
		 * @param outputOrigin The origin of the output frame defining the global position of the output frame's pixel coordinate (0, 0), with range (-infinity, infinity)x(-infinity, infinity)
		 * @param outputWidth The width of the output image in pixel, with range [1, infinity)
		 * @param outputHeight The height of the output image in pixel, with range [1, infinity)
		 * @param inputPaddingElements The number of padding elements at the end of each input frame row, in elements, with range [0, infinity)
		 * @param outputPaddingElements The number of padding elements at the end of each output frame row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 * @tparam tChannels Number of channels of the frame
		 * @see homographyMask8BitPerChannel(), homographyWithCamera8BitPerChannel(), homography().
		 */
		template <unsigned int tChannels>
		static inline void homography8BitPerChannel(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const SquareMatrix3& input_H_output, const uint8_t* borderColor, uint8_t* output, const PixelPositionI& outputOrigin, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, Worker* worker = nullptr);

		/**
		 * Rescales a given frame with 8 bit per data channel by using a bilinear interpolation with user-defined scaling factors.
		 * The frame must have a 1-plane pixel format with DT_UNSIGNED_INTEGER_8 as data type (e.g., FORMAT_Y8, FORMAT_RGB24, FORMAT_RGBA32, ...).<br>
		 * Information: This function is the equivalent to OpenCV's cv::resize().
		 * @param source The source frame buffer providing the image information to be resized, must be valid
		 * @param target The target frame buffer receiving the rescaled image information, must be valid
		 * @param sourceWidth Width of the source frame in pixel, with range [1, infinity)
		 * @param sourceHeight Height of the source frame in pixel, with range [1, infinity)
		 * @param targetWidth Width of the target frame in pixel, with range [1, infinity)
		 * @param targetHeight Height of the target frame in pixel, with range [1, infinity)
		 * @param sourceX_s_targetX The horizontal scale factor converting a location in the target frame to a location in the source frame (xSource = sourceX_s_targetX * xTarget), with range (0, sourceWidth/targetWidth]
		 * @param sourceY_s_targetY The vertical scale factor converting a location in the target frame to a location in the source frame (ySource = sourceY_s_targetY * yTarget), with range (0, sourceHeight/targetHeight]
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation to several CPU cores
		 * @tparam tChannels Number of channels of the frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static inline void scale8BitPerChannel(const uint8_t* source, uint8_t* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const double sourceX_s_targetX, const double sourceY_s_targetY, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Resizes a subset of a given frame with 8 bit per channel by a bilinear interpolation.
		 * @param source The image data of the source frame to be resized, must be valid
		 * @param target The target frame buffer receiving the interpolated (resized) source frame, must be valid
		 * @param sourceWidth Width of the source frame in pixel, with range [1, infinity)
		 * @param sourceHeight Height of the source frame in pixel, with range [1, infinity)
		 * @param targetWidth Width of the target frame in pixel, with range [1, infinity)
		 * @param targetHeight Height of the target frame in pixel, with range [1, infinity)
		 * @param sourceX_s_targetX The horizontal scale factor converting a location in the target frame to a location in the source frame (xSource = sourceX_s_targetX * xTarget), with range (0, sourceWidth/targetWidth]
		 * @param sourceY_s_targetY The vertical scale factor converting a location in the target frame to a location in the source frame (ySource = sourceY_s_targetY * yTarget), with range (0, sourceHeight/targetHeight]
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param firstTargetRow The first target row to be handled, with range [0, targetHeight)
		 * @param numberTargetRows The number of target row to be handled, with range [1, targetHeight - firstTargetRow]
		 * @tparam tChannels Number of frame channels, with range [0, infinity)
		 */
		template <unsigned int tChannels>
		static void scale8BitPerChannelSubset(const uint8_t* source, uint8_t* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const double sourceX_s_targetX, const double sourceY_s_targetY, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstTargetRow, const unsigned int numberTargetRows);

		/**
		 * Applies a (horizontal) linear interpolation for one row with 8 bit per channel.
		 * This function uses interpolation factors with 7 bit precision and does not apply any SIMD instructions.
		 * @param extendedSourceRow The source row extended with a copy of the last pixel for which the interpolation will be applied, must be valid
		 * @param targetRow The target row receiving the interpolation result, must be valid
		 * @param targetWidth The with of the target row in pixel, with range [8, infinity)
		 * @param channels The number of frame channels, must be identical with 'tChannels', possible values are 1, 4
		 * @param interpolationLocations The successive locations within the source row defining the location of the left pixels to be interpolated (specified in elements !not! in pixels - e.g., interpolationLocations[0] = firstInterpolationPixel * channels), one for each target pixel, with range [0, (targetWidth - 1) * channels]
		 * @param interpolationFactors The two successive (left !and! right) interpolation factors for each left and right source pixel, with range [0, 128]
		 * @see interpolateRowHorizontal8BitPerChannel7BitPrecisionNEON<tChannels>().
		 */
		static void interpolateRowHorizontal8BitPerChannel7BitPrecision(const uint8_t* extendedSourceRow, uint8_t* targetRow, const unsigned int targetWidth, const unsigned int channels, const unsigned int* interpolationLocations, const uint8_t* interpolationFactors);

		/**
		 * Applies a (vertical) linear interpolation between two rows with arbitrary data types.
		 * This function does not apply any SIMD instructions.<br>
		 * The length of both source rows is identical with the length of the target row.
		 * @param sourceRowTop The top source row to be used for interpolation, must be valid
		 * @param sourceRowBottom The bottom source row to be used for interpolation, must be valid
		 * @param targetRow The target row receiving the interpolation result, must be valid
		 * @param elements The number of elements in the row to (width * channels), with range [1, infinity)
		 * @param factorBottom The interpolation factor for all elements of the bottom row, with factorTop = 1 - factorBottom, with range [0, 1]
		 * @tparam T The data type of each element, should be 'float'
		 */
		template <typename T>
		static void interpolateRowVertical(const T* sourceRowTop, const T* sourceRowBottom, T* targetRow, const unsigned int elements, const float factorBottom);

		/**
		 * Applies a (horizontal) linear interpolation for one row with arbitrary data type.
		 * This function does not apply any SIMD instructions.
		 * @param extendedSourceRow The source row extended with a copy of the last pixel for which the interpolation will be applied, must be valid
		 * @param targetRow The target row receiving the interpolation result, must be valid
		 * @param targetWidth The with of the target row in pixel, with range [8, infinity)
		 * @param channels The number of frame channels, must be identical with 'tChannels', possible values are 1, 4
		 * @param interpolationLocations The successive locations within the source row defining the location of the left pixels to be interpolated (specified in elements !not! in pixels - e.g., interpolationLocations[0] = firstInterpolationPixel * channels), one for each target pixel, with range [0, (targetWidth - 1) * channels]
		 * @param interpolationFactorsRight The right interpolation factors for each right source pixel, with range [0, 1]
		 * @tparam T The data type of each element, should be 'float'
		 * @tparam tChannels The number of frame channels this function can handle, should be 1
		 * @see interpolateRowHorizontal8BitPerChannel7BitPrecision().
		 */
		template <typename T, unsigned int tChannels>
		static void interpolateRowHorizontal(const T* extendedSourceRow, T* targetRow, const unsigned int targetWidth, const unsigned int channels, const unsigned int* interpolationLocations, const float* interpolationFactorsRight);

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

		/**
		 * Applies a (vertical) linear interpolation between two rows with 8 bit per channel.
		 * This function applies NEON instructions and uses interpolation factors with 7 bit precision.<br>
		 * The length of both source rows is identical with the length of the target row.
		 * @param sourceRowTop The top source row to be used for interpolation, must be valid
		 * @param sourceRowBottom The bottom source row to be used for interpolation, must be valid
		 * @param targetRow The target row receiving the interpolation result, must be valid
		 * @param elements The number of elements in the row to (width * channels), with range [16, infinity)
		 * @param factorBottom The interpolation factor for all elements of the bottom row, with factorTop = 128 - factorBottom, with range [0, 128]
		 */
		static void interpolateRowVertical8BitPerChannel7BitPrecisionNEON(const uint8_t* sourceRowTop, const uint8_t* sourceRowBottom, uint8_t* targetRow, const unsigned int elements, const unsigned int factorBottom);

		/**
		 * Applies a (vertical) linear interpolation between two rows with arbitrary data types.
		 * This function applies NEON instructions.<br>
		 * The length of both source rows is identical with the length of the target row.
		 * @param sourceRowTop The top source row to be used for interpolation, must be valid
		 * @param sourceRowBottom The bottom source row to be used for interpolation, must be valid
		 * @param targetRow The target row receiving the interpolation result, must be valid
		 * @param elements The number of elements in the row to (width * channels), with range [16, infinity)
		 * @param factorBottom The interpolation factor for all elements of the bottom row, with factorTop = 1 - factorBottom, with range [0, 1]
		 * @tparam T The data type of each element, should be 'float'
		 */
		template <typename T>
		static void interpolateRowVerticalNEON(const T* sourceRowTop, const T* sourceRowBottom, T* targetRow, const unsigned int elements, const float factorBottom);

		/**
		 * Applies a (horizontal) linear interpolation for one row with 8 bit per channel.
		 * This function applies NEON instructions and uses interpolation factors with 7 bit precision.
		 * @param extendedSourceRow The source row extended with a copy of the last pixel for which the interpolation will be applied, must be valid
		 * @param targetRow The target row receiving the interpolation result, must be valid
		 * @param targetWidth The with of the target row in pixel, with range [8, infinity)
		 * @param channels The number of frame channels, must be identical with 'tChannels', possible values are 1, 4
		 * @param interpolationLocations The successive locations within the source row defining the location of the left pixels to be interpolated (specified in elements !not! in pixels - e.g., interpolationLocations[0] = firstInterpolationPixel * channels), one for each target pixel, with range [0, (targetWidth - 1) * channels]
		 * @param interpolationFactors The two successive (left !and! right) interpolation factors for each left and right source pixel, with range [0, 128]
		 * @tparam tChannels The number of frame channels this function can handle, possible values are 1, 4
		 * @see interpolateRowHorizontal8BitPerChannel7BitPrecision().
		 */
		template <unsigned int tChannels>
		static void interpolateRowHorizontal8BitPerChannel7BitPrecisionNEON(const uint8_t* extendedSourceRow, uint8_t* targetRow, const unsigned int targetWidth, const unsigned int channels, const unsigned int* interpolationLocations, const uint8_t* interpolationFactors);

		/**
		 * Applies a (horizontal) linear interpolation for one row with arbitrary data type.
		 * This function applies NEON instructions.
		 * @param extendedSourceRow The source row extended with a copy of the last pixel for which the interpolation will be applied, must be valid
		 * @param targetRow The target row receiving the interpolation result, must be valid
		 * @param targetWidth The with of the target row in pixel, with range [8, infinity)
		 * @param channels The number of frame channels, must be identical with 'tChannels', possible values are 1, 4
		 * @param interpolationLocations The successive locations within the source row defining the location of the left pixels to be interpolated (specified in elements !not! in pixels - e.g., interpolationLocations[0] = firstInterpolationPixel * channels), one for each target pixel, with range [0, (targetWidth - 1) * channels]
		 * @param interpolationFactorsRight The right interpolation factors for each right source pixel, with range [0, 1]
		 * @tparam T The data type of each element, should be 'float'
		 * @tparam tChannels The number of frame channels this function can handle, should be 1
		 * @see interpolateRowHorizontal8BitPerChannel7BitPrecision().
		 */
		template <typename T, unsigned int tChannels>
		static void interpolateRowHorizontalNEON(const T* extendedSourceRow, T* targetRow, const unsigned int targetWidth, const unsigned int channels, const unsigned int* interpolationLocations, const float* interpolationFactorsRight);

		/**
		 * Rescales a subset of a given frame with 8 bit per channel by a bilinear interpolation.
		 * This function applies NEON instructions and uses interpolation factors with 7 bit precision.
		 * @param source The image data of the source frame to be resized, must be valid
		 * @param target The target frame buffer receiving the interpolated (resized) source frame, must be valid
		 * @param sourceWidth Width of the source frame in pixel, with range [2, 65.535]
		 * @param sourceHeight Height of the source frame in pixel, with range [1, 65.535]
		 * @param targetWidth Width of the target frame in pixel, with range [tMinimalTargetWidth, 65.535]
		 * @param targetHeight Height of the target frame in pixel, with range [1, infinity)
		 * @param channels The number of channels both frames have, with range [1, infinity)
		 * @param sourceX_s_targetX The horizontal scale factor converting a location in the target frame to a location in the source frame (xSource = sourceX_s_targetX * xTarget), with range (0, sourceWidth/targetWidth]
		 * @param sourceY_s_targetY The vertical scale factor converting a location in the target frame to a location in the source frame (ySource = sourceY_s_targetY * yTarget), with range (0, sourceHeight/targetHeight]
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param firstTargetRow The first target row to be handled, with range [0, targetHeight)
		 * @param numberTargetRows The number of target row to be handled, with range [1, targetHeight - firstTargetRow]
		 * @see interpolateRowVertical8BitPerChannel7BitPrecisionNEON(), interpolateRowHorizontal8BitPerChannel7BitPrecisionNEON().
		 */
		static void scale8BitPerChannelSubset7BitPrecisionNEON(const uint8_t* source, uint8_t* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int channels, const double sourceX_s_targetX, const double sourceY_s_targetY, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstTargetRow, const unsigned int numberTargetRows);

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

		/**
		 * Resizes a subset of a given frame with arbitrary data type by a bilinear interpolation.
		 * @param source The image data of the source frame to be resized, must be valid
		 * @param target The target frame buffer receiving the interpolated (resized) source frame, must be valid
		 * @param sourceWidth Width of the source frame in pixel, with range [1, infinity)
		 * @param sourceHeight Height of the source frame in pixel, with range [1, infinity)
		 * @param targetWidth Width of the target frame in pixel, with range [1, infinity)
		 * @param targetHeight Height of the target frame in pixel, with range [1, infinity)
		 * @param sourceX_s_targetX The horizontal scale factor converting a location in the target frame to a location in the source frame (xSource = sourceX_s_targetX * xTarget), with range (0, sourceWidth/targetWidth]
		 * @param sourceY_s_targetY The vertical scale factor converting a location in the target frame to a location in the source frame (ySource = sourceY_s_targetY * yTarget), with range (0, sourceHeight/targetHeight]
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param firstTargetRow The first target row to be handled, with range [0, targetHeight)
		 * @param numberTargetRows The number of target row to be handled, with range [1, targetHeight - firstTargetRow]
		 * @tparam T The data type of each pixel channel, e.g., float, double, int, short, ...
		 * @tparam TScale The data type of the internal scaling factors to be used, should be 'float' or 'double'
		 * @tparam tChannels Number of frame channels, with range [0, infinity)
		 */
		template <typename T, typename TScale, unsigned int tChannels>
		static void scaleSubset(const T* source, T* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const double sourceX_s_targetX, const double sourceY_s_targetY, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstTargetRow, const unsigned int numberTargetRows);

		/**
		 * Rotates a subset of a given frame by a bilinear interpolation.
		 * @param source The source frame to be rotated, must be valid
		 * @param target The target frame which will receive the rotated image, with same frame type as the source frame, must be valid
		 * @param width The width of the source and target frame in pixel, with range [1, infinity)
		 * @param height The height of the source and target frame in pixel, with range [1, infinity)
		 * @param horizontalAnchorPosition Position of the rotation anchor in the horizontal direction, with range (-infinity, infinity)
		 * @param verticalAnchorPosition Position of the rotation anchor in the vertical direction, with range (-infinity, infinity)
		 * @param angle The counter clockwise rotation angle in radian, with range [0, 2PI)
		 * @param borderColor The color of border pixels for which now visual content exists, provide one value for each channel, nullptr to use 0x00 for each channel
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param firstTargetRow The first row of the target frame to be handled, with range [0, height)
		 * @param numberTargetRows The number of rows in the target frame to be handled, with range [1, height - firstTargetRow]
		 * @tparam tChannels Number of frame channels, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void rotate8BitPerChannelSubset(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const Scalar horizontalAnchorPosition, const Scalar verticalAnchorPosition, const Scalar angle, const uint8_t* borderColor, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstTargetRow, const unsigned int numberTargetRows);

		/**
		 * Subset function to apply an affine transform to an N-channel, 8-bit unsigned image.
		 * The affine transform must be provided in the following form: `sourcePoint = source_A_target * targetPoint`
		 * This function does not apply SIMD instructions and can be used for any frame dimensions.
		 * Please note that here the affine transformation is specified as a 3-by-3 matrix (in contrast to the more commonly used 2-by-3 matrix) and should take of the form:
		 * <pre>
		 * a c e
		 * b d f
		 * 0 0 1
		 * </pre>
		 * However, this function disregards the last row completely and only uses the top two rows, i.e., the elements a through f.
		 * @param source Input frame that will be transformed
		 * @param sourceWidth Width of both source images in pixel, with range [1, infinity)
		 * @param sourceHeight Height of both source images pixel, with range [1, infinity)
		 * @param source_A_target Affine transformation which is applied to the source frame.
		 * @param borderColor Color of undefined pixel positions, the size of the buffer must match to the number of channels, nullptr to assign 0x00 to each channel
		 * @param target Output frame using the given affine transform
		 * @param targetWidth The width of the target image in pixel, with range [1, infinity)
		 * @param targetHeight The height of the target image in pixel, with range [1, infinity)
		 * @param firstTargetRow The first target row to be handled
		 * @param numberTargetRows Number of target rows to be handled
		 * @param sourcePaddingElements The number of padding elements at the end of each source frame, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target frame, in elements, with range [0, infinity)
		 * @tparam tChannels Number of frame channels, with range [1, infinity)
		 * @see affine8BitPerChannelSSESubset(), affine8BitPerChannelNEONSubset()
		 */
		 template <unsigned int tChannels>
		 static inline void affine8BitPerChannelSubset(const uint8_t* source, const unsigned int sourceWidth, const unsigned int sourceHeight, const SquareMatrix3* source_A_target, const uint8_t* borderColor, uint8_t* target, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int firstTargetRow, const unsigned int numberTargetRows, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements);

		/**
		 * Transforms an 8 bit per channel frame using the given homography.
		 * The homography must provide the following transformation: inputPoint = homography * outputPoint
		 * This function does not apply SIMD instructions and can be used for any frame dimensions.
		 * @param input The input frame that will be transformed
		 * @param inputWidth Width of both input images in pixel, with range [1, infinity)
		 * @param inputHeight Height of both input images pixel, with range [1, infinity)
		 * @param input_H_output Homography used to transform the given input frame by following equation: inputPoint = input_H_output * outputPoint, must be valid
		 * @param borderColor Color of undefined pixel positions, the size of the buffer must match to the number of channels, nullptr to assign 0x00 to each channel
		 * @param output The output frame using the given homography
		 * @param outputWidth The width of the output image in pixel, with range [1, infinity)
		 * @param outputHeight The height of the output image in pixel, with range [1, infinity)
		 * @param inputPaddingElements The number of padding elements at the end of each input frame row, in elements, with range [0, infinity)
		 * @param outputPaddingElements The number of padding elements at the end of each output frame row, in elements, with range [0, infinity)
		 * @param firstOutputRow The first output row to be handled, with range [0, height - 1]
		 * @param numberOutputRows Number of output rows to be handled, with range [1, height - firstOutputRow]
		 * @tparam tChannels Number of frame channels, with range [1, infinity)
		 * @see homography8BitPerChannelSSESubset(), homography8BitPerChannelNEONSubset()
		 */
		template <unsigned int tChannels>
		static inline void homography8BitPerChannelSubset(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const SquareMatrix3* input_H_output, const uint8_t* borderColor, uint8_t* output, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, const unsigned int firstOutputRow, const unsigned int numberOutputRows);

		/**
		 * Transforms a frame with (almost) arbitrary pixel format using the given homography.
		 * This function does not apply SIMD instructions and can be used for any frame dimensions.
		 * @param input The input frame that will be transformed
		 * @param inputWidth Width of both input images in pixel, with range [1, infinity)
		 * @param inputHeight Height of both input images pixel, with range [1, infinity)
		 * @param input_H_output Homography used to transform the given input frame by following equation: inputPoint = input_H_output * outputPoint, must be valid
		 * @param borderColor Color of undefined pixel positions, the size of the buffer must match to the number of channels, nullptr to assign 0x00 to each channel
		 * @param output The output frame using the given homography
		 * @param outputWidth The width of the output image in pixel, with range [1, infinity)
		 * @param outputHeight The height of the output image in pixel, with range [1, infinity)
		 * @param inputPaddingElements The number of padding elements at the end of each input frame row, in elements, with range [0, infinity)
		 * @param outputPaddingElements The number of padding elements at the end of each output frame row, in elements, with range [0, infinity)
		 * @param firstOutputRow The first output row to be handled, with range [0, height - 1]
		 * @param numberOutputRows Number of output rows to be handled, with range [1, height - firstOutputRow]
		 * @tparam T Data type of each pixel channel, e.g., float, double, int
		 * @tparam tChannels Number of frame channels, with range [1, infinity)
		 * @see homography8BitPerChannelSSESubset().
		 */
		template <typename T, unsigned int tChannels>
		static inline void homographySubset(const T* input, const unsigned int inputWidth, const unsigned int inputHeight, const SquareMatrix3* input_H_output, const T* borderColor, T* output, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, const unsigned int firstOutputRow, const unsigned int numberOutputRows);

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

		/**
		 * Subset function to apply an affine transform to an N-channel, 8-bit unsigned image (using SSE).
		 * This function applies SSE instructions.<br>
		 * Beware: The target width 'targetWidth' must be >= 4, use affine8BitPerChannelSubset for small target frames
		 * This function has the property: sourcePoint = source_A_target * targetPoint
		 * Please note that here the affine transformation is specified as a 3-by-3 matrix (in contrast to the more commonly used 2-by-3 matrix) and should take of the form:
		 * <pre>
		 * a c e
		 * b d f
		 * 0 0 1
		 * </pre>
		 * However, this function disregards the last row completely and only uses the top two rows, i.e., the elements a through f.
		 * @param source Input frame that will be transformed
		 * @param sourceWidth Width of both source images in pixel, with range [1, infinity)
		 * @param sourceHeight Height of both source images pixel, with range [1, infinity)
		 * @param source_A_target Affine transformation which is applied to source frame.
		 * @param borderColor Color of undefined pixel positions, the size of the buffer must match to the number of channels, nullptr to assign 0x00 to each channel
		 * @param target The target frame where the result of the transformation will be stored
		 * @param targetWidth The width of the target image in pixel, with range [4, infinity)
		 * @param targetHeight The height of the target image in pixel, with range [1, infinity)
		 * @param firstTargetRow The first target row to be handled
		 * @param numberTargetRows Number of target rows to be handled
		 * @param sourcePaddingElements The number of padding elements at the end of each source frame, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target frame, in elements, with range [0, infinity)
		 * @tparam tChannels Number of frame channels
		 * @see affine8BitPerChannelSubset(), affine8BitPerChannelSubsetNEON().
		 */
		template <unsigned int tChannels>
		static inline void affine8BitPerChannelSSESubset(const uint8_t* source, const unsigned int sourceWidth, const unsigned int sourceHeight, const SquareMatrix3* source_A_target, const uint8_t* borderColor, uint8_t* target, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int firstTargetRow, const unsigned int numberTargetRows, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements);

		/**
		 * Transforms an 8 bit per channel frame using the given homography.
		 * This function applies SSE instructions.<br>
		 * Beware: The output width 'outputWidth' must be >= 4, use homography8BitPerChannelSubset for small output frames
		 * @param input The input frame that will be transformed, must be valid
		 * @param inputWidth Width of both input images in pixel, with range [1, infinity)
		 * @param inputHeight Height of both input images pixel, with range [1, infinity)
		 * @param input_H_output Homography used to transform the given input frame by following equation: inputPoint = input_H_output * outputPoint, must be valid
		 * @param borderColor Color of undefined pixel positions, the size of the buffer must match to the number of channels, nullptr to assign 0x00 to each channel
		 * @param output The output frame using the given homography, must be valid
		 * @param outputWidth The width of the output image in pixel, with range [4, infinity)
		 * @param outputHeight The height of the output image in pixel, with range [1, infinity)
		 * @param inputPaddingElements The number of padding elements at the end of each input frame row, in elements, with range [0, infinity)
		 * @param outputPaddingElements The number of padding elements at the end of each output frame row, in elements, with range [0, infinity)
		 * @param firstOutputRow The first output row to be handled, with range [0, outputHeight - 1]
		 * @param numberOutputRows Number of output rows to be handled, with range [1, outputHeight - firstOutputRow]
		 * @tparam tChannels Number of frame channels, with range [1, infinity)
		 * @see homography8BitPerChannelSubset().
		 */
		template <unsigned int tChannels>
		static inline void homography8BitPerChannelSSESubset(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const SquareMatrix3* input_H_output, const uint8_t* borderColor, uint8_t* output, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, const unsigned int firstOutputRow, const unsigned int numberOutputRows);

		/**
		 * Interpolates 4 independent pixels concurrently based on already known locations (top-left, top-right, bottom-left, and bottom-right) and interpolation factors for the source pixels.
		 * This function also supports to interpolate the pixel values for a subset of the four pixels only, valid pixels will be interpolated, invalid pixels receive a defined border color.
		 * @param source The source image in which the four independent pixels are located, must be valid
		 * @param offsetsTopLeft The four offsets within the source image for the four top-left pixels used for the interpolation, with range [0, (width * tChannels + sourcePaddingElements) * (height - 1) + width * tChannels), or the corresponding validPixel information is 0x00000000
		 * @param offsetsTopRight The four offsets within the source image for the four top-right pixels used for the interpolation, with range [0, (width * tChannels + sourcePaddingElements) * (height - 1) + width * tChannels), or the corresponding validPixel information is 0x00000000
		 * @param offsetsBottomLeft The four offsets within the source image for the four bottom-right pixels used for the interpolation, with range [0, (width * tChannels + sourcePaddingElements) * (height - 1) + width * tChannels), or the corresponding validPixel information is 0x00000000
		 * @param offsetsBottomRight The four offsets within the source image for the four bottom-right pixels used for the interpolation, with range [0, (width * tChannels + sourcePaddingElements) * (height - 1) + width * tChannels), or the corresponding validPixel information is 0x00000000
		 * @param validPixels For boolean states specifying whether which of the given four pixels will be interpolated and which one will receive the defined border color, 0x00000000 for invalid pixels everything else for valid pixels (e.g., 0xFFFFFFFF)
		 * @param borderColor The border color that will be assigned to each resulting pixel that is invalid (for which the corresponding validPixel information is 0x00000000
		 * @param m128_factorsRight The horizontal interpolation factors for right pixels, with range [0, 128], 128 to use the color information of the right pixels only, 0 to use the color information of the left pixels only
		 * @param m128_factorsBottom The vertical interpolation factors for bottom pixels, with range [0, 128], 128 to use the color information of the bottom pixels only, 0 to use the color information of the top pixels only
		 * @param targetPositionPixels The buffer that will receive the interpolated color values, must be valid
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static OCEAN_FORCE_INLINE void interpolate4Pixels8BitPerChannelSSE(const uint8_t* source, const unsigned int offsetsTopLeft[4], const unsigned int offsetsTopRight[4], const unsigned int offsetsBottomLeft[4], const unsigned int offsetsBottomRight[4], const unsigned int validPixels[4], const typename DataType<uint8_t, tChannels>::Type& borderColor, const __m128i& m128_factorsRight, const __m128i& m128_factorsBottom, typename DataType<uint8_t, tChannels>::Type* targetPositionPixels);

		/**
		 * Interpolates 4 independent pixels concurrently based on already known locations (top-left, top-right, bottom-left, and bottom-right) and interpolation factors for the source pixels.
		 * This function also supports to interpolate the pixel values for a subset of the four pixels only, valid pixels will be interpolated, invalid pixels receive a defined border color.
		 * @param m128_sourcesTopLeft The pixel values of the four top left pixels, starting at the first byte may contain unused bytes at the end, e.g., RGBARGBARGBARGBA or YUVYUVYUVYUV----
		 * @param m128_sourcesTopRight The pixel values of the four top right pixels, starting at the first byte may contain unused bytes at the end
		 * @param m128_sourcesBottomLeft The pixel values of the four bottom left pixels, starting at the first byte may contain unused bytes at the end
		 * @param m128_sourcesBottomRight The pixel values of the four bottom right pixels, starting at the first byte may contain unused bytes at the end
		 * @param m128_factorsTopLeft The four interpolation factors of the four top left pixels, with ranges [0, 128 * 128], so that (m128_factorsTopLeft + m128_factorsTopRight + m128_factorsBottomLeft + m128_factorsBottomRight) == (128 * 128)
		 * @param m128_factorsTopRight The four interpolation factors of the four top right pixels, with ranges [0, 128 * 128], so that (m128_factorsTopLeft + m128_factorsTopRight + m128_factorsBottomLeft + m128_factorsBottomRight) == (128 * 128)
		 * @param m128_factorsBottomLeft The four interpolation factors of the four bottom left pixels, with ranges [0, 128 * 128], so that (m128_factorsTopLeft + m128_factorsTopRight + m128_factorsBottomLeft + m128_factorsBottomRight) == (128 * 128)
		 * @param m128_factorsBottomRight The four interpolation factors of the four bottom right pixels, with ranges [0, 128 * 128], so that (m128_factorsTopLeft + m128_factorsTopRight + m128_factorsBottomLeft + m128_factorsBottomRight) == (128 * 128)
		 * @return The resulting interpolated pixel values, starting at the first byte may contain unused bytes at the end, e.g., RGBARGBARGBARGBA or YUVYUVYUVYUV----
		 * @tparam tChannels The number of frame channels, with range [3, 4]
		 */
		template <unsigned int tChannels>
		static OCEAN_FORCE_INLINE __m128i interpolate4Pixels8BitPerChannelSSE(const __m128i& m128_sourcesTopLeft, const __m128i& m128_sourcesTopRight, const __m128i& m128_sourcesBottomLeft, const __m128i& m128_sourcesBottomRight, const __m128i& m128_factorsTopLeft, const __m128i& m128_factorsTopRight, const __m128i& m128_factorsBottomLeft, const __m128i& m128_factorsBottomRight);

#endif // OCEAN_HARDWARE_SSE_VERSION

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

		/**
		 * Subset function to apply an affine transform to an N-channel, 8-bit unsigned image (using NEON).
		 * This function applies NEON instructions.<br>
		 * This one has the property: sourcePoint = source_A_target * targetPoint
		 * Beware: The target width 'targetWidth' must be >= 4, use affine8BitPerChannelSubset for small target frames
		 * Please note that here the affine transformation is specified as a 3-by-3 matrix (in contrast to the more commonly used 2-by-3 matrix) and should take of the form:
		 * <pre>
		 * a c e
		 * b d f
		 * 0 0 1
		 * </pre>
		 * However, this function disregards the last row completely and only uses the top two rows, i.e., the elements a through f.
		 * @param source The source frame that will be transformed
		 * @param sourceWidth Width of both source images in pixel, with range [1, infinity)
		 * @param sourceHeight Height of both source images pixel, with range [1, infinity)
		 * @param source_A_target Affine transform used to transform the given source frame.
		 * @param borderColor Color of undefined pixel positions, the size of the buffer must match to the number of channels, nullptr to assign 0x00 to each channel
		 * @param target The target frame using the given affine transform
		 * @param targetWidth The width of the target image in pixel, with range [4, infinity)
		 * @param targetHeight The height of the target image in pixel, with range [1, infinity)
		 * @param firstTargetRow The first target row to be handled
		 * @param numberTargetRows Number of target rows to be handled
		 * @param sourcePaddingElements The number of padding elements at the end of each source frame, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target frame, in elements, with range [0, infinity)
		 * @tparam tChannels Number of frame channels, with range [1, infinity)
		 * @see homography8BitPerChannelSubset().
		 */
		template <unsigned int tChannels>
		static inline void affine8BitPerChannelNEONSubset(const uint8_t* source, const unsigned int sourceWidth, const unsigned int sourceHeight, const SquareMatrix3* source_A_target, const uint8_t* borderColor, uint8_t* target, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int firstTargetRow, const unsigned int numberTargetRows, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements);

		/**
		 * Transforms an 8 bit per channel frame using the given homography.
		 * This function applies NEON instructions.<br>
		 * Beware: The output width 'outputWidth' must be >= 4, use homography8BitPerChannelSubset for small output frames.
		 * @param input The input frame that will be transformed
		 * @param inputWidth Width of both input images in pixel, with range [1, infinity)
		 * @param inputHeight Height of both input images pixel, with range [1, infinity)
		 * @param input_H_output Homography used to transform the given input frame by following equation: inputPoint = input_H_output * outputPoint, must be valid
		 * @param borderColor Color of undefined pixel positions, the size of the buffer must match to the number of channels, nullptr to assign 0x00 to each channel
		 * @param output The output frame using the given homography
		 * @param outputWidth The width of the output image in pixel, with range [4, infinity)
		 * @param outputHeight The height of the output image in pixel, with range [1, infinity)
		 * @param inputPaddingElements The number of padding elements at the end of each input frame row, in elements, with range [0, infinity)
		 * @param outputPaddingElements The number of padding elements at the end of each output frame row, in elements, with range [0, infinity)
		 * @param firstOutputRow The first output row to be handled, with range [0, outputHeight - 1]
		 * @param numberOutputRows Number of output rows to be handled, with range [1, outputHeight - firstOutputRow]
		 * @tparam tChannels Number of frame channels, with range [1, infinity)
		 * @see homography8BitPerChannelSubset().
		 */
		template <unsigned int tChannels>
		static inline void homography8BitPerChannelNEONSubset(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const SquareMatrix3* input_H_output, const uint8_t* borderColor, uint8_t* output, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, const unsigned int firstOutputRow, const unsigned int numberOutputRows);

		/**
		 * Interpolates 4 independent pixels concurrently based on already known locations (top-left, top-right, bottom-left, and bottom-right) and interpolation factors for the source pixels.
		 * This function also supports to interpolate the pixel values for a subset of the four pixels only, valid pixels will be interpolated, invalid pixels receive a defined border color.
		 * @param source The source image in which the four independent pixels are located, must be valid
		 * @param offsetsTopLeftElements The four offsets within the source image for the four top-left pixels used for the interpolation, in elements, with ranges [0, strideElements * height), or the corresponding validPixel information is 0x00000000
		 * @param offsetsTopRightElements The four offsets within the source image for the four top-right pixels used for the interpolation, in elements, with ranges [0, strideElements * height), or the corresponding validPixel information is 0x00000000
		 * @param offsetsBottomLeftElements The four offsets within the source image for the four bottom-right pixels used for the interpolation, in elements, with ranges [0, strideElements * height), or the corresponding validPixel information is 0x00000000
		 * @param offsetsBottomRightElements The four offsets within the source image for the four bottom-right pixels used for the interpolation, in elements, with ranges [0, strideElements * height), or the corresponding validPixel information is 0x00000000
		 * @param validPixels For boolean states specifying whether which of the given four pixels will be interpolated and which one will receive the defined border color, 0x00000000 for invalid pixels everything else for valid pixels (e.g., 0xFFFFFFFF)
		 * @param borderColor The border color that will be assigned to each resulting pixel that is invalid (for which the corresponding validPixel information is 0x00000000
		 * @param m128_factorsRight The horizontal interpolation factors for right pixels, with range [0, 128], 128 to use the color information of the right pixels only, 0 to use the color information of the left pixels only
		 * @param m128_factorsBottom The vertical interpolation factors for bottom pixels, with range [0, 128], 128 to use the color information of the bottom pixels only, 0 to use the color information of the top pixels only
		 * @param targetPositionPixels The buffer that will receive the interpolated color values, must be valid
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static OCEAN_FORCE_INLINE void interpolate4Pixels8BitPerChannelNEON(const uint8_t* source, const unsigned int offsetsTopLeftElements[4], const unsigned int offsetsTopRightElements[4], const unsigned int offsetsBottomLeftElements[4], const unsigned int offsetsBottomRightElements[4], const unsigned int validPixels[4], const typename DataType<uint8_t, tChannels>::Type& borderColor, const uint32x4_t& m128_factorsRight, const uint32x4_t& m128_factorsBottom, typename DataType<uint8_t, tChannels>::Type* targetPositionPixels);

		/**
		 * Interpolates 8 independent pixels concurrently of a 1 channel frame, the source pixel locations must be known already (top-left, top-right, bottom-left, and bottom-right), further the interpolation factors must be known already.
		 * @param topLeft_u_8x8 The 8 top left pixel values to be used for interpolation
		 * @param topRight_u_8x8 The 8 top right pixel values to be used for interpolation
		 * @param bottomLeft_u_8x8 The 8 bottom left pixel values to be used for interpolation
		 * @param bottomRight_u_8x8 The 8 bottom right pixel values to be used for interpolation
		 * @param factorsRight_factorsBottom_128_u_8x16 The eight horizontal interpolation factors for right pixels, and the eight vertical interpolation factors for the bottom pixels, with range [0, 128], 128 to use the color information of the right pixels only, 0 to use the color information of the left pixels only
		 * @param targetPositionPixels The buffer that will receive the interpolated color values, must be valid
		*/
		static OCEAN_FORCE_INLINE void interpolate8Pixels1Channel8BitNEON(const uint8x8_t& topLeft_u_8x8, const uint8x8_t& topRight_u_8x8, const uint8x8_t& bottomLeft_u_8x8, const uint8x8_t& bottomRight_u_8x8, const uint8x16_t& factorsRight_factorsBottom_128_u_8x16, uint8_t* targetPositionPixels);

#endif // OCEAN_HARDWARE_SSE_VERSION

		/**
		 * Transforms an 8 bit per channel frame using the given homographies.
		 * @param input The input frame that will be transformed
		 * @param inputWidth Width of both input images in pixel, with range [1, infinity)
		 * @param inputHeight Height of both input images pixel, with range [1, infinity)
		 * @param homographies Homographies used to transform the given input frame
		 * @param borderColor Color of undefined pixel positions, the size of the buffer must match to the number of channels, nullptr to assign 0x00 to each channel
		 * @param output The output frame using the given homography
		 * @param outputQuadrantCenterX The horizontal position of the four quadrants in the output frame (the local center not respecting the optional outputOrigin parameter), with range [0, outputWidth)
		 * @param outputQuadrantCenterY The vertical position of the four quadrants in the output frame (the local center not respecting the optional outputOrigin parameter), with range [0, outputHeight)
		 * @param outputOriginX The horizontal coordinate of the output frame's origin
		 * @param outputOriginY The vertical coordinate of the output frame's origin
		 * @param outputWidth The width of the output image in pixel, with range [1, infinity)
		 * @param outputHeight The height of the output image in pixel, with range [1, infinity)
		 * @param inputPaddingElements The number of padding elements at the end of each input frame, in elements, with range [0, infinity)
		 * @param outputPaddingElements The number of padding elements at the end of each output frame, in elements, with range [0, infinity)
		 * @param firstOutputRow The first output row to be handled
		 * @param numberOutputRows Number of output rows to be handled
		 * @tparam tChannels Number of frame channels
		 */
		template <unsigned int tChannels>
		static inline void homographies8BitPerChannelSubset(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const SquareMatrix3* homographies, const uint8_t* borderColor, uint8_t* output, const Scalar outputQuadrantCenterX, const Scalar outputQuadrantCenterY, const int outputOriginX, const int outputOriginY, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, const unsigned int firstOutputRow, const unsigned int numberOutputRows);

		/**
		 * Transforms an 8 bit per channel frame using the given homography.
		 * @param input The input frame that will be transformed, must be valid
		 * @param inputWidth Width of both input images in pixel, with range [1, infinity)
		 * @param inputHeight Height of both input images pixel, with range [1, infinity)
		 * @param input_H_output Homography used to transform the given input frame by following equation: inputPoint = input_H_output * outputPoint, must be valid
		 * @param output The output frame resulting by application of the given homography, must be valid
		 * @param outputMask Mask frame with 8 bit per pixel defining whether an output frame pixel has a valid corresponding pixel in the input frame
		 * @param maskValue 8 bit mask values for reference pixels lying inside the given camera frame, reference pixels lying outside the camera frame will be assigned with (0xFF - maskValue)
		 * @param outputWidth The width of the output image in pixel, with range [1, infinity)
		 * @param outputHeight The height of the output image in pixel, with range [1, infinity)
		 * @param inputPaddingElements The number of padding elements at the end of each input row, in elements, with range [0, infinity)
		 * @param outputPaddingElements The number of padding elements at the end of each output row, in elements, with range [0, infinity)
		 * @param outputMaskPaddingElements The number of padding elements at the end of output mask input row, in elements, with range [0, infinity)
		 * @param firstOutputRow The first output row to be handled
		 * @param numberOutputRows Number of output rows to be handled
		 * @tparam tChannels Number of frame channels, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static inline void homographyMask8BitPerChannelSubset(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const SquareMatrix3* input_H_output, uint8_t* output, uint8_t* outputMask, const uint8_t maskValue, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, const unsigned int outputMaskPaddingElements, const unsigned int firstOutputRow, const unsigned int numberOutputRows);

		/**
		 * Transforms an 8 bit per channel frame using the given homography.
		 * @param input The input frame that will be transformed
		 * @param inputWidth Width of both input images in pixel, with range [1, infinity)
		 * @param inputHeight Height of both input images pixel, with range [1, infinity)
		 * @param homographies Homographies used to transform the given input frame
		 * @param output The output frame resulting by application of the given homography
		 * @param outputMask Mask frame with 8 bit per pixel defining whether an output frame pixel has a valid corresponding pixel in the input frame
		 * @param maskValue 8 bit mask values for reference pixels lying inside the given camera frame, reference pixels lying outside the camera frame will be assigned with (0xFF - maskValue)
		 * @param outputQuadrantCenterX The horizontal position of the four quadrants in the output frame (the local center not respecting the optional outputOrigin parameter), with range [0, outputWidth)
		 * @param outputQuadrantCenterY The vertical position of the four quadrants in the output frame (the local center not respecting the optional outputOrigin parameter), with range [0, outputHeight)
		 * @param outputOriginX The horizontal coordinate of the output frame's origin
		 * @param outputOriginY The vertical coordinate of the output frame's origin
		 * @param outputWidth The width of the output image in pixel, with range [1, infinity)
		 * @param outputHeight The height of the output image in pixel, with range [1, infinity)
		 * @param inputPaddingElements The number of padding elements at the end of each input row, in elements, with range [0, infinity)
		 * @param outputPaddingElements The number of padding elements at the end of each output row, in elements, with range [0, infinity)
		 * @param outputMaskPaddingElements The number of padding elements at the end of each row of the output mask, in elements, with range [0, infinity)
		 * @param firstOutputRow The first output row to be handled
		 * @param numberOutputRows Number of output rows to be handled
		 * @tparam tChannels Number of frame channels
		 */
		template <unsigned int tChannels>
		static inline void homographiesMask8BitPerChannelSubset(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const SquareMatrix3* homographies, uint8_t* output, uint8_t* outputMask, const uint8_t maskValue, const Scalar outputQuadrantCenterX, const Scalar outputQuadrantCenterY, const int outputOriginX, const int outputOriginY, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, const unsigned int outputMaskPaddingElements, const unsigned int firstOutputRow, const unsigned int numberOutputRows);

		/**
		 * Transforms an 8 bit per channel frame using the given homography.
		 * @param inputCamera The pinhole camera profile to be applied for the input frame
		 * @param outputCamera The pinhole camera profile to be applied for the output frame
		 * @param outputCameraDistortionLookup The distortion lookup table of the of the output camera
		 * @param input The input frame that will be transformed
		 * @param normalizedHomography The homography used to transform the given input frame specified in normalized camera coordinates
		 * @param useDistortionParameters True, to apply the distortion parameters of the camera profile
		 * @param borderColor Color of undefined pixel positions, the size of the buffer must match to the number of channels, nullptr to assign 0x00 to each channel
		 * @param output The output frame resulting by application of the given homography
		 * @param inputPaddingElements The number of padding elements at the end of each input row, in elements, with range [0, infinity)
		 * @param outputPaddingElements The number of padding elements at the end of each output row, in elements, with range [0, infinity)
		 * @param firstRow The first row to be handled
		 * @param numberRows Number of rows to be handled
		 * @tparam tChannels Number of frame channels
		 */
		template <unsigned int tChannels>
		static void homographyWithCamera8BitPerChannelSubset(const PinholeCamera* inputCamera, const PinholeCamera* outputCamera, const PinholeCamera::DistortionLookup* outputCameraDistortionLookup, const uint8_t* input, const SquareMatrix3* normalizedHomography, const bool useDistortionParameters, const uint8_t* borderColor, uint8_t* output, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Transforms an 8 bit per channel frame using the given homography.
		 * @param inputCamera The pinhole camera profile to be applied for the input frame
		 * @param outputCamera The pinhole camera profile to be applied for the output frame
		 * @param outputCameraDistortionLookup The distortion lookup table of the of the output camera
		 * @param input The input frame that will be transformed, must be valid
		 * @param inputPaddingElements The number of padding elements at the end of each input row, in elements, with range [0, infinity)
		 * @param normalizedHomography The homography used to transform the given input frame specified in normalized camera coordinates
		 * @param output The output frame resulting by application of the given homography
		 * @param outputMask Mask frame with 8 bit per pixel defining whether an output frame pixel has a valid corresponding pixel in the input frame
		 * @param outputPaddingElements The number of padding elements at the end of each output row, in elements, with range [0, infinity)
		 * @param outputMaskPaddingElements The number of padding elements at the end of each output mask row, in elements, with range [0, infinity)
		 * @param maskValue 8 bit mask values for reference pixels lying inside the given camera frame, reference pixels lying outside the camera frame will be assigned with (0xFF - maskValue)
		 * @param firstRow The first row to be handled
		 * @param numberRows Number of rows to be handled
		 * @tparam tChannels Number of frame channels
		 */
		template <unsigned int tChannels>
		static void homographyWithCameraMask8BitPerChannelSubset(const PinholeCamera* inputCamera, const PinholeCamera* outputCamera, const PinholeCamera::DistortionLookup* outputCameraDistortionLookup, const uint8_t* input, const unsigned int inputPaddingElements, const SquareMatrix3* normalizedHomography, uint8_t* output, uint8_t* outputMask, const unsigned int outputPaddingElements, const unsigned int outputMaskPaddingElements, const uint8_t maskValue, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Transforms a subset of a given input frame with uint8_t as element type into an output frame by application of an interpolation lookup table.
		 * The output frame must have the same pixel format and pixel origin as the input frame.<br>
		 * @param input The input frame which will be transformed, must be valid
		 * @param inputWidth The width of the given input frame in pixel, with range [1, infinity)
		 * @param inputHeight The height of the given input frame in pixel, with range [1, infinity)
		 * @param input_LT_output The lookup table which defines the transformation from locations defined in the output frame to locations defined in the input frame (the lookup table stores the corresponding locations in the input frame), must be valid
		 * @param offset True, if the lookup table store local offsets; False, if the lookup table stores absolute positions
		 * @param borderColor Color of undefined pixel positions, the size of the buffer must match to the number of channels, nullptr to assign 0x00 to each channel
		 * @param output Resulting output frame with frame dimension equal to the size of the given lookup table
		 * @param inputPaddingElements Number of padding elements at the end of each input row, in elements, with range [0, infinity)
		 * @param outputPaddingElements Number of padding elements at the end of each output row, in elements, with range [0, infinity)
		 * @param firstRow First row to be handled, with range [0, input_LT_output->sizeY())
		 * @param numberRows Number of rows to be handled, with range [1, input_LT_output->sizeY() - firstRow]
		 * @tparam tChannels Number of channels of the frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void lookup8BitPerChannelSubset(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const LookupTable* input_LT_output, const bool offset, const uint8_t* borderColor, uint8_t* output, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Transforms a subset of a given input frame with arbitrary element type into an output frame by application of an interpolation lookup table.
		 * The output frame must have the same pixel format and pixel origin as the input frame.<br>
		 * @param input The input frame which will be transformed, must be valid
		 * @param inputWidth The width of the given input frame in pixel, with range [1, infinity)
		 * @param inputHeight The height of the given input frame in pixel, with range [1, infinity)
		 * @param input_LT_output The lookup table which defines the transformation from locations defined in the output frame to locations defined in the input frame (the lookup table stores the corresponding locations in the input frame), must be valid
		 * @param offset True, if the lookup table store local offsets; False, if the lookup table stores absolute positions
		 * @param borderColor Color of undefined pixel positions, the size of the buffer must match to the number of channels, nullptr to assign T(0) to each channel
		 * @param output Resulting output frame with frame dimension equal to the size of the given lookup table, must be valid
		 * @param inputPaddingElements Number of padding elements at the end of each input row, in elements, with range [0, infinity)
		 * @param outputPaddingElements Number of padding elements at the end of each output row, in elements, with range [0, infinity)
		 * @param firstRow First row to be handled, with range [0, input_LT_output->sizeY())
		 * @param numberRows Number of rows to be handled, with range [1, input_LT_output->sizeY() - firstRow]
		 * @tparam T Data type of each pixel channel, must not be 'uint8_t'
		 * @tparam tChannels Number of channels of the frame, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static void lookupSubset(const T* input, const unsigned int inputWidth, const unsigned int inputHeight, const LookupTable* input_LT_output, const bool offset, const T* borderColor, T* output, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, const unsigned int firstRow, const unsigned int numberRows);

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

		/**
		 * Transforms a subset of a given input frame into an output frame by application of an interpolation lookup table and uses NEON instructions.
		 * The output frame must have the same pixel format and pixel origin as the input frame.<br>
		 * @param input The input frame which will be transformed, must be valid
		 * @param inputWidth The width of the given input frame in pixel, with range [1, infinity)
		 * @param inputHeight The height of the given input frame in pixel, with range [1, infinity)
		 * @param input_LT_output The lookup table which defines the transformation from locations defined in the output frame to locations defined in the input frame (the lookup table stores the corresponding locations in the input frame), with table width >= 4, must be valid
		 * @param offset True, if the lookup table store local offsets; False, if the lookup table stores absolute positions
		 * @param borderColor Color of undefined pixel positions, the size of the buffer must match to the number of channels, nullptr to assign 0x00 to each channel
		 * @param output Resulting output frame with frame dimension equal to the size of the given lookup table
		 * @param inputPaddingElements Number of padding elements at the end of each input row, in elements, with range [0, infinity)
		 * @param outputPaddingElements Number of padding elements at the end of each output row, in elements, with range [0, infinity)
		 * @param firstRow First row to be handled, with range [0, input_LT_output->sizeY())
		 * @param numberRows Number of rows to be handled, with range [1, input_LT_output->sizeY() - firstRow]
		 * @tparam tChannels Number of channels of the frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void lookup8BitPerChannelSubsetNEON(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const LookupTable* input_LT_output, const bool offset, const uint8_t* borderColor, uint8_t* output, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, const unsigned int firstRow, const unsigned int numberRows);

#endif // defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

		/**
		 * Transforms a given input frame into an output frame by application of an interpolation lookup table.
		 * The output frame must have the same pixel format and pixel origin as the input frame.<br>
		 * Input frame pixels lying outside the frame will be masked in the resulting output mask frame, further theses pixels are untouched in the output frame.<br>
		 * @param input The input frame which will be transformed
		 * @param inputWidth The width of the given input frame in pixel, with range [1, infinity)
		 * @param inputHeight The height of the given input frame in pixel, with range [1, infinity)
		 * @param input_LT_output The lookup table which defines the transformation from locations defined in the output frame to locations defined in the input frame (the lookup table stores the corresponding locations in the input frame), must be valid
		 * @param offset True, if the lookup table store local offsets; False, if the lookup table stores absolute positions
		 * @param output Resulting output frame with frame dimension equal to the size of the given lookup table
		 * @param outputMask Resulting mask frame with 8 bit per pixel defining whether an output frame pixel has a valid corresponding pixel in the input frame
		 * @param maskValue 8 bit mask values for pixels lying inside the input frame, pixels lying outside the input frame will be assigned with (0xFF - maskValue)
		 * @param inputPaddingElements The number of padding elements at the end of each row of `input`, in elements, with range [0, infinity)
		 * @param outputPaddingElements The number of padding elements at the end of each row of `output`, in elements, with range [0, infinity)
		 * @param outputMaskPaddingElements The number of padding elements at the end of each row of `outputMask`, in elements, with range [0, infinity)
		 * @param firstRow First row to be handled
		 * @param numberRows Number of rows to be handled
		 * @tparam tChannels Number of channels of the frame
		 */
		template <unsigned int tChannels>
		static void lookupMask8BitPerChannelSubset(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const LookupTable* input_LT_output, const bool offset, uint8_t* output, uint8_t* outputMask, const uint8_t maskValue, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, const unsigned int outputMaskPaddingElements, const unsigned int firstRow, const unsigned int numberRows);
};

inline bool FrameInterpolatorBilinear::Comfort::resize(Frame& frame, const unsigned int width, const unsigned int height, Worker* worker)
{
	ocean_assert(frame.isValid());
	ocean_assert(width >= 1u && height >= 1u);

	Frame target(FrameType(frame, width, height));

	if (!resize(frame, target, worker))
	{
		return false;
	}

	frame = std::move(target);
	return true;
}

template <typename TScalar>
bool FrameInterpolatorBilinear::Comfort::interpolatePixel8BitPerChannel(const uint8_t* frame, const unsigned int channels, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const PixelCenter pixelCenter, const VectorT2<TScalar>& position, uint8_t* result)
{
	ocean_assert(frame != nullptr);
	ocean_assert(channels >= 1u && channels <= 8u);

	if (pixelCenter == PC_TOP_LEFT)
	{
		switch (channels)
		{
			case 1u:
				FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<1u, PC_TOP_LEFT, TScalar>(frame, width, height, framePaddingElements, position, result);
				return true;

			case 2u:
				FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<2u, PC_TOP_LEFT, TScalar>(frame, width, height, framePaddingElements, position, result);
				return true;

			case 3u:
				FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<3u, PC_TOP_LEFT, TScalar>(frame, width, height, framePaddingElements, position, result);
				return true;

			case 4u:
				FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<4u, PC_TOP_LEFT, TScalar>(frame, width, height, framePaddingElements, position, result);
				return true;

			case 5u:
				FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<5u, PC_TOP_LEFT, TScalar>(frame, width, height, framePaddingElements, position, result);
				return true;

			case 6u:
				FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<6u, PC_TOP_LEFT, TScalar>(frame, width, height, framePaddingElements, position, result);
				return true;

			case 7u:
				FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<7u, PC_TOP_LEFT, TScalar>(frame, width, height, framePaddingElements, position, result);
				return true;

			case 8u:
				FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<8u, PC_TOP_LEFT, TScalar>(frame, width, height, framePaddingElements, position, result);
				return true;
		}
	}
	else
	{
		ocean_assert(pixelCenter == PC_CENTER);

		switch (channels)
		{
			case 1u:
				FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<1u, PC_CENTER, TScalar>(frame, width, height, framePaddingElements, position, result);
				return true;

			case 2u:
				FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<2u, PC_CENTER, TScalar>(frame, width, height, framePaddingElements, position, result);
				return true;

			case 3u:
				FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<3u, PC_CENTER, TScalar>(frame, width, height, framePaddingElements, position, result);
				return true;

			case 4u:
				FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<4u, PC_CENTER, TScalar>(frame, width, height, framePaddingElements, position, result);
				return true;

			case 5u:
				FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<5u, PC_CENTER, TScalar>(frame, width, height, framePaddingElements, position, result);
				return true;

			case 6u:
				FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<6u, PC_CENTER, TScalar>(frame, width, height, framePaddingElements, position, result);
				return true;

			case 7u:
				FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<7u, PC_CENTER, TScalar>(frame, width, height, framePaddingElements, position, result);
				return true;

			case 8u:
				FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<8u, PC_CENTER, TScalar>(frame, width, height, framePaddingElements, position, result);
				return true;
		}
	}

	ocean_assert(false && "Invalid channel number");
	return false;
}

template <typename TSource, typename TTarget, typename TScalar, typename TIntermediate>
bool FrameInterpolatorBilinear::Comfort::interpolatePixel(const TSource* frame, const unsigned int channels, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const PixelCenter pixelCenter, const VectorT2<TScalar>& position, TTarget* result, const TIntermediate& resultBias)
{
	ocean_assert(frame != nullptr);
	ocean_assert(channels >= 1u && channels <= 8u);

	if (pixelCenter == PC_TOP_LEFT)
	{
		switch (channels)
		{
			case 1u:
				FrameInterpolatorBilinear::interpolatePixel<TSource, TTarget, 1u, PC_TOP_LEFT, TScalar, TIntermediate>(frame, width, height, framePaddingElements, position, result, resultBias);
				return true;

			case 2u:
				FrameInterpolatorBilinear::interpolatePixel<TSource, TTarget, 2u, PC_TOP_LEFT, TScalar, TIntermediate>(frame, width, height, framePaddingElements, position, result, resultBias);
				return true;

			case 3u:
				FrameInterpolatorBilinear::interpolatePixel<TSource, TTarget, 3u, PC_TOP_LEFT, TScalar, TIntermediate>(frame, width, height, framePaddingElements, position, result, resultBias);
				return true;

			case 4u:
				FrameInterpolatorBilinear::interpolatePixel<TSource, TTarget, 4u, PC_TOP_LEFT, TScalar, TIntermediate>(frame, width, height, framePaddingElements, position, result, resultBias);
				return true;

			case 5u:
				FrameInterpolatorBilinear::interpolatePixel<TSource, TTarget, 5u, PC_TOP_LEFT, TScalar, TIntermediate>(frame, width, height, framePaddingElements, position, result, resultBias);
				return true;

			case 6u:
				FrameInterpolatorBilinear::interpolatePixel<TSource, TTarget, 6u, PC_TOP_LEFT, TScalar, TIntermediate>(frame, width, height, framePaddingElements, position, result, resultBias);
				return true;

			case 7u:
				FrameInterpolatorBilinear::interpolatePixel<TSource, TTarget, 7u, PC_TOP_LEFT, TScalar, TIntermediate>(frame, width, height, framePaddingElements, position, result, resultBias);
				return true;

			case 8u:
				FrameInterpolatorBilinear::interpolatePixel<TSource, TTarget, 8u, PC_TOP_LEFT, TScalar, TIntermediate>(frame, width, height, framePaddingElements, position, result, resultBias);
				return true;
		}
	}
	else
	{
		ocean_assert(pixelCenter == PC_CENTER);

		switch (channels)
		{
			case 1u:
				FrameInterpolatorBilinear::interpolatePixel<TSource, TTarget, 1u, PC_CENTER, TScalar, TIntermediate>(frame, width, height, framePaddingElements, position, result, resultBias);
				return true;

			case 2u:
				FrameInterpolatorBilinear::interpolatePixel<TSource, TTarget, 2u, PC_CENTER, TScalar, TIntermediate>(frame, width, height, framePaddingElements, position, result, resultBias);
				return true;

			case 3u:
				FrameInterpolatorBilinear::interpolatePixel<TSource, TTarget, 3u, PC_CENTER, TScalar, TIntermediate>(frame, width, height, framePaddingElements, position, result, resultBias);
				return true;

			case 4u:
				FrameInterpolatorBilinear::interpolatePixel<TSource, TTarget, 4u, PC_CENTER, TScalar, TIntermediate>(frame, width, height, framePaddingElements, position, result, resultBias);
				return true;

			case 5u:
				FrameInterpolatorBilinear::interpolatePixel<TSource, TTarget, 5u, PC_CENTER, TScalar, TIntermediate>(frame, width, height, framePaddingElements, position, result, resultBias);
				return true;

			case 6u:
				FrameInterpolatorBilinear::interpolatePixel<TSource, TTarget, 6u, PC_CENTER, TScalar, TIntermediate>(frame, width, height, framePaddingElements, position, result, resultBias);
				return true;

			case 7u:
				FrameInterpolatorBilinear::interpolatePixel<TSource, TTarget, 7u, PC_CENTER, TScalar, TIntermediate>(frame, width, height, framePaddingElements, position, result, resultBias);
				return true;

			case 8u:
				FrameInterpolatorBilinear::interpolatePixel<TSource, TTarget, 8u, PC_CENTER, TScalar, TIntermediate>(frame, width, height, framePaddingElements, position, result, resultBias);
				return true;
		}
	}

	ocean_assert(false && "Invalid channel number");
	return false;
}

template <typename T, unsigned int tChannels>
inline void FrameInterpolatorBilinear::resize(const T* source, T* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(sourceWidth >= 1u && sourceHeight >= 1u);
	ocean_assert(targetWidth >= 1u && targetHeight >= 1u);

	const double sourceX_s_targetX = double(sourceWidth) / double(targetWidth);
	const double sourceY_s_targetY = double(sourceHeight) / double(targetHeight);

	scale<T, tChannels>(source, target, sourceWidth, sourceHeight, targetWidth, targetHeight, sourceX_s_targetX, sourceY_s_targetY, sourcePaddingElements, targetPaddingElements, worker);
}

template <typename T, unsigned int tChannels>
inline void FrameInterpolatorBilinear::scale(const T* source, T* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const double sourceX_s_targetX, const double sourceY_s_targetY, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(sourceWidth >= 1u && sourceHeight >= 1u);
	ocean_assert(targetWidth >= 1u && targetHeight >= 1u);
	ocean_assert(sourceX_s_targetX > 0.0);
	ocean_assert(sourceY_s_targetY > 0.0);

	if (sourceWidth == targetWidth && sourceHeight == targetHeight)
	{
		FrameConverter::subFrame<T>(source, target, sourceWidth, sourceHeight, targetWidth, targetHeight, tChannels, 0u, 0u, 0u, 0u, sourceWidth, sourceHeight, sourcePaddingElements, targetPaddingElements);
		return;
	}

	if (std::is_same<T, uint8_t>::value)
	{
		// we have a SIMD-based optimized version for 'uint8_t' data types

		scale8BitPerChannel<tChannels>((const uint8_t*)source, (uint8_t*)target, sourceWidth, sourceHeight, targetWidth, targetHeight, sourceX_s_targetX, sourceY_s_targetY, sourcePaddingElements, targetPaddingElements, worker);
	}
	else
	{
		typedef typename FloatTyper<T>::Type TScale;

		if (worker)
		{
			worker->executeFunction(Worker::Function::createStatic(&scaleSubset<T, TScale, tChannels>, source, target, sourceWidth, sourceHeight, targetWidth, targetHeight, sourceX_s_targetX, sourceY_s_targetY, sourcePaddingElements, targetPaddingElements, 0u, 0u), 0u, targetHeight);
		}
		else
		{
			scaleSubset<T, TScale, tChannels>(source, target, sourceWidth, sourceHeight, targetWidth, targetHeight, sourceX_s_targetX, sourceY_s_targetY, sourcePaddingElements, targetPaddingElements, 0u, targetHeight);
		}
	}
}

template <unsigned int tChannels>
inline void FrameInterpolatorBilinear::affine8BitPerChannel(const uint8_t* source, const unsigned int sourceWidth, const unsigned int sourceHeight, const SquareMatrix3& source_A_target, const uint8_t* borderColor, uint8_t* target, const CV::PixelPositionI& targetOrigin, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	// If applicable, apply an additional translation to the affine transformation.
	const SquareMatrix3 adjustedAffineTransform = source_A_target * SquareMatrix3(Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(Scalar(targetOrigin.x()), Scalar(targetOrigin.y()), 1));

	if (worker)
	{
		if (targetWidth >= 4u)
		{
#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41
			worker->executeFunction(Worker::Function::createStatic(&FrameInterpolatorBilinear::affine8BitPerChannelSSESubset<tChannels>, source, sourceWidth, sourceHeight, &adjustedAffineTransform, borderColor, target, targetWidth, targetHeight, 0u, 0u, sourcePaddingElements, targetPaddingElements), 0, targetHeight, 8u, 9u, 20u);
			return;
#elif defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
			worker->executeFunction(Worker::Function::createStatic(&FrameInterpolatorBilinear::affine8BitPerChannelNEONSubset<tChannels>, source, sourceWidth, sourceHeight, &adjustedAffineTransform, borderColor, target, targetWidth, targetHeight, 0u, 0u, sourcePaddingElements, targetPaddingElements), 0, targetHeight, 8u, 9u, 20u);
			return;
#endif
		}

		worker->executeFunction(Worker::Function::createStatic(&FrameInterpolatorBilinear::affine8BitPerChannelSubset<tChannels>, source, sourceWidth, sourceHeight, &adjustedAffineTransform, borderColor, target, targetWidth, targetHeight, 0u, 0u, sourcePaddingElements, targetPaddingElements), 0, targetHeight, 8u, 9u, 20u);
	}
	else
	{
		if (targetWidth >= 4u)
		{
#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41
			affine8BitPerChannelSSESubset<tChannels>(source, sourceWidth, sourceHeight, &adjustedAffineTransform, borderColor, target, targetWidth, targetHeight, 0u, targetHeight, sourcePaddingElements, targetPaddingElements);
			return;
#elif defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
			affine8BitPerChannelNEONSubset<tChannels>(source, sourceWidth, sourceHeight, &adjustedAffineTransform, borderColor, target, targetWidth, targetHeight, 0u, targetHeight, sourcePaddingElements, targetPaddingElements);
			return;
#endif
		}

		affine8BitPerChannelSubset<tChannels>(source, sourceWidth, sourceHeight, &adjustedAffineTransform, borderColor, target, targetWidth, targetHeight, 0u, targetHeight, sourcePaddingElements, targetPaddingElements);
	}
}

template <unsigned int tChannels>
inline void FrameInterpolatorBilinear::homography8BitPerChannel(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const SquareMatrix3& input_H_output, const uint8_t* borderColor, uint8_t* output, const CV::PixelPositionI& outputOrigin, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, Worker* worker)
{
	// we adjust the homography to address 'outputOrigin'
	const SquareMatrix3 input_H_shiftedOutput = input_H_output * SquareMatrix3(Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(Scalar(outputOrigin.x()), Scalar(outputOrigin.y()), 1));

	if (worker)
	{
		if (outputWidth >= 4u)
		{
#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41
			worker->executeFunction(Worker::Function::createStatic(&FrameInterpolatorBilinear::homography8BitPerChannelSSESubset<tChannels>, input, inputWidth, inputHeight, &input_H_shiftedOutput, borderColor, output, outputWidth, outputHeight, inputPaddingElements, outputPaddingElements, 0u, 0u), 0, outputHeight, 10u, 11u, 20u);
			return;
#elif defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
			worker->executeFunction(Worker::Function::createStatic(&FrameInterpolatorBilinear::homography8BitPerChannelNEONSubset<tChannels>, input, inputWidth, inputHeight, &input_H_shiftedOutput, borderColor, output, outputWidth, outputHeight, inputPaddingElements, outputPaddingElements, 0u, 0u), 0, outputHeight, 10u, 11u, 20u);
			return;
#endif
		}

		worker->executeFunction(Worker::Function::createStatic(&FrameInterpolatorBilinear::homography8BitPerChannelSubset<tChannels>, input, inputWidth, inputHeight, &input_H_shiftedOutput, borderColor, output, outputWidth, outputHeight, inputPaddingElements, outputPaddingElements, 0u, 0u), 0, outputHeight, 10u, 11u, 20u);
	}
	else
	{
		if (outputWidth >= 4u)
		{
#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41
			homography8BitPerChannelSSESubset<tChannels>(input, inputWidth, inputHeight, &input_H_shiftedOutput, borderColor, output, outputWidth, outputHeight, inputPaddingElements, outputPaddingElements, 0u, outputHeight);
			return;
#elif defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
			homography8BitPerChannelNEONSubset<tChannels>(input, inputWidth, inputHeight, &input_H_shiftedOutput, borderColor, output, outputWidth, outputHeight, inputPaddingElements, outputPaddingElements, 0u, outputHeight);
			return;
#endif
		}

		homography8BitPerChannelSubset<tChannels>(input, inputWidth, inputHeight, &input_H_shiftedOutput, borderColor, output, outputWidth, outputHeight, inputPaddingElements, outputPaddingElements, 0u, outputHeight);
	}
}

template <typename T, unsigned int tChannels>
inline void FrameInterpolatorBilinear::homography(const T* input, const unsigned int inputWidth, const unsigned int inputHeight, const SquareMatrix3& input_H_output, const T* borderColor, T* output, const CV::PixelPositionI& outputOrigin, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, Worker* worker)
{
	if (std::is_same<T, uint8_t>::value)
	{
		homography8BitPerChannel<tChannels>((const uint8_t*)input, inputWidth, inputHeight, input_H_output, (const uint8_t*)borderColor, (uint8_t*)output, outputOrigin, outputWidth, outputHeight, inputPaddingElements, outputPaddingElements, worker);
		return;
	}
	else
	{
		// we adjust the homography to address 'outputOrigin'
		const SquareMatrix3 input_H_shiftedOutput = input_H_output * SquareMatrix3(Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(Scalar(outputOrigin.x()), Scalar(outputOrigin.y()), 1));

		if (worker)
		{
			worker->executeFunction(Worker::Function::createStatic(&FrameInterpolatorBilinear::homographySubset<T, tChannels>, input, inputWidth, inputHeight, &input_H_shiftedOutput, borderColor, output, outputWidth, outputHeight, inputPaddingElements, outputPaddingElements, 0u, 0u), 0, outputHeight, 10u, 11u, 20u);
		}
		else
		{
			homographySubset<T, tChannels>(input, inputWidth, inputHeight, &input_H_shiftedOutput, borderColor, output, outputWidth, outputHeight, inputPaddingElements, outputPaddingElements, 0u, outputHeight);
		}
	}
}

template <unsigned int tChannels>
inline void FrameInterpolatorBilinear::homographies8BitPerChannel(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const SquareMatrix3 homographies[4], const uint8_t* borderColor, uint8_t* output, const Vector2& outputQuadrantCenter, const PixelPositionI& outputOrigin, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, Worker* worker)
{
	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameInterpolatorBilinear::homographies8BitPerChannelSubset<tChannels>, input, inputWidth, inputHeight, homographies, borderColor, output, outputQuadrantCenter.x(), outputQuadrantCenter.y(), outputOrigin.x(), outputOrigin.y(), outputWidth, outputHeight, inputPaddingElements, outputPaddingElements, 0u, 0u), 0, outputHeight, 14u, 15u, 20u);
	}
	else
	{
		homographies8BitPerChannelSubset<tChannels>(input, inputWidth, inputHeight, homographies, borderColor, output, outputQuadrantCenter.x(), outputQuadrantCenter.y(), outputOrigin.x(), outputOrigin.y(), outputWidth, outputHeight, inputPaddingElements, outputPaddingElements, 0u, outputHeight);
	}
}

template <unsigned int tChannels>
inline void FrameInterpolatorBilinear::homographyMask8BitPerChannel(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const SquareMatrix3& input_H_output, uint8_t* output, uint8_t* outputMask, const CV::PixelPositionI& outputOrigin, const unsigned int outputWidth, const unsigned int outputHeight, const uint8_t maskValue, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, const unsigned int outputMaskPaddingElements, Worker* worker)
{
	// we adjust the homography to address 'outputOrigin'
	const SquareMatrix3 input_H_shiftedOutput = input_H_output * SquareMatrix3(Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(Scalar(outputOrigin.x()), Scalar(outputOrigin.y()), 1));

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameInterpolatorBilinear::homographyMask8BitPerChannelSubset<tChannels>, input, inputWidth, inputHeight, &input_H_shiftedOutput, output, outputMask, maskValue, outputWidth, outputHeight, inputPaddingElements, outputPaddingElements, outputMaskPaddingElements, 0u, 0u), 0, outputHeight, 12u, 13u, 20u);
	}
	else
	{
		homographyMask8BitPerChannelSubset<tChannels>(input, inputWidth, inputHeight, &input_H_shiftedOutput, output, outputMask, maskValue, outputWidth, outputHeight, inputPaddingElements, outputPaddingElements, outputMaskPaddingElements, 0u, outputHeight);
	}
}

template <unsigned int tChannels>
inline void FrameInterpolatorBilinear::homographiesMask8BitPerChannel(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const SquareMatrix3 homographies[4], uint8_t* output, uint8_t* outputMask, const Vector2& outputQuadrantCenter, const CV::PixelPositionI& outputOrigin, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, const unsigned int outputMaskPaddingElements, Worker* worker, const uint8_t maskValue)
{
	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameInterpolatorBilinear::homographiesMask8BitPerChannelSubset<tChannels>, input, inputWidth, inputHeight, homographies, output, outputMask, maskValue, outputQuadrantCenter.x(), outputQuadrantCenter.y(), outputOrigin.x(), outputOrigin.y(), outputWidth, outputHeight, inputPaddingElements, outputPaddingElements, outputMaskPaddingElements, 0u, 0u), 0, outputHeight);
	}
	else
	{
		homographiesMask8BitPerChannelSubset<tChannels>(input, inputWidth, inputHeight, homographies, output, outputMask, maskValue, outputQuadrantCenter.x(), outputQuadrantCenter.y(), outputOrigin.x(), outputOrigin.y(), outputWidth, outputHeight, inputPaddingElements, outputPaddingElements, outputMaskPaddingElements, 0u, outputHeight);
	}
}

template <unsigned int tChannels>
inline void FrameInterpolatorBilinear::homographyWithCamera8BitPerChannel(const PinholeCamera& inputCamera, const PinholeCamera& outputCamera, const uint8_t* input, const SquareMatrix3& homography, const bool useDistortionParameters, const uint8_t* borderColor, uint8_t* output, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, Worker* worker)
{
	const SquareMatrix3 normalizedHomography(inputCamera.invertedIntrinsic() * homography * outputCamera.intrinsic());

	const PinholeCamera::DistortionLookup outputCameraDistortionLookup(outputCamera, 10u);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameInterpolatorBilinear::homographyWithCamera8BitPerChannelSubset<tChannels>, &inputCamera, &outputCamera, &outputCameraDistortionLookup, input, &normalizedHomography, useDistortionParameters, borderColor, output, inputPaddingElements, outputPaddingElements, 0u, 0u), 0, outputCamera.height());
	}
	else
	{
		homographyWithCamera8BitPerChannelSubset<tChannels>(&inputCamera, &outputCamera, &outputCameraDistortionLookup, input, &normalizedHomography, useDistortionParameters, borderColor, output, inputPaddingElements, outputPaddingElements, 0u, outputCamera.height());
	}
}

template <unsigned int tChannels>
inline void FrameInterpolatorBilinear::homographyWithCameraMask8BitPerChannel(const PinholeCamera& inputCamera, const PinholeCamera& outputCamera, const uint8_t* input, const unsigned int inputPaddingElements, const SquareMatrix3& homography, uint8_t* output, uint8_t* outputMask, const unsigned int outputPaddingElements, const unsigned int outputMaskPaddingElements, Worker* worker, const uint8_t maskValue)
{
	const SquareMatrix3 normalizedHomography(inputCamera.invertedIntrinsic() * homography * outputCamera.intrinsic());

	const PinholeCamera::DistortionLookup outputCameraDistortionLookup(outputCamera, 10u);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameInterpolatorBilinear::homographyWithCameraMask8BitPerChannelSubset<tChannels>, &inputCamera, &outputCamera, &outputCameraDistortionLookup, input, inputPaddingElements, &normalizedHomography, output, outputMask, outputPaddingElements, outputMaskPaddingElements, maskValue, 0u, 0u), 0, outputCamera.height(), 11u, 12u, 10u);
	}
	else
	{
		homographyWithCameraMask8BitPerChannelSubset<tChannels>(&inputCamera, &outputCamera, &outputCameraDistortionLookup, input, inputPaddingElements, &normalizedHomography, output, outputMask, outputPaddingElements, outputMaskPaddingElements, maskValue, 0u, outputCamera.height());
	}
}

template <typename T, unsigned int tChannels>
inline void FrameInterpolatorBilinear::lookup(const T* input, const unsigned int inputWidth, const unsigned int inputHeight, const LookupTable& input_LT_output, const bool offset, const T* borderColor, T* output, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, Worker* worker)
{
	if constexpr (std::is_same<T, uint8_t>::value)
	{
#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
		if ((tChannels >= 1u && input_LT_output.sizeX() >= 8) || (tChannels >= 2u && input_LT_output.sizeX() >= 4))
		{
			// NEON implementation for 1 channel: min width 8; for 2+ channels: min width 4

			if (worker)
			{
				worker->executeFunction(Worker::Function::createStatic(&FrameInterpolatorBilinear::lookup8BitPerChannelSubsetNEON<tChannels>, input, inputWidth, inputHeight, &input_LT_output, offset, borderColor, output, inputPaddingElements, outputPaddingElements, 0u, 0u), 0u, (unsigned int)(input_LT_output.sizeY()), 9u, 10u, 20u);
			}
			else
			{
				lookup8BitPerChannelSubsetNEON<tChannels>(input, inputWidth, inputHeight, &input_LT_output, offset, borderColor, output, inputPaddingElements, outputPaddingElements, 0u, (unsigned int)(input_LT_output.sizeY()));
			}

			return;
		}
#endif // defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

		if (worker)
		{
			worker->executeFunction(Worker::Function::createStatic(&FrameInterpolatorBilinear::lookup8BitPerChannelSubset<tChannels>, input, inputWidth, inputHeight, &input_LT_output, offset, borderColor, output, inputPaddingElements, outputPaddingElements, 0u, 0u), 0u, (unsigned int)input_LT_output.sizeY(), 9u, 10u, 20u);
		}
		else
		{
			lookup8BitPerChannelSubset<tChannels>(input, inputWidth, inputHeight, &input_LT_output, offset, borderColor, output, inputPaddingElements, outputPaddingElements, 0u, (unsigned int)(input_LT_output.sizeY()));
		}
	}
	else
	{
		ocean_assert((!std::is_same<T, uint8_t>::value));

		if (worker)
		{
			worker->executeFunction(Worker::Function::createStatic(&FrameInterpolatorBilinear::lookupSubset<T, tChannels>, input, inputWidth, inputHeight, &input_LT_output, offset, borderColor, output, inputPaddingElements, outputPaddingElements, 0u, 0u), 0u, (unsigned int)(input_LT_output.sizeY()), 9u, 10u, 20u);
		}
		else
		{
			lookupSubset<T, tChannels>(input, inputWidth, inputHeight, &input_LT_output, offset, borderColor, output, inputPaddingElements, outputPaddingElements, 0u, (unsigned int)(input_LT_output.sizeY()));
		}
	}
}

template <unsigned int tChannels>
inline void FrameInterpolatorBilinear::lookupMask8BitPerChannel(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const LookupTable& input_LT_output, const bool offset, uint8_t* output, uint8_t* outputMask, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, const unsigned int outputMaskPaddingElements, Worker* worker, const uint8_t maskValue)
{
	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameInterpolatorBilinear::lookupMask8BitPerChannelSubset<tChannels>, input, inputWidth, inputHeight, &input_LT_output, offset, output, outputMask, maskValue, inputPaddingElements, outputPaddingElements, outputMaskPaddingElements, 0u, 0u), 0u, (unsigned int)(input_LT_output.sizeY()), 11u, 12u, 20u);
	}
	else
	{
		lookupMask8BitPerChannelSubset<tChannels>(input, inputWidth, inputHeight, &input_LT_output, offset, output, outputMask, maskValue, inputPaddingElements, outputPaddingElements, outputMaskPaddingElements, 0u, (unsigned int)(input_LT_output.sizeY()));
	}
}

template <typename T, unsigned int tChannels>
void FrameInterpolatorBilinear::resampleCameraImage(const T* sourceFrame, const AnyCamera& sourceCamera, const SquareMatrix3& source_R_target, const AnyCamera& targetCamera, T* targetFrame, const unsigned int sourceFramePaddingElements, const unsigned int targetFramePaddingElements, LookupCorner2<Vector2>* source_OLT_target, Worker* worker, const unsigned int binSizeInPixel, const T* borderColor)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(sourceFrame != nullptr);
	ocean_assert(sourceCamera.isValid());
	ocean_assert(source_R_target.isOrthonormal());
	ocean_assert(targetCamera.isValid());
	ocean_assert(targetFrame != nullptr);
	ocean_assert(binSizeInPixel >= 1u);

	const size_t binsX = std::max(1u, targetCamera.width() / binSizeInPixel);
	const size_t binsY = std::max(1u, targetCamera.height() / binSizeInPixel);
	CV::FrameInterpolatorBilinear::LookupTable lookupTable(targetCamera.width(), targetCamera.height(), binsX, binsY);

	for (size_t yBin = 0; yBin <= lookupTable.binsY(); ++yBin)
	{
		for (size_t xBin = 0; xBin <= lookupTable.binsX(); ++xBin)
		{
			const Vector2 cornerPosition = lookupTable.binTopLeftCornerPosition(xBin, yBin);

			constexpr bool makeUnitVector = false; // we don't need a unit/normalized vector as we project the vector into the camera again

			const Vector3 rayI = source_R_target * targetCamera.vector(cornerPosition, makeUnitVector);
			const Vector3 rayIF = Vector3(rayI.x(), -rayI.y(), -rayI.z());

			if (rayIF.z() > Numeric::eps())
			{
				const Vector2 projectedPoint = sourceCamera.projectToImageIF(rayIF);

				lookupTable.setBinTopLeftCornerValue(xBin, yBin, projectedPoint - cornerPosition);
			}
			else
			{
				// simply a coordinate far outside the input
				lookupTable.setBinTopLeftCornerValue(xBin, yBin, Vector2(Scalar(sourceCamera.width() * 10u), Scalar(sourceCamera.height() * 10u)));
			}
		}
	}

	lookup<T, tChannels>(sourceFrame, sourceCamera.width(), sourceCamera.height(), lookupTable, true /*offset*/, borderColor, targetFrame, sourceFramePaddingElements, targetFramePaddingElements, worker);

	if (source_OLT_target)
	{
		*source_OLT_target = std::move(lookupTable);
	}
}

template <unsigned int tChannels>
void FrameInterpolatorBilinear::rotate8BitPerChannel(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const Scalar horizontalAnchorPosition, const Scalar verticalAnchorPosition, const Scalar angle, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker, const uint8_t* borderColor)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&rotate8BitPerChannelSubset<tChannels>, source, target, width, height, horizontalAnchorPosition, verticalAnchorPosition, angle, borderColor, sourcePaddingElements, targetPaddingElements, 0u, 0u), 0u, height);
	}
	else
	{
		rotate8BitPerChannelSubset<tChannels>(source, target, width, height, horizontalAnchorPosition, verticalAnchorPosition, angle, borderColor, sourcePaddingElements, targetPaddingElements, 0u, height);
	}
}

template <unsigned int tChannels, PixelCenter tPixelCenter, typename TScalar>
inline void FrameInterpolatorBilinear::interpolatePixel8BitPerChannel(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const VectorT2<TScalar>& position, uint8_t* result)
{
	static_assert(tChannels != 0u, "Invalid channel number!");
	static_assert(tPixelCenter == PC_TOP_LEFT || tPixelCenter == PC_CENTER, "Invalid pixel center!");

	ocean_assert(frame != nullptr && result != nullptr);
	ocean_assert(width != 0u && height != 0u);

	const unsigned int frameStrideElements = width * tChannels + framePaddingElements;

	ocean_assert(position.x() >= TScalar(0));
	ocean_assert(position.y() >= TScalar(0));

	if constexpr (tPixelCenter == PC_TOP_LEFT)
	{
		ocean_assert(position.x() <= TScalar(width - 1u));
		ocean_assert(position.y() <= TScalar(height - 1u));

		const unsigned int left = (unsigned int)(position.x());
		const unsigned int top = (unsigned int)(position.y());
		ocean_assert(left < width && top < height);

		const TScalar tx = position.x() - TScalar(left);
		ocean_assert(tx >= 0 && tx <= 1);
		const unsigned int txi = (unsigned int)(tx * TScalar(128) + TScalar(0.5));
		const unsigned int txi_ = 128u - txi;

		const TScalar ty = position.y() - TScalar(top);
		ocean_assert(ty >= 0 && ty <= 1);
		const unsigned int tyi = (unsigned int)(ty * TScalar(128) + TScalar(0.5));
		const unsigned int tyi_ = 128u - tyi;

		const unsigned int rightOffset = left + 1u < width ? tChannels : 0u;
		const unsigned int bottomOffset = top + 1u < height ? frameStrideElements : 0u;

		const uint8_t* const topLeft = frame + top * frameStrideElements + tChannels * left;

		const unsigned int txty = txi * tyi;
		const unsigned int txty_ = txi * tyi_;
		const unsigned int tx_ty = txi_ * tyi;
		const unsigned int tx_ty_ = txi_ * tyi_;

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			result[n] = uint8_t((topLeft[n] * tx_ty_ + topLeft[rightOffset + n] * txty_	+ topLeft[bottomOffset + n] * tx_ty + topLeft[bottomOffset + rightOffset + n] * txty + 8192u) >> 14u);
		}
	}
	else
	{
		ocean_assert(tPixelCenter == PC_CENTER);

		ocean_assert(position.x() <= TScalar(width));
		ocean_assert(position.y() <= TScalar(height));

		const TScalar xShifted = std::max(TScalar(0.0), position.x() - TScalar(0.5));
		const TScalar yShifted = std::max(TScalar(0.0), position.y() - TScalar(0.5));

		const unsigned int left = (unsigned int)(xShifted);
		const unsigned int top = (unsigned int)(yShifted);

		ocean_assert(left < width);
		ocean_assert(top < height);

		const TScalar tx = xShifted - TScalar(left);
		const TScalar ty = yShifted - TScalar(top);

		ocean_assert(tx >= 0 && tx <= 1);
		ocean_assert(ty >= 0 && ty <= 1);

		const unsigned int txi = (unsigned int)(tx * TScalar(128) + TScalar(0.5));
		const unsigned int txi_ = 128u - txi;

		const unsigned int tyi = (unsigned int)(ty * TScalar(128) + TScalar(0.5));
		const unsigned int tyi_ = 128u - tyi;

		const unsigned int rightOffset = left + 1u < width ? tChannels : 0u;
		const unsigned int bottomOffset = top + 1u < height ? frameStrideElements : 0u;

		const uint8_t* const topLeft = frame + top * frameStrideElements + left * tChannels;

		const unsigned int txty = txi * tyi;
		const unsigned int txty_ = txi * tyi_;
		const unsigned int tx_ty = txi_ * tyi;
		const unsigned int tx_ty_ = txi_ * tyi_;

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			result[n] = uint8_t((topLeft[n] * tx_ty_ + topLeft[rightOffset + n] * txty_	+ topLeft[bottomOffset + n] * tx_ty + topLeft[bottomOffset + rightOffset + n] * txty + 8192u) >> 14u);
		}
	}
}

template <typename TSource, typename TTarget, unsigned int tChannels, PixelCenter tPixelCenter, typename TScalar, typename TIntermediate>
inline void FrameInterpolatorBilinear::interpolatePixel(const TSource* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const VectorT2<TScalar>& position, TTarget* result, const TIntermediate& resultBias)
{
	static_assert(tChannels != 0u, "Invalid channel number!");
	static_assert(tPixelCenter == PC_TOP_LEFT || tPixelCenter == PC_CENTER, "Invalid pixel center!");

	ocean_assert(frame != nullptr && result != nullptr);
	ocean_assert(width != 0u && height != 0u);

	const unsigned int frameStrideElements = width * tChannels + framePaddingElements;

	ocean_assert(position.x() >= TScalar(0));
	ocean_assert(position.y() >= TScalar(0));

	if constexpr (tPixelCenter == PC_TOP_LEFT)
	{
		ocean_assert(position.x() <= TScalar(width - 1u));
		ocean_assert(position.y() <= TScalar(height - 1u));

		const unsigned int left = (unsigned int)(position.x());
		const unsigned int top = (unsigned int)(position.y());

		const TScalar tx = position.x() - TScalar(left);
		ocean_assert(tx >= 0 && tx <= 1);

		const TScalar ty = position.y() - TScalar(top);
		ocean_assert(ty >= 0 && ty <= 1);

		const unsigned int rightOffset = left + 1u < width ? tChannels : 0u;
		const unsigned int bottomOffset = top + 1u < height ? frameStrideElements : 0u;

		const TSource* const topLeft = frame + top * frameStrideElements + tChannels * left;

		const TIntermediate txty = TIntermediate(tx) * TIntermediate(ty);
		const TIntermediate txty_ = TIntermediate(tx) * (TIntermediate(1) - TIntermediate(ty));
		const TIntermediate tx_ty = (TIntermediate(1) - TIntermediate(tx)) * TIntermediate(ty);
		const TIntermediate tx_ty_ = (TIntermediate(1) - TIntermediate(tx)) * (TIntermediate(1) - TIntermediate(ty));

		ocean_assert_accuracy(NumericT<TIntermediate>::isEqual(txty + txty_ + tx_ty + tx_ty_, TIntermediate(1)));

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			result[n] = TTarget(TIntermediate(topLeft[n]) * tx_ty_ + TIntermediate(topLeft[rightOffset + n]) * txty_ + TIntermediate(topLeft[bottomOffset + n]) * tx_ty + TIntermediate(topLeft[bottomOffset + rightOffset + n]) * txty + resultBias);
		}
	}
	else
	{
		ocean_assert(tPixelCenter == PC_CENTER);

		ocean_assert(position.x() <= TScalar(width));
		ocean_assert(position.y() <= TScalar(height));

		const TScalar xShifted = std::max(TScalar(0.0), position.x() - TScalar(0.5));
		const TScalar yShifted = std::max(TScalar(0.0), position.y() - TScalar(0.5));

		const unsigned int left = (unsigned int)(xShifted);
		const unsigned int top = (unsigned int)(yShifted);

		ocean_assert(left < width);
		ocean_assert(top < height);

		const TScalar tx = xShifted - TScalar(left);
		const TScalar ty = yShifted - TScalar(top);

		ocean_assert(tx >= 0 && tx <= 1);
		ocean_assert(ty >= 0 && ty <= 1);

		const unsigned int rightOffset = left + 1u < width ? tChannels : 0u;
		const unsigned int bottomOffset = top + 1u < height ? frameStrideElements : 0u;

		const TSource* const topLeft = frame + top * frameStrideElements + tChannels * left;

		const TIntermediate txty = TIntermediate(tx) * TIntermediate(ty);
		const TIntermediate txty_ = TIntermediate(tx) * (TIntermediate(1) - TIntermediate(ty));
		const TIntermediate tx_ty = (TIntermediate(1) - TIntermediate(tx)) * TIntermediate(ty);
		const TIntermediate tx_ty_ = (TIntermediate(1) - TIntermediate(tx)) * (TIntermediate(1) - TIntermediate(ty));

		ocean_assert_accuracy(NumericT<TIntermediate>::isEqual(txty + txty_ + tx_ty + tx_ty_, TIntermediate(1)));

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			result[n] = TTarget(TIntermediate(topLeft[n]) * tx_ty_ + TIntermediate(topLeft[rightOffset + n]) * txty_ + TIntermediate(topLeft[bottomOffset + n]) * tx_ty + TIntermediate(topLeft[bottomOffset + rightOffset + n]) * txty + resultBias);
		}
	}
}

template <unsigned int tChannels, bool tAlphaAtFront, bool tTransparentIs0xFF>
inline void FrameInterpolatorBilinear::interpolate1PixelFullAlphaBorder8BitPerChannel(const uint8_t* frame, const unsigned int width, const unsigned int height, const Vector2& position, uint8_t* result, const unsigned int framePaddingElements)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(frame && result);

	const Vector2 pos(position.x() - Scalar(0.5), position.y() - Scalar(0.5));

	// check whether the position is outside the frame and will therefore be 100% transparent
	if (pos.x() <= Scalar(-1) || pos.y() <= Scalar(-1) || pos.x() >= Scalar(width) || pos.y() >= Scalar(height))
	{
		for (unsigned int n = 0u; n < tChannels - 1u; ++n)
		{
			result[n + FrameBlender::SourceOffset<tAlphaAtFront>::data()] = 0x00;
		}

		result[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()] = FrameBlender::fullTransparent8Bit<tTransparentIs0xFF>();

		return;
	}

	const unsigned int frameStrideElements = width * tChannels + framePaddingElements;

	const int left = int(Numeric::floor(pos.x()));
	const int top = int(Numeric::floor(pos.y()));

	ocean_assert(left >= -1 && left < int(width));
	ocean_assert(top >= -1 && top < int(height));

	if ((unsigned int)left < width - 1u && (unsigned int)top < height - 1u)
	{
		// we have a valid pixel position for the left, top, right and bottom pixel

		const unsigned int txi = (unsigned int)((pos.x() - Scalar(left)) * Scalar(128) + Scalar(0.5));
		const unsigned int txi_ = 128u - txi;

		const unsigned int tyi = (unsigned int)((pos.y() - Scalar(top)) * Scalar(128) + Scalar(0.5));
		const unsigned int tyi_ = 128u - tyi;

		const uint8_t* const topLeft = frame + top * frameStrideElements + left * tChannels;

		const unsigned int txty = txi * tyi;
		const unsigned int txty_ = txi * tyi_;
		const unsigned int tx_ty = txi_ * tyi;
		const unsigned int tx_ty_ = txi_ * tyi_;

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			result[n] = (topLeft[n] * tx_ty_ + topLeft[tChannels + n] * txty_
						+ topLeft[frameStrideElements + n] * tx_ty + topLeft[frameStrideElements + tChannels + n] * txty + 8192u) >> 14u;
		}
	}
	else
	{
		// we do not have a valid pixel for all 4-neighborhood pixels

		const unsigned int txi = (unsigned int)((pos.x() - Scalar(left)) * Scalar(128) + Scalar(0.5));
		const unsigned int txi_ = 128u - txi;

		const unsigned int tyi = (unsigned int)((pos.y() - Scalar(top)) * Scalar(128) + Scalar(0.5));
		const unsigned int tyi_ = 128u - tyi;

		const unsigned int rightOffset = (left >= 0 && left + 1u < width) ? tChannels : 0u;
		const unsigned int bottomOffset = (top >= 0 && top + 1u < height) ? frameStrideElements : 0u;

		ocean_assert(left < int(width) && top < int(height));
		const uint8_t* const topLeft = frame + max(0, top) * frameStrideElements + max(0, left) * tChannels;

		const unsigned int txty = txi * tyi;
		const unsigned int txty_ = txi * tyi_;
		const unsigned int tx_ty = txi_ * tyi;
		const unsigned int tx_ty_ = txi_ * tyi_;

		for (unsigned int n = FrameBlender::SourceOffset<tAlphaAtFront>::data(); n < tChannels + FrameBlender::SourceOffset<tAlphaAtFront>::data() - 1u; ++n)
		{
			result[n] = (topLeft[n] * tx_ty_ + topLeft[rightOffset + n] * txty_
						+ topLeft[bottomOffset + n] * tx_ty + topLeft[bottomOffset + rightOffset + n] * txty + 8192u) >> 14u;
		}

		const uint8_t alphaTopLeft = (left >= 0 && top >= 0) ? topLeft[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()] : FrameBlender::fullTransparent8Bit<tTransparentIs0xFF>();
		const uint8_t alphaTopRight = (left + 1u < width && top >= 0) ? topLeft[rightOffset + FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()] : FrameBlender::fullTransparent8Bit<tTransparentIs0xFF>();
		const uint8_t alphaBottomLeft = (left >= 0 && top + 1u < height) ? topLeft[bottomOffset + FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()] : FrameBlender::fullTransparent8Bit<tTransparentIs0xFF>();
		const uint8_t alphaBottomRight = (left + 1u < width && top + 1u < height) ? topLeft[bottomOffset + rightOffset + FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()] : FrameBlender::fullTransparent8Bit<tTransparentIs0xFF>();

		result[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()] = (alphaTopLeft * tx_ty_ + alphaTopRight * txty_ + alphaBottomLeft * tx_ty + alphaBottomRight * txty + 8192u) >> 14u;
	}
}

template <unsigned int tChannels>
void FrameInterpolatorBilinear::affine8BitPerChannelSubset(const uint8_t* source, const unsigned int sourceWidth, const unsigned int sourceHeight, const SquareMatrix3* source_A_target, const uint8_t* borderColor, uint8_t* target, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int firstTargetRow, const unsigned int numberOutputRows, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(sourceWidth > 0u && sourceHeight > 0u);
	ocean_assert_and_suppress_unused(targetWidth > 0u && targetHeight > 0u, targetHeight);
	ocean_assert(source_A_target);
	ocean_assert(!source_A_target->isNull() && Numeric::isEqualEps((*source_A_target)[2]) && Numeric::isEqualEps((*source_A_target)[5]));

	ocean_assert(firstTargetRow + numberOutputRows <= targetHeight);

	const unsigned int targetStrideElements = tChannels * targetWidth + targetPaddingElements;

	const Scalar scalarSourceWidth_1 = Scalar(sourceWidth - 1u);
	const Scalar scalarSourceHeight_1 = Scalar(sourceHeight - 1u);

	typedef typename DataType<uint8_t, tChannels>::Type PixelType;

	uint8_t zeroColor[tChannels] = {uint8_t(0)};
	const PixelType* const bColor = borderColor ? (PixelType*)borderColor : (PixelType*)zeroColor;

	for (unsigned int y = firstTargetRow; y < firstTargetRow + numberOutputRows; ++y)
	{
		PixelType* targetRow = (PixelType*)(target + y * targetStrideElements);

		/*
		 * We can slightly optimize the 3x3 matrix multiplication:
		 *
		 * | X0 Y0 Z0 |   | x |
		 * | X1 Y1 Z1 | * | y |
		 * |  0  0  1 |   | 1 |
		 *
		 * | xx |   | X0 * x |   | Y0 * y + Z0 |
		 * | yy | = | X1 * x | + | Y1 * y + Z1 |
		 *
		 * As y is constant within the inner loop, the two terms on the right side in the above equations can be pre-calculated:
		 *
		 *  C0 = Y0 * y + Z0
		 *  C1 = Y1 * y + Z1
		 *
		 * So the computation becomes:
		 *
		 * | x' |   | X0 * x |   | C0 |
		 * | y' | = | X1 * x | + | C1 |
		 */

		const Vector2 X(source_A_target->data() + 0);
		const Vector2 c(Vector2(source_A_target->data() + 3) * Scalar(y) + Vector2(source_A_target->data() + 6));

		for (unsigned int x = 0u; x < targetWidth; ++x)
		{
			const Vector2 sourcePosition = X * Scalar(x) + c;

#ifdef OCEAN_DEBUG
			const Scalar debugSourceX = (*source_A_target)[0] * Scalar(x) + (*source_A_target)[3] * Scalar(y) + (*source_A_target)[6];
			const Scalar debugSourceY = (*source_A_target)[1] * Scalar(x) + (*source_A_target)[4] * Scalar(y) + (*source_A_target)[7];
			ocean_assert(sourcePosition.isEqual(Vector2(debugSourceX, debugSourceY), Scalar(0.01)));
#endif

			if (sourcePosition.x() < Scalar(0) || sourcePosition.x() > scalarSourceWidth_1 || sourcePosition.y() < Scalar(0) || sourcePosition.y() > scalarSourceHeight_1)
			{
				*targetRow = *bColor;
			}
			else
			{
				interpolatePixel8BitPerChannel<tChannels, PC_TOP_LEFT>(source, sourceWidth, sourceHeight, sourcePaddingElements, sourcePosition, (uint8_t*)(targetRow));
			}

			targetRow++;
		}
	}
}

template <unsigned int tChannels>
void FrameInterpolatorBilinear::homography8BitPerChannelSubset(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const SquareMatrix3* input_H_output, const uint8_t* borderColor, uint8_t* output, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, const unsigned int firstOutputRow, const unsigned int numberOutputRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(input != nullptr && output != nullptr);
	ocean_assert(inputWidth > 0u && inputHeight > 0u);
	ocean_assert(outputWidth > 0u && outputHeight > 0u);
	ocean_assert(input_H_output != nullptr);

	ocean_assert_and_suppress_unused(firstOutputRow + numberOutputRows <= outputHeight, outputHeight);

	const unsigned int outputStrideElements = outputWidth * tChannels + outputPaddingElements;

	const Scalar scalarInputWidth_1 = Scalar(inputWidth - 1u);
	const Scalar scalarInputHeight_1 = Scalar(inputHeight - 1u);

	typedef typename DataType<uint8_t, tChannels>::Type PixelType;

	uint8_t zeroColor[tChannels] = {uint8_t(0)};
	const PixelType bColor = borderColor ? *(PixelType*)borderColor : *(PixelType*)zeroColor;

	for (unsigned int y = firstOutputRow; y < firstOutputRow + numberOutputRows; ++y)
	{
		/*
		 * We can slightly optimize the 3x3 matrix multiplication:
		 *
		 * | X0 Y0 Z0 |   | x |
		 * | X1 Y1 Z1 | * | y |
		 * | X2 Y2 Z2 |   | 1 |
		 *
		 * | xx |   | X0 * x |   | Y0 * y + Z0 |
		 * | yy | = | X1 * x | + | Y1 * y + Z1 |
		 * | zz |   | X2 * x |   | Y2 * y + Z2 |
		 *
		 * | xx |   | X0 * x |   | C0 |
		 * | yy | = | X1 * x | + | C1 |
		 * | zz |   | X2 * x |   | C2 |
		 *
		 * As y is constant within the inner loop, we can pre-calculate the following terms:
		 *
		 * | x' |   | (X0 * x + C0) / (X2 * x + C2) |
		 * | y' | = | (X1 * x + C1) / (X2 * x + C2) |
		 */

		const Vector2 X(input_H_output->data() + 0);
		const Vector2 c(Vector2(input_H_output->data() + 3) * Scalar(y) + Vector2(input_H_output->data() + 6));

		const Scalar X2 = (*input_H_output)(2, 0);
		const Scalar constValue2 = (*input_H_output)(2, 1) * Scalar(y) + (*input_H_output)(2, 2);

		PixelType* outputRowPixel = (PixelType*)(output + y * outputStrideElements);

		for (unsigned int x = 0u; x < outputWidth; ++x)
		{
			ocean_assert_accuracy(Numeric::isNotEqualEps((X2 * Scalar(x) + constValue2)));
			const Vector2 inputPosition((X * Scalar(x) + c) / (X2 * Scalar(x) + constValue2));

#ifdef OCEAN_DEBUG
			const Vector2 debugInputPosition(*input_H_output * Vector2(Scalar(x), Scalar(y)));
			ocean_assert(inputPosition.isEqual(debugInputPosition, Scalar(0.01)));
#endif

			if (inputPosition.x() < Scalar(0) || inputPosition.x() > scalarInputWidth_1 || inputPosition.y() < Scalar(0) || inputPosition.y() > scalarInputHeight_1)
			{
				*outputRowPixel = bColor;
			}
			else
			{
				interpolatePixel8BitPerChannel<tChannels, PC_TOP_LEFT>(input, inputWidth, inputHeight, inputPaddingElements, inputPosition, (uint8_t*)(outputRowPixel));
			}

			++outputRowPixel;
		}
	}
}

template <typename T, unsigned int tChannels>
void FrameInterpolatorBilinear::homographySubset(const T* input, const unsigned int inputWidth, const unsigned int inputHeight, const SquareMatrix3* input_H_output, const T* borderColor, T* output, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, const unsigned int firstOutputRow, const unsigned int numberOutputRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(input != nullptr && output != nullptr);
	ocean_assert(inputWidth > 0u && inputHeight > 0u);
	ocean_assert_and_suppress_unused(outputWidth > 0u && outputHeight > 0u, outputHeight);
	ocean_assert(input_H_output != nullptr);

	ocean_assert(firstOutputRow + numberOutputRows <= outputHeight);

	const unsigned int outputStrideElements = outputWidth * tChannels + outputPaddingElements;

	const Scalar scalarInputWidth1 = Scalar(inputWidth - 1u);
	const Scalar scalarInputHeight1 = Scalar(inputHeight - 1u);

	// we need to find a best matching floating point data type for the intermediate interpolation results
	typedef typename FloatTyper<T>::Type TIntermediate;

	typedef typename DataType<T, tChannels>::Type PixelType;

	constexpr T zeroColor[tChannels] = {T(0)};
	const PixelType* const bColor = borderColor ? (PixelType*)(borderColor) : (PixelType*)(zeroColor);

	constexpr TIntermediate bias = TIntermediate(0);

	for (unsigned int y = firstOutputRow; y < firstOutputRow + numberOutputRows; ++y)
	{
		/*
		 * We can slightly optimize the 3x3 matrix multiplication:
		 *
		 * | X0 Y0 Z0 |   | x |
		 * | X1 Y1 Z1 | * | y |
		 * | X2 Y2 Z2 |   | 1 |
		 *
		 * | xx |   | X0 * x |   | Y0 * y + Z0 |
		 * | yy | = | X1 * x | + | Y1 * y + Z1 |
		 * | zz |   | X2 * x |   | Y2 * y + Z2 |
		 *
		 * | xx |   | X0 * x |   | C0 |
		 * | yy | = | X1 * x | + | C1 |
		 * | zz |   | X2 * x |   | C3 |
		 *
		 * As y is constant within the inner loop, we can pre-calculate the following terms:
		 *
		 * | x' |   | (X0 * x + C0) / (X2 * x + C2) |
		 * | y' | = | (X1 * x + C1) / (X2 * x + C2) |
		 */

		const Vector2 X(input_H_output->data() + 0);
		const Vector2 c(Vector2(input_H_output->data() + 3) * Scalar(y) + Vector2(input_H_output->data() + 6));

		const Scalar X2 = (*input_H_output)(2, 0);
		const Scalar constValue2 = (*input_H_output)(2, 1) * Scalar(y) + (*input_H_output)(2, 2);

		PixelType* outputRowPixel = (PixelType*)(output + y * outputStrideElements);

		for (unsigned int x = 0u; x < outputWidth; ++x)
		{
			ocean_assert_accuracy(Numeric::isNotEqualEps((X2 * Scalar(x) + constValue2)));
			const Vector2 inputPosition((X * Scalar(x) + c) / (X2 * Scalar(x) + constValue2));

#ifdef OCEAN_DEBUG
			const Vector2 debugInputPosition(*input_H_output * Vector2(Scalar(x), Scalar(y)));
			ocean_assert((std::is_same<float, Scalar>::value) || inputPosition.isEqual(debugInputPosition, Scalar(0.01)));
#endif

			if (inputPosition.x() >= Scalar(0) && inputPosition.x() <= scalarInputWidth1 && inputPosition.y() >= Scalar(0) && inputPosition.y() <= scalarInputHeight1)
			{
				interpolatePixel<T, T, tChannels, CV::PC_TOP_LEFT, Scalar, TIntermediate>(input, inputWidth, inputHeight, inputPaddingElements, inputPosition, (T*)(outputRowPixel), bias);
			}
			else
			{
				*outputRowPixel = *bColor;
			}

			++outputRowPixel;
		}
	}
}

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

template <unsigned int tChannels>
inline void FrameInterpolatorBilinear::affine8BitPerChannelSSESubset(const uint8_t* source, const unsigned int sourceWidth, const unsigned int sourceHeight, const SquareMatrix3* source_A_target, const uint8_t* borderColor, uint8_t* target, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int firstTargetRow, const unsigned int numberTargetRows, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source && target);
	ocean_assert(sourceWidth > 0u && sourceHeight > 0u);
	ocean_assert(targetWidth >= 4u && targetHeight > 0u);
	ocean_assert(source_A_target);
	ocean_assert(!source_A_target->isNull() && Numeric::isEqualEps((*source_A_target)[2]) && Numeric::isEqualEps((*source_A_target)[5]));

	ocean_assert_and_suppress_unused(firstTargetRow + numberTargetRows <= targetHeight, targetHeight);

	const unsigned int sourceStrideElements = tChannels * sourceWidth + sourcePaddingElements;
	const unsigned int targetStrideElements = tChannels * targetWidth + targetPaddingElements;

	typedef typename DataType<uint8_t, tChannels>::Type PixelType;

	uint8_t zeroColor[tChannels] = {uint8_t(0)};
	const PixelType* const bColor = borderColor ? (PixelType*)borderColor : (PixelType*)zeroColor;

	OCEAN_ALIGN_DATA(16) unsigned int validPixels[4];

	OCEAN_ALIGN_DATA(16) unsigned int topLeftOffsets[4];
	OCEAN_ALIGN_DATA(16) unsigned int topRightOffsets[4];
	OCEAN_ALIGN_DATA(16) unsigned int bottomLeftOffsets[4];
	OCEAN_ALIGN_DATA(16) unsigned int bottomRightOffsets[4];

	// we store 4 floats: [X0, X0, X0, X0], and same with X1 and X2
	const __m128 m128_f_X0 = _mm_set_ps1(float((*source_A_target)(0, 0)));
	const __m128 m128_f_X1 = _mm_set_ps1(float((*source_A_target)(1, 0)));

	for (unsigned int y = firstTargetRow; y < firstTargetRow + numberTargetRows; ++y)
	{
		PixelType* targetRow = (PixelType*)(target + y * targetStrideElements);

		/*
		 * We can slightly optimize the 3x3 matrix multiplication:
		 *
		 * | X0 Y0 Z0 |   | x |
		 * | X1 Y1 Z1 | * | y |
		 * |  0  0  1 |   | 1 |
		 *
		 * | xx |   | X0 * x |   | Y0 * y + Z0 |
		 * | yy | = | X1 * x | + | Y1 * y + Z1 |
		 *
		 * As y is constant within the inner loop, the two terms on the right side in the above equations can be pre-calculated:
		 *
		 *  C0 = Y0 * y + Z0
		 *  C1 = Y1 * y + Z1
		 *
		 * So the computation becomes:
		 *
		 * | x' |   | X0 * x |   | C0 |
		 * | y' | = | X1 * x | + | C1 |
		 */

		// we store 4 floats: [C0, C0, C0, C0], and same with C1 and C2
		const __m128 m128_f_C0 = _mm_set_ps1(float((*source_A_target)(0, 1) * Scalar(y) + (*source_A_target)(0, 2)));
		const __m128 m128_f_C1 = _mm_set_ps1(float((*source_A_target)(1, 1) * Scalar(y) + (*source_A_target)(1, 2)));

		// we store 4 floats: [0.0f, 0.0f, 0.0f, 0.0f]
		const __m128 m128_f_zero = _mm_setzero_ps();

		// we store 4 integers: [tChannels, tChannels, tChannels, tChannels]
		const __m128i m128_i_channels = _mm_set1_epi32(tChannels);

		// we store 4 integers: [sourceStrideElements, sourceStrideElements, sourceStrideElements, sourceStrideElements]
		const __m128i m128_i_sourceStrideElements = _mm_set1_epi32(sourceStrideElements);

		// we store 4 integers: [inputWidth - 1, inputWidth - 1, inputWidth - 1, inputWidth - 1], and same with inputHeight
		const __m128i m128_i_sourceWidth_1 = _mm_set1_epi32(int(sourceWidth) - 1);
		const __m128i m128_i_sourceHeight_1 = _mm_set1_epi32(int(sourceHeight) - 1);

		// we store 4 floats: [inputWidth - 1, inputWidth - 1, inputWidth - 1, inputWidth - 1], and same with inputHeight
		const __m128 m128_f_sourceWidth_1 = _mm_set_ps1(float(sourceWidth - 1u));
		const __m128 m128_f_sourceHeight_1 = _mm_set_ps1(float(sourceHeight - 1u));

		for (unsigned int x = 0u; x < targetWidth; x += 4u)
		{
			if (x + 4u > targetWidth)
			{
				// the last iteration will not fit into the output frame,
				// so we simply shift x left by some pixels (at most 3) and we will calculate some pixels again

				ocean_assert(x >= 4u && targetWidth > 4u);
				const unsigned int newX = targetWidth - 4u;

				ocean_assert(x > newX);
				targetRow -= x - newX;

				x = newX;

				// the for loop will stop after this iteration
				ocean_assert(!(x + 4u < targetWidth));
			}


			// we need four successive x coordinate floats:
			// [x + 3.0f, x + 2.0f, x + 1.0f; x + 0.0f]
			const __m128 m128_f_x_0123 = _mm_set_ps(float(x + 3u), float(x + 2u), float(x + 1u), float(x + 0u));

			// we calculate xx and yy for [x + 3.0f, x + 2.0f, x + 1.0f, x + 0.0f]
			const __m128 m128_f_sourceX = _mm_add_ps(_mm_mul_ps(m128_f_X0, m128_f_x_0123), m128_f_C0);
			const __m128 m128_f_sourceY = _mm_add_ps(_mm_mul_ps(m128_f_X1, m128_f_x_0123), m128_f_C1);

			// now we check whether we are inside the input frame
			const __m128 m128_f_validPixelX = _mm_and_ps(_mm_cmple_ps(m128_f_sourceX, m128_f_sourceWidth_1), _mm_cmpge_ps(m128_f_sourceX, m128_f_zero)); // inputPosition.x() <= (inputWidth - 1) && inputPosition.x() >= 0 ? 0xFFFFFFFF : 0x00000000
			const __m128 m128_f_validPixelY = _mm_and_ps(_mm_cmple_ps(m128_f_sourceY, m128_f_sourceHeight_1), _mm_cmpge_ps(m128_f_sourceY, m128_f_zero)); // inputPosition.y() <= (inputHeight - 1) && inputPosition.y() >= 0 ? 0xFFFFFFFF : 0x00000000

			const __m128i m128_i_validPixel = _mm_castps_si128(_mm_and_ps(m128_f_validPixelX, m128_f_validPixelY)); // is_inside_input_frame(inputPosition) ? 0xFFFFFFFF : 0x00000000

			// we can stop here if all pixels are invalid
			if (_mm_test_all_zeros(m128_i_validPixel, _mm_set1_epi32(0xFFFFFFFF)))
			{
#ifdef OCEAN_DEBUG
				OCEAN_ALIGN_DATA(16) unsigned int debugValidPixels[4];
				_mm_store_si128((__m128i*)debugValidPixels, m128_i_validPixel);
				ocean_assert(!(debugValidPixels[0] || debugValidPixels[1] || debugValidPixels[2] || debugValidPixels[3]));
#endif

				targetRow[0] = *bColor;
				targetRow[1] = *bColor;
				targetRow[2] = *bColor;
				targetRow[3] = *bColor;

				targetRow += 4;

				continue;
			}

			// we store the result
			_mm_store_si128((__m128i*)validPixels, m128_i_validPixel);
			ocean_assert(validPixels[0] || validPixels[1] || validPixels[2] || validPixels[3]);


			// now we determine the left, top, right and bottom pixel used for the interpolation
			const __m128 m128_f_tx_floor = _mm_floor_ps(m128_f_sourceX);
			const __m128 m128_f_ty_floor = _mm_floor_ps(m128_f_sourceY);

			// left = floor(x); top = floor(y)
			const __m128i m128_i_left = _mm_cvtps_epi32(m128_f_tx_floor);
			const __m128i m128_i_top = _mm_cvtps_epi32(m128_f_ty_floor);

			// right = min(left + 1, width - 1); bottom = min(top + 1; height - 1)
			const __m128i m128_i_right = _mm_min_epu32(_mm_add_epi32(m128_i_left, _mm_set1_epi32(1)), m128_i_sourceWidth_1);
			const __m128i m128_i_bottom = _mm_min_epu32(_mm_add_epi32(m128_i_top, _mm_set1_epi32(1)), m128_i_sourceHeight_1);

			// offset = (y * sourceStrideElements + tChannels * x)
			const __m128i m128_i_topLeftOffset = _mm_add_epi32(_mm_mullo_epi32(m128_i_top, m128_i_sourceStrideElements), _mm_mullo_epi32(m128_i_channels, m128_i_left)); // topleftOffset = (top * sourceStrideElements + tChannels * left)
			const __m128i m128_i_topRightOffset = _mm_add_epi32(_mm_mullo_epi32(m128_i_top, m128_i_sourceStrideElements), _mm_mullo_epi32(m128_i_channels, m128_i_right)); // toprightOffset = (top * sourceStrideElements + tChannels * right)
			const __m128i m128_i_bottomLeftOffset = _mm_add_epi32(_mm_mullo_epi32(m128_i_bottom, m128_i_sourceStrideElements), _mm_mullo_epi32(m128_i_channels, m128_i_left)); // ...
			const __m128i m128_i_bottomRightOffset = _mm_add_epi32(_mm_mullo_epi32(m128_i_bottom, m128_i_sourceStrideElements), _mm_mullo_epi32(m128_i_channels, m128_i_right));

			// we store the offsets
			_mm_store_si128((__m128i*)topLeftOffsets, m128_i_topLeftOffset);
			_mm_store_si128((__m128i*)topRightOffsets, m128_i_topRightOffset);
			_mm_store_si128((__m128i*)bottomLeftOffsets, m128_i_bottomLeftOffset);
			_mm_store_si128((__m128i*)bottomRightOffsets, m128_i_bottomRightOffset);


			// now we need to determine the interpolation factors tx, tx_ and ty, ty_: (top_left * tx_ + top_right * tx) * ty_  +  (bottom_left * tx_ + bottom_right * tx) * ty

			// we determine the fractional portions of the x' and y':
			// e.g., [43.1231, -12.5543, -34.123, 99.2]
			//       [ 0.1231,   0.4457,   0.877,  0.2] // note the result for negative value - but we will not process negative values anyway due to 'validPixel'
			__m128 m128_f_tx = _mm_sub_ps(m128_f_sourceX, m128_f_tx_floor);
			__m128 m128_f_ty = _mm_sub_ps(m128_f_sourceY, m128_f_ty_floor);

			// we use integer interpolation [0.0, 1.0] -> [0, 128]
			m128_f_tx = _mm_mul_ps(m128_f_tx, _mm_set_ps1(128.0f));
			m128_f_ty = _mm_mul_ps(m128_f_ty, _mm_set_ps1(128.0f));

			m128_f_tx = _mm_round_ps(m128_f_tx, _MM_FROUND_TO_NEAREST_INT);
			m128_f_ty = _mm_round_ps(m128_f_ty, _MM_FROUND_TO_NEAREST_INT);

			const __m128i m128_i_tx = _mm_cvtps_epi32(m128_f_tx);
			const __m128i m128_i_ty = _mm_cvtps_epi32(m128_f_ty);

			interpolate4Pixels8BitPerChannelSSE<tChannels>(source, topLeftOffsets, topRightOffsets, bottomLeftOffsets, bottomRightOffsets, validPixels, *bColor, m128_i_tx, m128_i_ty, targetRow);
			targetRow += 4;
		}
	}
}

template <unsigned int tChannels>
inline void FrameInterpolatorBilinear::homography8BitPerChannelSSESubset(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const SquareMatrix3* input_H_output, const uint8_t* borderColor, uint8_t* output, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, const unsigned int firstOutputRow, const unsigned int numberOutputRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(input != nullptr && output != nullptr);
	ocean_assert(inputWidth > 0u && inputHeight > 0u);
	ocean_assert(outputWidth >= 4u && outputHeight > 0u);
	ocean_assert(input_H_output != nullptr);

	ocean_assert_and_suppress_unused(firstOutputRow + numberOutputRows <= outputHeight, outputHeight);

	const unsigned int inputStrideElements = inputWidth * tChannels + inputPaddingElements;
	const unsigned int outputStrideElements = outputWidth * tChannels + outputPaddingElements;

	typedef typename DataType<uint8_t, tChannels>::Type PixelType;

	uint8_t zeroColor[tChannels] = {uint8_t(0)};
	const PixelType* const bColor = borderColor ? (PixelType*)borderColor : (PixelType*)zeroColor;

	OCEAN_ALIGN_DATA(16) unsigned int validPixels[4];

	OCEAN_ALIGN_DATA(16) unsigned int topLeftOffsets[4];
	OCEAN_ALIGN_DATA(16) unsigned int topRightOffsets[4];
	OCEAN_ALIGN_DATA(16) unsigned int bottomLeftOffsets[4];
	OCEAN_ALIGN_DATA(16) unsigned int bottomRightOffsets[4];

	// we store 4 floats: [X0, X0, X0, X0], and same with X1 and X2
	const __m128 m128_f_X0 = _mm_set_ps1(float((*input_H_output)(0, 0)));
	const __m128 m128_f_X1 = _mm_set_ps1(float((*input_H_output)(1, 0)));
	const __m128 m128_f_X2 = _mm_set_ps1(float((*input_H_output)(2, 0)));

	// we store 4 floats: [0.0f, 0.0f, 0.0f, 0.0f]
	const __m128 m128_f_zero = _mm_setzero_ps();

	// we store 4 integers: [tChannels, tChannels, tChannels, tChannels]
	const __m128i m128_i_channels = _mm_set1_epi32(tChannels);

	// we store 4 integers: [inputStrideElements, inputStrideElements, inputStrideElements, inputStrideElements]
	const __m128i m128_i_inputStrideElements = _mm_set1_epi32(inputStrideElements);

	// we store 4 integers: [inputWidth - 1, inputWidth - 1, inputWidth -1, inputWidth -1], and same with inputHeight
	const __m128i m128_i_inputWidth_1 = _mm_set1_epi32(int(inputWidth) - 1);
	const __m128i m128_i_inputHeight_1 = _mm_set1_epi32(int(inputHeight) - 1);

	// we store 4 floats: [inputWidth - 1, inputWidth - 1, inputWidth - 1, inputWidth - 1], and same with inputHeight
	const __m128 m128_f_inputWidth_1 = _mm_set_ps1(float(inputWidth - 1u));
	const __m128 m128_f_inputHeight_1 = _mm_set_ps1(float(inputHeight - 1u));

	for (unsigned int y = firstOutputRow; y < firstOutputRow + numberOutputRows; ++y)
	{
		PixelType* outputPixelData = (PixelType*)(output + y * outputStrideElements);

		/*
		 * We can slightly optimize the 3x3 matrix multiplication:
		 *
		 * | X0 Y0 Z0 |   | x |
		 * | X1 Y1 Z1 | * | y |
		 * | X2 Y2 Z2 |   | 1 |
		 *
		 * | xx |   | X0 * x |   | Y0 * y + Z0 |
		 * | yy | = | X1 * x | + | Y1 * y + Z1 |
		 * | zz |   | X2 * x |   | Y2 * y + Z2 |
		 *
		 * | xx |   | X0 * x |   | C0 |
		 * | yy | = | X1 * x | + | C1 |
		 * | zz |   | X2 * x |   | C2 |
		 *
		 * As y is constant within the inner loop, we can pre-calculate the following terms:
		 *
		 * | x' |   | (X0 * x + C0) / (X2 * x + C2) |
		 * | y' | = | (X1 * x + C1) / (X2 * x + C2) |
		 */

		// we store 4 floats: [C0, C0, C0, C0], and same with C1 and C2
		const __m128 m128_f_C0 = _mm_set_ps1(float((*input_H_output)(0, 1) * Scalar(y) + (*input_H_output)(0, 2)));
		const __m128 m128_f_C1 = _mm_set_ps1(float((*input_H_output)(1, 1) * Scalar(y) + (*input_H_output)(1, 2)));
		const __m128 m128_f_C2 = _mm_set_ps1(float((*input_H_output)(2, 1) * Scalar(y) + (*input_H_output)(2, 2)));

		for (unsigned int x = 0u; x < outputWidth; x += 4u)
		{
			if (x + 4u > outputWidth)
			{
				// the last iteration will not fit into the output frame,
				// so we simply shift x left by some pixels (at most 3) and we will calculate some pixels again

				ocean_assert(x >= 4u && outputWidth > 4u);
				const unsigned int newX = outputWidth - 4u;

				ocean_assert(x > newX);
				outputPixelData -= x - newX;

				x = newX;

				// the for loop will stop after this iteration
				ocean_assert(!(x + 4u < outputWidth));
			}


			// we need four successive x coordinate floats:
			// [x + 3.0f, x + 2.0f, x + 1.0f; x + 0.0f]
			const __m128 m128_f_x_0123 = _mm_set_ps(float(x + 3u), float(x + 2u), float(x + 1u), float(x + 0u));

			// we calculate xx and yy and zz for [x + 3.0f, x + 2.0f, x + 1.0f, x + 0.0f]
			const __m128 m128_f_xx = _mm_add_ps(_mm_mul_ps(m128_f_X0, m128_f_x_0123), m128_f_C0);
			const __m128 m128_f_yy = _mm_add_ps(_mm_mul_ps(m128_f_X1, m128_f_x_0123), m128_f_C1);
			const __m128 m128_f_zz = _mm_add_ps(_mm_mul_ps(m128_f_X2, m128_f_x_0123), m128_f_C2);

#ifdef USE_APPROXIMATED_INVERSE_OF_ZZ // (not defined by default)

			// we calculate the (approximated) inverse of zz,
			// the overall performance will be approx. 5% better while the accuracy will be slightly worse:
			// [1/zz3, 1/zz2, 1/zz1, 1/zz0]
			const __m128 inv_zz_128 = _mm_rcp_ps(m128_f_zz);

			// we determine the normalized coordinates x' and y' for for x + 3.0f, x + 2.0f, ...)
			const __m128 m128_f_inputX = _mm_mul_ps(m128_f_xx, inv_zz_128);
			const __m128 m128_f_inputY = _mm_mul_ps(m128_f_yy, inv_zz_128);

#else

			// we determine the normalized coordinates x' and y' for for x + 3.0f, x + 2.0f, ...)
			const __m128 m128_f_inputX = _mm_div_ps(m128_f_xx, m128_f_zz);
			const __m128 m128_f_inputY = _mm_div_ps(m128_f_yy, m128_f_zz);

#endif // USE_APPROXIMATED_INVERSE_OF_ZZ


			// now we check whether we are inside the input frame
			const __m128 m128_f_validPixelX = _mm_and_ps(_mm_cmple_ps (m128_f_inputX, m128_f_inputWidth_1), _mm_cmpge_ps(m128_f_inputX, m128_f_zero)); // inputPosition.x() <= (inputWidth-1) && inputPosition.x() >= 0 ? 0xFFFFFF : 0x000000
			const __m128 m128_f_validPixelY = _mm_and_ps(_mm_cmple_ps (m128_f_inputY, m128_f_inputHeight_1), _mm_cmpge_ps(m128_f_inputY, m128_f_zero)); // inputPosition.y() <= (inputHeight-1) && inputPosition.y() >= 0 ? 0xFFFFFF : 0x000000

			const __m128i m128_i_validPixel = _mm_castps_si128(_mm_and_ps(m128_f_validPixelX, m128_f_validPixelY)); // is_inside_input_frame(inputPosition) ? 0xFFFFFF : 0x000000

			// we can stop here if all pixels are invalid
			if (_mm_test_all_zeros(m128_i_validPixel, _mm_set1_epi32(0xFFFFFFFF)))
			{
#ifdef OCEAN_DEBUG
				OCEAN_ALIGN_DATA(16) unsigned int debugValidPixels[4];
				_mm_store_si128((__m128i*)debugValidPixels, m128_i_validPixel);
				ocean_assert(!(debugValidPixels[0] || debugValidPixels[1] || debugValidPixels[2] || debugValidPixels[3]));
#endif

				outputPixelData[0] = *bColor;
				outputPixelData[1] = *bColor;
				outputPixelData[2] = *bColor;
				outputPixelData[3] = *bColor;

				outputPixelData += 4;

				continue;
			}

			// we store the result
			_mm_store_si128((__m128i*)validPixels, m128_i_validPixel);
			ocean_assert(validPixels[0] || validPixels[1] || validPixels[2] || validPixels[3]);


			// now we determine the left, top, right and bottom pixel used for the interpolation
			const __m128 m128_f_tx_floor = _mm_floor_ps(m128_f_inputX);
			const __m128 m128_f_ty_floor = _mm_floor_ps(m128_f_inputY);

			// left = floor(x); top = floor(y)
			const __m128i m128_i_left = _mm_cvtps_epi32(m128_f_tx_floor);
			const __m128i m128_i_top = _mm_cvtps_epi32(m128_f_ty_floor);

			// right = min(left + 1, width - 1); bottom = min(top + 1; height - 1)
			const __m128i m128_i_right = _mm_min_epu32(_mm_add_epi32(m128_i_left, _mm_set1_epi32(1)), m128_i_inputWidth_1);
			const __m128i m128_i_bottom = _mm_min_epu32(_mm_add_epi32(m128_i_top, _mm_set1_epi32(1)), m128_i_inputHeight_1);

			// offset = (y * inputStrideElements + tChannels * x)
			const __m128i m128_i_topLeftOffset = _mm_add_epi32(_mm_mullo_epi32(m128_i_top, m128_i_inputStrideElements), _mm_mullo_epi32(m128_i_channels, m128_i_left)); // topleftOffset = (top * inputStrideElements + tChannels * left)
			const __m128i m128_i_topRightOffset = _mm_add_epi32(_mm_mullo_epi32(m128_i_top, m128_i_inputStrideElements), _mm_mullo_epi32(m128_i_channels, m128_i_right)); // toprightOffset = (top * inputStrideElements + tChannels * right)
			const __m128i m128_i_bottomLeftOffset = _mm_add_epi32(_mm_mullo_epi32(m128_i_bottom, m128_i_inputStrideElements), _mm_mullo_epi32(m128_i_channels, m128_i_left)); // ...
			const __m128i m128_i_bottomRightOffset = _mm_add_epi32(_mm_mullo_epi32(m128_i_bottom, m128_i_inputStrideElements), _mm_mullo_epi32(m128_i_channels, m128_i_right));

			// we store the offsets
			_mm_store_si128((__m128i*)topLeftOffsets, m128_i_topLeftOffset);
			_mm_store_si128((__m128i*)topRightOffsets, m128_i_topRightOffset);
			_mm_store_si128((__m128i*)bottomLeftOffsets, m128_i_bottomLeftOffset);
			_mm_store_si128((__m128i*)bottomRightOffsets, m128_i_bottomRightOffset);


			// now we need to determine the interpolation factors tx, tx_ and ty, ty_: (top_left * tx_ + top_right * tx) * ty_  +  (bottom_left * tx_ + bottom_right * tx) * ty

			// we determine the fractional portions of the x' and y':
			// e.g., [43.1231, -12.5543, -34.123, 99.2]
			//       [ 0.1231,   0.4457,   0.877,  0.2] // note the result for negative value - but we will not process negative values anyway due to 'validPixel'
			__m128 m128_f_tx = _mm_sub_ps(m128_f_inputX, m128_f_tx_floor);
			__m128 m128_f_ty = _mm_sub_ps(m128_f_inputY, m128_f_ty_floor);

			// we use integer interpolation [0.0, 1.0] -> [0, 128]
			m128_f_tx = _mm_mul_ps(m128_f_tx, _mm_set_ps1(128.0f));
			m128_f_ty = _mm_mul_ps(m128_f_ty, _mm_set_ps1(128.0f));

			m128_f_tx = _mm_round_ps(m128_f_tx, _MM_FROUND_TO_NEAREST_INT);
			m128_f_ty = _mm_round_ps(m128_f_ty, _MM_FROUND_TO_NEAREST_INT);

			const __m128i m128_i_tx = _mm_cvtps_epi32(m128_f_tx);
			const __m128i m128_i_ty = _mm_cvtps_epi32(m128_f_ty);

			interpolate4Pixels8BitPerChannelSSE<tChannels>(input, topLeftOffsets, topRightOffsets, bottomLeftOffsets, bottomRightOffsets, validPixels, *bColor, m128_i_tx, m128_i_ty, outputPixelData);
			outputPixelData += 4;
		}
	}
}

template <>
OCEAN_FORCE_INLINE __m128i FrameInterpolatorBilinear::interpolate4Pixels8BitPerChannelSSE<3u>(const __m128i& sourcesTopLeft, const __m128i& sourcesTopRight, const __m128i& sourcesBottomLeft, const __m128i& sourcesBottomRight, const __m128i& factorsTopLeft, const __m128i& factorsTopRight, const __m128i& factorsBottomLeft, const __m128i& factorsBottomRight)
{
	// sourcesTopLeft stores the three color values of 4 (independent) pixels (the upper left pixels):
	// FEDC BA98 7654 3210
	// ---- VUYV UYVU YVUY
	// sourcesTopRight, sourcesBottomLeft, sourcesBottomRight have the same pattern

	// factorsTopLeft stores the 32 bit interpolation values for 4 pixels:
	// FEDC BA98 7654 3210
	//    3    2    1    0 (32 bit interpolation values, fitting into 16 bit)


	// we will simply extract each channel from the source pixels,
	// each extracted channel will be multiplied by the corresponding interpolation factor
	// and all interpolation results will be accumulated afterwards

	                                                                                            // FEDC BA98 7654 3210
	const __m128i mask32_Channel0 = SSE::set128i(0xFFFFFF09FFFFFF06ull, 0xFFFFFF03FFFFFF00ull); // ---9 ---6 ---3 ---0
	const __m128i mask32_Channel1 = SSE::set128i(0xFFFFFF0AFFFFFF07ull, 0xFFFFFF04FFFFFF01ull); // ---A ---7 ---4 ---1
	const __m128i mask32_Channel2 = SSE::set128i(0xFFFFFF0BFFFFFF08ull, 0xFFFFFF05FFFFFF02ull); // ---B ---8 ---5 ---2


	// we extract the first channel from the top left pixel values and multiply the channel with the interpolation factors
	// FEDC BA98 7654 3210
	// ---9 ---6 ---3 ---0
	//          *
	// FTL3 FTL2 FTL1 FTL0
	__m128i multiplication_channel0 = _mm_mullo_epi32(factorsTopLeft, _mm_shuffle_epi8(sourcesTopLeft, mask32_Channel0));

	// we the same multiplication for the second channel
	__m128i multiplication_channel1 = _mm_mullo_epi32(factorsTopLeft, _mm_shuffle_epi8(sourcesTopLeft, mask32_Channel1));

	// and third channel
	__m128i multiplication_channel2 = _mm_mullo_epi32(factorsTopLeft, _mm_shuffle_epi8(sourcesTopLeft, mask32_Channel2));


	// now we repeat the process for the top right pixel values
	multiplication_channel0 = _mm_add_epi32(multiplication_channel0, _mm_mullo_epi32(factorsTopRight, _mm_shuffle_epi8(sourcesTopRight, mask32_Channel0)));
	multiplication_channel1 = _mm_add_epi32(multiplication_channel1, _mm_mullo_epi32(factorsTopRight, _mm_shuffle_epi8(sourcesTopRight, mask32_Channel1)));
	multiplication_channel2 = _mm_add_epi32(multiplication_channel2, _mm_mullo_epi32(factorsTopRight, _mm_shuffle_epi8(sourcesTopRight, mask32_Channel2)));


	// and for the bottom left pixel values
	multiplication_channel0 = _mm_add_epi32(multiplication_channel0, _mm_mullo_epi32(factorsBottomLeft, _mm_shuffle_epi8(sourcesBottomLeft, mask32_Channel0)));
	multiplication_channel1 = _mm_add_epi32(multiplication_channel1, _mm_mullo_epi32(factorsBottomLeft, _mm_shuffle_epi8(sourcesBottomLeft, mask32_Channel1)));
	multiplication_channel2 = _mm_add_epi32(multiplication_channel2, _mm_mullo_epi32(factorsBottomLeft, _mm_shuffle_epi8(sourcesBottomLeft, mask32_Channel2)));


	// and for the bottom right pixel values
	multiplication_channel0 = _mm_add_epi32(multiplication_channel0, _mm_mullo_epi32(factorsBottomRight, _mm_shuffle_epi8(sourcesBottomRight, mask32_Channel0)));
	multiplication_channel1 = _mm_add_epi32(multiplication_channel1, _mm_mullo_epi32(factorsBottomRight, _mm_shuffle_epi8(sourcesBottomRight, mask32_Channel1)));
	multiplication_channel2 = _mm_add_epi32(multiplication_channel2, _mm_mullo_epi32(factorsBottomRight, _mm_shuffle_epi8(sourcesBottomRight, mask32_Channel2)));


	const __m128i m128_i_8192 = _mm_set1_epi32(8192);

	// we add 8192 for rounding and shift the result by 14 bits (division by 128*128)	 // TODO if using 256 we should be able to avoid the shifting by 14 bits (simply by using shuffle operations)

	// in addition to rounding and shifting, we need to move the interpolation results to the correct channel:
	//     target data: ---9 ---6 ---3 ---0
	// shufflet target: ---- --9- -6-- 3--0
	//   mask location: ---C ---8 ---4 ---0
	//            mask: ---- --C- -8-- 4--0
	__m128i interpolation_channel0 = _mm_shuffle_epi8(_mm_srli_epi32(_mm_add_epi32(multiplication_channel0, m128_i_8192), 14), SSE::set128i(0xFFFFFFFFFFFF0CFFull, 0xFF08FFFF04FFFF00ull));

	//     target data: ---A ---7 ---4 ---1
	// shufflet target: ---- -A-- 7--4 --1-
	//   mask location: ---C ---8 ---4 ---0
	//            mask: ---- -C-- 8--4 --0-
	__m128i interpolation_channel1 = _mm_shuffle_epi8(_mm_srli_epi32(_mm_add_epi32(multiplication_channel1, m128_i_8192), 14), SSE::set128i(0xFFFFFFFFFF0CFFFFull, 0x08FFFF04FFFF00FFull));

	//     target data: ---B ---8 ---5 ---2
	// shufflet target: ---- B--8 --5- -2--
	//   mask location: ---C ---8 ---4 ---0
	//            mask: ---- C--8 --4- -0--
	__m128i interpolation_channel2 = _mm_shuffle_epi8(_mm_srli_epi32(_mm_add_epi32(multiplication_channel2, m128_i_8192), 14), SSE::set128i(0xFFFFFFFF0CFFFF08ull, 0xFFFF04FFFF00FFFFull));


	// finally, we simply blend all interpolation results together

	return _mm_or_si128(_mm_or_si128(interpolation_channel0, interpolation_channel1), interpolation_channel2);
}

template <>
OCEAN_FORCE_INLINE __m128i FrameInterpolatorBilinear::interpolate4Pixels8BitPerChannelSSE<4u>(const __m128i& sourcesTopLeft, const __m128i& sourcesTopRight, const __m128i& sourcesBottomLeft, const __m128i& sourcesBottomRight, const __m128i& factorsTopLeft, const __m128i& factorsTopRight, const __m128i& factorsBottomLeft, const __m128i& factorsBottomRight)
{
	// sourcesTopLeft stores the four color values of 4 (independent) pixels (the upper left pixels):
	// FEDC BA98 7654 3210
	// AVUY AVUY AVUY AVUY
	// sourcesTopRight, sourcesBottomLeft, sourcesBottomRight have the same pattern

	// factorsTopLeft stores the 32 bit interpolation values for 4 pixels:
	// FEDC BA98 7654 3210
	//    3    2    1    0 (32 bit interpolation values, fitting into 16 bit)


	// we will simply extract each channel from the source pixels,
	// each extracted channel will be multiplied by the corresponding interpolation factor
	// and all interpolation results will be accumulated afterwards

	                                                                                            // FEDC BA98 7654 3210
	const __m128i mask32_Channel0 = SSE::set128i(0xA0A0A00CA0A0A008ull, 0xA0A0A004A0A0A000ull); // ---C ---8 ---4 ---0
	const __m128i mask32_Channel1 = SSE::set128i(0xA0A0A00DA0A0A009ull, 0xA0A0A005A0A0A001ull); // ---D ---9 ---5 ---1
	const __m128i mask32_Channel2 = SSE::set128i(0xA0A0A00EA0A0A00Aull, 0xA0A0A006A0A0A002ull); // ---E ---A ---6 ---2
	const __m128i mask32_Channel3 = SSE::set128i(0xA0A0A00FA0A0A00Bull, 0xA0A0A007A0A0A003ull); // ---F ---B ---7 ---3


	// we extract the first channel from the top left pixel values and multiply the channel with the interpolation factors
	// FEDC BA98 7654 3210
	// ---C ---8 ---4 ---0
	//          *
	// FTL3 FTL2 FTL1 FTL0
	__m128i multiplication_channel0 = _mm_mullo_epi32(factorsTopLeft, _mm_shuffle_epi8(sourcesTopLeft, mask32_Channel0));

	// we the same multiplication for the second channel
	__m128i multiplication_channel1 = _mm_mullo_epi32(factorsTopLeft, _mm_shuffle_epi8(sourcesTopLeft, mask32_Channel1));

	// and third channel
	__m128i multiplication_channel2 = _mm_mullo_epi32(factorsTopLeft, _mm_shuffle_epi8(sourcesTopLeft, mask32_Channel2));

	// and last channel
	__m128i multiplication_channel3 = _mm_mullo_epi32(factorsTopLeft, _mm_shuffle_epi8(sourcesTopLeft, mask32_Channel3));


	// now we repeat the process for the top right pixel values
	multiplication_channel0 = _mm_add_epi32(multiplication_channel0, _mm_mullo_epi32(factorsTopRight, _mm_shuffle_epi8(sourcesTopRight, mask32_Channel0)));
	multiplication_channel1 = _mm_add_epi32(multiplication_channel1, _mm_mullo_epi32(factorsTopRight, _mm_shuffle_epi8(sourcesTopRight, mask32_Channel1)));
	multiplication_channel2 = _mm_add_epi32(multiplication_channel2, _mm_mullo_epi32(factorsTopRight, _mm_shuffle_epi8(sourcesTopRight, mask32_Channel2)));
	multiplication_channel3 = _mm_add_epi32(multiplication_channel3, _mm_mullo_epi32(factorsTopRight, _mm_shuffle_epi8(sourcesTopRight, mask32_Channel3)));


	// and for the bottom left pixel values
	multiplication_channel0 = _mm_add_epi32(multiplication_channel0, _mm_mullo_epi32(factorsBottomLeft, _mm_shuffle_epi8(sourcesBottomLeft, mask32_Channel0)));
	multiplication_channel1 = _mm_add_epi32(multiplication_channel1, _mm_mullo_epi32(factorsBottomLeft, _mm_shuffle_epi8(sourcesBottomLeft, mask32_Channel1)));
	multiplication_channel2 = _mm_add_epi32(multiplication_channel2, _mm_mullo_epi32(factorsBottomLeft, _mm_shuffle_epi8(sourcesBottomLeft, mask32_Channel2)));
	multiplication_channel3 = _mm_add_epi32(multiplication_channel3, _mm_mullo_epi32(factorsBottomLeft, _mm_shuffle_epi8(sourcesBottomLeft, mask32_Channel3)));


	// and for the bottom right pixel values
	multiplication_channel0 = _mm_add_epi32(multiplication_channel0, _mm_mullo_epi32(factorsBottomRight, _mm_shuffle_epi8(sourcesBottomRight, mask32_Channel0)));
	multiplication_channel1 = _mm_add_epi32(multiplication_channel1, _mm_mullo_epi32(factorsBottomRight, _mm_shuffle_epi8(sourcesBottomRight, mask32_Channel1)));
	multiplication_channel2 = _mm_add_epi32(multiplication_channel2, _mm_mullo_epi32(factorsBottomRight, _mm_shuffle_epi8(sourcesBottomRight, mask32_Channel2)));
	multiplication_channel3 = _mm_add_epi32(multiplication_channel3, _mm_mullo_epi32(factorsBottomRight, _mm_shuffle_epi8(sourcesBottomRight, mask32_Channel3)));


	const __m128i m128_i_8192 = _mm_set1_epi32(8192);

	// we add 8192 for rounding and shift the result by 14 bits (division by 128*128)

	// in addition to rounding and shifting, we need to move the interpolation results to the correct channel:
	// ---C ---8 ---4 ---0
	// ---C ---9 ---4 ---0
	__m128i interpolation_channel0 = _mm_srli_epi32(_mm_add_epi32(multiplication_channel0, m128_i_8192), 14);

	// in addition to rounding and shifting, we need to move the interpolation results to the correct channel:
	// ---D ---9 ---5 ---1
	// --D- --9- --5- --1-
	__m128i interpolation_channel1 = _mm_slli_epi32(_mm_srli_epi32(_mm_add_epi32(multiplication_channel1, m128_i_8192), 14), 8);

	// ---E ---A ---6 ---2
	// -E-- -A-- -6-- -2--
	__m128i interpolation_channel2 = _mm_slli_epi32(_mm_srli_epi32(_mm_add_epi32(multiplication_channel2, m128_i_8192), 14), 16);

	// ---F ---B ---7 ---3
	// F--- B--- 7--- 3---
	__m128i interpolation_channel3 = _mm_slli_epi32(_mm_srli_epi32(_mm_add_epi32(multiplication_channel3, m128_i_8192), 14), 24);


	// finally, we simply blend all interpolation results together

	return _mm_or_si128(_mm_or_si128(interpolation_channel0, interpolation_channel1), _mm_or_si128(interpolation_channel2, interpolation_channel3));
}

#ifdef OCEAN_COMPILER_MSC

// we see a significant performance decrease with non-VS compilers/platforms,
// so we do not use the 3channel version with non-Windows compilers

template <>
OCEAN_FORCE_INLINE void FrameInterpolatorBilinear::interpolate4Pixels8BitPerChannelSSE<1u>(const uint8_t* source, const unsigned int offsetsTopLeft[4], const unsigned int offsetsTopRight[4], const unsigned int offsetsBottomLeft[4], const unsigned int offsetsBottomRight[4], const unsigned int validPixels[4], const DataType<uint8_t, 1u>::Type& borderColor, const __m128i& m128_factorsRight, const __m128i& m128_factorsBottom, typename DataType<uint8_t, 1u>::Type* targetPositionPixels)
{
	ocean_assert(source != nullptr);
	ocean_assert(targetPositionPixels != nullptr);

	typedef typename DataType<uint8_t, 1u>::Type PixelType;

	// as we do not initialize the following intermediate data,
	// we hopefully will not allocate memory on the stack each time this function is called
	OCEAN_ALIGN_DATA(16) PixelType pixels[16];

	// we gather the individual source pixel values from the source image,
	// based on the calculated pixel locations
	for (unsigned int i = 0u; i < 4u; ++i)
	{
		if (validPixels[i])
		{
			pixels[i * 4u + 0u] = *((PixelType*)(source + offsetsTopLeft[i]));
			pixels[i * 4u + 1u] = *((PixelType*)(source + offsetsTopRight[i]));
			pixels[i * 4u + 2u] = *((PixelType*)(source + offsetsBottomLeft[i]));
			pixels[i * 4u + 3u] = *((PixelType*)(source + offsetsBottomRight[i]));
		}
		else
		{
			pixels[i * 4u + 0u] = borderColor;
			pixels[i * 4u + 1u] = borderColor;
			pixels[i * 4u + 2u] = borderColor;
			pixels[i * 4u + 3u] = borderColor;
		}
	}

	static_assert(sizeof(__m128i) == sizeof(pixels), "Invalid data type!");

	const __m128i m128_pixels = _mm_load_si128((const __m128i*)pixels);


	// factorLeft = 128 - factorRight
	// factorTop = 128 - factorBottom

	const __m128i m128_factorsLeft = _mm_sub_epi32(_mm_set1_epi32(128), m128_factorsRight);
	const __m128i m128_factorsTop = _mm_sub_epi32(_mm_set1_epi32(128), m128_factorsBottom);

	// (top_left * factorLeft + top_right * factorRight) * factorTop  +  (bottom_left * factorLeft + bottom_right * factorRight) * factorBottom
	//    == top_left * factorTopLeft + top_right * factorTopRight + bottom_left * factorBottomLeft + bottom_right * factorBottomRight

	const __m128i m128_factorsTopLeft = _mm_mullo_epi32(m128_factorsTop, m128_factorsLeft);
	const __m128i m128_factorsTopRight = _mm_mullo_epi32(m128_factorsTop, m128_factorsRight);
	const __m128i m128_factorsBottomLeft = _mm_mullo_epi32(m128_factorsBottom, m128_factorsLeft);
	const __m128i m128_factorsBottomRight = _mm_mullo_epi32(m128_factorsBottom, m128_factorsRight);

	// pixels stores the four interpolation grascale pixel values (top left, top right, bottom left, bottom right) for 4 (independent) pixels:
	//  F  E  D  C    B  A  9  8    7  6  5  4    3  2  1  0
	// BR BL TR TL   BR BL TR TL   BR BL TR TL   BR BL TR TL

	// factorsTopLeft stores the 32 bit interpolation values for 4 pixels:
	// FEDC BA98 7654 3210
	//    3    2    1    0 (32 bit interpolation values, fitting into 16 bit)


	// we will simply extract each channel from the source pixels,
	// each extracted channel will be multiplied by the corresponding interpolation factor
	// and all interpolation results will be accumulated afterwards

	                                                                                               // FEDC BA98 7654 3210
	const __m128i mask32_topLeft =     SSE::set128i(0xFFFFFF0CFFFFFF08ull, 0xFFFFFF04FFFFFF00ull); // ---C ---8 ---4 ---0
	const __m128i mask32_topRight =    SSE::set128i(0xFFFFFF0DFFFFFF09ull, 0xFFFFFF05FFFFFF01ull); // ---D ---9 ---5 ---1
	const __m128i mask32_bottomLeft =  SSE::set128i(0xFFFFFF0EFFFFFF0Aull, 0xFFFFFF06FFFFFF02ull); // ---E ---A ---6 ---2
	const __m128i mask32_bottomRight = SSE::set128i(0xFFFFFF0FFFFFFF0Bull, 0xFFFFFF07FFFFFF03ull); // ---F ---B ---7 ---3


	// we extract the top left values and multiply them with the interpolation factors
	// FEDC BA98 7654 3210
	// ---C ---8 ---4 ---0
	//          *
	// FTL3 FTL2 FTL1 FTL0
	__m128i multiplicationA = _mm_mullo_epi32(m128_factorsTopLeft, _mm_shuffle_epi8(m128_pixels, mask32_topLeft));
	__m128i multiplicationB = _mm_mullo_epi32(m128_factorsTopRight, _mm_shuffle_epi8(m128_pixels, mask32_topRight));

	multiplicationA = _mm_add_epi32(multiplicationA, _mm_mullo_epi32(m128_factorsBottomLeft, _mm_shuffle_epi8(m128_pixels, mask32_bottomLeft)));
	multiplicationB = _mm_add_epi32(multiplicationB, _mm_mullo_epi32(m128_factorsBottomRight, _mm_shuffle_epi8(m128_pixels, mask32_bottomRight)));

	__m128i multiplication = _mm_add_epi32(multiplicationA, multiplicationB);

	const __m128i m128_i_8192 = _mm_set1_epi32(8192);

	// we add 8192 for rounding and shift the result by 14 bits (division by 128*128)	 // TODO if using 256 we should be able to avoid the shifting by 14 bits (simply by using shuffle operations)
	// additionally, we shuffle the individual results together

	const __m128i result = _mm_shuffle_epi8(_mm_srli_epi32(_mm_add_epi32(multiplication, m128_i_8192), 14), SSE::set128i(0xFFFFFFFFFFFFFFFFull, 0xFFFFFFFF0C080400ull));

	*((unsigned int*)targetPositionPixels) = _mm_extract_epi32(result, 0);
}

template <>
OCEAN_FORCE_INLINE void FrameInterpolatorBilinear::interpolate4Pixels8BitPerChannelSSE<3u>(const uint8_t* source, const unsigned int offsetsTopLeft[4], const unsigned int offsetsTopRight[4], const unsigned int offsetsBottomLeft[4], const unsigned int offsetsBottomRight[4], const unsigned int validPixels[4], const DataType<uint8_t, 3u>::Type& borderColor, const __m128i& m128_factorsRight, const __m128i& m128_factorsBottom, typename DataType<uint8_t, 3u>::Type* targetPositionPixels)
{
	ocean_assert(source != nullptr);
	ocean_assert(targetPositionPixels != nullptr);

	typedef typename DataType<uint8_t, 3u>::Type PixelType;

	// as we do not initialize the following intermediate data,
	// we hopefully will not allocate memory on the stack each time this function is called
	OCEAN_ALIGN_DATA(16) PixelType topLeftPixels[6];
	OCEAN_ALIGN_DATA(16) PixelType topRightPixels[6];
	OCEAN_ALIGN_DATA(16) PixelType bottomLeftPixels[6];
	OCEAN_ALIGN_DATA(16) PixelType bottomRightPixels[6];

	// we gather the individual source pixel values from the source image,
	// based on the calculated pixel locations
	for (unsigned int i = 0u; i < 4u; ++i)
	{
		if (validPixels[i])
		{
			topLeftPixels[i] = *((PixelType*)(source + offsetsTopLeft[i]));
			topRightPixels[i] = *((PixelType*)(source + offsetsTopRight[i]));
			bottomLeftPixels[i] = *((PixelType*)(source + offsetsBottomLeft[i]));
			bottomRightPixels[i] = *((PixelType*)(source + offsetsBottomRight[i]));
		}
		else
		{
			topLeftPixels[i] = borderColor;
			topRightPixels[i] = borderColor;
			bottomLeftPixels[i] = borderColor;
			bottomRightPixels[i] = borderColor;
		}
	}

	static_assert(sizeof(__m128i) <= sizeof(topLeftPixels), "Invalid data type!");

	const __m128i m128_topLeftPixels = _mm_load_si128((const __m128i*)topLeftPixels);
	const __m128i m128_topRightPixels = _mm_load_si128((const __m128i*)topRightPixels);
	const __m128i m128_bottomLeftPixels = _mm_load_si128((const __m128i*)bottomLeftPixels);
	const __m128i m128_bottomRightPixels = _mm_load_si128((const __m128i*)bottomRightPixels);


	// factorLeft = 128 - factorRight
	// factorTop = 128 - factorBottom

	const __m128i m128_factorsLeft = _mm_sub_epi32(_mm_set1_epi32(128), m128_factorsRight);
	const __m128i m128_factorsTop = _mm_sub_epi32(_mm_set1_epi32(128), m128_factorsBottom);

	// (top_left * factorLeft + top_right * factorRight) * factorTop  +  (bottom_left * factorLeft + bottom_right * factorRight) * factorBottom
	//    == top_left * factorTopLeft + top_right * factorTopRight + bottom_left * factorBottomLeft + bottom_right * factorBottomRight

	const __m128i m128_factorsTopLeft = _mm_mullo_epi32(m128_factorsTop, m128_factorsLeft);
	const __m128i m128_factorsTopRight = _mm_mullo_epi32(m128_factorsTop, m128_factorsRight);
	const __m128i m128_factorsBottomLeft = _mm_mullo_epi32(m128_factorsBottom, m128_factorsLeft);
	const __m128i m128_factorsBottomRight = _mm_mullo_epi32(m128_factorsBottom, m128_factorsRight);


	const __m128i m128_interpolationResult = interpolate4Pixels8BitPerChannelSSE<3u>(m128_topLeftPixels, m128_topRightPixels, m128_bottomLeftPixels, m128_bottomRightPixels, m128_factorsTopLeft, m128_factorsTopRight, m128_factorsBottomLeft, m128_factorsBottomRight);

	// we copy the first 12 bytes
	memcpy(targetPositionPixels, &m128_interpolationResult, 12u);
}

#endif // OCEAN_COMPILER_MSC

template <>
OCEAN_FORCE_INLINE void FrameInterpolatorBilinear::interpolate4Pixels8BitPerChannelSSE<4u>(const uint8_t* source, const unsigned int offsetsTopLeft[4], const unsigned int offsetsTopRight[4], const unsigned int offsetsBottomLeft[4], const unsigned int offsetsBottomRight[4], const unsigned int validPixels[4], const DataType<uint8_t, 4u>::Type& borderColor, const __m128i& m128_factorsRight, const __m128i& m128_factorsBottom, typename DataType<uint8_t, 4u>::Type* targetPositionPixels)
{
	ocean_assert(source != nullptr);
	ocean_assert(targetPositionPixels != nullptr);

	typedef typename DataType<uint8_t, 4u>::Type PixelType;

	// as we do not initialize the following intermediate data,
	// we hopefully will not allocate memory on the stack each time this function is called
	OCEAN_ALIGN_DATA(16) PixelType topLeftPixels[4];
	OCEAN_ALIGN_DATA(16) PixelType topRightPixels[4];
	OCEAN_ALIGN_DATA(16) PixelType bottomLeftPixels[4];
	OCEAN_ALIGN_DATA(16) PixelType bottomRightPixels[4];

	// we gather the individual source pixel values from the source image,
	// based on the calculated pixel locations

	for (unsigned int i = 0u; i < 4u; ++i)
	{
		if (validPixels[i])
		{
			topLeftPixels[i] = *((PixelType*)(source + offsetsTopLeft[i]));
			topRightPixels[i] = *((PixelType*)(source + offsetsTopRight[i]));
			bottomLeftPixels[i] = *((PixelType*)(source + offsetsBottomLeft[i]));
			bottomRightPixels[i] = *((PixelType*)(source + offsetsBottomRight[i]));
		}
		else
		{
			topLeftPixels[i] = borderColor;
			topRightPixels[i] = borderColor;
			bottomLeftPixels[i] = borderColor;
			bottomRightPixels[i] = borderColor;
		}
	}

	static_assert(sizeof(__m128i) == sizeof(topLeftPixels), "Invalid data type!");

	const __m128i m128_topLeftPixels = _mm_load_si128((const __m128i*)topLeftPixels);
	const __m128i m128_topRightPixels = _mm_load_si128((const __m128i*)topRightPixels);
	const __m128i m128_bottomLeftPixels = _mm_load_si128((const __m128i*)bottomLeftPixels);
	const __m128i m128_bottomRightPixels = _mm_load_si128((const __m128i*)bottomRightPixels);


	// factorLeft = 128 - factorRight
	// factorTop = 128 - factorBottom

	const __m128i m128_factorsLeft = _mm_sub_epi32(_mm_set1_epi32(128), m128_factorsRight);
	const __m128i m128_factorsTop = _mm_sub_epi32(_mm_set1_epi32(128), m128_factorsBottom);

	// (top_left * factorLeft + top_right * factorRight) * factorTop  +  (bottom_left * factorLeft + bottom_right * factorRight) * factorBottom
	//    == top_left * factorTopLeft + top_right * factorTopRight + bottom_left * factorBottomLeft + bottom_right * factorBottomRight

	const __m128i m128_factorsTopLeft = _mm_mullo_epi32(m128_factorsTop, m128_factorsLeft);
	const __m128i m128_factorsTopRight = _mm_mullo_epi32(m128_factorsTop, m128_factorsRight);
	const __m128i m128_factorsBottomLeft = _mm_mullo_epi32(m128_factorsBottom, m128_factorsLeft);
	const __m128i m128_factorsBottomRight = _mm_mullo_epi32(m128_factorsBottom, m128_factorsRight);


	const __m128i m128_interpolationResult = interpolate4Pixels8BitPerChannelSSE<4u>(m128_topLeftPixels, m128_topRightPixels, m128_bottomLeftPixels, m128_bottomRightPixels, m128_factorsTopLeft, m128_factorsTopRight, m128_factorsBottomLeft, m128_factorsBottomRight);

	_mm_storeu_si128((__m128i*)targetPositionPixels, m128_interpolationResult);
}

template <unsigned int tChannels>
OCEAN_FORCE_INLINE void FrameInterpolatorBilinear::interpolate4Pixels8BitPerChannelSSE(const uint8_t* source, const unsigned int offsetsTopLeft[4], const unsigned int offsetsTopRight[4], const unsigned int offsetsBottomLeft[4], const unsigned int offsetsBottomRight[4], const unsigned int validPixels[4], const typename DataType<uint8_t, tChannels>::Type& borderColor, const __m128i& m128_factorsRight, const __m128i& m128_factorsBottom, typename DataType<uint8_t, tChannels>::Type* targetPositionPixels)
{
	ocean_assert(source != nullptr);
	ocean_assert(targetPositionPixels != nullptr);

	// as we do not initialize the following intermediate data,
	// we hopefully will not allocate memory on the stack each time this function is called
	OCEAN_ALIGN_DATA(16) unsigned int factorsTopLeft[4];
	OCEAN_ALIGN_DATA(16) unsigned int factorsTopRight[4];
	OCEAN_ALIGN_DATA(16) unsigned int factorsBottomLeft[4];
	OCEAN_ALIGN_DATA(16) unsigned int factorsBottomRight[4];


	// factorLeft = 128 - factorRight
	// factorTop = 128 - factorBottom

	const __m128i m128_factorsLeft = _mm_sub_epi32(_mm_set1_epi32(128), m128_factorsRight);
	const __m128i m128_factorsTop = _mm_sub_epi32(_mm_set1_epi32(128), m128_factorsBottom);

	// (top_left * factorLeft + top_right * factorRight) * factorTop  +  (bottom_left * factorLeft + bottom_right * factorRight) * factorBottom
	//    == top_left * factorTopLeft + top_right * factorTopRight + bottom_left * factorBottomLeft + bottom_right * factorBottomRight

	const __m128i m128_factorsTopLeft = _mm_mullo_epi32(m128_factorsTop, m128_factorsLeft);
	const __m128i m128_factorsTopRight = _mm_mullo_epi32(m128_factorsTop, m128_factorsRight);
	const __m128i m128_factorsBottomLeft = _mm_mullo_epi32(m128_factorsBottom, m128_factorsLeft);
	const __m128i m128_factorsBottomRight = _mm_mullo_epi32(m128_factorsBottom, m128_factorsRight);


	// we store the interpolation factors
	_mm_store_si128((__m128i*)factorsTopLeft, m128_factorsTopLeft);
	_mm_store_si128((__m128i*)factorsTopRight, m128_factorsTopRight);
	_mm_store_si128((__m128i*)factorsBottomLeft, m128_factorsBottomLeft);
	_mm_store_si128((__m128i*)factorsBottomRight, m128_factorsBottomRight);

	for (unsigned int i = 0u; i < 4u; ++i)
	{
		if (validPixels[i])
		{
			const uint8_t* topLeft = source + offsetsTopLeft[i];
			const uint8_t* topRight = source + offsetsTopRight[i];

			const uint8_t* bottomLeft = source + offsetsBottomLeft[i];
			const uint8_t* bottomRight = source + offsetsBottomRight[i];

			const unsigned int& factorTopLeft = factorsTopLeft[i];
			const unsigned int& factorTopRight = factorsTopRight[i];
			const unsigned int& factorBottomLeft = factorsBottomLeft[i];
			const unsigned int& factorBottomRight = factorsBottomRight[i];

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				((uint8_t*)targetPositionPixels)[n] = (uint8_t)((topLeft[n] * factorTopLeft + topRight[n] * factorTopRight + bottomLeft[n] * factorBottomLeft + bottomRight[n] * factorBottomRight + 8192u) >> 14u);
			}
		}
		else
		{
			*targetPositionPixels = borderColor;
		}

		targetPositionPixels++;
	}
}

#endif // OCEAN_HARDWARE_SSE_VERSION

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

template <unsigned int tChannels>
void FrameInterpolatorBilinear::affine8BitPerChannelNEONSubset(const uint8_t* source, const unsigned int sourceWidth, const unsigned int sourceHeight, const SquareMatrix3* source_A_target, const uint8_t* borderColor, uint8_t* target, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int firstTargetRow, const unsigned int numberTargetRows, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source && target);
	ocean_assert(sourceWidth > 0u && sourceHeight > 0u);
	ocean_assert(targetWidth >= 4u && targetHeight > 0u);
	ocean_assert(source_A_target);
	ocean_assert(!source_A_target->isNull() && Numeric::isEqualEps((*source_A_target)[2]) && Numeric::isEqualEps((*source_A_target)[5]));

	ocean_assert(firstTargetRow + numberTargetRows <= targetHeight);

	const unsigned int sourceStrideElements = sourceWidth * tChannels + sourcePaddingElements;
	const unsigned int targetStrideElements = targetWidth * tChannels + targetPaddingElements;

	typedef typename DataType<uint8_t, tChannels>::Type PixelType;

	uint8_t zeroColor[tChannels] = {uint8_t(0)};
	const PixelType* const bColor = borderColor ? (PixelType*)borderColor : (PixelType*)zeroColor;

	unsigned int validPixels[4];

	unsigned int topLeftOffsetsElements[4];
	unsigned int topRightOffsetsElements[4];
	unsigned int bottomLeftOffsetsElements[4];
	unsigned int bottomRightOffsetsElements[4];

	const uint32x4_t constantChannels_u_32x4 = vdupq_n_u32(tChannels);

	// we store 4 floats: [X0, X0, X0, X0], and same with X1 and X2
	const float32x4_t m128_f_X0 = vdupq_n_f32(float((*source_A_target)(0, 0)));
	const float32x4_t m128_f_X1 = vdupq_n_f32(float((*source_A_target)(1, 0)));

	for (unsigned int y = firstTargetRow; y < firstTargetRow + numberTargetRows; ++y)
	{
		PixelType* targetRow = (PixelType*)(target + y * targetStrideElements);

		/*
		 * We can slightly optimize the 3x3 matrix multiplication:
		 *
		 * | X0 Y0 Z0 |   | x |
		 * | X1 Y1 Z1 | * | y |
		 * |  0  0  1 |   | 1 |
		 *
		 * | xx |   | X0 * x |   | Y0 * y + Z0 |
		 * | yy | = | X1 * x | + | Y1 * y + Z1 |
		 *
		 * As y is constant within the inner loop, the two terms on the right side in the above equations can be pre-calculated:
		 *
		 *  C0 = Y0 * y + Z0
		 *  C1 = Y1 * y + Z1
		 *
		 * So the computation becomes:
		 *
		 * | x' |   | X0 * x |   | C0 |
		 * | y' | = | X1 * x | + | C1 |
		 */

		// we store 4 floats: [C0, C0, C0, C0], and same with C1 and C2
		const float32x4_t m128_f_C0 = vdupq_n_f32(float((*source_A_target)(0, 1) * Scalar(y) + (*source_A_target)(0, 2)));
		const float32x4_t m128_f_C1 = vdupq_n_f32(float((*source_A_target)(1, 1) * Scalar(y) + (*source_A_target)(1, 2)));

		// we store 4 floats: [0.0f, 0.0f, 0.0f, 0.0f]
		const float32x4_t m128_f_zero = vdupq_n_f32(0.0f);

		// we store 4 integers: [sourceStrideElements, sourceStrideElements, sourceStrideElements, sourceStrideElements]
		const uint32x4_t m128_u_sourceStrideElements = vdupq_n_u32(sourceStrideElements);

		// we store 4 integers: [sourceWidth - 1, sourceWidth - 1, sourceWidth - 1, sourceWidth - 1], and same with sourceHeight
		const uint32x4_t m128_u_sourceWidth_1 = vdupq_n_u32(sourceWidth - 1u);
		const uint32x4_t m128_u_sourceHeight_1 = vdupq_n_u32(sourceHeight - 1u);

		// we store 4 floats: [sourceWidth - 1, sourceWidth - 1, sourceWidth - 1, sourceWidth - 1], and same with sourceHeight
		const float32x4_t m128_f_sourceWidth_1 = vdupq_n_f32(float(sourceWidth - 1u));
		const float32x4_t m128_f_sourceHeight_1 = vdupq_n_f32(float(sourceHeight - 1u));

		for (unsigned int x = 0u; x < targetWidth; x += 4u)
		{
			if (x + 4u > targetWidth)
			{
				// the last iteration will not fit into the target frame,
				// so we simply shift x left by some pixels (at most 3) and we will calculate some pixels again

				ocean_assert(x >= 4u && targetWidth > 4u);
				const unsigned int newX = targetWidth - 4u;

				ocean_assert(x > newX);
				targetRow -= x - newX;

				x = newX;

				// the for loop will stop after this iteration
				ocean_assert(!(x + 4u < targetWidth));
			}


			// we need four successive x coordinate floats:
			// [x + 3.0f, x + 2.0f, x + 1.0f; x + 0.0f]
			float x_0123[4] = {float(x + 0u), float(x + 1u), float(x + 2u), float(x + 3u)};
			const float32x4_t m128_f_x_0123 = vld1q_f32(x_0123);

			// we calculate xx and yy and zz for [x + 3.0f, x + 2.0f, x + 1.0f, x + 0.0f]
			const float32x4_t m128_f_sourceX = vmlaq_f32(m128_f_C0, m128_f_X0, m128_f_x_0123);
			const float32x4_t m128_f_sourceY = vmlaq_f32(m128_f_C1, m128_f_X1, m128_f_x_0123);


			// now we check whether we are inside the source frame
			const uint32x4_t m128_u_validPixelX = vandq_u32(vcleq_f32(m128_f_sourceX, m128_f_sourceWidth_1), vcgeq_f32(m128_f_sourceX, m128_f_zero)); // sourcePosition.x() <= (sourceWidth - 1) && sourcePosition.x() >= 0 ? 0xFFFFFFFF : 0x00000000
			const uint32x4_t m128_u_validPixelY = vandq_u32(vcleq_f32(m128_f_sourceY, m128_f_sourceHeight_1), vcgeq_f32(m128_f_sourceY, m128_f_zero)); // sourcePosition.y() <= (sourceHeight - 1) && sourcePosition.y() >= 0 ? 0xFFFFFFFF : 0x00000000

			const uint32x4_t m128_u_validPixel = vandq_u32(m128_u_validPixelX, m128_u_validPixelY); // is_inside_source_frame(sourcePosition) ? 0xFFFFFFFF : 0x00000000


			// we can stop here if all pixels are invalid
			const uint32x2_t m64_u_validPixel = vorr_u32(vget_low_u32(m128_u_validPixel), vget_high_u32(m128_u_validPixel));
			if ((vget_lane_u32(m64_u_validPixel, 0) | vget_lane_u32(m64_u_validPixel, 1)) == 0x00000000u)
			{
#ifdef OCEAN_DEBUG
				OCEAN_ALIGN_DATA(16) unsigned int debugValidPixels[4];
				vst1q_u32(debugValidPixels, m128_u_validPixel);
				ocean_assert(!(debugValidPixels[0] || debugValidPixels[1] || debugValidPixels[2] || debugValidPixels[3]));
#endif

				targetRow[0] = *bColor;
				targetRow[1] = *bColor;
				targetRow[2] = *bColor;
				targetRow[3] = *bColor;

				targetRow += 4;

				continue;
			}


			// we store the result
			vst1q_u32(validPixels, m128_u_validPixel);
			ocean_assert(validPixels[0] || validPixels[1] || validPixels[2] || validPixels[3]);


			// now we determine the left, top, right and bottom pixel used for the interpolation
			// left = floor(x); top = floor(y)
			const uint32x4_t m128_u_left = vcvtq_u32_f32(m128_f_sourceX);
			const uint32x4_t m128_u_top = vcvtq_u32_f32(m128_f_sourceY);

			// right = min(left + 1, width - 1); bottom = min(top + 1; height - 1)
			const uint32x4_t m128_u_right = vminq_u32(vaddq_u32(m128_u_left, vdupq_n_u32(1u)), m128_u_sourceWidth_1);
			const uint32x4_t m128_u_bottom = vminq_u32(vaddq_u32(m128_u_top, vdupq_n_u32(1u)), m128_u_sourceHeight_1);

			// offset = y * stride + x * channels
			const uint32x4_t m128_u_topLeftOffsetElements = vmlaq_u32(vmulq_u32(m128_u_left, constantChannels_u_32x4), m128_u_top, m128_u_sourceStrideElements); // topLeftOffset = top * strideElements + left * channels
			const uint32x4_t m128_u_topRightOffsetElements = vmlaq_u32(vmulq_u32(m128_u_right, constantChannels_u_32x4), m128_u_top, m128_u_sourceStrideElements); // topRightOffset = top * strideElements + right * channels
			const uint32x4_t m128_u_bottomLeftOffsetElements = vmlaq_u32(vmulq_u32(m128_u_left, constantChannels_u_32x4), m128_u_bottom, m128_u_sourceStrideElements); // ...
			const uint32x4_t m128_u_bottomRightOffsetElements = vmlaq_u32(vmulq_u32(m128_u_right, constantChannels_u_32x4), m128_u_bottom, m128_u_sourceStrideElements);

			// we store the offsets
			vst1q_u32(topLeftOffsetsElements, m128_u_topLeftOffsetElements);
			vst1q_u32(topRightOffsetsElements, m128_u_topRightOffsetElements);
			vst1q_u32(bottomLeftOffsetsElements, m128_u_bottomLeftOffsetElements);
			vst1q_u32(bottomRightOffsetsElements, m128_u_bottomRightOffsetElements);


			// now we need to determine the interpolation factors tx, tx_ and ty, ty_: (top_left * tx_ + top_right * tx) * ty_  +  (bottom_left * tx_ + bottom_right * tx) * ty

			// we determine the fractional portions of the x' and y':
			float32x4_t m128_f_tx = vsubq_f32(m128_f_sourceX, vcvtq_f32_u32(m128_u_left));
			float32x4_t m128_f_ty = vsubq_f32(m128_f_sourceY, vcvtq_f32_u32(m128_u_top));

			// we use integer interpolation [0.0, 1.0] -> [0, 128]
			m128_f_tx = vmulq_f32(m128_f_tx, vdupq_n_f32(128.0f));
			m128_f_ty = vmulq_f32(m128_f_ty, vdupq_n_f32(128.0f));

			const uint32x4_t m128_u_tx = vcvtq_u32_f32(vaddq_f32(m128_f_tx, vdupq_n_f32(0.5)));
			const uint32x4_t m128_u_ty = vcvtq_u32_f32(vaddq_f32(m128_f_ty, vdupq_n_f32(0.5)));

			if constexpr (tChannels > 4u)
			{
				// normally we would simply call instead of copying the code of the function to this location
				// however, if calling the function instead of applying the code here directly
				// clang ends with code approx. 20% slower
				// thus we make a copy of the code and keep the function for demonstration purposes

				//interpolate4Pixels8BitPerChannelNEON<tChannels>(source, topLeftOffsetsElements, topRightOffsetsElements, bottomLeftOffsetsElements, bottomRightOffsetsElements, validPixels, *bColor, m128_u_tx, m128_u_ty, targetPixelData);
				//targetPixelData += 4;

				const uint32x4_t m128_u_tx_ = vsubq_u32(vdupq_n_u32(128u), m128_u_tx);
				const uint32x4_t m128_u_ty_ = vsubq_u32(vdupq_n_u32(128u), m128_u_ty);

				// (top_left * tx_ + top_right * tx) * ty_  +  (bottom_left * tx_ + bottom_right * tx) * ty
				//    == top_left * tx_ty_ + top_right * txty_ + bottom_left * tx_ty + bottom_right * txty
				const uint32x4_t m128_u_tx_ty_ = vmulq_u32(m128_u_tx_, m128_u_ty_);
				const uint32x4_t m128_u_txty_ = vmulq_u32(m128_u_tx, m128_u_ty_);
				const uint32x4_t m128_u_tx_ty = vmulq_u32(m128_u_tx_, m128_u_ty);
				const uint32x4_t m128_u_txty = vmulq_u32(m128_u_tx, m128_u_ty);

				unsigned int tx_ty_s[4];
				unsigned int txty_s[4];
				unsigned int tx_tys[4];
				unsigned int txtys[4];

				// we store the interpolation factors
				vst1q_u32(tx_ty_s, m128_u_tx_ty_);
				vst1q_u32(txty_s, m128_u_txty_);
				vst1q_u32(tx_tys, m128_u_tx_ty);
				vst1q_u32(txtys, m128_u_txty);

				for (unsigned int i = 0u; i < 4u; ++i)
				{
					if (validPixels[i])
					{
						ocean_assert(topLeftOffsetsElements[i] < sourceStrideElements * sourceHeight);
						ocean_assert(topRightOffsetsElements[i] < sourceStrideElements * sourceHeight);
						ocean_assert(bottomLeftOffsetsElements[i] < sourceStrideElements * sourceHeight);
						ocean_assert(bottomRightOffsetsElements[i] < sourceStrideElements * sourceHeight);

						const uint8_t* topLeft = source + topLeftOffsetsElements[i];
						const uint8_t* topRight = source + topRightOffsetsElements[i];

						const uint8_t* bottomLeft = source + bottomLeftOffsetsElements[i];
						const uint8_t* bottomRight = source + bottomRightOffsetsElements[i];

						const unsigned int tx_ty_ = tx_ty_s[i];
						const unsigned int txty_ = txty_s[i];
						const unsigned int tx_ty = tx_tys[i];
						const unsigned int txty = txtys[i];

						ocean_assert(tx_ty_ + txty_ + tx_ty + txty == 128u * 128u);

						for (unsigned int n = 0u; n < tChannels; ++n)
						{
							((uint8_t*)targetRow)[n] = uint8_t((topLeft[n] * tx_ty_ + topRight[n] * txty_ + bottomLeft[n] * tx_ty + bottomRight[n] * txty + 8192u) >> 14u);
						}
					}
					else
					{
						*targetRow = *bColor;
					}

					targetRow++;
				}
			}
			else
			{
				interpolate4Pixels8BitPerChannelNEON<tChannels>(source, topLeftOffsetsElements, topRightOffsetsElements, bottomLeftOffsetsElements, bottomRightOffsetsElements, validPixels, *bColor, m128_u_tx, m128_u_ty, targetRow);
				targetRow += 4;
			}
		}
	}
}

template <unsigned int tChannels>
void FrameInterpolatorBilinear::homography8BitPerChannelNEONSubset(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const SquareMatrix3* input_H_output, const uint8_t* borderColor, uint8_t* output, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, const unsigned int firstOutputRow, const unsigned int numberOutputRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(input != nullptr && output != nullptr);
	ocean_assert(inputWidth > 0u && inputHeight > 0u);
	ocean_assert(outputWidth >= 4u && outputHeight > 0u);
	ocean_assert(input_H_output != nullptr);

	ocean_assert(firstOutputRow + numberOutputRows <= outputHeight);

	const unsigned int inputStrideElements = inputWidth * tChannels + inputPaddingElements;
	const unsigned int outputStrideElements = outputWidth * tChannels + outputPaddingElements;

	typedef typename DataType<uint8_t, tChannels>::Type PixelType;

	uint8_t zeroColor[tChannels] = {uint8_t(0)};
	const PixelType* const bColor = borderColor ? (PixelType*)borderColor : (PixelType*)zeroColor;

	unsigned int validPixels[4];

	unsigned int topLeftOffsetsElements[4];
	unsigned int topRightOffsetsElements[4];
	unsigned int bottomLeftOffsetsElements[4];
	unsigned int bottomRightOffsetsElements[4];

	const uint32x4_t constantChannels_u_32x4 = vdupq_n_u32(tChannels);

	// we store 4 floats: [X0, X0, X0, X0], and same with X1 and X2
	const float32x4_t m128_f_X0 = vdupq_n_f32(float((*input_H_output)(0, 0)));
	const float32x4_t m128_f_X1 = vdupq_n_f32(float((*input_H_output)(1, 0)));
	const float32x4_t m128_f_X2 = vdupq_n_f32(float((*input_H_output)(2, 0)));

	for (unsigned int y = firstOutputRow; y < firstOutputRow + numberOutputRows; ++y)
	{
		PixelType* outputPixelData = (PixelType*)(output + y * outputStrideElements);

		/*
		 * We can slightly optimize the 3x3 matrix multiplication:
		 *
		 * | X0 Y0 Z0 |   | x |
		 * | X1 Y1 Z1 | * | y |
		 * | X2 Y2 Z2 |   | 1 |
		 *
		 * | xx |   | X0 * x |   | Y0 * y + Z0 |
		 * | yy | = | X1 * x | + | Y1 * y + Z1 |
		 * | zz |   | X2 * x |   | Y2 * y + Z2 |
		 *
		 * | xx |   | X0 * x |   | C0 |
		 * | yy | = | X1 * x | + | C1 |
		 * | zz |   | X2 * x |   | C3 |
		 *
		 * As y is constant within the inner loop, we can pre-calculate the following terms:
		 *
		 * | x' |   | (X0 * x + C0) / (X2 * x + C2) |
		 * | y' | = | (X1 * x + C1) / (X2 * x + C2) |
		 */

		// we store 4 floats: [C0, C0, C0, C0], and same with C1 and C2
		const float32x4_t m128_f_C0 = vdupq_n_f32(float((*input_H_output)(0, 1) * Scalar(y) + (*input_H_output)(0, 2)));
		const float32x4_t m128_f_C1 = vdupq_n_f32(float((*input_H_output)(1, 1) * Scalar(y) + (*input_H_output)(1, 2)));
		const float32x4_t m128_f_C2 = vdupq_n_f32(float((*input_H_output)(2, 1) * Scalar(y) + (*input_H_output)(2, 2)));

		// we store 4 floats: [0.0f, 0.0f, 0.0f, 0.0f]
		const float32x4_t m128_f_zero = vdupq_n_f32(0.0f);

		// we store 4 integers: [inputStrideElements, inputStrideElements, inputStrideElements, inputStrideElements]
		const uint32x4_t m128_u_inputStrideElements = vdupq_n_u32(inputStrideElements);

		// we store 4 integers: [inputWidth - 1, inputWidth - 1, inputWidth - 1, inputWidth - 1], and same with inputHeight
		const uint32x4_t m128_u_inputWidth_1 = vdupq_n_u32(inputWidth - 1u);
		const uint32x4_t m128_u_inputHeight_1 = vdupq_n_u32(inputHeight - 1u);

		// we store 4 floats: [inputWidth - 1, inputWidth - 1, inputWidth - 1, inputWidth - 1], and same with inputHeight
		const float32x4_t m128_f_inputWidth_1 = vdupq_n_f32(float(inputWidth - 1u));
		const float32x4_t m128_f_inputHeight_1 = vdupq_n_f32(float(inputHeight - 1u));

		for (unsigned int x = 0u; x < outputWidth; x += 4u)
		{
			if (x + 4u > outputWidth)
			{
				// the last iteration will not fit into the output frame,
				// so we simply shift x left by some pixels (at most 3) and we will calculate some pixels again

				ocean_assert(x >= 4u && outputWidth > 4u);
				const unsigned int newX = outputWidth - 4u;

				ocean_assert(x > newX);
				outputPixelData -= x - newX;

				x = newX;

				// the for loop will stop after this iteration
				ocean_assert(!(x + 4u < outputWidth));
			}


			// we need four successive x coordinate floats:
			// [x + 3.0f, x + 2.0f, x + 1.0f; x + 0.0f]
			float x_0123[4] = {float(x + 0u), float(x + 1u), float(x + 2u), float(x + 3u)};
			const float32x4_t m128_f_x_0123 = vld1q_f32(x_0123);

			// we calculate xx and yy and zz for [x + 3.0f, x + 2.0f, x + 1.0f, x + 0.0f]
			const float32x4_t m128_f_xx = vmlaq_f32(m128_f_C0, m128_f_X0, m128_f_x_0123);
			const float32x4_t m128_f_yy = vmlaq_f32(m128_f_C1, m128_f_X1, m128_f_x_0123);
			const float32x4_t m128_f_zz = vmlaq_f32(m128_f_C2, m128_f_X2, m128_f_x_0123);

#ifdef USE_DIVISION_ARM64_ARCHITECTURE

			// using the division available from ARM64 is more precise
			const float32x4_t m128_f_inputX = vdivq_f32(m128_f_xx, m128_f_zz);
			const float32x4_t m128_f_inputY = vdivq_f32(m128_f_yy, m128_f_zz);

#else

			// we calculate the (approximated) inverse of zz
			// [1/zz3, 1/zz2, 1/zz1, 1/zz0]
			float32x4_t inv_zz_128 = vrecpeq_f32(m128_f_zz);
			inv_zz_128 = vmulq_f32(vrecpsq_f32(m128_f_zz, inv_zz_128), inv_zz_128); // improving the accuracy of the approx. inverse by Newton/Raphson

			// we determine the normalized coordinates x' and y' for for x + 3.0f, x + 2.0f, ...)
			const float32x4_t m128_f_inputX = vmulq_f32(m128_f_xx, inv_zz_128);
			const float32x4_t m128_f_inputY = vmulq_f32(m128_f_yy, inv_zz_128);

#endif // USE_DIVISION_ARM64_ARCHITECTURE


			// now we check whether we are inside the input frame
			const uint32x4_t m128_u_validPixelX = vandq_u32(vcleq_f32(m128_f_inputX, m128_f_inputWidth_1), vcgeq_f32(m128_f_inputX, m128_f_zero)); // inputPosition.x() >= 0 && inputPosition.x() <= (inputWidth - 1) ? 0xFFFFFF : 0x000000
			const uint32x4_t m128_u_validPixelY = vandq_u32(vcleq_f32(m128_f_inputY, m128_f_inputHeight_1), vcgeq_f32(m128_f_inputY, m128_f_zero)); // inputPosition.y() >= 0 && inputPosition.y() <= (inputHeight - 1) ? 0xFFFFFF : 0x000000

			const uint32x4_t m128_u_validPixel = vandq_u32(m128_u_validPixelX, m128_u_validPixelY); // is_inside_input_frame(inputPosition) ? 0xFFFFFF : 0x000000


			// we can stop here if all pixels are invalid
			const uint32x2_t m64_u_validPixel = vorr_u32(vget_low_u32(m128_u_validPixel), vget_high_u32(m128_u_validPixel));
			if ((vget_lane_u32(m64_u_validPixel, 0) | vget_lane_u32(m64_u_validPixel, 1)) == 0x00000000u)
			{
#ifdef OCEAN_DEBUG
				OCEAN_ALIGN_DATA(16) unsigned int debugValidPixels[4];
				vst1q_u32(debugValidPixels, m128_u_validPixel);
				ocean_assert(!(debugValidPixels[0] || debugValidPixels[1] || debugValidPixels[2] || debugValidPixels[3]));
#endif

				outputPixelData[0] = *bColor;
				outputPixelData[1] = *bColor;
				outputPixelData[2] = *bColor;
				outputPixelData[3] = *bColor;

				outputPixelData += 4;

				continue;
			}


			// we store the result
			vst1q_u32(validPixels, m128_u_validPixel);
			ocean_assert(validPixels[0] || validPixels[1] || validPixels[2] || validPixels[3]);


			// now we determine the left, top, right and bottom pixel used for the interpolation
			// left = floor(x); top = floor(y)
			const uint32x4_t m128_u_left = vcvtq_u32_f32(m128_f_inputX);
			const uint32x4_t m128_u_top = vcvtq_u32_f32(m128_f_inputY);

			// right = min(left + 1, width - 1); bottom = min(top + 1; height - 1)
			const uint32x4_t m128_u_right = vminq_u32(vaddq_u32(m128_u_left, vdupq_n_u32(1u)), m128_u_inputWidth_1);
			const uint32x4_t m128_u_bottom = vminq_u32(vaddq_u32(m128_u_top, vdupq_n_u32(1u)), m128_u_inputHeight_1);

			// offset = y * stride + x * channels
			const uint32x4_t m128_u_topLeftOffsetElements = vmlaq_u32(vmulq_u32(m128_u_left, constantChannels_u_32x4), m128_u_top, m128_u_inputStrideElements); // topLeftOffset = top * strideElements + left * channels
			const uint32x4_t m128_u_topRightOffsetElements = vmlaq_u32(vmulq_u32(m128_u_right, constantChannels_u_32x4), m128_u_top, m128_u_inputStrideElements); // topRightOffset = top * strideElements + right * channels
			const uint32x4_t m128_u_bottomLeftOffsetElements = vmlaq_u32(vmulq_u32(m128_u_left, constantChannels_u_32x4), m128_u_bottom, m128_u_inputStrideElements); // ...
			const uint32x4_t m128_u_bottomRightOffsetElements = vmlaq_u32(vmulq_u32(m128_u_right, constantChannels_u_32x4), m128_u_bottom, m128_u_inputStrideElements);

			// we store the offsets
			vst1q_u32(topLeftOffsetsElements, m128_u_topLeftOffsetElements);
			vst1q_u32(topRightOffsetsElements, m128_u_topRightOffsetElements);
			vst1q_u32(bottomLeftOffsetsElements, m128_u_bottomLeftOffsetElements);
			vst1q_u32(bottomRightOffsetsElements, m128_u_bottomRightOffsetElements);


			// now we need to determine the interpolation factors tx, tx_ and ty, ty_: (top_left * tx_ + top_right * tx) * ty_  +  (bottom_left * tx_ + bottom_right * tx) * ty

			// we determine the fractional portions of the x' and y':
			float32x4_t m128_f_tx = vsubq_f32(m128_f_inputX, vcvtq_f32_u32(m128_u_left));
			float32x4_t m128_f_ty = vsubq_f32(m128_f_inputY, vcvtq_f32_u32(m128_u_top));

			// we use integer interpolation [0.0, 1.0] -> [0, 128]
			m128_f_tx = vmulq_f32(m128_f_tx, vdupq_n_f32(128.0f));
			m128_f_ty = vmulq_f32(m128_f_ty, vdupq_n_f32(128.0f));

			const uint32x4_t m128_u_tx = vcvtq_u32_f32(vaddq_f32(m128_f_tx, vdupq_n_f32(0.5)));
			const uint32x4_t m128_u_ty = vcvtq_u32_f32(vaddq_f32(m128_f_ty, vdupq_n_f32(0.5)));

			if constexpr (tChannels > 4u)
			{
				// normally we would simply call instead of copying the code of the function to this location
				// however, if calling the function instead of applying the code here directly
				// clang ends with code approx. 20% slower
				// thus we make a copy of the code and keep the function for demonstration purposes

				//interpolate4Pixels8BitPerChannelNEON<tChannels>(input, topLeftOffsetsElements, topRightOffsetsElements, bottomLeftOffsetsElements, bottomRightOffsetsElements, validPixels, *bColor, m128_u_tx, m128_u_ty, outputPixelData);
				//outputPixelData += 4;

				const uint32x4_t m128_u_tx_ = vsubq_u32(vdupq_n_u32(128u), m128_u_tx);
				const uint32x4_t m128_u_ty_ = vsubq_u32(vdupq_n_u32(128u), m128_u_ty);

				// (top_left * tx_ + top_right * tx) * ty_  +  (bottom_left * tx_ + bottom_right * tx) * ty
				//    == top_left * tx_ty_ + top_right * txty_ + bottom_left * tx_ty + bottom_right * txty
				const uint32x4_t m128_u_tx_ty_ = vmulq_u32(m128_u_tx_, m128_u_ty_);
				const uint32x4_t m128_u_txty_ = vmulq_u32(m128_u_tx, m128_u_ty_);
				const uint32x4_t m128_u_tx_ty = vmulq_u32(m128_u_tx_, m128_u_ty);
				const uint32x4_t m128_u_txty = vmulq_u32(m128_u_tx, m128_u_ty);

				unsigned int tx_ty_s[4];
				unsigned int txty_s[4];
				unsigned int tx_tys[4];
				unsigned int txtys[4];

				// we store the interpolation factors
				vst1q_u32(tx_ty_s, m128_u_tx_ty_);
				vst1q_u32(txty_s, m128_u_txty_);
				vst1q_u32(tx_tys, m128_u_tx_ty);
				vst1q_u32(txtys, m128_u_txty);

				for (unsigned int i = 0u; i < 4u; ++i)
				{
					if (validPixels[i])
					{
						ocean_assert(topLeftOffsetsElements[i] < inputStrideElements * inputHeight);
						ocean_assert(topRightOffsetsElements[i] < inputStrideElements * inputHeight);
						ocean_assert(bottomLeftOffsetsElements[i] < inputStrideElements * inputHeight);
						ocean_assert(bottomRightOffsetsElements[i] < inputStrideElements * inputHeight);

						const uint8_t* topLeft = input + topLeftOffsetsElements[i];
						const uint8_t* topRight = input + topRightOffsetsElements[i];

						const uint8_t* bottomLeft = input + bottomLeftOffsetsElements[i];
						const uint8_t* bottomRight = input + bottomRightOffsetsElements[i];

						const unsigned int tx_ty_ = tx_ty_s[i];
						const unsigned int txty_ = txty_s[i];
						const unsigned int tx_ty = tx_tys[i];
						const unsigned int txty = txtys[i];

						ocean_assert(tx_ty_ + txty_ + tx_ty + txty == 128u * 128u);

						for (unsigned int n = 0u; n < tChannels; ++n)
						{
							((uint8_t*)outputPixelData)[n] = uint8_t((topLeft[n] * tx_ty_ + topRight[n] * txty_ + bottomLeft[n] * tx_ty + bottomRight[n] * txty + 8192u) >> 14u);
						}
					}
					else
					{
						*outputPixelData = *bColor;
					}

					outputPixelData++;
				}
			}
			else
			{
				interpolate4Pixels8BitPerChannelNEON<tChannels>(input, topLeftOffsetsElements, topRightOffsetsElements, bottomLeftOffsetsElements, bottomRightOffsetsElements, validPixels, *bColor, m128_u_tx, m128_u_ty, outputPixelData);
				outputPixelData += 4;
			}
		}
	}
}

template <>
OCEAN_FORCE_INLINE void FrameInterpolatorBilinear::interpolate4Pixels8BitPerChannelNEON<1u>(const uint8_t* source, const unsigned int offsetsTopLeftElements[4], const unsigned int offsetsTopRightElements[4], const unsigned int offsetsBottomLeftElements[4], const unsigned int offsetsBottomRightElements[4], const unsigned int validPixels[4], const DataType<uint8_t, 1u>::Type& borderColor, const uint32x4_t& m128_factorsRight, const uint32x4_t& m128_factorsBottom, typename DataType<uint8_t, 1u>::Type* targetPositionPixels)
{
	ocean_assert(source != nullptr);
	ocean_assert(targetPositionPixels != nullptr);

	// as we do not initialize the following intermediate data,
	// we hopefully will not allocate memory on the stack each time this function is called
	DataType<uint8_t, 1u>::Type pixels[16];

	// we will store the pixel information in the following pattern:
	//   F   E   D   C   B   A   9   8   7   6   5   4   3   2   1   0
	// BR3 BL3 TR3 TL3 BR2 BL2 TR2 TL2 BR1 BL1 TR1 TL1 BR0 BL0 TR0 TL0

	// we gather the individual source pixel values from the source image,
	// based on the calculated pixel locations
	for (unsigned int i = 0u; i < 4u; ++i)
	{
		if (validPixels[i])
		{
			pixels[i * 4u + 0u] = *((const DataType<uint8_t, 1u>::Type*)(source + offsetsTopLeftElements[i]));
			pixels[i * 4u + 1u] = *((const DataType<uint8_t, 1u>::Type*)(source + offsetsTopRightElements[i]));
			pixels[i * 4u + 2u] = *((const DataType<uint8_t, 1u>::Type*)(source + offsetsBottomLeftElements[i]));
			pixels[i * 4u + 3u] = *((const DataType<uint8_t, 1u>::Type*)(source + offsetsBottomRightElements[i]));
		}
		else
		{
			pixels[i * 4u + 0u] = borderColor;
			pixels[i * 4u + 1u] = borderColor;
			pixels[i * 4u + 2u] = borderColor;
			pixels[i * 4u + 3u] = borderColor;
		}
	}

	static_assert(sizeof(uint8x16_t) == sizeof(pixels), "Invalid data type!");

	const uint8x16_t m128_pixels = vld1q_u8((const uint8_t*)pixels);


	// factorLeft = 128 - factorRight
	// factorTop = 128 - factorBottom

	const uint32x4_t m128_factorsLeft = vsubq_u32(vdupq_n_u32(128u), m128_factorsRight);
	const uint32x4_t m128_factorsTop = vsubq_u32(vdupq_n_u32(128u), m128_factorsBottom);

	// (top_left * factorLeft + top_right * factorRight) * factorTop  +  (bottom_left * factorLeft + bottom_right * factorRight) * factorBottom
	//    == top_left * factorTopLeft + top_right * factorTopRight + bottom_left * factorBottomLeft + bottom_right * factorBottomRight

	const uint32x4_t m128_factorsTopLeft = vmulq_u32(m128_factorsTop, m128_factorsLeft);
	const uint32x4_t m128_factorsTopRight = vmulq_u32(m128_factorsTop, m128_factorsRight);
	const uint32x4_t m128_factorsBottomLeft = vmulq_u32(m128_factorsBottom, m128_factorsLeft);
	const uint32x4_t m128_factorsBottomRight = vmulq_u32(m128_factorsBottom, m128_factorsRight);

	// pixels stores the four interpolation grascale pixel values (top left, top right, bottom left, bottom right) for 4 (independent) pixels:
	//  F  E  D  C    B  A  9  8    7  6  5  4    3  2  1  0
	// BR BL TR TL   BR BL TR TL   BR BL TR TL   BR BL TR TL

	// factorsTopLeft stores the 32 bit interpolation values for 4 pixels:
	// FEDC BA98 7654 3210
	//    3    2    1    0 (32 bit interpolation values, fitting into 16 bit)


	// we will simply extract each channel from the source pixels,
	// each extracted channel will be multiplied by the corresponding interpolation factor
	// and all interpolation results will be accumulated afterwards

	const uint32x4_t m128_maskFirstByte = vdupq_n_u32(0x000000FFu);

	const uint32x4_t m128_muliplicationA = vmulq_u32(vandq_u32(vreinterpretq_u32_u8(m128_pixels), m128_maskFirstByte), m128_factorsTopLeft);
	const uint32x4_t m128_muliplicationB = vmulq_u32(vandq_u32(vshrq_n_u32(vreinterpretq_u32_u8(m128_pixels), 8), m128_maskFirstByte), m128_factorsTopRight);
	const uint32x4_t m128_muliplicationC = vmulq_u32(vandq_u32(vshrq_n_u32(vreinterpretq_u32_u8(m128_pixels), 16), m128_maskFirstByte), m128_factorsBottomLeft);
	const uint32x4_t m128_muliplicationD = vmulq_u32(vandq_u32(vshrq_n_u32(vreinterpretq_u32_u8(m128_pixels), 24), m128_maskFirstByte), m128_factorsBottomRight);

	const uint32x4_t m128_multiplication = vaddq_u32(vaddq_u32(m128_muliplicationA, m128_muliplicationB), vaddq_u32(m128_muliplicationC, m128_muliplicationD));

	// we add 8192 and shift by 14 bits

	const uint8x16_t m128_interpolation = vreinterpretq_u8_u32(vshrq_n_u32(vaddq_u32(m128_multiplication, vdupq_n_u32(8192u)), 14));

	// finally we have the following result:
	// ---C ---8 ---4 ---0
	// and we need to extract the four pixel values:
	//
	// NOTE: Because of a possible bug in Clang affecting ARMv7, vget_lane_u32()
	// seems to assume 32-bit memory alignment for output location, which cannot
	// be guaranteed. This results in bus errors and crashes the application.
	// ARM64 is not affected.
#if defined(__aarch64__)

	const uint8x8_t m64_mask0 = {0, 4, 1, 1, 1, 1, 1, 1};
	const uint8x8_t m64_mask1 = {1, 1, 0, 4, 1, 1, 1, 1};

	const uint8x8_t m64_interpolation01 = vtbl1_u8(vget_low_u8(m128_interpolation), m64_mask0);
	const uint8x8_t m64_interpolation23 = vtbl1_u8(vget_high_u8(m128_interpolation), m64_mask1);

	const uint8x8_t m64_interpolation0123 = vorr_u8(m64_interpolation01, m64_interpolation23);

	const uint32_t result = vget_lane_u32(vreinterpret_u32_u8(m64_interpolation0123), 0);
	memcpy(targetPositionPixels, &result, sizeof(uint32_t));

#else

	*((uint8_t*)targetPositionPixels + 0) = vgetq_lane_u8(m128_interpolation, 0);
	*((uint8_t*)targetPositionPixels + 1) = vgetq_lane_u8(m128_interpolation, 4);
	*((uint8_t*)targetPositionPixels + 2) = vgetq_lane_u8(m128_interpolation, 8);
	*((uint8_t*)targetPositionPixels + 3) = vgetq_lane_u8(m128_interpolation, 12);

#endif
}

OCEAN_FORCE_INLINE void FrameInterpolatorBilinear::interpolate8Pixels1Channel8BitNEON(const uint8x8_t& topLeft_u_8x8, const uint8x8_t& topRight_u_8x8, const uint8x8_t& bottomLeft_u_8x8, const uint8x8_t& bottomRight_u_8x8, const uint8x16_t& factorsRight_factorsBottom_128_u_8x16, uint8_t* targetPositionPixels)
{
	const uint8x16_t factorsLeft_factorsTop_128_u_8x16 = vsubq_u8(vdupq_n_u8(128u), factorsRight_factorsBottom_128_u_8x16); // factorLeft = 128 - factorRight, factorTop = 128 - factorBottomv

	const uint8x8_t factorsRight_u_8x8 = vget_low_u8(factorsRight_factorsBottom_128_u_8x16);
	const uint16x8_t factorsBottom_u_16x8 = vmovl_u8(vget_high_u8(factorsRight_factorsBottom_128_u_8x16));

	const uint8x8_t factorsLeft_u_8x8 = vget_low_u8(factorsLeft_factorsTop_128_u_8x16);
	const uint16x8_t factorsTop_u_16x8 = vmovl_u8(vget_high_u8(factorsLeft_factorsTop_128_u_8x16));

	const uint16x8_t intermediateTop_u_16x8 = vmlal_u8(vmull_u8(topLeft_u_8x8, factorsLeft_u_8x8), topRight_u_8x8, factorsRight_u_8x8); // intermediateTop = topLeft * factorLeft + topRight * factorRight
	const uint16x8_t intermediateBottom_u_16x8 = vmlal_u8(vmull_u8(bottomLeft_u_8x8, factorsLeft_u_8x8), bottomRight_u_8x8, factorsRight_u_8x8); // intermediateBottom = bottomLeft * factorLeft + bottomRight * factorRight

	const uint32x4_t resultA_32x4 = vmlal_u16(vmull_u16(vget_low_u16(intermediateTop_u_16x8), vget_low_u16(factorsTop_u_16x8)), vget_low_u16(intermediateBottom_u_16x8), vget_low_u16(factorsBottom_u_16x8)); // result = intermediateTop * factorTop + intermediateBottom + factorBottom
	const uint32x4_t resultB_32x4 = vmlal_u16(vmull_u16(vget_high_u16(intermediateTop_u_16x8), vget_high_u16(factorsTop_u_16x8)), vget_high_u16(intermediateBottom_u_16x8), vget_high_u16(factorsBottom_u_16x8));

	const uint16x8_t result_16x8 = vcombine_u16(vrshrn_n_u32(resultA_32x4, 14), vrshrn_n_u32(resultB_32x4, 14)); // round(result / 16384.0)

	const uint8x8_t result_8x8 = vmovn_u16(result_16x8);

	vst1_u8(targetPositionPixels, result_8x8);
}

template <>
OCEAN_FORCE_INLINE void FrameInterpolatorBilinear::interpolate4Pixels8BitPerChannelNEON<2u>(const uint8_t* source, const unsigned int offsetsTopLeftElements[4], const unsigned int offsetsTopRightElements[4], const unsigned int offsetsBottomLeftElements[4], const unsigned int offsetsBottomRightElements[4], const unsigned int validPixels[4], const DataType<uint8_t, 2u>::Type& borderColor, const uint32x4_t& m128_factorsRight, const uint32x4_t& m128_factorsBottom, typename DataType<uint8_t, 2u>::Type* targetPositionPixels)
{
	ocean_assert(source != nullptr);
	ocean_assert(targetPositionPixels != nullptr);

	typedef typename DataType<uint8_t, 2u>::Type PixelType;

	// as we do not initialize the following intermediate data,
	// we hopefully will not allocate memory on the stack each time this function is called
	PixelType topPixels[8];
	PixelType bottomPixels[8];

	// we will store the pixel information in the following pattern (here for YA):
	// FE DC  BA 98  76 54  32 10
	// YA YA  YA YA  YA YA  YA YA
	// TR TL  TR TL  TR TL  TR TL

	// we gather the individual source pixel values from the source image,
	// based on the calculated pixel locations
	for (unsigned int i = 0u; i < 4u; ++i)
	{
		if (validPixels[i])
		{
			*(topPixels + i * 2u + 0u) = *((const PixelType*)(source + offsetsTopLeftElements[i]));
			*(topPixels + i * 2u + 1u) = *((const PixelType*)(source + offsetsTopRightElements[i]));
			*(bottomPixels + i * 2u + 0u) = *((const PixelType*)(source + offsetsBottomLeftElements[i]));
			*(bottomPixels + i * 2u + 1u) = *((const PixelType*)(source + offsetsBottomRightElements[i]));
		}
		else
		{
			*(topPixels + i * 2u + 0u) = borderColor;
			*(topPixels + i * 2u + 1u) = borderColor;
			*(bottomPixels + i * 2u + 0u) = borderColor;
			*(bottomPixels + i * 2u + 1u) = borderColor;
		}
	}

	static_assert(sizeof(uint32x4_t) == sizeof(topPixels), "Invalid data type!");

	const uint32x4_t m128_topPixels = vreinterpretq_u32_u8(vld1q_u8((const uint8_t*)topPixels));
	const uint32x4_t m128_bottomPixels = vreinterpretq_u32_u8(vld1q_u8((const uint8_t*)bottomPixels));


	// factorLeft = 128 - factorRight
	// factorTop = 128 - factorBottom

	const uint32x4_t m128_factorsLeft = vsubq_u32(vdupq_n_u32(128u), m128_factorsRight);
	const uint32x4_t m128_factorsTop = vsubq_u32(vdupq_n_u32(128u), m128_factorsBottom);

	// (top_left * factorLeft + top_right * factorRight) * factorTop  +  (bottom_left * factorLeft + bottom_right * factorRight) * factorBottom
	//    == top_left * factorTopLeft + top_right * factorTopRight + bottom_left * factorBottomLeft + bottom_right * factorBottomRight

	const uint32x4_t m128_factorsTopLeft = vmulq_u32(m128_factorsTop, m128_factorsLeft);
	const uint32x4_t m128_factorsTopRight = vmulq_u32(m128_factorsTop, m128_factorsRight);
	const uint32x4_t m128_factorsBottomLeft = vmulq_u32(m128_factorsBottom, m128_factorsLeft);
	const uint32x4_t m128_factorsBottomRight = vmulq_u32(m128_factorsBottom, m128_factorsRight);


	const uint32x4_t m128_maskFirstByte = vdupq_n_u32(0x000000FFu);

	uint32x4_t m128_muliplicationChannel0 = vmulq_u32(vandq_u32(m128_topPixels, m128_maskFirstByte), m128_factorsTopLeft);
	uint32x4_t m128_muliplicationChannel1 = vmulq_u32(vandq_u32(vshrq_n_u32(m128_topPixels, 8), m128_maskFirstByte), m128_factorsTopLeft);

	m128_muliplicationChannel0 = vaddq_u32(m128_muliplicationChannel0, vmulq_u32(vandq_u32(vshrq_n_u32(m128_topPixels, 16), m128_maskFirstByte), m128_factorsTopRight));
	m128_muliplicationChannel1 = vaddq_u32(m128_muliplicationChannel1, vmulq_u32(vandq_u32(vshrq_n_u32(m128_topPixels, 24), m128_maskFirstByte), m128_factorsTopRight));

	m128_muliplicationChannel0 = vaddq_u32(m128_muliplicationChannel0, vmulq_u32(vandq_u32(m128_bottomPixels, m128_maskFirstByte), m128_factorsBottomLeft));
	m128_muliplicationChannel1 = vaddq_u32(m128_muliplicationChannel1, vmulq_u32(vandq_u32(vshrq_n_u32(m128_bottomPixels, 8), m128_maskFirstByte), m128_factorsBottomLeft));

	m128_muliplicationChannel0 = vaddq_u32(m128_muliplicationChannel0, vmulq_u32(vandq_u32(vshrq_n_u32(m128_bottomPixels, 16), m128_maskFirstByte), m128_factorsBottomRight));
	m128_muliplicationChannel1 = vaddq_u32(m128_muliplicationChannel1, vmulq_u32(vandq_u32(vshrq_n_u32(m128_bottomPixels, 24), m128_maskFirstByte), m128_factorsBottomRight));


	// we add 8192 and shift by 14 bits

	const uint32x4_t m128_interpolation0 = vshrq_n_u32(vaddq_u32(m128_muliplicationChannel0, vdupq_n_u32(8192u)), 14);
	const uint32x4_t m128_interpolation1 = vshrq_n_u32(vaddq_u32(m128_muliplicationChannel1, vdupq_n_u32(8192u)), 14);

	// finaly we blend the interpolation results together to get the following pattern:
	// FE DC  BA 98  76 54  32 10
	// 00 YA  00 YA  00 YA  00 YA

	const uint32x4_t m128_interpolation = vorrq_u32(m128_interpolation0, vshlq_n_u32(m128_interpolation1, 8));

	// we shuffle the 128 bit register to a 64 bit register:

	const uint8x8_t m64_mask0 = {0, 1, 4, 5, 2, 2, 2, 2};
	const uint8x8_t m64_mask1 = {2, 2, 2, 2, 0, 1, 4, 5};

	const uint8x8_t m64_interpolation_low = vtbl1_u8(vget_low_u8(vreinterpretq_u8_u32(m128_interpolation)), m64_mask0);
	const uint8x8_t m64_interpolation_high = vtbl1_u8(vget_high_u8(vreinterpretq_u8_u32(m128_interpolation)), m64_mask1);

	const uint8x8_t m64_interpolation = vorr_u8(m64_interpolation_low, m64_interpolation_high);

	// no we can store the following pattern as one block:

	// 76 54  32 10
	// YA YA  YA YA

	vst1_u8((uint8_t*)targetPositionPixels, m64_interpolation);
}

template <>
OCEAN_FORCE_INLINE void FrameInterpolatorBilinear::interpolate4Pixels8BitPerChannelNEON<3u>(const uint8_t* source, const unsigned int offsetsTopLeftElements[4], const unsigned int offsetsTopRightElements[4], const unsigned int offsetsBottomLeftElements[4], const unsigned int offsetsBottomRightElements[4], const unsigned int validPixels[4], const DataType<uint8_t, 3u>::Type& borderColor, const uint32x4_t& m128_factorsRight, const uint32x4_t& m128_factorsBottom, typename DataType<uint8_t, 3u>::Type* targetPositionPixels)
{
	ocean_assert(source != nullptr);
	ocean_assert(targetPositionPixels != nullptr);

	// as we do not initialize the following intermediate data,
	// we hopefully will not allocate memory on the stack each time this function is called
	uint32_t topLeftPixels[4];
	uint32_t topRightPixels[4];
	uint32_t bottomLeftPixels[4];
	uint32_t bottomRightPixels[4];

	// we will store the pixel information in the following pattern, note the padding byte after each pixel (here for RGB):
	// FEDCBA9876543210
	//  BGR BGR BGR BGR

	// we gather the individual source pixel values from the source image,
	// based on the calculated pixel locations
	for (unsigned int i = 0u; i < 4u; ++i)
	{
		if (validPixels[i])
		{
			memcpy(topLeftPixels + i, source + offsetsTopLeftElements[i], sizeof(uint8_t) * 3);
			memcpy(topRightPixels + i, source + offsetsTopRightElements[i], sizeof(uint8_t) * 3);
			memcpy(bottomLeftPixels + i, source + offsetsBottomLeftElements[i], sizeof(uint8_t) * 3);
			memcpy(bottomRightPixels + i, source + offsetsBottomRightElements[i], sizeof(uint8_t) * 3);
		}
		else
		{
			memcpy(topLeftPixels + i, &borderColor, sizeof(uint8_t) * 3);
			memcpy(topRightPixels + i, &borderColor, sizeof(uint8_t) * 3);
			memcpy(bottomLeftPixels + i, &borderColor, sizeof(uint8_t) * 3);
			memcpy(bottomRightPixels + i, &borderColor, sizeof(uint8_t) * 3);
		}
	}

	static_assert(sizeof(uint32x4_t) == sizeof(topLeftPixels), "Invalid data type!");

	const uint32x4_t m128_topLeftPixels = vld1q_u32(topLeftPixels);
	const uint32x4_t m128_topRightPixels = vld1q_u32(topRightPixels);
	const uint32x4_t m128_bottomLeftPixels = vld1q_u32(bottomLeftPixels);
	const uint32x4_t m128_bottomRightPixels = vld1q_u32(bottomRightPixels);


	// factorLeft = 128 - factorRight
	// factorTop = 128 - factorBottom

	const uint32x4_t m128_factorsLeft = vsubq_u32(vdupq_n_u32(128u), m128_factorsRight);
	const uint32x4_t m128_factorsTop = vsubq_u32(vdupq_n_u32(128u), m128_factorsBottom);

	// (top_left * factorLeft + top_right * factorRight) * factorTop  +  (bottom_left * factorLeft + bottom_right * factorRight) * factorBottom
	//    == top_left * factorTopLeft + top_right * factorTopRight + bottom_left * factorBottomLeft + bottom_right * factorBottomRight

	const uint32x4_t m128_factorsTopLeft = vmulq_u32(m128_factorsTop, m128_factorsLeft);
	const uint32x4_t m128_factorsTopRight = vmulq_u32(m128_factorsTop, m128_factorsRight);
	const uint32x4_t m128_factorsBottomLeft = vmulq_u32(m128_factorsBottom, m128_factorsLeft);
	const uint32x4_t m128_factorsBottomRight = vmulq_u32(m128_factorsBottom, m128_factorsRight);


	const uint32x4_t m128_maskFirstByte = vdupq_n_u32(0x000000FFu);

	uint32x4_t m128_muliplicationChannel0 = vmulq_u32(vandq_u32(m128_topLeftPixels, m128_maskFirstByte), m128_factorsTopLeft);
	uint32x4_t m128_muliplicationChannel1 = vmulq_u32(vandq_u32(vshrq_n_u32(m128_topLeftPixels, 8), m128_maskFirstByte), m128_factorsTopLeft);
	uint32x4_t m128_muliplicationChannel2 = vmulq_u32(vandq_u32(vshrq_n_u32(m128_topLeftPixels, 16), m128_maskFirstByte), m128_factorsTopLeft);

	m128_muliplicationChannel0 = vaddq_u32(m128_muliplicationChannel0, vmulq_u32(vandq_u32(m128_topRightPixels, m128_maskFirstByte), m128_factorsTopRight));
	m128_muliplicationChannel1 = vaddq_u32(m128_muliplicationChannel1, vmulq_u32(vandq_u32(vshrq_n_u32(m128_topRightPixels, 8), m128_maskFirstByte), m128_factorsTopRight));
	m128_muliplicationChannel2 = vaddq_u32(m128_muliplicationChannel2, vmulq_u32(vandq_u32(vshrq_n_u32(m128_topRightPixels, 16), m128_maskFirstByte), m128_factorsTopRight));

	m128_muliplicationChannel0 = vaddq_u32(m128_muliplicationChannel0, vmulq_u32(vandq_u32(m128_bottomLeftPixels, m128_maskFirstByte), m128_factorsBottomLeft));
	m128_muliplicationChannel1 = vaddq_u32(m128_muliplicationChannel1, vmulq_u32(vandq_u32(vshrq_n_u32(m128_bottomLeftPixels, 8), m128_maskFirstByte), m128_factorsBottomLeft));
	m128_muliplicationChannel2 = vaddq_u32(m128_muliplicationChannel2, vmulq_u32(vandq_u32(vshrq_n_u32(m128_bottomLeftPixels, 16), m128_maskFirstByte), m128_factorsBottomLeft));

	m128_muliplicationChannel0 = vaddq_u32(m128_muliplicationChannel0, vmulq_u32(vandq_u32(m128_bottomRightPixels, m128_maskFirstByte), m128_factorsBottomRight));
	m128_muliplicationChannel1 = vaddq_u32(m128_muliplicationChannel1, vmulq_u32(vandq_u32(vshrq_n_u32(m128_bottomRightPixels, 8), m128_maskFirstByte), m128_factorsBottomRight));
	m128_muliplicationChannel2 = vaddq_u32(m128_muliplicationChannel2, vmulq_u32(vandq_u32(vshrq_n_u32(m128_bottomRightPixels, 16), m128_maskFirstByte), m128_factorsBottomRight));


	// we add 8192 and shift by 14 bits

	const uint32x4_t m128_interpolation0 = vshrq_n_u32(vaddq_u32(m128_muliplicationChannel0, vdupq_n_u32(8192u)), 14);
	const uint32x4_t m128_interpolation1 = vshrq_n_u32(vaddq_u32(m128_muliplicationChannel1, vdupq_n_u32(8192u)), 14);
	const uint32x4_t m128_interpolation2 = vshrq_n_u32(vaddq_u32(m128_muliplicationChannel2, vdupq_n_u32(8192u)), 14);

	// finaly we blend the interpolation results together

	const uint32x4_t m128_interpolation = vorrq_u32(vorrq_u32(m128_interpolation0, vshlq_n_u32(m128_interpolation1, 8)), vshlq_n_u32(m128_interpolation2, 16));

	// we have to extract the get rid of the padding byte:
	// FEDCBA9876543210
	//  BGR BGR BGR BGR

	uint32_t intermediateBuffer[4];
	vst1q_u32(intermediateBuffer, m128_interpolation);

	for (unsigned int i = 0u; i < 4u; ++i)
	{
		memcpy(targetPositionPixels + i, intermediateBuffer + i, sizeof(uint8_t) * 3);
	}
}

template <>
OCEAN_FORCE_INLINE void FrameInterpolatorBilinear::interpolate4Pixels8BitPerChannelNEON<4u>(const uint8_t* source, const unsigned int offsetsTopLeftElements[4], const unsigned int offsetsTopRightElements[4], const unsigned int offsetsBottomLeftElements[4], const unsigned int offsetsBottomRightElements[4], const unsigned int validPixels[4], const DataType<uint8_t, 4u>::Type& borderColor, const uint32x4_t& m128_factorsRight, const uint32x4_t& m128_factorsBottom, typename DataType<uint8_t, 4u>::Type* targetPositionPixels)
{
	ocean_assert(source != nullptr);
	ocean_assert(targetPositionPixels != nullptr);

	typedef typename DataType<uint8_t, 4u>::Type PixelType;

	// as we do not initialize the following intermediate data,
	// we hopefully will not allocate memory on the stack each time this function is called
	PixelType topLeftPixels[4];
	PixelType topRightPixels[4];
	PixelType bottomLeftPixels[4];
	PixelType bottomRightPixels[4];

	// we will store the pixel information in the following pattern (here for RGBA):
	// FEDC BA98 7654 3210
	// ABGR ABGR ABGR ABGR

	// we gather the individual source pixel values from the source image,
	// based on the calculated pixel locations
	for (unsigned int i = 0u; i < 4u; ++i)
	{
		if (validPixels[i])
		{
			*(topLeftPixels + i) = *((const PixelType*)(source + offsetsTopLeftElements[i]));
			*(topRightPixels + i) = *((const PixelType*)(source + offsetsTopRightElements[i]));
			*(bottomLeftPixels + i) = *((const PixelType*)(source + offsetsBottomLeftElements[i]));
			*(bottomRightPixels + i) = *((const PixelType*)(source + offsetsBottomRightElements[i]));
		}
		else
		{
			*(topLeftPixels + i) = borderColor;
			*(topRightPixels + i) = borderColor;
			*(bottomLeftPixels + i) = borderColor;
			*(bottomRightPixels + i) = borderColor;
		}
	}

	static_assert(sizeof(uint32x4_t) == sizeof(topLeftPixels), "Invalid data type!");

	const uint32x4_t m128_topLeftPixels = vreinterpretq_u32_u8(vld1q_u8((const uint8_t*)topLeftPixels));
	const uint32x4_t m128_topRightPixels = vreinterpretq_u32_u8(vld1q_u8((const uint8_t*)topRightPixels));
	const uint32x4_t m128_bottomLeftPixels = vreinterpretq_u32_u8(vld1q_u8((const uint8_t*)bottomLeftPixels));
	const uint32x4_t m128_bottomRightPixels = vreinterpretq_u32_u8(vld1q_u8((const uint8_t*)bottomRightPixels));


	// factorLeft = 128 - factorRight
	// factorTop = 128 - factorBottom

	const uint32x4_t m128_factorsLeft = vsubq_u32(vdupq_n_u32(128u), m128_factorsRight);
	const uint32x4_t m128_factorsTop = vsubq_u32(vdupq_n_u32(128u), m128_factorsBottom);

	// (top_left * factorLeft + top_right * factorRight) * factorTop  +  (bottom_left * factorLeft + bottom_right * factorRight) * factorBottom
	//    == top_left * factorTopLeft + top_right * factorTopRight + bottom_left * factorBottomLeft + bottom_right * factorBottomRight

	const uint32x4_t m128_factorsTopLeft = vmulq_u32(m128_factorsTop, m128_factorsLeft);
	const uint32x4_t m128_factorsTopRight = vmulq_u32(m128_factorsTop, m128_factorsRight);
	const uint32x4_t m128_factorsBottomLeft = vmulq_u32(m128_factorsBottom, m128_factorsLeft);
	const uint32x4_t m128_factorsBottomRight = vmulq_u32(m128_factorsBottom, m128_factorsRight);


	const uint32x4_t m128_maskFirstByte = vdupq_n_u32(0x000000FFu);

	uint32x4_t m128_muliplicationChannel0 = vmulq_u32(vandq_u32(m128_topLeftPixels, m128_maskFirstByte), m128_factorsTopLeft);
	uint32x4_t m128_muliplicationChannel1 = vmulq_u32(vandq_u32(vshrq_n_u32(m128_topLeftPixels, 8), m128_maskFirstByte), m128_factorsTopLeft);
	uint32x4_t m128_muliplicationChannel2 = vmulq_u32(vandq_u32(vshrq_n_u32(m128_topLeftPixels, 16), m128_maskFirstByte), m128_factorsTopLeft);
	uint32x4_t m128_muliplicationChannel3 = vmulq_u32(vandq_u32(vshrq_n_u32(m128_topLeftPixels, 24), m128_maskFirstByte), m128_factorsTopLeft);

	m128_muliplicationChannel0 = vaddq_u32(m128_muliplicationChannel0, vmulq_u32(vandq_u32(m128_topRightPixels, m128_maskFirstByte), m128_factorsTopRight));
	m128_muliplicationChannel1 = vaddq_u32(m128_muliplicationChannel1, vmulq_u32(vandq_u32(vshrq_n_u32(m128_topRightPixels, 8), m128_maskFirstByte), m128_factorsTopRight));
	m128_muliplicationChannel2 = vaddq_u32(m128_muliplicationChannel2, vmulq_u32(vandq_u32(vshrq_n_u32(m128_topRightPixels, 16), m128_maskFirstByte), m128_factorsTopRight));
	m128_muliplicationChannel3 = vaddq_u32(m128_muliplicationChannel3, vmulq_u32(vandq_u32(vshrq_n_u32(m128_topRightPixels, 24), m128_maskFirstByte), m128_factorsTopRight));

	m128_muliplicationChannel0 = vaddq_u32(m128_muliplicationChannel0, vmulq_u32(vandq_u32(m128_bottomLeftPixels, m128_maskFirstByte), m128_factorsBottomLeft));
	m128_muliplicationChannel1 = vaddq_u32(m128_muliplicationChannel1, vmulq_u32(vandq_u32(vshrq_n_u32(m128_bottomLeftPixels, 8), m128_maskFirstByte), m128_factorsBottomLeft));
	m128_muliplicationChannel2 = vaddq_u32(m128_muliplicationChannel2, vmulq_u32(vandq_u32(vshrq_n_u32(m128_bottomLeftPixels, 16), m128_maskFirstByte), m128_factorsBottomLeft));
	m128_muliplicationChannel3 = vaddq_u32(m128_muliplicationChannel3, vmulq_u32(vandq_u32(vshrq_n_u32(m128_bottomLeftPixels, 24), m128_maskFirstByte), m128_factorsBottomLeft));

	m128_muliplicationChannel0 = vaddq_u32(m128_muliplicationChannel0, vmulq_u32(vandq_u32(m128_bottomRightPixels, m128_maskFirstByte), m128_factorsBottomRight));
	m128_muliplicationChannel1 = vaddq_u32(m128_muliplicationChannel1, vmulq_u32(vandq_u32(vshrq_n_u32(m128_bottomRightPixels, 8), m128_maskFirstByte), m128_factorsBottomRight));
	m128_muliplicationChannel2 = vaddq_u32(m128_muliplicationChannel2, vmulq_u32(vandq_u32(vshrq_n_u32(m128_bottomRightPixels, 16), m128_maskFirstByte), m128_factorsBottomRight));
	m128_muliplicationChannel3 = vaddq_u32(m128_muliplicationChannel3, vmulq_u32(vandq_u32(vshrq_n_u32(m128_bottomRightPixels, 24), m128_maskFirstByte), m128_factorsBottomRight));


	// we add 8192 and shift by 14 bits

	const uint32x4_t m128_interpolation0 = vshrq_n_u32(vaddq_u32(m128_muliplicationChannel0, vdupq_n_u32(8192u)), 14);
	const uint32x4_t m128_interpolation1 = vshrq_n_u32(vaddq_u32(m128_muliplicationChannel1, vdupq_n_u32(8192u)), 14);
	const uint32x4_t m128_interpolation2 = vshrq_n_u32(vaddq_u32(m128_muliplicationChannel2, vdupq_n_u32(8192u)), 14);
	const uint32x4_t m128_interpolation3 = vshrq_n_u32(vaddq_u32(m128_muliplicationChannel3, vdupq_n_u32(8192u)), 14);

	// finaly we blend the interpolation results together

	const uint32x4_t m128_interpolation = vorrq_u32(vorrq_u32(m128_interpolation0, vshlq_n_u32(m128_interpolation1, 8)), vorrq_u32(vshlq_n_u32(m128_interpolation2, 16), vshlq_n_u32(m128_interpolation3, 24)));

	vst1q_u8((uint8_t*)targetPositionPixels, vreinterpretq_u8_u32(m128_interpolation));
}

template <unsigned int tChannels>
OCEAN_FORCE_INLINE void FrameInterpolatorBilinear::interpolate4Pixels8BitPerChannelNEON(const uint8_t* source, const unsigned int offsetsTopLeftElements[4], const unsigned int offsetsTopRightElements[4], const unsigned int offsetsBottomLeftElements[4], const unsigned int offsetsBottomRightElements[4], const unsigned int validPixels[4], const typename DataType<uint8_t, tChannels>::Type& borderColor, const uint32x4_t& m128_factorsRight, const uint32x4_t& m128_factorsBottom, typename DataType<uint8_t, tChannels>::Type* targetPositionPixels)
{
	ocean_assert(source != nullptr);
	ocean_assert(targetPositionPixels != nullptr);

	// as we do not initialize the following intermediate data,
	// we hopefully will not allocate memory on the stack each time this function is called
	unsigned int factorsTopLeft[4];
	unsigned int factorsTopRight[4];
	unsigned int factorsBottomLeft[4];
	unsigned int factorsBottomRight[4];


	// factorLeft = 128 - factorRight
	// factorTop = 128 - factorBottom

	const uint32x4_t m128_factorsLeft = vsubq_u32(vdupq_n_u32(128u), m128_factorsRight);
	const uint32x4_t m128_factorsTop = vsubq_u32(vdupq_n_u32(128u), m128_factorsBottom);

	// (top_left * factorLeft + top_right * factorRight) * factorTop  +  (bottom_left * factorLeft + bottom_right * factorRight) * factorBottom
	//    == top_left * factorTopLeft + top_right * factorTopRight + bottom_left * factorBottomLeft + bottom_right * factorBottomRight

	const uint32x4_t m128_factorsTopLeft = vmulq_u32(m128_factorsTop, m128_factorsLeft);
	const uint32x4_t m128_factorsTopRight = vmulq_u32(m128_factorsTop, m128_factorsRight);
	const uint32x4_t m128_factorsBottomLeft = vmulq_u32(m128_factorsBottom, m128_factorsLeft);
	const uint32x4_t m128_factorsBottomRight = vmulq_u32(m128_factorsBottom, m128_factorsRight);


	// we store the interpolation factors
	vst1q_u32(factorsTopLeft, m128_factorsTopLeft);
	vst1q_u32(factorsTopRight, m128_factorsTopRight);
	vst1q_u32(factorsBottomLeft, m128_factorsBottomLeft);
	vst1q_u32(factorsBottomRight, m128_factorsBottomRight);

	for (unsigned int i = 0u; i < 4u; ++i)
	{
		if (validPixels[i])
		{
			const uint8_t* topLeft = source + offsetsTopLeftElements[i];
			const uint8_t* topRight = source + offsetsTopRightElements[i];

			const uint8_t* bottomLeft = source + offsetsBottomLeftElements[i];
			const uint8_t* bottomRight = source + offsetsBottomRightElements[i];

			const unsigned int& factorTopLeft = factorsTopLeft[i];
			const unsigned int& factorTopRight = factorsTopRight[i];
			const unsigned int& factorBottomLeft = factorsBottomLeft[i];
			const unsigned int& factorBottomRight = factorsBottomRight[i];

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				((uint8_t*)targetPositionPixels)[n] = (topLeft[n] * factorTopLeft + topRight[n] * factorTopRight + bottomLeft[n] * factorBottomLeft + bottomRight[n] * factorBottomRight + 8192u) >> 14u;
			}
		}
		else
		{
			*targetPositionPixels = borderColor;
		}

		targetPositionPixels++;
	}
}

#endif // OCEAN_HARDWARE_NEON_VERSION

template <unsigned int tChannels>
inline void FrameInterpolatorBilinear::homographies8BitPerChannelSubset(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const SquareMatrix3* homographies, const uint8_t* borderColor, uint8_t* output, const Scalar outputQuadrantCenterX, const Scalar outputQuadrantCenterY, const int outputOriginX, const int outputOriginY, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, const unsigned int firstOutputRow, const unsigned int numberOutputRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(input && output);
	ocean_assert(inputWidth > 0u && inputHeight > 0u);
	ocean_assert(outputWidth > 0u && outputHeight > 0u);

	ocean_assert(outputQuadrantCenterX >= 0 && outputQuadrantCenterX < Scalar(outputWidth));
	ocean_assert(outputQuadrantCenterY >= 0 && outputQuadrantCenterY < Scalar(outputHeight));
	ocean_assert(homographies);

	const unsigned int outputStrideElements = outputWidth * tChannels + outputPaddingElements;

	const Scalar scalarInputWidth_1 = Scalar(inputWidth - 1u);
	const Scalar scalarInputHeight_1 = Scalar(inputHeight - 1u);

	constexpr uint8_t zeroColor[tChannels] = {uint8_t(0)};
	const uint8_t* const bColor = borderColor ? borderColor : zeroColor;

	uint8_t* outputData = output + firstOutputRow * outputStrideElements;

	const Scalar left = Scalar(outputQuadrantCenterX) * Scalar(0.5);
	const Scalar right = (Scalar(outputWidth) + Scalar(outputQuadrantCenterX)) * Scalar(0.5);

	const Scalar top = Scalar(outputQuadrantCenterY) * Scalar(0.5);
	const Scalar bottom = (Scalar(outputHeight) + Scalar(outputQuadrantCenterY)) * Scalar(0.5);

	ocean_assert(right - left > Numeric::eps());
	ocean_assert(bottom - top > Numeric::eps());

	const Scalar invWidth = Scalar(1) / Scalar(right - left);
	const Scalar invHeight = Scalar(1) / Scalar(bottom - top);

	for (unsigned int y = firstOutputRow; y < firstOutputRow + numberOutputRows; ++y)
	{
		for (unsigned int x = 0; x < outputWidth; ++x)
		{
			Vector2 outputPosition = Vector2(Scalar(int(x)), Scalar(int(y)));

			const Scalar _tx = minmax<Scalar>(0, (outputPosition.x() - left) * invWidth, 1);
			const Scalar _ty = minmax<Scalar>(0, (outputPosition.y() - top) * invHeight, 1);

			outputPosition += Vector2(Scalar(outputOriginX), Scalar(outputOriginY));

			const Scalar tx = 1 - _tx;
			const Scalar ty = 1 - _ty;

			const Vector2 inputPositionTopLeft(homographies[0] * outputPosition);
			const Vector2 inputPositionTopRight(homographies[1] * outputPosition);
			const Vector2 inputPositionBottomLeft(homographies[2] * outputPosition);
			const Vector2 inputPositionBottomRight(homographies[3] * outputPosition);

			const Scalar tTopLeft = tx * ty;
			const Scalar tTopRight = _tx * ty;
			const Scalar tBottomLeft = tx * _ty;
			const Scalar tBottomRight = _tx * _ty;

			const Vector2 inputPosition = inputPositionTopLeft * tTopLeft + inputPositionTopRight * tTopRight
												+ inputPositionBottomLeft * tBottomLeft + inputPositionBottomRight * tBottomRight;

			if (inputPosition.x() < Scalar(0) || inputPosition.x() > scalarInputWidth_1 || inputPosition.y() < Scalar(0) || inputPosition.y() > scalarInputHeight_1)
			{
				for (unsigned int c = 0u; c < tChannels; ++c)
				{
					outputData[c] = bColor[c];
				}
			}
			else
			{
				interpolatePixel8BitPerChannel<tChannels, PC_TOP_LEFT>(input, inputWidth, inputHeight, inputPaddingElements, inputPosition, outputData);
			}

			outputData += tChannels;
		}

		outputData += outputPaddingElements;
	}
}

template <unsigned int tChannels>
void FrameInterpolatorBilinear::homographyMask8BitPerChannelSubset(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const SquareMatrix3* input_H_output, uint8_t* output, uint8_t* outputMask, const uint8_t maskValue, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, const unsigned int outputMaskPaddingElements, unsigned int firstOutputRow, const unsigned int numberOutputRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(input != nullptr && output != nullptr);
	ocean_assert(inputWidth > 0u && inputHeight > 0u);
	ocean_assert(outputWidth > 0u && outputHeight > 0u);
	ocean_assert(input_H_output != nullptr);

	ocean_assert_and_suppress_unused(firstOutputRow + numberOutputRows <= outputHeight, outputHeight);

	const unsigned int outputStrideElements = outputWidth * tChannels + outputPaddingElements;
	const unsigned int outputMaskStrideElements = outputWidth + outputMaskPaddingElements;

	const Scalar scalarInputWidth_1 = Scalar(inputWidth - 1u);
	const Scalar scalarInputHeight_1 = Scalar(inputHeight - 1u);

	typedef typename DataType<uint8_t, tChannels>::Type PixelType;

	for (unsigned int y = firstOutputRow; y < firstOutputRow + numberOutputRows; ++y)
	{
		PixelType* outputData = (PixelType*)(output + y * outputStrideElements);
		uint8_t* outputMaskData = outputMask + y * outputMaskStrideElements;

		/*
		 * We can slightly optimize the 3x3 matrix multiplication:
		 *
		 * | X0 Y0 Z0 |   | x |
		 * | X1 Y1 Z1 | * | y |
		 * | X2 Y2 Z2 |   | 1 |
		 *
		 * | x' |   | X0 * x |   | Y0 * y + Z0 |
		 * | y' | = | X1 * x | + | Y1 * y + Z1 |
		 * | z' |   | X2 * x |   | Y2 * y + Z2 |
		 *
		 * As y is constant within the inner loop, we can pre-calculate the following terms:
		 *
		 * | x' |   | (X0 * x + constValue0) / (X2 * x + constValue2) |
		 * | y' | = | (X1 * x + constValue1) / (X2 * x + constValue2) |
		 *
		 * | p | = | (X * x + c) / (X2 * x + constValue2) |
		 */

		const Vector2 X(input_H_output->data() + 0);
		const Vector2 c(Vector2(input_H_output->data() + 3) * Scalar(y) + Vector2(input_H_output->data() + 6));

		const Scalar X2 = (*input_H_output)(2, 0);
		const Scalar constValue2 = (*input_H_output)(2, 1) * Scalar(y) + (*input_H_output)(2, 2);

		for (unsigned int x = 0; x < outputWidth; ++x)
		{
			const Vector2 inputPosition((X * Scalar(x) + c) / (X2 * Scalar(x) + constValue2));

#ifdef OCEAN_DEBUG
			const Vector2 debugInputPosition(*input_H_output * Vector2(Scalar(x), Scalar(y)));
			ocean_assert(inputPosition.isEqual(debugInputPosition, Scalar(0.01)));
#endif

			if (inputPosition.x() < Scalar(0) || inputPosition.x() > scalarInputWidth_1 || inputPosition.y() < Scalar(0) || inputPosition.y() > scalarInputHeight_1)
			{
				*outputMaskData = 0xFF - maskValue;
			}
			else
			{
				interpolatePixel8BitPerChannel<tChannels, PC_TOP_LEFT>(input, inputWidth, inputHeight, inputPaddingElements, inputPosition, (uint8_t*)(outputData));
				*outputMaskData = maskValue;
			}

			outputData++;
			outputMaskData++;
		}
	}
}

template <unsigned int tChannels>
inline void FrameInterpolatorBilinear::homographiesMask8BitPerChannelSubset(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const SquareMatrix3* homographies, uint8_t* output, uint8_t* outputMask, const uint8_t maskValue, const Scalar outputQuadrantCenterX, const Scalar outputQuadrantCenterY, const int outputOriginX, const int outputOriginY, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, const unsigned int outputMaskPaddingElements, const unsigned int firstOutputRow, const unsigned int numberOutputRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(input && output);
	ocean_assert(inputWidth > 0u && inputHeight > 0u);
	ocean_assert(outputWidth > 0u && outputHeight > 0u);

	ocean_assert(outputQuadrantCenterX >= 0 && outputQuadrantCenterX < Scalar(outputWidth));
	ocean_assert(outputQuadrantCenterY >= 0 && outputQuadrantCenterY < Scalar(outputHeight));
	ocean_assert(homographies);

	const unsigned int outputStrideElements = tChannels * outputWidth + outputPaddingElements;
	const unsigned int outputMaskStrideElements = outputWidth + outputMaskPaddingElements;

	const Scalar scalarInputWidth_1 = Scalar(inputWidth - 1u);
	const Scalar scalarInputHeight_1 = Scalar(inputHeight - 1u);

	uint8_t* outputData = output + firstOutputRow * outputStrideElements;
	outputMask += firstOutputRow * outputMaskStrideElements;

	const Scalar left = Scalar(outputQuadrantCenterX) * Scalar(0.5);
	const Scalar right = (Scalar(outputWidth) + Scalar(outputQuadrantCenterX)) * Scalar(0.5);

	const Scalar top = Scalar(outputQuadrantCenterY) * Scalar(0.5);
	const Scalar bottom = (Scalar(outputHeight) + Scalar(outputQuadrantCenterY)) * Scalar(0.5);

	ocean_assert(right - left > Numeric::eps());
	ocean_assert(bottom - top > Numeric::eps());

	const Scalar invWidth = Scalar(1) / Scalar(right - left);
	const Scalar invHeight = Scalar(1) / Scalar(bottom - top);

	for (unsigned int y = firstOutputRow; y < firstOutputRow + numberOutputRows; ++y)
	{
		for (unsigned int x = 0u; x < outputWidth; ++x)
		{
			Vector2 outputPosition = Vector2(Scalar(int(x)), Scalar(int(y)));

			const Scalar _tx = minmax<Scalar>(0, (outputPosition.x() - left) * invWidth, 1);
			const Scalar _ty = minmax<Scalar>(0, (outputPosition.y() - top) * invHeight, 1);

			outputPosition += Vector2(Scalar(outputOriginX), Scalar(outputOriginY));

			const Scalar tx = 1 - _tx;
			const Scalar ty = 1 - _ty;

			const Vector2 inputPositionTopLeft(homographies[0] * outputPosition);
			const Vector2 inputPositionTopRight(homographies[1] * outputPosition);
			const Vector2 inputPositionBottomLeft(homographies[2] * outputPosition);
			const Vector2 inputPositionBottomRight(homographies[3] * outputPosition);

			const Scalar tTopLeft = tx * ty;
			const Scalar tTopRight = _tx * ty;
			const Scalar tBottomLeft = tx * _ty;
			const Scalar tBottomRight = _tx * _ty;

			const Vector2 inputPosition = inputPositionTopLeft * tTopLeft + inputPositionTopRight * tTopRight
												+ inputPositionBottomLeft * tBottomLeft + inputPositionBottomRight * tBottomRight;

			if (inputPosition.x() < Scalar(0) || inputPosition.x() > scalarInputWidth_1 || inputPosition.y() < Scalar(0) || inputPosition.y() > scalarInputHeight_1)
			{
				*outputMask = 0xFFu - maskValue;
			}
			else
			{
				interpolatePixel8BitPerChannel<tChannels, PC_TOP_LEFT>(input, inputWidth, inputHeight, inputPaddingElements, inputPosition, outputData);
				*outputMask = maskValue;
			}

			outputData += tChannels;
			outputMask++;
		}

		outputData += outputPaddingElements;
		outputMask += outputMaskPaddingElements;
	}
}

template <unsigned int tChannels>
void FrameInterpolatorBilinear::homographyWithCamera8BitPerChannelSubset(const PinholeCamera* inputCamera, const PinholeCamera* outputCamera, const PinholeCamera::DistortionLookup* outputCameraDistortionLookup, const uint8_t* input, const SquareMatrix3* normalizedHomography, const bool useDistortionParameters, const uint8_t* borderColor, uint8_t* output, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(inputCamera && outputCamera && normalizedHomography);
	ocean_assert(input && output);

	ocean_assert(firstRow + numberRows <= outputCamera->height());

	const unsigned int outputStrideElements = tChannels * outputCamera->width() + outputPaddingElements;

	const Scalar scalarInputWidth_1 = Scalar(inputCamera->width() - 1u);
	const Scalar scalarInputHeight_1 = Scalar(inputCamera->height() - 1u);

	const SquareMatrix3 combinedMatrix(*normalizedHomography * outputCamera->invertedIntrinsic());

	typedef typename DataType<uint8_t, tChannels>::Type PixelType;

	const uint8_t zeroColor[tChannels] = {uint8_t(0)};
	const PixelType* const bColor = borderColor ? (PixelType*)borderColor : (PixelType*)zeroColor;

	uint8_t* outputData = output + firstRow * outputStrideElements;

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		for (unsigned int x = 0; x < outputCamera->width(); ++x)
		{
			const Vector2 inputPosition(inputCamera->normalizedImagePoint2imagePoint<true>(combinedMatrix * outputCameraDistortionLookup->undistortedImagePoint(Vector2(Scalar(x), Scalar(y))), useDistortionParameters));

			if (inputPosition.x() < Scalar(0) || inputPosition.x() > scalarInputWidth_1 || inputPosition.y() < Scalar(0) || inputPosition.y() > scalarInputHeight_1)
			{
				*((PixelType*)outputData) = *bColor;
			}
			else
			{
				interpolatePixel8BitPerChannel<tChannels, PC_TOP_LEFT>(input, inputCamera->width(), inputCamera->height(), inputPaddingElements, inputPosition, outputData);
			}

			outputData += tChannels;
		}

		outputData += outputPaddingElements;
	}
}

template <unsigned int tChannels>
void FrameInterpolatorBilinear::homographyWithCameraMask8BitPerChannelSubset(const PinholeCamera* inputCamera, const PinholeCamera* outputCamera, const PinholeCamera::DistortionLookup* outputCameraDistortionLookup, const uint8_t* input, const unsigned int inputPaddingElements, const SquareMatrix3* normalizedHomography, uint8_t* output, uint8_t* outputMask, const unsigned int outputPaddingElements, const unsigned int outputMaskPaddingElements, const uint8_t maskValue, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(inputCamera != nullptr && outputCamera != nullptr && normalizedHomography != nullptr);
	ocean_assert(input != nullptr && output != nullptr);

	ocean_assert(firstRow + numberRows <= outputCamera->height());

	const unsigned int outputStrideElements = outputCamera->width() * tChannels + outputPaddingElements;
	const unsigned int outputMaskStrideElements = outputCamera->width() + outputMaskPaddingElements;

	const Scalar scalarInputWidth_1 = Scalar(inputCamera->width() - 1u);
	const Scalar scalarInputHeight_1 = Scalar(inputCamera->height() - 1u);

	const SquareMatrix3 combinedMatrix(*normalizedHomography * outputCamera->invertedIntrinsic());

	uint8_t* outputData = output + firstRow * outputStrideElements;
	outputMask += firstRow * outputMaskStrideElements;

	constexpr bool useDistortionParameters = true;

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		for (unsigned int x = 0; x < outputCamera->width(); ++x)
		{
			const Vector2 inputPosition(inputCamera->normalizedImagePoint2imagePoint<true>(combinedMatrix * outputCameraDistortionLookup->undistortedImagePoint(Vector2(Scalar(x), Scalar(y))), useDistortionParameters));

			if (inputPosition.x() < Scalar(0) || inputPosition.x() > scalarInputWidth_1 || inputPosition.y() < Scalar(0) || inputPosition.y() > scalarInputHeight_1)
			{
				*outputMask = 0xFF - maskValue;
			}
			else
			{
				interpolatePixel8BitPerChannel<tChannels, PC_TOP_LEFT>(input, inputCamera->width(), inputCamera->height(), inputPaddingElements, inputPosition, outputData);
				*outputMask = maskValue;
			}

			outputData += tChannels;
			++outputMask;
		}

		outputData += outputPaddingElements;
		outputMask += outputMaskPaddingElements;
	}
}

template <unsigned int tChannels>
void FrameInterpolatorBilinear::lookup8BitPerChannelSubset(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const LookupTable* input_LT_output, const bool offset, const uint8_t* borderColor, uint8_t* output, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(input_LT_output != nullptr);
	ocean_assert(input != nullptr && output != nullptr);

	ocean_assert(inputWidth != 0u && inputHeight != 0u);
	ocean_assert(firstRow + numberRows <= input_LT_output->sizeY());

	typedef typename DataType<uint8_t, tChannels>::Type PixelType;

	const uint8_t zeroColor[tChannels] = {uint8_t(0)};
	const PixelType* const bColor = borderColor ? (PixelType*)borderColor : (PixelType*)zeroColor;

	const unsigned int columns = (unsigned int)(input_LT_output->sizeX());

	const unsigned int outputStrideElements = tChannels * columns + outputPaddingElements;

	static_assert(std::is_same<Vector2, LookupTable::Type>::value, "Invalid data type!");

	const Scalar inputWidth1 = Scalar(inputWidth - 1u);
	const Scalar inputHeight1 = Scalar(inputHeight - 1u);

	Memory rowLookupMemory = Memory::create<Vector2>(columns);
	Vector2* const rowLookupData = rowLookupMemory.data<Vector2>();

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		input_LT_output->bilinearValues(y, rowLookupData);

		PixelType* outputData = (PixelType*)(output + y * outputStrideElements);

		for (unsigned int x = 0u; x < columns; ++x)
		{
			const Vector2& lookupValue = rowLookupData[x];

			const Vector2 inputPosition = offset ? Vector2(Scalar(x) + lookupValue.x(), Scalar(y) + lookupValue.y()) : lookupValue;

			if (inputPosition.x() >= Scalar(0) && inputPosition.y() >= Scalar(0) && inputPosition.x() <= inputWidth1 && inputPosition.y() <= inputHeight1)
			{
				interpolatePixel8BitPerChannel<tChannels, PC_TOP_LEFT>(input, inputWidth, inputHeight, inputPaddingElements, inputPosition, (uint8_t*)(outputData));
			}
			else
			{
				*outputData = *bColor;
			}

			outputData++;
		}
	}
}

template <typename T, unsigned int tChannels>
void FrameInterpolatorBilinear::lookupSubset(const T* input, const unsigned int inputWidth, const unsigned int inputHeight, const LookupTable* input_LT_output, const bool offset, const T* borderColor, T* output, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert((!std::is_same<uint8_t, T>::value));

	ocean_assert(input_LT_output != nullptr);
	ocean_assert(input != nullptr && output != nullptr);

	ocean_assert(inputWidth != 0u && inputHeight != 0u);
	ocean_assert(firstRow + numberRows <= input_LT_output->sizeY());

	typedef typename DataType<T, tChannels>::Type PixelType;

	const T zeroColor[tChannels] = {T(0)};
	const PixelType* const bColor = borderColor ? (PixelType*)borderColor : (PixelType*)zeroColor;

	const unsigned int columns = (unsigned int)(input_LT_output->sizeX());

	const unsigned int outputStrideElements = tChannels * columns + outputPaddingElements;

	static_assert(std::is_same<Vector2, LookupTable::Type>::value, "Invalid data type!");

	const Scalar inputWidth1 = Scalar(inputWidth - 1u);
	const Scalar inputHeight1 = Scalar(inputHeight - 1u);

	Memory rowLookupMemory = Memory::create<Vector2>(columns);
	Vector2* const rowLookupData = rowLookupMemory.data<Vector2>();

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		input_LT_output->bilinearValues(y, rowLookupData);

		PixelType* outputData = (PixelType*)(output + y * outputStrideElements);

		for (unsigned int x = 0u; x < columns; ++x)
		{
			const Vector2& lookupValue = rowLookupData[x];

			const Vector2 inputPosition = offset ? Vector2(Scalar(x) + lookupValue.x(), Scalar(y) + lookupValue.y()) : lookupValue;

			if (inputPosition.x() >= Scalar(0) && inputPosition.y() >= Scalar(0) && inputPosition.x() <= inputWidth1 && inputPosition.y() <= inputHeight1)
			{
				interpolatePixel<T, T, tChannels, PC_TOP_LEFT>(input, inputWidth, inputHeight, inputPaddingElements, inputPosition, (T*)(outputData));
			}
			else
			{
				*outputData = *bColor;
			}

			outputData++;
		}
	}
}

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

template <>
inline void FrameInterpolatorBilinear::lookup8BitPerChannelSubsetNEON<1u>(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const LookupTable* input_LT_output, const bool offset, const uint8_t* borderColor, uint8_t* output, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(input_LT_output != nullptr);
	ocean_assert(input != nullptr && output != nullptr);

	ocean_assert(inputWidth != 0u && inputHeight != 0u);
	ocean_assert(firstRow + numberRows <= input_LT_output->sizeY());

	typedef uint8_t PixelType;

	const uint8x16_t constantBorderColor_u_8x16 = vdupq_n_u8(borderColor ? *borderColor : 0u);

	const unsigned int outputWidth = (unsigned int)(input_LT_output->sizeX());
	ocean_assert(outputWidth >= 8u);

	static_assert(std::is_same<Vector2, LookupTable::Type>::value, "Invalid data type!");

	const unsigned int inputStrideElements = inputWidth + inputPaddingElements;
	const unsigned int outputStrideElements = outputWidth + outputPaddingElements;

	Memory rowLookupMemory = Memory::create<VectorF2>(outputWidth);
	VectorF2* const rowLookupData = rowLookupMemory.data<VectorF2>();

	const float32x4_t constantZero_f_32x4 = vdupq_n_f32(0.0f); // [0.0f, 0.0f, 0.0f, 0.0f]
	const float32x4_t constantEight_f_32x4 = vdupq_n_f32(8.0f); // [4.0f, 4.0f, 4.0f, 4.0f]

	// [0.0f, 1.0f, 2.0f, 3.0f, ...]
	const float f_01234567[8] = {0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f};
	const float32x4_t conststant0123_f_32x4 = vld1q_f32(f_01234567 + 0);
	const float32x4_t conststant4567_f_32x4 = vld1q_f32(f_01234567 + 4);

	const float32x4_t constant128_f_32x4 = vdupq_n_f32(128.0f);

	const uint32x4_t constantOne_u_32x4 = vdupq_n_u32(1u);

	const uint32x4_t constantChannels_u_32x4 = vdupq_n_u32(1u);

	const float32x4_t constantInputWidth1_f_32x4 = vdupq_n_f32(float(inputWidth - 1u));
	const float32x4_t constantInputHeight1_f_32x4 = vdupq_n_f32(float(inputHeight - 1u));

	const uint32x4_t constantInputStrideElements_u_32x4 = vdupq_n_u32(inputStrideElements);
	const uint32x4_t constantInputHeight1_u_32x4 = vdupq_n_u32(inputHeight - 1u);

	unsigned int validPixels[8];

	unsigned int topLeftOffsetsElements[8];
	unsigned int bottomLeftOffsetsElements[8];

	uint8_t pixels[32];

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		PixelType* outputPixelData = (PixelType*)(output + y * outputStrideElements);

		input_LT_output->bilinearValues<VectorF2>(y, rowLookupData);

		float32x4_t additionalInputOffsetX0123_f_32x4 = conststant0123_f_32x4;
		float32x4_t additionalInputOffsetX4567_f_32x4 = conststant4567_f_32x4;

		const float32x4_t additionalInputOffsetY_f_32x4 = vdupq_n_f32(float(y));

		for (unsigned int x = 0u; x < outputWidth; x += 8u)
		{
			if (x + 8u > outputWidth)
			{
				// the last iteration will not fit into the output frame,
				// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

				ocean_assert(x >= 8u && outputWidth > 8u);
				const unsigned int newX = outputWidth - 8u;

				ocean_assert(x > newX);
				const unsigned int xOffset = x - newX;

				outputPixelData -= xOffset;

				if (offset)
				{
					additionalInputOffsetX0123_f_32x4 = vsubq_f32(additionalInputOffsetX0123_f_32x4, vdupq_n_f32(float(xOffset)));
					additionalInputOffsetX4567_f_32x4 = vsubq_f32(additionalInputOffsetX4567_f_32x4, vdupq_n_f32(float(xOffset)));
				}

				x = newX;

				// the for loop will stop after this iteration
				ocean_assert(!(x + 8u < outputWidth));
			}

			const float32x4x2_t inputPositions0123_f_32x4x2 = vld2q_f32((const float*)(rowLookupData + x + 0u));
			const float32x4x2_t inputPositions4567_f_32x4x2 = vld2q_f32((const float*)(rowLookupData + x + 4u));

			float32x4_t inputPositionsX0123_f_32x4 = inputPositions0123_f_32x4x2.val[0];
			float32x4_t inputPositionsY0123_f_32x4 = inputPositions0123_f_32x4x2.val[1];

			float32x4_t inputPositionsX4567_f_32x4 = inputPositions4567_f_32x4x2.val[0];
			float32x4_t inputPositionsY4567_f_32x4 = inputPositions4567_f_32x4x2.val[1];

			if (offset)
			{
				inputPositionsX0123_f_32x4 = vaddq_f32(inputPositionsX0123_f_32x4, additionalInputOffsetX0123_f_32x4);
				inputPositionsY0123_f_32x4 = vaddq_f32(inputPositionsY0123_f_32x4, additionalInputOffsetY_f_32x4);

				inputPositionsX4567_f_32x4 = vaddq_f32(inputPositionsX4567_f_32x4, additionalInputOffsetX4567_f_32x4);
				inputPositionsY4567_f_32x4 = vaddq_f32(inputPositionsY4567_f_32x4, additionalInputOffsetY_f_32x4);

				additionalInputOffsetX0123_f_32x4 = vaddq_f32(additionalInputOffsetX0123_f_32x4, constantEight_f_32x4);
				additionalInputOffsetX4567_f_32x4 = vaddq_f32(additionalInputOffsetX4567_f_32x4, constantEight_f_32x4);
			}

			// now we check whether we are inside the input frame
			const uint32x4_t validPixelsX0123_u_32x4 = vandq_u32(vcltq_f32(inputPositionsX0123_f_32x4, constantInputWidth1_f_32x4), vcgeq_f32(inputPositionsX0123_f_32x4, constantZero_f_32x4)); // inputPosition.x() >= 0 && inputPosition.x() < (inputWidth - 1) ? 0xFFFFFF : 0x000000
			const uint32x4_t validPixelsX4567_u_32x4 = vandq_u32(vcltq_f32(inputPositionsX4567_f_32x4, constantInputWidth1_f_32x4), vcgeq_f32(inputPositionsX4567_f_32x4, constantZero_f_32x4));

			const uint32x4_t validPixelsY0123_u_32x4 = vandq_u32(vcltq_f32(inputPositionsY0123_f_32x4, constantInputHeight1_f_32x4), vcgeq_f32(inputPositionsY0123_f_32x4, constantZero_f_32x4)); // inputPosition.y() >= 0 && inputPosition.y() < (inputHeight - 1) ? 0xFFFFFF : 0x000000
			const uint32x4_t validPixelsY4567_u_32x4 = vandq_u32(vcltq_f32(inputPositionsY4567_f_32x4, constantInputHeight1_f_32x4), vcgeq_f32(inputPositionsY4567_f_32x4, constantZero_f_32x4));

			const uint32x4_t validPixels0123_u_32x4 = vandq_u32(validPixelsX0123_u_32x4, validPixelsY0123_u_32x4); // is_inside_input_frame(inputPosition) ? 0xFFFFFF : 0x000000
			const uint32x4_t validPixels4567_u_32x4 = vandq_u32(validPixelsX4567_u_32x4, validPixelsY4567_u_32x4);

			vst1q_u32(validPixels + 0, validPixels0123_u_32x4);
			vst1q_u32(validPixels + 4, validPixels4567_u_32x4);


			const uint32x4_t inputPositionsLeft0123_u_32x4 = vcvtq_u32_f32(inputPositionsX0123_f_32x4);
			const uint32x4_t inputPositionsLeft4567_u_32x4 = vcvtq_u32_f32(inputPositionsX4567_f_32x4);

			const uint32x4_t inputPositionsTop0123_u_32x4 = vcvtq_u32_f32(inputPositionsY0123_f_32x4);
			const uint32x4_t inputPositionsTop4567_u_32x4 = vcvtq_u32_f32(inputPositionsY4567_f_32x4);

			const uint32x4_t inputPositionsBottom0123_u_32x4 = vminq_u32(vaddq_u32(inputPositionsTop0123_u_32x4, constantOne_u_32x4), constantInputHeight1_u_32x4);
			const uint32x4_t inputPositionsBottom4567_u_32x4 = vminq_u32(vaddq_u32(inputPositionsTop4567_u_32x4, constantOne_u_32x4), constantInputHeight1_u_32x4);


			const uint32x4_t topLeftOffsetsElements0123_u_32x4 = vmlaq_u32(vmulq_u32(inputPositionsLeft0123_u_32x4, constantChannels_u_32x4), inputPositionsTop0123_u_32x4, constantInputStrideElements_u_32x4); // topLeftOffset = top * strideElements + left * channels
			vst1q_u32(topLeftOffsetsElements + 0, topLeftOffsetsElements0123_u_32x4);
			const uint32x4_t topLeftOffsetsElements4567_u_32x4 = vmlaq_u32(vmulq_u32(inputPositionsLeft4567_u_32x4, constantChannels_u_32x4), inputPositionsTop4567_u_32x4, constantInputStrideElements_u_32x4);
			vst1q_u32(topLeftOffsetsElements + 4, topLeftOffsetsElements4567_u_32x4);

			const uint32x4_t bottomLeftOffsetsElements0123_u_32x4 = vmlaq_u32(vmulq_u32(inputPositionsLeft0123_u_32x4, constantChannels_u_32x4), inputPositionsBottom0123_u_32x4, constantInputStrideElements_u_32x4);
			vst1q_u32(bottomLeftOffsetsElements + 0, bottomLeftOffsetsElements0123_u_32x4);
			const uint32x4_t bottomLeftOffsetsElements4567_u_32x4 = vmlaq_u32(vmulq_u32(inputPositionsLeft4567_u_32x4, constantChannels_u_32x4), inputPositionsBottom4567_u_32x4, constantInputStrideElements_u_32x4);
			vst1q_u32(bottomLeftOffsetsElements + 4, bottomLeftOffsetsElements4567_u_32x4);


			// we determine the fractional portions of the x' and y' and [0.0, 1.0] -> [0, 128]
			float32x4_t tx0123_f_32x4 = vmulq_f32(vsubq_f32(inputPositionsX0123_f_32x4, vcvtq_f32_u32(inputPositionsLeft0123_u_32x4)), constant128_f_32x4);
			float32x4_t tx4567_f_32x4 = vmulq_f32(vsubq_f32(inputPositionsX4567_f_32x4, vcvtq_f32_u32(inputPositionsLeft4567_u_32x4)), constant128_f_32x4);

			float32x4_t ty0123_f_32x4 = vmulq_f32(vsubq_f32(inputPositionsY0123_f_32x4, vcvtq_f32_u32(inputPositionsTop0123_u_32x4)), constant128_f_32x4);
			float32x4_t ty4567_f_32x4 = vmulq_f32(vsubq_f32(inputPositionsY4567_f_32x4, vcvtq_f32_u32(inputPositionsTop4567_u_32x4)), constant128_f_32x4);

			const uint32x4_t tx0123_128_u_32x4 = vcvtq_u32_f32(vaddq_f32(tx0123_f_32x4, vdupq_n_f32(0.5)));
			const uint32x4_t tx4567_128_u_32x4 = vcvtq_u32_f32(vaddq_f32(tx4567_f_32x4, vdupq_n_f32(0.5)));

			const uint32x4_t ty0123_128_u_32x4 = vcvtq_u32_f32(vaddq_f32(ty0123_f_32x4, vdupq_n_f32(0.5)));
			const uint32x4_t ty4567_128_u_32x4 = vcvtq_u32_f32(vaddq_f32(ty4567_f_32x4, vdupq_n_f32(0.5)));

			const uint16x8_t tx01234567_128_u_16x8 = vcombine_u16(vmovn_u32(tx0123_128_u_32x4), vmovn_u32(tx4567_128_u_32x4));
			const uint16x8_t ty01234567_128_u_16x8 = vcombine_u16(vmovn_u32(ty0123_128_u_32x4), vmovn_u32(ty4567_128_u_32x4));

			const uint8x16_t tx_ty_128_u_8x16 = vcombine_u8(vmovn_u16(tx01234567_128_u_16x8), vmovn_u16(ty01234567_128_u_16x8));


			vst1q_u8(pixels + 0, constantBorderColor_u_8x16); // initialize with border color
			vst1q_u8(pixels + 16, constantBorderColor_u_8x16);

			struct LeftRightPixel
			{
				uint8_t left;
				uint8_t right;
			};

			static_assert(sizeof(LeftRightPixel) == 2, "Invalid data type!");

			// we gather the individual source pixel values from the source image,
			// based on the calculated pixel locations
			for (unsigned int i = 0u; i < 8u; ++i)
			{
				if (validPixels[i])
				{
					ocean_assert((topLeftOffsetsElements[i] % inputStrideElements) < inputWidth - 1u); // we need to have one additional pixel to the right (as we copy two pixels at once)
					ocean_assert((bottomLeftOffsetsElements[i] % inputStrideElements) < inputWidth - 1u);

					((LeftRightPixel*)pixels)[0u + i] = *(LeftRightPixel*)(input + topLeftOffsetsElements[i]);
					((LeftRightPixel*)pixels)[8u + i] = *(LeftRightPixel*)(input + bottomLeftOffsetsElements[i]);
				}
			}

			const uint8x8x2_t topLeft_topRight_u_8x8x2 = vld2_u8(pixels);
			const uint8x8x2_t bottomLeft_bottomRight_u_8x8x2 = vld2_u8(pixels + 16);

			interpolate8Pixels1Channel8BitNEON(topLeft_topRight_u_8x8x2.val[0], topLeft_topRight_u_8x8x2.val[1], bottomLeft_bottomRight_u_8x8x2.val[0], bottomLeft_bottomRight_u_8x8x2.val[1], tx_ty_128_u_8x16, outputPixelData);

			outputPixelData += 8;
		}
	}
}

template <unsigned int tChannels>
void FrameInterpolatorBilinear::lookup8BitPerChannelSubsetNEON(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const LookupTable* input_LT_output, const bool offset, const uint8_t* borderColor, uint8_t* output, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(input_LT_output != nullptr);
	ocean_assert(input != nullptr && output != nullptr);

	ocean_assert(inputWidth != 0u && inputHeight != 0u);
	ocean_assert(firstRow + numberRows <= input_LT_output->sizeY());

	typedef typename DataType<uint8_t, tChannels>::Type PixelType;

	const uint8_t zeroColor[tChannels] = {uint8_t(0)};
	const PixelType* const bColor = borderColor ? (PixelType*)borderColor : (PixelType*)zeroColor;

	const unsigned int outputWidth = (unsigned int)(input_LT_output->sizeX());
	ocean_assert(outputWidth >= 4u);

	static_assert(std::is_same<Vector2, LookupTable::Type>::value, "Invalid data type!");

	const unsigned int inputStrideElements = inputWidth * tChannels + inputPaddingElements;
	const unsigned int outputStrideElements = outputWidth * tChannels + outputPaddingElements;

	Memory rowLookupMemory = Memory::create<VectorF2>(outputWidth);
	VectorF2* const rowLookupData = rowLookupMemory.data<VectorF2>();

	const float32x4_t constantZero_f_32x4 = vdupq_n_f32(0.0f); // [0.0f, 0.0f, 0.0f, 0.0f]
	const float32x4_t constantFour_f_32x4 = vdupq_n_f32(4.0f); // [4.0f, 4.0f, 4.0f, 4.0f]

	// [0.0f, 1.0f, 2.0f, 3.0f]
	const float f_0123[4] = {0.0f, 1.0f, 2.0f, 3.0f};
	float32x4_t conststant0123_f_32x4 = vld1q_f32(f_0123);

	const uint32x4_t constantOne_u_32x4 = vdupq_n_u32(1u);

	const uint32x4_t constantChannels_u_32x4 = vdupq_n_u32(tChannels);

	const float32x4_t constantInputWidth1_f_32x4 = vdupq_n_f32(float(inputWidth - 1u));
	const float32x4_t constantInputHeight1_f_32x4 = vdupq_n_f32(float(inputHeight - 1u));

	const uint32x4_t constantInputStrideElements_u_32x4 = vdupq_n_u32(inputStrideElements);
	const uint32x4_t constantInputWidth1_u_32x4 = vdupq_n_u32(inputWidth - 1u);
	const uint32x4_t constantInputHeight1_u_32x4 = vdupq_n_u32(inputHeight - 1u);

	unsigned int validPixels[4];

	unsigned int topLeftOffsetsElements[4];
	unsigned int topRightOffsetsElements[4];
	unsigned int bottomLeftOffsetsElements[4];
	unsigned int bottomRightOffsetsElements[4];

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		PixelType* outputPixelData = (PixelType*)(output + y * outputStrideElements);

		input_LT_output->bilinearValues<VectorF2>(y, rowLookupData);

		float32x4_t additionalInputOffsetX_f_32x4 = conststant0123_f_32x4;
		const float32x4_t additionalInputOffsetY_f_32x4 = vdupq_n_f32(float(y));

		for (unsigned int x = 0u; x < outputWidth; x += 4u)
		{
			if (x + 4u > outputWidth)
			{
				// the last iteration will not fit into the output frame,
				// so we simply shift x left by some pixels (at most 3) and we will calculate some pixels again

				ocean_assert(x >= 4u && outputWidth > 4u);
				const unsigned int newX = outputWidth - 4u;

				ocean_assert(x > newX);
				const unsigned int xOffset = x - newX;

				outputPixelData -= xOffset;

				if (offset)
				{
					additionalInputOffsetX_f_32x4 = vsubq_f32(additionalInputOffsetX_f_32x4, vdupq_n_f32(float(xOffset)));
				}

				x = newX;

				// the for loop will stop after this iteration
				ocean_assert(!(x + 4u < outputWidth));
			}

			const float32x4x2_t inputPositions_f_32x4x2 = vld2q_f32((const float*)(rowLookupData + x));

			float32x4_t inputPositionsX_f_32x4 = inputPositions_f_32x4x2.val[0];
			float32x4_t inputPositionsY_f_32x4 = inputPositions_f_32x4x2.val[1];

			if (offset)
			{
				inputPositionsX_f_32x4 = vaddq_f32(inputPositionsX_f_32x4, additionalInputOffsetX_f_32x4);
				inputPositionsY_f_32x4 = vaddq_f32(inputPositionsY_f_32x4, additionalInputOffsetY_f_32x4);

				additionalInputOffsetX_f_32x4 = vaddq_f32(additionalInputOffsetX_f_32x4, constantFour_f_32x4);
			}

			// now we check whether we are inside the input frame
			const uint32x4_t validPixelsX_u_32x4 = vandq_u32(vcleq_f32(inputPositionsX_f_32x4, constantInputWidth1_f_32x4), vcgeq_f32(inputPositionsX_f_32x4, constantZero_f_32x4)); // inputPosition.x() >= 0 && inputPosition.x() <= (inputWidth - 1) ? 0xFFFFFF : 0x000000
			const uint32x4_t validPixelsY_u_32x4 = vandq_u32(vcleq_f32(inputPositionsY_f_32x4, constantInputHeight1_f_32x4), vcgeq_f32(inputPositionsY_f_32x4, constantZero_f_32x4)); // inputPosition.y() >= 0 && inputPosition.y() <= (inputHeight - 1) ? 0xFFFFFF : 0x000000

			const uint32x4_t validPixels_u_32x4 = vandq_u32(validPixelsX_u_32x4, validPixelsY_u_32x4); // is_inside_input_frame(inputPosition) ? 0xFFFFFF : 0x000000

			vst1q_u32(validPixels, validPixels_u_32x4);

			const uint32x4_t inputPositionsLeft_u_32x4 = vcvtq_u32_f32(inputPositionsX_f_32x4);
			const uint32x4_t inputPositionsTop_u_32x4 = vcvtq_u32_f32(inputPositionsY_f_32x4);

			const uint32x4_t inputPositionsRight_u_32x4 = vminq_u32(vaddq_u32(inputPositionsLeft_u_32x4, constantOne_u_32x4), constantInputWidth1_u_32x4);
			const uint32x4_t inputPositionsBottom_u_32x4 = vminq_u32(vaddq_u32(inputPositionsTop_u_32x4, constantOne_u_32x4), constantInputHeight1_u_32x4);

			const uint32x4_t topLeftOffsetsElements_u_32x4 = vmlaq_u32(vmulq_u32(inputPositionsLeft_u_32x4, constantChannels_u_32x4), inputPositionsTop_u_32x4, constantInputStrideElements_u_32x4); // topLeftOffset = top * strideElements + left * channels
			const uint32x4_t topRightOffsetsElements_u_32x4 = vmlaq_u32(vmulq_u32(inputPositionsRight_u_32x4, constantChannels_u_32x4), inputPositionsTop_u_32x4, constantInputStrideElements_u_32x4);
			const uint32x4_t bottomLeftOffsetsElements_u_32x4 = vmlaq_u32(vmulq_u32(inputPositionsLeft_u_32x4, constantChannels_u_32x4), inputPositionsBottom_u_32x4, constantInputStrideElements_u_32x4);
			const uint32x4_t bottomRightOffsetsElements_u_32x4 = vmlaq_u32(vmulq_u32(inputPositionsRight_u_32x4, constantChannels_u_32x4), inputPositionsBottom_u_32x4, constantInputStrideElements_u_32x4);

			vst1q_u32(topLeftOffsetsElements, topLeftOffsetsElements_u_32x4);
			vst1q_u32(topRightOffsetsElements, topRightOffsetsElements_u_32x4);
			vst1q_u32(bottomLeftOffsetsElements, bottomLeftOffsetsElements_u_32x4);
			vst1q_u32(bottomRightOffsetsElements, bottomRightOffsetsElements_u_32x4);

			// we determine the fractional portions of the x' and y':
			float32x4_t tx_f_32x4 = vsubq_f32(inputPositionsX_f_32x4, vcvtq_f32_u32(inputPositionsLeft_u_32x4));
			float32x4_t ty_f_32x4 = vsubq_f32(inputPositionsY_f_32x4, vcvtq_f32_u32(inputPositionsTop_u_32x4));

			// we use integer interpolation [0.0, 1.0] -> [0, 128]
			tx_f_32x4 = vmulq_f32(tx_f_32x4, vdupq_n_f32(128.0f));
			ty_f_32x4 = vmulq_f32(ty_f_32x4, vdupq_n_f32(128.0f));

			const uint32x4_t tx_128_u_32x4 = vcvtq_u32_f32(vaddq_f32(tx_f_32x4, vdupq_n_f32(0.5)));
			const uint32x4_t ty_128_u_32x4 = vcvtq_u32_f32(vaddq_f32(ty_f_32x4, vdupq_n_f32(0.5)));

			interpolate4Pixels8BitPerChannelNEON<tChannels>(input, topLeftOffsetsElements, topRightOffsetsElements, bottomLeftOffsetsElements, bottomRightOffsetsElements, validPixels, *bColor, tx_128_u_32x4, ty_128_u_32x4, outputPixelData);

			outputPixelData += 4;
		}
	}
}

#endif // defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

template <unsigned int tChannels>
void FrameInterpolatorBilinear::lookupMask8BitPerChannelSubset(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const LookupTable* input_LT_output, const bool offset, uint8_t* output, uint8_t* outputMask, const uint8_t maskValue, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, const unsigned int outputMaskPaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(input_LT_output != nullptr);
	ocean_assert(input != nullptr && output != nullptr);

	ocean_assert(inputWidth != 0u && inputHeight != 0u);
	ocean_assert(firstRow + numberRows <= input_LT_output->sizeY());

	typedef typename DataType<uint8_t, tChannels>::Type PixelType;

	const unsigned int columns = (unsigned int)(input_LT_output->sizeX());

	const unsigned int outputStrideElements = tChannels * columns + outputPaddingElements;
	const unsigned int outputMaskStrideElements = columns + outputMaskPaddingElements;

	static_assert(std::is_same<Vector2, LookupTable::Type>::value, "Invalid data type!");

	const Scalar inputWidth1 = Scalar(inputWidth - 1u);
	const Scalar inputHeight1 = Scalar(inputHeight - 1u);

	Memory rowLookupMemory = Memory::create<Vector2>(columns);
	Vector2* const rowLookupData = rowLookupMemory.data<Vector2>();

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		input_LT_output->bilinearValues(y, rowLookupData);

		PixelType* outputData = (PixelType*)(output + y * outputStrideElements);
		uint8_t* outputMaskData = outputMask + y * outputMaskStrideElements;

		for (unsigned int x = 0u; x < columns; ++x)
		{
			const Vector2& lookupValue = rowLookupData[x];

			const Vector2 inputPosition = offset ? Vector2(Scalar(x) + lookupValue.x(), Scalar(y) + lookupValue.y()) : lookupValue;

			if (inputPosition.x() >= 0 && inputPosition.y() >= 0 && inputPosition.x() <= inputWidth1 && inputPosition.y() <= inputHeight1)
			{
				interpolatePixel8BitPerChannel<tChannels, PC_TOP_LEFT>(input, inputWidth, inputHeight, inputPaddingElements, inputPosition, (uint8_t*)(outputData));
				*outputMaskData = maskValue;
			}
			else
			{
				*outputMaskData = 0xFFu - maskValue;
			}

			outputData++;
			outputMaskData++;
		}
	}
}

template <unsigned int tChannels>
void FrameInterpolatorBilinear::scale8BitPerChannel(const uint8_t* source, uint8_t* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const double sourceX_s_targetX, const double sourceY_s_targetY, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(sourceWidth >= 1u && sourceHeight >= 1u);
	ocean_assert(targetWidth >= 1u && targetHeight >= 1u);
	ocean_assert(sourceX_s_targetX > 0.0);
	ocean_assert(sourceY_s_targetY > 0.0);

	if (sourceWidth == targetWidth && sourceHeight == targetHeight)
	{
		FrameConverter::subFrame<uint8_t>(source, target, sourceWidth, sourceHeight, targetWidth, targetHeight, tChannels, 0u, 0u, 0u, 0u, sourceWidth, sourceHeight, sourcePaddingElements, targetPaddingElements);
		return;
	}

	if (worker && sourceWidth * tChannels >= 16u && targetWidth >= 8u)
	{
#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
		if (sourceWidth <= 65535u && sourceHeight <= 65535u && targetWidth <= 65535u && targetHeight <= 65535u)
		{
			worker->executeFunction(Worker::Function::createStatic(&scale8BitPerChannelSubset7BitPrecisionNEON, source, target, sourceWidth, sourceHeight, targetWidth, targetHeight, tChannels, sourceX_s_targetX, sourceY_s_targetY, sourcePaddingElements, targetPaddingElements, 0u, 0u), 0u, targetHeight);
			return;
		}
#else
		worker->executeFunction(Worker::Function::createStatic(&scale8BitPerChannelSubset<tChannels>, source, target, sourceWidth, sourceHeight, targetWidth, targetHeight, sourceX_s_targetX, sourceY_s_targetY, sourcePaddingElements, targetPaddingElements, 0u, 0u), 0u, targetHeight);
#endif
	}
	else
	{
		if (sourceWidth * tChannels >= 16u && targetWidth >= 8u)
		{
#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
			if (sourceWidth <= 65535u && sourceHeight <= 65535u && targetWidth <= 65535u && targetHeight <= 65535u)
			{
				scale8BitPerChannelSubset7BitPrecisionNEON(source, target, sourceWidth, sourceHeight, targetWidth, targetHeight, tChannels, sourceX_s_targetX, sourceY_s_targetY, sourcePaddingElements, targetPaddingElements, 0u, targetHeight);
				return;
			}
#endif
		}

		scale8BitPerChannelSubset<tChannels>(source, target, sourceWidth, sourceHeight, targetWidth, targetHeight, sourceX_s_targetX, sourceY_s_targetY, sourcePaddingElements, targetPaddingElements, 0u, targetHeight);
	}
}

template <unsigned int tChannels>
void FrameInterpolatorBilinear::scale8BitPerChannelSubset(const uint8_t* source, uint8_t* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const double sourceX_s_targetX, const double sourceY_s_targetY, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstTargetRow, const unsigned int numberTargetRows)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(sourceWidth != 0u && sourceHeight != 0u);
	ocean_assert_and_suppress_unused(targetWidth >= 1u && targetHeight >= 1u, targetHeight);
	ocean_assert(sourceX_s_targetX > 0.0 && sourceY_s_targetY > 0.0);

	const Scalar sourceX_T_targetX = Scalar(sourceX_s_targetX);
	const Scalar sourceY_T_targetY = Scalar(sourceY_s_targetY);

	/*
	 * We determine the sub-pixel accurate source location for each target pixel as follows:
	 *
	 * Example with a downsampling by factor 4:
	 * sourceRow with 12 pixels: | 0 1 2 3 4 5 6 7 8 9 A B |
	 * targetRow with  3 pixels: | 0 1 2 |
	 *
	 * Thus, the source row can be separated into three blocks;
	 * and we want to extract the color information from the center of the blocks:
	 * sourceRow with 12 pixels: | 0 1 2 3 | 4 5 6 7 | 8 9 A B |
	 * targetRow with  3 pixels: |    0    |    1    |    2    | (sourceX_s_targetX = 4)
	 *
	 * Thus, we add 0.5 to each target coordinate before converting it to a source location;
	 * and subtract 0.5 again afterwards:
	 * sourceX = (targetX + 0.5) * sourceX_s_targetX - 0.5
	 *
	 * e.g., (0 + 0.5) * 4 - 0.5 = 1.5
	 *       (1 + 0.5) * 4 - 0.5 = 5.5
	 *
	 *
	 * Example with a downsampling by factor 3:
	 * sourceRow with 9 pixels: | 0 1 2 3 4 5 6 7 8 |
	 * targetRow with 3 pixels: | 0 1 2 |
	 *
	 * sourceRow with 9 pixels: | 0 1 2 | 3 4 5 | 6 7 8 |
	 * targetRow with 3 pixels: |   0   |   1   |   2   | (sourceX_s_targetX = 3)
	 *
	 * e.g., (0 + 0.5) * 3 - 0.5 = 1
	 *       (1 + 0.5) * 3 - 0.5 = 4
	 *
	 *
	 * Example with a downsampling by factor 2:
	 * sourceRow with 6 pixels: | 0 1 2 3 4 5 |
	 * targetRow with 3 pixels: | 0 1 2 |
	 *
	 * sourceRow with 6 pixels: | 0 1 | 2 3 | 4 5 |
	 * targetRow with 3 pixels: |  0  |  1  |  2  | (sourceX_s_targetX = 2)
	 *
	 * e.g., (0 + 0.5) * 2 - 0.5 = 0.5
	 *       (1 + 0.5) * 2 - 0.5 = 2.5
	 *
	 *
	 * we can simplify the calculation (as we have a constant term):
	 * sourceX = (sourceX_s_targetX * targetX) + (sourceX_s_targetX * 0.5 - 0.5)
	 */

	const unsigned int sourceStrideElements = sourceWidth * tChannels + sourcePaddingElements;

	const Scalar sourceX_T_targetXOffset = sourceX_T_targetX * Scalar(0.5) - Scalar(0.5);
	const Scalar sourceY_T_targetYOffset = sourceY_T_targetY * Scalar(0.5) - Scalar(0.5);

	const Scalar sourceWidth_1 = Scalar(sourceWidth - 1u);
	const Scalar sourceHeight_1 = Scalar(sourceHeight - 1u);

	target += (targetWidth * tChannels + targetPaddingElements) * firstTargetRow;

	for (unsigned int y = firstTargetRow; y < firstTargetRow + numberTargetRows; ++y)
	{
		const Scalar sy = minmax(Scalar(0), sourceY_T_targetYOffset + sourceY_T_targetY * Scalar(y), sourceHeight_1);
		ocean_assert(sy >= Scalar(0) && sy < Scalar(sourceHeight));

		const unsigned int sTop = (unsigned int)sy;
		ocean_assert(sy >= Scalar(sTop));

		const Scalar ty = sy - Scalar(sTop);
		ocean_assert(ty >= 0 && ty <= 1);

		const unsigned int factorBottom = (unsigned int)(ty * Scalar(128) + Scalar(0.5));
		const unsigned int factorTop = 128u - factorBottom;

		const uint8_t* const sourceTop = source + sourceStrideElements * sTop;
		const uint8_t* const sourceBottom = (sTop + 1u < sourceHeight) ? sourceTop + sourceStrideElements : sourceTop;

		for (unsigned int x = 0; x < targetWidth; ++x)
		{
			const Scalar sx = minmax(Scalar(0), sourceX_T_targetXOffset + sourceX_T_targetX * Scalar(x), sourceWidth_1);
			ocean_assert(sx >= Scalar(0) && sx < Scalar(sourceWidth));

			const unsigned int sLeft = (unsigned int)sx;
			ocean_assert(sx >= Scalar(sLeft));

			const Scalar tx = sx - Scalar(sLeft);
			ocean_assert(tx >= 0 && tx <= 1);

			const unsigned int factorRight = (unsigned int)(tx * Scalar(128) + Scalar(0.5));
			const unsigned int factorLeft = 128u - factorRight;

			const unsigned int sourceRightOffset = sLeft + 1u < sourceWidth ? tChannels : 0u;

			const uint8_t* const sourceTopLeft = sourceTop + sLeft * tChannels;
			const uint8_t* const sourceBottomLeft = sourceBottom + sLeft * tChannels;

			const unsigned int factorTopLeft = factorTop * factorLeft;
			const unsigned int factorTopRight = factorTop * factorRight;
			const unsigned int factorBottomLeft = factorBottom * factorLeft;
			const unsigned int factorBottomRight = factorBottom * factorRight;

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				target[n] = (uint8_t)((sourceTopLeft[n] * factorTopLeft + sourceTopLeft[sourceRightOffset + n] * factorTopRight
								+ sourceBottomLeft[n] * factorBottomLeft + sourceBottomLeft[sourceRightOffset + n] * factorBottomRight + 8192u) >> 14u);
			}

			target += tChannels;
		}

		target += targetPaddingElements;
	}
}

template <typename T>
void FrameInterpolatorBilinear::interpolateRowVertical(const T* sourceRowTop, const T* sourceRowBottom, T* targetRow, const unsigned int elements, const float factorBottom)
{
	ocean_assert(sourceRowTop != nullptr);
	ocean_assert(sourceRowBottom != nullptr);
	ocean_assert(targetRow != nullptr);
	ocean_assert(elements >= 1u);
	ocean_assert(factorBottom >= 0.0f && factorBottom <= 1.0f);

	typedef typename FloatTyper<T>::Type FloatType;

	const FloatType internalFactorBottom = FloatType(factorBottom);
	const FloatType internalFactorTop = FloatType(1.0f - factorBottom);

	for (unsigned int n = 0u; n < elements; ++n)
	{
		targetRow[n] = T(FloatType(sourceRowTop[n]) * internalFactorTop + FloatType(sourceRowBottom[n]) * internalFactorBottom);
	}
}

template <typename T, unsigned int tChannels>
void FrameInterpolatorBilinear::interpolateRowHorizontal(const T* extendedSourceRow, T* targetRow, const unsigned int targetWidth, const unsigned int channels, const unsigned int* interpolationLocations, const float* interpolationFactorsRight)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(extendedSourceRow != nullptr);
	ocean_assert(targetRow != nullptr);
	ocean_assert(targetWidth >= 1u);
	ocean_assert(interpolationLocations != nullptr);
	ocean_assert(interpolationFactorsRight != nullptr);
	ocean_assert(channels == tChannels);

	typedef typename FloatTyper<T>::Type FloatType;

	for (unsigned int x = 0u; x < targetWidth; ++x)
	{
		const FloatType internalFactorRight = FloatType(interpolationFactorsRight[x]);
		ocean_assert(internalFactorRight >= FloatType(0) && internalFactorRight <= FloatType(1));

		const FloatType internalFactorLeft = FloatType(1.0f - interpolationFactorsRight[x]);

		const unsigned int& leftLocation = interpolationLocations[x];
		const unsigned int rightLocation = leftLocation + tChannels; // location is defined in relation to elements, not to pixels

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			targetRow[x * tChannels + n] = T(FloatType(extendedSourceRow[leftLocation + n]) * internalFactorLeft + FloatType(extendedSourceRow[rightLocation + n]) * internalFactorRight);
		}
	}
}

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

#ifdef OCEAN_WE_KEEP_THIS_IMPLEMENTATION_AS_WE_NEED_THIS_TO_FOR_A_NEW_NEON_IMPLEMENTATION

template <>
inline void FrameInterpolatorBilinear::scale8BitPerChannelSubset7BitPrecisionNEON<2u, 8u>(const uint8_t* source, uint8_t* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const double sourceX_s_targetX, const double sourceY_s_targetY, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstTargetRow, const unsigned int numberTargetRows)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(sourceWidth >= 2u && sourceWidth <= 65535u);
	ocean_assert(sourceHeight >= 1u && sourceHeight <= 65535u);
	ocean_assert(targetWidth >= 8u && targetWidth <= 65535u);
	ocean_assert(targetHeight >= 1u && targetHeight <= 65535u);
	ocean_assert(sourceX_s_targetX > 0.0 && sourceY_s_targetY > 0.0);

	ocean_assert(sourcePaddingElements == 0u); // not supported
	ocean_assert(targetPaddingElements == 0u);

	typedef typename DataType<uint8_t, 2u>::Type PixelType;

	PixelType* targetPixelData = (PixelType*)target + firstTargetRow * targetWidth;
	const PixelType* const sourcePixelData = (const PixelType*)source;

	// our offset values for the eight left pixels in relation to the first pixel of the row
	unsigned int leftOffsets[8];

	// this function uses fixed point numbers with 16 bit for the calculation of the interpolation positions and factors:
	// fixedPointLocation = floatLocation * 2^16
	//
	// [FEDCBA98, 76543210]
	// [pixel   , subpixel]
	//
	// fixedPointLocation = pixel + subpixel / 2^16
	//
	// Thus, the upper 16 bit represent the location of e.g., the left pixel (for the linear interpolation)
	// while the lower 16 bit represent one of both interpolation factors (and 2^16 - subpixel represents the second interpolation factor)

	const unsigned int sourceX_T_targetX_fixed16 = (unsigned int)(double(0x10000u) * sourceX_s_targetX + 0.5);
	const unsigned int sourceY_T_targetY_fixed16 = (unsigned int)(double(0x10000u) * sourceY_s_targetY + 0.5);

	const int targetOffsetX_fixed16 = (int)(double(0x10000u) * ((sourceX_s_targetX * 0.5) - 0.5) + 0.5);
	const int targetOffsetY_fixed16 = (int)(double(0x10000u) * ((sourceY_s_targetY * 0.5) - 0.5) + 0.5);

	// we store 4 integers: [sourceX_T_targetX_fixed16, sourceX_T_targetX_fixed16, sourceX_T_targetX_fixed16, sourceX_T_targetX_fixed16]
	const uint32x4_t m128_u_sourceX_T_targetX_fixed16 = vdupq_n_u32(sourceX_T_targetX_fixed16);

	// we store 4 integers: [sourceX_T_targetX_fixed16, sourceX_T_targetX_fixed16, sourceX_T_targetX_fixed16, sourceX_T_targetX_fixed16]
	const int32x4_t m128_s_targetOffsetX_fixed16 = vdupq_n_s32(targetOffsetX_fixed16);

	// we store 4 integers: [sourceWidth - 2, sourceWidth - 2, sourceWidth - 2, sourceWidth - 2]
	const uint32x4_t m128_u_sourceWidth_2 = vdupq_n_u32(sourceWidth - 2u);

	// we store 4 integers: [0, 0, 0, 0]
	const int32x4_t m128_s_zero = vdupq_n_s32(0);

	const unsigned int u_0123[4] = {0u, 1u, 2u, 3u};
	const uint32x4_t m128_u_0123 = vld1q_u32(u_0123);

	for (unsigned int y = firstTargetRow; y < firstTargetRow + numberTargetRows; ++y)
	{
		const unsigned int sourceY_fixed16 = minmax<int>(0, targetOffsetY_fixed16 + int(sourceY_T_targetY_fixed16 * y), (sourceHeight - 1u) << 16u);

		const unsigned int sourceRowTop = sourceY_fixed16 >> 16u; // we must not round here
		const unsigned int factorBottom_fixed16 = sourceY_fixed16 & 0x0000FFFFu;
		const unsigned int factorBottom = factorBottom_fixed16 >> 9u;

		const uint8x8_t m64_u_factorsBottom = vdup_n_u8(factorBottom);
		// factorTop = 128 - factorBottom
		const uint8x8_t m64_u_factorsTop = vdup_n_u8(128u - factorBottom);

		const unsigned int sourceRowBottom = min(sourceRowTop + 1u, sourceHeight - 1u);

		const PixelType* const sourceTopRowPixelData = sourcePixelData + sourceRowTop * sourceWidth;
		const PixelType* const sourceBottomRowPixelData = sourcePixelData + sourceRowBottom * sourceWidth;

		for (unsigned int x = 0; x < targetWidth; x += 8u)
		{
			if (x + 8u > targetWidth)
			{
				// the last iteration will not fit into the output frame,
				// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

				ocean_assert(x >= 8u && targetWidth > 8u);
				const unsigned int newX = targetWidth - 8u;

				ocean_assert(x > newX);
				targetPixelData -= x - newX;

				x = newX;

				// the for loop will stop after this iteration
				ocean_assert(!(x + 8u < targetWidth));
			}


			// we need four successive x coordinate floats:
			// [x + 3, x + 2, x + 1; x + 0]
			const uint32x4_t m128_u_x_0123 = vaddq_u32(vdupq_n_u32(x), m128_u_0123);
			const uint32x4_t m128_u_x_4567 = vaddq_u32(vdupq_n_u32(x + 4u), m128_u_0123);

			// we calculate the four source locations for our four target locations
			const int32x4_t m128_s_sourceX_0123_fixed16 = vmaxq_s32(m128_s_zero, vaddq_s32(m128_s_targetOffsetX_fixed16, vreinterpretq_s32_u32(vmulq_u32(m128_u_sourceX_T_targetX_fixed16, m128_u_x_0123))));
			const uint32x4_t m128_u_sourceX_0123_fixed16 = vreinterpretq_u32_s32(m128_s_sourceX_0123_fixed16);

			const int32x4_t m128_s_sourceX_4567_fixed16 = vmaxq_s32(m128_s_zero, vaddq_s32(m128_s_targetOffsetX_fixed16, vreinterpretq_s32_u32(vmulq_u32(m128_u_sourceX_T_targetX_fixed16, m128_u_x_4567))));
			const uint32x4_t m128_u_sourceX_4567_fixed16 = vreinterpretq_u32_s32(m128_s_sourceX_4567_fixed16);

			// now we determine the pixel/integer accurate source locations
			// m128_u_left = min(floor(m128_f_sourceX), sourceWidth - 2)
			const uint32x4_t m128_u_left_0123 = vminq_u32(vshrq_n_u32(m128_u_sourceX_0123_fixed16, 16), m128_u_sourceWidth_2); // not vrshrq_n_u32 as we must not round here
			const uint32x4_t m128_u_left_4567 = vminq_u32(vshrq_n_u32(m128_u_sourceX_4567_fixed16, 16), m128_u_sourceWidth_2);

			// we store the offsets we have calculated
			vst1q_u32(leftOffsets + 0, m128_u_left_0123);
			vst1q_u32(leftOffsets + 4, m128_u_left_4567);



			// we load the individal pixels to our four (de-interleaved) 8x8 bit registers (we do this for the top-left and top-right pixels)
			// note: loading of each pixel individually is significantly slower than loading two neighboring pixels within one iteration

			uint8x8x2_t topLeftPixels;
			uint8x8x2_t topRightPixels;

			topLeftPixels = vld2_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[0u] + 0), topLeftPixels, 0);
			topRightPixels = vld2_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[0u] + 1), topRightPixels, 0);

			topLeftPixels = vld2_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[1u] + 0), topLeftPixels, 1);
			topRightPixels = vld2_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[1u] + 1), topRightPixels, 1);

			topLeftPixels = vld2_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[2u] + 0), topLeftPixels, 2);
			topRightPixels = vld2_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[2u] + 1), topRightPixels, 2);

			topLeftPixels = vld2_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[3u] + 0), topLeftPixels, 3);
			topRightPixels = vld2_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[3u] + 1), topRightPixels, 3);

			topLeftPixels = vld2_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[4u] + 0), topLeftPixels, 4);
			topRightPixels = vld2_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[4u] + 1), topRightPixels, 4);

			topLeftPixels = vld2_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[5u] + 0), topLeftPixels, 5);
			topRightPixels = vld2_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[5u] + 1), topRightPixels, 5);

			topLeftPixels = vld2_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[6u] + 0), topLeftPixels, 6);
			topRightPixels = vld2_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[6u] + 1), topRightPixels, 6);

			topLeftPixels = vld2_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[7u] + 0), topLeftPixels, 7);
			topRightPixels = vld2_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[7u] + 1), topRightPixels, 7);


			// we load the individual pixels to our four (de-interleaved) 8x8 bit registers (we do this for the bottom-left and bottom-right pixels)

			uint8x8x2_t bottomLeftPixels;
			uint8x8x2_t bottomRightPixels;

			bottomLeftPixels = vld2_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[0u] + 0), bottomLeftPixels, 0);
			bottomRightPixels = vld2_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[0u] + 1), bottomRightPixels, 0);

			bottomLeftPixels = vld2_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[1u] + 0), bottomLeftPixels, 1);
			bottomRightPixels = vld2_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[1u] + 1), bottomRightPixels, 1);

			bottomLeftPixels = vld2_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[2u] + 0), bottomLeftPixels, 2);
			bottomRightPixels = vld2_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[2u] + 1), bottomRightPixels, 2);

			bottomLeftPixels = vld2_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[3u] + 0), bottomLeftPixels, 3);
			bottomRightPixels = vld2_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[3u] + 1), bottomRightPixels, 3);

			bottomLeftPixels = vld2_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[4u] + 0), bottomLeftPixels, 4);
			bottomRightPixels = vld2_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[4u] + 1), bottomRightPixels, 4);

			bottomLeftPixels = vld2_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[5u] + 0), bottomLeftPixels, 5);
			bottomRightPixels = vld2_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[5u] + 1), bottomRightPixels, 5);

			bottomLeftPixels = vld2_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[6u] + 0), bottomLeftPixels, 6);
			bottomRightPixels = vld2_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[6u] + 1), bottomRightPixels, 6);

			bottomLeftPixels = vld2_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[7u] + 0), bottomLeftPixels, 7);
			bottomRightPixels = vld2_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[7u] + 1), bottomRightPixels, 7);



			// we determine the multiplication factors for the right pixels - which are already stored in the lower 16 bits
			// we need an accuracy of 7 bits (values between 0 and 128):
			//  76 54 32 10
			// [F3 F2 F1 F0]
			const uint16x4_t m64_u_factorsRight_0123 = vrshrn_n_u32(vandq_u32(m128_u_sourceX_0123_fixed16, vdupq_n_u32(0x0000FFFFu)), 9);
			const uint16x4_t m64_u_factorsRight_4567 = vrshrn_n_u32(vandq_u32(m128_u_sourceX_4567_fixed16, vdupq_n_u32(0x0000FFFFu)), 9);

			// as we have the pixel information de-interleaved, we can store all 8 interpolation factors together into one 8x8 bit register:
			const uint16x8_t m128_u_factorsRight = vcombine_u16(m64_u_factorsRight_0123, m64_u_factorsRight_4567);
			const uint8x8_t m64_u_factorsRight = vqmovn_u16(m128_u_factorsRight);
			const uint8x8_t m64_u_factorsLeft = vsub_u8(vdup_n_u8(128u), m64_u_factorsRight);



			// we determine the intermediate interpolation results for the top row (and we narrow down the 16 bit results 8 bit results)
			uint16x8_t m128_muliplicationChannel_0 = vmull_u8(topLeftPixels.val[0], m64_u_factorsLeft);
			uint16x8_t m128_muliplicationChannel_1 = vmull_u8(topLeftPixels.val[1], m64_u_factorsLeft);

			m128_muliplicationChannel_0 = vmlal_u8(m128_muliplicationChannel_0, topRightPixels.val[0], m64_u_factorsRight);
			m128_muliplicationChannel_1 = vmlal_u8(m128_muliplicationChannel_1, topRightPixels.val[1], m64_u_factorsRight);

			uint8x8_t m64_topRowChannel_0 = vrshrn_n_u16(m128_muliplicationChannel_0, 7);
			uint8x8_t m64_topRowChannel_1 = vrshrn_n_u16(m128_muliplicationChannel_1, 7);



			// we determine the intermediate interpolation results for the bottom row (and we narrow down the 16 bit results 8 bit results)
			m128_muliplicationChannel_0 = vmull_u8(bottomLeftPixels.val[0], m64_u_factorsLeft);
			m128_muliplicationChannel_1 = vmull_u8(bottomLeftPixels.val[1], m64_u_factorsLeft);

			m128_muliplicationChannel_0 = vmlal_u8(m128_muliplicationChannel_0, bottomRightPixels.val[0], m64_u_factorsRight);
			m128_muliplicationChannel_1 = vmlal_u8(m128_muliplicationChannel_1, bottomRightPixels.val[1], m64_u_factorsRight);

			uint8x8_t m64_bottomRowChannel_0 = vrshrn_n_u16(m128_muliplicationChannel_0, 7);
			uint8x8_t m64_bottomRowChannel_1 = vrshrn_n_u16(m128_muliplicationChannel_1, 7);



			// finnally we determine the interpolation result between top and bottom row
			m128_muliplicationChannel_0 = vmull_u8(m64_topRowChannel_0, m64_u_factorsTop);
			m128_muliplicationChannel_1 = vmull_u8(m64_topRowChannel_1, m64_u_factorsTop);

			m128_muliplicationChannel_0 = vmlal_u8(m128_muliplicationChannel_0, m64_bottomRowChannel_0, m64_u_factorsBottom);
			m128_muliplicationChannel_1 = vmlal_u8(m128_muliplicationChannel_1, m64_bottomRowChannel_1, m64_u_factorsBottom);


			// we narrow down the interpolation results and we store them
			uint8x8x2_t result;
			result.val[0] = vrshrn_n_u16(m128_muliplicationChannel_0, 7);
			result.val[1] = vrshrn_n_u16(m128_muliplicationChannel_1, 7);

			// we write back the results and interleave them automatically
			vst2_u8((uint8_t*)targetPixelData, result);

			targetPixelData += 8;
		}

		// we need to process the last pixel again, as this pixel may have received wrong interpolation factors as we always load two successive pixels into our NEON registers
		// **TODO** this is just a temporary solution, check how we can avoid this additional step

		const unsigned int firstInvalidTargetX = (((sourceWidth - 1u) << 16u) - targetOffsetX_fixed16) / sourceX_T_targetX_fixed16;

		for (unsigned int x = firstInvalidTargetX; x < targetWidth; ++x)
		{
			const unsigned int lastSourcePixelPosition_fixed16 = minmax<int>(0, targetOffsetX_fixed16 + int(sourceX_T_targetX_fixed16 * x), (sourceWidth - 1u) << 16u);

			const unsigned int lastSourcePixelLeft = lastSourcePixelPosition_fixed16 >> 16u;
			ocean_assert(lastSourcePixelLeft < sourceWidth);
			const unsigned int lastSourcePixelRight = min(lastSourcePixelLeft + 1u, sourceWidth - 1u);

			const unsigned int factorRight_fixed16 = lastSourcePixelPosition_fixed16 & 0x0000FFFFu;

			const unsigned int factorRight = factorRight_fixed16 >> 9u;
			const unsigned int factorLeft = 128u - factorRight;

			for (unsigned int c = 0u; c < 2u; ++c)
			{
				((uint8_t*)(targetPixelData - (targetWidth - x)))[c] = ((((const uint8_t*)(sourceTopRowPixelData + lastSourcePixelLeft))[c] * factorLeft + ((const uint8_t*)(sourceTopRowPixelData + lastSourcePixelRight))[c] * factorRight) * (128u - factorBottom)
																			  + (((const uint8_t*)(sourceBottomRowPixelData + lastSourcePixelLeft))[c] * factorLeft + ((const uint8_t*)(sourceBottomRowPixelData + lastSourcePixelRight))[c] * factorRight) * factorBottom + 8192u) >> 14u;
			}
		}
	}
}

#endif // OCEAN_WE_KEEP_THIS_IMPLEMENTATION_AS_WE_NEED_THIS_TO_FOR_A_NEW_NEON_IMPLEMENTATION

#ifdef OCEAN_WE_KEEP_ALSO_THIS_SLOW_IMPLEMENTATION_SHOWING_A_SLIGHTLY_DIFFERENT_APPROACH

template <>
inline void FrameInterpolatorBilinear::scale8BitPerChannelSubset7BitPrecisionNEON<2u, 8u>(const uint8_t* source, uint8_t* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const double sourceX_s_targetX, const double sourceY_s_targetY, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstTargetRow, const unsigned int numberTargetRows)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(sourceWidth >= 2u && sourceWidth <= 65535u);
	ocean_assert(sourceHeight >= 0u && sourceHeight <= 65535u);
	ocean_assert(targetWidth >= 8u && targetWidth <= 65535u)
	ocean_assert(targetHeight >= 1u && targetHeight <= 65535u);
	ocean_assert(sourceX_s_targetX > 0.0 && sourceY_s_targetY > 0.0);

	ocean_assert(sourcePaddingElements == 0u); // not supported
	ocean_assert(targetPaddingElements == 0u);

	typedef typename DataType<uint8_t, 2u>::Type PixelType;

	PixelType* targetPixelData = (PixelType*)target + firstTargetRow * targetWidth;
	const PixelType* const sourcePixelData = (const PixelType*)source;

	// our offset values for the four left pixels in relation to the first pixel of the row
	unsigned int leftOffsets[8];

	// our color values of the eight top and bottom pixels (32 bit = 16 bit left and 16 bit right)
	unsigned int topPixels[8];
	unsigned int bottomPixels[8];

	// this function uses fixed point numbers with 16 bit for the calculation of the interpolation positions and factors:
	// fixedPointLocation = floatLocation * 2^16
	//
	// [FEDCBA98, 76543210]
	// [pixel   , subpixel]
	//
	// fixedPointLocation = pixel + subpixel / 2^16
	//
	// Thus, the upper 16 bit represent the location of e.g., the left pixel (for the linear interpolation)
	// while the lower 16 bit represent one of both interpolation factors (and 2^16 - subpixel represents the second interpolation factor)

	const unsigned int sourceX_T_targetX_fixed16 = (unsigned int)(double(0x10000u) * sourceX_s_targetX + 0.5);
	const unsigned int sourceY_T_targetY_fixed16 = (unsigned int)(double(0x10000u) * sourceY_s_targetY + 0.5);

	const int targetOffsetX_fixed16 = (int)(double(0x10000u) * ((sourceX_s_targetX * 0.5) - 0.5) + 0.5);
	const int targetOffsetY_fixed16 = (int)(double(0x10000u) * ((sourceY_s_targetY * 0.5) - 0.5) + 0.5);

	// we store 4 integers: [sourceX_T_targetX_fixed16, sourceX_T_targetX_fixed16, sourceX_T_targetX_fixed16, sourceX_T_targetX_fixed16]
	const uint32x4_t m128_u_sourceX_T_targetX_fixed16 = vdupq_n_u32(sourceX_T_targetX_fixed16);

	// we store 4 integers: [sourceX_T_targetX_fixed16, sourceX_T_targetX_fixed16, sourceX_T_targetX_fixed16, sourceX_T_targetX_fixed16]
	const int32x4_t m128_s_targetOffsetX_fixed16 = vdupq_n_s32(targetOffsetX_fixed16);

	// we store 4 integers: [sourceWidth - 2, sourceWidth - 2, sourceWidth - 2, sourceWidth - 2]
	const uint32x4_t m128_u_sourceWidth_2 = vdupq_n_u32(sourceWidth - 2u);

	// we store 4 integers: [0, 0, 0, 0]
	const int32x4_t m128_s_zero = vdupq_n_s32(0);

	const unsigned int u_0123[4] = {0u, 1u, 2u, 3u};
	const uint32x4_t m128_u_0123 = vld1q_u32(u_0123);

	for (unsigned int y = firstTargetRow; y < firstTargetRow + numberTargetRows; ++y)
	{
		const unsigned int sourceY_fixed16 = minmax<int>(0, targetOffsetY_fixed16 + int(sourceY_T_targetY_fixed16 * y), (sourceHeight - 1u) << 16u);

		const unsigned int sourceRowTop = sourceY_fixed16 >> 16u; // we must not round here
		const unsigned int factorBottom_fixed16 = sourceY_fixed16 & 0x0000FFFFu;
		const unsigned int factorBottom = factorBottom_fixed16 >> 9u;

		const uint8x8_t m64_u_factorsBottom = vdup_n_u8(factorBottom);
		// factorTop = 128 - factorBottom
		const uint8x8_t m64_u_factorsTop = vdup_n_u8(128u - factorBottom);

		const unsigned int sourceRowBottom = min(sourceRowTop + 1u, sourceHeight - 1u);

		const PixelType* const sourceTopRowPixelData = sourcePixelData + sourceRowTop * sourceWidth;
		const PixelType* const sourceBottomRowPixelData = sourcePixelData + sourceRowBottom * sourceWidth;

		for (unsigned int x = 0; x < targetWidth; x += 8u)
		{
			if (x + 8u > targetWidth)
			{
				// the last iteration will not fit into the output frame,
				// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

				ocean_assert(x >= 8u && targetWidth > 8u);
				const unsigned int newX = targetWidth - 8u;

				ocean_assert(x > newX);
				targetPixelData -= x - newX;

				x = newX;

				// the for loop will stop after this iteration
				ocean_assert(!(x + 8u < targetWidth));
			}


			// we need four successive x coordinate floats:
			// [x + 3, x + 2, x + 1; x + 0]
			const uint32x4_t m128_u_x_0123 = vaddq_u32(vdupq_n_u32(x), m128_u_0123);
			const uint32x4_t m128_u_x_4567 = vaddq_u32(vdupq_n_u32(x + 4u), m128_u_0123);

			// we calculate the four source locations for our four target locations
			const int32x4_t m128_s_sourceX_0123_fixed16 = vmaxq_s32(m128_s_zero, vaddq_s32(m128_s_targetOffsetX_fixed16, vreinterpretq_s32_u32(vmulq_u32(m128_u_sourceX_T_targetX_fixed16, m128_u_x_0123))));
			const uint32x4_t m128_u_sourceX_0123_fixed16 = vreinterpretq_u32_s32(m128_s_sourceX_0123_fixed16);

			const int32x4_t m128_s_sourceX_4567_fixed16 = vmaxq_s32(m128_s_zero, vaddq_s32(m128_s_targetOffsetX_fixed16, vreinterpretq_s32_u32(vmulq_u32(m128_u_sourceX_T_targetX_fixed16, m128_u_x_4567))));
			const uint32x4_t m128_u_sourceX_4567_fixed16 = vreinterpretq_u32_s32(m128_s_sourceX_4567_fixed16);

			// now we determine the pixel/integer accurate source locations
			// m128_u_left = min(floor(m128_f_sourceX), sourceWidth - 2)
			const uint32x4_t m128_u_left_0123 = vminq_u32(vshrq_n_u32(m128_u_sourceX_0123_fixed16, 16), m128_u_sourceWidth_2); // not vrshrq_n_u32 as we must not round here
			const uint32x4_t m128_u_left_4567 = vminq_u32(vshrq_n_u32(m128_u_sourceX_4567_fixed16, 16), m128_u_sourceWidth_2);

			// we store the offsets we have calculated
			vst1q_u32(leftOffsets + 0, m128_u_left_0123);
			vst1q_u32(leftOffsets + 4, m128_u_left_4567);



			// we load the left and the right pixels into an intermediate buffer
			// with following pattern (with top-left TL, and top-right TR):
			//    F   E   D   C     B   A   9   8     7   6   5   4     3   2   1   0
			// [TR3 TR3 TL3 TL3   TR2 TR2 TL2 TL2   TR1 TR1 TL1 TL1   TR0 TR0 TL0 TL0]
			// [TR7 TR7 TL7 TL7   TR6 TR6 TL6 TL6   TR5 TR5 TL5 TL5   TR4 TR4 TL4 TL4]

			for (unsigned int n = 0u; n < 8u; ++n)
			{
				topPixels[n] = *(unsigned int*)(sourceTopRowPixelData + leftOffsets[n]);
			}

			const uint16x8_t m128_topPixels_0123 = vreinterpretq_u16_u32(vld1q_u32(topPixels + 0));
			const uint16x8_t m128_topPixels_4567 = vreinterpretq_u16_u32(vld1q_u32(topPixels + 4));

			for (unsigned int n = 0u; n < 8u; ++n)
			{
				bottomPixels[n] = *(unsigned int*)(sourceBottomRowPixelData + leftOffsets[n]);
			}

			const uint16x8_t m128_bottomPixels_0123 = vreinterpretq_u16_u32(vld1q_u32(bottomPixels + 0));
			const uint16x8_t m128_bottomPixels_4567 = vreinterpretq_u16_u32(vld1q_u32(bottomPixels + 4));


			// we determine the multiplication factors for the right pixels - which are already stored in the lower 16 bits
			// we need an accuracy of 7 bits (values between 0 and 128):
			//  76 54 32 10
			// [F3 F2 F1 F0]
			const uint16x4_t m64_u_factorsRight_0123 = vrshrn_n_u32(vandq_u32(m128_u_sourceX_0123_fixed16, vdupq_n_u32(0x0000FFFFu)), 9);
			const uint16x4_t m64_u_factorsRight_4567 = vrshrn_n_u32(vandq_u32(m128_u_sourceX_4567_fixed16, vdupq_n_u32(0x0000FFFFu)), 9);

			// as we will have the pixel information de-interleaved, we can store all 8 interpolation factors together into one 8x8 bit register:
			const uint16x8_t m128_u_factorsRight = vcombine_u16(m64_u_factorsRight_0123, m64_u_factorsRight_4567);
			const uint8x8_t m64_u_factorsRight = vqmovn_u16(m128_u_factorsRight);

			// nw we have the interpolation factors for 8 left and 8 right pixels:
			//   7  6  5  4  3  2  1  0
			// [F7 F6 F5 F4 F3 F2 F1 F0]
			const uint8x8_t m64_u_factorsLeft = vsub_u8(vdup_n_u8(128u), m64_u_factorsRight);


			// we de-interleave the top pixels to left and right pixels:
			//    F   E    D   C    B   A    9   8    7   6    5   4    3   2    1   0
			// [TL7 TL7  TL6 TL6  TL5 TL5  TL4 TL4  TL3 TL3  TL2 TL2  TL1 TL1  TL0 TL0]
			// [TR7 TR7  TR6 TR6  TR5 TR5  TR4 TR4  TR3 TR3  TR2 TR2  TR1 TR1  TR0 TR0]
			const uint16x8x2_t m2_128_topPixelsLeftRight = vuzpq_u16(m128_topPixels_0123, m128_topPixels_4567);

			// we de-interleave the pixels again to separate channel 0 and channel 1:
			//               7   6   5   4   3   2   1   0
			// channel 0: [TL7 TL6 TL5 TL4 TL3 TL2 TL1 TL0]
			// channel 1: [TL7 TL6 TL5 TL4 TL3 TL2 TL1 TL0]
			const uint8x8x2_t m2_64_topPixelsLeft_channels_01 = vuzp_u8(vget_low_u8(vreinterpretq_u8_u16(m2_128_topPixelsLeftRight.val[0])), vget_high_u8(vreinterpretq_u8_u16(m2_128_topPixelsLeftRight.val[0])));
			const uint8x8x2_t m2_64_topPixelsRight_channels_01 = vuzp_u8(vget_low_u8(vreinterpretq_u8_u16(m2_128_topPixelsLeftRight.val[1])), vget_high_u8(vreinterpretq_u8_u16(m2_128_topPixelsLeftRight.val[1])));

			const uint8x8_t& m64_topPixelsLeft_channel_0 = m2_64_topPixelsLeft_channels_01.val[0];
			const uint8x8_t& m64_topPixelsLeft_channel_1 = m2_64_topPixelsLeft_channels_01.val[1];

			const uint8x8_t& m64_topPixelsRight_channel_0 = m2_64_topPixelsRight_channels_01.val[0];
			const uint8x8_t& m64_topPixelsRight_channel_1 = m2_64_topPixelsRight_channels_01.val[1];


			// we determine the intermediate interpolation results for the top row (and we narrow down the 16 bit results 8 bit results)
			uint16x8_t m128_muliplication_channel_0 = vmull_u8(m64_topPixelsLeft_channel_0, m64_u_factorsLeft);
			uint16x8_t m128_muliplication_channel_1 = vmull_u8(m64_topPixelsLeft_channel_1, m64_u_factorsLeft);

			m128_muliplication_channel_0 = vmlal_u8(m128_muliplication_channel_0, m64_topPixelsRight_channel_0, m64_u_factorsRight);
			m128_muliplication_channel_1 = vmlal_u8(m128_muliplication_channel_1, m64_topPixelsRight_channel_1, m64_u_factorsRight);

			const uint8x8_t m64_topRow_channel_0 = vrshrn_n_u16(m128_muliplication_channel_0, 7);
			const uint8x8_t m64_topRow_channel_1 = vrshrn_n_u16(m128_muliplication_channel_1, 7);


			// we proceed with the bottom pixels (as we did with the top pixels)
			const uint16x8x2_t m2_128_bottomPixelsLeftRight = vuzpq_u16(m128_bottomPixels_0123, m128_bottomPixels_4567);

			const uint8x8x2_t m2_64_bottomPixelsLeft_channels_01 = vuzp_u8(vget_low_u8(vreinterpretq_u8_u16(m2_128_bottomPixelsLeftRight.val[0])), vget_high_u8(vreinterpretq_u8_u16(m2_128_bottomPixelsLeftRight.val[0])));
			const uint8x8x2_t m2_64_bottomPixelsRight_channels_01 = vuzp_u8(vget_low_u8(vreinterpretq_u8_u16(m2_128_bottomPixelsLeftRight.val[1])), vget_high_u8(vreinterpretq_u8_u16(m2_128_bottomPixelsLeftRight.val[1])));

			const uint8x8_t& m64_bottomPixelsLeft_channel_0 = m2_64_bottomPixelsLeft_channels_01.val[0];
			const uint8x8_t& m64_bottomPixelsLeft_channel_1 = m2_64_bottomPixelsLeft_channels_01.val[1];

			const uint8x8_t& m64_bottomPixelsRight_channel_0 = m2_64_bottomPixelsRight_channels_01.val[0];
			const uint8x8_t& m64_bottomPixelsRight_channel_1 = m2_64_bottomPixelsRight_channels_01.val[1];


			// we determine the intermediate interpolation results for the bottom row (and we narrow down the 16 bit results 8 bit results)
			m128_muliplication_channel_0 = vmull_u8(m64_bottomPixelsLeft_channel_0, m64_u_factorsLeft);
			m128_muliplication_channel_1 = vmull_u8(m64_bottomPixelsLeft_channel_1, m64_u_factorsLeft);

			m128_muliplication_channel_0 = vmlal_u8(m128_muliplication_channel_0, m64_bottomPixelsRight_channel_0, m64_u_factorsRight);
			m128_muliplication_channel_1 = vmlal_u8(m128_muliplication_channel_1, m64_bottomPixelsRight_channel_1, m64_u_factorsRight);

			const uint8x8_t m64_bottomRow_channel_0 = vrshrn_n_u16(m128_muliplication_channel_0, 7);
			const uint8x8_t m64_bottomRow_channel_1 = vrshrn_n_u16(m128_muliplication_channel_1, 7);


			// finnally we determine the interpolation result between top and bottom row
			m128_muliplication_channel_0 = vmull_u8(m64_topRow_channel_0, m64_u_factorsTop);
			m128_muliplication_channel_1 = vmull_u8(m64_topRow_channel_1, m64_u_factorsTop);

			m128_muliplication_channel_0 = vmlal_u8(m128_muliplication_channel_0, m64_bottomRow_channel_0, m64_u_factorsBottom);
			m128_muliplication_channel_1 = vmlal_u8(m128_muliplication_channel_1, m64_bottomRow_channel_1, m64_u_factorsBottom);


			// we narrow down the interpolation results and we store them
			uint8x8x2_t m2_64_result;
			m2_64_result.val[0] = vrshrn_n_u16(m128_muliplication_channel_0, 7);
			m2_64_result.val[1] = vrshrn_n_u16(m128_muliplication_channel_1, 7);

			// we write back the results and interleave them automatically
			vst2_u8((uint8_t*)targetPixelData, m2_64_result);

			targetPixelData += 8;
		}

		// we need to process the last pixel again, as this pixel may have received wrong interpolation factors as we always load two successive pixels into our NEON registers
		// **TODO** this is just a temporary solution, check how we can avoid this additional step

		const unsigned int firstInvalidTargetX = (((sourceWidth - 1u) << 16u) - targetOffsetX_fixed16) / sourceX_T_targetX_fixed16;

		for (unsigned int x = firstInvalidTargetX; x < targetWidth; ++x)
		{
			const unsigned int lastSourcePixelPosition_fixed16 = minmax<int>(0, targetOffsetX_fixed16 + int(sourceX_T_targetX_fixed16 * x), (sourceWidth - 1u) << 16u);

			const unsigned int lastSourcePixelLeft = lastSourcePixelPosition_fixed16 >> 16u;
			ocean_assert(lastSourcePixelLeft < sourceWidth);
			const unsigned int lastSourcePixelRight = min(lastSourcePixelLeft + 1u, sourceWidth - 1u);

			const unsigned int factorRight_fixed16 = lastSourcePixelPosition_fixed16 & 0x0000FFFFu;

			const unsigned int factorRight = factorRight_fixed16 >> 9u;
			const unsigned int factorLeft = 128u - factorRight;

			for (unsigned int c = 0u; c < 2u; ++c)
			{
				((uint8_t*)(targetPixelData - (targetWidth - x)))[c] = ((((const uint8_t*)(sourceTopRowPixelData + lastSourcePixelLeft))[c] * factorLeft + ((const uint8_t*)(sourceTopRowPixelData + lastSourcePixelRight))[c] * factorRight) * (128u - factorBottom)
																			  + (((const uint8_t*)(sourceBottomRowPixelData + lastSourcePixelLeft))[c] * factorLeft + ((const uint8_t*)(sourceBottomRowPixelData + lastSourcePixelRight))[c] * factorRight) * factorBottom + 8192u) >> 14u;
			}
		}
	}
}

#endif // OCEAN_WE_KEEP_ALSO_THIS_SLOW_IMPLEMENTATION_SHOWING_A_SLIGHTLY_DIFFERENT_APPROACH

#ifdef OCEAN_WE_KEEP_THIS_IMPLEMENTATION_AS_WE_NEED_THIS_TO_FOR_A_NEW_NEON_IMPLEMENTATION

template <>
inline void FrameInterpolatorBilinear::scale8BitPerChannelSubset7BitPrecisionNEON<3u, 8u>(const uint8_t* source, uint8_t* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const double sourceX_s_targetX, const double sourceY_s_targetY, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstTargetRow, const unsigned int numberTargetRows)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(sourceWidth >= 2u && sourceWidth <= 65535u);
	ocean_assert(sourceHeight >= 1u && sourceHeight <= 65535u);
	ocean_assert(targetWidth >= 8u && targetWidth <= 65535u);
	ocean_assert(targetHeight >= 1u && targetHeight <= 65535u);
	ocean_assert(sourceX_s_targetX > 0.0 && sourceY_s_targetY > 0.0);

	ocean_assert(sourcePaddingElements == 0u); // not supported
	ocean_assert(targetPaddingElements == 0u);

	typedef typename DataType<uint8_t, 3u>::Type PixelType;

	PixelType* targetPixelData = (PixelType*)target + firstTargetRow * targetWidth;
	const PixelType* const sourcePixelData = (const PixelType*)source;

	// our offset values for the eight left pixels in relation to the first pixel of the row
	unsigned int leftOffsets[8];

	// this function uses fixed point numbers with 16 bit for the calculation of the interpolation positions and factors:
	// fixedPointLocation = floatLocation * 2^16
	//
	// [FEDCBA98, 76543210]
	// [pixel   , subpixel]
	//
	// fixedPointLocation = pixel + subpixel / 2^16
	//
	// Thus, the upper 16 bit represent the location of e.g., the left pixel (for the linear interpolation)
	// while the lower 16 bit represent one of both interpolation factors (and 2^16 - subpixel represents the second interpolation factor)

	const unsigned int sourceX_T_targetX_fixed16 = (unsigned int)(double(0x10000u) * sourceX_s_targetX + 0.5);
	const unsigned int sourceY_T_targetY_fixed16 = (unsigned int)(double(0x10000u) * sourceY_s_targetY + 0.5);

	const int targetOffsetX_fixed16 = (int)(double(0x10000u) * ((sourceX_s_targetX * 0.5) - 0.5) + 0.5);
	const int targetOffsetY_fixed16 = (int)(double(0x10000u) * ((sourceY_s_targetY * 0.5) - 0.5) + 0.5);

	// we store 4 integers: [sourceX_T_targetX_fixed16, sourceX_T_targetX_fixed16, sourceX_T_targetX_fixed16, sourceX_T_targetX_fixed16]
	const uint32x4_t m128_u_sourceX_T_targetX_fixed16 = vdupq_n_u32(sourceX_T_targetX_fixed16);

	// we store 4 integers: [sourceX_T_targetX_fixed16, sourceX_T_targetX_fixed16, sourceX_T_targetX_fixed16, sourceX_T_targetX_fixed16]
	const int32x4_t m128_s_targetOffsetX_fixed16 = vdupq_n_s32(targetOffsetX_fixed16);

	// we store 4 integers: [sourceWidth - 2, sourceWidth - 2, sourceWidth - 2, sourceWidth - 2]
	const uint32x4_t m128_u_sourceWidth_2 = vdupq_n_u32(sourceWidth - 2u);

	// we store 4 integers: [0, 0, 0, 0]
	const int32x4_t m128_s_zero = vdupq_n_s32(0);

	const unsigned int u_0123[4] = {0u, 1u, 2u, 3u};
	const uint32x4_t m128_u_0123 = vld1q_u32(u_0123);

	for (unsigned int y = firstTargetRow; y < firstTargetRow + numberTargetRows; ++y)
	{
		const unsigned int sourceY_fixed16 = minmax<int>(0, targetOffsetY_fixed16 + int(sourceY_T_targetY_fixed16 * y), (sourceHeight - 1u) << 16u);

		const unsigned int sourceRowTop = sourceY_fixed16 >> 16u; // we must not round here
		const unsigned int factorBottom_fixed16 = sourceY_fixed16 & 0x0000FFFFu;
		const unsigned int factorBottom = factorBottom_fixed16 >> 9u;

		const uint8x8_t m64_u_factorsBottom = vdup_n_u8(factorBottom);
		// factorTop = 128 - factorBottom
		const uint8x8_t m64_u_factorsTop = vdup_n_u8(128u - factorBottom);

		const unsigned int sourceRowBottom = min(sourceRowTop + 1u, sourceHeight - 1u);

		const PixelType* const sourceTopRowPixelData = sourcePixelData + sourceRowTop * sourceWidth;
		const PixelType* const sourceBottomRowPixelData = sourcePixelData + sourceRowBottom * sourceWidth;

		for (unsigned int x = 0; x < targetWidth; x += 8u)
		{
			if (x + 8u > targetWidth)
			{
				// the last iteration will not fit into the output frame,
				// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

				ocean_assert(x >= 8u && targetWidth > 8u);
				const unsigned int newX = targetWidth - 8u;

				ocean_assert(x > newX);
				targetPixelData -= x - newX;

				x = newX;

				// the for loop will stop after this iteration
				ocean_assert(!(x + 8u < targetWidth));
			}


			// we need four successive x coordinate floats:
			// [x + 3, x + 2, x + 1; x + 0]
			const uint32x4_t m128_u_x_0123 = vaddq_u32(vdupq_n_u32(x), m128_u_0123);
			const uint32x4_t m128_u_x_4567 = vaddq_u32(vdupq_n_u32(x + 4u), m128_u_0123);

			// we calculate the four source locations for our four target locations
			const int32x4_t m128_s_sourceX_0123_fixed16 = vmaxq_s32(m128_s_zero, vaddq_s32(m128_s_targetOffsetX_fixed16, vreinterpretq_s32_u32(vmulq_u32(m128_u_sourceX_T_targetX_fixed16, m128_u_x_0123))));
			const uint32x4_t m128_u_sourceX_0123_fixed16 = vreinterpretq_u32_s32(m128_s_sourceX_0123_fixed16);

			const int32x4_t m128_s_sourceX_4567_fixed16 = vmaxq_s32(m128_s_zero, vaddq_s32(m128_s_targetOffsetX_fixed16, vreinterpretq_s32_u32(vmulq_u32(m128_u_sourceX_T_targetX_fixed16, m128_u_x_4567))));
			const uint32x4_t m128_u_sourceX_4567_fixed16 = vreinterpretq_u32_s32(m128_s_sourceX_4567_fixed16);

			// now we determine the pixel/integer accurate source locations
			// m128_u_left = min(floor(m128_f_sourceX), sourceWidth - 2)
			const uint32x4_t m128_u_left_0123 = vminq_u32(vshrq_n_u32(m128_u_sourceX_0123_fixed16, 16), m128_u_sourceWidth_2); // not vrshrq_n_u32 as we must not round here
			const uint32x4_t m128_u_left_4567 = vminq_u32(vshrq_n_u32(m128_u_sourceX_4567_fixed16, 16), m128_u_sourceWidth_2);

			// we store the offsets we have calculated
			vst1q_u32(leftOffsets + 0, m128_u_left_0123);
			vst1q_u32(leftOffsets + 4, m128_u_left_4567);



			// we load the individal pixels to our four (de-interleaved) 8x8 bit registers (we do this for the top-left and top-right pixels)
			// note: loading of each pixel individually is significantly slower than loading two neighboring pixels within one iteration

			uint8x8x3_t topLeftPixels;
			uint8x8x3_t topRightPixels;

			topLeftPixels = vld3_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[0u] + 0), topLeftPixels, 0);
			topRightPixels = vld3_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[0u] + 1), topRightPixels, 0);

			topLeftPixels = vld3_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[1u] + 0), topLeftPixels, 1);
			topRightPixels = vld3_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[1u] + 1), topRightPixels, 1);

			topLeftPixels = vld3_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[2u] + 0), topLeftPixels, 2);
			topRightPixels = vld3_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[2u] + 1), topRightPixels, 2);

			topLeftPixels = vld3_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[3u] + 0), topLeftPixels, 3);
			topRightPixels = vld3_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[3u] + 1), topRightPixels, 3);

			topLeftPixels = vld3_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[4u] + 0), topLeftPixels, 4);
			topRightPixels = vld3_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[4u] + 1), topRightPixels, 4);

			topLeftPixels = vld3_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[5u] + 0), topLeftPixels, 5);
			topRightPixels = vld3_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[5u] + 1), topRightPixels, 5);

			topLeftPixels = vld3_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[6u] + 0), topLeftPixels, 6);
			topRightPixels = vld3_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[6u] + 1), topRightPixels, 6);

			topLeftPixels = vld3_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[7u] + 0), topLeftPixels, 7);
			topRightPixels = vld3_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[7u] + 1), topRightPixels, 7);


			// we load the individal pixels to our four (de-interleaved) 8x8 bit registers (we do this for the bottom-left and bottom-right pixels)

			uint8x8x3_t bottomLeftPixels;
			uint8x8x3_t bottomRightPixels;

			bottomLeftPixels = vld3_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[0u] + 0), bottomLeftPixels, 0);
			bottomRightPixels = vld3_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[0u] + 1), bottomRightPixels, 0);

			bottomLeftPixels = vld3_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[1u] + 0), bottomLeftPixels, 1);
			bottomRightPixels = vld3_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[1u] + 1), bottomRightPixels, 1);

			bottomLeftPixels = vld3_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[2u] + 0), bottomLeftPixels, 2);
			bottomRightPixels = vld3_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[2u] + 1), bottomRightPixels, 2);

			bottomLeftPixels = vld3_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[3u] + 0), bottomLeftPixels, 3);
			bottomRightPixels = vld3_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[3u] + 1), bottomRightPixels, 3);

			bottomLeftPixels = vld3_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[4u] + 0), bottomLeftPixels, 4);
			bottomRightPixels = vld3_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[4u] + 1), bottomRightPixels, 4);

			bottomLeftPixels = vld3_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[5u] + 0), bottomLeftPixels, 5);
			bottomRightPixels = vld3_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[5u] + 1), bottomRightPixels, 5);

			bottomLeftPixels = vld3_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[6u] + 0), bottomLeftPixels, 6);
			bottomRightPixels = vld3_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[6u] + 1), bottomRightPixels, 6);

			bottomLeftPixels = vld3_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[7u] + 0), bottomLeftPixels, 7);
			bottomRightPixels = vld3_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[7u] + 1), bottomRightPixels, 7);



			// we determine the multiplication factors for the right pixels - which are already stored in the lower 16 bits
			// we need an accuracy of 7 bits (values between 0 and 128):
			//  76 54 32 10
			// [F3 F2 F1 F0]
			const uint16x4_t m64_u_factorsRight_0123 = vrshrn_n_u32(vandq_u32(m128_u_sourceX_0123_fixed16, vdupq_n_u32(0x0000FFFFu)), 9);
			const uint16x4_t m64_u_factorsRight_4567 = vrshrn_n_u32(vandq_u32(m128_u_sourceX_4567_fixed16, vdupq_n_u32(0x0000FFFFu)), 9);

			// as we have the pixel information de-interleaved, we can store all 8 interpolation factors together into one 8x8 bit register:
			const uint16x8_t m128_u_factorsRight = vcombine_u16(m64_u_factorsRight_0123, m64_u_factorsRight_4567);
			const uint8x8_t m64_u_factorsRight = vqmovn_u16(m128_u_factorsRight);
			const uint8x8_t m64_u_factorsLeft = vsub_u8(vdup_n_u8(128u), m64_u_factorsRight);



			// we determine the intermediate interpolation results for the top row (and we narrow down the 16 bit results 8 bit results)
			uint16x8_t m128_muliplicationChannel_0 = vmull_u8(topLeftPixels.val[0], m64_u_factorsLeft);
			uint16x8_t m128_muliplicationChannel_1 = vmull_u8(topLeftPixels.val[1], m64_u_factorsLeft);
			uint16x8_t m128_muliplicationChannel_2 = vmull_u8(topLeftPixels.val[2], m64_u_factorsLeft);

			m128_muliplicationChannel_0 = vmlal_u8(m128_muliplicationChannel_0, topRightPixels.val[0], m64_u_factorsRight);
			m128_muliplicationChannel_1 = vmlal_u8(m128_muliplicationChannel_1, topRightPixels.val[1], m64_u_factorsRight);
			m128_muliplicationChannel_2 = vmlal_u8(m128_muliplicationChannel_2, topRightPixels.val[2], m64_u_factorsRight);

			uint8x8_t m64_topRowChannel_0 = vrshrn_n_u16(m128_muliplicationChannel_0, 7);
			uint8x8_t m64_topRowChannel_1 = vrshrn_n_u16(m128_muliplicationChannel_1, 7);
			uint8x8_t m64_topRowChannel_2 = vrshrn_n_u16(m128_muliplicationChannel_2, 7);



			// we determine the intermediate interpolation results for the bottom row (and we narrow down the 16 bit results 8 bit results)
			m128_muliplicationChannel_0 = vmull_u8(bottomLeftPixels.val[0], m64_u_factorsLeft);
			m128_muliplicationChannel_1 = vmull_u8(bottomLeftPixels.val[1], m64_u_factorsLeft);
			m128_muliplicationChannel_2 = vmull_u8(bottomLeftPixels.val[2], m64_u_factorsLeft);

			m128_muliplicationChannel_0 = vmlal_u8(m128_muliplicationChannel_0, bottomRightPixels.val[0], m64_u_factorsRight);
			m128_muliplicationChannel_1 = vmlal_u8(m128_muliplicationChannel_1, bottomRightPixels.val[1], m64_u_factorsRight);
			m128_muliplicationChannel_2 = vmlal_u8(m128_muliplicationChannel_2, bottomRightPixels.val[2], m64_u_factorsRight);

			uint8x8_t m64_bottomRowChannel_0 = vrshrn_n_u16(m128_muliplicationChannel_0, 7);
			uint8x8_t m64_bottomRowChannel_1 = vrshrn_n_u16(m128_muliplicationChannel_1, 7);
			uint8x8_t m64_bottomRowChannel_2 = vrshrn_n_u16(m128_muliplicationChannel_2, 7);



			// finnally we determine the interpolation result between top and bottom row
			m128_muliplicationChannel_0 = vmull_u8(m64_topRowChannel_0, m64_u_factorsTop);
			m128_muliplicationChannel_1 = vmull_u8(m64_topRowChannel_1, m64_u_factorsTop);
			m128_muliplicationChannel_2 = vmull_u8(m64_topRowChannel_2, m64_u_factorsTop);

			m128_muliplicationChannel_0 = vmlal_u8(m128_muliplicationChannel_0, m64_bottomRowChannel_0, m64_u_factorsBottom);
			m128_muliplicationChannel_1 = vmlal_u8(m128_muliplicationChannel_1, m64_bottomRowChannel_1, m64_u_factorsBottom);
			m128_muliplicationChannel_2 = vmlal_u8(m128_muliplicationChannel_2, m64_bottomRowChannel_2, m64_u_factorsBottom);


			// we narrow down the interpolation results and we store them
			uint8x8x3_t result;
			result.val[0] = vrshrn_n_u16(m128_muliplicationChannel_0, 7);
			result.val[1] = vrshrn_n_u16(m128_muliplicationChannel_1, 7);
			result.val[2] = vrshrn_n_u16(m128_muliplicationChannel_2, 7);

			// we write back the results and interleave them automatically
			vst3_u8((uint8_t*)targetPixelData, result);

			targetPixelData += 8;
		}

		// we need to process the last pixel again, as this pixel may have received wrong interpolation factors as we always load two successive pixels into our NEON registers
		// **TODO** this is just a temporary solution, check how we can avoid this additional step

		const unsigned int firstInvalidTargetX = (((sourceWidth - 1u) << 16u) - targetOffsetX_fixed16) / sourceX_T_targetX_fixed16;

		for (unsigned int x = firstInvalidTargetX; x < targetWidth; ++x)
		{
			const unsigned int lastSourcePixelPosition_fixed16 = minmax<int>(0, targetOffsetX_fixed16 + int(sourceX_T_targetX_fixed16 * x), (sourceWidth - 1u) << 16u);

			const unsigned int lastSourcePixelLeft = lastSourcePixelPosition_fixed16 >> 16u;
			ocean_assert(lastSourcePixelLeft < sourceWidth);
			const unsigned int lastSourcePixelRight = min(lastSourcePixelLeft + 1u, sourceWidth - 1u);

			const unsigned int factorRight_fixed16 = lastSourcePixelPosition_fixed16 & 0x0000FFFFu;

			const unsigned int factorRight = factorRight_fixed16 >> 9u;
			const unsigned int factorLeft = 128u - factorRight;

			for (unsigned int c = 0u; c < 3u; ++c)
			{
				((uint8_t*)(targetPixelData - (targetWidth - x)))[c] = ((((const uint8_t*)(sourceTopRowPixelData + lastSourcePixelLeft))[c] * factorLeft + ((const uint8_t*)(sourceTopRowPixelData + lastSourcePixelRight))[c] * factorRight) * (128u - factorBottom)
																			  + (((const uint8_t*)(sourceBottomRowPixelData + lastSourcePixelLeft))[c] * factorLeft + ((const uint8_t*)(sourceBottomRowPixelData + lastSourcePixelRight))[c] * factorRight) * factorBottom + 8192u) >> 14u;
			}
		}
	}
}

#endif // OCEAN_WE_KEEP_THIS_IMPLEMENTATION_AS_WE_NEED_THIS_TO_FOR_A_NEW_NEON_IMPLEMENTATION

#ifdef OCEAN_WE_KEEP_ALSO_THIS_SLOW_IMPLEMENTATION_SHOWING_A_MORE_GENERIC_APPROACH

/// \cond DOXYGEN_DO_NOT_DOCUMENT

template <>
inline void FrameInterpolatorBilinear::resize8BitPerChannelSubset7BitPrecisionNEON<4u, 8u>(const uint8_t* source, uint8_t* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int firstTargetRow, const unsigned int numberTargetRows)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(sourceWidth >= 2u && sourceWidth <= 65535u);
	ocean_assert(sourceHeight >= 1u && sourceHeight <= 65535u);
	ocean_assert(targetWidth >= 8u && targetWidth <= 65535u);
	ocean_assert(targetHeight >= 1u && targetHeight <= 65535u);
	ocean_assert(sourceX_s_targetX > 0.0 && sourceY_s_targetY > 0.0);

	ocean_assert(sourcePaddingElements == 0u); // not supported
	ocean_assert(targetPaddingElements == 0u);

	typedef typename DataType<uint8_t, 4u>::Type PixelType;

	PixelType* targetPixelData = (PixelType*)target + firstTargetRow * targetWidth;
	const PixelType* const sourcePixelData = (const PixelType*)source;

	// our offset values for the eight left pixels in relation to the first pixel of the row
	unsigned int leftOffsets[8];

	// this function uses fixed point numbers with 16 bit for the calculation of const unsigned int sourceX_T_targetX_fixed16 = (unsigned int)(double(0x10000u) * sourceX_s_targetX + 0.5);
	const unsigned int sourceY_T_targetY_fixed16 = (unsigned int)(double(0x10000u) * sourceY_s_targetY + 0.5);

	// this function uses fixed point numbers with 16 bit for the calculation of the interpolation positions and factors:
	// fixedPointLocation = floatLocation * 2^16
	//
	// [FEDCBA98, 76543210]
	// [pixel   , subpixel]
	//
	// fixedPointLocation = pixel + subpixel / 2^16
	//
	// Thus, the upper 16 bit represent the location of e.g., the left pixel (for the linear interpolation)
	// while the lower 16 bit represent one of both interpolation factors (and 2^16 - subpixel represents the second interpolation factor)

	const unsigned int sourceX_T_targetX_fixed16 = (unsigned int)(double(0x10000u) * sourceX_s_targetX + 0.5);
	const unsigned int sourceY_T_targetY_fixed16 = (unsigned int)(double(0x10000u) * sourceY_s_targetY + 0.5);

	const int targetOffsetX_fixed16 = (int)(double(0x10000u) * ((sourceX_s_targetX * 0.5) - 0.5) + 0.5);
	const int targetOffsetY_fixed16 = (int)(double(0x10000u) * ((sourceY_s_targetY * 0.5) - 0.5) + 0.5);

	// we store 4 integers: [sourceX_T_targetX_fixed16, sourceX_T_targetX_fixed16, sourceX_T_targetX_fixed16, sourceX_T_targetX_fixed16]
	const uint32x4_t m128_u_sourceX_T_targetX_fixed16 = vdupq_n_u32(sourceX_T_targetX_fixed16);

	// we store 4 integers: [sourceX_T_targetX_fixed16, sourceX_T_targetX_fixed16, sourceX_T_targetX_fixed16, sourceX_T_targetX_fixed16]
	const int32x4_t m128_s_targetOffsetX_fixed16 = vdupq_n_s32(targetOffsetX_fixed16);

	// we store 4 integers: [sourceWidth - 2, sourceWidth - 2, sourceWidth - 2, sourceWidth - 2]
	const uint32x4_t m128_u_sourceWidth_2 = vdupq_n_u32(sourceWidth - 2u);

	// we store 4 integers: [0, 0, 0, 0]
	const int32x4_t m128_s_zero = vdupq_n_s32(0);

	const unsigned int u_0123[4] = {0u, 1u, 2u, 3u};
	const uint32x4_t m128_u_0123 = vld1q_u32(u_0123);

	for (unsigned int y = firstTargetRow; y < firstTargetRow + numberTargetRows; ++y)
	{
		const unsigned int sourceY_fixed16 = minmax<int>(0, targetOffsetY_fixed16 + int(sourceY_T_targetY_fixed16 * y), (sourceHeight - 1u) << 16u);

		const unsigned int sourceRowTop = sourceY_fixed16 >> 16u; // we must not round here
		const unsigned int factorBottom_fixed16 = sourceY_fixed16 & 0x0000FFFFu;
		const unsigned int factorBottom = factorBottom_fixed16 >> 9u;

		const uint8x8_t m64_u_factorsBottom = vdup_n_u8(factorBottom);
		// factorTop = 128 - factorBottom
		const uint8x8_t m64_u_factorsTop = vdup_n_u8(128u - factorBottom);

		const unsigned int sourceRowBottom = min(sourceRowTop + 1u, sourceHeight - 1u);

		const PixelType* const sourceTopRowPixelData = sourcePixelData + sourceRowTop * sourceWidth;
		const PixelType* const sourceBottomRowPixelData = sourcePixelData + sourceRowBottom * sourceWidth;

		for (unsigned int x = 0; x < targetWidth; x += 8u)
		{
			if (x + 8u > targetWidth)
			{
				// the last iteration will not fit into the output frame,
				// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

				ocean_assert(x >= 8u && targetWidth > 8u);
				const unsigned int newX = targetWidth - 8u;

				ocean_assert(x > newX);
				targetPixelData -= x - newX;

				x = newX;

				// the for loop will stop after this iteration
				ocean_assert(!(x + 8u < targetWidth));
			}


			// we need four successive x coordinate floats:
			// [x + 3, x + 2, x + 1; x + 0]
			const uint32x4_t m128_u_x_0123 = vaddq_u32(vdupq_n_u32(x), m128_u_0123);
			const uint32x4_t m128_u_x_4567 = vaddq_u32(vdupq_n_u32(x + 4u), m128_u_0123);

			// we calculate the four source locations for our four target locations
			const int32x4_t m128_s_sourceX_0123_fixed16 = vmaxq_s32(m128_s_zero, vaddq_s32(m128_s_targetOffsetX_fixed16, vreinterpretq_s32_u32(vmulq_u32(m128_u_sourceX_T_targetX_fixed16, m128_u_x_0123))));
			const uint32x4_t m128_u_sourceX_0123_fixed16 = vreinterpretq_u32_s32(m128_s_sourceX_0123_fixed16);

			const int32x4_t m128_s_sourceX_4567_fixed16 = vmaxq_s32(m128_s_zero, vaddq_s32(m128_s_targetOffsetX_fixed16, vreinterpretq_s32_u32(vmulq_u32(m128_u_sourceX_T_targetX_fixed16, m128_u_x_4567))));
			const uint32x4_t m128_u_sourceX_4567_fixed16 = vreinterpretq_u32_s32(m128_s_sourceX_4567_fixed16);

			// now we determine the pixel/integer accurate source locations
			// m128_u_left = min(floor(m128_f_sourceX), sourceWidth - 2)
			const uint32x4_t m128_u_left_0123 = vminq_u32(vshrq_n_u32(m128_u_sourceX_0123_fixed16, 16), m128_u_sourceWidth_2); // not vrshrq_n_u32 as we must not round here
			const uint32x4_t m128_u_left_4567 = vminq_u32(vshrq_n_u32(m128_u_sourceX_4567_fixed16, 16), m128_u_sourceWidth_2);

			// we store the offsets we have calculated
			vst1q_u32(leftOffsets + 0, m128_u_left_0123);
			vst1q_u32(leftOffsets + 4, m128_u_left_4567);



			// we load the individal pixels to our four (de-interleaved) 8x8 bit registers (we do this for the top-left and top-right pixels)
			// note: loading of each pixel individually is significantly slower than loading two neighboring pixels within one iteration

			uint8x8x4_t topLeftPixels;
			uint8x8x4_t topRightPixels;

			topLeftPixels = vld4_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[0u] + 0), topLeftPixels, 0);
			topRightPixels = vld4_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[0u] + 1), topRightPixels, 0);

			topLeftPixels = vld4_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[1u] + 0), topLeftPixels, 1);
			topRightPixels = vld4_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[1u] + 1), topRightPixels, 1);

			topLeftPixels = vld4_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[2u] + 0), topLeftPixels, 2);
			topRightPixels = vld4_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[2u] + 1), topRightPixels, 2);

			topLeftPixels = vld4_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[3u] + 0), topLeftPixels, 3);
			topRightPixels = vld4_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[3u] + 1), topRightPixels, 3);

			topLeftPixels = vld4_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[4u] + 0), topLeftPixels, 4);
			topRightPixels = vld4_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[4u] + 1), topRightPixels, 4);

			topLeftPixels = vld4_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[5u] + 0), topLeftPixels, 5);
			topRightPixels = vld4_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[5u] + 1), topRightPixels, 5);

			topLeftPixels = vld4_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[6u] + 0), topLeftPixels, 6);
			topRightPixels = vld4_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[6u] + 1), topRightPixels, 6);

			topLeftPixels = vld4_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[7u] + 0), topLeftPixels, 7);
			topRightPixels = vld4_lane_u8((uint8_t*)(sourceTopRowPixelData + leftOffsets[7u] + 1), topRightPixels, 7);


			// we load the individal pixels to our four (de-interleaved) 8x8 bit registers (we do this for the bottom-left and bottom-right pixels)

			uint8x8x4_t bottomLeftPixels;
			uint8x8x4_t bottomRightPixels;

			bottomLeftPixels = vld4_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[0u] + 0), bottomLeftPixels, 0);
			bottomRightPixels = vld4_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[0u] + 1), bottomRightPixels, 0);

			bottomLeftPixels = vld4_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[1u] + 0), bottomLeftPixels, 1);
			bottomRightPixels = vld4_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[1u] + 1), bottomRightPixels, 1);

			bottomLeftPixels = vld4_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[2u] + 0), bottomLeftPixels, 2);
			bottomRightPixels = vld4_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[2u] + 1), bottomRightPixels, 2);

			bottomLeftPixels = vld4_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[3u] + 0), bottomLeftPixels, 3);
			bottomRightPixels = vld4_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[3u] + 1), bottomRightPixels, 3);

			bottomLeftPixels = vld4_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[4u] + 0), bottomLeftPixels, 4);
			bottomRightPixels = vld4_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[4u] + 1), bottomRightPixels, 4);

			bottomLeftPixels = vld4_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[5u] + 0), bottomLeftPixels, 5);
			bottomRightPixels = vld4_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[5u] + 1), bottomRightPixels, 5);

			bottomLeftPixels = vld4_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[6u] + 0), bottomLeftPixels, 6);
			bottomRightPixels = vld4_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[6u] + 1), bottomRightPixels, 6);

			bottomLeftPixels = vld4_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[7u] + 0), bottomLeftPixels, 7);
			bottomRightPixels = vld4_lane_u8((uint8_t*)(sourceBottomRowPixelData + leftOffsets[7u] + 1), bottomRightPixels, 7);



			// we determine the multiplication factors for the right pixels - which are already stored in the lower 16 bits
			// we need an accuracy of 7 bits (values between 0 and 128):
			//  76 54 32 10
			// [F3 F2 F1 F0]
			const uint16x4_t m64_u_factorsRight_0123 = vrshrn_n_u32(vandq_u32(m128_u_sourceX_0123_fixed16, vdupq_n_u32(0x0000FFFFu)), 9);
			const uint16x4_t m64_u_factorsRight_4567 = vrshrn_n_u32(vandq_u32(m128_u_sourceX_4567_fixed16, vdupq_n_u32(0x0000FFFFu)), 9);

			// as we have the pixel information de-interleaved, we can store all 8 interpolation factors together into one 8x8 bit register:
			const uint16x8_t m128_u_factorsRight = vcombine_u16(m64_u_factorsRight_0123, m64_u_factorsRight_4567);
			const uint8x8_t m64_u_factorsRight = vqmovn_u16(m128_u_factorsRight);
			const uint8x8_t m64_u_factorsLeft = vsub_u8(vdup_n_u8(128u), m64_u_factorsRight);



			// we determine the intermediate interpolation results for the top row (and we narrow down the 16 bit results 8 bit results)
			uint16x8_t m128_muliplicationChannel_0 = vmull_u8(topLeftPixels.val[0], m64_u_factorsLeft);
			uint16x8_t m128_muliplicationChannel_1 = vmull_u8(topLeftPixels.val[1], m64_u_factorsLeft);
			uint16x8_t m128_muliplicationChannel_2 = vmull_u8(topLeftPixels.val[2], m64_u_factorsLeft);
			uint16x8_t m128_muliplicationChannel_3 = vmull_u8(topLeftPixels.val[3], m64_u_factorsLeft);

			m128_muliplicationChannel_0 = vmlal_u8(m128_muliplicationChannel_0, topRightPixels.val[0], m64_u_factorsRight);
			m128_muliplicationChannel_1 = vmlal_u8(m128_muliplicationChannel_1, topRightPixels.val[1], m64_u_factorsRight);
			m128_muliplicationChannel_2 = vmlal_u8(m128_muliplicationChannel_2, topRightPixels.val[2], m64_u_factorsRight);
			m128_muliplicationChannel_3 = vmlal_u8(m128_muliplicationChannel_3, topRightPixels.val[3], m64_u_factorsRight);

			uint8x8_t m64_topRowChannel_0 = vrshrn_n_u16(m128_muliplicationChannel_0, 7);
			uint8x8_t m64_topRowChannel_1 = vrshrn_n_u16(m128_muliplicationChannel_1, 7);
			uint8x8_t m64_topRowChannel_2 = vrshrn_n_u16(m128_muliplicationChannel_2, 7);
			uint8x8_t m64_topRowChannel_3 = vrshrn_n_u16(m128_muliplicationChannel_3, 7);



			// we determine the intermediate interpolation results for the bottom row (and we narrow down the 16 bit results 8 bit results)
			m128_muliplicationChannel_0 = vmull_u8(bottomLeftPixels.val[0], m64_u_factorsLeft);
			m128_muliplicationChannel_1 = vmull_u8(bottomLeftPixels.val[1], m64_u_factorsLeft);
			m128_muliplicationChannel_2 = vmull_u8(bottomLeftPixels.val[2], m64_u_factorsLeft);
			m128_muliplicationChannel_3 = vmull_u8(bottomLeftPixels.val[3], m64_u_factorsLeft);

			m128_muliplicationChannel_0 = vmlal_u8(m128_muliplicationChannel_0, bottomRightPixels.val[0], m64_u_factorsRight);
			m128_muliplicationChannel_1 = vmlal_u8(m128_muliplicationChannel_1, bottomRightPixels.val[1], m64_u_factorsRight);
			m128_muliplicationChannel_2 = vmlal_u8(m128_muliplicationChannel_2, bottomRightPixels.val[2], m64_u_factorsRight);
			m128_muliplicationChannel_3 = vmlal_u8(m128_muliplicationChannel_3, bottomRightPixels.val[3], m64_u_factorsRight);

			uint8x8_t m64_bottomRowChannel_0 = vrshrn_n_u16(m128_muliplicationChannel_0, 7);
			uint8x8_t m64_bottomRowChannel_1 = vrshrn_n_u16(m128_muliplicationChannel_1, 7);
			uint8x8_t m64_bottomRowChannel_2 = vrshrn_n_u16(m128_muliplicationChannel_2, 7);
			uint8x8_t m64_bottomRowChannel_3 = vrshrn_n_u16(m128_muliplicationChannel_3, 7);



			// finnally we determine the interpolation result between top and bottom row
			m128_muliplicationChannel_0 = vmull_u8(m64_topRowChannel_0, m64_u_factorsTop);
			m128_muliplicationChannel_1 = vmull_u8(m64_topRowChannel_1, m64_u_factorsTop);
			m128_muliplicationChannel_2 = vmull_u8(m64_topRowChannel_2, m64_u_factorsTop);
			m128_muliplicationChannel_3 = vmull_u8(m64_topRowChannel_3, m64_u_factorsTop);

			m128_muliplicationChannel_0 = vmlal_u8(m128_muliplicationChannel_0, m64_bottomRowChannel_0, m64_u_factorsBottom);
			m128_muliplicationChannel_1 = vmlal_u8(m128_muliplicationChannel_1, m64_bottomRowChannel_1, m64_u_factorsBottom);
			m128_muliplicationChannel_2 = vmlal_u8(m128_muliplicationChannel_2, m64_bottomRowChannel_2, m64_u_factorsBottom);
			m128_muliplicationChannel_3 = vmlal_u8(m128_muliplicationChannel_3, m64_bottomRowChannel_3, m64_u_factorsBottom);


			// we narrow down the interpolation results and we store them
			uint8x8x4_t result;
			result.val[0] = vrshrn_n_u16(m128_muliplicationChannel_0, 7);
			result.val[1] = vrshrn_n_u16(m128_muliplicationChannel_1, 7);
			result.val[2] = vrshrn_n_u16(m128_muliplicationChannel_2, 7);
			result.val[3] = vrshrn_n_u16(m128_muliplicationChannel_3, 7);

			// we write back the results and interleave them automatically
			vst4_u8((uint8_t*)targetPixelData, result);

			targetPixelData += 8;
		}

		// we need to process the last pixel again, as this pixel may have received wrong interpolation factors as we always load two successive pixels into our NEON registers
		// **TODO** this is just a temporary solution, check how we can avoid this additional step

		const unsigned int firstInvalidTargetX = (((sourceWidth - 1u) << 16u) - targetOffsetX_fixed16) / sourceX_T_targetX_fixed16;

		for (unsigned int x = firstInvalidTargetX; x < targetWidth; ++x)
		{
			const unsigned int lastSourcePixelPosition_fixed16 = minmax<int>(0, targetOffsetX_fixed16 + int(sourceX_T_targetX_fixed16 * x), (sourceWidth - 1u) << 16u);

			const unsigned int lastSourcePixelLeft = lastSourcePixelPosition_fixed16 >> 16u;
			ocean_assert(lastSourcePixelLeft < sourceWidth);
			const unsigned int lastSourcePixelRight = min(lastSourcePixelLeft + 1u, sourceWidth - 1u);

			const unsigned int factorRight_fixed16 = lastSourcePixelPosition_fixed16 & 0x0000FFFFu;

			const unsigned int factorRight = factorRight_fixed16 >> 9u;
			const unsigned int factorLeft = 128u - factorRight;

			for (unsigned int c = 0u; c < 4u; ++c)
			{
				((uint8_t*)(targetPixelData - (targetWidth - x)))[c] = ((((const uint8_t*)(sourceTopRowPixelData + lastSourcePixelLeft))[c] * factorLeft + ((const uint8_t*)(sourceTopRowPixelData + lastSourcePixelRight))[c] * factorRight) * factorTop
																			  + (((const uint8_t*)(sourceBottomRowPixelData + lastSourcePixelLeft))[c] * factorLeft + ((const uint8_t*)(sourceBottomRowPixelData + lastSourcePixelRight))[c] * factorRight) * factorBottom + 8192u) >> 14u;
			}
		}
	}
}

/// \endcond

#endif // OCEAN_WE_KEEP_ALSO_THIS_SLOW_IMPLEMENTATION_SHOWING_A_MORE_GENERIC_APPROACH

template <>
inline void FrameInterpolatorBilinear::interpolateRowVerticalNEON<float>(const float* sourceRowTop, const float* sourceRowBottom, float* targetRow, const unsigned int elements, const float factorBottom)
{
	ocean_assert(sourceRowTop != nullptr);
	ocean_assert(sourceRowBottom != nullptr);
	ocean_assert(targetRow != nullptr);
	ocean_assert(elements >= 16u);
	ocean_assert(factorBottom >= 0.0f && factorBottom <= 1.0f);

	// [1.0f, 1.0f, 1.0f, 1.0f]
	const float32x4_t constant_1_f_32x4 = vdupq_n_f32(1.0f);

	const float32x4_t factorsBottom_f_32x4 = vdupq_n_f32(factorBottom);
	const float32x4_t factorsTop_f_32x4 = vsubq_f32(constant_1_f_32x4, factorsBottom_f_32x4); // factorTop = 1 - factorBottom

	for (unsigned int n = 0u; n < elements; n += 16u)
	{
		if (n + 16u > elements)
		{
			// the last iteration will not fit into the output frame,
			// so we simply shift x left by some elements (at most 15) and we will calculate some elements again

			ocean_assert(n >= 16u && elements > 16u);
			const unsigned int offset = n - (elements - 16u);
			ocean_assert(offset < 16u);

			sourceRowTop -= offset;
			sourceRowBottom -= offset;
			targetRow -= offset;

			// the for loop will stop after this iteration
			ocean_assert(!(n + 16u < elements));
		}

		// loading the next four 32 bit values from the top and bottom row
		const float32x4_t top_03_32x4 = vld1q_f32(sourceRowTop + 0);
		const float32x4_t top_47_32x4 = vld1q_f32(sourceRowTop + 4);
		const float32x4_t top_8B_32x4 = vld1q_f32(sourceRowTop + 8);
		const float32x4_t top_CF_32x4 = vld1q_f32(sourceRowTop + 12);

		const float32x4_t bottom_03_32x4 = vld1q_f32(sourceRowBottom + 0);
		const float32x4_t bottom_47_32x4 = vld1q_f32(sourceRowBottom + 4);
		const float32x4_t bottom_8B_32x4 = vld1q_f32(sourceRowBottom + 8);
		const float32x4_t bottom_CF_32x4 = vld1q_f32(sourceRowBottom + 12);

		// interpolatedRow_32x4 = top_32x4 * factorsTop + bottom_32x4 * factorsBottom
		float32x4_t interpolatedRow_03_32x4 = vmulq_f32(top_03_32x4, factorsTop_f_32x4);
		float32x4_t interpolatedRow_47_32x4 = vmulq_f32(top_47_32x4, factorsTop_f_32x4);
		float32x4_t interpolatedRow_8B_32x4 = vmulq_f32(top_8B_32x4, factorsTop_f_32x4);
		float32x4_t interpolatedRow_CF_32x4 = vmulq_f32(top_CF_32x4, factorsTop_f_32x4);

		interpolatedRow_03_32x4 = vmlaq_f32(interpolatedRow_03_32x4, bottom_03_32x4, factorsBottom_f_32x4);
		interpolatedRow_47_32x4 = vmlaq_f32(interpolatedRow_47_32x4, bottom_47_32x4, factorsBottom_f_32x4);
		interpolatedRow_8B_32x4 = vmlaq_f32(interpolatedRow_8B_32x4, bottom_8B_32x4, factorsBottom_f_32x4);
		interpolatedRow_CF_32x4 = vmlaq_f32(interpolatedRow_CF_32x4, bottom_CF_32x4, factorsBottom_f_32x4);

		// writing back the four interpolated 32 bit results
		vst1q_f32(targetRow + 0, interpolatedRow_03_32x4);
		vst1q_f32(targetRow + 4, interpolatedRow_47_32x4);
		vst1q_f32(targetRow + 8, interpolatedRow_8B_32x4);
		vst1q_f32(targetRow + 12, interpolatedRow_CF_32x4);

		sourceRowTop += 16;
		sourceRowBottom += 16;
		targetRow += 16;
	}
}

template <>
inline void FrameInterpolatorBilinear::interpolateRowHorizontalNEON<float, 1u>(const float* extendedSourceRow, float* targetRow, const unsigned int targetWidth, const unsigned int channels, const unsigned int* interpolationLocations, const float* interpolationFactorsRight)
{
	ocean_assert(extendedSourceRow != nullptr);
	ocean_assert(targetRow != nullptr);
	ocean_assert(targetWidth >= 8u);
	ocean_assert(interpolationLocations != nullptr);
	ocean_assert(interpolationFactorsRight != nullptr);

	ocean_assert(channels == 1u);

	// [1.0f, 1.0f, 1.0f, 1.0f]
	const float32x4_t constant_1_f_32x4 = vdupq_n_f32(1.0f);

	for (unsigned int x = 0; x < targetWidth; x += 8u)
	{
		if (x + 8u > targetWidth)
		{
			// the last iteration will not fit into the output frame,
			// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

			ocean_assert(x >= 8u && targetWidth > 8u);
			const unsigned int newX = targetWidth - 8u;

			ocean_assert(x > newX);
			const unsigned int offset = x - newX;

			targetRow -= offset;
			interpolationLocations -= offset;
			interpolationFactorsRight -= offset;

			x = newX;

			// the for loop will stop after this iteration
			ocean_assert(!(x + 8u < targetWidth));
		}

		// we load the left and the right pixels (for four resulting target pixels)

		const float32x2_t pixel_0_f_32x2 = vld1_f32(extendedSourceRow + interpolationLocations[0]);
		const float32x2_t pixel_1_f_32x2 = vld1_f32(extendedSourceRow + interpolationLocations[1]);
		const float32x4_t pixel_01_f_32x4 = vcombine_f32(pixel_0_f_32x2, pixel_1_f_32x2);

		const float32x2_t pixel_2_f_32x2 = vld1_f32(extendedSourceRow + interpolationLocations[2]);
		const float32x2_t pixel_3_f_32x2 = vld1_f32(extendedSourceRow + interpolationLocations[3]);
		const float32x4_t pixel_23_f_32x4 = vcombine_f32(pixel_2_f_32x2, pixel_3_f_32x2);

		const float32x2_t pixel_4_f_32x2 = vld1_f32(extendedSourceRow + interpolationLocations[4]);
		const float32x2_t pixel_5_f_32x2 = vld1_f32(extendedSourceRow + interpolationLocations[5]);
		const float32x4_t pixel_45_f_32x4 = vcombine_f32(pixel_4_f_32x2, pixel_5_f_32x2);

		const float32x2_t pixel_6_f_32x2 = vld1_f32(extendedSourceRow + interpolationLocations[6]);
		const float32x2_t pixel_7_f_32x2 = vld1_f32(extendedSourceRow + interpolationLocations[7]);
		const float32x4_t pixel_67_f_32x4 = vcombine_f32(pixel_6_f_32x2, pixel_7_f_32x2);

		const float32x4_t factorsRight_0123_f_32x4 = vld1q_f32(interpolationFactorsRight + 0);
		const float32x4_t factorsLeft_0123_f_32x4 = vsubq_f32(constant_1_f_32x4, factorsRight_0123_f_32x4);
		const float32x4x2_t factorsLeftRight_0123_f_32x4_2 = vzipq_f32(factorsLeft_0123_f_32x4, factorsRight_0123_f_32x4);

		const float32x4_t factorsRight_4567_f_32x4 = vld1q_f32(interpolationFactorsRight + 4);
		const float32x4_t factorsLeft_4567_f_32x4 = vsubq_f32(constant_1_f_32x4, factorsRight_4567_f_32x4);
		const float32x4x2_t factorsLeftRight_4567_f_32x4_2 = vzipq_f32(factorsLeft_4567_f_32x4, factorsRight_4567_f_32x4);

		const float32x4_t multiplied_01_f_32x4 = vmulq_f32(pixel_01_f_32x4, factorsLeftRight_0123_f_32x4_2.val[0]);
		const float32x4_t multiplied_23_f_32x4 = vmulq_f32(pixel_23_f_32x4, factorsLeftRight_0123_f_32x4_2.val[1]);

		const float32x4_t multiplied_45_f_32x4 = vmulq_f32(pixel_45_f_32x4, factorsLeftRight_4567_f_32x4_2.val[0]);
		const float32x4_t multiplied_67_f_32x4 = vmulq_f32(pixel_67_f_32x4, factorsLeftRight_4567_f_32x4_2.val[1]);

		const float32x2_t result_01_f_32x2 = vpadd_f32(vget_low_f32(multiplied_01_f_32x4), vget_high_f32(multiplied_01_f_32x4));
		const float32x2_t result_23_f_32x2 = vpadd_f32(vget_low_f32(multiplied_23_f_32x4), vget_high_f32(multiplied_23_f_32x4));

		const float32x2_t result_45_f_32x2 = vpadd_f32(vget_low_f32(multiplied_45_f_32x4), vget_high_f32(multiplied_45_f_32x4));
		const float32x2_t result_67_f_32x2 = vpadd_f32(vget_low_f32(multiplied_67_f_32x4), vget_high_f32(multiplied_67_f_32x4));

		const float32x4_t result_0123_f_32x4 = vcombine_f32(result_01_f_32x2, result_23_f_32x2);
		const float32x4_t result_4567_f_32x4 = vcombine_f32(result_45_f_32x2, result_67_f_32x2);

		vst1q_f32(targetRow + 0, result_0123_f_32x4);
		vst1q_f32(targetRow + 4, result_4567_f_32x4);

		targetRow += 8;
		interpolationLocations += 8;
		interpolationFactorsRight += 8;
	}
}

template <>
inline void FrameInterpolatorBilinear::scaleSubset<float, float, 1u>(const float* source, float* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const double sourceX_s_targetX, const double sourceY_s_targetY, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstTargetRow, const unsigned int numberTargetRows)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(sourceWidth >= 1u && sourceHeight >= 1u);
	ocean_assert(targetWidth >= 1u && targetHeight >= 1u);
	ocean_assert(sourceX_s_targetX > 0.0 && sourceY_s_targetY > 0.0);

	ocean_assert(sourceWidth != targetWidth || sourceHeight != targetHeight);

	const unsigned int sourceStrideElements = sourceWidth * 1u + sourcePaddingElements;
	const unsigned int targetStrideElements = targetWidth * 1u + targetPaddingElements;

	typedef void (*InterpolateRowVerticalFunction)(const float*, const float*, float*, const unsigned int, const float);
	typedef void (*InterpolateRowHorizontalFunction)(const float*, float*, const unsigned int, const unsigned int, const unsigned int*, const float*);

	InterpolateRowVerticalFunction interpolateRowVerticalFunction = interpolateRowVertical<float>;
	InterpolateRowHorizontalFunction interpolateRowHorizontalFunction = interpolateRowHorizontal<float, 1u>;

	if (sourceWidth * 1u >= 16u)
	{
		interpolateRowVerticalFunction = interpolateRowVerticalNEON<float>;
	}

	if (targetWidth >= 8u)
	{
		interpolateRowHorizontalFunction = interpolateRowHorizontalNEON<float, 1u>;
	}

	target += targetStrideElements * firstTargetRow;

	const float sourceX_T_targetX = float(sourceX_s_targetX);
	const float sourceY_T_targetY = float(sourceY_s_targetY);

	// See the generic template function for a detailed documentation regarding interpolation factors.

	Memory memoryIntermediateExtendedRow;
	Memory memoryHorizontalInterpolationLocations;
	Memory memoryHorizontalInterpolationFactorsRight;

	if (sourceWidth != targetWidth)
	{
		// in case we are scaling the width of the frame, we use an intermediate buffer and pre-calculated interpolation locations and factors

		memoryIntermediateExtendedRow = Memory::create<float>(sourceWidth + 1u); // one additional pixel

		memoryHorizontalInterpolationLocations = Memory::create<unsigned int>(targetWidth); // one offset for each target pixel

		memoryHorizontalInterpolationFactorsRight = Memory::create<float>(targetWidth); // one factors (right) for each target pixel
	}

	if (memoryHorizontalInterpolationLocations)
	{
		ocean_assert(memoryHorizontalInterpolationFactorsRight);

		if (targetWidth >= 4u)
		{
			const float32x4_t sourceX_T_targetX_f_32x4 = vdupq_n_f32(sourceX_T_targetX);
			const float32x4_t targetOffsetX_f_32x4 = vdupq_n_f32(sourceX_T_targetX * 0.5f - 0.5f);

			// [0.0f, 0.0f, 0.0f, 0.0f]
			const float32x4_t constant_0_f_32x4 = vdupq_n_f32(0);

			// [4.0f, 4.0f, 4.0f, 4.0f]
			const float32x4_t constant_4_f_32x4 = vdupq_n_f32(4.0f);

			// we store 4 integers: [sourceWidth - 1, sourceWidth - 1, sourceWidth - 1, sourceWidth - 1]
			const uint32x4_t sourceWidth_1_u_32x4 = vdupq_n_u32(sourceWidth - 1u);

			// [0.0f, 1.0f, 2.0f, 3.0f]
			const float f_0123[4] = {0.0f, 1.0f, 2.0f, 3.0f};
			float32x4_t x_0123_f_32x4 = vld1q_f32(f_0123);

			// we pre-calculate the interpolation factors and pixel locations in horizontal direction

			for (unsigned int x = 0u; x < targetWidth; x += 4u)
			{
				if (x + 4u > targetWidth)
				{
					// the last iteration will not fit into the output frame,
					// so we simply shift x left by some pixels (at most 3) and we will calculate some pixels again

					ocean_assert(x >= 4u && targetWidth > 4u);
					const unsigned int newX = targetWidth - 4u;

					ocean_assert(x > newX);
					const unsigned int offset = x - newX;

					x = newX;

					x_0123_f_32x4 = vsubq_f32(x_0123_f_32x4, vdupq_n_f32(float(offset)));

					// the for loop will stop after this iteration
					ocean_assert(!(x + 4u < targetWidth));
				}

				// we calculate the four source locations for our four target locations
				const float32x4_t sourceX_0123_f_32x4 = vmaxq_f32(constant_0_f_32x4, vaddq_f32(targetOffsetX_f_32x4, vmulq_f32(sourceX_T_targetX_f_32x4, x_0123_f_32x4)));

				// now we determine the pixel/integer accurate source locations
				// left = min(floor(sourceX), sourceWidth - 1)
				uint32x4_t left_0123_u_32x4 = vminq_u32(vcvtq_u32_f32(sourceX_0123_f_32x4), sourceWidth_1_u_32x4); // no rounding here

				// we store the offsets we have calculated
				vst1q_u32(memoryHorizontalInterpolationLocations.data<unsigned int>() + x, left_0123_u_32x4);

				// factorRight = sourcceX - float(left)
				const float32x4_t factorsRight_f_32x4 = vsubq_f32(sourceX_0123_f_32x4, vcvtq_f32_u32(left_0123_u_32x4));

				vst1q_f32(memoryHorizontalInterpolationFactorsRight.data<float>() + x, factorsRight_f_32x4);

				// [x + 0, x + 1, x + 2, x + 3] + [4, 4, 4, 4]
				x_0123_f_32x4 = vaddq_f32(x_0123_f_32x4, constant_4_f_32x4);
			}
		}
		else
		{
			const float targetOffsetX = sourceX_T_targetX * 0.5f - 0.5f;

			// we pre-calculate the interpolation factors and pixel locations in horizontal direction

			for (unsigned int x = 0u; x < targetWidth; ++x)
			{
				const float sourceX = max(0.0f, targetOffsetX + float(x) * sourceX_T_targetX);

				const unsigned int left = min((unsigned int)sourceX, sourceWidth - 1u); // no rounding here

				memoryHorizontalInterpolationLocations.data<unsigned int>()[x] = left;

				const float factorRight = sourceX - float(left);
				ocean_assert(factorRight >= 0.0f && factorRight <= 1.0f);

				memoryHorizontalInterpolationFactorsRight.data<float>()[x] = factorRight;
			}
		}
	}

	const float targetOffsetY = sourceY_T_targetY * 0.5f - 0.5f;

	for (unsigned int y = firstTargetRow; y < firstTargetRow + numberTargetRows; ++y)
	{
		const float sourceY = minmax<float>(0.0f, targetOffsetY + sourceY_T_targetY * float(y), float(sourceHeight) - 1.0f);

		const unsigned int sourceRowTop = (unsigned int)sourceY; // we must not round here
		const float factorBottom = sourceY - float(sourceRowTop);
		ocean_assert(factorBottom >= 0.0f && factorBottom <= 1.0f);

		const unsigned int sourceRowBottom = min(sourceRowTop + 1u, sourceHeight - 1u);

		const float* const sourceTopRow = source + sourceStrideElements * sourceRowTop;
		const float* const sourceBottomRow = source + sourceStrideElements * sourceRowBottom;

		float* targetRow = nullptr;

		if (sourceHeight == targetHeight)
		{
			ocean_assert(sourceWidth != targetWidth);
			ocean_assert(memoryIntermediateExtendedRow);

			// we do not need to interpolate two lines, thus we simply need to copy the row (as we need an additional pixel at the end)
			memcpy(memoryIntermediateExtendedRow.data<float>(), sourceTopRow, sourceWidth * sizeof(float));
		}
		else
		{
			// in case we do not scale the width of the frame, we can write the result to the target frame directly
			targetRow = memoryIntermediateExtendedRow.isNull() ? target : memoryIntermediateExtendedRow.data<float>();

			ocean_assert(targetRow != nullptr);
			ocean_assert(interpolateRowVerticalFunction != nullptr);
			interpolateRowVerticalFunction(sourceTopRow, sourceBottomRow, targetRow, sourceWidth * 1u, factorBottom);
		}

		if (memoryIntermediateExtendedRow) // sourceWidth != targetWidth
		{
			// we use an extended row (with one additional pixel at the end - equal to the last pixel)
			// so we have to copy the last pixel
			memoryIntermediateExtendedRow.data<float>()[sourceWidth] = memoryIntermediateExtendedRow.data<float>()[sourceWidth - 1u];

			interpolateRowHorizontalFunction(memoryIntermediateExtendedRow.data<float>(), target, targetWidth, 1u, memoryHorizontalInterpolationLocations.data<unsigned int>(), memoryHorizontalInterpolationFactorsRight.data<float>());
		}

		target += targetStrideElements;
	}
}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

template <typename T, typename TScale, unsigned int tChannels>
void FrameInterpolatorBilinear::scaleSubset(const T* source, T* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const double sourceX_s_targetX, const double sourceY_s_targetY, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstTargetRow, const unsigned int numberTargetRows)
{
	static_assert((std::is_same<float, TScale>::value || std::is_same<double, TScale>::value), "Invalid TScale type");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(sourceWidth != 0u && sourceHeight != 0u);
	ocean_assert_and_suppress_unused(targetWidth >= 1u && targetHeight >= 1u, targetHeight);
	ocean_assert(sourceX_s_targetX > 0.0 && sourceY_s_targetY > 0.0);

	const unsigned int sourceStrideElements = sourceWidth * tChannels + sourcePaddingElements;
	const unsigned int targetStrideElements = targetWidth * tChannels + targetPaddingElements;

	const TScale sourceX_T_targetX = TScale(sourceX_s_targetX);
	const TScale sourceY_T_targetY = TScale(sourceY_s_targetY);

	/*
	 * We determine the sub-pixel accurate source location for each target pixel as follows:
	 *
	 * Example with a downsampling by factor 4:
	 * sourceRow with 12 pixels: | 0 1 2 3 4 5 6 7 8 9 A B |
	 * targetRow with  3 pixels: | 0 1 2 |
	 *
	 * Thus, the source row can be separated into three blocks;
	 * and we want to extract the color information from the center of the blocks:
	 * sourceRow with 12 pixels: | 0 1 2 3 | 4 5 6 7 | 8 9 A B |
	 * targetRow with  3 pixels: |    0    |    1    |    2    | (targetTSourceX = 4)
	 *
	 * Thus, we add 0.5 to each target coordinate before converting it to a source location;
	 * and subtract 0.5 again afterwards:
	 * sourceX = (targetX + 0.5) * targetTSourceX - 0.5
	 *
	 * e.g., (0 + 0.5) * 4 - 0.5 = 1.5
	 *       (1 + 0.5) * 4 - 0.5 = 5.5
	 *
	 *
	 * Example with a downsampling by factor 3:
	 * sourceRow with 9 pixels: | 0 1 2 3 4 5 6 7 8 |
	 * targetRow with 3 pixels: | 0 1 2 |
	 *
	 * sourceRow with 9 pixels: | 0 1 2 | 3 4 5 | 6 7 8 |
	 * targetRow with 3 pixels: |   0   |   1   |   2   | (targetTSourceX = 3)
	 *
	 * e.g., (0 + 0.5) * 3 - 0.5 = 1
	 *       (1 + 0.5) * 3 - 0.5 = 4
	 *
	 *
	 * Example with a downsampling by factor 2:
	 * sourceRow with 6 pixels: | 0 1 2 3 4 5 |
	 * targetRow with 3 pixels: | 0 1 2 |
	 *
	 * sourceRow with 6 pixels: | 0 1 | 2 3 | 4 5 |
	 * targetRow with 3 pixels: |  0  |  1  |  2  | (targetTSourceX = 2)
	 *
	 * e.g., (0 + 0.5) * 2 - 0.5 = 0.5
	 *       (1 + 0.5) * 2 - 0.5 = 2.5
	 *
	 *
	 * we can simplify the calculation (as we have a constant term):
	 * sourceX = (targetX * targetTSourceX) + (0.5 * targetTSourceX - 0.5)
	 */

	const TScale sourceX_T_targetXOffset = sourceX_T_targetX * TScale(0.5) - TScale(0.5);
	const TScale sourceY_T_targetYOffset = sourceY_T_targetY * TScale(0.5) - TScale(0.5);

	const TScale sourceWidth_1 = TScale(sourceWidth - 1u);
	const TScale sourceHeight_1 = TScale(sourceHeight - 1u);

	target += targetStrideElements * firstTargetRow;

	for (unsigned int y = firstTargetRow; y < firstTargetRow + numberTargetRows; ++y)
	{
		const TScale sy = minmax(TScale(0), sourceY_T_targetYOffset + sourceY_T_targetY * TScale(y), sourceHeight_1);
		ocean_assert(sy >= TScale(0) && sy < TScale(sourceHeight));

		const unsigned int sTop = (unsigned int)sy;
		ocean_assert(sy >= TScale(sTop));

		const TScale factorBottom = sy - TScale(sTop);
		ocean_assert(factorBottom >= TScale(0) && factorBottom <= TScale(1));

		const TScale factorTop = TScale(1) - factorBottom;
		ocean_assert(factorTop >= TScale(0) && factorTop <= TScale(1));

		const T* const sourceTop = source + sTop * sourceStrideElements;
		const T* const sourceBottom = (sTop + 1u < sourceHeight) ? sourceTop + sourceStrideElements : sourceTop;

		for (unsigned int x = 0; x < targetWidth; ++x)
		{
			const TScale sx = minmax(TScale(0), sourceX_T_targetXOffset + sourceX_T_targetX * TScale(x), sourceWidth_1);
			ocean_assert(sx >= TScale(0) && sx < TScale(sourceWidth));

			const unsigned int sLeft = (unsigned int)sx;
			ocean_assert(sx >= TScale(sLeft));

			const TScale factorRight = sx - TScale(sLeft);
			ocean_assert(factorRight >= TScale(0) && factorRight <= TScale(1));

			const TScale factorLeft = TScale(1) - factorRight;
			ocean_assert(factorLeft >= TScale(0) && factorLeft <= TScale(1));

			const unsigned int sourceRightOffset = sLeft + 1u < sourceWidth ? tChannels : 0u;

			const T* const sourceTopLeft = sourceTop + sLeft * tChannels;
			const T* const sourceBottomLeft = sourceBottom + sLeft * tChannels;

			const TScale factorTopLeft = factorTop * factorLeft;
			const TScale factorTopRight = factorTop * factorRight;
			const TScale factorBottomLeft = factorBottom * factorLeft;
			const TScale factorBottomRight = factorBottom * factorRight;

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				target[n] = T(TScale(sourceTopLeft[n]) * factorTopLeft + TScale(sourceTopLeft[sourceRightOffset + n]) * factorTopRight
								+ TScale(sourceBottomLeft[n]) * factorBottomLeft + TScale(sourceBottomLeft[sourceRightOffset + n]) * factorBottomRight);
			}

			target += tChannels;
		}

		target += targetPaddingElements;
	}
}

template <unsigned int tChannels>
void FrameInterpolatorBilinear::rotate8BitPerChannelSubset(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const Scalar horizontalAnchorPosition, const Scalar verticalAnchorPosition, const Scalar angle, const uint8_t* borderColor, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstTargetRow, const unsigned int numberTargetRows)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(firstTargetRow + numberTargetRows <= height);

	typedef typename DataType<uint8_t, tChannels>::Type PixelType;

	const unsigned int targetStrideElements = width * tChannels + targetPaddingElements;

	uint8_t zeroColor[tChannels] = {uint8_t(0)};
	const PixelType bColor = borderColor ? *(const PixelType*)borderColor : *(const PixelType*)zeroColor;

	const SquareMatrix3 rotationMatrix3(Rotation(0, 0, 1, angle));
	const SquareMatrix2 rotationMatrix2(rotationMatrix3(0, 0), rotationMatrix3(1, 0), rotationMatrix3(0, 1), rotationMatrix3(1, 1));

	const Scalar width_1 = Scalar(width - 1u);
	const Scalar height_1 = Scalar(height - 1u);
	const Vector2 anchorPosition(horizontalAnchorPosition, verticalAnchorPosition);

	for (unsigned int y = firstTargetRow; y < firstTargetRow + numberTargetRows; ++y)
	{
		PixelType* targetPixel = (PixelType*)(target + y * targetStrideElements);

		const Scalar floatY = Scalar(y);

		for (unsigned int x = 0; x < width; ++x)
		{
			const Vector2 sourceLocation(anchorPosition + rotationMatrix2 * (Vector2(Scalar(x), floatY) - anchorPosition));

			if (sourceLocation.x() >= 0 && sourceLocation.y() >= 0 && sourceLocation.x() <= width_1 && sourceLocation.y() <= height_1)
			{
				interpolatePixel8BitPerChannel<tChannels, PC_TOP_LEFT>(source, width, height, sourcePaddingElements, sourceLocation, (uint8_t*)(targetPixel));
			}
			else
			{
				*targetPixel = bColor;
			}

			++targetPixel;
		}
	}
}

} // namespace CV

} // namespace Ocean

#endif // META_OCEAN_CV_FRAME_INTERPOLATOR_BILINEAR_H
