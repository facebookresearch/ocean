// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/cv/FramePyramid.h"
#include "ocean/cv/FrameShrinker.h"
#include "ocean/cv/FrameShrinkerAlpha.h"

namespace Ocean
{

namespace CV
{

FramePyramid::FramePyramid(const FramePyramid& framePyramid)
{
	if (framePyramid.memory_.isOwner())
	{
		memory_ = Memory(framePyramid.memory_.size(), memoryAlignmentBytes_);

		ocean_assert(memory_.size() == framePyramid.memory_.size());
		memcpy(memory_.data(), framePyramid.memory_.constdata(), memory_.size());

		for (size_t n = 0; n < framePyramid.layers_.size(); ++n)
		{
			const FrameType& type = framePyramid.layers_[n].frameType();

			ocean_assert(framePyramid.memory_.constdata());
			ocean_assert(framePyramid.layers_[n].constdata());

			ocean_assert(framePyramid.memory_.constdata() <= framePyramid.layers_[n].constdata());
			const size_t offset = framePyramid.layers_[n].constdata<uint8_t>() - framePyramid.memory_.constdata<uint8_t>();

			layers_.push_back(LegacyFrame(type, memory_.data<uint8_t>() + offset, false));
		}
	}
	else
	{
		layers_ = framePyramid.layers_;
	}

	if (framePyramid.memory_)
	{
		memory_ = Memory(framePyramid.memory_.constdata(), framePyramid.memory_.size());
	}
}

FramePyramid::FramePyramid(const unsigned int layers, const FrameType& frameType)
{
	ocean_assert(frameType.isValid() && layers >= 1u);

	const bool result = replace(frameType, true /*forceOwner*/, layers);
	ocean_assert_and_suppress_unused(result, result);
}

FramePyramid::FramePyramid(FramePyramid&& framePyramid) noexcept
{
	*this = std::move(framePyramid);
}

FramePyramid::FramePyramid(const FramePyramid& framePyramid, bool copyData,	const unsigned int layerIndex, const unsigned int layerCount, Worker* worker, const DownsamplingMode downsamplingMode, const CallbackDownsampling& customDownsamplingFunction)
{
	ocean_assert(framePyramid.layers() >= 0u);
	ocean_assert(layerIndex < framePyramid.layers());
	ocean_assert(layerCount >= 0u);

	// we should either have a custom downsampling function of a non-custom downsampling mode
	ocean_assert(customDownsamplingFunction.isNull() || downsamplingMode == DM_CUSTOM);

	const LegacyFrame& firstLayer = framePyramid.layers_[layerIndex];
	const unsigned int maxUsableSourceLayers = framePyramid.layers() - layerIndex;

	if (copyData)
	{
		unsigned int resultingLayers;
		const size_t pyramidFrameSize = calculateMemorySize(firstLayer.width(), firstLayer.height(), firstLayer.pixelFormat(), layerCount, true /*includeFirstLayer*/, &resultingLayers);

		if (pyramidFrameSize == 0)
		{
			return;
		}

		unsigned int selectedSourceLayers = min(resultingLayers, maxUsableSourceLayers);

		const LegacyFrame& lastLayer = framePyramid.layers_[layerIndex + selectedSourceLayers - 1u];
		const size_t sizeSelectedSourceLayers = size_t(lastLayer.constdata() - firstLayer.constdata()) + lastLayer.size();

		ocean_assert(framePyramid.memory_.isInside(firstLayer.constdata(), firstLayer.size()));
		ocean_assert(framePyramid.memory_.isInside(lastLayer.constdata(), lastLayer.size()));
		ocean_assert((lastLayer.constdata() == firstLayer.constdata() && lastLayer.size() == firstLayer.size()) || lastLayer.constdata() >= firstLayer.constdata() + firstLayer.size());

		memory_ = Memory(pyramidFrameSize, memoryAlignmentBytes_);
		memcpy(memory_.data(), firstLayer.constdata(), sizeSelectedSourceLayers);

		// Create layers from source:
		layers_.reserve(resultingLayers);
		layers_.push_back(LegacyFrame(firstLayer.frameType(), firstLayer.timestamp(), memory_.data<uint8_t>(), false));

		for (unsigned int index = 1u; index < selectedSourceLayers; index++)
		{
			const LegacyFrame& sourceLayer = framePyramid.layers_[index + layerIndex];
			LegacyFrame& previousLayer = layers_[index - 1u];

			layers_.push_back(LegacyFrame(sourceLayer.frameType(), sourceLayer.timestamp(), previousLayer.data() + previousLayer.size(), false));
		}

		// Create additional missing layers, if frame data is not a reference:
		if (resultingLayers > layers_.size())
		{
			// Pyramid frame needs to hold requested number of layers:
			ocean_assert(memory_.size() >= calculateMemorySize(finestWidth(), finestHeight(), finestLayer().pixelFormat(), resultingLayers, true /*includeFirstLayer*/, nullptr));

			for (unsigned int n = (unsigned int)(layers_.size()); n < resultingLayers && layers_[n - 1u].width() > 1u && layers_[n - 1u].height() > 1u; ++n)
			{
				ocean_assert(n == (unsigned int)layers_.size());
				if (!addLayer(worker, downsamplingMode, customDownsamplingFunction))
				{
					break;
				}
			}
		}
	}
	else
	{
		const unsigned int selectedSourceLayers = min(layerCount, maxUsableSourceLayers);
		const LegacyFrame& lastLayer = framePyramid.layers_[layerIndex + selectedSourceLayers - 1u];

		ocean_assert(framePyramid.memory_.isInside(firstLayer.constdata(), firstLayer.size()));
		ocean_assert(framePyramid.memory_.isInside(lastLayer.constdata(), lastLayer.size()));
		ocean_assert((lastLayer.constdata() == firstLayer.constdata() && lastLayer.size() == firstLayer.size()) || lastLayer.constdata() >= firstLayer.constdata() + firstLayer.size());

		ocean_assert(lastLayer.constdata() - firstLayer.constdata() >= 0);
		memory_ = Memory(firstLayer.constdata(), lastLayer.constdata() - firstLayer.constdata() + lastLayer.size());

		layers_.reserve(selectedSourceLayers);
		layers_.push_back(LegacyFrame(firstLayer.frameType(), firstLayer.timestamp(), firstLayer.constdata(), false));

		for (unsigned int index = 1u; index < selectedSourceLayers; index++)
		{
			const LegacyFrame& sourceLayer = framePyramid.layers_[index + layerIndex];
			const LegacyFrame& previousLayer = layers_[index - 1u];

			layers_.push_back(LegacyFrame(sourceLayer.frameType(), sourceLayer.timestamp(), previousLayer.constdata() + previousLayer.size(), false));
		}
	}
}

bool FramePyramid::replace(const FrameType& frameType, const bool reserveFirstLayerMemory, const bool forceOwner, const unsigned int layers)
{
	ocean_assert(frameType.isValid());
	ocean_assert(layers >= 1u);

	if (!frameType.isValid() || layers == 0u)
	{
		return false;
	}

	unsigned int resultingLayers = 0u;
	const size_t bytes = calculateMemorySize(frameType.width(), frameType.height(), frameType.pixelFormat(), layers, reserveFirstLayerMemory, &resultingLayers);

	if (resultingLayers == 0u)
	{
		ocean_assert(false && "This should never happen!");
		ocean_assert(bytes == 0);

		return false;
	}

	if (bytes == 0 && reserveFirstLayerMemory)
	{
		ocean_assert(false && "This should never happen!");

		return false;
	}

	if (resultingLayers <= layers_.size() && layers_.front().frameType() == frameType && resultingLayers <= (unsigned int)(layers_.size()))
	{
		// in the case the frame pyramid has the correct size and the correct frame type we may be done

		if (!forceOwner || isOwner())
		{
			if (resultingLayers < layers_.size())
			{
				layers_.resize(resultingLayers);
			}

			return true;
		}
	}

	if (bytes > memory_.size())
	{
		memory_ = Memory(bytes, memoryAlignmentBytes_);
	}

	if (bytes != 0 && !memory_)
	{
		// we seem to be out of memory
		return false;
	}

	layers_.clear();
	layers_.reserve(resultingLayers);

	unsigned int layerWidth = frameType.width();
	unsigned int layerHeight = frameType.height();

	uint8_t* layerData = memory_.data<uint8_t>();

	unsigned int firstLayerIndex = 0u;

	if (!reserveFirstLayerMemory)
	{
		layers_.emplace_back(); // place holder frame, which needs to be initialized outside of this function

		layerWidth /= 2u;
		layerHeight /= 2u;

		firstLayerIndex = 1u;
	}

	for (unsigned int n = firstLayerIndex; n < resultingLayers; ++n)
	{
		ocean_assert(layerWidth >= 1u && layerHeight >= 1u);

		const FrameType layerFrameType(frameType, layerWidth, layerHeight);

		layers_.push_back(LegacyFrame(layerFrameType, layerData, false));

		layerWidth /= 2u;
		layerHeight /= 2u;
		layerData += layerFrameType.frameTypeSize();
	}

	return true;
}

bool FramePyramid::addLayer(Worker* worker, const DownsamplingMode downsamplingMode, const CallbackDownsampling& customDownsamplingFunction)
{
	ocean_assert(!layers_.empty());

	// we should either have a custom downsampling function of a non-custom downsampling mode
	ocean_assert(customDownsamplingFunction.isNull() || downsamplingMode == DM_CUSTOM);

	LegacyFrame& oldCoarsestLayer = layers_.back();
	const unsigned int newLayerIndex = (unsigned int)layers_.size();

	const unsigned int layerWidth = oldCoarsestLayer.width() / 2u;
	const unsigned int layerHeight = oldCoarsestLayer.height() / 2u;

	ocean_assert(oldCoarsestLayer && !oldCoarsestLayer.isReadOnly());

	const FrameType layerFrameType(oldCoarsestLayer.frameType(), layerWidth, layerHeight);
	layers_.push_back(LegacyFrame(layerFrameType, oldCoarsestLayer.timestamp(), oldCoarsestLayer.data() + oldCoarsestLayer.size(), false));

	ocean_assert((unsigned int)layers_.size() == newLayerIndex + 1u);

	if (customDownsamplingFunction)
	{
		customDownsamplingFunction(layers_[newLayerIndex - 1u], layers_[newLayerIndex], worker);
	}
	else
	{
		switch (downsamplingMode)
		{
			case DM_FILTER_14641:
				FrameShrinker::downsampleByTwo14641(layers_[newLayerIndex - 1u], layers_[newLayerIndex], worker);
				break;

			default:
				ocean_assert(downsamplingMode == DM_FILTER_11);

				if (layers_[newLayerIndex - 1u].hasAlphaChannel())
				{
					Frame targetLayer(layers_[newLayerIndex], Frame::temporary_ACM_USE_KEEP_LAYOUT);

					FrameShrinkerAlpha::Comfort::divideByTwo<false>(Frame(layers_[newLayerIndex - 1u], Frame::temporary_ACM_USE_KEEP_LAYOUT), targetLayer, worker);
				}
				else
				{
					FrameShrinker::downsampleByTwo11(layers_[newLayerIndex - 1u], layers_[newLayerIndex], worker);
				}

				break;
		}
	}

	ocean_assert(!layers_[newLayerIndex].isOwner());

	return true;
}

bool FramePyramid::replace(const Frame& frame, const DownsamplingMode downsamplingMode, const unsigned int layers, Worker* worker)
{
	ocean_assert(frame.isValid());
	ocean_assert(layers >= 1u);

	//**TODO** call downsampling function-based replace() function here

	if (downsamplingMode == DM_FILTER_11 && !frame.hasAlphaChannel())
	{
		return replace8BitPerChannel11(frame, layers, true /*copyFirstLayer*/, worker);
	}

	if (!replace(frame.frameType(), true /*reserveFirstLayerMemory*/, true /*forceOwner*/, layers))
	{
		clear();

		return false;
	}

	ocean_assert(!layers_.empty());
	ocean_assert(memory_.size() >= frame.frameTypeSize());

	constexpr unsigned int memoryPaddingElements = 0u;
	Frame memoryFinestLayer(frame.frameType(), memory_.data(), Frame::CM_USE_KEEP_LAYOUT, memoryPaddingElements);

	if (!memoryFinestLayer.copy(0, 0, frame))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	if (downsamplingMode == DM_FILTER_11)
	{
		ocean_assert(frame.hasAlphaChannel());

		for (size_t layerIndex = 1; layerIndex < layers_.size(); ++layerIndex)
		{
			const Frame sourceLayer(layers_[layerIndex - 1u], Frame::temporary_ACM_USE_KEEP_LAYOUT);
			Frame targetLayer(layers_[layerIndex], Frame::temporary_ACM_USE_KEEP_LAYOUT);

			if (!FrameShrinkerAlpha::Comfort::divideByTwo<false>(sourceLayer, targetLayer, worker))
			{
				clear();

				return false;
			}
		}
	}
	else
	{
		ocean_assert(downsamplingMode == DM_FILTER_14641);

		for (size_t layerIndex = 1; layerIndex < layers_.size(); ++layerIndex)
		{
			const Frame sourceLayer(layers_[layerIndex - 1u], Frame::temporary_ACM_USE_KEEP_LAYOUT);
			Frame targetLayer(layers_[layerIndex], Frame::temporary_ACM_USE_KEEP_LAYOUT);

			if (!FrameShrinker::downsampleByTwo14641(sourceLayer, targetLayer, worker))
			{
				clear();

				return false;
			}
		}
	}

	for (LegacyFrame& layer : layers_)
	{
		layer.setTimestamp(frame.timestamp());
	}

	return true;
}

bool FramePyramid::replace(Frame&& frame, const DownsamplingMode downsamplingMode, const unsigned int layers, Worker* worker)
{
	ocean_assert(frame.isValid());
	ocean_assert(layers >= 1u);

	//**TODO** call downsampling function-based replace() function here

	if (downsamplingMode == DM_FILTER_11 && !frame.hasAlphaChannel())
	{
		if (!replace8BitPerChannel11(frame, layers, false /*copyFirstLayer*/, worker))
		{
			return false;
		}

		layers_[0] = std::move(frame);

		return true;
	}

	if (!replace(frame.frameType(), false /*reserveFirstLayerMemory*/, true /*forceOwner*/, layers))
	{
		clear();

		return false;
	}

	ocean_assert(!layers_.empty());

	layers_[0] = std::move(frame);

	if (downsamplingMode == DM_FILTER_11)
	{
		ocean_assert(frame.hasAlphaChannel());

		for (size_t layerIndex = 1u; layerIndex < layers_.size(); ++layerIndex)
		{
			const Frame sourceLayer(layers_[layerIndex - 1u], Frame::temporary_ACM_USE_KEEP_LAYOUT);
			Frame targetLayer(layers_[layerIndex], Frame::temporary_ACM_USE_KEEP_LAYOUT);

			if (!FrameShrinkerAlpha::Comfort::divideByTwo<false>(sourceLayer, targetLayer, worker))
			{
				clear();

				return false;
			}
		}
	}
	else
	{
		ocean_assert(downsamplingMode == DM_FILTER_14641);

		for (size_t layerIndex = 1u; layerIndex < layers_.size(); ++layerIndex)
		{
			const Frame sourceLayer(layers_[layerIndex - 1u], Frame::temporary_ACM_USE_KEEP_LAYOUT);
			Frame targetLayer(layers_[layerIndex], Frame::temporary_ACM_USE_KEEP_LAYOUT);

			if (!FrameShrinker::downsampleByTwo14641(sourceLayer, targetLayer, worker))
			{
				clear();

				return false;
			}
		}
	}

	for (size_t layerIndex = 1; layerIndex < layers_.size(); ++layerIndex)
	{
		layers_[layerIndex].setTimestamp(layers_[0].timestamp());
	}

	return true;
}

bool FramePyramid::replace(const Frame& frame, const CallbackDownsampling& downsamplingFunction, const unsigned int layers, Worker* worker)
{
	ocean_assert(frame.isValid());
	ocean_assert(layers >= 1u);
	ocean_assert(downsamplingFunction);

	if (!downsamplingFunction)
	{
		clear();

		return false;
	}

	if (!replace(frame.frameType(), true /*reserveFirstLayerMemory*/, true /*forceOwner*/, layers))
	{
		clear();

		return false;
	}

	ocean_assert(!layers_.empty());
	ocean_assert(memory_.size() >= frame.frameTypeSize());

	constexpr unsigned int memoryPaddingElements = 0u;
	Frame memoryFinestLayer(frame.frameType(), memory_.data(), Frame::CM_USE_KEEP_LAYOUT, memoryPaddingElements);

	if (!memoryFinestLayer.copy(0, 0, frame))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	for (size_t layerIndex = 1; layerIndex < layers_.size(); ++layerIndex)
	{
		const Frame sourceLayer(layers_[layerIndex - 1u], Frame::temporary_ACM_USE_KEEP_LAYOUT);
		Frame targetLayer(layers_[layerIndex], Frame::temporary_ACM_USE_KEEP_LAYOUT);

		if (!downsamplingFunction(layers_[layerIndex - 1u], layers_[layerIndex], worker))
		{
			clear();

			return false;
		}
	}

	for (LegacyFrame& layer : layers_)
	{
		layer.setTimestamp(frame.timestamp());
	}

	return true;
}

bool FramePyramid::replace(Frame&& frame, const CallbackDownsampling& downsamplingFunction, const unsigned int layers, Worker* worker)
{
	ocean_assert(frame.isValid());
	ocean_assert(layers >= 1u);
	ocean_assert(downsamplingFunction);

	if (!downsamplingFunction)
	{
		clear();

		return false;
	}

	if (!replace(frame.frameType(), false /*reserveFirstLayerMemory*/, true /*forceOwner*/, layers))
	{
		clear();

		return false;
	}

	ocean_assert(!layers_.empty());

	layers_[0] = std::move(frame);

	for (size_t layerIndex = 1; layerIndex < layers_.size(); ++layerIndex)
	{
		const Frame sourceLayer(layers_[layerIndex - 1u], Frame::temporary_ACM_USE_KEEP_LAYOUT);
		Frame targetLayer(layers_[layerIndex], Frame::temporary_ACM_USE_KEEP_LAYOUT);

		if (!downsamplingFunction(layers_[layerIndex - 1u], layers_[layerIndex], worker))
		{
			clear();

			return false;
		}
	}

	for (size_t layerIndex = 1; layerIndex < layers_.size(); ++layerIndex)
	{
		layers_[layerIndex].setTimestamp(layers_[0].timestamp());
	}

	return true;
}

bool FramePyramid::replace8BitPerChannel11(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int channels, const FrameType::PixelOrigin pixelOrigin, const unsigned int layers, const unsigned int framePaddingElements, const bool copyFirstLayer, Worker* worker, const FrameType::PixelFormat pixelFormat, const Timestamp timestamp)
{
	ocean_assert(frame != nullptr && width >= 1u && height >= 1u && layers > 0u);
	ocean_assert(channels >= 1u);
	ocean_assert(layers >= 1u);

	if (frame == nullptr || width == 0u || height == 0u || channels == 0u || layers == 0u)
	{
		clear();
		return false;
	}

	const FrameType::PixelFormat genericPixelFormat = FrameType::genericPixelFormat<uint8_t>(channels);

	ocean_assert(pixelFormat == FrameType::FORMAT_UNDEFINED || FrameType::arePixelFormatsCompatible(pixelFormat, genericPixelFormat));

	const FrameType::PixelFormat usePixelFormat = pixelFormat != FrameType::FORMAT_UNDEFINED ? pixelFormat : genericPixelFormat;

	unsigned int expectedLayers = 0u;
	size_t bytes = calculateMemorySize(width, height, usePixelFormat, layers, copyFirstLayer, &expectedLayers);

	ocean_assert(expectedLayers <= layers);

	if (expectedLayers == 0u)
	{
		ocean_assert(bytes == 0 && expectedLayers == 0u);
		return false;
	}

	ocean_assert(bytes != 0 || (expectedLayers == 1u && !copyFirstLayer)); // in case we don't copy the first layer, but we also don't need more than one layer, we don't need any memory

	if (bytes > memory_.size())
	{
		memory_ = Memory(bytes, memoryAlignmentBytes_);
	}

	if (bytes != 0 && !memory_)
	{
		// we seem to be out of memory
		return false;
	}

	layers_.clear();
	layers_.reserve(expectedLayers);

	if (FrameShrinker::pyramidByTwo8BitPerChannel11(frame, memory_.data<uint8_t>(), width, height, channels, memory_.size(), expectedLayers, framePaddingElements, copyFirstLayer, worker))
	{
		if (copyFirstLayer)
		{
			// the pyramid memory contains the first layer (writable)

			layers_.push_back(LegacyFrame(FrameType(width, height, usePixelFormat, pixelOrigin), timestamp, memory_.data<uint8_t>(), false));
		}
		else
		{
			// the pyramid memory does not contain the first layer, so we use the provided frame (as read-only)

			Frame tmpFrame(FrameType(width, height, usePixelFormat, pixelOrigin), frame, Frame::CM_USE_KEEP_LAYOUT, framePaddingElements, timestamp);

			layers_.push_back(LegacyFrame(tmpFrame, LegacyFrame::FCM_USE_IF_POSSIBLE)); // **TODO** temporary workaround until `layers_` does not use `LegacyFrame` anymore
		}

		uint8_t* pyramidLayerData = memory_.data<uint8_t>();

		if (copyFirstLayer)
		{
			// the first layer is part of the pyramid's memory
			pyramidLayerData += layers_.front().frameTypeSize();
		}

		for (unsigned int n = 1u; n < expectedLayers; ++n)
		{
			ocean_assert(layers_.size() == n);
			ocean_assert(layers_.back());

			const unsigned int layerWidth = layers_.back().width() / 2u;
			const unsigned int layerHeight = layers_.back().height() / 2u;

			const FrameType layerFrameType(layerWidth, layerHeight, usePixelFormat, pixelOrigin);

			layers_.push_back(LegacyFrame(layerFrameType, timestamp, pyramidLayerData, false));
			ocean_assert(layers_.size() == n + 1u);
			ocean_assert(!layers_.back().isOwner());

			pyramidLayerData += layerFrameType.frameTypeSize();
		}
	}
	else
	{
		ocean_assert(false && "This should never happen!");
	}

#ifdef OCEAN_DEBUG

	for (size_t n = 0; n < layers_.size(); ++n)
	{
		if (n == 0 && !copyFirstLayer)
		{
			// skipping the very first layer
			continue;
		}

		ocean_assert(memory_.isInside(layers_[n].constdata(), layers_[n].size()));
	}

#endif // OCEAN_DEBUG

	return true;
}

void FramePyramid::reduceLayers(const size_t layers)
{
	ocean_assert(layers <= layers_.size());

	if (layers == 0)
	{
		clear();
	}
	else if (layers < layers_.size())
	{
		layers_.resize(layers);
	}
}

bool FramePyramid::isOwner(const unsigned int layerIndex) const
{
	if (layerIndex == (unsigned int)(-1))
	{
		if (layers_.empty())
		{
			return false;
		}

		for (const LegacyFrame& layer : layers_)
		{
			if (layer.isOwner())
			{
				continue;
			}

			if (memory_.isOwner() && memory_.isInside(layer.constdata(), layer.size()))
			{
				continue;
			}

			return false;
		}

		return true;
	}

	if (layerIndex < layers_.size())
	{
		const LegacyFrame& layer = layers_[layerIndex];

		if (layer.isOwner())
		{
			return true;
		}

		if (memory_.isOwner() && memory_.isInside(layer.constdata(), layer.size()))
		{
			return true;
		}
	}

	return false;
}

unsigned int FramePyramid::idealLayers(const unsigned int width, const unsigned int height, const unsigned int invalidWidthOrHeight)
{
	ocean_assert(width >= 1u && height >= 1u);

	unsigned int layerSize = std::min(width, height);

	if (layerSize <= invalidWidthOrHeight)
	{
		// the resolution is already too small for one pyramid layer
		return 0u;
	}

	ocean_assert(invalidWidthOrHeight < layerSize);

	unsigned int layers = 1u;

	while (true)
	{
		const unsigned int nextLayerSize = layerSize / 2u;

		if (nextLayerSize <= invalidWidthOrHeight)
		{
			break;
		}

		layerSize = nextLayerSize;

		++layers;
	}

	return layers;
}

unsigned int FramePyramid::idealLayers(const unsigned int width, const unsigned int height, const unsigned int invalidWidth, const unsigned int invalidHeight)
{
	ocean_assert(width >= 1u && height >= 1u);

	if (width <= invalidWidth || height <= invalidHeight)
	{
		// the resolution is already too small for one pyramid layer
		return 0u;
	}

	unsigned int layerWidth = width;
	unsigned int layerHeight = height;
	ocean_assert(invalidWidth < layerWidth);
	ocean_assert(invalidHeight < layerHeight);

	unsigned int layers = 1u;

	while (true)
	{
		const unsigned int nextLayerWidth = layerWidth / 2u;
		const unsigned int nextLayerHeight = layerHeight / 2u;

		if (nextLayerWidth <= invalidWidth || nextLayerHeight <= invalidHeight)
		{
			break;
		}

		layerWidth = nextLayerWidth;
		layerHeight = nextLayerHeight;

		++layers;
	}

	return layers;
}

unsigned int FramePyramid::idealLayers(const unsigned int width, const unsigned int height, const unsigned int invalidWidth, const unsigned int invalidHeight, const unsigned int layerFactor, const unsigned int maximalRadius, const unsigned int coarsestLayerRadius)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(layerFactor >= 2u && coarsestLayerRadius >= 2u);

	if (width <= invalidWidth || height <= invalidHeight)
	{
		// the resolution is already too small for one pyramid layer
		return 0u;
	}

	unsigned int layerWidth = width;
	unsigned int layerHeight = height;
	unsigned int totalRadius = coarsestLayerRadius;

	unsigned int layers = 1u;

	while (true)
	{
		const unsigned int nextLayerWidth = layerWidth / layerFactor;
		const unsigned int nextLayerHeight = layerHeight / layerFactor;

		if (nextLayerWidth <= invalidWidth || nextLayerHeight <= invalidHeight)
		{
			break;
		}

		if (totalRadius >= maximalRadius)
		{
			break;
		}

		layerWidth = nextLayerWidth;
		layerHeight = nextLayerHeight;

		totalRadius *= layerFactor;

		++layers;
	}

	return layers;
}

FramePyramid& FramePyramid::operator=(const FramePyramid& right)
{
	if (this != &right)
	{
		if (right.isNull())
		{
			clear();
			return *this;
		}

		ocean_assert(right.memory_.isOwner());

		memory_ = Memory(right.memory_.size(), memoryAlignmentBytes_);
		memcpy(memory_.data(), right.memory_.constdata(), memory_.size());

		const LegacyFrame& frame = right.finestLayer();

		layers_.clear();
		layers_.push_back(LegacyFrame(frame.frameType(), frame.timestamp(), memory_.data<uint8_t>(), false));

		for (unsigned int n = 1; n < right.layers() && layers_[n - 1u].width() > 1u && layers_[n - 1u].height() > 1u; ++n)
		{
			const unsigned int layerWidth = layers_[n - 1u].width() / 2u;
			const unsigned int layerHeight = layers_[n - 1u].height() / 2u;

			layers_.push_back(LegacyFrame(FrameType(layerWidth, layerHeight, frame.pixelFormat(), frame.pixelOrigin()), frame.timestamp(), layers_[n - 1u].data() + layers_[n - 1u].size(), false));
		}
	}

	return *this;
}

FramePyramid& FramePyramid::operator=(FramePyramid&& right) noexcept
{
	if (this != &right)
	{
		clear();

		layers_ = std::move(right.layers_);
		memory_ = std::move(right.memory_);
	}

	return *this;
}

bool FramePyramid::addLayer11(Worker* worker)
{
	ocean_assert(!layers_.empty());

	LegacyFrame& oldCoarsestLayer = layers_.back();
	const unsigned int newLayerIndex = (unsigned int)layers_.size();

	const unsigned int layerWidth = oldCoarsestLayer.width() / 2u;
	const unsigned int layerHeight = oldCoarsestLayer.height() / 2u;

	ocean_assert(oldCoarsestLayer && !oldCoarsestLayer.isReadOnly());

	const FrameType layerFrameType(oldCoarsestLayer.frameType(), layerWidth, layerHeight);
	layers_.push_back(LegacyFrame(layerFrameType, oldCoarsestLayer.timestamp(), oldCoarsestLayer.data() + oldCoarsestLayer.size(), false));

	ocean_assert((unsigned int)layers_.size() == newLayerIndex + 1u);

	FrameShrinker::downsampleByTwo8BitPerChannel11(layers_[newLayerIndex - 1u].constdata(), layers_[newLayerIndex].data(), layers_[newLayerIndex - 1u].width(), layers_[newLayerIndex - 1u].height(), layerFrameType.channels(), 0u, 0u, worker);

	ocean_assert(!layers_[newLayerIndex].isOwner());

	return true;
}

size_t FramePyramid::calculateMemorySize(const unsigned int width, const unsigned int height, const FrameType::PixelFormat pixelFormat, const unsigned int layers, const bool includeFirstLayer, unsigned int* totalLayers)
{
	ocean_assert(width <= 65535u && height <= 65535u);

	if (width == 0u || height == 0u || width > 65535u || height > 65535u)
	{
		return 0;
	}

	ocean_assert(FrameType::numberPlanes(pixelFormat) == 1u && FrameType::formatIsGeneric(pixelFormat));
	if (FrameType::numberPlanes(pixelFormat) != 1u || !FrameType::formatIsGeneric(pixelFormat))
	{
		return 0;
	}

	const unsigned int bytesPerPixel = FrameType::channels(pixelFormat) * FrameType::bytesPerDataType(FrameType::dataType(pixelFormat));
	ocean_assert(bytesPerPixel <= 256u);

	if (bytesPerPixel > 256u)
	{
		return 0;
	}

	unsigned int layerWidth = width;
	unsigned int layerHeight = height;

	unsigned int layerIndex = 0u;
	uint64_t bytes = 0u;

	while (layerIndex < layers && layerWidth >= 1u && layerHeight >= 1u)
	{
		const uint64_t layerPixels = uint64_t(layerWidth) * uint64_t(layerHeight);
		const uint64_t layerBytes = layerPixels * uint64_t(bytesPerPixel);

		if (!NumericT<size_t>::isInsideValueRange(layerBytes))
		{
			ocean_assert(false && "This should never happen!");
			return 0;
		}

		if (!NumericT<size_t>::isInsideValueRange(bytes + layerBytes))
		{
			ocean_assert(false && "This should never happen!");
			return 0;
		}

		if (layerIndex != 0u || includeFirstLayer)
		{
			bytes += layerBytes;
		}

		++layerIndex;

		layerWidth /= 2u;
		layerHeight /= 2u;
	}

	if (totalLayers != nullptr)
	{
		*totalLayers = layerIndex;
	}

	ocean_assert(NumericT<size_t>::isInsideValueRange(bytes));

	return size_t(bytes);
}

}

}
