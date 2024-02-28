// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/cv/synthesis/ImageCompletionLayer_patchmask.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Messenger.h"
#include "ocean/base/String.h"

#include "ocean/math/Random.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

const Scalar ImageCompletionLayer_patchmask::searchAreaFactors[12] =
{
	Scalar(1),            // 1 : 1
	Scalar(0.5),          // 1 : 2
	Scalar(0.25),         // 1 : 4
	Scalar(0.125),        // 1 : 8
	Scalar(0.0625),       // 1 : 16
	Scalar(0.03125),      // 1 : 32
	Scalar(0.015625),     // 1 : 64
	Scalar(0.0078125),    // 1 : 128
	Scalar(0.00390625),   // 1 : 256
	Scalar(0.001953125),  // 1 : 512
	Scalar(0.0009765625), // 1 : 1024
	Scalar(0.00048828125) // 1 : 2048
};

ImageCompletionLayer_patchmask::ImageCompletionLayer_patchmask() :
	width_(0),
	height_(0),
	layerPatches(nullptr)
{
	// nothing to do here
}

ImageCompletionLayer_patchmask::ImageCompletionLayer_patchmask(const ImageCompletionLayer_patchmask& layer) :
	width_(layer.width()),
	height_(layer.height()),
	layerPatches(nullptr)
{
	if (width_ != 0 && height_ != 0)
	{
		layerPatches = new Patch[pixels()];
		ocean_assert(layerPatches != nullptr);

		memcpy(layerPatches, layer.layerPatches, sizeof(Patch) * pixels());
	}
}

ImageCompletionLayer_patchmask::ImageCompletionLayer_patchmask(const unsigned int width, const unsigned int height) :
	width_(width),
	height_(height),
	layerPatches(nullptr)
{
	ocean_assert(width_ != 0 && height_ != 0);

	layerPatches = new Patch[pixels()];
	ocean_assert(layerPatches != nullptr);
}

ImageCompletionLayer_patchmask::ImageCompletionLayer_patchmask(const unsigned int width, const unsigned int height, const uint8_t* mask, const unsigned int patchSize, const ImageCompletionLayer_patchmask& layer) :
	width_(width),
	height_(height),
	layerPatches(nullptr)
{
	ocean_assert(width_ != 0 && height_ != 0);
	ocean_assert(layer.width_ != 0 && layer.height_ != 0);
	ocean_assert(mask);

	const unsigned int pixels = width_ * height_;

	layerPatches = new Patch[pixels];
	ocean_assert(layerPatches != nullptr);

	// **TODO** **HACK**
	Log::warning() << "**HACK**";
	//ocean_assert((layerWidth % layer.layerWidth) == 0);
	//ocean_assert((layerHeight % layer.layerHeight) == 0);

	adopt(layer, mask, patchSize);
}

ImageCompletionLayer_patchmask::ImageCompletionLayer_patchmask(const unsigned int width, const unsigned int height, const uint8_t* mask, const unsigned int patchSize, const ImageCompletionLayer_patchmask& layer, Worker& worker) :
	width_(width),
	height_(height),
	layerPatches(nullptr)
{
	ocean_assert(width_ != 0 && height_ != 0);
	ocean_assert(layer.width_ != 0 && layer.height_ != 0);
	ocean_assert(mask);

	const unsigned int pixels = width_ * height_;

	layerPatches = new Patch[pixels];
	ocean_assert(layerPatches != nullptr);

	ocean_assert((width_ % layer.width_) == 0);
	ocean_assert((height_ % layer.height_) == 0);

	adopt(layer, mask, patchSize, worker);
}

ImageCompletionLayer_patchmask::ImageCompletionLayer_patchmask(const unsigned int width, const unsigned int height, const uint8_t* mask, const unsigned int patchSize, const ImageCompletionLayer_patchmask& layer, Worker& worker, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows) :
	width_(width),
	height_(height),
	layerPatches(nullptr)
{
	ocean_assert(width_ != 0 && height_ != 0);
	ocean_assert(layer.width_ != 0 && layer.height_ != 0);
	ocean_assert(mask);

	const unsigned int pixels = width_ * height_;
	layerPatches = new Patch[pixels];
	ocean_assert(layerPatches != nullptr);

	ocean_assert((width_ % layer.width_) == 0);
	ocean_assert((height_ % layer.height_) == 0);

	ocean_assert(firstColumn + numberColumns <= width_);
	ocean_assert(firstRow + numberRows <= height_);

	adopt(layer, mask, patchSize, worker, firstColumn, numberColumns, firstRow, numberRows);
}

ImageCompletionLayer_patchmask::~ImageCompletionLayer_patchmask()
{
	delete [] layerPatches;
}

bool ImageCompletionLayer_patchmask::initializeNull(const uint8_t* mask, const unsigned int patchSize)
{
	ocean_assert(layerPatches);

	if (mask == nullptr || patchSize == 0 || (patchSize % 2) != 1)
		return false;

	const unsigned int patchHalf = patchSize >> 1;

	const unsigned int maxFrameX = width_ - patchHalf - 1;
	const unsigned int maxFrameY = height_ - patchHalf - 1;

	// random initialization
	for (unsigned int y = patchHalf; y <= maxFrameY; ++y)
		for (unsigned int x = patchHalf; x <= maxFrameX; ++x)
			if (mask[y * width_ + x] == 0)
				layerPatches[y * width_ + x] = Patch(x, y, 0xFFFFFFF0, 0xFFFFFFF0);

	return true;
}

bool ImageCompletionLayer_patchmask::initializeRandom8Bit(const uint8_t* frame, const uint8_t* mask, const unsigned int patchSize)
{
	ocean_assert(layerPatches);

	if (frame == nullptr || mask == nullptr || patchSize == 0 || (patchSize % 2) != 1)
		return false;

	const unsigned int patchHalf = patchSize >> 1;

	const unsigned int maxFrameX = width_ - patchHalf - 1;
	const unsigned int maxFrameY = height_ - patchHalf - 1;

	// random initialization
	for (unsigned int y = patchHalf; y <= maxFrameY; ++y)
		for (unsigned int x = patchHalf; x <= maxFrameX; ++x)
			if (mask[y * width_ + x] == 0)
			{
				bool found = false;

				for (unsigned int n = 0; n < 20; ++n)
				{
					const int sourceX = Random::random(int(patchHalf), int(maxFrameX));
					const int sourceY = Random::random(int(patchHalf), int(maxFrameY));

					const uint8_t* const maskUpper = mask + (sourceY - patchHalf) * width_ + sourceX - patchHalf;
					const uint8_t* const maskLower = maskUpper + patchSize * width_;

					if (*maskUpper && *(maskUpper + patchSize) && *maskLower && *(maskLower + patchSize))
					{
						const unsigned int ssd = calculateSSD8Bit(frame, width_, x, y, sourceX, sourceY, patchHalf);
						const unsigned int squareDistance = sqrLength(sourceX - x, sourceY - y);

						layerPatches[y * width_ + x] = Patch(sourceX, sourceY, ssd, squareDistance);
						found = true;
						break;
					}
				}

				// if no valid patch has been found we take randomly any
				if (found == false)
				{
					const int sourceX = Random::random(int(patchHalf), int(maxFrameX));
					const int sourceY = Random::random(int(patchHalf), int(maxFrameY));

					const unsigned int ssd = calculateSSD8Bit(frame, width_, x, y, sourceX, sourceY, patchHalf);
					const unsigned int squareDistance = sqrLength(sourceX - x, sourceY - y);

					layerPatches[y * width_ + x] = Patch(sourceX, sourceY, ssd, squareDistance);
				}
			}

	return true;
}

bool ImageCompletionLayer_patchmask::initializeRandom24Bit(const uint8_t* frame, const uint8_t* mask, const unsigned int patchSize)
{
	ocean_assert(layerPatches);

	if (frame == nullptr || mask == nullptr || patchSize == 0 || (patchSize % 2) != 1)
		return false;

	const unsigned int patchHalf = patchSize >> 1;

	const unsigned int maxFrameX = width_ - patchHalf - 1;
	const unsigned int maxFrameY = height_ - patchHalf - 1;

	// random initialization
	for (unsigned int y = patchHalf; y <= maxFrameY; ++y)
		for (unsigned int x = patchHalf; x <= maxFrameX; ++x)
			if (mask[y * width_ + x] == 0)
			{
				bool found = false;

				for (unsigned int n = 0; n < 20; ++n)
				{
					const int sourceX = Random::random(int(patchHalf), int(maxFrameX));
					const int sourceY = Random::random(int(patchHalf), int(maxFrameY));

					const uint8_t* const maskUpper = mask + (sourceY - patchHalf) * width_ + sourceX - patchHalf;
					const uint8_t* const maskLower = maskUpper + patchSize * width_;

					if (*maskUpper && *(maskUpper + patchSize) && *maskLower && *(maskLower + patchSize))
					{
						const unsigned int ssd = calculateSSD24Bit(frame, width_, x, y, sourceX, sourceY, patchHalf);
						const unsigned int squareDistance = sqrLength(sourceX - x, sourceY - y);

						layerPatches[y * width_ + x] = Patch(sourceX, sourceY, ssd, squareDistance);
						found = true;
						break;
					}
				}

				// if no valid patch has been found we take randomly any
				if (found == false)
				{
					const int sourceX = Random::random(int(patchHalf), int(maxFrameX));
					const int sourceY = Random::random(int(patchHalf), int(maxFrameY));

					const unsigned int ssd = calculateSSD24Bit(frame, width_, x, y, sourceX, sourceY, patchHalf);
					const unsigned int squareDistance = sqrLength(sourceX - x, sourceY - y);

					layerPatches[y * width_ + x] = Patch(sourceX, sourceY, ssd, squareDistance);
				}
			}

	return true;
}

bool ImageCompletionLayer_patchmask::improveRandom8Bit(const uint8_t* frame, const uint8_t* mask, const uint8_t* sourcePatchMask, const uint8_t* targetPatchMask, const unsigned int patchSize, const unsigned int iterations, Worker& worker)
{
	if (frame == nullptr || mask == nullptr || patchSize == 0 || (patchSize % 2) != 1)
		return false;

	const unsigned int patchHalf = patchSize >> 1; // patchSize / 2

	const unsigned int numberColumns = width_ - patchSize + 1;
	const unsigned int numberRows = height_ - patchSize + 1;

	worker.executeFunction(Worker::Function::create(*this, &ImageCompletionLayer_patchmask::improveRandom8Bit, frame, mask, sourcePatchMask, targetPatchMask, patchSize, iterations, patchHalf, numberColumns, 0u, 0u), patchHalf, numberRows, 8, 9);
	return true;
}

bool ImageCompletionLayer_patchmask::improveRandom24Bit(const uint8_t* frame, const uint8_t* mask, const uint8_t* sourcePatchMask, const uint8_t* targetPatchMask, const unsigned int patchSize, const unsigned int iterations, Worker& worker)
{
	if (frame == nullptr || mask == nullptr || patchSize == 0 || (patchSize % 2) != 1)
		return false;

	const unsigned int patchHalf = patchSize >> 1; // patchSize / 2

	const unsigned int numberColumns = width_ - patchSize + 1;
	const unsigned int numberRows = height_ - patchSize + 1;

	worker.executeFunction(Worker::Function::create(*this, &ImageCompletionLayer_patchmask::improveRandom24Bit, frame, mask, sourcePatchMask, targetPatchMask, patchSize, iterations, patchHalf, numberColumns, 0u, 0u), patchHalf, numberRows, 8, 9);
	return true;
}

bool ImageCompletionLayer_patchmask::improveRandom8Bit(const uint8_t* frame, const uint8_t* mask, const uint8_t* sourcePatchMask, const uint8_t* targetPatchMask, const unsigned int patchSize, const unsigned int iterations, Worker& worker, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows)
{
	if (frame == nullptr || mask == nullptr || patchSize == 0 || (patchSize % 2) != 1)
		return false;

	worker.executeFunction(Worker::Function::create(*this, &ImageCompletionLayer_patchmask::improveRandom8Bit, frame, mask, sourcePatchMask, targetPatchMask, patchSize, iterations, firstColumn, numberColumns, 0u, 0u), firstRow, numberRows, 8, 9);
	return true;
}

bool ImageCompletionLayer_patchmask::improveRandom24Bit(const uint8_t* frame, const uint8_t* mask, const uint8_t* sourcePatchMask, const uint8_t* targetPatchMask, const unsigned int patchSize, const unsigned int iterations, Worker& worker, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows)
{
	if (frame == nullptr || mask == nullptr || patchSize == 0 || (patchSize % 2) != 1)
		return false;

	worker.executeFunction(Worker::Function::create(*this, &ImageCompletionLayer_patchmask::improveRandom24Bit, frame, mask, sourcePatchMask, targetPatchMask, patchSize, iterations, firstColumn, numberColumns, 0u, 0u), firstRow, numberRows, 8, 9);
	return true;
}

bool ImageCompletionLayer_patchmask::coherenceImage8Bit(const uint8_t* frame, const uint8_t* mask, const uint8_t* sourcePatchMask, const uint8_t* /*targetPatchMask*/, uint8_t* result, const unsigned int patchSize)
{
	if (frame == nullptr || mask == nullptr || result == nullptr || patchSize == 0 || (patchSize % 2) != 1)
	{
		return false;
	}

	const unsigned int patchHalf = patchSize >> 1; // patchSize / 2

	const unsigned int maxFrameX = width_ - patchHalf - 1;
	const unsigned int maxFrameY = height_ - patchHalf - 1;

	Scalar* accumulatedCoherenceValues = new Scalar[pixels()];
	Scalar* accumulatedCoherenceWeights = new Scalar[pixels()];
	unsigned int* numberCoherenceValues = new unsigned int[pixels()];

	if (accumulatedCoherenceValues == nullptr || accumulatedCoherenceWeights == nullptr || numberCoherenceValues == nullptr)
	{
		delete [] accumulatedCoherenceValues;
		delete [] accumulatedCoherenceWeights;
		delete [] numberCoherenceValues;

		return false;
	}

	memset(accumulatedCoherenceValues, 0, sizeof(Scalar) * pixels());
	memset(accumulatedCoherenceWeights, 0, sizeof(Scalar) * pixels());
	memset(numberCoherenceValues, 0, sizeof(unsigned int) * pixels());

	const Scalar ssdNormalization = Scalar(1) / Scalar(255 * 255);

	// determine coherence color values
	/*for (unsigned int y = patchHalf; y <= maxFrameY; ++y)
		for (unsigned int x = patchHalf; x < maxFrameX; ++x)
			if (mask[y * layerWidth + x] == 0 && layerPatches[y * layerWidth + x].ssd() != 0xFFFFFFFF)
			{
				const Patch& patch = layerPatches[y * layerWidth + x];

				ocean_assert(patch.ssd() != 0xFFFFFFFF);
				ocean_assert(patch.sqrDistance() != 0xFFFFFFFF);

				const Scalar weight = Numeric::pow(2.71828183, -Numeric::sqrt(Scalar(patch.ssd() * ssdNormalization)));
				ocean_assert(Numeric::isNotEqualEps(weight));

				ocean_assert(patch.x() >= patchHalf && patch.x() <= maxFrameX);
				ocean_assert(patch.y() >= patchHalf && patch.y() <= maxFrameY);

				unsigned int sourceY = patch.y() - patchHalf;
				for (unsigned int targetY = y - patchHalf; targetY <= y + patchHalf; ++targetY)
				{
					unsigned int sourceX = patch.x() - patchHalf;

					for (unsigned int targetX = x - patchHalf; targetX <= x + patchHalf; ++targetX)
					{
						accumulatedCoherenceValues[targetY * layerWidth + targetX] += weight * Scalar(frame[sourceY * layerWidth + sourceX]);
						accumulatedCoherenceWeights[targetY * layerWidth + targetX] += weight;
						++numberCoherenceValues[targetY * layerWidth + targetX];

						++sourceX;
					}

					++sourceY;
				}
			}*/

	const unsigned int patchRowJump = width_ - patchSize;

	for (unsigned int y = patchHalf; y <= maxFrameY; ++y)
	{
		const uint8_t* const maskRow = mask + y * width_;
		const Patch* const patchRow = layerPatches + y * width_;

		for (unsigned int x = patchHalf; x <= maxFrameX; ++x)
			if (maskRow[x] == 0 && patchRow[x].ssd() != 0xFFFFFFFF)
			{
				const Patch& patch = patchRow[x];

				ocean_assert(patch.ssd() != 0xFFFFFFFF);
				ocean_assert(patch.sqrDistance() != 0xFFFFFFFF);

				ocean_assert(patch.x() >= patchHalf && patch.x() <= maxFrameX);
				ocean_assert(patch.y() >= patchHalf && patch.y() <= maxFrameY);

				Scalar weight = Numeric::pow(Scalar(2.71828183), -Numeric::sqrt(Scalar(patch.ssd()) * ssdNormalization));
				//weight = 1;
				//ocean_assert(Numeric::isNotEqualEps(weight));

				const unsigned int sourceXUpperLeft = patch.x() - patchHalf;
				const unsigned int sourceYUpperLeft = patch.y() - patchHalf;

				const unsigned int targetXUpperLeft = x - patchHalf;
				const unsigned int targetYUpperLeft = y - patchHalf;

				const uint8_t* frameData = frame + sourceYUpperLeft * width_ + sourceXUpperLeft - 1;
				Scalar* accumulatedCoherenceValuesData = accumulatedCoherenceValues + targetYUpperLeft * width_ + targetXUpperLeft - 1;
				Scalar* accumulatedCoherenceWeightsData = accumulatedCoherenceWeights + targetYUpperLeft * width_ + targetXUpperLeft - 1;
				unsigned int* numberCoherenceValuesData = numberCoherenceValues + targetYUpperLeft * width_ + targetXUpperLeft - 1;

				const uint8_t* const frameDataEnd = frameData + width_ * patchSize;

				while (frameData != frameDataEnd)
				{
					const uint8_t* const frameDataRowEnd = frameData + patchSize;

					while (frameData != frameDataRowEnd)
					{
						*++accumulatedCoherenceValuesData += weight * Scalar(*++frameData);
						*++accumulatedCoherenceWeightsData += weight;
						++(*++numberCoherenceValuesData);
					}

					frameData += patchRowJump;
					accumulatedCoherenceValuesData += patchRowJump;
					accumulatedCoherenceWeightsData += patchRowJump;
					numberCoherenceValuesData += patchRowJump;
				}
			}
	}

	const unsigned int patchArea = patchSize * patchSize;
	const Scalar areaFactor = Scalar(1) / Scalar(patchArea);

	// summation and weighting of coherence color values
	--result;
	--frame;
	--sourcePatchMask;
	const uint8_t* const resultEnd = result + pixels();

	const Scalar* coherenceValues = accumulatedCoherenceValues - 1;
	const Scalar* coherenceWeights = accumulatedCoherenceWeights - 1;
	const unsigned int* coherenceNumbers = numberCoherenceValues - 1;

	while (result != resultEnd)
	{
		unsigned int coherenceNumber = *++coherenceNumbers;

		//if (coherenceNumber > 0)
		//	coherenceNumber = patchArea;

		if (*++sourcePatchMask == 0 && coherenceNumber)
		{
			if (*mask == 0)
			{
				*++result = uint8_t(*++coherenceValues / *++coherenceWeights);
				//*result = 0;
				++frame;
			}
			else
			{
			const Scalar coherenceFactor = Scalar(coherenceNumber) / Scalar(*++coherenceWeights);
			const unsigned int frameNumber = patchArea - coherenceNumber;
			*++result = uint8_t((*++coherenceValues * coherenceFactor + Scalar(*++frame * frameNumber)) * areaFactor);
			//*result = 0xFF;

			//*++result = uint8_t(*++coherenceValues / *++coherenceWeights);
			//*++result = uint8_t((coherenceNumber * *++coherenceValues / *++coherenceWeights) / coherenceNumber);
			//++frame;
			}
		}
		else
		{
			*++result = *++frame;

			++coherenceValues;
			++coherenceWeights;
		}

		++mask;
	}

	delete [] accumulatedCoherenceValues;
	delete [] accumulatedCoherenceWeights;
	delete [] numberCoherenceValues;

	return true;
}

bool ImageCompletionLayer_patchmask::coherenceImage8Bit(const uint8_t* frame, const uint8_t* mask, const uint8_t* /*sourcePatchMask*/, const uint8_t* /*targetPatchMask*/, uint8_t* result, const unsigned int patchSize, const unsigned int offset)
{
	if (frame == nullptr || mask == nullptr || result == nullptr || patchSize == 0 || (patchSize % 2) != 1 || offset == 0 || (patchSize % offset) != 0)
		return false;

	const unsigned int patchHalf = patchSize >> 1; // patchSize / 2

	const unsigned int maxFrameX = width_ - patchHalf - 1;
	const unsigned int maxFrameY = height_ - patchHalf - 1;

	Scalar* accumulatedCoherenceValues = new Scalar[pixels()];
	Scalar* accumulatedCoherenceWeights = new Scalar[pixels()];
	unsigned int* numberCoherenceValues = new unsigned int[pixels()];

	if (accumulatedCoherenceValues == nullptr || accumulatedCoherenceWeights == nullptr || numberCoherenceValues == nullptr)
	{
		delete [] accumulatedCoherenceValues;
		delete [] accumulatedCoherenceWeights;
		delete [] numberCoherenceValues;

		return false;
	}

	memset(accumulatedCoherenceValues, 0, sizeof(Scalar) * pixels());
	memset(accumulatedCoherenceWeights, 0, sizeof(Scalar) * pixels());
	memset(numberCoherenceValues, 0, sizeof(unsigned int) * pixels());

	const Scalar ssdNormalization = Scalar(1) / Scalar(255 * 255);

	// determine coherence color values
	/*for (unsigned int y = patchHalf; y <= maxFrameY; y += offset)
		for (unsigned int x = patchHalf; x < maxFrameX; x += offset)
			if (mask[y * layerWidth + x] == 0 && layerPatches[y * layerWidth + x].ssd() != 0xFFFFFFFF)
			{
				const Patch& patch = layerPatches[y * layerWidth + x];

				ocean_assert(patch.ssd() != 0xFFFFFFFF);
				ocean_assert(patch.sqrDistance() != 0xFFFFFFFF);

				const Scalar weight = Numeric::pow(2.71828183, -Numeric::sqrt(Scalar(patch.ssd() * ssdNormalization)));
				ocean_assert(Numeric::isNotEqualEps(weight));

				ocean_assert(patch.x() >= patchHalf && patch.x() <= maxFrameX);
				ocean_assert(patch.y() >= patchHalf && patch.y() <= maxFrameY);

				unsigned int sourceY = patch.y() - patchHalf;
				for (unsigned int targetY = y - patchHalf; targetY <= y + patchHalf; ++targetY)
				{
					unsigned int sourceX = patch.x() - patchHalf;

					for (unsigned int targetX = x - patchHalf; targetX <= x + patchHalf; ++targetX)
					{
						accumulatedCoherenceValues[targetY * layerWidth + targetX] += weight * Scalar(frame[sourceY * layerWidth + sourceX]);
						accumulatedCoherenceWeights[targetY * layerWidth + targetX] += weight;
						++numberCoherenceValues[targetY * layerWidth + targetX];

						++sourceX;
					}

					++sourceY;
				}
			}*/

	const unsigned int patchRowJump = width_ - patchSize;

	for (unsigned int y = patchHalf; y <= maxFrameY; y += offset)
	{
		const uint8_t* const maskRow = mask + y * width_;
		const Patch* const patchRow = layerPatches + y * width_;

		for (unsigned int x = patchHalf; x <= maxFrameX; x += offset)
			if (maskRow[x] == 0 && patchRow[x].ssd() != 0xFFFFFFFF)
			{
				const Patch& patch = patchRow[x];

				ocean_assert(patch.ssd() != 0xFFFFFFFF);
				ocean_assert(patch.sqrDistance() != 0xFFFFFFFF);

				const Scalar weight = Numeric::pow(Scalar(2.71828183), -Numeric::sqrt(Scalar(patch.ssd()) * ssdNormalization));
				ocean_assert(Numeric::isNotEqualEps(weight));

				ocean_assert(patch.x() >= patchHalf && patch.x() <= maxFrameX);
				ocean_assert(patch.y() >= patchHalf && patch.y() <= maxFrameY);

				const unsigned int sourceXUpperLeft = patch.x() - patchHalf;
				const unsigned int sourceYUpperLeft = patch.y() - patchHalf;

				const unsigned int targetXUpperLeft = x - patchHalf;
				const unsigned int targetYUpperLeft = y - patchHalf;

				const uint8_t* frameData = frame + sourceYUpperLeft * width_ + sourceXUpperLeft - 1;
				Scalar* accumulatedCoherenceValuesData = accumulatedCoherenceValues + targetYUpperLeft * width_ + targetXUpperLeft - 1;
				Scalar* accumulatedCoherenceWeightsData = accumulatedCoherenceWeights + targetYUpperLeft * width_ + targetXUpperLeft - 1;
				unsigned int* numberCoherenceValuesData = numberCoherenceValues + targetYUpperLeft * width_ + targetXUpperLeft - 1;

				const uint8_t* const frameDataEnd = frameData + width_ * patchSize;

				while (frameData != frameDataEnd)
				{
					const uint8_t* const frameDataRowEnd = frameData + patchSize;

					while (frameData != frameDataRowEnd)
					{
						*++accumulatedCoherenceValuesData += weight * Scalar(*++frameData);
						*++accumulatedCoherenceWeightsData += weight;
						++(*++numberCoherenceValuesData);
					}

					frameData += patchRowJump;
					accumulatedCoherenceValuesData += patchRowJump;
					accumulatedCoherenceWeightsData += patchRowJump;
					numberCoherenceValuesData += patchRowJump;
				}
			}
	}

	const unsigned int patchArea = patchSize * patchSize / (offset * offset);
	const Scalar areaFactor = Scalar(1) / Scalar(patchArea);

	// summation and weighting of coherence color values
	--result;
	--frame;
	const uint8_t* const resultEnd = result + pixels();

	const Scalar* coherenceValues = accumulatedCoherenceValues - 1;
	const Scalar* coherenceWeights = accumulatedCoherenceWeights - 1;
	const unsigned int* coherenceNumbers = numberCoherenceValues - 1;

	while (result != resultEnd)
	{
		const unsigned int coherenceNumber = *++coherenceNumbers;

		if (coherenceNumber)
		{
			*++result = uint8_t((Scalar(coherenceNumber) * *++coherenceValues / *++coherenceWeights + Scalar(*++frame * (patchArea - coherenceNumber))) * areaFactor);
		}
		else
		{
			*++result = *++frame;

			++coherenceValues;
			++coherenceWeights;
		}
	}

	delete [] accumulatedCoherenceValues;
	delete [] accumulatedCoherenceWeights;
	delete [] numberCoherenceValues;

	return true;
}

bool ImageCompletionLayer_patchmask::coherenceImage24Bit(const uint8_t* frame, const uint8_t* mask, const uint8_t* sourcePatchMask, const uint8_t* /*targetPatchMask*/, uint8_t* result, const unsigned int patchSize)
{
	if (frame == nullptr || mask == nullptr || result == nullptr || patchSize == 0 || (patchSize % 2) != 1)
		return false;

	const unsigned int patchHalf = patchSize >> 1; // patchSize / 2

	const unsigned int maxFrameX = width_ - patchHalf - 1;
	const unsigned int maxFrameY = height_ - patchHalf - 1;

	Scalar* accumulatedCoherenceValues = new Scalar[3 * pixels()];
	Scalar* accumulatedCoherenceWeights = new Scalar[pixels()];
	unsigned int* numberCoherenceValues = new unsigned int[pixels()];

	if (accumulatedCoherenceValues == nullptr || accumulatedCoherenceWeights == nullptr || numberCoherenceValues == nullptr)
	{
		delete [] accumulatedCoherenceValues;
		delete [] accumulatedCoherenceWeights;
		delete [] numberCoherenceValues;

		return false;
	}

	memset(accumulatedCoherenceValues, 0, sizeof(Scalar) * 3 * pixels());
	memset(accumulatedCoherenceWeights, 0, sizeof(Scalar) * pixels());
	memset(numberCoherenceValues, 0, sizeof(unsigned int) * pixels());

	const Scalar ssdNormalization = Scalar(1) / Scalar(255 * 255);

	// determine coherence color values
	/*for (unsigned int y = patchHalf; y <= maxFrameY; ++y)
		for (unsigned int x = patchHalf; x < maxFrameX; ++x)
			if (mask[y * layerWidth + x] == 0 && layerPatches[y * layerWidth + x].ssd() != 0xFFFFFFFF)
			{
				const Patch& patch = layerPatches[y * layerWidth + x];

				ocean_assert(patch.ssd() != 0xFFFFFFFF);
				ocean_assert(patch.sqrDistance() != 0xFFFFFFFF);

				const Scalar weight = Numeric::pow(2.71828183, -Numeric::sqrt(Scalar(patch.ssd() * ssdNormalization)));
				ocean_assert(Numeric::isNotEqualEps(weight));

				ocean_assert(patch.x() >= patchHalf && patch.x() <= maxFrameX);
				ocean_assert(patch.y() >= patchHalf && patch.y() <= maxFrameY);

				unsigned int sourceY = patch.y() - patchHalf;
				for (unsigned int targetY = y - patchHalf; targetY <= y + patchHalf; ++targetY)
				{
					unsigned int sourceX = patch.x() - patchHalf;
					for (unsigned int targetX = x - patchHalf; targetX <= x + patchHalf; ++targetX)
					{
						Scalar* aValues = accumulatedCoherenceValues + targetY * 3 * layerWidth + 3 * targetX;
						const uint8_t* fValues = frame + sourceY * 3 * layerWidth + 3 * sourceX;

						aValues[0] += weight * fValues[0];
						aValues[1] += weight * fValues[1];
						aValues[2] += weight * fValues[2];

						accumulatedCoherenceWeights[targetY * layerWidth + targetX] += weight;
						++numberCoherenceValues[targetY * layerWidth + targetX];

						++sourceX;
					}

					++sourceY;
				}
			}*/

	const unsigned int patchRowJump = width_ - patchSize;
	const unsigned int patchRowJump3 = 3 * patchRowJump;

	for (unsigned int y = patchHalf; y <= maxFrameY; ++y)
	{
		const uint8_t* const maskRow = mask + y * width_;
		const Patch* const patchRow = layerPatches + y * width_;

		for (unsigned int x = patchHalf; x <= maxFrameX; ++x)
			if (maskRow[x] == 0 && patchRow[x].ssd() != 0xFFFFFFFF)
			{
				const Patch& patch = patchRow[x];

				ocean_assert(patch.ssd() != 0xFFFFFFFF);
				ocean_assert(patch.sqrDistance() != 0xFFFFFFFF);

				const Scalar weight = Numeric::pow(Scalar(2.71828183), -Numeric::sqrt(Scalar(patch.ssd()) * ssdNormalization));
				//ocean_assert(Numeric::isNotEqualEps(weight));

				ocean_assert(patch.x() >= patchHalf && patch.x() <= maxFrameX);
				ocean_assert(patch.y() >= patchHalf && patch.y() <= maxFrameY);

				const unsigned int sourceXUpperLeft = patch.x() - patchHalf;
				const unsigned int sourceYUpperLeft = patch.y() - patchHalf;

				const unsigned int targetXUpperLeft = x - patchHalf;
				const unsigned int targetYUpperLeft = y - patchHalf;

				const uint8_t* frameData = frame + sourceYUpperLeft * 3 * width_ + 3 * sourceXUpperLeft - 1;
				Scalar* accumulatedCoherenceValuesData = accumulatedCoherenceValues + targetYUpperLeft * 3 * width_ + 3 * targetXUpperLeft - 1;
				Scalar* accumulatedCoherenceWeightsData = accumulatedCoherenceWeights + targetYUpperLeft * width_ + targetXUpperLeft - 1;
				unsigned int* numberCoherenceValuesData = numberCoherenceValues + targetYUpperLeft * width_ + targetXUpperLeft - 1;

				const uint8_t* const frameDataEnd = frameData + 3 * width_ * patchSize;

				while (frameData != frameDataEnd)
				{
					const uint8_t* const frameDataRowEnd = frameData + 3 * patchSize;

					while (frameData != frameDataRowEnd)
					{
						*++accumulatedCoherenceValuesData += weight * Scalar(*++frameData);
						*++accumulatedCoherenceValuesData += weight * Scalar(*++frameData);
						*++accumulatedCoherenceValuesData += weight * Scalar(*++frameData);

						*++accumulatedCoherenceWeightsData += weight;
						++(*++numberCoherenceValuesData);
					}

					frameData += patchRowJump3;
					accumulatedCoherenceValuesData += patchRowJump3;
					accumulatedCoherenceWeightsData += patchRowJump;
					numberCoherenceValuesData += patchRowJump;
				}
			}
	}

	const unsigned int patchArea = patchSize * patchSize;
	const Scalar areaFactor = Scalar(1) / Scalar(patchArea);

	// summation and weighting of coherence color values
	--result;
	--frame;
	--sourcePatchMask;
	const uint8_t* const resultEnd = result + 3 * pixels();

	const Scalar* coherenceValues = accumulatedCoherenceValues - 1;
	const Scalar* coherenceWeights = accumulatedCoherenceWeights - 1;
	const unsigned int* coherenceNumbers = numberCoherenceValues - 1;

	while (result != resultEnd)
	{
		const unsigned int coherenceNumber = *++coherenceNumbers;

		if (*++sourcePatchMask == 0 && coherenceNumber)
		{
			if (*mask == 0)
			{
				const Scalar coherenceFactor = Scalar(1) / Scalar(*++coherenceWeights);

				*++result = uint8_t(*++coherenceValues * coherenceFactor);
				*++result = uint8_t(*++coherenceValues * coherenceFactor);
				*++result = uint8_t(*++coherenceValues * coherenceFactor);

				frame += 3;
			}
			else
			{
				const Scalar coherenceFactor = Scalar(coherenceNumber) / Scalar(*++coherenceWeights);
				const unsigned int frameNumber = patchArea - coherenceNumber;

				*++result = uint8_t((*++coherenceValues * coherenceFactor + Scalar(*++frame * frameNumber)) * areaFactor);
				*++result = uint8_t((*++coherenceValues * coherenceFactor + Scalar(*++frame * frameNumber)) * areaFactor);
				*++result = uint8_t((*++coherenceValues * coherenceFactor + Scalar(*++frame * frameNumber)) * areaFactor);
			}
		}
		else
		{
			*++result = *++frame;
			*++result = *++frame;
			*++result = *++frame;

			++coherenceWeights;
			coherenceValues += 3;
		}

		++mask;
	}

	delete [] accumulatedCoherenceValues;
	delete [] accumulatedCoherenceWeights;
	delete [] numberCoherenceValues;

	return true;
}

bool ImageCompletionLayer_patchmask::coherenceImage24Bit(const uint8_t* frame, const uint8_t* mask, const uint8_t* /*sourcePatchMask*/, const uint8_t* /*targetPatchMask*/, uint8_t* result, const unsigned int patchSize, const unsigned int offset)
{
	if (frame == nullptr || mask == nullptr || result == nullptr || patchSize == 0 || (patchSize % 2) != 1 || offset == 0 || (patchSize % offset) != 0)
		return false;

	const unsigned int patchHalf = patchSize >> 1; // patchSize / 2

	const unsigned int maxFrameX = width_ - patchHalf - 1;
	const unsigned int maxFrameY = height_ - patchHalf - 1;

	Scalar* accumulatedCoherenceValues = new Scalar[3 * pixels()];
	Scalar* accumulatedCoherenceWeights = new Scalar[pixels()];
	unsigned int* numberCoherenceValues = new unsigned int[pixels()];

	if (accumulatedCoherenceValues == nullptr || accumulatedCoherenceWeights == nullptr || numberCoherenceValues == nullptr)
	{
		delete [] accumulatedCoherenceValues;
		delete [] accumulatedCoherenceWeights;
		delete [] numberCoherenceValues;

		return false;
	}

	memset(accumulatedCoherenceValues, 0, sizeof(Scalar) * 3 * pixels());
	memset(accumulatedCoherenceWeights, 0, sizeof(Scalar) * pixels());
	memset(numberCoherenceValues, 0, sizeof(unsigned int) * pixels());

	const Scalar ssdNormalization = Scalar(1) / Scalar(3 * 255 * 255);

	// determine coherence color values
	/*for (unsigned int y = patchHalf; y <= maxFrameY; ++y)
		for (unsigned int x = patchHalf; x < maxFrameX; ++x)
			if (mask[y * layerWidth + x] == 0 && layerPatches[y * layerWidth + x].ssd() != 0xFFFFFFFF)
			{
				const Patch& patch = layerPatches[y * layerWidth + x];

				ocean_assert(patch.ssd() != 0xFFFFFFFF);
				ocean_assert(patch.sqrDistance() != 0xFFFFFFFF);

				const Scalar weight = Numeric::pow(2.71828183, -Numeric::sqrt(Scalar(patch.ssd() * ssdNormalization)));
				ocean_assert(Numeric::isNotEqualEps(weight));

				ocean_assert(patch.x() >= patchHalf && patch.x() <= maxFrameX);
				ocean_assert(patch.y() >= patchHalf && patch.y() <= maxFrameY);

				unsigned int sourceY = patch.y() - patchHalf;
				for (unsigned int targetY = y - patchHalf; targetY <= y + patchHalf; ++targetY)
				{
					unsigned int sourceX = patch.x() - patchHalf;
					for (unsigned int targetX = x - patchHalf; targetX <= x + patchHalf; ++targetX)
					{
						Scalar* aValues = accumulatedCoherenceValues + targetY * 3 * layerWidth + 3 * targetX;
						const uint8_t* fValues = frame + sourceY * 3 * layerWidth + 3 * sourceX;

						aValues[0] += weight * fValues[0];
						aValues[1] += weight * fValues[1];
						aValues[2] += weight * fValues[2];

						accumulatedCoherenceWeights[targetY * layerWidth + targetX] += weight;
						++numberCoherenceValues[targetY * layerWidth + targetX];

						++sourceX;
					}

					++sourceY;
				}
			}*/

	const unsigned int patchRowJump = width_ - patchSize;
	const unsigned int patchRowJump3 = 3 * patchRowJump;

	for (unsigned int y = patchHalf; y <= maxFrameY; y += offset)
	{
		const uint8_t* const maskRow = mask + y * width_;
		const Patch* const patchRow = layerPatches + y * width_;

		for (unsigned int x = patchHalf; x <= maxFrameX; x += offset)
			if (maskRow[x] == 0 && patchRow[x].ssd() != 0xFFFFFFFF)
			{
				const Patch& patch = patchRow[x];

				ocean_assert(patch.ssd() != 0xFFFFFFFF);
				ocean_assert(patch.sqrDistance() != 0xFFFFFFFF);

				const Scalar weight = Numeric::pow(Scalar(2.71828183), -Numeric::sqrt(Scalar(patch.ssd()) * ssdNormalization));
				ocean_assert(Numeric::isNotEqualEps(weight));

				ocean_assert(patch.x() >= patchHalf && patch.x() <= maxFrameX);
				ocean_assert(patch.y() >= patchHalf && patch.y() <= maxFrameY);

				const unsigned int sourceXUpperLeft = patch.x() - patchHalf;
				const unsigned int sourceYUpperLeft = patch.y() - patchHalf;

				const unsigned int targetXUpperLeft = x - patchHalf;
				const unsigned int targetYUpperLeft = y - patchHalf;

				const uint8_t* frameData = frame + sourceYUpperLeft * 3 * width_ + 3 * sourceXUpperLeft - 1;
				Scalar* accumulatedCoherenceValuesData = accumulatedCoherenceValues + targetYUpperLeft * 3 * width_ + 3 * targetXUpperLeft - 1;
				Scalar* accumulatedCoherenceWeightsData = accumulatedCoherenceWeights + targetYUpperLeft * width_ + targetXUpperLeft - 1;
				unsigned int* numberCoherenceValuesData = numberCoherenceValues + targetYUpperLeft * width_ + targetXUpperLeft - 1;

				const uint8_t* const frameDataEnd = frameData + 3 * width_ * patchSize;

				while (frameData != frameDataEnd)
				{
					const uint8_t* const frameDataRowEnd = frameData + 3 * patchSize;

					while (frameData != frameDataRowEnd)
					{
						*++accumulatedCoherenceValuesData += weight * Scalar(*++frameData);
						*++accumulatedCoherenceValuesData += weight * Scalar(*++frameData);
						*++accumulatedCoherenceValuesData += weight * Scalar(*++frameData);

						*++accumulatedCoherenceWeightsData += weight;
						++(*++numberCoherenceValuesData);
					}

					frameData += patchRowJump3;
					accumulatedCoherenceValuesData += patchRowJump3;
					accumulatedCoherenceWeightsData += patchRowJump;
					numberCoherenceValuesData += patchRowJump;
				}
			}
	}

	const unsigned int patchArea = patchSize * patchSize / (offset * offset);
	const Scalar areaFactor = Scalar(1) / Scalar(patchArea);

	// summation and weighting of coherence color values
	--result;
	--frame;
	const uint8_t* const resultEnd = result + 3 * pixels();

	const Scalar* coherenceValues = accumulatedCoherenceValues - 1;
	const Scalar* coherenceWeights = accumulatedCoherenceWeights - 1;
	const unsigned int* coherenceNumbers = numberCoherenceValues - 1;

	while (result != resultEnd)
	{
		const unsigned int coherenceNumber = *++coherenceNumbers;

		if (coherenceNumber)
		{
			const Scalar coherenceFactor = Scalar(coherenceNumber) / Scalar(*++coherenceWeights);
			const unsigned int frameNumber = patchArea - coherenceNumber;

			*++result = uint8_t((*++coherenceValues * coherenceFactor + Scalar(*++frame * frameNumber)) * areaFactor);
			*++result = uint8_t((*++coherenceValues * coherenceFactor + Scalar(*++frame * frameNumber)) * areaFactor);
			*++result = uint8_t((*++coherenceValues * coherenceFactor + Scalar(*++frame * frameNumber)) * areaFactor);
		}
		else
		{
			*++result = *++frame;
			*++result = *++frame;
			*++result = *++frame;

			++coherenceWeights;
			coherenceValues += 3;
		}
	}

	delete [] accumulatedCoherenceValues;
	delete [] accumulatedCoherenceWeights;
	delete [] numberCoherenceValues;

	return true;
}

ImageCompletionLayer_patchmask& ImageCompletionLayer_patchmask::operator=(const ImageCompletionLayer_patchmask& layer)
{
	if (width_ != layer.width_ || height_ != layer.height_)
	{
		delete [] layerPatches;
		layerPatches = nullptr;

		width_ = layer.width_;
		height_ = layer.height_;

		if (width_ != 0 && height_ != 0)
		{
			layerPatches = new Patch[pixels()];
			ocean_assert(layerPatches != nullptr);
		}
	}

	if (pixels() != 0)
		memcpy(layerPatches, layer.layerPatches, sizeof(Patch) * pixels());

	return *this;
}

void ImageCompletionLayer_patchmask::improveRandom8Bit(const uint8_t* frame, const uint8_t* /*mask*/, const uint8_t* sourcePatchMask, const uint8_t* targetPatchMask, const unsigned int patchSize, const unsigned int iterations, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(layerPatches);

	ocean_assert(frame != nullptr);
	ocean_assert(patchSize == 0 || (patchSize % 2) == 1);

	ocean_assert(sourcePatchMask && targetPatchMask);

	const unsigned int patchHalf = patchSize >> 1;

	const unsigned int minFrameX = max(patchHalf, firstColumn);
	const unsigned int minFrameY = max(patchHalf, firstRow);

	const unsigned int maxFrameX = min(width_ - patchHalf - 1, firstColumn + numberColumns);
	const unsigned int maxFrameY = min(height_ - patchHalf - 1, firstRow + numberRows);


	// improved top-left patch
	for (unsigned int y = minFrameY; y <= maxFrameY; ++y)
		for (unsigned int x = minFrameX; x <= maxFrameX; ++x)
			//if (mask[y * layerWidth + x] == 0)
			if (sourcePatchMask[y * width_ + x] == 0)
			{
				Patch* const patch = layerPatches + y * width_ + x;
				ocean_assert(patch->ssd() != 0xFFFFFFFF);
				ocean_assert(patch->sqrDistance() != 0xFFFFFFFF);

				for (unsigned int n = 0; n < pixels() / 1000; ++n)
				{
					// random search
					unsigned int index = 0;
					while (index < 8)
					{
						const unsigned int testX = (unsigned int)(Scalar(patch->x()) + Scalar(width_ )* searchAreaFactors[index] * Random::scalar(-1, 1));
						const unsigned int testY = (unsigned int)(Scalar(patch->y()) + Scalar(height_) * searchAreaFactors[index] * Random::scalar(-1, 1));

						if (testX < width_ && testY < height_)
						{
							const uint8_t* const targetPatchMaskCenter = targetPatchMask + testY * width_ + testX;

							//const uint8_t* const maskUpper = mask + (testY - patchHalf) * layerWidth + testX - patchHalf;
							//const uint8_t* const maskLower = maskUpper + (patchSize - 1) * layerWidth;

							//if (testX >= patchHalf && testX < layerWidth - patchHalf && testY >= patchHalf && testY < layerHeight - patchHalf
							//	&& *maskUpper && *(maskUpper + patchSize) && *maskLower && *(maskLower + patchSize)) //&& mask[testY * layerWidth + testX]
							if (*targetPatchMaskCenter == 0xFF)
							{
								const unsigned int ssd = calculateSSD8Bit(frame, width_, x, y, testX, testY, patchHalf);
								const unsigned int sqrDistance = sqrLength(testX - x, testY - y);

								if (ssd + sqrDistance < patch->ssd() + patch->sqrDistance())
								//if (ssd < patch->ssd())
									*patch = Patch(testX, testY, ssd, sqrDistance);
							}
						}

						++index;
					}
				}

				y = maxFrameY + 1;
				break;
			}

	// improved bottom-right patch
	for (int y = maxFrameY; y >= int(minFrameY); --y)
		for (int x = maxFrameX; x >= int(minFrameX); --x)
			//if (mask[y * layerWidth + x] == 0)
			if (sourcePatchMask[y * width_ + x] == 0)
			{
				Patch* const patch = layerPatches + y * width_ + x;
				ocean_assert(patch->ssd() != 0xFFFFFFFF);
				ocean_assert(patch->sqrDistance() != 0xFFFFFFFF);

				for (unsigned int n = 0; n < pixels() / 1000; ++n)
				{
					// random search
					unsigned int index = 0;
					while (index < 8)
					{
						const unsigned int testX = (unsigned int)(Scalar(patch->x()) + Scalar(width_) * searchAreaFactors[index] * Random::scalar(-1, 1));
						const unsigned int testY = (unsigned int)(Scalar(patch->y()) + Scalar(height_) * searchAreaFactors[index] * Random::scalar(-1, 1));

						if (testX < width_ && testY < height_)
						{
						/*const uint8_t* const maskUpper = mask + (testY - patchHalf) * layerWidth + testX - patchHalf;
						const uint8_t* const maskLower = maskUpper + (patchSize - 1) * layerWidth;

						if (testX >= patchHalf && testX < layerWidth - patchHalf && testY >= patchHalf && testY < layerHeight - patchHalf
							&& *maskUpper && *(maskUpper + patchSize) && *maskLower && *(maskLower + patchSize)) //&& mask[testY * layerWidth + testX]*/

							const uint8_t* const targetPatchMaskCenter = targetPatchMask + testY * width_ + testX;

							if (*targetPatchMaskCenter == 0xFF)
							{
								const unsigned int ssd = calculateSSD8Bit(frame, width_, x, y, testX, testY, patchHalf);
								const unsigned int sqrDistance = sqrLength(testX - x, testY - y);

								if (ssd + sqrDistance < patch->ssd() + patch->sqrDistance())
								//if (ssd < patch->ssd())
									*patch = Patch(testX, testY, ssd, sqrDistance);
							}
						}

						++index;
					}
				}

				y = int(minFrameY) - 1;
				break;
			}


	for (unsigned int n = 0; n < iterations; n++)
	{
		// top to bottom
		for (unsigned int y = minFrameY; y <= maxFrameY; ++y)
			for (unsigned int x = minFrameX; x <= maxFrameX; ++x)
				//if (mask[y * layerWidth + x] == 0)
				if (sourcePatchMask[y * width_ + x] == 0)
				{
					Patch* const patch = layerPatches + y * width_ + x;

					ocean_assert(patch->ssd() != 0xFFFFFFFF);
					ocean_assert(patch->sqrDistance() != 0xFFFFFFFF);

					// top propagation
					const Patch* const patchTop = patch - width_;

					if (patchTop->ssd() != 0xFFFFFFFF)
					{
						const unsigned int testX = patchTop->x();
						const unsigned int testY = patchTop->y() + 1;
						//const unsigned int testY = min(layerHeight - patchHalf - 1, patchTop->y() + 1);
						//const unsigned int testY = min(layerHeight - patchHalf - 1, patchTop->y() + 1);

						if (testX < width_ && testY < height_)
						{
							const uint8_t* const targetPatchMaskCenter = targetPatchMask + testY * width_ + testX;

							// this check should be enlarged to the entire patch size (corners), however the center pixel test allows a smother border
							//if (mask[testY * layerWidth + testX])
							if (*targetPatchMaskCenter == 0xFF)
							{
								const unsigned int ssd = calculateSSD8Bit(frame, width_, x, y, testX, testY, patchHalf);
								const unsigned int sqrDistance = sqrLength(testX - x, testY - y);

								if (ssd + sqrDistance < patch->ssd() + patch->sqrDistance())
								//if (ssd < patch->ssd())
									*patch = Patch(testX, testY, ssd, sqrDistance);
							}
						}
					}

					// left propagation
					const Patch* const patchLeft = patch - 1;

					if (patchLeft->ssd() != 0xFFFFFFFF)
					{
						//const unsigned int testX = min(layerWidth - patchHalf - 1, patchLeft->x() + 1);
						const unsigned int testX = patchLeft->x() + 1;
						const unsigned int testY = patchLeft->y();

						if (testX < width_ && testY < height_)
						{
							const uint8_t* const targetPatchMaskCenter = targetPatchMask + testY * width_ + testX;

							// this check should be enlarged to the entire patch size (corners), however the center pixel test allows a smother border
							//if (mask[testY * layerWidth + testX])
							if (*targetPatchMaskCenter == 0xFF)
							{
								const unsigned int ssd = calculateSSD8Bit(frame, width_, x, y, testX, testY, patchHalf);
								const unsigned int sqrDistance = sqrLength(testX - x, testY - y);

								if (ssd + sqrDistance < patch->ssd() + patch->sqrDistance())
								//if (ssd < patch->ssd())
									*patch = Patch(testX, testY, ssd, sqrDistance);
							}
						}
					}

					// random search
					unsigned int index = 0;
					while (index < 8)
					{
						const unsigned int testX = (unsigned int)(Scalar(patch->x()) + Scalar(width_) * searchAreaFactors[index] * Random::scalar(-1, 1));
						const unsigned int testY = (unsigned int)(Scalar(patch->y()) + Scalar(height_) * searchAreaFactors[index] * Random::scalar(-1, 1));

						if (testX < width_ && testY < height_)
						{
							/*const uint8_t* const maskUpper = mask + (testY - patchHalf) * layerWidth + testX - patchHalf;
							const uint8_t* const maskLower = maskUpper + (patchSize - 1) * layerWidth;

							if (testX >= patchHalf && testX < layerWidth - patchHalf && testY >= patchHalf && testY < layerHeight - patchHalf
								&& *maskUpper && *(maskUpper + patchSize) && *maskLower && *(maskLower + patchSize)) //&& mask[testY * layerWidth + testX]*/

							const uint8_t* const targetPatchMaskCenter = targetPatchMask + testY * width_ + testX;

							if (*targetPatchMaskCenter == 0xFF)
							{
								const unsigned int ssd = calculateSSD8Bit(frame, width_, x, y, testX, testY, patchHalf);
								const unsigned int sqrDistance = sqrLength(testX - x, testY - y);

								if (ssd + sqrDistance < patch->ssd() + patch->sqrDistance())
								//if (ssd < patch->ssd())
									*patch = Patch(testX, testY, ssd, sqrDistance);
							}
						}

						//++index;
						++index;
					}
				}

		// bottom to top
		for (unsigned int y = maxFrameY; y >= minFrameY && y != 0xFFFFFFFF; --y)
			for (unsigned int x = maxFrameX; x >= minFrameX && x != 0xFFFFFFFF; --x)
				//if (mask[y * layerWidth + x] == 0)
				if (sourcePatchMask[y * width_ + x] == 0)
				{
					Patch* const patch = layerPatches + y * width_ + x;

					ocean_assert(patch->ssd() != 0xFFFFFFFF);
					ocean_assert(patch->sqrDistance() != 0xFFFFFFFF);

					// bottom propagation
					const Patch* const patchBottom = patch + width_;

					if (patchBottom->ssd() != 0xFFFFFFFF)
					{
						const unsigned int testX = patchBottom->x();
						//const unsigned int testY = max(patchHalf, patchBottom->y() - 1);
						const unsigned int testY = patchBottom->y() - 1;

						if (testX < width_ && testY < height_)
						{
							const uint8_t* const targetPatchMaskCenter = targetPatchMask + testY * width_ + testX;

							// this check should be enlarged to the entire patch size (corners), however the center pixel test allows a smother border
							//if (mask[testY * layerWidth + testX])
							if (*targetPatchMaskCenter == 0xFF)
							{
								const unsigned int ssd = calculateSSD8Bit(frame, width_, x, y, testX, testY, patchHalf);
								const unsigned int sqrDistance = sqrLength(testX - x, testY - y);

								if (ssd + sqrDistance < patch->ssd() + patch->sqrDistance())
								//if (ssd < patch->ssd())
									*patch = Patch(testX, testY, ssd, sqrDistance);
							}
						}
					}

					// right propagation
					const Patch* const patchRight = patch + 1;

					if (patchRight->ssd() != 0xFFFFFFFF)
					{
						//const unsigned int testX = max(patchHalf, patchRight->x() - 1);
						const unsigned int testX =  patchRight->x() - 1;
						const unsigned int testY = patchRight->y();

						if (testX < width_ && testY < height_)
						{
							const uint8_t* const targetPatchMaskCenter = targetPatchMask + testY * width_ + testX;

							// this check should be enlarged to the entire patch size (corners), however the center pixel test allows a smother border
							//if (mask[testY * layerWidth + testX])
							if (*targetPatchMaskCenter == 0xFF)
							{
								const unsigned int ssd = calculateSSD8Bit(frame, width_, x, y, testX, testY, patchHalf);
								const unsigned int sqrDistance = sqrLength(testX - x, testY - y);

								if (ssd + sqrDistance < patch->ssd() + patch->sqrDistance())
								//if (ssd < patch->ssd())
									*patch = Patch(testX, testY, ssd, sqrDistance);
							}
						}
					}

					// random search
					unsigned int index = 0;
					while (index < 8)
					{
						const unsigned int testX = (unsigned int)(Scalar(patch->x()) + Scalar(width_) * searchAreaFactors[index] * Random::scalar(-1, 1));
						const unsigned int testY = (unsigned int)(Scalar(patch->y()) + Scalar(height_) * searchAreaFactors[index] * Random::scalar(-1, 1));

						if (testX < width_ && testY < height_)
						{

							/*const uint8_t* const maskUpper = mask + (testY - patchHalf) * layerWidth + testX - patchHalf;
							const uint8_t* const maskLower = maskUpper + (patchSize - 1) * layerWidth;

							if (testX >= patchHalf && testX < layerWidth - patchHalf && testY >= patchHalf && testY < layerHeight - patchHalf
								&& *maskUpper && *(maskUpper + patchSize) && *maskLower && *(maskLower + patchSize)) //&& mask[testY * layerWidth + testX]*/

							const uint8_t* const targetPatchMaskCenter = targetPatchMask + testY * width_ + testX;

							if (*targetPatchMaskCenter == 0xFF)
							{
								const unsigned int ssd = calculateSSD8Bit(frame, width_, x, y, testX, testY, patchHalf);
								const unsigned int sqrDistance = sqrLength(testX - x, testY - y);

								if (ssd + sqrDistance < patch->ssd() + patch->sqrDistance())
								//if (ssd < patch->ssd())
									*patch = Patch(testX, testY, ssd, sqrDistance);
							}
						}

						//++index;
						++index;
					}
				}
	}
}

void ImageCompletionLayer_patchmask::improveRandom24Bit(const uint8_t* frame, const uint8_t* /*mask*/, const uint8_t* sourcePatchMask, const uint8_t* targetPatchMask, const unsigned int patchSize, const unsigned int iterations, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(layerPatches);

	ocean_assert(frame != nullptr);
	ocean_assert(patchSize == 0 || (patchSize % 2) == 1);

	ocean_assert(sourcePatchMask && targetPatchMask);

	const unsigned int patchHalf = patchSize >> 1;

	const unsigned int minFrameX = max(patchHalf, firstColumn);
	const unsigned int minFrameY = max(patchHalf, firstRow);

	const unsigned int maxFrameX = min(width_ - patchHalf - 1, firstColumn + numberColumns);
	const unsigned int maxFrameY = min(height_ - patchHalf - 1, firstRow + numberRows);


	// improved top-left patch
	for (unsigned int y = minFrameY; y <= maxFrameY; ++y)
		for (unsigned int x = minFrameX; x <= maxFrameX; ++x)
			//if (mask[y * layerWidth + x] == 0)
			if (sourcePatchMask[y * width_ + x] == 0)
			{
				Patch* const patch = layerPatches + y * width_ + x;
				ocean_assert(patch->ssd() != 0xFFFFFFFF);
				ocean_assert(patch->sqrDistance() != 0xFFFFFFFF);

				for (unsigned int n = 0; n < pixels() / 1000; ++n)
				{
					// random search
					unsigned int index = 0;
					while (index < 8)
					{
						const unsigned int testX = (unsigned int)(Scalar(patch->x()) + Scalar(width_) * searchAreaFactors[index] * Random::scalar(-1, 1));
						const unsigned int testY = (unsigned int)(Scalar(patch->y()) + Scalar(height_) * searchAreaFactors[index] * Random::scalar(-1, 1));

						if (testX < width_ && testY < height_)
						{
							const uint8_t* const targetPatchMaskCenter = targetPatchMask + testY * width_ + testX;

							//const uint8_t* const maskUpper = mask + (testY - patchHalf) * layerWidth + testX - patchHalf;
							//const uint8_t* const maskLower = maskUpper + (patchSize - 1) * layerWidth;

							//if (testX >= patchHalf && testX < layerWidth - patchHalf && testY >= patchHalf && testY < layerHeight - patchHalf
							//	&& *maskUpper && *(maskUpper + patchSize) && *maskLower && *(maskLower + patchSize)) //&& mask[testY * layerWidth + testX]
							if (*targetPatchMaskCenter == 0xFF)
							{
								const unsigned int ssd = calculateSSD24Bit(frame, width_, x, y, testX, testY, patchHalf);
								const unsigned int sqrDistance = 3 * sqrLength(testX - x, testY - y);
								OCEAN_SUPPRESS_UNUSED_WARNING(sqrDistance);

								//if (ssd + sqrDistance < patch->ssd() + patch->sqrDistance())
								if (ssd < patch->ssd())
									*patch = Patch(testX, testY, ssd, 0);
							}
						}

						++index;
					}
				}

				y = maxFrameY + 1;
				break;
			}

	// improved bottom-right patch
	for (int y = maxFrameY; y >= int(minFrameY); --y)
		for (int x = maxFrameX; x >= int(minFrameX); --x)
			//if (mask[y * layerWidth + x] == 0)
			if (sourcePatchMask[y * width_ + x] == 0)
			{
				Patch* const patch = layerPatches + y * width_ + x;
				ocean_assert(patch->ssd() != 0xFFFFFFFF);
				ocean_assert(patch->sqrDistance() != 0xFFFFFFFF);

				for (unsigned int n = 0; n < pixels() / 1000; ++n)
				{
					// random search
					unsigned int index = 0;
					while (index < 8)
					{
						const unsigned int testX = (unsigned int)(Scalar(patch->x()) + Scalar(width_) * searchAreaFactors[index] * Random::scalar(-1, 1));
						const unsigned int testY = (unsigned int)(Scalar(patch->y()) + Scalar(height_) * searchAreaFactors[index] * Random::scalar(-1, 1));

						if (testX < width_ && testY < height_)
						{
						/*const uint8_t* const maskUpper = mask + (testY - patchHalf) * layerWidth + testX - patchHalf;
						const uint8_t* const maskLower = maskUpper + (patchSize - 1) * layerWidth;

						if (testX >= patchHalf && testX < layerWidth - patchHalf && testY >= patchHalf && testY < layerHeight - patchHalf
							&& *maskUpper && *(maskUpper + patchSize) && *maskLower && *(maskLower + patchSize)) //&& mask[testY * layerWidth + testX]*/

							const uint8_t* const targetPatchMaskCenter = targetPatchMask + testY * width_ + testX;

							if (*targetPatchMaskCenter == 0xFF)
							{
								const unsigned int ssd = calculateSSD24Bit(frame, width_, x, y, testX, testY, patchHalf);
								const unsigned int sqrDistance = 3 * sqrLength(testX - x, testY - y);
								OCEAN_SUPPRESS_UNUSED_WARNING(sqrDistance);

								//if (ssd + sqrDistance < patch->ssd() + patch->sqrDistance())
								if (ssd < patch->ssd())
									*patch = Patch(testX, testY, ssd, 0);
							}
						}

						++index;
					}
				}

				y = int(minFrameY) - 1;
				break;
			}


	for (unsigned int n = 0; n < iterations; n++)
	{
		// top to bottom
		for (unsigned int y = minFrameY; y <= maxFrameY; ++y)
			for (unsigned int x = minFrameX; x <= maxFrameX; ++x)
				//if (mask[y * layerWidth + x] == 0)
				if (sourcePatchMask[y * width_ + x] == 0)
				{
					Patch* const patch = layerPatches + y * width_ + x;

					ocean_assert(patch->ssd() != 0xFFFFFFFF);
					ocean_assert(patch->sqrDistance() != 0xFFFFFFFF);

					// top propagation
					const Patch* const patchTop = patch - width_;

					if (patchTop->ssd() != 0xFFFFFFFF)
					{
						const unsigned int testX = patchTop->x();
						const unsigned int testY = patchTop->y() + 1;
						//const unsigned int testY = min(layerHeight - patchHalf - 1, patchTop->y() + 1);
						//const unsigned int testY = min(layerHeight - patchHalf - 1, patchTop->y() + 1);

						if (testX < width_ && testY < height_)
						{
							const uint8_t* const targetPatchMaskCenter = targetPatchMask + testY * width_ + testX;

							// this check should be enlarged to the entire patch size (corners), however the center pixel test allows a smother border
							//if (mask[testY * layerWidth + testX])
							if (*targetPatchMaskCenter == 0xFF)
							{
								const unsigned int ssd = calculateSSD24Bit(frame, width_, x, y, testX, testY, patchHalf);
								const unsigned int sqrDistance = 3 * sqrLength(testX - x, testY - y);
								OCEAN_SUPPRESS_UNUSED_WARNING(sqrDistance);

								//if (ssd + sqrDistance < patch->ssd() + patch->sqrDistance())
								if (ssd < patch->ssd())
									*patch = Patch(testX, testY, ssd, 0);
							}
						}
					}

					// left propagation
					const Patch* const patchLeft = patch - 1;

					if (patchLeft->ssd() != 0xFFFFFFFF)
					{
						//const unsigned int testX = min(layerWidth - patchHalf - 1, patchLeft->x() + 1);
						const unsigned int testX = patchLeft->x() + 1;
						const unsigned int testY = patchLeft->y();

						if (testX < width_ && testY < height_)
						{
							const uint8_t* const targetPatchMaskCenter = targetPatchMask + testY * width_ + testX;

							// this check should be enlarged to the entire patch size (corners), however the center pixel test allows a smother border
							//if (mask[testY * layerWidth + testX])
							if (*targetPatchMaskCenter == 0xFF)
							{
								const unsigned int ssd = calculateSSD24Bit(frame, width_, x, y, testX, testY, patchHalf);
								const unsigned int sqrDistance = 3 * sqrLength(testX - x, testY - y);
								OCEAN_SUPPRESS_UNUSED_WARNING(sqrDistance);

								//if (ssd + sqrDistance < patch->ssd() + patch->sqrDistance())
								if (ssd < patch->ssd())
									*patch = Patch(testX, testY, ssd, 0);
							}
						}
					}

					// random search
					unsigned int index = 0;
					while (index < 8)
					{
						const unsigned int testX = (unsigned int)(Scalar(patch->x()) + Scalar(width_) * searchAreaFactors[index] * Random::scalar(-1, 1));
						const unsigned int testY = (unsigned int)(Scalar(patch->y()) + Scalar(height_) * searchAreaFactors[index] * Random::scalar(-1, 1));

						if (testX < width_ && testY < height_)
						{
							/*const uint8_t* const maskUpper = mask + (testY - patchHalf) * layerWidth + testX - patchHalf;
							const uint8_t* const maskLower = maskUpper + (patchSize - 1) * layerWidth;

							if (testX >= patchHalf && testX < layerWidth - patchHalf && testY >= patchHalf && testY < layerHeight - patchHalf
								&& *maskUpper && *(maskUpper + patchSize) && *maskLower && *(maskLower + patchSize)) //&& mask[testY * layerWidth + testX]*/

							const uint8_t* const targetPatchMaskCenter = targetPatchMask + testY * width_ + testX;

							if (*targetPatchMaskCenter == 0xFF)
							{
								const unsigned int ssd = calculateSSD24Bit(frame, width_, x, y, testX, testY, patchHalf);

								Scalar distanceCost = 0;

								if (sourcePatchMask[y * width_ + x - 1] == 0)
								{
									const int xOffset = (layerPatches[y * width_ + x - 1].x() - testX) + 1;
									const int yOffset = layerPatches[y * width_ + x - 1].y() - testY;

									distanceCost += Numeric::sqrt(Scalar(xOffset * xOffset + yOffset * yOffset)) / Numeric::sqrt(Scalar(width_ * width_ + height_ * height_));
								}

								if (sourcePatchMask[(y - 1) * width_ + x] == 0)
								{
									const int xOffset = layerPatches[(y - 1) * width_ + x].x() - testX;
									const int yOffset = (layerPatches[(y - 1) * width_ + x].y() - testY) + 1;

									distanceCost += Numeric::sqrt(Scalar(xOffset * xOffset + yOffset * yOffset)) / Numeric::sqrt(Scalar(width_ * width_ + height_ * height_));
								}

								const unsigned int sqrDistance = 2 * (unsigned int)distanceCost * 255 * 255 * 4;

								//const unsigned int sqrDistance = 3 * sqrLength(testX - x, testY - y);

								if (ssd + sqrDistance < patch->ssd() + patch->sqrDistance())
								//if (ssd < patch->ssd())
									*patch = Patch(testX, testY, ssd, sqrDistance);
							}
						}

						//++index;
						++index;
					}
				}

		// bottom to top
		for (unsigned int y = maxFrameY; y >= minFrameY && y != 0xFFFFFFFF; --y)
			for (unsigned int x = maxFrameX; x >= minFrameX && x != 0xFFFFFFFF; --x)
				//if (mask[y * layerWidth + x] == 0)
				if (sourcePatchMask[y * width_ + x] == 0)
				{
					Patch* const patch = layerPatches + y * width_ + x;

					ocean_assert(patch->ssd() != 0xFFFFFFFF);
					ocean_assert(patch->sqrDistance() != 0xFFFFFFFF);

					// bottom propagation
					const Patch* const patchBottom = patch + width_;

					if (patchBottom->ssd() != 0xFFFFFFFF)
					{
						const unsigned int testX = patchBottom->x();
						//const unsigned int testY = max(patchHalf, patchBottom->y() - 1);
						const unsigned int testY = patchBottom->y() - 1;

						if (testX < width_ && testY < height_)
						{
							const uint8_t* const targetPatchMaskCenter = targetPatchMask + testY * width_ + testX;

							// this check should be enlarged to the entire patch size (corners), however the center pixel test allows a smother border
							//if (mask[testY * layerWidth + testX])
							if (*targetPatchMaskCenter == 0xFF)
							{
								const unsigned int ssd = calculateSSD24Bit(frame, width_, x, y, testX, testY, patchHalf);
								const unsigned int sqrDistance = 3 * sqrLength(testX - x, testY - y);
								OCEAN_SUPPRESS_UNUSED_WARNING(sqrDistance);

								//if (ssd + sqrDistance < patch->ssd() + patch->sqrDistance())
								if (ssd < patch->ssd())
									*patch = Patch(testX, testY, ssd, 0);
							}
						}
					}

					// right propagation
					const Patch* const patchRight = patch + 1;

					if (patchRight->ssd() != 0xFFFFFFFF)
					{
						//const unsigned int testX = max(patchHalf, patchRight->x() - 1);
						const unsigned int testX =  patchRight->x() - 1;
						const unsigned int testY = patchRight->y();

						if (testX < width_ && testY < height_)
						{
							const uint8_t* const targetPatchMaskCenter = targetPatchMask + testY * width_ + testX;

							// this check should be enlarged to the entire patch size (corners), however the center pixel test allows a smother border
							//if (mask[testY * layerWidth + testX])
							if (*targetPatchMaskCenter == 0xFF)
							{
								const unsigned int ssd = calculateSSD24Bit(frame, width_, x, y, testX, testY, patchHalf);
								const unsigned int sqrDistance = 3 * sqrLength(testX - x, testY - y);
								OCEAN_SUPPRESS_UNUSED_WARNING(sqrDistance);

								//if (ssd + sqrDistance < patch->ssd() + patch->sqrDistance())
								if (ssd < patch->ssd())
									*patch = Patch(testX, testY, ssd, 0);
							}
						}
					}

					// random search
					unsigned int index = 0;
					while (index < 8)
					{
						const unsigned int testX = (unsigned int)(Scalar(patch->x()) + Scalar(width_) * searchAreaFactors[index] * Random::scalar(-1, 1));
						const unsigned int testY = (unsigned int)(Scalar(patch->y()) + Scalar(height_) * searchAreaFactors[index] * Random::scalar(-1, 1));

						if (testX < width_ && testY < height_)
						{

							/*const uint8_t* const maskUpper = mask + (testY - patchHalf) * layerWidth + testX - patchHalf;
							const uint8_t* const maskLower = maskUpper + (patchSize - 1) * layerWidth;

							if (testX >= patchHalf && testX < layerWidth - patchHalf && testY >= patchHalf && testY < layerHeight - patchHalf
								&& *maskUpper && *(maskUpper + patchSize) && *maskLower && *(maskLower + patchSize)) //&& mask[testY * layerWidth + testX]*/

							const uint8_t* const targetPatchMaskCenter = targetPatchMask + testY * width_ + testX;

							if (*targetPatchMaskCenter == 0xFF)
							{
								const unsigned int ssd = calculateSSD24Bit(frame, width_, x, y, testX, testY, patchHalf);
								//const unsigned int sqrDistance = 3 * sqrLength(testX - x, testY - y);

								Scalar distanceCost = 0;

								if (sourcePatchMask[y * width_ + x + 1] == 0)
								{
									const int xOffset = (layerPatches[y * width_ + x + 1].x() - testX) - 1;
									const int yOffset = layerPatches[y * width_ + x + 1].y() - testY;

									distanceCost += Numeric::sqrt(Scalar(xOffset * xOffset + yOffset * yOffset)) / Numeric::sqrt(Scalar(width_ * width_ + height_ * height_));
								}

								if (sourcePatchMask[(y + 1) * width_ + x] == 0)
								{
									const int xOffset = layerPatches[(y + 1) * width_ + x].x() - testX;
									const int yOffset = (layerPatches[(y + 1) * width_ + x].y() - testY) - 1;

									distanceCost += Numeric::sqrt(Scalar(xOffset * xOffset + yOffset * yOffset)) / Numeric::sqrt(Scalar(width_ * width_ + height_ * height_));
								}

								const unsigned int sqrDistance = 2 * (unsigned int)distanceCost * 255 * 255 * 4;


								if (ssd + sqrDistance < patch->ssd() + patch->sqrDistance())
								//if (ssd < patch->ssd())
									*patch = Patch(testX, testY, ssd, sqrDistance);
							}
						}

						//++index;
						++index;
					}
				}
	}
}

void ImageCompletionLayer_patchmask::clearAndAdopt(const unsigned int width, const unsigned int height, const uint8_t* mask, const unsigned int patchSize, const ImageCompletionLayer_patchmask& layer, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows)
{
	if (width != width_ || height != height_)
	{
		delete [] layerPatches;
		layerPatches = nullptr;

		width_ = width;
		height_ = height;

		if (width_ != 0 && height_ != 0)
		{
			layerPatches = new Patch[pixels()];
			ocean_assert(layerPatches != nullptr);
		}
	}

	for (unsigned int n = 0; n < pixels(); ++n)
		layerPatches[n] = Patch();

	adopt(&layer, mask, patchSize, firstColumn, numberColumns, firstRow, numberRows);
}

void ImageCompletionLayer_patchmask::clearAndAdopt(const unsigned int width, const unsigned int height, const uint8_t* mask, const unsigned int patchSize, const ImageCompletionLayer_patchmask& layer, Worker& worker, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows)
{
	if (width != width_ || height != height_)
	{
		delete [] layerPatches;
		layerPatches = nullptr;

		width_ = width;
		height_ = height;

		if (width_ != 0 && height_ != 0)
		{
			layerPatches = new Patch[pixels()];
			ocean_assert(layerPatches != nullptr);
		}
	}

	for (unsigned int n = 0; n < pixels(); ++n)
		layerPatches[n] = Patch();

	adopt(layer, mask, patchSize, worker, firstColumn, numberColumns, firstRow, min(firstRow + numberRows, height_) - firstRow);
}

void ImageCompletionLayer_patchmask::adopt(const ImageCompletionLayer_patchmask& layer, const uint8_t* mask, const unsigned int patchSize, Worker& worker)
{
	ocean_assert(mask != nullptr);

	worker.executeFunction(Worker::Function::create(*this, &ImageCompletionLayer_patchmask::adopt, &layer, mask, patchSize, 0u, width_, 0u, 0u), 0, height_, 5, 6);

	for (unsigned int n = 0; n < pixels(); ++n)
		ocean_assert(layerPatches[n].x() == 0xFFFFFFFF || layerPatches[n].x() < width());
}

void ImageCompletionLayer_patchmask::adopt(const ImageCompletionLayer_patchmask& layer, const uint8_t* mask, const unsigned int patchSize, Worker& worker, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(mask != nullptr);

	worker.executeFunction(Worker::Function::create(*this, &ImageCompletionLayer_patchmask::adopt, &layer, mask, patchSize, firstColumn, numberColumns, 0u, 0u), firstRow, numberRows, 5, 6);

	for (unsigned int n = 0; n < pixels(); ++n)
		ocean_assert(layerPatches[n].x() == 0xFFFFFFFF || layerPatches[n].x() < width());
}

void ImageCompletionLayer_patchmask::adopt(const ImageCompletionLayer_patchmask* layer, const uint8_t* mask, const unsigned int patchSize, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(layer);
	ocean_assert(layerPatches);
	ocean_assert(firstRow + numberRows <= height_);

	ocean_assert(patchSize == 0 || (patchSize % 2) == 1);
	const unsigned int patchHalf = patchSize >> 1;

	const unsigned int xSizeFactor = width_ / layer->width_;
	const unsigned int ySizeFactor = height_ / layer->height_;

	// **TODO** **HACK**
	Log::warning() << "**HACK**";
	//ocean_assert(layer->layerWidth * xSizeFactor == layerWidth);
	//ocean_assert(layer->layerHeight * ySizeFactor == layerHeight);

	const unsigned int lastPatchX = width_ - patchHalf - 1;
	const unsigned int lastPatchY = height_ - patchHalf - 1;

	const unsigned int endY = firstRow + numberRows;
	const unsigned int endX = firstColumn + numberColumns;

	if (ySizeFactor == 1 && xSizeFactor == 1)
	{
		for (unsigned int y = max(1u, firstRow); y < min(endY, height_ - 1); ++y)
		{
			Patch* const patchRow = layerPatches + y * width_;
			const uint8_t* const maskRow = mask + y * width_;

			for (unsigned int x = max(1u, firstColumn); x < min(endX, width_ - 1); ++x)
				if (maskRow[x] == 0)
				{
					Patch& newPatch = patchRow[x];

					const Patch& patch = layer->layerPatches[y * layer->width_ + x];
					if (patch.ssd() != 0xFFFFFFFF)
					{
						ocean_assert(patch.sqrDistance() != 0xFFFFFFFF);

						const unsigned int patchX = patch.x();
						const unsigned int patchY = patch.y();

						if (patchX >= patchHalf && patchX <= lastPatchX && patchY >= patchHalf && patchY <= lastPatchY)
						{
							newPatch = Patch(patchX, patchY, 10 * patch.ssd(), sqrLength(patchX - x, patchY - y));
							continue;
						}
					}

					const Patch& topPatch = layer->layerPatches[(y - 1) * layer->width_ + x];
					if (topPatch.ssd() != 0xFFFFFFFF)
					{
						ocean_assert(topPatch.sqrDistance() != 0xFFFFFFFF);

						const unsigned int patchX = topPatch.x();
						const unsigned int patchY = topPatch.y() - 1;

						if (patchX >= patchHalf && patchX <= lastPatchX && patchY >= patchHalf && patchY <= lastPatchY)
						{
							newPatch = Patch(patchX, patchY, 10 * topPatch.ssd(), sqrLength(patchX - x, patchY - y));
							continue;
						}
					}

					const Patch& leftPatch = layer->layerPatches[y * layer->width_ + x - 1];
					if (leftPatch.ssd() != 0xFFFFFFFF)
					{
						ocean_assert(leftPatch.sqrDistance() != 0xFFFFFFFF);

						const unsigned int patchX = leftPatch.x() - 1;
						const unsigned int patchY = leftPatch.y();

						if (patchX >= patchHalf && patchX <= lastPatchX && patchY >= patchHalf && patchY <= lastPatchY)
						{
							newPatch = Patch(patchX, patchY, 10 * leftPatch.ssd(), sqrLength(patchX - x, patchY - y));
							continue;
						}
					}

					const Patch& rightPatch = layer->layerPatches[y * layer->width_ + x + 1];
					if (rightPatch.ssd() != 0xFFFFFFFF)
					{
						ocean_assert(rightPatch.sqrDistance() != 0xFFFFFFFF);

						const unsigned int patchX = rightPatch.x() + 1;
						const unsigned int patchY = rightPatch.y();

						if (patchX >= patchHalf && patchX <= lastPatchX && patchY >= patchHalf && patchY <= lastPatchY)
						{
							newPatch = Patch(patchX, patchY, 10 * rightPatch.ssd(), sqrLength(patchX - x, patchY - y));
							continue;
						}
					}

					const Patch& bottomPatch = layer->layerPatches[(y + 1) * layer->width_ + x];
					if (bottomPatch.ssd() != 0xFFFFFFFF)
					{
						ocean_assert(bottomPatch.sqrDistance() != 0xFFFFFFFF);

						const unsigned int patchX = bottomPatch.x();
						const unsigned int patchY = bottomPatch.y() + 1;

						if (patchX >= patchHalf && patchX <= lastPatchX && patchY >= patchHalf && patchY <= lastPatchY)
						{
							newPatch = Patch(patchX, patchY, 10 * bottomPatch.ssd(), sqrLength(patchX - x, patchY - y));
							continue;
						}
					}

					//ocean_assert(x >= patchHalf && x <= lastPatchX);
					//ocean_assert(y >= patchHalf && y <= lastPatchY);

					//newPatch = Patch(x, y, 0xFFFFFFF0, 0xFFFFFFF0);
					newPatch = Patch(max(patchHalf, min(x, lastPatchX)), max(patchHalf, min(y, lastPatchY)), 0xFFFFFFF0, 0xFFFFFFF0);
				}
		}
	}
	else
	{
		for (unsigned int y = firstRow; y < endY; ++y)
		{
			const unsigned int adoptY = y / ySizeFactor;
			const unsigned int offsetY = y % ySizeFactor;

			Patch* const patchRow = layerPatches + y * width_;
			const uint8_t* const maskRow = mask + y * width_;

			for (unsigned int x = firstColumn; x < endX; ++x)
				if (maskRow[x] == 0)
				{
					const unsigned int adoptX = x / xSizeFactor;
					const unsigned int offsetX = x % xSizeFactor;

					Patch& newPatch = patchRow[x];

					const Patch& patch = layer->layerPatches[adoptY * layer->width_ + adoptX];
					if (patch.ssd() != 0xFFFFFFFF)
					{
						ocean_assert(patch.sqrDistance() != 0xFFFFFFFF);

						const unsigned int patchX = patch.x() * xSizeFactor + offsetX;
						const unsigned int patchY = patch.y() * ySizeFactor + offsetY;

						newPatch = Patch(patchX, patchY, 10 * patch.ssd(), sqrLength(patchX - x, patchY - y));
						continue;
					}

					if (adoptY > 0)
					{
						const Patch& topPatch = layer->layerPatches[(adoptY - 1) * layer->width_ + adoptX];
						if (topPatch.ssd() != 0xFFFFFFFF)
						{
							ocean_assert(topPatch.sqrDistance() != 0xFFFFFFFF);

							const unsigned int patchX = topPatch.x() * xSizeFactor + offsetX;
							const unsigned int patchY = (topPatch.y() - 1) * ySizeFactor + offsetY;

							newPatch = Patch(patchX, patchY, 10 * topPatch.ssd(), sqrLength(patchX - x, patchY - y));
							continue;
						}
					}

					if (adoptX > 0)
					{
						const Patch& leftPatch = layer->layerPatches[adoptY * layer->width_ + adoptX - 1];
						if (leftPatch.ssd() != 0xFFFFFFFF)
						{
							ocean_assert(leftPatch.sqrDistance() != 0xFFFFFFFF);

							const unsigned int patchX = (leftPatch.x() - 1) * xSizeFactor + offsetX;
							const unsigned int patchY = leftPatch.y() * ySizeFactor + offsetY;

							newPatch = Patch(patchX, patchY, 10 * leftPatch.ssd(), sqrLength(patchX - x, patchY - y));
							continue;
						}
					}

					if (adoptX + 1 < layer->width_)
					{
						const Patch& rightPatch = layer->layerPatches[adoptY * layer->width_ + adoptX + 1];
						if (rightPatch.ssd() != 0xFFFFFFFF)
						{
							ocean_assert(rightPatch.sqrDistance() != 0xFFFFFFFF);

							const unsigned int patchX = (rightPatch.x() + 1) * xSizeFactor + offsetX;
							const unsigned int patchY = rightPatch.y() * ySizeFactor + offsetY;

							newPatch = Patch(patchX, patchY, 10 * rightPatch.ssd(), sqrLength(patchX - x, patchY - y));
							continue;
						}
					}

					if (adoptY + 1 < layer->height_)
					{
						const Patch& bottomPatch = layer->layerPatches[(adoptY + 1) * layer->width_ + adoptX];
						if (bottomPatch.ssd() != 0xFFFFFFFF)
						{
							ocean_assert(bottomPatch.sqrDistance() != 0xFFFFFFFF);

							const unsigned int patchX = bottomPatch.x() * xSizeFactor + offsetX;
							const unsigned int patchY = (bottomPatch.y() + 1) * ySizeFactor + offsetY;

							newPatch = Patch(patchX, patchY, 10 * bottomPatch.ssd(), sqrLength(patchX - x, patchY - y));
							continue;
						}
					}

					//ocean_assert(x >= patchHalf && x <= lastPatchX);
					//ocean_assert(y >= patchHalf && y <= lastPatchY);

					newPatch = Patch(max(patchHalf, min(x, lastPatchX)), max(patchHalf, min(y, lastPatchY)), 0xFFFFFFF0, 0xFFFFFFF0);
				}
		}
	}
}

}

}

}
