/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_ADVANCED_BLACK_POINT_DETECTOR_H
#define META_OCEAN_CV_ADVANCED_BLACK_POINT_DETECTOR_H

#include "ocean/cv/advanced/Advanced.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/Histogram.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

/**
 * Provides functions for black point detection in a frame.
 * @ingroup cvadvanced
 */
class OCEAN_CV_ADVANCED_EXPORT BlackPointDetector
{
	public:

		/**
		 * Definition of a vector holding 32 bit floating point values.
		 */
		typedef std::vector<float> Color;

	public:

		/**
		 * Attempts to detect the black point in the given frame with three channels.
		 * @param frame The frame in which the black-point will be detected, must be valid
		 * @param worker Optional worker object to distribute the computation
		 * @return The resulting color of the black points, one for each channel
		 * @return True, if the detection was successful
		 */
		static Color detectBlackPointHistogram(const Frame& frame, Worker* worker = nullptr);

		/**
		 * Attempts to detect the black point in the given frame with three channels.
		 * @param frame The frame in which the black-point will be detected, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return The resulting color of the black points, one for each channel
		 * @return True, if the detection was successful
		 */
		template <unsigned int tChannels>
		static Color detectBlackPointHistogram8BitPerChannel(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, Worker* worker = nullptr);
};

template <unsigned int tChannels>
BlackPointDetector::Color BlackPointDetector::detectBlackPointHistogram8BitPerChannel(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, Worker* worker)
{
	static_assert(tChannels != 0u, "Invalid channel number!");
	ocean_assert(frame && width != 0u && height != 0u);

	Color blackPointColor(tChannels);

	const CV::Histogram::Histogram8BitPerChannel<tChannels> histogram(CV::Histogram::determineHistogram8BitPerChannel<tChannels>(frame, width, height, framePaddingElements, worker));

	const unsigned int threshold = max(1u, (width * height + 1000u) / 2000u); // 0.05 % of number of pixels

	constexpr float inv255 = 1.0f / 255.0f;

	for (unsigned int c = 0u; c < tChannels; c++)
	{
		const unsigned int* bins = histogram.bins(c);

		// we iterates through the histogram from (channel-wise) lowest to highest bin to find the first bin that has more votes than the specified threshold

		for (unsigned int i = 0u; i < 256u; ++i)
		{
			if (bins[i] >= threshold)
			{
				blackPointColor[c] = float(i) * inv255;
				break;
			}
		}
	}

	return blackPointColor;
}

}

}

}

#endif // META_OCEAN_CV_ADVANCED_BLACK_POINT_DETECTOR_H
