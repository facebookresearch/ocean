/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/FrameShrinker.h"
#include "ocean/cv/FrameConverter.h"

#include "ocean/base/Memory.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace CV
{

bool FrameShrinker::downsampleByTwo11(const Frame& source, Frame& target, Worker* worker)
{
	ocean_assert(source.isValid() && &source != &target);

	if (source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
	{
		const unsigned int widthMultiple = FrameType::widthMultiple(source.pixelFormat());
		const unsigned int heightMultiple = FrameType::heightMultiple(source.pixelFormat());

		const unsigned int targetWidth = source.width() / 2u;
		const unsigned int targetHeight = source.height() / 2u;

		if (targetWidth % widthMultiple == 0u || targetHeight % heightMultiple == 0u)
		{
			if (!target.set(FrameType(source, targetWidth, targetHeight), false /*forceOwner*/, true /*forceWritable*/))
			{
				ocean_assert(false && "This should never happen!");
				return false;
			}

			for (unsigned int planeIndex = 0u; planeIndex < source.numberPlanes(); ++planeIndex)
			{
				downsampleByTwo8BitPerChannel11(source.constdata<uint8_t>(planeIndex), target.data<uint8_t>(planeIndex), source.planeWidth(planeIndex), source.planeHeight(planeIndex), source.planeChannels(planeIndex), source.paddingElements(planeIndex), target.paddingElements(planeIndex), worker);
			}

			return true;
		}
	}

	ocean_assert(false && "FrameShrinker: Invalid frame!");
	return false;
}

bool FrameShrinker::downsampleBinayMaskByTwo11(const Frame& source, Frame& target, const unsigned int threshold, Worker* worker)
{
	ocean_assert(source.isValid());
	ocean_assert(source.width() >= 2u && source.height() >= 2u);
	ocean_assert(threshold <= 255 * 4u);

	if (source.isValid() && source.width() >= 2u && source.height() >= 2u && source.numberPlanes() == 1u && source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
	{
		const unsigned int width_2 = source.width() / 2u;
		const unsigned int height_2 = source.height() / 2u;

		switch (source.channels())
		{
			case 1u:
			{
				if (!target.set(FrameType(source, width_2, height_2), false /*forceOwner*/, true /*forceWritable*/))
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				downsampleBinayMaskByTwo8BitPerChannel11(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), threshold, worker);
				return true;
			}
		}
	}

	ocean_assert(false && "FrameShrinker: Invalid frame!");
	return false;
}

bool FrameShrinker::downsampleByTwo14641(const Frame& source, Frame& target, Worker* worker)
{
	if (!source.isValid() || (source.width() < 2u && source.height() < 2u))
	{
		return false;
	}

	if (target.isValid())
	{
		// downsampling supports e.g., 640x480 -> 320x240 or 641x481 -> 321x241

		if (((source.width() + 0u) / 2u != target.width() && (source.width() + 1u) / 2u != target.width())
			|| ((source.height() + 0u) / 2u != target.height() && (source.height() + 1u) / 2u != target.height()))
		{
			return false;
		}
	}

	if (source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8 && source.numberPlanes() == 1u)
	{
		const unsigned int targetWidth = target.isValid() ? target.width() : (source.width() / 2u);
		const unsigned int targetHeight = target.isValid() ? target.height() : (source.height() / 2u);

		ocean_assert(targetWidth >= 1u && targetHeight >= 1u);

		const unsigned int widthMultiple = FrameType::widthMultiple(source.pixelFormat());
		const unsigned int heightMultiple = FrameType::heightMultiple(source.pixelFormat());

		if (targetWidth % widthMultiple == 0u || targetHeight % heightMultiple == 0u)
		{
			if (!target.set(FrameType(targetWidth, targetHeight, source.pixelFormat(), source.pixelOrigin()), false /*forceOwner*/, true /*forceWritable*/))
			{
				ocean_assert(false && "This should never happen!");
				return false;
			}

			for (unsigned int planeIndex = 0u; planeIndex < source.numberPlanes(); ++planeIndex)
			{
				downsampleByTwo8BitPerChannel14641(source.constdata<uint8_t>(planeIndex), target.data<uint8_t>(planeIndex), source.planeWidth(planeIndex), source.planeHeight(planeIndex), target.planeWidth(planeIndex), target.planeHeight(planeIndex), source.planeChannels(planeIndex), source.paddingElements(planeIndex), target.paddingElements(planeIndex), worker);
			}

			return true;
		}
	}

	ocean_assert(false && "FrameShrinker: Invalid frame!");
	return false;
}

bool FrameShrinker::pyramidByTwo11(const Frame& source, uint8_t* const pyramidTarget, const size_t pyramidTargetSize, const unsigned int layers, const bool copyFirstLayer, Worker* worker)
{
	ocean_assert(source.isValid());
	ocean_assert(layers >= 1u);

	if (!source.isValid() || source.numberPlanes() != 1u || source.dataType() != FrameType::DT_UNSIGNED_INTEGER_8)
	{
		ocean_assert(false && "FrameShrinker: Invalid frame!");
		return false;
	}

	if (!copyFirstLayer)
	{
		if (layers == 1u || source.width() == 1u || source.height() == 1u)
		{
			// nothing to do here

			return true;
		}
	}

	ocean_assert(pyramidTarget != nullptr);

	if (pyramidTarget == nullptr)
	{
		ocean_assert(false && "Invalid pyramid memory!");
		return false;
	}

	return pyramidByTwo8BitPerChannel11(source.constdata<uint8_t>(), pyramidTarget, source.width(), source.height(), source.channels(), pyramidTargetSize, layers, source.paddingElements(), copyFirstLayer, worker);
}

bool FrameShrinker::pyramidByTwo8BitPerChannel11(const uint8_t* source, uint8_t* pyramidTarget, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int channels, const size_t pyramidTargetSize, const unsigned int layers, const unsigned int sourcePaddingElements, const bool copyFirstLayer, Worker* worker)
{
	ocean_assert(source != nullptr);
	ocean_assert(sourceWidth >= 1u && sourceHeight >= 1u);
	ocean_assert(channels >= 1u);
	ocean_assert(layers >= 1u);

	if (!copyFirstLayer)
	{
		if (layers == 1u || sourceWidth == 1u || sourceHeight == 1u)
		{
			// nothing to do here

			return true;
		}
	}

	ocean_assert(pyramidTarget != nullptr);

	if (source == nullptr || sourceWidth == 0u || sourceHeight == 0u)
	{
		ocean_assert(false && "FrameShrinker: Invalid frame!");
		return false;
	}

	// if a worker is provided we try to invoke the pyramid calculation as efficient as possible
	// we could invoke a new multi-thread execution on each pyramid layer, once the previous layer has been processed successfully
	// or we can assign a specific image region to each thread which then starts at the finest layer and reaches almost the coarsest layer
	// the later option avoid the synchronization between the individual threads after each pyramid layer has been processed

	if (worker != nullptr && *worker)
	{
		// obviously we must not use more than sourceHeight/2 threads
		unsigned int availableThreads = min(worker->threads(), sourceHeight / 2u);

		// first, we determine the number of threads that should be used to process the pyramid
		// we try to find a configuration that ensures an equally distributed data load for each thread, as long as we have more than two threads

		if (availableThreads >= 3u)
		{
			const unsigned int firstSubsetsSourceHeight = 1u << ((unsigned int)(NumericF::log2(float(sourceHeight) / (2.0f * float(availableThreads)))) + 1u);
			const unsigned int firstSubsetsSourceHeightNextLarger = firstSubsetsSourceHeight * 2u;

			ocean_assert(firstSubsetsSourceHeightNextLarger <= sourceHeight);

			const unsigned int threadsNextLarger = (unsigned int)(Numeric::ceil(float(sourceHeight) / float(firstSubsetsSourceHeightNextLarger)));
			ocean_assert(threadsNextLarger <= availableThreads);

			if (firstSubsetsSourceHeight * (availableThreads - 1u) < firstSubsetsSourceHeightNextLarger * (threadsNextLarger - 1u))
			{
				availableThreads = threadsNextLarger;
			}
		}

		if (availableThreads > 1u)
		{
			// second, we determine the maximal number of layers that can be process in parallel (we need at least one row for each thread on the last multi-thread layer)
			// therefore, find largest 'multiThreadLayers' so that the following holds: 2 ^ (multiThreadLayers - 1) * threads <= finestHeight

			const unsigned int multiThreadLayers = std::min(layers, (unsigned int)(NumericF::log2(float(sourceHeight) / float(availableThreads))) + 1u);

			ocean_assert(multiThreadLayers == 0u || (availableThreads * 2u << (multiThreadLayers - 2u) <= sourceHeight));

			unsigned int layerHeight = sourceHeight;
			unsigned int layerWidth = sourceWidth;
			size_t layersOffset = 0;

			if (multiThreadLayers >= 1u)
			{
				ocean_assert(availableThreads >= 2u);

				if (!pyramidByTwo8BitPerChannel11WithThreads(source, pyramidTarget, sourceWidth, sourceHeight, channels, pyramidTargetSize, multiThreadLayers, copyFirstLayer, sourcePaddingElements, availableThreads, worker))
				{
					return false;
				}

				if (layers == multiThreadLayers)
				{
					// we have processed all necessary layers
					return true;
				}

				for (unsigned int i = 1u; i < multiThreadLayers; ++i)
				{
					const uint64_t newLayerOffset = uint64_t(layersOffset) + uint64_t(layerHeight * layerWidth * channels);

					if (!NumericT<size_t>::isInsideValueRange(newLayerOffset))
					{
						ocean_assert(false && "This should never happen!");
						return false;
					}

					layersOffset = size_t(newLayerOffset);
					layerHeight /= 2u;
					layerWidth /= 2u;
				}
			}

			if (layerWidth <= 1u || layerHeight <= 1u)
			{
				return true;
			}

			// now we process the last layers single threaded

			const unsigned int sourcePyramidLayerPaddingElements = multiThreadLayers == 0u ? sourcePaddingElements : 0u;

			if (!copyFirstLayer)
			{
				// the finest layer is not part of the memory, so we must not count this layer

				ocean_assert(size_t(layerHeight * layerWidth * channels) < layersOffset);
				layersOffset -= size_t(sourceHeight * sourceWidth * channels);
			}

			ocean_assert(layersOffset < pyramidTargetSize);

			const uint64_t coarserLayerOffset = uint64_t(layersOffset) + uint64_t(layerHeight * layerWidth * channels);

			if (!NumericT<size_t>::isInsideValueRange(coarserLayerOffset))
			{
				ocean_assert(false && "This should never happen!");
				return false;
			}

			ocean_assert(layers > multiThreadLayers || multiThreadLayers == 0u);
			return pyramidByTwo8BitPerChannel11WithThreads(pyramidTarget + layersOffset, pyramidTarget + coarserLayerOffset, layerWidth, layerHeight, channels, pyramidTargetSize - layersOffset, layers - multiThreadLayers + 1u, false /*copyFirstLayer*/, sourcePyramidLayerPaddingElements, 1u /*threads*/, nullptr /*worker*/);
		}
	}

	return pyramidByTwo8BitPerChannel11WithThreads(source, pyramidTarget, sourceWidth, sourceHeight, channels, pyramidTargetSize, layers, copyFirstLayer, sourcePaddingElements, 1u /*threads*/, nullptr /*worker*/);
}

bool FrameShrinker::pyramidByTwo8BitPerChannel11WithThreads(const uint8_t* source, uint8_t* pyramidTarget, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int channels, const size_t pyramidTargetSize, const unsigned int layers, const bool copyFirstLayer, const unsigned int sourcePaddingElements, const unsigned int threads, Worker* worker)
{
	ocean_assert(source != nullptr && pyramidTarget != nullptr );
	ocean_assert(sourceWidth >= 1u && sourceHeight >= 1u && layers >= 1u);
	ocean_assert(channels >= 1u);
	ocean_assert(pyramidTargetSize > 0);
	ocean_assert(threads >= 1u);

	// we have to ensure that each thread stays in it's sub-region

	if (worker != nullptr && threads >= 2u)
	{
		ocean_assert(threads <= worker->threads());
		ocean_assert(layers == 0u || (threads * 2u << (layers - 2u) <= sourceHeight));

		// now we determine the height of the first threads-1 subsets, the last subset will take the remaining image content

		const unsigned int firstSubsetsSourceHeight = 1u << ((unsigned int)NumericF::log2(float(sourceHeight) / (2.0f * float(threads))) + 1u);

#ifdef OCEAN_DEBUG
		{
			unsigned int debugFirstSubsetsSourceHeight = 1u;
			while (debugFirstSubsetsSourceHeight * threads <= sourceHeight / 2u)
			{
				debugFirstSubsetsSourceHeight *= 2u;
			}

			ocean_assert(firstSubsetsSourceHeight == debugFirstSubsetsSourceHeight);
		}
#endif

		ocean_assert(Utilities::isPowerOfTwo(firstSubsetsSourceHeight));
		ocean_assert(firstSubsetsSourceHeight * (threads - 1u) < sourceHeight);

		worker->executeFunction(Worker::Function::createStatic(&pyramidByTwo8BitPerChannel11WithThreadsSubset, source, pyramidTarget, sourceWidth, sourceHeight, channels, pyramidTargetSize, layers, copyFirstLayer, sourcePaddingElements, firstSubsetsSourceHeight, threads, 0u, 0u), 0u, threads);
	}
	else
	{
		pyramidByTwo8BitPerChannel11WithThreadsSubset(source, pyramidTarget, sourceWidth, sourceHeight, channels, pyramidTargetSize, layers, copyFirstLayer, sourcePaddingElements, sourceHeight, 1u, 0u, 1u);
	}

	return true;
}

void FrameShrinker::pyramidByTwo8BitPerChannel11WithThreadsSubset(const uint8_t* source, uint8_t* pyramidTarget, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int channels, const size_t pyramidTargetSize, const unsigned int layers, const bool copyFirstLayer, const unsigned int sourcePaddingElements, const unsigned int firstSubsetsSourceHeight, const unsigned int subsets, const unsigned int subsetIndex, const unsigned int valueOne)
{
	ocean_assert(source != nullptr && pyramidTarget != nullptr);
	ocean_assert(sourceWidth >= 1u && sourceHeight >= 1u && layers >= 1u);
	ocean_assert(channels >= 1u);
	ocean_assert(pyramidTargetSize > 0);

	ocean_assert(subsetIndex < subsets);
	ocean_assert_and_suppress_unused(valueOne == 1u, valueOne);
	ocean_assert((subsets == 1u && firstSubsetsSourceHeight == sourceHeight) || (subsets > 1u && firstSubsetsSourceHeight * (subsets - 1u) < sourceHeight));

	unsigned int subsetSourceFirstRow = firstSubsetsSourceHeight * subsetIndex;
	unsigned int subsetSourceRows = firstSubsetsSourceHeight;

	// all subsets but the last one have the same size, the last subset will process the remaining information
	ocean_assert(subsets == 1u || Utilities::isPowerOfTwo(firstSubsetsSourceHeight));

	if (subsets > 1u && subsetIndex == subsets - 1u)
	{
		subsetSourceRows = sourceHeight - firstSubsetsSourceHeight * (subsets - 1u);
		ocean_assert(subsetSourceRows >= 1u);
	}

	size_t remainingPyramidTargetSize = pyramidTargetSize;

	const uint8_t* currentPyramidLayer = source;
	uint8_t* nextPyramidLayer = pyramidTarget;

	unsigned int currentPyramidLayerPaddingElements = sourcePaddingElements;

	if (copyFirstLayer)
	{
		// for the first layer we simply copy the content

		const size_t sizeFirstLayer = size_t(sourceWidth * sourceHeight * channels) * sizeof(uint8_t);

		if (remainingPyramidTargetSize < sizeFirstLayer)
		{
			ocean_assert(false && "This should never happen!");
			return;
		}

		constexpr unsigned int targetPaddingElements = 0u;
		CV::FrameConverter::subFrame<uint8_t>(currentPyramidLayer, nextPyramidLayer, sourceWidth, sourceHeight, sourceWidth, sourceHeight, channels, 0u, subsetSourceFirstRow, 0u, subsetSourceFirstRow, sourceWidth, subsetSourceRows, currentPyramidLayerPaddingElements, targetPaddingElements);

		remainingPyramidTargetSize -= sizeFirstLayer;

		currentPyramidLayer = nextPyramidLayer;
		nextPyramidLayer += sourceWidth * sourceHeight * channels;

		currentPyramidLayerPaddingElements = targetPaddingElements;
	}

	// now we proceed the remaining layers, and stay in the same subset of the frame

	unsigned int layerWidth = sourceWidth;
	unsigned int layerHeight = sourceHeight;

	for (unsigned int n = 1u; n < layers; ++n)
	{
		if (subsetSourceRows <= 1u)
		{
			ocean_assert(subsetSourceRows == 1u && subsetIndex == subsets - 1u);
			break;
		}

		if (layerWidth <= 1u)
		{
			ocean_assert(layerWidth == 1u);
			break;
		}

		ocean_assert((subsetSourceFirstRow % 2u) == 0u);

		// we need an even number of source rows unless we are the last subset
		ocean_assert((subsetSourceRows % 2u) == 0u || subsetIndex == subsets - 1u);
		subsetSourceFirstRow /= 2u;
		subsetSourceRows /= 2u;

		const unsigned int targetLayerWidth = layerWidth / 2u;
		const unsigned int targetLayerHeight = layerHeight / 2u;

		const size_t sizeTargetLayer = size_t(targetLayerWidth * targetLayerHeight * channels) * sizeof(uint8_t);

		if (remainingPyramidTargetSize < sizeTargetLayer)
		{
			ocean_assert(false && "This should never happen!");
			return;
		}

		constexpr unsigned int nextPyramidLayerPaddingElements = 0u;

		downsampleByTwo8BitPerChannel11Subset(currentPyramidLayer, nextPyramidLayer, layerWidth, layerHeight, channels, currentPyramidLayerPaddingElements, nextPyramidLayerPaddingElements, subsetSourceFirstRow, subsetSourceRows);

		currentPyramidLayer = nextPyramidLayer;
		nextPyramidLayer += targetLayerWidth * targetLayerHeight * channels;

		currentPyramidLayerPaddingElements = nextPyramidLayerPaddingElements;

		layerWidth = targetLayerWidth;
		layerHeight = targetLayerHeight;

		remainingPyramidTargetSize -= sizeTargetLayer;
	}
}

void FrameShrinker::downsampleByTwo8BitPerChannel11Subset(const uint8_t* source, uint8_t* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int channels, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstTargetRow, const unsigned int numberTargetRows)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(sourceWidth >= 2u && sourceHeight >= 2u);
	ocean_assert(channels >= 1u);

	DownsampleBlockByTwo8BitPerChannelFunction downsampleBlockFunction = nullptr;
	unsigned int sourceElementsPerBlock = 0u;
	determineFunctionDownsampleBlockByTwo8Bit11(sourceWidth, channels, downsampleBlockFunction, sourceElementsPerBlock);

#ifdef OCEAN_DEBUG
	const uint8_t* const debugSource = source;
	const uint8_t* const debugTarget = target;
#endif

	const unsigned int targetWidth = sourceWidth / 2u;
	const unsigned int targetHeight = sourceHeight / 2u;

	ocean_assert(firstTargetRow + numberTargetRows <= targetHeight);

	const unsigned int sourceElements = sourceWidth * channels;
	ocean_assert_and_suppress_unused(sourceElements >= sourceElementsPerBlock && "The block size must not be larger than the number of elements in one row", sourceElements);

	const unsigned int targetRowElements = targetWidth * channels;

	const unsigned int targetElementsPerBlock = sourceElementsPerBlock / 2u;
	ocean_assert(targetElementsPerBlock * 2u == sourceElementsPerBlock);

	const unsigned int sourceStrideElements = sourceWidth * channels + sourcePaddingElements;
	const unsigned int targetStrideElements = targetWidth * channels + targetPaddingElements;

	const bool sourceWidthIsOdd = (sourceWidth & 0x01u) != 0u;
	const bool sourceHeightIsOdd = (sourceHeight & 0x01u) != 0u;
	const bool subsetContainsLastOddRow = sourceHeightIsOdd && (firstTargetRow + numberTargetRows == targetHeight);

	source += sourceStrideElements * firstTargetRow * 2u;
	target += targetStrideElements * firstTargetRow;

	const unsigned int numberEvenTargetRows = subsetContainsLastOddRow ? (numberTargetRows - 1u) : numberTargetRows;

	const uint8_t* const targetEnd = target + targetStrideElements * numberEvenTargetRows;

	while (target != targetEnd)
	{
		ocean_assert(target < targetEnd);

		ocean_assert((size_t(source) - size_t(debugSource)) % sourceStrideElements == 0);
		ocean_assert((size_t(target) - size_t(debugTarget)) % targetStrideElements == 0);

		if (targetElementsPerBlock == 0u)
		{
			// we do not have a function to down sample a block of elements

			const uint8_t* source1 = source + sourceStrideElements;

			for (unsigned int n = 0u; n < targetWidth; ++n)
			{
				for (unsigned int c = 0u; c < channels; ++c)
				{
					target[c] = (source[c] + source[channels + c] + source1[c] + source1[channels + c] + 2u) / 4u;
				}

				target += channels;
				source += channels * 2u;
				source1 += channels * 2u;
			}
		}
		else
		{
			ocean_assert(targetRowElements >= targetElementsPerBlock);

			for (unsigned int n = 0u; n < targetRowElements; n += targetElementsPerBlock)
			{
				if (n + targetElementsPerBlock > targetRowElements)
				{
					// the last iteration does not fit anymore
					// we can simply shift left by some elements (at most 'targetBlockElements' - 1)

					ocean_assert(n >= targetElementsPerBlock && targetRowElements > targetElementsPerBlock);
					const unsigned int newN = targetRowElements - targetElementsPerBlock;

					ocean_assert(n > newN);
					const unsigned int targetOffset = n - newN;

					source -= targetOffset * 2u;
					target -= targetOffset;

					n = newN;

					// the for loop will stop after this iteration
					ocean_assert(!(n + targetElementsPerBlock < targetRowElements));
				}

				downsampleBlockFunction(source, source + sourceStrideElements, target);

				source += sourceElementsPerBlock;
				target += targetElementsPerBlock;
			}
		}

		if (sourceWidthIsOdd)
		{
			// In case, the width is odd, we apply a 1-2-1 downsampling for the last three columns
			downsampleByTwoOneRowThreeColumns8BitPerChannel121(source - channels * 2u, target - channels, channels, sourceStrideElements);

			source += channels; // we have to move one additional column
		}

		ocean_assert(source <= debugSource + sourceHeight * sourceStrideElements);
		ocean_assert(target <= debugTarget + targetHeight * targetStrideElements);

		source += sourcePaddingElements + sourceStrideElements;
		target += targetPaddingElements;
	}

	if (subsetContainsLastOddRow)
	{
		ocean_assert(source == debugSource + (sourceHeight - 3u) * sourceStrideElements);
		ocean_assert(target == debugTarget + (targetHeight - 1u) * targetStrideElements);

		// In case, the height is odd, and in case this subset contains the last row, we apply a 1-2-1 downsampling for the last tree rows
		downsampleByTwoThreeRows8BitPerChannel121(source, target, sourceWidth, channels, sourceStrideElements);
	}
}

void FrameShrinker::determineFunctionDownsampleBlockByTwo8Bit11(const unsigned int sourceWidth, const unsigned int channels, DownsampleBlockByTwo8BitPerChannelFunction& downsampleBlockFunction, unsigned int& sourceElementsPerBlock)
{
	downsampleBlockFunction = nullptr;
	sourceElementsPerBlock = 0u;

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

	const unsigned int sourceElements = sourceWidth * channels;

	switch (channels)
	{
		case 1u:
			if (sourceElements >= 32u)
			{
				downsampleBlockFunction = SSE::average32Elements1Channel8Bit2x2;
				sourceElementsPerBlock = 32u;
			}
			else if (sourceElements >= 16u)
			{
				downsampleBlockFunction = SSE::average16Elements1Channel8Bit2x2;
				sourceElementsPerBlock = 16u;
			}
			else if (sourceElements >= 8u)
			{
				downsampleBlockFunction = SSE::average8Elements1Channel8Bit2x2;
				sourceElementsPerBlock = 8u;
			}
			break;

		case 2u:
			if (sourceElements >= 32u)
			{
				downsampleBlockFunction = SSE::average32Elements2Channel16Bit2x2;
				sourceElementsPerBlock = 32u;
			}
			else if (sourceElements >= 16u)
			{
				downsampleBlockFunction = SSE::average16Elements2Channel16Bit2x2;
				sourceElementsPerBlock = 16u;
			}
			else if (sourceElements >= 8u)
			{
				downsampleBlockFunction = SSE::average8Elements2Channel16Bit2x2;
				sourceElementsPerBlock = 8u;
			}
			break;

		case 3u:
			if (sourceElements >= 24u)
			{
				downsampleBlockFunction = SSE::average24Elements3Channel24Bit2x2;
				sourceElementsPerBlock = 24u;
			}
			break;

		case 4u:
			if (sourceElements >= 32u)
			{
				downsampleBlockFunction = SSE::average32Elements4Channel32Bit2x2;
				sourceElementsPerBlock = 32u;
			}
			else if (sourceElements >= 16u)
			{
				downsampleBlockFunction = SSE::average16Elements4Channel32Bit2x2;
				sourceElementsPerBlock = 16u;
			}
			break;
	}

#elif defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	const unsigned int sourceElements = sourceWidth * channels;

	switch (channels)
	{
		case 1u:
			if (sourceElements >= 32u)
			{
				downsampleBlockFunction = NEON::average32Elements1Channel8Bit2x2;
				sourceElementsPerBlock = 32u;
			}
			else if (sourceElements >= 16u)
			{
				downsampleBlockFunction = NEON::average16Elements1Channel8Bit2x2;
				sourceElementsPerBlock = 16u;
			}
			break;

		case 2u:
			if (sourceElements >= 64u)
			{
				downsampleBlockFunction = NEON::average64Elements2Channel16Bit2x2;
				sourceElementsPerBlock = 64u;
			}
			else if (sourceElements >= 32u)
			{
				downsampleBlockFunction = NEON::average32Elements2Channel16Bit2x2;
				sourceElementsPerBlock = 32u;
			}
			break;

		case 3u:
			if (sourceElements >= 48u)
			{
				downsampleBlockFunction = NEON::average48Elements3Channel24Bit2x2;
				sourceElementsPerBlock = 48u;
			}
			break;

		case 4u:
			if (sourceElements >= 64u)
			{
				downsampleBlockFunction = NEON::average64Elements4Channel32Bit2x2;
				sourceElementsPerBlock = 64u;
			}
			break;
	}

#else

	OCEAN_SUPPRESS_UNUSED_WARNING(sourceWidth);
	OCEAN_SUPPRESS_UNUSED_WARNING(channels);

#endif
}

void FrameShrinker::downsampleBinayMaskByTwo8BitPerChannel11Subset(const uint8_t* source, uint8_t* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int threshold, const unsigned int firstTargetRow, const unsigned int numberTargetRows)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(sourceWidth >= 2u && sourceHeight >= 2u);

	DownsampleBlockByTwoBinary8BitPerChannelFunction downsampleBlockFunction = nullptr;
	unsigned int sourceElementsPerBlock = 0u;

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

	if (sourceWidth >= 32u)
	{
		downsampleBlockFunction = SSE::average32ElementsBinary1Channel8Bit2x2;
		sourceElementsPerBlock = 32u;
	}
	else if (sourceWidth >= 16u)
	{
		downsampleBlockFunction = SSE::average16ElementsBinary1Channel8Bit2x2;
		sourceElementsPerBlock = 16u;
	}
	else if (sourceWidth >= 8u)
	{
		downsampleBlockFunction = SSE::average8ElementsBinary1Channel8Bit2x2;
		sourceElementsPerBlock = 8u;
	}

#endif // OCEAN_HARDWARE_SSE_VERSION

#ifdef OCEAN_DEBUG
	const uint8_t* const debugSource = source;
	const uint8_t* const debugTarget = target;
#endif

	const unsigned int targetWidth = sourceWidth / 2u;
	const unsigned int targetHeight = sourceHeight / 2u;

	ocean_assert(firstTargetRow + numberTargetRows <= targetHeight);

	ocean_assert(sourceWidth >= sourceElementsPerBlock && "The block size must not be larger than the number of elements in one row");

	const unsigned int targetElementsPerBlock = sourceElementsPerBlock / 2u;
	ocean_assert(targetElementsPerBlock * 2u == sourceElementsPerBlock);

	const unsigned int sourceStrideElements = sourceWidth + sourcePaddingElements;
	const unsigned int targetStrideElements = targetWidth + targetPaddingElements;

	const bool sourceWidthIsOdd = (sourceWidth & 0x01u) != 0u;
	const bool sourceHeightIsOdd = (sourceHeight & 0x01u) != 0u;
	const bool subsetContainsLastOddRow = sourceHeightIsOdd && (firstTargetRow + numberTargetRows == targetHeight);

	source += sourceStrideElements * firstTargetRow * 2u;
	target += targetStrideElements * firstTargetRow;

	const unsigned int numberEvenTargetRows = subsetContainsLastOddRow ? (numberTargetRows - 1u) : numberTargetRows;

	const uint8_t* const targetEnd = target + targetStrideElements * numberEvenTargetRows;

	while (target != targetEnd)
	{
		ocean_assert(target < targetEnd);

		ocean_assert((size_t(source) - size_t(debugSource)) % sourceStrideElements == 0);
		ocean_assert((size_t(target) - size_t(debugTarget)) % targetStrideElements == 0);

		if (targetElementsPerBlock == 0u)
		{
			// we do not have a function to down sample a block of elements

			const uint8_t* source1 = source + sourceStrideElements;

			for (unsigned int n = 0u; n < targetWidth; ++n)
			{
				const unsigned int value = source[0] + source[1] + source1[0] + source1[1];

				*target++ = value >= threshold ? 0xFF : 0x00;

				source += 2u;
				source1 += 2u;
			}
		}
		else
		{
			ocean_assert(targetWidth >= targetElementsPerBlock);

			for (unsigned int n = 0u; n < targetWidth; n += targetElementsPerBlock)
			{
				if (n + targetElementsPerBlock > targetWidth)
				{
					// the last iteration does not fit anymore
					// we can simply shift left by some elements (at most 'targetBlockElements' - 1)

					ocean_assert(n >= targetElementsPerBlock && targetWidth > targetElementsPerBlock);
					const unsigned int newN = targetWidth - targetElementsPerBlock;

					ocean_assert(n > newN);
					const unsigned int targetOffset = n - newN;

					source -= targetOffset * 2u;
					target -= targetOffset;

					n = newN;

					// the for loop will stop after this iteration
					ocean_assert(!(n + targetElementsPerBlock < targetWidth));
				}

				ocean_assert(threshold <= NumericT<uint16_t>::maxValue());
				downsampleBlockFunction(source, source + sourceStrideElements, target, uint16_t(threshold));

				source += sourceElementsPerBlock;
				target += targetElementsPerBlock;
			}
		}

		if (sourceWidthIsOdd)
		{
			// three pixels left

			// source0: | 1 2 1 |
			// source1: | 1 2 1 | / 8

			const uint8_t* source1 = source + sourceStrideElements;

			const unsigned int value = (*(source - 2) + *(source - 1) * 2u + *(source + 0) + *(source1 - 2) + *(source1 - 1) * 2u + *(source1 + 0));

			*(target - 1) = value >= 2u * threshold ? 0xFFu : 0x00u;

			++source;
		}

		ocean_assert(source <= debugSource + sourceHeight * sourceStrideElements);
		ocean_assert(target <= debugTarget + targetHeight * targetStrideElements);

		source += sourcePaddingElements + sourceStrideElements;
		target += targetPaddingElements;
	}

	if (subsetContainsLastOddRow)
	{
		ocean_assert(source == debugSource + (sourceHeight - 3u) * sourceStrideElements);
		ocean_assert(target == debugTarget + (targetHeight - 1u) * targetStrideElements);

		const uint8_t* source1 = source + sourceStrideElements;
		const uint8_t* source2 = source + sourceStrideElements * 2u;

		for (unsigned int n = 0u; n < targetWidth; ++n)
		{
			// source0: | 1 1 |
			// source1: | 2 2 | / 8
			// source2: | 1 1 |

			const unsigned int value = (*(source + 0) + *(source + 1) + *(source1 + 0) * 2u + *(source1 + 1) * 2u + *(source2 + 0) + *(source2 + 1));

			*target++ = value >= 2u * threshold ? 0xFFu : 0x00u;

			source += 2;
			source1 += 2;
			source2 += 2;
		}

		if (sourceWidthIsOdd)
		{
			// three pixels left

			// source0: | 1 2 1 |
			// source1: | 2 4 2 | / 16
			// source2: | 1 2 1 |

			const unsigned int value = (*(source - 2) + *(source - 1) * 2u + *(source + 0) + *(source1 - 2) * 2u + *(source1 - 1) * 4u + *(source1 + 0) * 2u + *(source2 - 2) + *(source2 - 1) * 2u + *(source2 + 0));

			*(target - 1) = value >= 4u * threshold ? 0xFFu : 0x00u;
		}
	}
}

OCEAN_PREVENT_INLINE void FrameShrinker::downsampleByTwoOneRowThreeColumns8BitPerChannel121(const uint8_t* source, uint8_t* target, const unsigned int channels, const unsigned int sourceStrideElements)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(channels >= 1u);
	ocean_assert(sourceStrideElements >= channels * 3u);

	// three pixels at the right border

	// source0: | 1 2 1 |
	// source1: | 1 2 1 | / 8

	const uint8_t* source0 = source;
	const uint8_t* source1 = source + sourceStrideElements;

	for (unsigned int c = 0u; c < channels; ++c)
	{
		target[c] = (source0[c] + source0[channels + c] * 2u + source0[2u * channels + c] + source1[c] + source1[channels + c] * 2u + source1[2u * channels + c] + 4u) / 8u;
	}
}

OCEAN_PREVENT_INLINE void FrameShrinker::downsampleByTwoThreeRows8BitPerChannel121(const uint8_t* source, uint8_t* target, const unsigned int sourceWidth, const unsigned int channels, const unsigned int sourceStrideElements)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(sourceWidth >= 1u);
	ocean_assert(channels >= 1u);

	ocean_assert(sourceStrideElements >= sourceWidth * channels);

	const uint8_t* source0 = source;
	const uint8_t* source1 = source + sourceStrideElements;
	const uint8_t* source2 = source + sourceStrideElements * 2u;

	if (sourceWidth == 1u)
	{
		// source0: | 1 |
		// source1: | 2 | / 4
		// source2: | 1 |

		for (unsigned int c = 0u; c < channels; ++c)
		{
			target[c] = (source0[c] + source1[c] * 2u + source2[c] + 2u) / 4u;
		}

		return;
	}

	const unsigned int oddTargetPixel = sourceWidth & 0x01u;
	const unsigned int evenTargetPixels = (sourceWidth / 2u) - oddTargetPixel;

	for (unsigned int tx = 0u; tx < evenTargetPixels; ++tx)
	{
		// source0: | 1 1 |
		// source1: | 2 2 | / 8
		// source2: | 1 1 |

		for (unsigned int c = 0u; c < channels; ++c)
		{
			target[c] = (source0[c] + source0[channels + c] + (source1[c] + source1[channels + c]) * 2u + source2[c] + source2[channels + c] + 4u) / 8u;
		}

		target += channels;
		source0 += channels * 2u;
		source1 += channels * 2u;
		source2 += channels * 2u;
	}

	if (oddTargetPixel)
	{
		// three pixels at the right border

		// source0: | 1 2 1 |
		// source1: | 2 4 2 | / 16
		// source2: | 1 2 1 |

		for (unsigned int c = 0u; c < channels; ++c)
		{
			target[c] = (source0[c] + source0[channels + c] * 2u + source0[2u * channels + c] + (source1[c] + source1[channels + c] * 2u + source1[2u * channels + c]) * 2u + source2[c] + source2[channels + c] * 2u + source2[2u * channels + c] + 8u) / 16u;
		}
	}
}

void FrameShrinker::downsampleByTwoRowVertical8BitPerChannel14641(const uint8_t* const source, uint16_t* targetRow, const unsigned int sourceElements, const unsigned int sourceHeight, const unsigned int sourceStride, const unsigned int ySource)
{
	ocean_assert(source != nullptr);
	ocean_assert(targetRow != nullptr);
	ocean_assert(sourceStride >= 1u && sourceHeight >= 2u);

	// the vertical filtering does not need to know anything about channels

	const uint8_t* source0 = source + sourceStride * mirroredBorderLocationLeft(int(ySource) - 2);
	const uint8_t* source1 = source + sourceStride * mirroredBorderLocationLeft(int(ySource) - 1);
	const uint8_t* source2 = source + sourceStride * ySource;
	const uint8_t* source3 = source + sourceStride * mirroredBorderLocationRight(ySource + 1u, sourceHeight);
	const uint8_t* source4 = source + sourceStride * mirroredBorderLocationRight(ySource + 2u, sourceHeight);

	for (unsigned int x = 0u; x < sourceElements; ++x)
	{
		*targetRow++ = uint16_t(*source0++ + (*source1++ + *source3++) * 4u + *source2++ * 6u + *source4++);
	}
}

void FrameShrinker::downsampleByTwoRowHorizontal8BitPerChannel14641(const uint16_t* sourceRow, uint8_t* targetRow, const unsigned int targetWidth, const unsigned int channels)
{
	ocean_assert(sourceRow != nullptr);
	ocean_assert(targetRow != nullptr);
	ocean_assert(targetWidth >= 1u);
	ocean_assert(channels >= 1u);

	for (unsigned int xTarget = 0u; xTarget < targetWidth; ++xTarget)
	{
		for (unsigned int n = 0u; n < channels; ++n)
		{
			targetRow[n] = (uint8_t)((sourceRow[channels * 0u + n] + (sourceRow[channels * 1u + n] + sourceRow[channels * 3u + n]) * 4u + sourceRow[channels * 2u + n] * 6u + sourceRow[channels * 4u + n] + 128u) / 256u);
		}

		targetRow += channels;
		sourceRow += channels * 2u;
	}
}

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

void FrameShrinker::downsampleByTwoRowVertical8BitPerChannel14641SSE(const uint8_t* const source, uint16_t* targetRow, const unsigned int sourceElements, const unsigned int sourceHeight, const unsigned int sourceStride, const unsigned int ySource)
{
	ocean_assert(source != nullptr);
	ocean_assert(targetRow != nullptr);
	ocean_assert(sourceStride >= 8u && sourceHeight >= 2u);

	/*
	 * We determine 8 filter responses within one loop iteration.
	 * For 1 channel frames, with row = 0, we apply the following mirroring strategy:
	 *
	 * Source Data:
	 *     1  Y 1 1 1 1 1 1 1 1
	 *     0  Y 4 4 4 4 4 4 4 4
	 *        -----------------
	 *     0  Y 6 6 6 6 6 6 6 6 <---------
	 *     1  Y 4 4 4 4 4 4 4 4
	 *     2  Y 1 1 1 1 1 1 1 1
	 *     3  Y
	 *
	 * For frames with n channels the strategy stays the same.
	 */

	const uint8_t* source0 = source + sourceStride * mirroredBorderLocationLeft(int(ySource) - 2);
	const uint8_t* source1 = source + sourceStride * mirroredBorderLocationLeft(int(ySource) - 1);
	const uint8_t* source2 = source + sourceStride * ySource;
	const uint8_t* source3 = source + sourceStride * mirroredBorderLocationRight(ySource + 1u, sourceHeight);
	const uint8_t* source4 = source + sourceStride * mirroredBorderLocationRight(ySource + 2u, sourceHeight);

	for (unsigned int x = 0u; x < sourceElements; x += 8u)
	{
		if (x + 8u > sourceElements)
		{
			// the last iteration does not fit,
			// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

			ocean_assert(x >= 8u && sourceElements > 8u);
			const unsigned int newX = sourceElements - 8u;

			const unsigned int offset = x - newX;
			ocean_assert(offset < sourceElements);

			ocean_assert(x > newX);
			source0 -= offset;
			source1 -= offset;
			source2 -= offset;
			source3 -= offset;
			source4 -= offset;
			targetRow -= offset;

			x = newX;

			// the for loop will stop after this iteration
			ocean_assert(x + 8u == sourceElements);
			ocean_assert(!(x + 8u < sourceElements));
		}

		// loading the source information
		const __m128i source_a_16x8 = _mm_cvtepu8_epi16(_mm_loadl_epi64((const __m128i*)source0)); // * 1
		const __m128i source_b_16x8 = _mm_cvtepu8_epi16(_mm_loadl_epi64((const __m128i*)source1)); // * 4
		const __m128i source_c_16x8 = _mm_cvtepu8_epi16(_mm_loadl_epi64((const __m128i*)source2)); // * 6
		const __m128i source_d_16x8 = _mm_cvtepu8_epi16(_mm_loadl_epi64((const __m128i*)source3)); // * 4
		const __m128i source_e_16x8 = _mm_cvtepu8_epi16(_mm_loadl_epi64((const __m128i*)source4)); // * 1

		// source_a + source_e
		const __m128i source_ae_16x8 = _mm_add_epi16(source_a_16x8, source_e_16x8);

		// 2 * source_c
		const __m128i source_c2_16x8 = _mm_slli_epi16(source_c_16x8, 1);

		// source_b + source_c + source_d
		const __m128i source_bcd_16x8 = _mm_add_epi16(_mm_add_epi16(source_c_16x8, source_d_16x8), source_b_16x8);

		// (source_b + source_c + source_d) * 4
		const __m128i source_bcd4_16x8 = _mm_slli_epi16(source_bcd_16x8, 2);

		// (source_a + source_e) + (source_c * 2)
		const __m128i source_ae_c2 = _mm_add_epi16(source_ae_16x8, source_c2_16x8);

		// (source_a + source_e) + (source_c * 2) + (source_b + source_c + source_d) * 4
		const __m128i result_16x8 = _mm_add_epi16(source_ae_c2, source_bcd4_16x8);

		// storing the data
		_mm_storeu_si128((__m128i*)targetRow, result_16x8);

		source0 += 8;
		source1 += 8;
		source2 += 8;
		source3 += 8;
		source4 += 8;

		targetRow += 8;
	}
}

#endif // OCEAN_HARDWARE_SSE_VERSION >= 41

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

void FrameShrinker::downsampleByTwoRowVertical8BitPerChannel14641NEON(const uint8_t* const source, uint16_t* targetRow, const unsigned int sourceElements, const unsigned int sourceHeight, const unsigned int sourceStride, const unsigned int ySource)
{
	ocean_assert(source != nullptr);
	ocean_assert(targetRow != nullptr);
	ocean_assert(sourceStride >= 16u && sourceHeight >= 2u);

	/*
	 * We determine 8 filter responses within one loop iteration.
	 * For 1 channel frames, with row = 0, we apply the following mirroring strategy:
	 *
	 * Source Data:
	 *     1  Y 1 1 1 1 1 1 1 1
	 *     0  Y 4 4 4 4 4 4 4 4
	 *        -----------------
	 *     0  Y 6 6 6 6 6 6 6 6 <---------
	 *     1  Y 4 4 4 4 4 4 4 4
	 *     2  Y 1 1 1 1 1 1 1 1
	 *     3  Y
	 *
	 * For frames with n channels the strategy stays the same.
	 */

	const uint8_t* source0 = source + sourceStride * mirroredBorderLocationLeft(int(ySource) - 2);
	const uint8_t* source1 = source + sourceStride * mirroredBorderLocationLeft(int(ySource) - 1);
	const uint8_t* source2 = source + sourceStride * ySource;
	const uint8_t* source3 = source + sourceStride * mirroredBorderLocationRight(ySource + 1u, sourceHeight);
	const uint8_t* source4 = source + sourceStride * mirroredBorderLocationRight(ySource + 2u, sourceHeight);

	const uint8x8_t constant_6_u_8x8 = vdup_n_u8(6u);

	for (unsigned int x = 0u; x < sourceElements; x += 16u)
	{
		if (x + 16u > sourceElements)
		{
			// the last iteration does not fit,
			// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

			ocean_assert(x >= 16u && sourceElements > 16u);
			const unsigned int newX = sourceElements - 16u;

			const unsigned int offset = x - newX;
			ocean_assert(offset < sourceElements);

			ocean_assert(x > newX);
			source0 -= offset;
			source1 -= offset;
			source2 -= offset;
			source3 -= offset;
			source4 -= offset;
			targetRow -= offset;

			x = newX;

			// the for loop will stop after this iteration
			ocean_assert(x + 16u == sourceElements);
			ocean_assert(!(x + 16u < sourceElements));
		}

		// loading the source information
		const uint8x16_t source_a_8x16 = vld1q_u8(source0); // * 1
		const uint8x16_t source_b_8x16 = vld1q_u8(source1); // * 4
		const uint8x16_t source_c_8x16 = vld1q_u8(source2); // * 6
		const uint8x16_t source_d_8x16 = vld1q_u8(source3); // * 4
		const uint8x16_t source_e_8x16 = vld1q_u8(source4); // * 1

		// source_a + source_e
		uint16x8_t result_low_16x8 = vaddl_u8(vget_low_u8(source_a_8x16), vget_low_u8(source_e_8x16));
		uint16x8_t result_high_16x8 = vaddl_u8(vget_high_u8(source_a_8x16), vget_high_u8(source_e_8x16));

		// result += (source_b + source_d) * 4
		result_low_16x8 = vaddq_u16(result_low_16x8, vshlq_n_u16(vaddl_u8(vget_low_u8(source_b_8x16), vget_low_u8(source_d_8x16)), 2));
		result_high_16x8 = vaddq_u16(result_high_16x8, vshlq_n_u16(vaddl_u8(vget_high_u8(source_b_8x16), vget_high_u8(source_d_8x16)), 2));

		// result += source_c * 6
		result_low_16x8 = vmlal_u8(result_low_16x8, vget_low_u8(source_c_8x16), constant_6_u_8x8);
		result_high_16x8 = vmlal_u8(result_high_16x8, vget_high_u8(source_c_8x16), constant_6_u_8x8);

#ifdef OCEAN_WE_KEEP_THIS_CODE_TO_SHOW_AN_ALTERNATIVE_IMPLEMENTATION_WHICH_IS_SLOWER

		// the following code avoids all multiplications and applies shifts instead

		// source_a + source_e
		const uint16x8_t source_ae_low_16x8 = vaddl_u8(vget_low_u8(source_a_8x16), vget_low_u8(source_e_8x16));
		const uint16x8_t source_ae_high_16x8 = vaddl_u8(vget_high_u8(source_a_8x16), vget_high_u8(source_e_8x16));

		// 2 * source_c
		const uint16x8_t source_c2_low_16x8 = vshll_n_u8(vget_low_u8(source_c_8x16), 1);
		const uint16x8_t source_c2_high_16x8 = vshll_n_u8(vget_high_u8(source_c_8x16), 1);

		// source_b + source_c + source_d
		const uint16x8_t source_bcd_low_16x8 = vaddw_u8(vaddl_u8(vget_low_u8(source_c_8x16), vget_low_u8(source_d_8x16)), vget_low_u8(source_b_8x16));
		const uint16x8_t source_bcd_high_16x8 = vaddw_u8(vaddl_u8(vget_high_u8(source_c_8x16), vget_high_u8(source_d_8x16)), vget_high_u8(source_b_8x16));

		// (source_b + source_c + source_d) * 4
		const uint16x8_t source_bcd4_low_16x8 = vshlq_n_u16(source_bcd_low_16x8, 2);
		const uint16x8_t source_bcd4_high_16x8 = vshlq_n_u16(source_bcd_high_16x8, 2);

		// (source_a + source_e) + (source_c * 2)
		const uint16x8_t source_ae_c2_low_16x8 = vaddq_u16(source_ae_low_16x8, source_c2_low_16x8);
		const uint16x8_t source_ae_c2_high_16x8 = vaddq_u16(source_ae_high_16x8, source_c2_high_16x8);

		// (source_a + source_e) + (source_c * 2) + (source_b + source_c + source_d) * 4
		const uint16x8_t result_low_16x8 = vaddq_u16(source_ae_c2_low_16x8, source_bcd4_low_16x8);
		const uint16x8_t result_high_16x8 = vaddq_u16(source_ae_c2_high_16x8, source_bcd4_high_16x8);

#endif // OCEAN_WE_KEEP_THIS_CODE_TO_SHOW_AN_ALTERNATIVE_IMPLEMENTATION_WHICH_IS_SLOWER

		// storing the data
		vst1q_u16(targetRow, result_low_16x8);
		vst1q_u16(targetRow + 8, result_high_16x8);

		source0 += 16;
		source1 += 16;
		source2 += 16;
		source3 += 16;
		source4 += 16;

		targetRow += 16;
	}

}

template <>
inline void FrameShrinker::downsampleByTwoRowHorizontal8BitPerChannel14641NEON<1u>(const uint16_t* sourceRow, uint8_t* targetRow, const unsigned int targetWidth, const unsigned int channels)
{
	ocean_assert(sourceRow != nullptr);
	ocean_assert(targetRow != nullptr);
	ocean_assert(targetWidth >= 8u);

	ocean_assert(channels == 1u);
	const unsigned int targetElements = targetWidth * 1u;

	/**
	 * We determine 8 filter responses within one loop iteration.
	 * For 1 channel frames we apply the following strategy:
	 *
	 * Source Data: Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y Y (if the source data has a Y8 pixel format)
	 *              1   1   1   1   1   1   1   1   .
	 *                4 . 4   4   4   4   4   4   4 .
	 *                  6   6   6   6   6   6   6   6
	 *                  . 4   4   4   4   4   4   4 . 4
	 *                  .   1   1   1   1   1   1   1   1
	 *                  .                           .
	 * Target Data: - - Y   Y   Y   Y   Y   Y   Y   Y - -
	 *
	 */

	constexpr uint8x8_t mask1233 = {2, 3, 4, 5, 6, 7, 6, 7};

	const uint16x8_t constant_6_u_16x8 = vdupq_n_u16(6u);

	for (unsigned int x = 0u; x < targetElements; x += 8u)
	{
		if (x + 8u > targetElements)
		{
			// the last iteration does not fit,
			// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

			ocean_assert(x >= 8u && targetElements > 8u);
			const unsigned int newX = targetElements - 8u;

			ocean_assert(x > newX);
			const unsigned int offset = x - newX;

			sourceRow -= offset * 2u;
			targetRow -= offset;

			x = newX;

			// the for loop will stop after this iteration
			ocean_assert(x + 8u == targetElements);
			ocean_assert(!(x + 8u < targetElements));
		}

		// loading the source information

#ifdef OCEAN_WE_KEEP_THIS_CODE_TO_SHOW_AN_ALTERNATIVE_IMPLEMENTATION_WHICH_IS_SLOWER

		// the following code loads our 5 neon registers, however we will use a slightly different approach
		// needing less load instructions but more shuffle instructions

		const uint16x8_t source_a_16x8 = vld2q_u16((const uint16_t*)sourceRow + 0).val[0]; // * 1
		const uint16x8_t source_b_16x8 = vld2q_u16((const uint16_t*)sourceRow + 1).val[0]; // * 4
		const uint16x8_t source_c_16x8 = vld2q_u16((const uint16_t*)sourceRow + 2).val[0]; // * 6
		const uint16x8_t source_d_16x8 = vld2q_u16((const uint16_t*)sourceRow + 3).val[0]; // * 4
		const uint16x8_t source_e_16x8 = vld2q_u16((const uint16_t*)sourceRow + 4).val[0]; // * 1

#endif

		const uint16x8_t soruce_00_07_16x8 = vld1q_u16((const uint16_t*)sourceRow +  0);
		const uint16x8_t soruce_08_15_16x8 = vld1q_u16((const uint16_t*)sourceRow +  8);
		const uint16x4_t source_16_18_16x4 = vreinterpret_u16_u8(vtbl1_u8(vreinterpret_u8_u16(vld1_u16((const uint16_t*)sourceRow + 15)), mask1233));
		const uint16x8_t source_16_18_16x8 = vcombine_u16(source_16_18_16x4, source_16_18_16x4);

		uint16x4x2_t source_first_16x4x2 = vuzp_u16(vget_low_u16(soruce_00_07_16x8), vget_high_u16(soruce_00_07_16x8));  // [Y00 Y02 Y04 Y06], [Y01 Y03 Y05 Y07]
		uint16x4x2_t source_second_16x4x2 = vuzp_u16(vget_low_u16(soruce_08_15_16x8), vget_high_u16(soruce_08_15_16x8)); // [Y08 Y10 Y12 Y14], [Y09 Y11 Y13 Y15]
		const uint16x8_t source_a_16x8 = vcombine_u16(source_first_16x4x2.val[0], source_second_16x4x2.val[0]); // [Y0 Y2 .. .. Y14]
		const uint16x8_t source_b_16x8 = vcombine_u16(source_first_16x4x2.val[1], source_second_16x4x2.val[1]); // [Y1 Y3 .. .. Y15]

		const uint16x8_t source_03_10_16x8 = vextq_u16(soruce_00_07_16x8, soruce_08_15_16x8, 3);
		const uint16x8_t source_11_18_16x8 = vextq_u16(soruce_08_15_16x8, source_16_18_16x8, 3);

		source_first_16x4x2 = vuzp_u16(vget_low_u16(source_03_10_16x8), vget_high_u16(source_03_10_16x8));  // [Y03 Y05 Y07 Y09], [Y04 Y06 Y08 Y10]
		source_second_16x4x2 = vuzp_u16(vget_low_u16(source_11_18_16x8), vget_high_u16(source_11_18_16x8)); // [Y11 Y13 Y15 Y17], [Y12 Y14 Y16 Y18]
		const uint16x8_t source_d_16x8 = vcombine_u16(source_first_16x4x2.val[0], source_second_16x4x2.val[0]); // [Y3 Y5 .. .. Y17]
		const uint16x8_t source_e_16x8 = vcombine_u16(source_first_16x4x2.val[1], source_second_16x4x2.val[1]); // [Y4 Y6 .. .. Y18]

		const uint16x8_t source_e_end_16x8 = vdupq_lane_u16(vget_high_u16(source_e_16x8), 2);

		const uint16x8_t source_c_16x8 = vextq_u16(source_a_16x8, source_e_end_16x8, 1); // [Y2 Y4 .. .. Y16]


		// now we can apply the actual filter

		// result = source_a + source_e
		uint16x8_t result_16x8 = vaddq_u16(source_a_16x8, source_e_16x8);

		// result += (source_b + source_d) * 4
		result_16x8 = vaddq_u16(result_16x8, vshlq_n_u16(vaddq_u16(source_b_16x8, source_d_16x8), 2));

		// result += source_c * 6
		result_16x8 = vmlaq_u16(result_16x8, source_c_16x8, constant_6_u_16x8);

		// result_8x8 = (result_16x8 + 2^7) / 2^8
		const uint8x8_t result_8x8 = vrshrn_n_u16(result_16x8, 8);


#ifdef OCEAN_WE_KEEP_THIS_CODE_TO_SHOW_AN_ALTERNATIVE_IMPLEMENTATION_WHICH_IS_SLOWER

		// source_a + source_e
		const uint16x8_t source_ae_16x8 = vaddq_u16(source_a_16x8, source_e_16x8);

		// 2 * source_c
		const uint16x8_t source_c2_16x8 = vshlq_n_u16(source_c_16x8, 1);

		// source_b + source_c + source_d
		const uint16x8_t source_bcd_16x8 = vaddq_u16(source_b_16x8, vaddq_u16(source_c_16x8, source_d_16x8));

		// (source_b + source_c + source_d) * 4
		const uint16x8_t source_bcd4_16x8 = vshlq_n_u16(source_bcd_16x8, 2);

		// (source_a + source_e) + (source_c * 2)
		const uint16x8_t source_ae_c2 = vaddq_u16(source_ae_16x8, source_c2_16x8);

		// (source_a + source_e) + (source_c * 2) + (source_b + source_c + source_d) * 4
		const uint16x8_t result_16x8 = vaddq_u16(source_ae_c2, source_bcd4_16x8);

		// result_8x8 = (result_16x8 + 2^7) / 2^8
		const uint8x8_t result_8x8 = vrshrn_n_u16(result_16x8, 8);

#endif // OCEAN_WE_KEEP_THIS_CODE_TO_SHOW_AN_ALTERNATIVE_IMPLEMENTATION_WHICH_IS_SLOWER

		// storing the data
		vst1_u8(targetRow, result_8x8);

		sourceRow += 16;
		targetRow += 8;
	}
}

template <>
inline void FrameShrinker::downsampleByTwoRowHorizontal8BitPerChannel14641NEON<2u>(const uint16_t* sourceRow, uint8_t* targetRow, const unsigned int targetWidth, const unsigned int channels)
{
	ocean_assert(sourceRow != nullptr);
	ocean_assert(targetRow != nullptr);
	ocean_assert(targetWidth >= 4u);

	ocean_assert(channels == 2u);
	const unsigned int targetElements = targetWidth * 2u;

	/**
	 * We determine 8 filter responses within one loop iteration.
	 * For 2 channel frames we apply the following strategy:
	 *
	 * Source Data: YA YA YA YA YA YA YA YA YA YA YA YA YA YA YA YA YA YA YA YA YA YA (if the source data has a YA16 pixel format)
	 *              11    11    11    11    ..
	 *                 44 .. 44    44    44 ..
	 *                    66    66    66    66
	 *                    .. 44    44    44 .. 44
	 *                    ..    11    11    11    11
	 *                    ..                ..
	 * Target Data: -- -- YA    YA    YA    YA -- --
	 *
	 */

	for (unsigned int x = 0u; x < targetElements; x += 8u)
	{
		if (x + 8u > targetElements)
		{
			// the last iteration does not fit,
			// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

			ocean_assert(x >= 8u && targetElements > 8u);
			const unsigned int newX = targetElements - 8u;

			ocean_assert(x > newX);
			const unsigned int offset = x - newX;

			sourceRow -= offset * 2u;
			targetRow -= offset;

			x = newX;

			// the for loop will stop after this iteration
			ocean_assert(x + 8u == targetElements);
			ocean_assert(!(x + 8u < targetElements));
		}

		// loading the source information
		const uint16x8_t source_a_16x8 = vreinterpretq_u16_u32(vld2q_u32((const uint32_t*)sourceRow + 0).val[0]); // * 1
		const uint16x8_t source_b_16x8 = vreinterpretq_u16_u32(vld2q_u32((const uint32_t*)sourceRow + 1).val[0]); // * 4
		const uint16x8_t source_c_16x8 = vreinterpretq_u16_u32(vld2q_u32((const uint32_t*)sourceRow + 2).val[0]); // * 6
		const uint16x8_t source_d_16x8 = vreinterpretq_u16_u32(vld2q_u32((const uint32_t*)sourceRow + 3).val[0]); // * 4
		const uint16x8_t source_e_16x8 = vreinterpretq_u16_u32(vld2q_u32((const uint32_t*)sourceRow + 4).val[0]); // * 1

		// source_a + source_e
		const uint16x8_t source_ae_16x8 = vaddq_u16(source_a_16x8, source_e_16x8);

		// 2 * source_c
		const uint16x8_t source_c2_16x8 = vshlq_n_u16(source_c_16x8, 1);

		// source_b + source_c + source_d
		const uint16x8_t source_bcd_16x8 = vaddq_u16(source_b_16x8, vaddq_u16(source_c_16x8, source_d_16x8));

		// (source_b + source_c + source_d) * 4
		const uint16x8_t source_bcd4_16x8 = vshlq_n_u16(source_bcd_16x8, 2);

		// (source_a + source_e) + (source_c * 2)
		const uint16x8_t source_ae_c2 = vaddq_u16(source_ae_16x8, source_c2_16x8);

		// (source_a + source_e) + (source_c * 2) + (source_b + source_c + source_d) * 4
		const uint16x8_t result_16x8 = vaddq_u16(source_ae_c2, source_bcd4_16x8);

		// result_8x8 = (result_16x8 + 2^7) / 2^8
		const uint8x8_t result_8x8 = vrshrn_n_u16(result_16x8, 8);

		// storing the data
		vst1_u8(targetRow, result_8x8);

		sourceRow += 16;
		targetRow += 8;
	}
}

template <>
inline void FrameShrinker::downsampleByTwoRowHorizontal8BitPerChannel14641NEON<3u>(const uint16_t* sourceRow, uint8_t* targetRow, const unsigned int targetWidth, const unsigned int channels)
{
	ocean_assert(sourceRow != nullptr);
	ocean_assert(targetRow != nullptr);
	ocean_assert(targetWidth >= 8u);

	ocean_assert(channels == 3u);
	const unsigned int targetElements = targetWidth * 3u;

	/**
	 * We determine 8 filter responses within one loop iteration.
	 * For 3 channel frames we apply the following strategy:
	 *
	 * Source Data: RGB RGB RGB RGB RGB RGB RGB RGB RGB RGB RGB RGB RGB RGB RGB RGB RGB RGB RGB (if the source data has a RGB24 pixel format)
	 *              111     111     111     111     111     111     111     111     ...
	 *                  444 ... 444     444     444     444     444     444     444 ...
	 *                      666     666     666     666     666     666     666     666
	 *                      ... 444     444     444     444     444     444     444 ... 444
	 *                      ...     111     111     111     111     111     111     111     111
	 *                      ...                                                     ...
	 * Target Data: --- --- RGB     RGB     RGB     RGB     RGB     RGB     RGB     RGB --- ---
	 *
	 */

	ocean_assert(targetElements >= 24u);

	const uint16x8_t value_six_16x8 = vdupq_n_u16(6u);

	for (unsigned int x = 0u; x < targetElements; x += 24u)
	{
		if (x + 24u > targetElements)
		{
			// the last iteration does not fit,
			// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

			ocean_assert(x >= 24u && targetElements > 24u);
			const unsigned int newX = targetElements - 24u;

			ocean_assert(x > newX);
			const unsigned int offset = x - newX;

			sourceRow -= offset * 2u;
			targetRow -= offset;

			x = newX;

			// the for loop will stop after this iteration
			ocean_assert(x + 24u == targetElements);
			ocean_assert(!(x + 24u < targetElements));
		}

		// the first 24 bytes (8 pixels)
		const uint16x8x3_t source_16x8x3_00_23 = vld3q_u16((const uint16_t*)sourceRow + 0);

		// separating the first 24 bytes to odd and even pixel values (by channel)
		const uint16x4x2_t source_first_channel0_16x4x2 = vuzp_u16(vget_low_u16(source_16x8x3_00_23.val[0]), vget_high_u16(source_16x8x3_00_23.val[0])); // [R0 R2 R4 R6], [R1 R3 R5 R7]
		const uint16x4x2_t source_first_channel1_16x4x2 = vuzp_u16(vget_low_u16(source_16x8x3_00_23.val[1]), vget_high_u16(source_16x8x3_00_23.val[1])); // [G0 G2 G4 G6], [G1 G3 G5 G7]
		const uint16x4x2_t source_first_channel2_16x4x2 = vuzp_u16(vget_low_u16(source_16x8x3_00_23.val[2]), vget_high_u16(source_16x8x3_00_23.val[2])); // [B0 B2 B4 B6], [B1 B3 B5 B7]

		// the second 24 bytes (8 pixels)
		const uint16x8x3_t source_16x8x3_24_47 = vld3q_u16((const uint16_t*)sourceRow + 24);

		// separating the second 24 bytes to odd and even pixel values (by channel)
		const uint16x4x2_t source_second_channel0_16x4x2 = vuzp_u16(vget_low_u16(source_16x8x3_24_47.val[0]), vget_high_u16(source_16x8x3_24_47.val[0])); // [R8 R10 R12 R14], [R9 R11 R13 R15]
		const uint16x4x2_t source_second_channel1_16x4x2 = vuzp_u16(vget_low_u16(source_16x8x3_24_47.val[1]), vget_high_u16(source_16x8x3_24_47.val[1]));
		const uint16x4x2_t source_second_channel2_16x4x2 = vuzp_u16(vget_low_u16(source_16x8x3_24_47.val[2]), vget_high_u16(source_16x8x3_24_47.val[2]));

		// separating the first 8 even pixels (by channel): 0 2 4 6 ...
		uint16x8_t source_channel0_even_16x8 = vcombine_u16(source_first_channel0_16x4x2.val[0], source_second_channel0_16x4x2.val[0]);
		uint16x8_t source_channel1_even_16x8 = vcombine_u16(source_first_channel1_16x4x2.val[0], source_second_channel1_16x4x2.val[0]);
		uint16x8_t source_channel2_even_16x8 = vcombine_u16(source_first_channel2_16x4x2.val[0], source_second_channel2_16x4x2.val[0]);

		// separating the first 8 odd pixels (by channel): 1 3 5 7 ...
		uint16x8_t source_channel0_odd_16x8 = vcombine_u16(source_first_channel0_16x4x2.val[1], source_second_channel0_16x4x2.val[1]);
		uint16x8_t source_channel1_odd_16x8 = vcombine_u16(source_first_channel1_16x4x2.val[1], source_second_channel1_16x4x2.val[1]);
		uint16x8_t source_channel2_odd_16x8 = vcombine_u16(source_first_channel2_16x4x2.val[1], source_second_channel2_16x4x2.val[1]);

		// we can multiply all odd values by 4 -> (1 4 6 4 1)
		source_channel0_odd_16x8 = vshlq_n_u16(source_channel0_odd_16x8, 2);
		source_channel1_odd_16x8 = vshlq_n_u16(source_channel1_odd_16x8, 2);
		source_channel2_odd_16x8 = vshlq_n_u16(source_channel2_odd_16x8, 2);

		// * 1 with even values
		uint16x8_t result_channel0_16x8 = source_channel0_even_16x8;
		uint16x8_t result_channel1_16x8 = source_channel1_even_16x8;
		uint16x8_t result_channel2_16x8 = source_channel2_even_16x8;

		// * 4 with odd values (have been multiplied by 4 already)
		result_channel0_16x8 = vaddq_u16(result_channel0_16x8, source_channel0_odd_16x8);
		result_channel1_16x8 = vaddq_u16(result_channel1_16x8, source_channel1_odd_16x8);
		result_channel2_16x8 = vaddq_u16(result_channel2_16x8, source_channel2_odd_16x8);



		// the last 12 bytes (3 pixels) - while we have to load 16 bytes
		const uint16x4x3_t source_16x8x3_45_56 = vld3_u16((const uint16_t*)sourceRow + 45); // loading pixel 15 16 17 18 (starting with an odd pixel)

		uint16x4x2_t source_third_channel0_16x4x2 = vuzp_u16(source_16x8x3_45_56.val[0], source_16x8x3_45_56.val[0]); // [R15 R17 R15 R17], [R16 R18 R16 R18]
		uint16x4x2_t source_third_channel1_16x4x2 = vuzp_u16(source_16x8x3_45_56.val[1], source_16x8x3_45_56.val[1]);
		uint16x4x2_t source_third_channel2_16x4x2 = vuzp_u16(source_16x8x3_45_56.val[2], source_16x8x3_45_56.val[2]);

		// we actually do not need R15, we want R17
		source_third_channel0_16x4x2.val[0] = vrev32_u16(source_third_channel0_16x4x2.val[0]); // [R17 R15 R17 R15], [R16 R18 R16 R18]
		source_third_channel1_16x4x2.val[0] = vrev32_u16(source_third_channel1_16x4x2.val[0]);
		source_third_channel2_16x4x2.val[0] = vrev32_u16(source_third_channel2_16x4x2.val[0]);

		// we multiply the new odd values by 4
		source_third_channel0_16x4x2.val[0] = vshl_n_u16(source_third_channel0_16x4x2.val[0], 2);
		source_third_channel1_16x4x2.val[0] = vshl_n_u16(source_third_channel1_16x4x2.val[0], 2);
		source_third_channel2_16x4x2.val[0] = vshl_n_u16(source_third_channel2_16x4x2.val[0], 2);

		const uint16x8_t source_remaining_channel0_even_16x8 = vcombine_u16(source_third_channel0_16x4x2.val[1], source_third_channel0_16x4x2.val[1]); // [R16 R18 R16 R18 R16 R18 R16 R18]
		const uint16x8_t source_remaining_channel1_even_16x8 = vcombine_u16(source_third_channel1_16x4x2.val[1], source_third_channel1_16x4x2.val[1]);
		const uint16x8_t source_remaining_channel2_even_16x8 = vcombine_u16(source_third_channel2_16x4x2.val[1], source_third_channel2_16x4x2.val[1]);

		const uint16x8_t source_remaining_channel0_odd_16x8 = vcombine_u16(source_third_channel0_16x4x2.val[0], source_third_channel0_16x4x2.val[0]); // [R15 R17 R15 R17 R15 R17 R15 R17] * 4
		const uint16x8_t source_remaining_channel1_odd_16x8 = vcombine_u16(source_third_channel1_16x4x2.val[0], source_third_channel1_16x4x2.val[0]);
		const uint16x8_t source_remaining_channel2_odd_16x8 = vcombine_u16(source_third_channel2_16x4x2.val[0], source_third_channel2_16x4x2.val[0]);

		uint16x8_t intermediate_source_channel0_even_16x8 = vextq_u16(source_channel0_even_16x8, source_remaining_channel0_even_16x8, 1);
		uint16x8_t intermediate_source_channel1_even_16x8 = vextq_u16(source_channel1_even_16x8, source_remaining_channel1_even_16x8, 1);
		uint16x8_t intermediate_source_channel2_even_16x8 = vextq_u16(source_channel2_even_16x8, source_remaining_channel2_even_16x8, 1);

		// * 6 with even values
		result_channel0_16x8 = vmlaq_u16(result_channel0_16x8, intermediate_source_channel0_even_16x8, value_six_16x8);
		result_channel1_16x8 = vmlaq_u16(result_channel1_16x8, intermediate_source_channel1_even_16x8, value_six_16x8);
		result_channel2_16x8 = vmlaq_u16(result_channel2_16x8, intermediate_source_channel2_even_16x8, value_six_16x8);

		source_channel0_odd_16x8 = vextq_u16(source_channel0_odd_16x8, source_remaining_channel0_odd_16x8, 1);
		source_channel1_odd_16x8 = vextq_u16(source_channel1_odd_16x8, source_remaining_channel1_odd_16x8, 1);
		source_channel2_odd_16x8 = vextq_u16(source_channel2_odd_16x8, source_remaining_channel2_odd_16x8, 1);

		// * 4 with odd values
		result_channel0_16x8 = vaddq_u16(result_channel0_16x8, source_channel0_odd_16x8);
		result_channel1_16x8 = vaddq_u16(result_channel1_16x8, source_channel1_odd_16x8);
		result_channel2_16x8 = vaddq_u16(result_channel2_16x8, source_channel2_odd_16x8);

		source_channel0_even_16x8 = vextq_u16(source_channel0_even_16x8, source_remaining_channel0_even_16x8, 2);
		source_channel1_even_16x8 = vextq_u16(source_channel1_even_16x8, source_remaining_channel1_even_16x8, 2);
		source_channel2_even_16x8 = vextq_u16(source_channel2_even_16x8, source_remaining_channel2_even_16x8, 2);

		// * 1 with even values
		result_channel0_16x8 = vaddq_u16(result_channel0_16x8, source_channel0_even_16x8);
		result_channel1_16x8 = vaddq_u16(result_channel1_16x8, source_channel1_even_16x8);
		result_channel2_16x8 = vaddq_u16(result_channel2_16x8, source_channel2_even_16x8);

		// result_16x4x3 = (result_16x4x3 + 2^7) / 2^8
		uint8x8x3_t result_8x8x3;
		result_8x8x3.val[0] = vrshrn_n_u16(result_channel0_16x8, 8);
		result_8x8x3.val[1] = vrshrn_n_u16(result_channel1_16x8, 8);
		result_8x8x3.val[2] = vrshrn_n_u16(result_channel2_16x8, 8);

		vst3_u8(targetRow, result_8x8x3);

		sourceRow += 16 * 3;
		targetRow += 8 * 3;
	}
}

template <>
inline void FrameShrinker::downsampleByTwoRowHorizontal8BitPerChannel14641NEON<4u>(const uint16_t* sourceRow, uint8_t* targetRow, const unsigned int targetWidth, const unsigned int channels)
{
	ocean_assert(sourceRow != nullptr);
	ocean_assert(targetRow != nullptr);
	ocean_assert(targetWidth >= 2u);

	ocean_assert(channels == 4u);
	const unsigned int targetElements = targetWidth * 4u;

	/**
	 * We determine 8 filter responses within one loop iteration.
	 * For 4 channel frames we apply the following strategy:
	 *
	 * Source Data: RGBA RGBA RGBA RGBA RGBA RGBA RGBA (if the source data has a RGBA32 pixel format)
	 *              1111      1111      ....
	 *                   4444 .... 4444 ....
	 *                        6666      6666
	 *                        .... 4444 .... 4444
	 *                        ....      1111      1111
	 *                        ....      ....
	 * Target Data: ---- ---- RGBA      RGBA ---- ----
	 *
	 */

	ocean_assert(targetElements >= 8u);

	for (unsigned int x = 0u; x < targetElements; x += 8u)
	{
		if (x + 8u > targetElements)
		{
			// the last iteration does not fit,
			// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

			ocean_assert(x >= 8u && targetElements > 8u);
			const unsigned int newX = targetElements - 8u;

			ocean_assert(x > newX);
			const unsigned int offset = x - newX;

			sourceRow -= offset * 2u;
			targetRow -= offset;

			x = newX;

			// the for loop will stop after this iteration
			ocean_assert(x + 8u == targetElements);
			ocean_assert(!(x + 8u < targetElements));
		}

		// loading the source information
		const uint64x1x4_t source_0_3_64x1x2 = vld4_u64((const uint64_t*)sourceRow + 0);
		const uint64x1x3_t source_4_6_64x1x2 = vld3_u64((const uint64_t*)sourceRow + 4);

		const uint16x8_t source_a_16x8 = vcombine_u16(vreinterpret_u16_u64(source_0_3_64x1x2.val[0]), vreinterpret_u16_u64(source_0_3_64x1x2.val[2]));
		const uint16x8_t source_b_16x8 = vcombine_u16(vreinterpret_u16_u64(source_0_3_64x1x2.val[1]), vreinterpret_u16_u64(source_0_3_64x1x2.val[3]));
		const uint16x8_t source_c_16x8 = vcombine_u16(vreinterpret_u16_u64(source_0_3_64x1x2.val[2]), vreinterpret_u16_u64(source_4_6_64x1x2.val[0]));
		const uint16x8_t source_d_16x8 = vcombine_u16(vreinterpret_u16_u64(source_0_3_64x1x2.val[3]), vreinterpret_u16_u64(source_4_6_64x1x2.val[1]));
		const uint16x8_t source_e_16x8 = vcombine_u16(vreinterpret_u16_u64(source_4_6_64x1x2.val[0]), vreinterpret_u16_u64(source_4_6_64x1x2.val[2]));

		// source_a + source_e
		const uint16x8_t source_ae_16x8 = vaddq_u16(source_a_16x8, source_e_16x8);

		// 2 * source_c
		const uint16x8_t source_c2_16x8 = vshlq_n_u16(source_c_16x8, 1);

		// source_b + source_c + source_d
		const uint16x8_t source_bcd_16x8 = vaddq_u16(source_b_16x8, vaddq_u16(source_c_16x8, source_d_16x8));

		// (source_b + source_c + source_d) * 4
		const uint16x8_t source_bcd4_16x8 = vshlq_n_u16(source_bcd_16x8, 2);

		// (source_a + source_e) + (source_c * 2)
		const uint16x8_t source_ae_c2 = vaddq_u16(source_ae_16x8, source_c2_16x8);

		// (source_a + source_e) + (source_c * 2) + (source_b + source_c + source_d) * 4
		const uint16x8_t result_16x8 = vaddq_u16(source_ae_c2, source_bcd4_16x8);

		// result_8x8 = (result_16x8 + 2^7) / 2^8
		const uint8x8_t result_8x8 = vrshrn_n_u16(result_16x8, 8);

		// storing the data
		vst1_u8(targetRow, result_8x8);

		sourceRow += 16;
		targetRow += 8;
	}
}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

void FrameShrinker::downsampleByTwo8BitPerChannel14641Subset(const uint8_t* source, uint8_t* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int channels, const unsigned int sourceStrideElements, const unsigned int targetStrideElements, const unsigned int firstTargetRow, const unsigned int numberTargetRows)
{
	ocean_assert(source != nullptr);
	ocean_assert(target != nullptr);
	ocean_assert(sourceWidth != 0u && sourceHeight != 0u);

	ocean_assert((sourceWidth + 0u) / 2u == targetWidth || (sourceWidth + 1u) / 2u == targetWidth);

	ocean_assert(channels != 0u);

	// we have specialized horizontal downsampling functions,
	// we use a function pointer to select the correct function

	typedef void (*DownsampleByTwoRowVertical8BitPerChannel14641Function)(const uint8_t* const, uint16_t*, const unsigned int, const unsigned int, const unsigned int, const unsigned int);
	typedef void (*DownsampleByTwoRowHorizontal8BitPerChannel14641Function)(const uint16_t*, uint8_t*, const unsigned int, const unsigned int);

	DownsampleByTwoRowVertical8BitPerChannel14641Function downsampleByTwoRowVerticalFunction = downsampleByTwoRowVertical8BitPerChannel14641;
	DownsampleByTwoRowHorizontal8BitPerChannel14641Function downsampleByTwoRowHorizontalFunction = downsampleByTwoRowHorizontal8BitPerChannel14641;

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

	if (targetWidth * channels >= 8u)
	{
		downsampleByTwoRowVerticalFunction = downsampleByTwoRowVertical8BitPerChannel14641SSE;
	}

#endif // OCEAN_HARDWARE_SSE_VERSION

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	if (targetWidth * channels >= 16u)
	{
		downsampleByTwoRowVerticalFunction = downsampleByTwoRowVertical8BitPerChannel14641NEON;
	}

	if (channels == 1u && targetWidth >= 8u)
	{
		downsampleByTwoRowHorizontalFunction = downsampleByTwoRowHorizontal8BitPerChannel14641NEON<1u>;
	}
	else if (channels == 2u && targetWidth >= 4u)
	{
		downsampleByTwoRowHorizontalFunction = downsampleByTwoRowHorizontal8BitPerChannel14641NEON<2u>;
	}
	else if (channels == 3u && targetWidth >= 8u)
	{
		downsampleByTwoRowHorizontalFunction = downsampleByTwoRowHorizontal8BitPerChannel14641NEON<3u>;
	}
	else if (channels == 4u && targetWidth >= 2u)
	{
		downsampleByTwoRowHorizontalFunction = downsampleByTwoRowHorizontal8BitPerChannel14641NEON<4u>;
	}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

	// the intermediate row gets 2 additional (mirrored) pixels at the left side and 2 on the right side
	Memory intermediateRowMeory = Memory::create<uint16_t>((sourceWidth + 4u) * channels);
	uint16_t* intermediateRow = intermediateRowMeory.data<uint16_t>();

	target += firstTargetRow * targetStrideElements;

	for (unsigned int yTarget = firstTargetRow; yTarget < firstTargetRow + numberTargetRows; ++yTarget)
	{
		const unsigned int ySource = yTarget * 2u;
		ocean_assert(ySource < sourceHeight);

		// first we apply the vertical filtering and store the result in an intermediate row (with same width as the source frame)
		downsampleByTwoRowVerticalFunction(source, intermediateRow + 2u * channels, sourceWidth * channels, sourceHeight, sourceStrideElements, ySource);

		// mirroring the left and right border
		for (unsigned int n = 0u; n < channels; ++n)
		{
			// left border
			intermediateRow[0u * channels + n] = intermediateRow[3u * channels + n];
			intermediateRow[1u * channels + n] = intermediateRow[2u * channels + n];

			// right border
			intermediateRow[(sourceWidth + 2u) * channels + n] = intermediateRow[(sourceWidth + 1u) * channels + n];
			intermediateRow[(sourceWidth + 3u) * channels + n] = intermediateRow[(sourceWidth + 0u) * channels + n];
		}

		ocean_assert(downsampleByTwoRowHorizontalFunction != nullptr);
		downsampleByTwoRowHorizontalFunction(intermediateRow, target, targetWidth, channels);

		target += targetStrideElements;
	}
}

}

}
