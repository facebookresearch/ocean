/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_ADVANCED_WHITE_POINT_DETECTOR_H
#define META_OCEAN_CV_ADVANCED_WHITE_POINT_DETECTOR_H

#include "ocean/cv/advanced/Advanced.h"
#include "ocean/cv/Histogram.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

/**
 * Provides functions for white point detection and correction
 * @ingroup cvadvanced
 */
class OCEAN_CV_ADVANCED_EXPORT WhitePointDetector
{
	public:

		/**
		 * Attempts to detect the white point in the specified image frame.
		 * @param frame The frame in which the white point will be detected, must have a RGB24 pixel format, must be valid
		 * @param worker Optional worker instance to distribute the computational load
		 * @return The resulting white point
		 */
		static VectorF3 detectWhitePointHistogram(const Frame& frame, Worker* worker = nullptr);

		/**
		 * Attempts to detect the white point in the specified frame using the iterative gray points algorithm.
		 * @param frame The frame in which the white point will be detected, must have a RGB24 pixel format, must be valid
		 * @param grayThreshold Tolerance for gray point detection, higher tolerance is results in a larger set of gray points, with range [0, 255]
		 * @return The resulting white point
		 */
		static VectorF3 detectWhitePointGrayPoints(const Frame& frame, const float grayThreshold = 0.4f);

		/**
		 * Applies white point balancing on the image frame.
		 * @param frame The frame to be corrected, must have a RGB24 pixel format, must be valid
		 * @param whitePoint White point of the image.
		 * @param worker Optional worker instance to distribute the computational load
		 */
		static void correctWhitePoint(Frame& frame, const VectorF3& whitePoint, Worker* worker = nullptr);

	protected:

		/**
		 * Iterates through the specified color channel histogram from highest to lowest bin to find the first bin that has more votes than the specified threshold.
		 * @param histogram The histogram that is searched
		 * @param channel Specifies the color channel for which its histogram is examined, with range [0, 2]
		 * @param threshold Number of votes threshold
		 * @return Color value that satisfies the number of votes threshold
		 */
		static unsigned char colorRange(const CV::Histogram::Histogram8BitPerChannel<3u>& histogram, const unsigned char channel, const unsigned int threshold);

		/**
		 * Converts from RGB to YUV color space
		 * @param r Red channel value
		 * @param g Green channel value
		 * @param b Blue channel value
		 * @param y Resulting luminance value
		 * @param u Resulting U channel value
		 * @param v Resulting V channel value
		 */
		static inline void rgb2yuv(const float r, const float g, const float b, float& y, float& u, float& v);

		/**
		 * Converts from YUV to RGB color space
		 * @param y Luminance value
		 * @param u U channel value
		 * @param v V channel value
		 * @param r Resulting red channel value
		 * @param g Resulting green channel value
		 * @param b Resulting blue channel value
		 */
		static inline void yuv2rgb(const float y, const float u, const float v, float& r, float& g, float& b);
};

inline void WhitePointDetector::rgb2yuv(const float r, const float g, const float b, float& y, float& u, float& v)
{
	// RGB to ITU-R BT.601 YPbPr
	y =  0.299000f * r + 0.587000f * g + 0.114000f * b;
	u = -0.168736f * r - 0.331264f * g + 0.500000f * b;
	v =  0.500000f * r - 0.418688f * g - 0.081312f * b;
}

inline void WhitePointDetector::yuv2rgb(const float y, const float u, const float v, float& r, float& g, float& b)
{
	// ITU-R BT.601 YPbPr to RGB
	r =  y - 1.21889419e-06f * u + 1.40199959e+00f * v;
	g =  y - 3.44135678e-01f * u - 7.14136156e-01f * v;
	b =  y + 1.77200007e+00f * u + 4.06298063e-07f * v;
}

}

}

}

#endif // META_OCEAN_CV_ADVANCED_WHITE_POINT_DETECTOR_H
