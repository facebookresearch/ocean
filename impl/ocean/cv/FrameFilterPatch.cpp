/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/FrameFilterPatch.h"
#include "ocean/cv/IntegralImage.h"

namespace Ocean
{

namespace CV
{

void FrameFilterPatch::filter1Channel8Bit(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int patchSize, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width > 0u && height > 0u && patchSize > 0u);

	Frame integralFrame(FrameType(width + 2u * patchSize + 1u, height + 2u * patchSize + 1u, FrameType::FORMAT_Y32, FrameType::ORIGIN_UPPER_LEFT));
	CV::IntegralImage::createBorderedImageMirror<uint8_t, uint32_t, 1u>(source, integralFrame.data<unsigned int>(), width, height, patchSize, sourcePaddingElements, integralFrame.paddingElements());

	ocean_assert(integralFrame.isContinuous());

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameFilterPatch::filter1Channel8BitSubset, integralFrame.constdata<uint32_t>(), target, width, height, patchSize, targetPaddingElements, 0u, 0u), 0u, height, 6u, 7u, 20u);
	}
	else
	{
		filter1Channel8BitSubset(integralFrame.constdata<uint32_t>(), target, width, height, patchSize, targetPaddingElements, 0u, height);
	}
}

void FrameFilterPatch::filter1Channel8BitSubset(const uint32_t* borderedIntegral, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int patchSize, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(borderedIntegral != nullptr && target != nullptr);
	ocean_assert(width > 0 && height > 0 && patchSize >= 1);

	ocean_assert_and_suppress_unused(firstRow + numberRows <= height, height);

	/**
	 *  -----   -----
	 * |  A  | |  B  |
	 * |     | |     |
	 *  -----   -----
	 *        X
	 *  -----   -----
	 * |  C  | |  D  |
	 * |     | |     |
	 *  -----   -----
	 */

	const unsigned int integralWidth = width + 2u * patchSize + 1u;
	const unsigned int filterOffset = 2u * patchSize + 1u;

	const uint32_t* row0 = borderedIntegral + firstRow * integralWidth;
	const uint32_t* row1 = row0 + patchSize * integralWidth;
	const uint32_t* row2 = row1 + integralWidth;
	const uint32_t* row3 = row2 + patchSize * integralWidth;

	target += firstRow * (width + targetPaddingElements);

	const uint32_t* const row0End = row0 + numberRows * integralWidth;

	while (row0 != row0End)
	{
		ocean_assert(row0 < row0End);
		ocean_assert((row0 - borderedIntegral) % integralWidth == 0);

		const uint32_t* const row0EndRow = row0 + width;

		while (row0 != row0EndRow)
		{
			ocean_assert(row0 < row0End);
			ocean_assert(row0 < row0EndRow);

			const uint32_t a = *(row0 + 0) - *(row0 + patchSize) - *(row1 + 0) + *(row1 + patchSize); // topLeft
			const uint32_t b = *(row0 + patchSize + 1u) - *(row0 + filterOffset) - *(row1 + patchSize + 1u) + *(row1 + filterOffset); // topRight

			const uint32_t c = *(row2 + 0) - *(row2 + patchSize) - *(row3 + 0) + *(row3 + patchSize); // bottomLeft
			const uint32_t d = *(row2 + patchSize + 1u) - *(row2 + filterOffset) - *(row3 + patchSize + 1u) + *(row3 + filterOffset); // bottomRight

			const uint32_t area = a + b + c + d;

			const uint32_t a_bcd = abs(int32_t(4u * a) - int32_t(area));
			const uint32_t b_acd = abs(int32_t(4u * b) - int32_t(area));
			const uint32_t c_abd = abs(int32_t(4u * c) - int32_t(area));
			const uint32_t d_abc = abs(int32_t(4u * d) - int32_t(area));
			const uint32_t maxSingle = max(a_bcd, max(b_acd, max(c_abd, d_abc))) / (3u * patchSize * patchSize);
			ocean_assert(maxSingle <= 0xFF);

			const uint32_t a_bc = abs(int32_t(3u * a + d) - int32_t(area));
			const uint32_t a_bd = abs(int32_t(3u * a + c) - int32_t(area));
			const uint32_t a_cd = abs(int32_t(3u * a + b) - int32_t(area));

			const uint32_t b_ac = abs(int32_t(3u * b + d) - int32_t(area));
			const uint32_t b_ad = abs(int32_t(3u * b + c) - int32_t(area));
			const uint32_t b_cd = abs(int32_t(3u * b + a) - int32_t(area));

			const uint32_t c_ab = abs(int32_t(3u * c + d) - int32_t(area));
			const uint32_t c_ad = abs(int32_t(3u * c + b) - int32_t(area));
			const uint32_t c_bd = abs(int32_t(3u * c + a) - int32_t(area));

			const uint32_t d_ab = abs(int32_t(3u * d + c) - int32_t(area));
			const uint32_t d_ac = abs(int32_t(3u * d + b) - int32_t(area));
			const uint32_t d_bc = abs(int32_t(3u * d + a) - int32_t(area));

			const uint32_t maxDouble = max(a_bc, max(a_bd, max(a_cd, max(b_ac, max(b_ad, max(b_cd, max(c_ab, max(c_ad, max(c_bd, max(d_ab, max(d_ac, d_bc))))))))))) / (2u * patchSize * patchSize);

			ocean_assert(maxDouble <= 0xFF);
			*target = uint8_t(max(maxSingle, maxDouble));

			++row0;
			++row1;
			++row2;
			++row3;
			++target;
		}

		row0 += 2u * patchSize + 1u;
		row1 += 2u * patchSize + 1u;
		row2 += 2u * patchSize + 1u;
		row3 += 2u * patchSize + 1u;

		target += targetPaddingElements;
	}
}

}

}
