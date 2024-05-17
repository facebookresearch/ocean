/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/advanced/PoissonBlending.h"

#include "ocean/cv/FrameChannels.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

void PoissonBlending::poissonBlending(const Frame& source, const Frame& sourceMask, Frame& target, const int left, const int top, const uint8_t maskValue, Worker* worker)
{
	ocean_assert(source.isValid() && source.numberPlanes() == 1u && source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);
	ocean_assert(sourceMask.frameType() == FrameType(source, FrameType::FORMAT_Y8));
	ocean_assert(target.isValid() && target.pixelFormat() == source.pixelFormat());

	const unsigned int sourceLeft = (unsigned int)(max(0, -left));
	const unsigned int sourceTop = (unsigned int)(max(0, -top));

	const unsigned int sourceRightEnd = (unsigned int)(minmax(0, int(target.width()) - left, int(source.width())));
	const unsigned int sourceBottomEnd = (unsigned int)(minmax(0, int(target.height()) - top, int(source.height())));

	const unsigned int targetLeft = (unsigned int)(int(sourceLeft) + left);
	const unsigned int targetTop = (unsigned int)(int(sourceTop) + top);

	if (sourceLeft >= sourceRightEnd || sourceTop >= sourceBottomEnd)
	{
		return;
	}

	const CV::PixelBoundingBox sourceBoundingBox(sourceLeft, sourceTop, sourceRightEnd - 1u, sourceBottomEnd - 1u);
	ocean_assert(sourceBoundingBox.isValid());

	Frame indexLookup;
	const size_t size = createIndexLookup(sourceMask, sourceBoundingBox, maskValue, indexLookup);

	if (size == 0)
	{
		return;
	}

	SparseMatrixF matrixA;
	createMaskNeighborRelationsMatrix(indexLookup, sourceBoundingBox, targetLeft, targetTop, target.width(), target.height(), matrixA);
	ocean_assert(matrixA.columns() == size && matrixA.rows() == size);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&PoissonBlending::poissonBlendingSubset, (const Frame*)(&indexLookup), &source, &target, (const SparseMatrixF*)(&matrixA), &sourceBoundingBox, targetLeft, targetTop, 0u, 0u), 0u, source.channels());
	}
	else
	{
		poissonBlendingSubset(&indexLookup, &source, &target, &matrixA, &sourceBoundingBox, targetLeft, targetTop, 0u, source.channels());
	}
}

void PoissonBlending::poissonBlendingSubset(const Frame* indexLookup, const Frame* source, Frame* target, const SparseMatrixF* matrixA, const CV::PixelBoundingBox* sourceBoundingBox, const unsigned int targetLeft, const unsigned int targetTop, const unsigned int firstChannel, const unsigned int channelCount)
{
	ocean_assert(indexLookup && indexLookup->isValid());

	ocean_assert(source != nullptr && source->isValid() && source->numberPlanes() == 1u && source->dataType() == FrameType::DT_UNSIGNED_INTEGER_8);
	ocean_assert(target != nullptr && target->isValid() && target->pixelFormat() == source->pixelFormat());

	ocean_assert(matrixA != nullptr);

	ocean_assert(sourceBoundingBox != nullptr);

	const unsigned int endChannel = firstChannel + channelCount;

	for (unsigned int channel = firstChannel; channel < endChannel; channel++)
	{
		MatrixF vectorB(matrixA->columns(), 1);
		createSummedBorderLaplacianVector(*indexLookup, *source, *target, *sourceBoundingBox, targetLeft, targetTop, channel, vectorB);

		MatrixF x;
		bool result = matrixA->solve(vectorB, x); // **TODO**: Use faster solver, Poisson blending paper used two approaches: Gauss-Seidel iteration with successive over relaxation and V-cycle multi grid, also mentioned a GPU-based multi grid implementation
		ocean_assert(result);

		if (result)
		{
			insertResultDataToChannel(*indexLookup, x, *sourceBoundingBox, targetLeft, targetTop, channel, *target);
		}
	}
}

size_t PoissonBlending::createIndexLookup(const Frame& sourceMask, const CV::PixelBoundingBox& sourceBoundingBox, const uint8_t maskValue, Frame& indexLookup)
{
	ocean_assert(sourceMask.pixelFormat() == FrameType::FORMAT_Y8);
	ocean_assert(sourceBoundingBox.isValid() && sourceBoundingBox.right() < sourceMask.width() && sourceBoundingBox.bottom() < sourceMask.height());

	if (!indexLookup.set(FrameType(sourceMask.width() + 2u, sourceMask.height() + 2u, FrameType::FORMAT_Y32, FrameType::ORIGIN_UPPER_LEFT), true, true))
	{
		ocean_assert(false && "This should never happen!");
		return 0;
	}

	indexLookup.setValue(0xFF); // sets all pixels to invalid index

	uint32_t index = 0u;

	for (unsigned int y = sourceBoundingBox.top(); y < sourceBoundingBox.bottomEnd(); ++y)
	{
		uint32_t* indexRow = indexLookup.row<uint32_t>(y + 1u);

		const uint8_t* sourceMaskRow = sourceMask.constrow<uint8_t>(y);

		for (unsigned int x = sourceBoundingBox.left(); x < sourceBoundingBox.rightEnd(); ++x)
		{
			if (sourceMaskRow[x] == maskValue)
			{
				indexRow[x + 1u] = index++;
			}
		}
	}

	return size_t(index);
}

void PoissonBlending::createMaskNeighborRelationsMatrix(const Frame& indexLookup, const CV::PixelBoundingBox& sourceBoundingBox, const unsigned int targetLeft, const unsigned int targetTop, const unsigned int targetWidth, const unsigned int targetHeight, SparseMatrixF& matrixA)
{
	ocean_assert(indexLookup && indexLookup.pixelFormat() == FrameType::FORMAT_Y32);
	ocean_assert(sourceBoundingBox.isValid());
	ocean_assert(sourceBoundingBox.right() + 2u < indexLookup.width() && sourceBoundingBox.bottom() + 2u < indexLookup.height());

	SparseMatrixF::Entries entries;

	size_t matrixSize = 0;

	for (unsigned int y = sourceBoundingBox.top(); y < sourceBoundingBox.bottomEnd(); ++y)
	{
		const unsigned int yTarget = targetTop + y;

		const uint32_t* indexPixel = indexLookup.constpixel<uint32_t>(sourceBoundingBox.left() + 1u, y + 1u);
		const uint32_t* indexPixelTop = indexPixel - indexLookup.strideElements();
		const uint32_t* indexPixelBottom = indexPixel + indexLookup.strideElements();

		for (unsigned int x = sourceBoundingBox.left(); x < sourceBoundingBox.rightEnd(); ++x)
		{
			const unsigned int xTarget = targetLeft + x;

			if (*indexPixel != uint32_t(-1))
			{
				const uint32_t neighbors[4] = {*(indexPixel - 1u), *(indexPixel + 1u), *(indexPixelTop), *(indexPixelBottom)};
				const bool hasBorder[4] = {xTarget > 0u, xTarget < targetWidth - 1u, yTarget > 0u, yTarget < targetHeight - 1u};

				unsigned int count = 0u;

				for (unsigned int i = 0u; i < 4u; i++)
				{
					if (neighbors[i] != uint32_t(-1))
					{
						entries.emplace_back(neighbors[i], *indexPixel, -1.0f);
						++count;
					}
					else if (hasBorder[i])
					{
						++count;
					}
				}

				entries.emplace_back(*indexPixel, *indexPixel, float(count));

				++matrixSize;
			}

			++indexPixel;
			++indexPixelTop;
			++indexPixelBottom;
		}
	}

	matrixA = SparseMatrixF(matrixSize, matrixSize, entries);
}

void PoissonBlending::createSummedBorderLaplacianVector(const Frame& indexLookup, const Frame& source, const Frame& target, const CV::PixelBoundingBox& sourceBoundingBox, const unsigned int targetLeft, const unsigned int targetTop, const unsigned int channelIndex, MatrixF& vectorB)
{
	ocean_assert(indexLookup.isValid() && indexLookup.frameType() == FrameType(source.width() + 2u, source.height() + 2u, FrameType::FORMAT_Y32, indexLookup.pixelOrigin()));

	ocean_assert(source.isValid() && source.numberPlanes() == 1u && source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);
	ocean_assert(target.isValid() && target.numberPlanes() == 1u && target.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	ocean_assert(sourceBoundingBox.isValid() && sourceBoundingBox.right() < source.width() && sourceBoundingBox.bottom() < source.height());

	const unsigned int channels = source.channels();
	ocean_assert(channelIndex < channels);

	ocean_assert(vectorB.rows() > 0u && vectorB.columns() == 1u);

	const unsigned int sourceStrideElements = source.width() * source.channels();
	const unsigned int targetStrideElements = target.strideElements();
	const unsigned int indexLookupStrideElements = indexLookup.strideElements();

	float* bData = vectorB.data();

	for (unsigned int y = 0u; y < sourceBoundingBox.height(); ++y)
	{
		const unsigned int ySource = y + sourceBoundingBox.top();
		const unsigned int yTarget = y + targetTop;

		const uint32_t* indexPixel = indexLookup.constpixel<uint32_t>(sourceBoundingBox.left() + 1u, ySource + 1u);

		const uint8_t* sourcePixel = source.constpixel<uint8_t>(sourceBoundingBox.left(), ySource) + channelIndex;
		const uint8_t* targetPixel = target.constpixel<uint8_t>(targetLeft, yTarget) + channelIndex;

		for (unsigned int x = 0u; x < sourceBoundingBox.width(); ++x)
		{
			const unsigned int xSource = x + sourceBoundingBox.left();
			const unsigned int xTarget = x + targetLeft;

			if (*indexPixel != uint32_t(-1))
			{
				float sourceValue = 0.0f;
				float targetValue = 0.0f;
				unsigned int count = 0u;

				if (xSource > sourceBoundingBox.left())
				{
					sourceValue -= *(sourcePixel - channels);
					++count;
				}

				if (xSource < sourceBoundingBox.right())
				{
					sourceValue -= *(sourcePixel + channels);
					++count;
				}

				if (ySource > sourceBoundingBox.top())
				{
					sourceValue -= *(sourcePixel - sourceStrideElements);
					++count;
				}

				if (ySource < sourceBoundingBox.bottom())
				{
					sourceValue -= *(sourcePixel + sourceStrideElements);
					++count;
				}

				if (*(indexPixel - 1u) == uint32_t(-1) && xTarget > 0u)
				{
					targetValue += *(targetPixel - channels);
				}

				if (*(indexPixel + 1u) == uint32_t(-1) && xTarget < target.width() - 1u)
				{
					targetValue += *(targetPixel + channels);
				}

				if (*(indexPixel - indexLookupStrideElements) == uint32_t(-1) && yTarget > 0u)
				{
					targetValue += *(targetPixel - targetStrideElements);
				}

				if (*(indexPixel + indexLookupStrideElements) == uint32_t(-1) && yTarget < target.height() - 1u)
				{
					targetValue += *(targetPixel + targetStrideElements);
				}

				*bData++ = targetValue + float(*sourcePixel * count) + sourceValue;
			}

			++indexPixel;
			sourcePixel += channels;
			targetPixel += channels;
		}
	}

	ocean_assert(bData == vectorB.data() + vectorB.rows());
}

void PoissonBlending::insertResultDataToChannel(const Frame& indexLookup, const MatrixF& vectorX, const CV::PixelBoundingBox& sourceBoundingBox, const unsigned int targetLeft, const unsigned int targetTop, const unsigned int channelIndex, Frame& target)
{
	ocean_assert(indexLookup.isValid() && indexLookup.pixelFormat() == FrameType::FORMAT_Y32);
	ocean_assert(vectorX.rows() > 0u && vectorX.columns() == 1u);

	ocean_assert(sourceBoundingBox.isValid() && sourceBoundingBox.right() + 2u < indexLookup.width() && sourceBoundingBox.bottom() + 2u < indexLookup.height());

	ocean_assert(target.isValid());

	const unsigned int channels = target.channels();
	ocean_assert(channelIndex < channels);

	const float* xData = vectorX.data();

	for (unsigned int y = 0u; y < sourceBoundingBox.height(); ++y)
	{
		const uint32_t* indexPixel = indexLookup.constpixel<uint32_t>(sourceBoundingBox.left() + 1u, sourceBoundingBox.top() + y + 1u);

		uint8_t* targetPixel = target.pixel<uint8_t>(targetLeft, targetTop + y) + channelIndex;

		for (unsigned int x = 0u; x < sourceBoundingBox.width(); ++x)
		{
			if (*indexPixel != uint32_t(-1))
			{
				*targetPixel = uint8_t(minmax(0.0f, *xData++ + 0.5f, 255.0f));
			}

			++indexPixel;

			targetPixel += channels;
		}
	}

	ocean_assert(xData == vectorX.data() + vectorX.rows());
}

}

}

}
