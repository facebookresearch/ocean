/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_ADVANCED_ADVANCED_FRAME_FILTER_SOBEL_H
#define META_OCEAN_CV_ADVANCED_ADVANCED_FRAME_FILTER_SOBEL_H

#include "ocean/cv/advanced/Advanced.h"

#include "ocean/base/Worker.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

/**
 * This class implements an advanced sobel frame filter allowing to filter 3D image content.
 * @ingroup cvadvanced
 */
class OCEAN_CV_ADVANCED_EXPORT AdvancedFrameFilterSobel
{
	public:

		/**
		 * Maximum of the absolute of horizontal and vertical sobel filter for an 8 bit gray scale image.
		 * The border pixels are set to zero.<br>
		 * The filter response holds one 16 bit value per pixel holding the maximum of the absolute filter response.<br>
		 * @param source The source frame to filter
		 * @param target The target frame receiving the filter response
		 * @param width The width of the frame in pixel
		 * @param height The height of the frame in pixel
		 * @param depth Depth of the frame in pixel
		 * @param worker Optional worker object to distribute the computational load
		 */
		static void filterHorizontalVerticalMaximum8Bit(const unsigned char* source, unsigned short* target, const unsigned int width, const unsigned int height, const unsigned int depth, Worker* worker = nullptr);

		/**
		 * Maximum of the absolute of horizontal and vertical sobel filter for a given pixel in an 8 bit gray scale image.
		 * For border pixels zero will be returned.<br>
		 * The filter response holds one 16 bit value per pixel holding the maximum of the absolute filter response.<br>
		 * @param source The source frame to filter
		 * @param width The width of the content in pixel
		 * @param height The height of the content in pixel
		 * @param depth Depth of the content in pixel
		 * @param x Horizontal filter position in pixel
		 * @param y Vertical filter position in pixel
		 * @param z Depth filter position in pixel
		 * @return Resulting filter response for the defined pixel
		 */
		static inline unsigned short filterHorizontalVerticalMaximum8BitPixel(const unsigned char* source, const unsigned int width, const unsigned int height, const unsigned int depth, const unsigned int x, const unsigned int y, const unsigned int z);

	private:

		/**
		 * Maximum of the absolute of horizontal and vertical sobel filter for an 8 bit gray scale image.
		 * The border pixels are set to zero.<br>
		 * The filter response holds one 16 bit value per pixel holding the maximum of the absolute filter response.<br>
		 * @param source The source frame to filter
		 * @param target The target frame receiving the filter response
		 * @param width The width of the frame in pixel
		 * @param height The height of the frame in pixel
		 * @param depth Depth of the frame in pixel
		 * @param firstFrame First frame to be handled
		 * @param numberFrames Number of frame to be handled
		 */
		static void filterHorizontalVerticalMaximum8Bit(const unsigned char* source, unsigned short* target, const unsigned int width, const unsigned int height, const unsigned int depth, const unsigned int firstFrame, const unsigned int numberFrames);

};

inline unsigned short AdvancedFrameFilterSobel::filterHorizontalVerticalMaximum8BitPixel(const unsigned char* source, const unsigned int width, const unsigned int height, const unsigned int depth, const unsigned int x, const unsigned int y, const unsigned int z)
{
	ocean_assert(source);

	if (x - 1u < width - 2u && y - 1u < height - 2u && z - 1u < depth - 2u)
	{
		const unsigned int frameSize = width * height;

		const unsigned char* const source1 = source + z * frameSize + y * width + x;

		// xy-axis
		const unsigned char* source0 = source1 - width;
		const unsigned char* source2 = source1 + width;

		// | -1 0 1 |
		// | -2 0 2 |
		// | -1 0 1 |
		unsigned int result = max(abs(*(source0 + 1) - *(source0 - 1) + (*(source1 + 1) - *(source1 - 1)) * 2 + *(source2 + 1) - *(source2 - 1)),

		// | -1 -2 -1 |
		// |  0  0  0 |
		// |  1  2  1 |
					abs(*(source2 - 1) + *(source2) * 2 + *(source2 + 1) - *(source0 - 1) - *(source0) * 2 - *(source0 + 1)));



		// xz-axis
		source0 = source1 - frameSize;
		source2 = source1 + frameSize;

		//   / -1 0 1 /
		//  / -2 0 2 /
		// / -1 0 1 /
		result = max(result, (unsigned int)max(abs(*(source0 + 1) - *(source0 - 1) + (*(source1 + 1) - *(source1 - 1)) * 2 + *(source2 + 1) - *(source2 - 1)),

		//   / -1 -2 -1 /
		//  /  0  0  0 /
		// /  1  2  1 /
					abs(*(source2 - 1) + *(source2) * 2 + *(source2 + 1) - *(source0 - 1) - *(source0) * 2 - *(source0 + 1))));



		// yz-axis
		source0 = source1 - width;
		source2 = source1 + width;


		//     / -1 |
		//   / 0 -2 |
		// | 1 0 -1 |
		// | 2 0  /
		// | 1  /
		result = max(result, (unsigned int)max(abs(*(source0 + frameSize) - *(source0 - frameSize) + (*(source1 + frameSize) - *(source1 - frameSize)) * 2 + *(source2 + frameSize) - *(source2 - frameSize)),

		//       / -1 |
		//    / -2  0 |
		// | -1  0  1 |
		// |  0  2  /
		// |  1  /
					abs(*(source2 - frameSize) + *(source2) * 2 + *(source2 + frameSize) - *(source0 - frameSize) - *(source0) * 2 - *(source0 + frameSize))));

		ocean_assert(result <= (unsigned int)(NumericT<unsigned short>::maxValue()));
		return (unsigned short)(result);
	}

	return 0u;
}

}

}

}

#endif // META_OCEAN_CV_ADVANCED_ADVANCED_FRAME_FILTER_SOBEL_H
