/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/advanced/AdvancedFrameFilterSobel.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

void AdvancedFrameFilterSobel::filterHorizontalVerticalMaximum8Bit(const unsigned char* source, unsigned short* target, const unsigned int width, const unsigned int height, const unsigned int depth, Worker* worker)
{
	ocean_assert(source && target);

	if (worker)
		worker->executeFunction(Worker::Function::createStatic(&AdvancedFrameFilterSobel::filterHorizontalVerticalMaximum8Bit, source, target, width, height, depth, 0u, 0u), 0u, depth, 5u, 6u);
	else
		filterHorizontalVerticalMaximum8Bit(source, target, width, height, depth, 0u, depth);
}

void AdvancedFrameFilterSobel::filterHorizontalVerticalMaximum8Bit(const unsigned char* source, unsigned short* target, const unsigned int width, const unsigned int height, const unsigned int depth, const unsigned int firstFrame, const unsigned int numberFrames)
{
	ocean_assert(source && target);
	ocean_assert(firstFrame + numberFrames <= depth);

	const unsigned int beginFilterFrame = max(0, int(firstFrame) - 1) + 1; // inclusive filter position
	const unsigned int endFilterFrame = min(firstFrame + numberFrames + 1, depth) - 1; // exclusive filter position

	const unsigned int frameSize = width * height;

	// set the first frame to zero
	if (beginFilterFrame == 1u)
		memset(target, 0, frameSize * sizeof(unsigned short));

	ocean_assert(beginFilterFrame >= 1u);

	target += beginFilterFrame * frameSize;

	const unsigned char* source1 = source + beginFilterFrame * frameSize + width + 1u;
	const unsigned char* source0 = source1 - width;
	const unsigned char* source2 = source1 + width;

	const unsigned char* const source0End = source0 + frameSize * (endFilterFrame - beginFilterFrame);
	while (source0 != source0End)
	{
		ocean_assert(source0 < source0End);
		ocean_assert((source0 - source) % width == 1);
		ocean_assert((source0 - source) % frameSize == 1);

		// set the first row to zero
		memset(target, 0, width * sizeof(unsigned short));
		target += width;

		const unsigned char* const source0FrameEnd = source0 + frameSize - width * 2u;

		while (source0 != source0FrameEnd)
		{
			ocean_assert(source0 < source0End);
			ocean_assert(source0 < source0FrameEnd);

			// set the first pixel to zero
			*target = 0;
			++target;

			const unsigned char* const source0RowEnd = source0 + width - 2u;

			while (source0 != source0RowEnd)
			{
				ocean_assert(source0 < source0End);
				ocean_assert(source0 < source0FrameEnd);
				ocean_assert(source0 < source0RowEnd);

				// xy
				// | -1 0 1 |
				// | -2 0 2 |
				// | -1 0 1 |
				unsigned int result = max(abs(*(source0 + 1) - *(source0 - 1) + (*(source1 + 1) - *(source1 - 1)) * 2 + *(source2 + 1) - *(source2 - 1)),

				// | -1 -2 -1 |
				// |  0  0  0 |
				// |  1  2  1 |
							abs(*(source2 - 1) + *(source2) * 2 + *(source2 + 1) - *(source0 - 1) - *(source0) * 2 - *(source0 + 1)));



				// xz
				const unsigned char* const xz_s0 = source1 - frameSize;
				const unsigned char* const xz_s2 = source1 + frameSize;

				//   / -1 0 1 /
				//  / -2 0 2 /
				// / -1 0 1 /
				result = max(result, (unsigned int)max(abs(*(xz_s0 + 1) - *(xz_s0 - 1) + (*(source1 + 1) - *(source1 - 1)) * 2 + *(xz_s2 + 1) - *(xz_s2 - 1)),

				//   / -1 -2 -1 /
				//  /  0  0  0 /
				// /  1  2  1 /
							abs(*(xz_s2 - 1) + *(xz_s2) * 2 + *(xz_s2 + 1) - *(xz_s0 - 1) - *(xz_s0) * 2 - *(xz_s0 + 1))));



				// yz
				const unsigned char* const yz_s0 = source1 - width;
				const unsigned char* const yz_s2 = source1 + width;

				//     / -1 |
				//   / 0 -2 |
				// | 1 0 -1 |
				// | 2 0  /
				// | 1  /
				*target = (unsigned short)(max(result, (unsigned int)max(abs(*(yz_s0 + frameSize) - *(yz_s0 - frameSize) + (*(source1 + frameSize) - *(source1 - frameSize)) * 2 + *(yz_s2 + frameSize) - *(yz_s2 - frameSize)),

				//       / -1 |
				//    / -2  0 |
				// | -1  0  1 |
				// |  0  2  /
				// |  1  /
							abs(*(yz_s2 - frameSize) + *(yz_s2) * 2 + *(yz_s2 + frameSize) - *(yz_s0 - frameSize) - *(yz_s0) * 2 - *(yz_s0 + frameSize)))));

				++target;
				++source0;
				++source1;
				++source2;
			}

			// set the last pixel to zero
			*target = 0;
			++target;

			source0 += 2;
			source1 += 2;
			source2 += 2;
		}

		// set the last row to zero
		memset(target, 0, width * sizeof(unsigned short));
		target += width;

		source0 += width * 2u;
		source1 += width * 2u;
		source2 += width * 2u;
	}

	// set the last frame to zero
	if (endFilterFrame == depth - 1u)
		memset(target, 0, frameSize * sizeof(unsigned short));
}

}

}

}
