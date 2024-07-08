/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_INTERPOLATOR_NEAREST_PIXEL_H
#define META_OCEAN_CV_FRAME_INTERPOLATOR_NEAREST_PIXEL_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameTransposer.h"
#include "ocean/cv/PixelPosition.h"

#include "ocean/base/DataType.h"
#include "ocean/base/Frame.h"
#include "ocean/base/Memory.h"
#include "ocean/base/Worker.h"

#include "ocean/math/Lookup2.h"
#include "ocean/math/SquareMatrix3.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements a nearest pixel frame interpolator.
 * Actually, no pixels are interpolated, but the color intensities from the nearest pixels (e.g., based on rounding) is used.<br>
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameInterpolatorNearestPixel
{
	public:

		/// Definition of a lookup table for 2D vectors.
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
				 * Resizes a given frame by a nearest pixel search.
				 * The pixel format of the frame must be zipped with DT_UNSIGNED_INTEGER_8 as data type (e.g., FORMAT_Y8, FORMAT_RGB24, FORMAT_RGBA32, ...).
				 * @param source The source frame that will be resized, must have a zipped pixel format, must be valid
				 * @param target The target frame that receives the image information of the source frame, the pixel format and pixel origin must match with the source frame
				 * @param worker Optional worker object to distribute the computational load
				 * @return True, if succeeded
				 */
				static bool resize(const Frame& source, Frame& target, Worker* worker = nullptr);

				/**
				 * Resizes a given frame in place by a nearest pixel search.
				 * The pixel format of the frame must be zipped with DT_UNSIGNED_INTEGER_8 as data type (e.g., FORMAT_Y8, FORMAT_RGB24, FORMAT_RGBA32, ...).
				 * @param frame The frame that will be resized, must have a zipped pixel format, must be valid
				 * @param targetWidth Width of the new target frame in pixel, with range [1, infinity)
				 * @param targetHeight Height of the new target frame in pixel, with range [1, infinity)
				 * @param worker Optional worker object to distribute the computational load
				 * @return True, if succeeded
				 */
				static inline bool resize(Frame& frame, const unsigned int targetWidth, const unsigned int targetHeight, Worker* worker = nullptr);

				/**
				 * Applies an affine image transformation to a frame (with zipped pixel format) and renders using nearest-neighbor interpolation
				 * The output frame must have the same pixel format and pixel origin as the input frame, however the dimension (and position) of the output frame can be arbitrary.
				 * This function allows the creation of an output frame fully covering the input frame (if the position and dimension of the output frame covers the affine transformation
				 * The pixel format of the frame must be zipped with DT_UNSIGNED_INTEGER_8 as data type (e.g., FORMAT_Y8, FORMAT_RGB24, FORMAT_RGBA32, ...).
				 * @param input The input frame that will be transformed, must have a zipped pixel format, must be valid
				 * @param output The output frame resulting by application of the given affine transformation, with same pixel format and pixel origin as the input frame, must have a valid dimension
				 * @param input_A_output Affine transformation used to transform the given input frame, transforming points defined in the output frame into points defined in the input frame
				 * @param borderColor Color of undefined pixel positions, the size of the buffer must match to the number of channels, nullptr to assign 0x00 to each channel
				 * @param worker Optional worker object to distribute the computational load
				 * @param outputOrigin The origin of the output frame defining the global position of the output frame's pixel coordinate (0, 0), with range (-infinity, infinity)x(-infinity, infinity)
				 * @return True, if succeeded
				 */
				static bool affine(const Frame& input, Frame& output, const SquareMatrix3& input_A_output, const uint8_t* borderColor = nullptr, Worker* worker = nullptr, const PixelPositionI& outputOrigin = PixelPositionI(0, 0));

				/**
				 * Transforms a given input frame (with zipped pixel format) into an output frame by application of a homography.
				 * The output frame must have the same pixel format and pixel origin as the input frame, however the dimension (and position) of the output frame can be arbitrary.<br>
				 * This function allows the creation of an output frame fully covering the input frame (if the position and dimension of the output frame covers the transformation of the homography.<br>
				 * The pixel format of the frame must be zipped with DT_UNSIGNED_INTEGER_8 as data type (e.g., FORMAT_Y8, FORMAT_RGB24, FORMAT_RGBA32, ...).
				 * @param input The input frame that will be transformed, must have a zipped pixel format, must be valid
				 * @param output The output frame resulting by application of the given homography, with same pixel format and pixel origin as the input frame, must be valid
				 * @param input_H_output Homography used to transform the given input frame by following equation: inputPoint = input_H_output * outputPoint, must be valid
				 * @param borderColor Color of undefined pixel positions, the size of the buffer must match to the number of channels, nullptr to assign 0x00 to each channel
				 * @param worker Optional worker object to distribute the computational load
				 * @param outputOrigin The origin of the output frame defining the global position of the output frame's pixel coordinate (0, 0), with range (-infinity, infinity)x(-infinity, infinity)
				 * @return True, if succeeded
				 */
				static bool homography(const Frame& input, Frame& output, const SquareMatrix3& input_H_output, const void* borderColor = nullptr, Worker* worker = nullptr, const PixelPositionI& outputOrigin = PixelPositionI(0, 0));

				/**
				 * Transforms a given input frame (with zipped pixel format) into an output frame (with arbitrary frame dimension) by application of a homography.
				 * The output frame must have the same pixel format and pixel origin as the input frame, however the dimension (and position) of the output frame can be arbitrary.<br>
				 * This function allows the creation of an output frame fully covering the input frame (if the position and dimension of the output frame covers the transformation of the homography.<br>
				 * Input frame pixels lying outside the frame will be masked in the resulting output mask frame, further theses pixels are untouched in the output frame.<br>
				 * The pixel format of the frame must be zipped with DT_UNSIGNED_INTEGER_8 as data type (e.g., FORMAT_Y8, FORMAT_RGB24, FORMAT_RGBA32, ...).
				 * @param input The input frame that will be transformed, must have a zipped pixel format, must be valid
				 * @param output The output frame resulting by application of the given homography, with same pixel format and pixel origin as the input frame, must have a valid dimension
				 * @param outputMask Mask frame with 8 bit per pixel defining whether an output frame pixel has a valid corresponding pixel in the input frame
				 * @param input_H_output Homography used to transform the given input frame by following equation: inputPoint = input_H_output * outputPoint, must be valid
				 * @param worker Optional worker object to distribute the computational load
				 * @param maskValue 8 bit mask values for reference pixels lying inside the given camera frame, reference pixels lying outside the camera frame will be assigned with (0xFF - maskValue)
				 * @param outputOrigin The origin of the output frame defining the global position of the output frame's pixel coordinate (0, 0), with range (-infinity, infinity)x(-infinity, infinity)
				 * @return True, if succeeded
				 * @see Geometry::Homography::coversHomographyInputFrame().
				 */
				static bool homographyMask(const Frame& input, Frame& output, Frame& outputMask, const SquareMatrix3& input_H_output, Worker* worker = nullptr, const uint8_t maskValue = 0xFFu, const PixelPositionI& outputOrigin = PixelPositionI(0, 0));

				/**
				 * Transforms a given input frame (with 1 plane) into an output frame by application of an interpolation lookup table.
				 * The output frame must have the same pixel format and pixel origin as the input frame.<br>
				 * The pixel format of the frame must be zipped with DT_UNSIGNED_INTEGER_8 as data type (e.g., FORMAT_Y8, FORMAT_RGB24, FORMAT_RGBA32, ...).
				 * @param input The input frame which will be transformed, must have a zipped pixel format, must be valid
				 * @param output Resulting output frame, the frame dimension will be set to match the size of the lookup table, pixel format and pixel origin will be set to match the given input frame
				 * @param lookupTable The lookup table which defines the transformation, must be valid
				 * @param offset True, if the lookup table store local offsets; False, if the lookup table stores absolute positions
				 * @param borderColor Color of undefined pixel positions, the size of the buffer must match to the number of channels, nullptr to assign 0x00 to each channel
				 * @param worker Optional worker object to distribute the computation
				 * @return True, if succeeded
				 */
				static bool transform(const Frame& input, Frame& output, const LookupTable& lookupTable, const bool offset, const uint8_t* borderColor, Worker* worker = nullptr);

				/**
				 * Transforms a given input frame (with zipped pixel format) into an output frame by application of an interpolation lookup table.
				 * The output frame must have the same pixel format and pixel origin as the input frame.<br>
				 * Input frame pixels lying outside the frame will be masked in the resulting output mask frame, further theses pixels are untouched in the output frame.<br>
				 * The pixel format of the frame must be zipped with DT_UNSIGNED_INTEGER_8 as data type (e.g., FORMAT_Y8, FORMAT_RGB24, FORMAT_RGBA32, ...).
				 * @param input The input frame which will be transformed
				 * @param output Resulting output frame, the frame dimension will be set to match the size of the lookup table, pixel format and pixel origin will be set to match the given input frame
				 * @param outputMask Resulting mask frame with 8 bit per pixel defining whether an output frame pixel has a valid corresponding pixel in the input frame
				 * @param lookupTable The lookup table which defines the transformation, must be valid
				 * @param offset True, if the lookup table store local offsets; False, if the lookup table stores absolute positions
				 * @param worker Optional worker object to distribute the computation
				 * @param maskValue 8 bit mask values for pixels lying inside the input frame, pixels lying outside the input frame will be assigned with (0xFF - maskValue)
				 * @return True, if succeeded
				 */
				static bool transformMask(const Frame& input, Frame& output, Frame& outputMask, const LookupTable& lookupTable, const bool offset, Worker* worker = nullptr, const uint8_t maskValue = 0xFFu);

				/**
				 * Rotates a given frame either clockwise or counter-clockwise by 90 degrees.
				 * This function is just a wrapper for CV::FrameTransposer::Comfort::rotate90().
				 * @param input The input frame which will be rotated, must be valid
				 * @param output The resulting rotated output frame, the frame type will be set automatically
				 * @param clockwise True, to rotate the frame clockwise; False, to rotate the frame counter-clockwise
				 * @param worker Optional worker object to distribute the computation
				 * @return True, if succeeded
				 */
				static inline bool rotate90(const Frame& input, Frame& output, const bool clockwise, Worker* worker = nullptr);

				/**
				 * Rotates a given frame by 180 degrees.
				 * This function is just a wrapper for CV::FrameTransposer::Comfort::rotate180().
				 * @param input The input frame which will be rotated, must be valid
				 * @param output The resulting rotated output frame, the frame type will be set automatically
				 * @param worker Optional worker object to distribute the computation
				 * @return True, if succeeded
				 */
				static inline bool rotate180(const Frame& input, Frame& output, Worker* worker = nullptr);

				/**
				 * Rotates a given frame with 90 degree steps.
				 * This function is just a wrapper for CV::FrameTransposer::Comfort::rotate().
				 * @param input The input frame which will be rotated, must be valid
				 * @param output The resulting rotated output frame, the frame type will be set automatically
				 * @param angle The clockwise rotation angle to be used, must be a multiple of +/- 90, with range (-infinity, infinity)
				 * @param worker Optional worker object to distribute the computation
				 * @return True, if succeeded
				 */
				static bool rotate(const Frame& input, Frame& output, const int angle, Worker* worker = nullptr);
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
				 * @param sourcePaddingElements Optional padding at the end of each source row in elements, with range [0, infinity)
				 * @param targetPaddingElements Optional padding at the end of each target row in elements, with range [0, infinity)
				 * @see FrameInterpolatorBilinear::resize<T, tChannels>().
				 */
				static void resize400x400To224x224_8BitPerChannel(const uint8_t* const source, uint8_t* const target, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements);
		};

		/**
		 * Resizes a given frame by a nearest pixel search and uses several CPU cores to speed update the process.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param sourceWidth Width of the source frame in pixel, with range [1, infinity)
		 * @param sourceHeight Height of the source frame in pixel, with range [1, infinity)
		 * @param targetWidth Width of the target frame in pixel, with range [1, infinity)
		 * @param targetHeight Height of the target frame in pixel, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker to distribute the computation
		 * @tparam T Data type the pixel channel values
		 * @tparam tChannels Number of data channels, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static inline void resize(const T* source, T* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Applies an affine image transformation to an 8 bit per channel input frame and renders the output.
		 * The output frame must have the same pixel format and pixel origin as the input frame, however the dimension (and position) of the output frame can be arbitrary.
		 * This function allows the creation of an output frame fully covering the input frame (if the position and dimension of the output frame covers the affine transformation.
		 * @param input The input frame that will be transformed, must be valid
		 * @param inputWidth Width of both input images in pixel, with range [1, infinity)
		 * @param inputHeight Height of both input images pixel, with range [1, infinity)
		 * @param input_A_output The affine transformation used to transform the given input frame, transforming output points to input points, must be valid
		 * @param borderColor Color of undefined pixel positions, the size of the buffer must match to the number of channels, nullptr to assign 0x00 to each channel
		 * @param output The output frame using the given affine transform, must be valid
		 * @param outputOrigin The origin of the output frame defining the global position of the output frame's pixel coordinate (0, 0), with range (-infinity, infinity)x(-infinity, infinity)
		 * @param outputWidth The width of the output image in pixel, with range [1, infinity)
		 * @param outputHeight The height of the output image in pixel, with range [1, infinity)
		 * @param inputPaddingElements The number of padding elements at the end of each input frame row, in elements, with range [0, infinity)
		 * @param outputPaddingElements The number of padding elements at the end of each output frame row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 * @tparam tChannels The number of channels of the frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static inline void affine8BitPerChannel(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const SquareMatrix3& input_A_output, const uint8_t* borderColor, uint8_t* output, const PixelPositionI& outputOrigin, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, Worker* worker = nullptr);

		/**
		 * Transforms a given input frame into an output frame by application of a homography.
		 * The output frame must have the same pixel format and pixel origin as the input frame, however the dimension (and position) of the output frame can be arbitrary.<br>
		 * This function allows the creation of an output frame fully covering the input frame (if the position and dimension of the output frame covers the transformation of the homography.<br>
		 * @param input The input frame that will be transformed, must be valid
		 * @param inputWidth Width of both input images in pixel, with range [1, infinity)
		 * @param inputHeight Height of both input images pixel, with range [1, infinity)
		 * @param input_H_output The homography used to transform the given input frame by following equation: inputPoint = input_H_output * outputPoint, must be valid
		 * @param borderColor Color of undefined pixel positions, the size of the buffer must match to the number of channels, nullptr to assign 0 to each channel
		 * @param output The output frame using the given homography, must be valid
		 * @param outputOrigin The origin of the output frame defining the global position of the output frame's pixel coordinate (0, 0), with range (-infinity, infinity)x(-infinity, infinity)
		 * @param outputWidth The width of the output image in pixel, with range [1, infinity)
		 * @param outputHeight The height of the output image in pixel, with range [1, infinity)
		 * @param inputPaddingElements The number of padding elements at the end of each input row, in elements, with range [0, infinity)
		 * @param outputPaddingElements The number of padding elements at the end of each output row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 * @tparam T Data type of each pixel channel, e.g., float, double, int
		 * @tparam tChannels Number of channels of the frame, with range [1, infinity)
		 * @see homographyMask8BitPerChannel().
		 */
		template <typename T, unsigned int tChannels>
		static inline void homography(const T* input, const unsigned int inputWidth, const unsigned int inputHeight, const SquareMatrix3& input_H_output, const T* borderColor, T* output, const PixelPositionI& outputOrigin, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, Worker* worker = nullptr);

		/**
		 * Transforms a given 8 bit per channel input frame into an output frame by application of a homography.
		 * The output frame must have the same pixel format and pixel origin as the input frame, however the dimension (and position) of the output frame can be arbitrary.<br>
		 * This function allows the creation of an output frame fully covering the input frame (if the position and dimension of the output frame covers the transformation of the homography.<br>
		 * @param input The input frame that will be transformed, must be valid
		 * @param inputWidth Width of both input images in pixel, with range [1, infinity)
		 * @param inputHeight Height of both input images pixel, with range [1, infinity)
		 * @param inputPaddingElements The number of padding elements at the end of each input frame row, in elements, with range [0, infinity)
		 * @param input_H_output Homography used to transform the given input frame by following equation: inputPoint = input_H_output * outputPoint, must be valid
		 * @param output The output frame using the given homography, must be valid
		 * @param outputPaddingElements The number of padding elements at the end of each output frame row, in elements, with range [0, infinity)
		 * @param outputMask Mask frame with 8 bit per pixel defining whether an output frame pixel has a valid corresponding pixel in the input frame
		 * @param outputMaskPaddingElements The number of padding elements at the end of each output mask row, in elements, with range [0, infinity)
		 * @param outputOrigin The origin of the output frame defining the global position of the output frame's pixel coordinate (0, 0), with range (-infinity, infinity)x(-infinity, infinity)
		 * @param outputWidth The width of the output image in pixel, with range [1, infinity)
		 * @param outputHeight The height of the output image in pixel, with range [1, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 * @param maskValue 8 bit mask values for reference pixels lying inside the given camera frame, reference pixels lying outside the camera frame will be assigned with (0xFF - maskValue)
		 * @tparam tChannels Number of channels of the frame, with range [1, infinity)
		 * @see homographyMask8BitPerChannel().
		 */
		template <unsigned int tChannels>
		static inline void homographyMask8BitPerChannel(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const unsigned int inputPaddingElements, const SquareMatrix3& input_H_output, uint8_t* output, const unsigned int outputPaddingElements, uint8_t* outputMask, const unsigned int outputMaskPaddingElements, const PixelPositionI& outputOrigin, const unsigned int outputWidth, const unsigned int outputHeight, Worker* worker = nullptr, const uint8_t maskValue = 0xFF);

		/**
		 * Transforms a given input frame (with zipped pixel format) into an output frame by application of an interpolation lookup table.
		 * The output frame must have the same pixel format and pixel origin as the input frame.<br>
		 * @param input The input frame which will be transformed, must be valid
		 * @param inputWidth The width of the given input frame in pixel, with range [1, infinity)
		 * @param inputHeight The height of the given input frame in pixel, with range [1, infinity)
		 * @param lookupTable The lookup table which defines the transformation, must be valid
		 * @param offset True, if the lookup table store local offsets; False, if the lookup table stores absolute positions
		 * @param borderColor Color of undefined pixel positions, the size of the buffer must match to the number of channels, nullptr to assign 0x00 to each channel
		 * @param output Resulting output frame with frame dimension equal to the size of the given lookup table, must be valid
		 * @param inputPaddingElements The number of padding elements at the end of each input row, in elements, with range [0, infinity)
		 * @param outputPaddingElements The number of padding elements at the end of each output row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannels Number of channels of the frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static inline void transform8BitPerChannel(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const LookupTable& lookupTable, const bool offset, const uint8_t* borderColor, uint8_t* output, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, Worker* worker = nullptr);

		/**
		 * Transforms a given input frame (with zipped pixel format) into an output frame by application of an interpolation lookup table.
		 * The output frame must have the same pixel format and pixel origin as the input frame.<br>
		 * Input frame pixels lying outside the frame will be masked in the resulting output mask frame, further theses pixels are untouched in the output frame.<br>
		 * @param input The input frame which will be transformed, must be valid
		 * @param inputWidth The width of the given input frame in pixel, with range [1, infinity)
		 * @param inputHeight The height of the given input frame in pixel, with range [1, infinity)
		 * @param inputPaddingElements The number of padding elements at the end of each input frame row, in elements, with range [0, infinity)
		 * @param lookupTable The lookup table which defines the transformation, must be valid
		 * @param offset True, if the lookup table store local offsets; False, if the lookup table stores absolute positions
		 * @param output Resulting output frame with frame dimension equal to the size of the given lookup table, must be valid
		 * @param outputPaddingElements The number of padding elements at the end of each output frame row, in elements, with range [0, infinity)
		 * @param outputMask Resulting mask frame with 8 bit per pixel defining whether an output frame pixel has a valid corresponding pixel in the input frame
		 * @param outputMaskPaddingElements The number of padding elements at the end of each output mask row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @param maskValue 8 bit mask values for pixels lying inside the input frame, pixels lying outside the input frame will be assigned with (0xFF - maskValue)
		 * @tparam tChannels Number of channels of the frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static inline void transformMask8BitPerChannel(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const unsigned int inputPaddingElements, const LookupTable& lookupTable, const bool offset, uint8_t* output, const unsigned int outputPaddingElements, uint8_t* outputMask, const unsigned int outputMaskPaddingElements, Worker* worker = nullptr, const uint8_t maskValue = 0xFF);

		/**
		 * Rotates a given frame either clockwise or counter-clockwise by 90 degree.
		 * @param source The source frame which will be rotated, must be valid
		 * @param target The resulting rotated target frame, must be valid and must have the same buffer size as the source frame
		 * @param sourceWidth The width of the source frame in pixel, with range [1, infinity)
		 * @param sourceHeight The height of the source frame in pixel, with range [1, infinity)
		 * @param clockwise True, to rotate the frame clockwise; False, to rotate the frame counter-clockwise
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam TElementType Data type of the elements of the image pixels
		 * @tparam tChannels Number of data channels, with range [1, infinity)
		 */
		template <typename TElementType, unsigned int tChannels>
		static inline void rotate90(const TElementType* source, TElementType* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const bool clockwise, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Checks whether the application of a given homography for a specified input frame and output frame covers the entire image information (which is necessary for the nearest pixel 'interpolation') or whether the homography relies on missing image information.
		 * @param inputWidth The width of the input frame in pixel, with range [1, infinity)
		 * @param inputHeight The height of the input frame in pixel, with range [1, infinity)
		 * @param outputWidth The width of the output frame in pixel, with range [1, infinity)
		 * @param outputHeight The height of the output frame in pixel, with range [1, infinity)
		 * @param input_H_output The homography to check which transforms points by following equation: inputPoint = input_H_output * outputPoint, must be valid
		 * @param outputOriginX The horizontal origin of the output frame defining the global position of the output frame's pixel coordinate (0, 0), with range (-infinity, infinity)
		 * @param outputOriginY The vertical origin of the output frame defining the global position of the output frame's pixel coordinate (0, 0), with range (-infinity, infinity)
		 * @return True, if the homography covers the entire input image information (if all output pixels will receive valid data from the input frame); False, otherwise
		 */
		static bool coversHomographyInputFrame(const unsigned int inputWidth, const unsigned int inputHeight, const unsigned int outputWidth, const unsigned int outputHeight, const SquareMatrix3& input_H_output, const int outputOriginX = 0, const int outputOriginY = 0);

	private:

		/**
		 * Resizes a given frame by a nearest pixel search.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param sourceWidth Width of the source frame in pixel, with range [1, infinity)
		 * @param sourceHeight Height of the source frame in pixel, with range [1, infinity)
		 * @param targetWidth Width of the target frame in pixel, with range [1, infinity)
		 * @param targetHeight Height of the target frame in pixel, with range [1, infinity)
		 * @param sourcePaddingElements Optional padding at the end of each source row in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional padding at the end of each target row in elements, with range [0, infinity)
		 * @param firstTargetRow First (including) row to convert, with range [0, targetHeight)
		 * @param numberTargetRows Number of rows to convert, with range [1, targetHeight - firstTargetRow]
		 * @tparam T Data type the pixel channel values
		 * @tparam tChannels Number of data channels, range: [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static void resizeSubset(const T* source, T* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstTargetRow, const unsigned int numberTargetRows);

		/**
		 * Apply an affine image transformation to an 8 bit per channel frame using nearest neighbor interpolation
		 * @param input The input frame that will be transformed, must be valid
		 * @param inputWidth Width of both input images in pixel, with range [1, infinity)
		 * @param inputHeight Height of both input images pixel, with range [1, infinity)
		 * @param inputPaddingElements The number of padding elements at the end of each input frame row, in elements, with range [0, infinity)
		 * @param affineTransform Affine transformation used to transform the given input frame, must be valid
		 * @param borderColor Color of undefined pixel positions, the size of the buffer must match to the number of channels, nullptr to assign 0x00 to each channel
		 * @param output The output frame using the given affine transform, must be valid
		 * @param outputWidth The width of the output image in pixel, with range [1, infinity)
		 * @param outputHeight The height of the output image in pixel, with range [1, infinity)
		 * @param outputPaddingElements The number of padding elements at the end of each output frame row, in elements, with range [0, infinity)
		 * @param firstOutputRow The first output row to be handled, with range [0, outputHeight)
		 * @param numberOutputRows Number of output rows to be handled, with range [1, outputHeight - firstOutputRow]
		 * @tparam tChannels Number of frame channels, range: [1, infinity)
		 */
		template <unsigned int tChannels>
		static void affine8BitPerChannelSubset(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const unsigned int inputPaddingElements, const SquareMatrix3* affineTransform, const uint8_t* borderColor, uint8_t* output, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int outputPaddingElements, const unsigned int firstOutputRow, const unsigned int numberOutputRows);

		/**
		 * Copies the image content of an input image to a subset of an output image by application of a given homography transformation.
		 * @param input The input frame that will be transformed, must be valid
		 * @param inputWidth Width of both input images in pixel, with range [1, infinity)
		 * @param inputHeight Height of both input images pixel, with range [1, infinity)
		 * @param input_H_output Homography used to transform the given input frame by following equation: inputPoint = input_H_output * outputPoint, must be valid
		 * @param borderColor Color of undefined pixel positions, the size of the buffer must match to the number of channels, nullptr to assign 0 to each channel
		 * @param output The output frame using the given homography, must be valid
		 * @param outputWidth The width of the output image in pixel, with range [1, infinity)
		 * @param outputHeight The height of the output image in pixel, with range [1, infinity)
		 * @param inputPaddingElements The number of padding elements at the end of each input row, in elements, with range [0, infinity)
		 * @param outputPaddingElements The number of padding elements at the end of each output row, in elements, with range [0, infinity)
		 * @param firstOutputRow The first output row to be handled, with range [0, outputHeight)
		 * @param numberOutputRows Number of output rows to be handled, with range [1, outputHeight - firstOutputRow]
		 * @tparam T Data type of each pixel channel, e.g., float, double, int
		 * @tparam tChannels Number of channels of the frame, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static void homographySubset(const T* input, const unsigned int inputWidth, const unsigned int inputHeight, const SquareMatrix3* input_H_output, const T* borderColor, T* output, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, const unsigned int firstOutputRow, const unsigned int numberOutputRows);

 #if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

		/**
		 * Apply an affine image transformation to an 8 bit per channel frame using nearest neighbor interpolation (using SSE)
		 * However, this function disregards the last row completely and only uses the top two rows, i.e., the elements a through f.
		 * @param input The input frame that will be transformed
		 * @param inputWidth Width of both input images in pixel, with range [1, infinity)
		 * @param inputHeight Height of both input images pixel, with range [1, infinity)
		 * @param inputPaddingElements The number of padding elements at the end of each input frame row, in elements, with range [0, infinity)
		 * @param affineTransform Affine transformation which is applied to input frame.
		 * @param borderColor Color of undefined pixel positions, the size of the buffer must match to the number of channels, nullptr to assign 0x00 to each channel
		 * @param output The output frame where the result of the transformation will be stored
		 * @param outputWidth The width of the output image in pixel, with range [4, infinity)
		 * @param outputHeight The height of the output image in pixel, with range [1, infinity)
		 * @param outputPaddingElements The number of padding elements at the end of each output frame row, in elements, with range [0, infinity)
		 * @param firstOutputRow The first output row to be handled
		 * @param numberOutputRows Number of output rows to be handled
		 * @tparam tChannels Number of frame channels
		 * @see affine8BitPerChannelSubset(), affine8BitPerChannelSubsetNEON().
		 */
		template <unsigned int tChannels>
		static inline void affine8BitPerChannelSSESubset(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const unsigned int inputPaddingElements, const SquareMatrix3* affineTransform, const uint8_t* borderColor, uint8_t* output, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int outputPaddingElements, const unsigned int firstOutputRow, const unsigned int numberOutputRows);

		/**
		 * Copies the image content of an input image to a subset of an output image by application of a given homography transformation (using SSE).
		 * @param input The input frame that will be transformed, must be valid
		 * @param inputWidth Width of both input images in pixel, with range [1, infinity)
		 * @param inputHeight Height of both input images pixel, with range [1, infinity)
		 * @param input_H_output Homography used to transform the given input frame by following equation: inputPoint = input_H_output * outputPoint, must be valid
		 * @param borderColor Color of undefined pixel positions, the size of the buffer must match to the number of channels, nullptr to assign 0 to each channel
		 * @param output The output frame using the given homography, must be valid
		 * @param outputWidth The width of the output image in pixel, with range [1, infinity)
		 * @param outputHeight The height of the output image in pixel, with range [1, infinity)
		 * @param inputPaddingElements The number of padding elements at the end of each input row, in elements, with range [0, infinity)
		 * @param outputPaddingElements The number of padding elements at the end of each output row, in elements, with range [0, infinity)
		 * @param firstOutputRow The first output row to be handled, with range [0, outputHeight)
		 * @param numberOutputRows Number of output rows to be handled, with range [1, outputHeight - firstOutputRow]
		 * @tparam T Data type of each pixel channel, e.g., float, double, int
		 * @tparam tChannels Number of channels of the frame, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static void homographySSESubset(const T* input, const unsigned int inputWidth, const unsigned int inputHeight, const SquareMatrix3* input_H_output, const T* borderColor, T* output, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, const unsigned int firstOutputRow, const unsigned int numberOutputRows);

 #endif // OCEAN_HARDWARE_SSE_VERSION >= 41

 #if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

		/**
		 * Affine image transformation for 8-bit per channel frames using nearest neighbor interpolation (using NEON and integer fixed-point arithmetic)
		 * @param input The input frame that will be transformed, must be valid
		 * @param inputWidth Width of both input images in pixel, with range [1, 65536)
		 * @param inputHeight Height of both input images pixel, with range [1, 65536)
		 * @param inputPaddingElements The number of padding elements at the end of each input frame row, in elements, with range [0, infinity)
		 * @param affineTransform Affine transformation used to transform the given input frame, must be valid
		 * @param borderColor Color of undefined pixel positions, the size of the buffer must match to the number of channels, nullptr to assign 0x00 to each channel
		 * @param output The output frame using the given affine transform, must be valid
		 * @param outputWidth The width of the output image in pixel, with range [1, 65536)
		 * @param outputHeight The height of the output image in pixel, with range [1, 65536)
		 * @param outputPaddingElements The number of padding elements at the end of each output frame row, in elements, with range [0, infinity)
		 * @param firstOutputRow The first output row to be handled, with range [0, outputHeight)
		 * @param numberOutputRows Number of output rows to be handled, with range [1, outputHeight - firstOutputRow]
		 * @tparam tChannels Number of frame channels
		 */
		template <unsigned int tChannels>
		static inline void affine8BitPerChannelIntegerNEONSubset(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const unsigned int inputPaddingElements, const SquareMatrix3* affineTransform, const uint8_t* borderColor, uint8_t* output, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int outputPaddingElements, const unsigned int firstOutputRow, const unsigned int numberOutputRows);

		/**
		 * Copies the image content of an input image to a subset of an output image by application of a given homography transformation (using NEON).
		 * Beware: The output width 'outputWidth' must be >= 4, use homographySubset for small output frames
		 * @param input The input frame that will be transformed
		 * @param inputWidth Width of both input images in pixel, with range [1, infinity)
		 * @param inputHeight Height of both input images pixel, with range [1, infinity)
		 * @param input_H_output Homography used to transform the given input frame by following equation: inputPoint = input_H_output * outputPoint, must be valid
		 * @param borderColor Color of undefined pixel positions, the size of the buffer must match to the number of channels, @c nullptr to assign 0 to each channel
		 * @param output The output frame using the given homography
		 * @param outputWidth The width of the output image in pixel, with range [4, infinity)
		 * @param outputHeight The height of the output image in pixel, with range [1, infinity)
		 * @param inputPaddingElements The number of padding elements at the end of each input row, in elements, with range [0, infinity)
		 * @param outputPaddingElements The number of padding elements at the end of each output row, in elements, with range [0, infinity)
		 * @param firstOutputRow The first output row to be handled
		 * @param numberOutputRows Number of output rows to be handled
		 * @tparam T Data type of each pixel channel, e.g., float, double, int
		 * @tparam tChannels Number of channels of the frame, with range [1, infinity)
		 * @see homographySubset().
		 */
		template <typename T, unsigned int tChannels>
		static inline void homographyNEONSubset(const T* input, const unsigned int inputWidth, const unsigned int inputHeight, const SquareMatrix3* input_H_output, const T* borderColor, T* output, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, const unsigned int firstOutputRow, const unsigned int numberOutputRows);

 #endif // OCEAN_HARDWARE_NEON_VERSION

		/**
		 * Transforms an 8 bit per channel frame using the given homography.
		 * @param input The input frame that will be transformed, must be valid
		 * @param inputWidth Width of both input images in pixel, with range [1, infinity)
		 * @param inputHeight Height of both input images pixel, with range [1, infinity)
		 * @param inputPaddingElements The number of padding elements at the end of each input frame row, in elements, with range [0, infinity)
		 * @param input_H_output Homography used to transform the given input frame by following equation: inputPoint = input_H_output * outputPoint, must be valid
		 * @param output The output frame using the given homography, must be valid
		 * @param outputPaddingElements The number of padding elements at the end of each output frame row, in elements, with range [0, infinity)
		 * @param outputMask Mask frame with 8 bit per pixel defining whether an output frame pixel has a valid corresponding pixel in the input frame
		 * @param outputMaskPaddingElements The number of padding elements at the end of each output mask row, in elements, with range [0, infinity)
		 * @param maskValue 8 bit mask values for reference pixels lying inside the given camera frame, reference pixels lying outside the camera frame will be assigned with (0xFF - maskValue)
		 * @param outputOriginX The horizontal coordinate of the output frame's origin
		 * @param outputOriginY The vertical coordinate of the output frame's origin
		 * @param outputWidth The width of the output image in pixel, with range [1, infinity)
		 * @param outputHeight The height of the output image in pixel, with range [1, infinity)
		 * @param firstOutputRow The first output row to be handled, with range [0, outputHeight)
		 * @param numberOutputRows Number of output rows to be handled, with range [1, outputHeight - firstOutputRow]
		 * @tparam tChannels Number of frame channels
		 */
		template <unsigned int tChannels>
		static inline void homographyMask8BitPerChannelSubset(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const unsigned int inputPaddingElements, const SquareMatrix3* input_H_output, uint8_t* output, const unsigned int outputPaddingElements, uint8_t* outputMask, const unsigned int outputMaskPaddingElements, const uint8_t maskValue, const int outputOriginX, const int outputOriginY, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int firstOutputRow, const unsigned int numberOutputRows);

		/**
		 * Transforms a subset of a given input frame (with zipped pixel format) into an output frame by application of an interpolation lookup table.
		 * The output frame must have the same pixel format and pixel origin as the input frame.<br>
		 * @param input the input frame which will be transformed, must be valid
		 * @param inputWidth The width of the given input frame in pixel, with range [1, infinity)
		 * @param inputHeight The height of the given input frame in pixel, with range [1, infinity)
		 * @param lookupTable The lookup table which defines the transformation, must be valid
		 * @param offset True, if the lookup table store local offsets; False, if the lookup table stores absolute positions
		 * @param borderColor Color of undefined pixel positions, the size of the buffer must match to the number of channels, nullptr to assign 0x00 to each channel
		 * @param output Resulting output frame with frame dimension equal to the size of the given lookup table, must be valid
		 * @param inputPaddingElements The number of padding elements at the end of each input row, in elements, with range [0, infinity)
		 * @param outputPaddingElements The number of padding elements at the end of each output row, in elements, with range [0, infinity)
		 * @param firstRow First row to be handled, with range [0, lookupTable->sizeY())
		 * @param numberRows Number of rows to be handled, with range [1, lookupTable->sizeY() - firstRow]
		 * @tparam tChannels Number of channels of the frame
		 */
		template <unsigned int tChannels>
		static void transform8BitPerChannelSubset(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const LookupTable* lookupTable, const bool offset, const uint8_t* borderColor, uint8_t* output, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Transforms a given input frame (with zipped pixel format) into an output frame by application of an interpolation lookup table.
		 * The output frame must have the same pixel format and pixel origin as the input frame.<br>
		 * Input frame pixels lying outside the frame will be masked in the resulting output mask frame, further theses pixels are untouched in the output frame.<br>
		 * @param input The input frame which will be transformed, must be valid
		 * @param inputWidth The width of the given input frame in pixel, with range [1, infinity)
		 * @param inputHeight The height of the given input frame in pixel, with range [1, infinity)
		 * @param inputPaddingElements The number of padding elements at the end of each input frame row, in elements, with range [0, infinity)
		 * @param lookupTable The lookup table which defines the transformation, must be valid
		 * @param offset True, if the lookup table store local offsets; False, if the lookup table stores absolute positions
		 * @param output Resulting output frame with frame dimension equal to the size of the given lookup table, must be valid
		 * @param outputPaddingElements The number of padding elements at the end of each output frame row, in elements, with range [0, infinity)
		 * @param outputMask Resulting mask frame with 8 bit per pixel defining whether an output frame pixel has a valid corresponding pixel in the input frame
		 * @param outputMaskPaddingElements The number of padding elements at the end of each output mask row, in elements, with range [0, infinity)
		 * @param maskValue 8 bit mask values for pixels lying inside the input frame, pixels lying outside the input frame will be assigned with (0xFF - maskValue)
		 * @param firstRow First row to be handled, with range [0, lookupTable->sizeY())
		 * @param numberRows Number of rows to be handled, with range [1, lookupTable->sizeY() - firstRow]
		 * @tparam tChannels Number of channels of the frame
		 */
		template <unsigned int tChannels>
		static void transformMask8BitPerChannelSubset(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const unsigned int inputPaddingElements, const LookupTable* lookupTable, const bool offset, uint8_t* output, const unsigned int outputPaddingElements, uint8_t* outputMask, const unsigned int outputMaskPaddingElements, const uint8_t maskValue, const unsigned int firstRow, const unsigned int numberRows);
};

inline bool FrameInterpolatorNearestPixel::Comfort::resize(Frame& frame, const unsigned int targetWidth, const unsigned int targetHeight, Worker* worker)
{
	ocean_assert(frame && targetWidth >= 1u && targetHeight >= 1u);

	Frame tmpFrame(FrameType(frame, targetWidth, targetHeight));

	if (!resize(frame, tmpFrame, worker))
	{
		return false;
	}

	frame = std::move(tmpFrame);
	return true;
}

inline bool FrameInterpolatorNearestPixel::Comfort::rotate90(const Frame& input, Frame& output, const bool clockwise, Worker* worker)
{
	return FrameTransposer::Comfort::rotate90(input, output, clockwise, worker);
}

inline bool FrameInterpolatorNearestPixel::Comfort::rotate180(const Frame& input, Frame& output, Worker* worker)
{
	return FrameTransposer::Comfort::rotate180(input, output, worker);
}

inline bool FrameInterpolatorNearestPixel::Comfort::rotate(const Frame& input, Frame& output, const int angle, Worker* worker)
{
	return FrameTransposer::Comfort::rotate(input, output, angle, worker);
}

template <typename T, unsigned int tChannels>
inline void FrameInterpolatorNearestPixel::resize(const T* source, T* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source && target);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameInterpolatorNearestPixel::resizeSubset<T, tChannels>, source, target, sourceWidth, sourceHeight, targetWidth, targetHeight, sourcePaddingElements, targetPaddingElements, 0u, 0u), 0u, targetHeight);
	}
	else
	{
		resizeSubset<T, tChannels>(source, target, sourceWidth, sourceHeight, targetWidth, targetHeight, sourcePaddingElements, targetPaddingElements, 0u, targetHeight);
	}
}

template <unsigned int tChannels>
inline void FrameInterpolatorNearestPixel::affine8BitPerChannel(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const SquareMatrix3& affineTransform, const uint8_t* borderColor, uint8_t* output, const PixelPositionI& outputOrigin, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, Worker* worker)
{
	// Merge the additional translation into the affine transformation
	const SquareMatrix3 adjustedAffineTransform = affineTransform * SquareMatrix3(Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(Scalar(outputOrigin.x()), Scalar(outputOrigin.y()), 1));

	if (worker)
	{
		if (outputWidth >= 4u)
		{
#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41
			worker->executeFunction(Worker::Function::createStatic(&FrameInterpolatorNearestPixel::affine8BitPerChannelSSESubset<tChannels>, input, inputWidth, inputHeight, inputPaddingElements, &adjustedAffineTransform, borderColor, output, outputWidth, outputHeight, outputPaddingElements, 0u, 0u), 0, outputHeight, 10u, 11u, 20u);
			return;
#elif defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
			if (inputWidth <= 65535u && inputHeight <= 65535u && outputWidth <= 65535u && outputHeight <= 65535u)
			{
				worker->executeFunction(Worker::Function::createStatic(&FrameInterpolatorNearestPixel::affine8BitPerChannelIntegerNEONSubset<tChannels>, input, inputWidth, inputHeight, inputPaddingElements, &adjustedAffineTransform, borderColor, output, outputWidth, outputHeight, outputPaddingElements, 0u, 0u), 0, outputHeight, 10u, 11u, 32u);
				return;
			}
#endif
		}

		worker->executeFunction(Worker::Function::createStatic(&FrameInterpolatorNearestPixel::affine8BitPerChannelSubset<tChannels>, input, inputWidth, inputHeight, inputPaddingElements, &adjustedAffineTransform, borderColor, output, outputWidth, outputHeight, outputPaddingElements, 0u, 0u), 0, outputHeight, 10u, 11u, 20u);
	}
	else
	{
		if (outputWidth >= 4u)
		{
#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41
			affine8BitPerChannelSSESubset<tChannels>(input, inputWidth, inputHeight, inputPaddingElements, &adjustedAffineTransform, borderColor, output, outputWidth, outputHeight, outputPaddingElements, 0u, outputHeight);
			return;
#elif defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
			if (inputWidth <= 65535u && inputHeight <= 65535u && outputWidth <= 65535u && outputHeight <= 65535u)
			{
				affine8BitPerChannelIntegerNEONSubset<tChannels>(input, inputWidth, inputHeight, inputPaddingElements, &adjustedAffineTransform, borderColor, output, outputWidth, outputHeight, outputPaddingElements, 0u, outputHeight);
				return;
			}
#endif
		}

		affine8BitPerChannelSubset<tChannels>(input, inputWidth, inputHeight, inputPaddingElements, &adjustedAffineTransform, borderColor, output, outputWidth, outputHeight, outputPaddingElements, 0u, outputHeight);
	}
}

template <typename T, unsigned int tChannels>
inline void FrameInterpolatorNearestPixel::homography(const T* input, const unsigned int inputWidth, const unsigned int inputHeight, const SquareMatrix3& input_H_output, const T* borderColor, T* output, const PixelPositionI& outputOrigin, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, Worker* worker)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	// Merge the additional translation into the homography
	const SquareMatrix3 input_H_adjustedOutput = input_H_output * SquareMatrix3(Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(Scalar(outputOrigin.x()), Scalar(outputOrigin.y()), 1));

	typedef typename TypeMapper<T>::Type MappedTypeT;

	if (worker)
	{
		if (outputWidth >= 4u)
		{
#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41
			worker->executeFunction(Worker::Function::createStatic(&FrameInterpolatorNearestPixel::homographySSESubset<MappedTypeT, tChannels>, (const MappedTypeT*)(input), inputWidth, inputHeight, &input_H_adjustedOutput, (const MappedTypeT*)(borderColor), (MappedTypeT*)(output), outputWidth, outputHeight, inputPaddingElements, outputPaddingElements, 0u, 0u), 0, outputHeight, 10u, 11u, 20u);
			return;
#elif defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
			worker->executeFunction(Worker::Function::createStatic(&FrameInterpolatorNearestPixel::homographyNEONSubset<MappedTypeT, tChannels>, (const MappedTypeT*)(input), inputWidth, inputHeight, &input_H_adjustedOutput, (const MappedTypeT*)(borderColor), (MappedTypeT*)(output), outputWidth, outputHeight, inputPaddingElements, outputPaddingElements, 0u, 0u), 0, outputHeight, 10u, 11u, 20u);
			return;
#endif
		}

		worker->executeFunction(Worker::Function::createStatic(&FrameInterpolatorNearestPixel::homographySubset<MappedTypeT, tChannels>, (const MappedTypeT*)(input), inputWidth, inputHeight, &input_H_adjustedOutput, (const MappedTypeT*)(borderColor), (MappedTypeT*)(output), outputWidth, outputHeight, inputPaddingElements, outputPaddingElements, 0u, 0u), 0, outputHeight, 10u, 11u, 20u);
	}
	else
	{
		if (outputWidth >= 4u)
		{
#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41
			homographySSESubset<MappedTypeT, tChannels>((const MappedTypeT*)(input), inputWidth, inputHeight, &input_H_adjustedOutput, (const MappedTypeT*)(borderColor), (MappedTypeT*)(output), outputWidth, outputHeight, inputPaddingElements, outputPaddingElements, 0u, outputHeight);
			return;
#elif defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
			homographyNEONSubset<MappedTypeT, tChannels>((const MappedTypeT*)(input), inputWidth, inputHeight, &input_H_adjustedOutput, (const MappedTypeT*)(borderColor), (MappedTypeT*)(output), outputWidth, outputHeight, inputPaddingElements, outputPaddingElements, 0u, outputHeight);
			return;
#endif
		}

		homographySubset<MappedTypeT, tChannels>((const MappedTypeT*)(input), inputWidth, inputHeight, &input_H_adjustedOutput, (const MappedTypeT*)(borderColor), (MappedTypeT*)(output), outputWidth, outputHeight, inputPaddingElements, outputPaddingElements, 0u, outputHeight);
	}
}

template <unsigned int tChannels>
inline void FrameInterpolatorNearestPixel::homographyMask8BitPerChannel(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const unsigned int inputPaddingElements, const SquareMatrix3& input_H_output, uint8_t* output, const unsigned int outputPaddingElements, uint8_t* outputMask, const unsigned int outputMaskPaddingElements, const PixelPositionI& outputOrigin, const unsigned int outputWidth, const unsigned int outputHeight, Worker* worker, const uint8_t maskValue)
{
	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameInterpolatorNearestPixel::homographyMask8BitPerChannelSubset<tChannels>, input, inputWidth, inputHeight, inputPaddingElements, &input_H_output, output, outputPaddingElements, outputMask, outputMaskPaddingElements, maskValue, outputOrigin.x(), outputOrigin.y(), outputWidth, outputHeight, 0u, 0u), 0u, outputHeight, 14u, 15u, 20u);
	}
	else
	{
		homographyMask8BitPerChannelSubset<tChannels>(input, inputWidth, inputHeight, inputPaddingElements, &input_H_output, output, outputPaddingElements, outputMask, outputMaskPaddingElements, maskValue, outputOrigin.x(), outputOrigin.y(), outputWidth, outputHeight, 0u, outputHeight);
	}
}

template <unsigned int tChannels>
inline void FrameInterpolatorNearestPixel::transform8BitPerChannel(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const LookupTable& lookupTable, const bool offset, const uint8_t* borderColor, uint8_t* output, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, Worker* worker)
{
	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameInterpolatorNearestPixel::transform8BitPerChannelSubset<tChannels>, input, inputWidth, inputHeight, &lookupTable, offset, borderColor, output, inputPaddingElements, outputPaddingElements, 0u, 0u), 0u, (unsigned int)(lookupTable.sizeY()), 9u, 10u, 20u);
	}
	else
	{
		transform8BitPerChannelSubset<tChannels>(input, inputWidth, inputHeight, &lookupTable, offset, borderColor, output, inputPaddingElements, outputPaddingElements, 0u, (unsigned int)(lookupTable.sizeY()));
	}
}

template <unsigned int tChannels>
inline void FrameInterpolatorNearestPixel::transformMask8BitPerChannel(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const unsigned int inputPaddingElements, const LookupTable& lookupTable, const bool offset, uint8_t* output, const unsigned int outputPaddingElements, uint8_t* outputMask, const unsigned int outputMaskPaddingElements, Worker* worker, const uint8_t maskValue)
{
	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameInterpolatorNearestPixel::transformMask8BitPerChannelSubset<tChannels>, input, inputWidth, inputHeight, inputPaddingElements, &lookupTable, offset, output, outputPaddingElements, outputMask, outputMaskPaddingElements, maskValue, 0u, 0u), 0u, (unsigned int)(lookupTable.sizeY()), 11u, 12u, 20u);
	}
	else
	{
		transformMask8BitPerChannelSubset<tChannels>(input, inputWidth, inputHeight, inputPaddingElements, &lookupTable, offset, output, outputPaddingElements, outputMask, outputMaskPaddingElements, maskValue, 0u, (unsigned int)lookupTable.sizeY());
	}
}

template <typename TElementType, unsigned int tChannels>
inline void FrameInterpolatorNearestPixel::rotate90(const TElementType* source, TElementType* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const bool clockwise, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(source != target);
	ocean_assert(sourceWidth >= 1u && sourceHeight >= 1u);

	FrameTransposer::rotate90<TElementType, tChannels>(source, target, sourceWidth, sourceHeight, clockwise, sourcePaddingElements, targetPaddingElements, worker);
}

template <typename T, unsigned int tChannels>
void FrameInterpolatorNearestPixel::resizeSubset(const T* source, T* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstTargetRow, const unsigned int numberTargetRows)
{
	static_assert(tChannels > 0u, "Invalid channel number!");
	static_assert(sizeof(T) != 0, "Invalid data type!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(sourceWidth != 0u && sourceHeight != 0u);
	ocean_assert(targetWidth != 0u && targetHeight != 0u);

	ocean_assert(firstTargetRow + numberTargetRows <= targetHeight);

	const unsigned int sourceStrideElements = sourceWidth * tChannels + sourcePaddingElements;
	const unsigned int targetStrideElements = targetWidth * tChannels + targetPaddingElements;

	Memory memoryHorizontalLookups = Memory::create<unsigned int>(targetWidth);
	unsigned int* horizontalLookups = memoryHorizontalLookups.data<unsigned int>();

	for (unsigned int tx = 0u; tx < targetWidth; ++tx)
	{
		const unsigned int sx = tx * sourceWidth / targetWidth;
		ocean_assert(sx < sourceWidth);

		horizontalLookups[tx] = sx * tChannels;
	}

	target += firstTargetRow * targetStrideElements;

	for (unsigned int ty = firstTargetRow; ty < firstTargetRow + numberTargetRows; ++ty)
	{
		const unsigned int sy = ty * sourceHeight / targetHeight;
		ocean_assert(sy < sourceHeight);

		const T* const sourceRow = source + sy * sourceStrideElements;

		for (unsigned int tx = 0; tx < targetWidth; ++tx)
		{
			const T* const sourcePointer = sourceRow + horizontalLookups[tx];

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				*target++ = sourcePointer[n];
			}
		}

		target += targetPaddingElements;
	}
}

template <unsigned int tChannels>
void FrameInterpolatorNearestPixel::affine8BitPerChannelSubset(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const unsigned int inputPaddingElements, const SquareMatrix3* affineTransform, const uint8_t* borderColor, uint8_t* output, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int outputPaddingElements, const unsigned int firstOutputRow, const unsigned int numberOutputRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(input != nullptr && output != nullptr);
	ocean_assert(inputWidth > 0u && inputHeight > 0u);
	ocean_assert_and_suppress_unused(outputWidth > 0u && outputHeight > 0u, outputHeight);
	ocean_assert(affineTransform);
	ocean_assert(!affineTransform->isNull() && Numeric::isEqualEps((*affineTransform)[2]) && Numeric::isEqualEps((*affineTransform)[5]));

	ocean_assert(firstOutputRow + numberOutputRows <= outputHeight);

	typedef typename DataType<uint8_t, tChannels>::Type PixelType;

	const uint8_t zeroColor[tChannels] = {uint8_t(0)};
	const PixelType* const bColor = borderColor ? (PixelType*)borderColor : (PixelType*)zeroColor;

	PixelType* outputData = (PixelType*)(output + firstOutputRow * (outputWidth * tChannels + outputPaddingElements));

	for (unsigned int y = firstOutputRow; y < firstOutputRow + numberOutputRows; ++y)
	{
		//
		// We can slightly optimize the 3x3 matrix multiplication:
		//
		// | X0 Y0 Z0 |   | x |
		// | X1 Y1 Z1 | * | y |
		// |  0  0  1 |   | 1 |
		//
		// | xx |   | X0 * x |   | Y0 * y + Z0 |
		// | yy | = | X1 * x | + | Y1 * y + Z1 |
		//
		// As y is constant within the inner loop, the two terms on the right side in the above equations can be pre-calculated:
		//
		//  C0 = Y0 * y + Z0
		//  C1 = Y1 * y + Z1
		//
		// So the computation becomes:
		//
		// | x' |   | X0 * x |   | C0 |
		// | y' | = | X1 * x | + | C1 |
		//

		const Vector2 X(affineTransform->data() + 0);
		const Vector2 c(Vector2(affineTransform->data() + 3) * Scalar(y) + Vector2(affineTransform->data() + 6));

		for (unsigned int x = 0u; x < outputWidth; ++x)
		{
			const Vector2 inputPosition = X * Scalar(x) + c;

#ifdef OCEAN_DEBUG
			const Scalar debugX = (*affineTransform)[0] * Scalar(x) + (*affineTransform)[3] * Scalar(y) + (*affineTransform)[6];
			const Scalar debugY = (*affineTransform)[1] * Scalar(x) + (*affineTransform)[4] * Scalar(y) + (*affineTransform)[7];
			ocean_assert(inputPosition.isEqual(Vector2(debugX, debugY), Scalar(0.01)));
#endif

			const unsigned int inputX = Numeric::round32(inputPosition.x());
			const unsigned int inputY = Numeric::round32(inputPosition.y());

			if (inputX < inputWidth && inputY < inputHeight)
				*outputData = *(PixelType*)(input + inputY * (inputWidth * tChannels + inputPaddingElements) + inputX * tChannels);
			else
				*outputData = *bColor;

			outputData++;
		}

		outputData = (PixelType*)((uint8_t*)outputData + outputPaddingElements);
	}
}

template <typename T, unsigned int tChannels>
void FrameInterpolatorNearestPixel::homographySubset(const T* input, const unsigned int inputWidth, const unsigned int inputHeight, const SquareMatrix3* input_H_output, const T* borderColor, T* output, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, const unsigned int firstOutputRow, const unsigned int numberOutputRows)
{
	static_assert(tChannels > 0u, "Invalid channel number!");

	ocean_assert(input != nullptr && output != nullptr);
	ocean_assert(inputWidth > 0u && inputHeight > 0u);
	ocean_assert(outputWidth > 0u && outputHeight > 0u);
	ocean_assert(input_H_output != nullptr && !input_H_output->isSingular());

	ocean_assert_and_suppress_unused(firstOutputRow + numberOutputRows <= outputHeight, outputHeight);

	const unsigned int inputStrideElements = inputWidth * tChannels + inputPaddingElements;
	const unsigned int outputStrideElements = outputWidth * tChannels + outputPaddingElements;

	typedef typename DataType<T, tChannels>::Type PixelType;

	const T zeroColor[tChannels] = {T(0)};
	const PixelType bColor = borderColor ? *(const PixelType*)(borderColor) : *(const PixelType*)(zeroColor);

	for (unsigned int y = firstOutputRow; y < firstOutputRow + numberOutputRows; ++y)
	{
		PixelType* outputData = (PixelType*)(output + y * outputStrideElements);

		for (unsigned int x = 0u; x < outputWidth; ++x)
		{
			const Vector2 outputPosition = Vector2(Scalar(x), Scalar(y));
			const Vector2 inputPosition(*input_H_output * outputPosition);

			const unsigned int inputX = Numeric::round32(inputPosition.x());
			const unsigned int inputY = Numeric::round32(inputPosition.y());

			if (inputX < inputWidth && inputY < inputHeight)
			{
				*outputData = *((const PixelType*)(input + inputY * inputStrideElements) + inputX);
			}
			else
			{
				*outputData = bColor;
			}

			outputData++;
		}
	}
}

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

template <unsigned int tChannels>
inline void FrameInterpolatorNearestPixel::affine8BitPerChannelSSESubset(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const unsigned int inputPaddingElements, const SquareMatrix3* affineTransform, const uint8_t* borderColor, uint8_t* output, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int outputPaddingElements, const unsigned int firstOutputRow, const unsigned int numberOutputRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(input && output);
	ocean_assert(inputWidth > 0u && inputHeight > 0u);
	ocean_assert(outputWidth >= 4u && outputHeight > 0u);
	ocean_assert(affineTransform);
	ocean_assert(!affineTransform->isNull() && Numeric::isEqualEps((*affineTransform)[2]) && Numeric::isEqualEps((*affineTransform)[5]));

	ocean_assert_and_suppress_unused(firstOutputRow + numberOutputRows <= outputHeight, outputHeight);

	typedef typename DataType<uint8_t, tChannels>::Type PixelType;

	const uint8_t zeroColor[tChannels] = {uint8_t(0)};
	const PixelType* const bColor = borderColor ? (PixelType*)borderColor : (PixelType*)zeroColor;

	PixelType* outputPixelData = (PixelType*)(output + firstOutputRow * (outputWidth * tChannels + outputPaddingElements));

	OCEAN_ALIGN_DATA(16)
	unsigned int nearestNeighbours[4];

	// we store 4 floats: [X0, X0, X0, X0], and same with X1 and X2
	const __m128 m128_f_X0 = _mm_set_ps1(float((*affineTransform)(0, 0)));
	const __m128 m128_f_X1 = _mm_set_ps1(float((*affineTransform)(1, 0)));

	// m128_u_inputStrideElements = [rowStride, rowStride, rowStride, rowStride], rowStride = inputWidth * tChannels + inputPaddingElements
	const __m128i m128_i_inputStrideElements = _mm_set1_epi32(inputWidth * tChannels + inputPaddingElements);

	// m128_u_channels = [tChannels, tChannels, tChannels, tChannels]
	const __m128i m128_i_channels = _mm_set1_epi32(tChannels);

	// m128_i_inputWidth_1 = [inputWidth - 1u, inputWidth - 1u, inputWidth - 1u, inputWidth - 1u]
	const __m128i m128_i_inputWidth_1 = _mm_set1_epi32(inputWidth - 1u);

	// m128_i_inputHeight_1 = [inputHeight - 1u, inputHeight - 1u, inputHeight - 1u, inputHeight - 1u]
	const __m128i m128_i_inputHeight_1 = _mm_set1_epi32(inputHeight - 1u);

	// m128_i_zero = [0, 0, 0, 0]
	const __m128i m128_i_zero = _mm_setzero_si128();

	// Indices (of elements) above this value in the input image are considered as outside of the image (intentionally not counting the last padding elements)
	const unsigned int inputElementsEnd = inputHeight * inputWidth * tChannels + (inputHeight - 1u) * inputPaddingElements;

	for (unsigned int y = firstOutputRow; y < firstOutputRow + numberOutputRows; ++y)
	{
		// We can slightly optimize the 3x3 matrix multiplication:
		//
		// | X0 Y0 Z0 |   | x |
		// | X1 Y1 Z1 | * | y |
		// |  0  0  1 |   | 1 |
		//
		// | xx |   | X0 * x |   | Y0 * y + Z0 |
		// | yy | = | X1 * x | + | Y1 * y + Z1 |
		//
		// As y is constant within the inner loop, the two terms on the right side in the above equations can be pre-calculated:
		//
		//  C0 = Y0 * y + Z0
		//  C1 = Y1 * y + Z1
		//
		// So the computation becomes:
		//
		// | x' |   | X0 * x |   | C0 |
		// | y' | = | X1 * x | + | C1 |

		// we store 4 floats: [C0, C0, C0, C0], and same with C1 and C2
		const __m128 m128_f_C0 = _mm_set_ps1(float((*affineTransform)(0, 1) * Scalar(y) + (*affineTransform)(0, 2)));
		const __m128 m128_f_C1 = _mm_set_ps1(float((*affineTransform)(1, 1) * Scalar(y) + (*affineTransform)(1, 2)));

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

			// we calculate xx and yy for [x + 3.0f, x + 2.0f, x + 1.0f, x + 0.0f]
			const __m128 m128_f_inputX = _mm_add_ps(_mm_mul_ps(m128_f_X0, m128_f_x_0123), m128_f_C0);
			const __m128 m128_f_inputY = _mm_add_ps(_mm_mul_ps(m128_f_X1, m128_f_x_0123), m128_f_C1);

			// Compute the coordinates of the nearest neighbors
			const __m128i m128_i_inputX = _mm_cvtps_epi32(_mm_round_ps(m128_f_inputX, _MM_FROUND_TO_NEAREST_INT)); // x' = (int)round(x)
			const __m128i m128_i_inputY = _mm_cvtps_epi32(_mm_round_ps(m128_f_inputY, _MM_FROUND_TO_NEAREST_INT)); // y' = (int)round(y)

			// Note: Detection of input position outside the input image
			//
			// If the input point is outside the input image, then set the index
			// of its nearest neighbor to a value that is above the number of
			// available pixels in the image. When writing to the output, a
			// check will make sure to use the background color for those
			// pixels:
			//
			// nearestNeighbour = isOutsideImage ? 0xFFFFFFFF : y * w + x
			// output = nearestNeighbour < inputPixelIndexEnd ? foregroundColor : backgroundColor.
			//
			// This approach keeps the amount of data that has to be transferred
			// between SSE and CPU registers to a minimum.

			// isOutsideImage = (inputX < 0 || inputX > (width - 1u) || inputY < 0 || inputY > (height - 1u) ? 0xFFFFFFFF : 0x00000000;
			const __m128i m128_i_isOutsideImage = _mm_or_si128(
				_mm_or_si128(_mm_cmplt_epi32(m128_i_inputX, m128_i_zero), _mm_cmplt_epi32(m128_i_inputY, m128_i_zero)),
				_mm_or_si128(_mm_cmpgt_epi32(m128_i_inputX, m128_i_inputWidth_1), _mm_cmpgt_epi32(m128_i_inputY, m128_i_inputHeight_1)));

			// Compute pixel index of the nearest neighbors of the valid pixels and store their pixel values
			// nearestNeighborsElement = (isOutsideImage ? 0xFFFFFFFF : (inputY * inputStrideElements) + (inputX * channels))
			const __m128i m_128_i_nearestNeighborElements = _mm_or_si128(m128_i_isOutsideImage, _mm_add_epi32(_mm_mullo_epi32(m128_i_inputY, m128_i_inputStrideElements), _mm_mullo_epi32(m128_i_inputX, m128_i_channels)));
			_mm_store_si128((__m128i*)nearestNeighbours, m_128_i_nearestNeighborElements);

			// Update the output pixels
			outputPixelData[0] = nearestNeighbours[0] < inputElementsEnd ? *(const PixelType*)(input + nearestNeighbours[0]) : *bColor;
			outputPixelData[1] = nearestNeighbours[1] < inputElementsEnd ? *(const PixelType*)(input + nearestNeighbours[1]) : *bColor;
			outputPixelData[2] = nearestNeighbours[2] < inputElementsEnd ? *(const PixelType*)(input + nearestNeighbours[2]) : *bColor;
			outputPixelData[3] = nearestNeighbours[3] < inputElementsEnd ? *(const PixelType*)(input + nearestNeighbours[3]) : *bColor;

			outputPixelData += 4u;
		}

		outputPixelData = (PixelType*)((uint8_t*)outputPixelData + outputPaddingElements);
	}
}

template <typename T, unsigned int tChannels>
void FrameInterpolatorNearestPixel::homographySSESubset(const T* input, const unsigned int inputWidth, const unsigned int inputHeight, const SquareMatrix3* input_H_output, const T* borderColor, T* output, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, const unsigned int firstOutputRow, const unsigned int numberOutputRows)
{
	static_assert(tChannels > 0u, "Invalid channel number!");

	ocean_assert(input != nullptr && output != nullptr);
	ocean_assert(inputWidth > 0u && inputHeight > 0u);
	ocean_assert(outputWidth >= 4u && outputHeight > 0u);
	ocean_assert(input_H_output != nullptr && !input_H_output->isSingular());

	ocean_assert_and_suppress_unused(firstOutputRow + numberOutputRows <= outputHeight, outputHeight);

	const unsigned int inputStrideElements = inputWidth * tChannels + inputPaddingElements;
	const unsigned int outputStrideElements = outputWidth * tChannels + outputPaddingElements;

	typedef typename DataType<T, tChannels>::Type PixelType;

	const T zeroColor[tChannels] = {T(0)};
	const PixelType bColor = borderColor ? *(const PixelType*)(borderColor) : *(const PixelType*)(zeroColor);

	OCEAN_ALIGN_DATA(16) unsigned int nearestNeighbourElementOffsets[4];

	//                | X0 Y0 Z0 |            | x |
	// Homography H = | X1 Y1 Z1 |, point p = | y |
	//                | X2 Y2 Z2 |            | 1 |
	//
	//                  | xx |
	//     pp = H * p = | yy |
	//                  | zz |
	//
	//     | xx |   | X0 Y0 Z0 |   | x |
	// <=> | yy | = | X1 Y1 Z1 | * | y |
	//     | zz |   | X2 Y2 Z2 |   | 1 |
	//
	//     | xx |   | X0 * x |   | Y0 * y + Z0 |
	// <=> | yy | = | X1 * x | + | Y1 * y + Z1 |
	//     | zz |   | X2 * x |   | Y2 * y + Z2 |
	//
	//     | xx |   | X0 * x |   | C0 |      | Y0 * y + Z0 |
	// <=> | yy | = | X1 * x | + | C1 |, C = | Y1 * y + Z1 |
	//     | zz |   | X2 * x |   | C2 |      | Y2 * y + Z2 |
	//
	// Where C is a constant term that can be pre-computed (per image row)
	//
	//      | x' |   | xx / zz |   | (X0 * x + C0) / (X2 * x + C2) |
	// p' = | y' | = | yy / zz | = | (X1 * x + C1) / (X2 * x + C2) |

	// [Xi, Xi, Xi, Xi], i = {0, 1, 2}
	const __m128 m128_f_X0 = _mm_set_ps1((float)(*input_H_output)(0, 0));
	const __m128 m128_f_X1 = _mm_set_ps1((float)(*input_H_output)(1, 0));
	const __m128 m128_f_X2 = _mm_set_ps1((float)(*input_H_output)(2, 0));

	// Store 4 integers: [inputStrideElements, inputStrideElements, inputStrideElements, inputStrideElements]
	const __m128i m128_i_inputStrideElements = _mm_set1_epi32(inputStrideElements);

	const unsigned int inputPixelElementIndexEnd = inputHeight * inputStrideElements;

	// m128_i_inputWidth_1 = [inputWidth - 1u, inputWidth - 1u, inputWidth - 1u, inputWidth - 1u]
	const __m128i m128_i_inputWidth_1 = _mm_set1_epi32(inputWidth - 1u);

	// m128_i_inputHeight_1 = [inputHeight - 1u, inputHeight - 1u, inputHeight - 1u, inputHeight - 1u]
	const __m128i m128_i_inputHeight_1 = _mm_set1_epi32(inputHeight - 1u);

	// [tChannels, tChannels, tChannels tChannels]
	const __m128i m128_i_channels = _mm_set1_epi32(tChannels);

	// m128_i_zero = [0, 0, 0, 0]
	const __m128i m128_i_zero = _mm_setzero_si128();

	for (unsigned int y = firstOutputRow; y < firstOutputRow + numberOutputRows; ++y)
	{
		PixelType* outputPixelData = (PixelType*)(output + y * outputStrideElements);

		// Pre-compute the constant terms [Ci, Ci, Ci, Ci], i={0, 1, 2}
		const __m128 m128_f_C0 = _mm_set_ps1((float)((*input_H_output)(0, 1) * Scalar(y) + ((*input_H_output)(0, 2))));
		const __m128 m128_f_C1 = _mm_set_ps1((float)((*input_H_output)(1, 1) * Scalar(y) + ((*input_H_output)(1, 2))));
		const __m128 m128_f_C2 = _mm_set_ps1((float)((*input_H_output)(2, 1) * Scalar(y) + ((*input_H_output)(2, 2))));

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

			// Compute the coordinates of the nearest neighbors
			const __m128i m128_i_inputX = _mm_cvtps_epi32(_mm_round_ps(m128_f_inputX, _MM_FROUND_TO_NEAREST_INT)); // x' = (int)round(x)
			const __m128i m128_i_inputY = _mm_cvtps_epi32(_mm_round_ps(m128_f_inputY, _MM_FROUND_TO_NEAREST_INT)); // y' = (int)round(y)

			// Note: Detection of input position outside the input image
			//
			// If the input point is outside the input image, then set the index
			// of its nearest neighbor to a value that is above the number of
			// available pixels in the image. When writing to the output, a
			// check will make sure to use the background color for those
			// pixels:
			//
			// nearestNeighbour = isOutsideImage ? 0xFFFFFFFF : y * w + x
			// output = nearestNeighbour < inputPixelIndexEnd ? foregroundColor : backgroundColor.
			//
			// This approach keeps the amount of data that has to be transferred
			// between SSE and CPU registers to a minimum.

			// isOutsideImage = (inputX < 0 || inputX > (width - 1u) || inputY < 0 || inputY > (height - 1u) ? 0xFFFFFFFF : 0x00000000;
			const __m128i m128_i_isOutsideImage = _mm_or_si128(
				_mm_or_si128(_mm_cmplt_epi32(m128_i_inputX, m128_i_zero), _mm_cmplt_epi32(m128_i_inputY, m128_i_zero)),
				_mm_or_si128(_mm_cmpgt_epi32(m128_i_inputX, m128_i_inputWidth_1), _mm_cmpgt_epi32(m128_i_inputY, m128_i_inputHeight_1)));

			// Compute pixel index of the nearest neighbors of the valid pixels and store their pixel values
			// m_128_i_nearestNeighbors = (isOutsideImage ? 0xFFFFFFFF : inputY * inputWidth + inputX)
			const __m128i m_128_i_nearestNeighbors = _mm_or_si128(m128_i_isOutsideImage, _mm_add_epi32(_mm_mullo_epi32(m128_i_inputY, m128_i_inputStrideElements), _mm_mullo_epi32(m128_i_inputX, m128_i_channels))); // nn = y' * inputWidth + x'
			_mm_store_si128((__m128i*)nearestNeighbourElementOffsets, m_128_i_nearestNeighbors);

			// Update the output pixels
			outputPixelData[0] = nearestNeighbourElementOffsets[0] < inputPixelElementIndexEnd ? *(const PixelType*)(input + nearestNeighbourElementOffsets[0]) : bColor;
			outputPixelData[1] = nearestNeighbourElementOffsets[1] < inputPixelElementIndexEnd ? *(const PixelType*)(input + nearestNeighbourElementOffsets[1]) : bColor;
			outputPixelData[2] = nearestNeighbourElementOffsets[2] < inputPixelElementIndexEnd ? *(const PixelType*)(input + nearestNeighbourElementOffsets[2]) : bColor;
			outputPixelData[3] = nearestNeighbourElementOffsets[3] < inputPixelElementIndexEnd ? *(const PixelType*)(input + nearestNeighbourElementOffsets[3]) : bColor;

			outputPixelData += 4u;
		}
	}
}

#endif // OCEAN_HARDWARE_SSE_VERSION >= 41

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

template <unsigned int tChannels>
void FrameInterpolatorNearestPixel::affine8BitPerChannelIntegerNEONSubset(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const unsigned int inputPaddingElements, const SquareMatrix3* affineTransform, const uint8_t* borderColor, uint8_t* output, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int outputPaddingElements, const unsigned int firstOutputRow, const unsigned int numberOutputRows)
{
	// The following optimizations have been applied:
	//
	// - Matrix-vector multiplication for affine transformations:
	//
	//     | x' |   | X0 Y0 Z0 |   | x |
	//     | y' | = | X1 Y1 Z1 | * | y |
	//     | 1  |   |  0  0  1 |   | 1 |
	//
	//     which is
	//
	//     x' = X0 * x + Y0 * y + Z0
	//     y' = X1 * x + Y1 * y + Z1
	//
	//     We can slightly optimize this operation, since y is constant within the inner
	//     loop. The two terms on the right side in the above equations can be
	//     pre-calculated:
	//
	//     C0 = Y0 * y + Z0
	//     C1 = Y1 * y + Z1
	//
	//     So the computation becomes:
	//
	//     | x' |   | X0 * x |   | C0 |
	//     | y' | = | X1 * x | + | C1 |
	//
	// - For better utilization of cache coherence, the (output) image is processed
	//   in blocks (64 x 64 pixels, if possible)
	//
	// - Integer fixed-point arithmetic.
	//
	// - Update products from floating point numbers with the beginning of blocks,
	//   because the rounding error of fixed-point operations increases for larger
	//   values:
	//
	//     f - float number
	//     i - fixed-point representation of f
	//     v - coordinate value
	//     eps = (f - i) - loss of precision (eps > 0)
	//
	//     Rounding error:
	//
	//       e = |(v * f) - (v * i)| = |v * (f - i)| = |v * eps|
	//       (increases linearly for larger coordinate values v, i.e., with image size)
	//
	//   The rounding error can be kept at bay by replacing the product (v * i) with
	//   (v * f) at the beginning of each block followed by adding an offset for all
	//   other pixels in the block, (N * f) where N is the number of pixels which
	//   are processed concurrently by SIMD instructions.
	//

	static_assert(tChannels >= 1u, "Invalid channel number!");

	constexpr unsigned int fractionalBits = 15u;
	constexpr unsigned int totalBits = (unsigned int)(CHAR_BIT * sizeof(int));

	static_assert((fractionalBits + 1u /* sign bit */) < totalBits, "Number of fractional bits exceeds number of total bits");

	constexpr unsigned int maxImageEdgeLength = 1u << (totalBits - fractionalBits - 1u /* sign bit */);

	// Scale to convert float value, v, to fixed-point value, v_q = int(round(fixedPointScale * v))
	constexpr Scalar fixedPointScale = Scalar(1u << fractionalBits);

	// Number of pixels processed by NEON in each iteration
	constexpr unsigned int pixelsPerIteration = 4u;

	ocean_assert(input && output);
	ocean_assert_and_suppress_unused(inputWidth > 0u && inputHeight > 0u && inputWidth <= maxImageEdgeLength && inputHeight <= maxImageEdgeLength, maxImageEdgeLength);
	ocean_assert_and_suppress_unused(outputWidth >= pixelsPerIteration && outputHeight > 0u && outputWidth <= maxImageEdgeLength && outputHeight <= maxImageEdgeLength, maxImageEdgeLength);
	ocean_assert(affineTransform);
	ocean_assert(!affineTransform->isNull() && Numeric::isEqualEps((*affineTransform)[2]) && Numeric::isEqualEps((*affineTransform)[5]));

	ocean_assert(firstOutputRow + numberOutputRows <= outputHeight);

	typedef typename DataType<uint8_t, tChannels>::Type PixelType;

	const uint8_t zeroColor[tChannels] = {uint8_t(0)};
	const PixelType* const bColor = borderColor ? (PixelType*)borderColor : (PixelType*)zeroColor;

	PixelType* outputPixelData = (PixelType*)(output + firstOutputRow * (outputWidth * tChannels + outputPaddingElements));

	const unsigned int outputRowEnd = firstOutputRow + numberOutputRows;

	// Inidices of the final nearest neighbor pixel, which are used to the interpolation
	unsigned int nearestNeighboursElements[4];

	// Indices (of elements) above this value in the input image are considered as outside of the image (intentionally not counting the last padding elements)
	const unsigned int inputElementsEnd = inputHeight * inputWidth * tChannels + (inputHeight - 1u) * inputPaddingElements;

	// m128_u_inputWidth = [inputWidth, intputWidth, intputWidth, intputWidth], and the same for inputHeight
	const uint32x4_t m128_u_inputWidth = vdupq_n_u32(inputWidth);
	const uint32x4_t m128_u_inputHeight = vdupq_n_u32(inputHeight);

	// m128_u_inputStrideElements = [rowStride, rowStride, rowStride, rowStride], rowStride = inputWidth * tChannels + inputPaddingElements
	const uint32x4_t m128_u_inputStrideElements = vdupq_n_u32(inputWidth * tChannels + inputPaddingElements);

	// m128_u_channels = [tChannels, tChannels, tChannels, tChannels]
	const uint32x4_t m128_u_channels = vdupq_n_u32(tChannels);

	// m128_s_offsets_0123 = [0, 1, 2, 3]
	const int offsets_0123[4] = { 0, 1, 2, 3 };
	const int32x4_t m128_s_offsets_0123 = vld1q_s32(offsets_0123);

	// m128_f_pixelsPerIteration = [4.0f, 4.0f, 4.0f, 4.0f]
	const float32x4_t m128_f_pixelsPerIteration = vdupq_n_f32((float)pixelsPerIteration);

	// Float-based transformation value X0 multiplied with scale for fixed-point
	// numbers. This is used to update the fixed-point products, X0 * x and X1 * x,
	// at the beginning of each block, i.e.
	// m128_f_q_X0 = [v, v, v, v], v = fixedPointScale * X0, and the same for X1
	const float32x4_t m128_f_X0 = vdupq_n_f32(float(fixedPointScale * (*affineTransform)(0, 0)));
	const float32x4_t m128_f_X1 = vdupq_n_f32(float(fixedPointScale * (*affineTransform)(1, 0)));

	// Increment that is added to fixed-point product computed at the beginning of
	// each block, X0 * x and X1 * x, in each iteration inside the block
	const int32x4_t m128_s_q_X0x_increment = vcvtq_s32_f32(vmulq_f32(m128_f_X0, m128_f_pixelsPerIteration));
	const int32x4_t m128_s_q_X1x_increment = vcvtq_s32_f32(vmulq_f32(m128_f_X1, m128_f_pixelsPerIteration));

	// Determine the optimal block size
	constexpr unsigned int blockSize = 64u;
	constexpr unsigned int blockElements = blockSize * blockSize;
	const unsigned int blockWidth = std::min(blockElements / std::min(numberOutputRows, blockSize), outputWidth);
	const unsigned int blockHeight = std::min(blockElements / blockWidth, numberOutputRows);
	ocean_assert(blockWidth > 0u && blockWidth <= outputWidth);
	ocean_assert(blockHeight > 0u && blockHeight <= numberOutputRows);

	// Index of pixel that is the last in a block of #pixelsPerIterations pixels, i.e. number of remaining pixels after
	// this point are less than #pixelsPerIterations. When this pixel index is reached all pointers will be moved left
	// so that we can process one last block of #pixelsPerIterations pixels. That also means that depending on the width
	// of the output image between [1, pixelsPerIterations) pixels will be computed a second time.
	const unsigned int lastMultipleNeonPixelBlockStart = outputWidth - pixelsPerIteration;

	// m128_f_lastMultipleNeonPixelBlockStart = [(float)(lastMultipleNeonPixelBlockStart + 0), (float)(lastMultipleNeonPixelBlockStart + 1), (float)(lastMultipleNeonPixelBlockStart + 2), (float)(lastMultipleNeonPixelBlockStart + 3)]
	const float32x4_t m128_f_lastMultipleNeonPixelBlockStart = vcvtq_f32_s32(vaddq_s32(vdupq_n_s32((int)lastMultipleNeonPixelBlockStart), m128_s_offsets_0123));

	// m128_s_q_X0x_lastMultipleNeonPixelBlockStart = [v0, v1, v2, v3], vi = int(round(fixedPointScale * X0 * (lastMultipleNeonPixelBlockStart + i))), i = 0...3, and similarly for X1
	const int32x4_t m128_s_q_X0x_lastMultipleNeonPixelBlockStart = vcvtq_s32_f32(vmulq_f32(m128_f_X0, m128_f_lastMultipleNeonPixelBlockStart));
	const int32x4_t m128_s_q_X1x_lastMultipleNeonPixelBlockStart = vcvtq_s32_f32(vmulq_f32(m128_f_X1, m128_f_lastMultipleNeonPixelBlockStart));

	for (unsigned int blockYStart = firstOutputRow; blockYStart < outputRowEnd; blockYStart += blockHeight)
	{
		const unsigned int blockYEnd = std::min(blockYStart + blockHeight, outputRowEnd);

		for (unsigned int blockXStart = 0u; blockXStart < outputWidth; blockXStart += blockWidth)
		{
			const unsigned int blockXEnd = std::min(blockXStart + blockWidth, outputWidth);

			for (unsigned int y = blockYStart; y < blockYEnd; ++y)
			{
				outputPixelData = (PixelType*)(output + y * (outputWidth * tChannels + outputPaddingElements) + blockXStart * tChannels);

				// Constant parts, cf. optimization of matrix-vector multiplication above
				// m128_s_C0 = [C0, C0, C0, C0], C0 = int(round(leftShiftFactor * (Y0 * y + Z0))), and similarly for C1
				const int32x4_t m128_s_q_C0 = vdupq_n_s32(Numeric::round32(fixedPointScale * ((*affineTransform)(0, 1) * Scalar(y) + (*affineTransform)(0, 2))));
				const int32x4_t m128_s_q_C1 = vdupq_n_s32(Numeric::round32(fixedPointScale * ((*affineTransform)(1, 1) * Scalar(y) + (*affineTransform)(1, 2))));

				// Update products, X0 * x and X1 * x, from floating point numbers with the
				// beginning of this block, since the rounding error of fixed-point operations
				// increases for larger coordinate values, cf. list of optimizations above.
				//
				// m128_s_x_0123 = [blockXStart + 0, blockXStart + 1, blockXStart + 2, blockXStart + 3]
				const int32x4_t m128_s_x_0123 = vaddq_s32(vdupq_n_s32(int(blockXStart)), m128_s_offsets_0123);

				// m128_f_x_0123 = [(float)(x + 0), (float)(x + 1), (float)(x + 2), (float)(x + 3)]
				const float32x4_t m128_f_x_0123 = vcvtq_f32_s32(m128_s_x_0123);

				// m128_s_q_X0x = [v0, v1, v2, v3], vi = int(round(fixedPointScale * X0 * (x + i))), i = 0...3, and similarly for X1
				int32x4_t m128_s_q_X0x = vcvtq_s32_f32(vmulq_f32(m128_f_X0, m128_f_x_0123));
				int32x4_t m128_s_q_X1x = vcvtq_s32_f32(vmulq_f32(m128_f_X1, m128_f_x_0123));

				for (unsigned int x = blockXStart; x < blockXEnd; x += pixelsPerIteration)
				{
					if (x + pixelsPerIteration > outputWidth)
					{
						ocean_assert(x + pixelsPerIteration > outputWidth);
						ocean_assert(x >= pixelsPerIteration && outputWidth > pixelsPerIteration);
						ocean_assert(lastMultipleNeonPixelBlockStart == (outputWidth - pixelsPerIteration));

						outputPixelData -= (x - lastMultipleNeonPixelBlockStart);

						x = lastMultipleNeonPixelBlockStart;

						m128_s_q_X0x = m128_s_q_X0x_lastMultipleNeonPixelBlockStart;
						m128_s_q_X1x = m128_s_q_X1x_lastMultipleNeonPixelBlockStart;

						// the for loop will stop after this iteration
						ocean_assert(!(x + pixelsPerIteration < outputWidth));
					}

					// Compute pixel location in the input image
					// m128_s_q_inputX = x' = C0 + X0 * x
					// m128_s_q_inputY = y' = C1 + X1 * y
					const int32x4_t m128_s_q_inputX = vaddq_s32(m128_s_q_C0, m128_s_q_X0x);
					const int32x4_t m128_s_q_inputY = vaddq_s32(m128_s_q_C1, m128_s_q_X1x);

					// Convert (signed) fixed-point location to unsigned int, i.e., negative values
					// will be larger than image dimensions (width, height), cf. note below
					//
					// m128_u_inputX = (unsigned int) round(inputX >> N)
					// m128_u_inputY = (unsigned int) round(inputY >> N)
					const uint32x4_t m128_u_inputX = vreinterpretq_u32_s32(vrshrq_n_s32(m128_s_q_inputX, fractionalBits));
					const uint32x4_t m128_u_inputY = vreinterpretq_u32_s32(vrshrq_n_s32(m128_s_q_inputY, fractionalBits));

					// Note: Detection of input position outside the input image
					//
					// If the input point is outside the input image, then set the index
					// of its nearest neighbor to a value that is above the number of
					// available pixels in the image. When writing to the output, a
					// check will make sure to use the background color for those
					// pixels:
					//
					// nearestNeighbour = isOutsideImage ? 0xFFFFFFFF : y * w + x
					// output = nearestNeighbour < inputPixelIndexEnd ? foregroundColor : backgroundColor.
					//
					// This approach keeps the amount of data that has to be transferred
					// between NEON and CPU registers to a minimum.

					// Casting negative signed values to unsigned value results in very large values, e.g., ((unsigned int) -1) > inputWidth.
					// We'll exploit that below to check is pixel coordinates are outside the image.
					// m128_u_isOutsideImage = (x >= inputWidth || y >= inputHeight) ? 0xFFFFFFFF : 0x00000000;
					const uint32x4_t m128_u_isOutsideImage = vorrq_u32(vcgeq_u32(m128_u_inputX, m128_u_inputWidth), vcgeq_u32(m128_u_inputY, m128_u_inputHeight));

					// Determine the pixel indices of the nearest neighbors and store the result
					// If the pixel is outside the image then set the index of the nearest neighbor to the largest possible value
					// m_128_u_nearestNeighbors = m128_u_isOutsideImage | (inputY * inputStrideElements) + (inputX * channels);
					// which is equivalent to
					// m_128_u_nearestNeighborElements = (m128_u_isOutsideImage ? 0xFFFFFFFF : (inputY * inputStrideElements) + (inputX * channels))
					const uint32x4_t m_128_u_nearestNeighborsElements = vorrq_u32(m128_u_isOutsideImage, vaddq_u32(vmulq_u32(m128_u_inputY, m128_u_inputStrideElements), vmulq_u32(m128_u_inputX, m128_u_channels)));
					vst1q_u32(nearestNeighboursElements, m_128_u_nearestNeighborsElements);

					outputPixelData[0] = nearestNeighboursElements[0] < inputElementsEnd ? *(const PixelType*)(input + nearestNeighboursElements[0]) : *bColor;
					outputPixelData[1] = nearestNeighboursElements[1] < inputElementsEnd ? *(const PixelType*)(input + nearestNeighboursElements[1]) : *bColor;
					outputPixelData[2] = nearestNeighboursElements[2] < inputElementsEnd ? *(const PixelType*)(input + nearestNeighboursElements[2]) : *bColor;
					outputPixelData[3] = nearestNeighboursElements[3] < inputElementsEnd ? *(const PixelType*)(input + nearestNeighboursElements[3]) : *bColor;

					outputPixelData += pixelsPerIteration;

					// m128_s_q_X0x += m128_s_q_X0x_increment, and similarly for X1
					m128_s_q_X0x = vaddq_s32(m128_s_q_X0x, m128_s_q_X0x_increment);
					m128_s_q_X1x = vaddq_s32(m128_s_q_X1x, m128_s_q_X1x_increment);
				}
			}
		}

		outputPixelData = (PixelType*)((uint8_t*)outputPixelData + outputPaddingElements);
	}
}

template <typename T, unsigned int tChannels>
void FrameInterpolatorNearestPixel::homographyNEONSubset(const T* input, const unsigned int inputWidth, const unsigned int inputHeight, const SquareMatrix3* input_H_output, const T* borderColor, T* output, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, const unsigned int firstOutputRow, const unsigned int numberOutputRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(input != nullptr && output != nullptr);
	ocean_assert(inputWidth > 0u && inputHeight > 0u);
	ocean_assert(outputWidth >= 4u && outputHeight > 0u);
	ocean_assert(input_H_output != nullptr && !input_H_output->isSingular());

	ocean_assert(firstOutputRow + numberOutputRows <= outputHeight);

	const unsigned int inputStrideElements = inputWidth * tChannels + inputPaddingElements;
	const unsigned int outputStrideElements = outputWidth * tChannels + outputPaddingElements;

	typedef typename DataType<T, tChannels>::Type PixelType;

	const T zeroColor[tChannels] = {T(0)};
	const PixelType bColor = borderColor ? *(const PixelType*)(borderColor) : *(const PixelType*)(zeroColor);

	unsigned int validPixels[4];
	unsigned int nearestNeighbourElementOffsets[4];

	//                | X0 Y0 Z0 |            | x |
	// Homography H = | X1 Y1 Z1 |, point p = | y |
	//                | X2 Y2 Z2 |            | 1 |
	//
	//                  | xx |
	//     pp = H * p = | yy |
	//                  | zz |
	//
	//     | xx |   | X0 Y0 Z0 |   | x |
	// <=> | yy | = | X1 Y1 Z1 | * | y |
	//     | zz |   | X2 Y2 Z2 |   | 1 |
	//
	//     | xx |   | X0 * x |   | Y0 * y + Z0 |
	// <=> | yy | = | X1 * x | + | Y1 * y + Z1 |
	//     | zz |   | X2 * x |   | Y2 * y + Z2 |
	//
	//     | xx |   | X0 * x |   | C0 |      | Y0 * y + Z0 |
	// <=> | yy | = | X1 * x | + | C1 |, C = | Y1 * y + Z1 |
	//     | zz |   | X2 * x |   | C2 |      | Y2 * y + Z2 |
	//
	// Where C is a constant term that can be pre-computed (per image row)
	//
	//      | x' |   | xx / zz |   | (X0 * x + C0) / (X2 * x + C2) |
	// p' = | y' | = | yy / zz | = | (X1 * x + C1) / (X2 * x + C2) |

	// we store 4 floats: [X0, X0, X0, X0], and same with X1 and X2
	const float32x4_t m128_f_X0 = vdupq_n_f32(float((*input_H_output)(0, 0)));
	const float32x4_t m128_f_X1 = vdupq_n_f32(float((*input_H_output)(1, 0)));
	const float32x4_t m128_f_X2 = vdupq_n_f32(float((*input_H_output)(2, 0)));

	// we store 4 floats: [0.5f, 0.5f, 0.5f, 0.5f]
	const float32x4_t m128_f_pointFive = vdupq_n_f32(0.5f);
	const float32x4_t m128_f_negPointFive = vdupq_n_f32(-0.5f);

	// we store 4 integers: [inputStrideElements, inputStrideElements, inputStrideElements, inputStrideElements]
	const uint32x4_t m128_u_inputStrideElements = vdupq_n_u32(inputStrideElements);

	const uint32x4_t m128_u_channels = vdupq_n_u32(tChannels);

	// we store 4 floats: [inputWidth - 0.5f, inputWidth - 0.5f, inputWidth - 0.5f, inputWidth - 0.5f], and same with inputHeight
	const float32x4_t m128_f_inputWidth_pointFive = vdupq_n_f32(float(inputWidth) - 0.5f);
	const float32x4_t m128_f_inputHeight_pointFive = vdupq_n_f32(float(inputHeight) - 0.5f);

	for (unsigned int y = firstOutputRow; y < firstOutputRow + numberOutputRows; ++y)
	{
		PixelType* outputPixelData = (PixelType*)(output + y * outputStrideElements);

		// Pre-compute the constant terms [Ci, Ci, Ci, Ci], i={0, 1, 2}
		const float32x4_t m128_f_C0 = vdupq_n_f32(float((*input_H_output)(0, 1) * Scalar(y) + (*input_H_output)(0, 2)));
		const float32x4_t m128_f_C1 = vdupq_n_f32(float((*input_H_output)(1, 1) * Scalar(y) + (*input_H_output)(1, 2)));
		const float32x4_t m128_f_C2 = vdupq_n_f32(float((*input_H_output)(2, 1) * Scalar(y) + (*input_H_output)(2, 2)));

		for (unsigned int x = 0u; x < outputWidth; x += 4u)
		{
			if (x + 4u > outputWidth)
			{
				// Since the last iteration will not fit into the output frame, we'll shift N pixel left so that it fits again (at most 3 pixels).

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
			float x_0123[4] = { float(x + 0u), float(x + 1u), float(x + 2u), float(x + 3u) };
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

			// Mark pixels inside the input image as valid, all others as invalid
			const uint32x4_t m128_u_validPixelX = vandq_u32(vcltq_f32(m128_f_inputX, m128_f_inputWidth_pointFive), vcgtq_f32(m128_f_inputX, m128_f_negPointFive)); // inputX < (inputWidth - 0.5) && inputX >= -0.5 ? 0xFFFFFFFF : 0x00000000
			const uint32x4_t m128_u_validPixelY = vandq_u32(vcltq_f32(m128_f_inputY, m128_f_inputHeight_pointFive), vcgtq_f32(m128_f_inputY, m128_f_negPointFive)); // inputY < (inputHeight - 0.5) && inputY > -0.5 ? 0xFFFFFFFF : 0x00000000

			const uint32x4_t m128_u_validPixel = vandq_u32(m128_u_validPixelX, m128_u_validPixelY); // is_inside_input_frame(inputPosition) ? 0xFFFFFFFF : 0x00000000

			// Stop here if all pixels are invalid
			const uint32x2_t m64_u_validPixel = vorr_u32(vget_low_u32(m128_u_validPixel), vget_high_u32(m128_u_validPixel));
			if ((vget_lane_u32(m64_u_validPixel, 0) | vget_lane_u32(m64_u_validPixel, 1)) == 0x00000000u)
			{
#ifdef OCEAN_DEBUG
				// clang-format off
				OCEAN_ALIGN_DATA(16) unsigned int debugValidPixels[4];
				// clang-format on
				vst1q_u32(debugValidPixels, m128_u_validPixel);
				ocean_assert(!(debugValidPixels[0] || debugValidPixels[1] || debugValidPixels[2] || debugValidPixels[3]));
#endif

				outputPixelData[0] = bColor;
				outputPixelData[1] = bColor;
				outputPixelData[2] = bColor;
				outputPixelData[3] = bColor;

				outputPixelData += 4;

				continue;
			}

			// Determine the pixel indices of the nearest neighbors and store the result
			vst1q_u32(validPixels, m128_u_validPixel);
			ocean_assert(validPixels[0] || validPixels[1] || validPixels[2] || validPixels[3]);

			const uint32x4_t m128_u_inputX = vcvtq_u32_f32(vaddq_f32(m128_f_inputX, m128_f_pointFive)); // Round to nearest integer: x' = (int) (x + 0.5f)
			const uint32x4_t m128_u_inputY = vcvtq_u32_f32(vaddq_f32(m128_f_inputY, m128_f_pointFive)); // Round to nearest integer: y' = (int) (y + 0.5f)
			const uint32x4_t m_128_u_nearestNeighbourElementOffsets = vmlaq_u32(vmulq_u32(m128_u_inputY, m128_u_inputStrideElements), m128_u_inputX, m128_u_channels); // nn = y' * inputStrideElements + x' * channels
			vst1q_u32(nearestNeighbourElementOffsets, m_128_u_nearestNeighbourElementOffsets);

#ifdef OCEAN_DEBUG
			unsigned int debugInputX[4];
			unsigned int debugInputY[4];
			vst1q_u32(debugInputX, m128_u_inputX);
			vst1q_u32(debugInputY, m128_u_inputY);
			ocean_assert(!validPixels[0] || (debugInputX[0] < inputWidth && debugInputY[0] < inputHeight));
			ocean_assert(!validPixels[1] || (debugInputX[1] < inputWidth && debugInputY[1] < inputHeight));
			ocean_assert(!validPixels[2] || (debugInputX[2] < inputWidth && debugInputY[2] < inputHeight));
			ocean_assert(!validPixels[3] || (debugInputX[3] < inputWidth && debugInputY[3] < inputHeight));
#endif

			outputPixelData[0] = validPixels[0] ? *(const PixelType*)(input + nearestNeighbourElementOffsets[0]) : bColor;
			outputPixelData[1] = validPixels[1] ? *(const PixelType*)(input + nearestNeighbourElementOffsets[1]) : bColor;
			outputPixelData[2] = validPixels[2] ? *(const PixelType*)(input + nearestNeighbourElementOffsets[2]) : bColor;
			outputPixelData[3] = validPixels[3] ? *(const PixelType*)(input + nearestNeighbourElementOffsets[3]) : bColor;

			outputPixelData += 4;
		}
	}
}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

template <unsigned int tChannels>
void FrameInterpolatorNearestPixel::homographyMask8BitPerChannelSubset(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const unsigned int inputPaddingElements, const SquareMatrix3* input_H_output, uint8_t* output, const unsigned int outputPaddingElements, uint8_t* outputMask, const unsigned int outputMaskPaddingElements, const uint8_t maskValue, const int outputOriginX, const int outputOriginY, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int firstOutputRow, const unsigned int numberOutputRows)
{
	static_assert(tChannels > 0u, "Invalid channel number!");

	ocean_assert(input != nullptr && output != nullptr && outputMask != nullptr);
	ocean_assert(inputWidth > 0u && inputHeight > 0u);
	ocean_assert(outputWidth > 0u && outputHeight > 0u);
	ocean_assert(input_H_output != nullptr && !input_H_output->isSingular());

	ocean_assert_and_suppress_unused(firstOutputRow + numberOutputRows <= outputHeight, outputHeight);

	const unsigned int inputStrideElements = inputWidth * tChannels + inputPaddingElements;
	const unsigned int outputStrideElements = outputWidth * tChannels + outputPaddingElements;
	const unsigned int outputMaskStrideElements = outputWidth + outputMaskPaddingElements;

	typedef typename DataType<uint8_t, tChannels>::Type PixelType;

	output += firstOutputRow * outputStrideElements;
	outputMask += firstOutputRow * outputMaskStrideElements;

	for (unsigned int y = firstOutputRow; y < firstOutputRow + numberOutputRows; ++y)
	{
		PixelType* outputPixel = (PixelType*)(output);

		for (unsigned int x = 0; x < outputWidth; ++x)
		{
			const Vector2 outputPosition = Vector2(Scalar(int(x) + outputOriginX), Scalar(int(y) + outputOriginY));
			const Vector2 inputPosition(*input_H_output * outputPosition);

			const unsigned int inputX = Numeric::round32(inputPosition.x());
			const unsigned int inputY = Numeric::round32(inputPosition.y());

			if (inputX < inputWidth && inputY < inputHeight)
			{
				*outputPixel = *((PixelType*)(input + inputY * inputStrideElements + inputX * tChannels));
				*outputMask = maskValue;
			}
			else
			{
				*outputMask = 0xFFu - maskValue;
			}

			++outputPixel;
			++outputMask;
		}

		output += outputStrideElements;
		outputMask += outputMaskPaddingElements;
	}
}

template <unsigned int tChannels>
void FrameInterpolatorNearestPixel::transform8BitPerChannelSubset(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const LookupTable* lookupTable, const bool offset, const uint8_t* borderColor, uint8_t* output, const unsigned int inputPaddingElements, const unsigned int outputPaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannels > 0u, "Invalid channel number!");

	ocean_assert(lookupTable != nullptr);
	ocean_assert(input != nullptr && output != nullptr);

	ocean_assert(inputWidth != 0u && inputHeight != 0u);
	ocean_assert(firstRow + numberRows <= lookupTable->sizeY());

	const unsigned int outputWidth = (unsigned int)(lookupTable->sizeX());

	const unsigned int inputStrideElements = inputWidth * tChannels + inputPaddingElements;
	const unsigned int outputStrideElements = outputWidth * tChannels + outputPaddingElements;

	typedef typename DataType<uint8_t, tChannels>::Type PixelType;

	const uint8_t zeroColor[tChannels] = {uint8_t(0)};
	const PixelType* const bColor = borderColor ? (PixelType*)(borderColor) : (PixelType*)(zeroColor);

	if (offset)
	{
		for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
		{
			PixelType* outputData = (PixelType*)(output + y * outputStrideElements);

			for (unsigned int x = 0u; x < outputWidth; ++x)
			{
				const Vector2 inputOffset(lookupTable->bilinearValue(Scalar(x), Scalar(y)));
				const Vector2 inputPosition(Scalar(x) + inputOffset.x(), Scalar(y) + inputOffset.y());

				const unsigned int inputX = Numeric::round32(inputPosition.x());
				const unsigned int inputY = Numeric::round32(inputPosition.y());

				if (inputX < inputWidth && inputY < inputHeight)
				{
					*outputData = *((const PixelType*)(input + inputY * inputStrideElements) + inputX);
				}
				else
				{
					*outputData = *bColor;
				}

				++outputData;
			}
		}
	}
	else
	{
		for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
		{
			PixelType* outputData = (PixelType*)(output + y * outputStrideElements);

			for (unsigned int x = 0u; x < outputWidth; ++x)
			{
				const Vector2 inputPosition(lookupTable->bilinearValue(Scalar(x), Scalar(y)));

				const unsigned int inputX = Numeric::round32(inputPosition.x());
				const unsigned int inputY = Numeric::round32(inputPosition.y());

				if (inputX < inputWidth && inputY < inputHeight)
				{
					*outputData = *((const PixelType*)(input + inputY * inputStrideElements) + inputX);
				}
				else
				{
					*outputData = *bColor;
				}

				++outputData;
			}
		}
	}
}

template <unsigned int tChannels>
void FrameInterpolatorNearestPixel::transformMask8BitPerChannelSubset(const uint8_t* input, const unsigned int inputWidth, const unsigned int inputHeight, const unsigned int inputPaddingElements, const LookupTable* lookupTable, const bool offset, uint8_t* output, const unsigned int outputPaddingElements, uint8_t* outputMask, const unsigned int outputMaskPaddingElements, const uint8_t maskValue, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannels > 0u, "Invalid channel number!");

	ocean_assert(lookupTable != nullptr);
	ocean_assert(input != nullptr && output != nullptr);

	ocean_assert(inputWidth != 0u && inputHeight != 0u);
	ocean_assert(firstRow + numberRows <= lookupTable->sizeY());

	ocean_assert(NumericT<unsigned int>::isInsideValueRange(lookupTable->sizeX()));
	const unsigned int outputWidth = (unsigned int)(lookupTable->sizeX());

	const unsigned int inputStrideElements = inputWidth * tChannels + inputPaddingElements;
	const unsigned int outputStrideElements = outputWidth * tChannels + outputPaddingElements;
	const unsigned int outputMaskStrideElements = outputWidth + outputMaskPaddingElements;

	typedef typename DataType<uint8_t, tChannels>::Type PixelType;

	output += firstRow * outputStrideElements;
	outputMask += firstRow * outputMaskStrideElements;

	if (offset)
	{
		for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
		{
			PixelType* outputPixel = (PixelType*)(output);

			for (unsigned int x = 0u; x < lookupTable->sizeX(); ++x)
			{
				const Vector2 inputOffset(lookupTable->bilinearValue(Scalar(x), Scalar(y)));
				const Vector2 inputPosition(Scalar(x) + inputOffset.x(), Scalar(y) + inputOffset.y());

				const unsigned int inputX = Numeric::round32(inputPosition.x());
				const unsigned int inputY = Numeric::round32(inputPosition.y());

				if (inputX < inputWidth && inputY < inputHeight)
				{
					*outputPixel = *((PixelType*)(input + inputY * inputStrideElements + inputX * tChannels));
					*outputMask = maskValue;
				}
				else
				{
					*outputMask = 0xFF - maskValue;
				}

				++outputPixel;
				++outputMask;
			}

			output += outputStrideElements;
			outputMask += outputMaskPaddingElements;
		}
	}
	else
	{
		for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
		{
			PixelType* outputPixel = (PixelType*)(output);

			for (unsigned int x = 0u; x < lookupTable->sizeX(); ++x)
			{
				const Vector2 inputPosition(lookupTable->bilinearValue(Scalar(x), Scalar(y)));

				const unsigned int inputX = Numeric::round32(inputPosition.x());
				const unsigned int inputY = Numeric::round32(inputPosition.y());

				if (inputX < inputWidth && inputY < inputHeight)
				{
					*outputPixel = *((PixelType*)(input + inputY * inputStrideElements + inputX * tChannels));
					*outputMask = maskValue;
				}
				else
				{
					*outputMask = 0xFF - maskValue;
				}

				++outputPixel;
				++outputMask;
			}

			output += outputStrideElements;
			outputMask += outputMaskPaddingElements;
		}
	}
}

} // namespace CV

} // namespace Ocean

#endif // META_OCEAN_CV_FRAME_INTERPOLATOR_NEAREST_PIXEL_H
