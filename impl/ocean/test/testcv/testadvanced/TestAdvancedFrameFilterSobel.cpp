/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testadvanced/TestAdvancedFrameFilterSobel.h"

#include "ocean/cv/advanced/AdvancedFrameFilterSobel.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestAdvanced
{

bool TestAdvancedFrameFilterSobel::test(const unsigned int width, const unsigned int height, const unsigned int depth, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 3u && height > 3u && depth > 3u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   3D sobel filter test for: " << width << "x" << height << "x" << depth << ":   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	std::vector<unsigned char> buffer(size_t(width) * size_t(height) * size_t(depth));
	unsigned char* const frame = buffer.data();

	const size_t size = size_t(width) * size_t(height) * size_t(depth);
	for (size_t n = 0; n < size; ++n)
	{
		frame[n] = (unsigned char)(RandomI::random(255u));
	}

	allSucceeded = testSinglePixel(frame, width, height, depth, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testFilterHorizontalVerticalMaximum8Bit(frame, width, height, depth, testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "3D sobel filter test: succeeded.";
	}
	else
	{
		Log::info() << "3D sobel filter test: FAILED!";
	}

	return allSucceeded;
}

bool TestAdvancedFrameFilterSobel::testSinglePixel(const unsigned char* frame, const unsigned int width, const unsigned int height, const unsigned int depth, const double testDuration)
{
	ocean_assert(frame && width >= 3u && height > 3u && depth > 3u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "3D sobel filter single pixel test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const unsigned int x = RandomI::random(width - 1u);
			const unsigned int y = RandomI::random(height - 1u);
			const unsigned int z = RandomI::random(depth - 1u);

			if (sobel(frame, width, height, depth, x, y, z) != CV::Advanced::AdvancedFrameFilterSobel::filterHorizontalVerticalMaximum8BitPixel(frame, width, height, depth, x, y, z))
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestAdvancedFrameFilterSobel::testFilterHorizontalVerticalMaximum8Bit(const unsigned char* frame, const unsigned int width, const unsigned int height, const unsigned int depth, const double testDuration, Worker& worker)
{
	ocean_assert(frame && width >= 3u && height > 3u && depth > 3u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing 8 bit maximum 3D horizontal and vertical maximum filter test:";

	std::vector<unsigned short> buffer(size_t(width) * size_t(height) * size_t(depth));
	unsigned short* const sobel = (unsigned short*)buffer.data();

	bool result = true;

	HighPerformanceStatistic performance;
	const Timestamp startTimestamp(true);

	do
	{
		performance.start();
		CV::Advanced::AdvancedFrameFilterSobel::filterHorizontalVerticalMaximum8Bit(frame, sobel, width, height, depth);
		performance.stop();
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance: Best: " << performance.bestMseconds() << "ms, worst: " << performance.worstMseconds() << "ms, average: " << performance.averageMseconds() << "ms";

	if (validationHorizontalVerticalMaximum8Bit(frame, sobel, width, height, depth))
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
		result = false;
	}

	if (worker)
	{
		Log::info() << "Testing 8 bit maximum 3D horizontal and vertical maximum filter test (multicore):";

		HighPerformanceStatistic performanceMulticore;
		const Timestamp startTimestampMulticore(true);

		do
		{
			performanceMulticore.start();
			CV::Advanced::AdvancedFrameFilterSobel::filterHorizontalVerticalMaximum8Bit(frame, sobel, width, height, depth, &worker);
			performanceMulticore.stop();
		}
		while (startTimestampMulticore + testDuration > Timestamp(true));

		Log::info() << "Multicore performance: Best: " << performanceMulticore.bestMseconds() << "ms, worst: " << performanceMulticore.worstMseconds() << "ms, average: " << performanceMulticore.averageMseconds() << "ms";

		if (validationHorizontalVerticalMaximum8Bit(frame, sobel, width, height, depth))
		{
			Log::info() << "Validation: succeeded.";
		}
		else
		{
			Log::info() << "Validation: FAILED!";
			result = false;
		}

		Log::info() << "Multicore boost: Best: " << performance.best() / performanceMulticore.best() << ", worst: " << performance.worst() / performanceMulticore.worst() << ", average: " << performance.average() / performanceMulticore.average();
	}

	return result;
}

unsigned short TestAdvancedFrameFilterSobel::sobel(const unsigned char* frame, const unsigned int width, const unsigned int height, const unsigned int depth, const unsigned int x, const unsigned int y, const unsigned int z)
{
	ocean_assert(frame);

	if (x == 0u || y == 0u || z == 0u || x >= width - 1u || y >= height - 1u || z >= depth - 1u)
	{
		return 0u;
	}

	// xy-axis

	// | -1 0 1 |
	// | -2 0 2 |
	// | -1 0 1 |
	const int xy0 = pixel(frame, width, height, depth, x - 1u, y - 1u, z) * -1
						+ pixel(frame, width, height, depth, x - 1u, y + 0u, z) * -2
						+ pixel(frame, width, height, depth, x - 1u, y + 1u, z) * -1
						+ pixel(frame, width, height, depth, x + 1u, y - 1u, z) * 1
						+ pixel(frame, width, height, depth, x + 1u, y + 0u, z) * 2
						+ pixel(frame, width, height, depth, x + 1u, y + 1u, z) * 1;

	// | -1 -2 -1 |
	// |  0  0  0 |
	// |  1  2  1 |
	const int xy1 = pixel(frame, width, height, depth, x - 1u, y - 1u, z) * -1
						+ pixel(frame, width, height, depth, x + 0u, y - 1u, z) * -2
						+ pixel(frame, width, height, depth, x + 1u, y - 1u, z) * -1
						+ pixel(frame, width, height, depth, x - 1u, y + 1u, z) * 1
						+ pixel(frame, width, height, depth, x + 0u, y + 1u, z) * 2
						+ pixel(frame, width, height, depth, x + 1u, y + 1u, z) * 1;


	// xz-axis

	//   / -1  0  1 /
	//  / -2  0  2 /
	// / -1  0  1 /
	const int xz0 = pixel(frame, width, height, depth, x - 1u, y, z - 1u) * -1
						+ pixel(frame, width, height, depth, x - 1u, y, z + 0u) * -2
						+ pixel(frame, width, height, depth, x - 1u, y, z + 1u) * -1
						+ pixel(frame, width, height, depth, x + 1u, y, z - 1u) * 1
						+ pixel(frame, width, height, depth, x + 1u, y, z + 0u) * 2
						+ pixel(frame, width, height, depth, x + 1u, y, z + 1u) * 1;

	//   / -1 -2 -1 /
	//  /  0  0  0 /
	// /  1  2  1 /
	const int xz1 = pixel(frame, width, height, depth, x - 1u, y, z - 1u) * -1
						+ pixel(frame, width, height, depth, x + 0u, y, z - 1u) * -2
						+ pixel(frame, width, height, depth, x + 1u, y, z - 1u) * -1
						+ pixel(frame, width, height, depth, x - 1u, y, z + 1u) * 1
						+ pixel(frame, width, height, depth, x + 0u, y, z + 1u) * 2
						+ pixel(frame, width, height, depth, x + 1u, y, z + 1u) * 1;



	// yz-axis

	//     / -1 |
	//   / 0 -2 |
	// | 1 0 -1 |
	// | 2 0  /
	// | 1 /
	const int yz0 = pixel(frame, width, height, depth, x, y - 1u, z - 1u) * -1
						+ pixel(frame, width, height, depth, x, y + 0u, z - 1u) * -2
						+ pixel(frame, width, height, depth, x, y + 1u, z - 1u) * -1
						+ pixel(frame, width, height, depth, x, y - 1u, z + 1u) * 1
						+ pixel(frame, width, height, depth, x, y + 0u, z + 1u) * 2
						+ pixel(frame, width, height, depth, x, y + 1u, z + 1u) * 1;

	//       / -1 |
	//    / -2  0 |
	// | -1  0  1 |
	// |  0  2  /
	// |  1  /
	const int yz1 = pixel(frame, width, height, depth, x, y - 1u, z - 1u) * -1
						+ pixel(frame, width, height, depth, x, y - 1u, z + 0u) * -2
						+ pixel(frame, width, height, depth, x, y - 1u, z + 1u) * -1
						+ pixel(frame, width, height, depth, x, y + 1u, z - 1u) * 1
						+ pixel(frame, width, height, depth, x, y + 1u, z + 0u) * 2
						+ pixel(frame, width, height, depth, x, y + 1u, z + 1u) * 1;

	const unsigned int xy = max(abs(xy0), abs(xy1));
	const unsigned int xz = max(abs(xz0), abs(xz1));
	const unsigned int yz = max(abs(yz0), abs(yz1));

	return (unsigned short)(max(xy, max(xz, yz)));
}

bool TestAdvancedFrameFilterSobel::validationHorizontalVerticalMaximum8Bit(const unsigned char* original, const unsigned short* filtered, const unsigned int width, const unsigned int height, const unsigned int depth)
{
	ocean_assert(original && filtered);

	const unsigned int frameSize = width * height;

	for (unsigned int z = 0u; z < depth; ++z)
	{
		for (unsigned int y = 0u; y < height; ++y)
		{
			for (unsigned int x = 0u; x < width; ++x)
			{
				const unsigned short sobelValue = sobel(original, width, height, depth, x, y, z);
				if (sobelValue != filtered[size_t(z) * size_t(frameSize) + size_t(y) * size_t(width) + size_t(x)])
				{
					return false;
				}
			}
		}
	}

	return true;
}

}

}

}

}
