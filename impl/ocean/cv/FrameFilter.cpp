/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/FrameFilter.h"

#include "ocean/math/Approximation.h"

namespace Ocean
{

namespace CV
{

void FrameFilter::magnitude2Channels8BitPerChannelSubset(const uint8_t* frame, uint16_t* magnitude, const unsigned int channels, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int magnitudePaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(frame != nullptr);
	ocean_assert(magnitude != nullptr);
	ocean_assert(channels >= 2u);
	ocean_assert(width >= 1u && height >= 1u);

	ocean_assert_and_suppress_unused(firstRow + numberRows <= height, height);

	const unsigned int frameStrideElements = width * channels + framePaddingElements;
	const unsigned int magnitudeStrideElements = width + magnitudePaddingElements;

	frame += firstRow * frameStrideElements;
	magnitude += firstRow * magnitudeStrideElements;

	for (unsigned int y = 0u; y < numberRows; ++y)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			uint32_t sqrMagnitude = 0u;

			for (unsigned int n = 0u; n < channels; ++n)
			{
				sqrMagnitude += uint32_t(frame[n]) * uint32_t(frame[n]);
			}

			*magnitude = Approximation::sqrt(sqrMagnitude);

			frame += channels;
			++magnitude;
		}

		frame += framePaddingElements;
		magnitude += magnitudePaddingElements;
	}
}

}

}
