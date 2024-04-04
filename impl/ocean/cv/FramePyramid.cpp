// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/cv/FramePyramid.h"
#include "ocean/cv/FrameShrinker.h"

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

FramePyramid::FramePyramid(const FrameType& frameType, const unsigned int layers)
{
	resize(frameType, layers);
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
		const size_t pyramidFrameSize = size(firstLayer.width(), firstLayer.height(), firstLayer.pixelFormat(), layerCount, &resultingLayers);

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
			ocean_assert(memory_.size() >= size(finestWidth(), finestHeight(), finestLayer().pixelFormat(), resultingLayers));

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

bool FramePyramid::replace(const Frame& frame, const unsigned int layers, Worker* worker, const DownsamplingMode downsamplingMode, const CallbackDownsampling& customDownsamplingFunction)
{
	return replace(LegacyFrame(frame, LegacyFrame::FCM_USE_IF_POSSIBLE), layers, worker, downsamplingMode, customDownsamplingFunction);
}

bool FramePyramid::replace(const LegacyFrame& frame, const unsigned int layers, Worker* worker, const DownsamplingMode downsamplingMode, const CallbackDownsampling& customDownsamplingFunction)
{
	ocean_assert(frame);
	ocean_assert(layers != 0u);

	// we should either have a custom downsampling function of a non-custom downsampling mode
	ocean_assert(customDownsamplingFunction.isNull() || downsamplingMode == DM_CUSTOM);

	if (!(layers_.empty() || isOwner()))
	{
		ocean_assert(false && "Only pyramids that are uninitialized or own their frame data may proceeded");
		return false;
	}

	if (layers != 0u)
	{
		unsigned int expectedLayers = 0u;
		const size_t bytes = size(frame.width(), frame.height(), frame.pixelFormat(), layers, &expectedLayers);

		if (bytes == 0)
		{
			return false;
		}

		memory_ = Memory(bytes, memoryAlignmentBytes_);

		layers_.clear();
		layers_.reserve(expectedLayers);

		if (downsamplingMode == DM_FILTER_11 && FrameShrinker::pyramidByTwo11(Frame(frame, Frame::temporary_ACM_USE_KEEP_LAYOUT), memory_.data<uint8_t>(), memory_.size(), expectedLayers, worker))
		{
			layers_.push_back(LegacyFrame(frame.frameType(), frame.timestamp(), memory_.data<uint8_t>(), false));

			for (unsigned int n = 1u; n < expectedLayers; ++n)
			{
				ocean_assert(layers_.size() == n);
				ocean_assert(layers_.back() && !layers_.back().isReadOnly());

				const unsigned int layerWidth = layers_.back().width() / 2u;
				const unsigned int layerHeight = layers_.back().height() / 2u;

				layers_.push_back(LegacyFrame(FrameType(layerWidth, layerHeight, frame.pixelFormat(), frame.pixelOrigin()), frame.timestamp(), layers_.back().data() + layers_.back().size(), false));
				ocean_assert(layers_.size() == n + 1u);
				ocean_assert(!layers_[n].isOwner());
			}
		}
		else
		{
			// we create the first layer and simply copy the content
			layers_.push_back(LegacyFrame(frame.frameType(), frame.timestamp(), memory_.data<uint8_t>(), false));
			memcpy(layers_.front().data(), frame.constdata(), frame.size());

			for (unsigned int n = 1u; n < layers && layers_[n - 1u].width() > 1u && layers_[n - 1u].height() > 1u; ++n)
			{
				ocean_assert((unsigned int)layers_.size() == n);
				if (!addLayer(worker, downsamplingMode, customDownsamplingFunction))
				{
					break;
				}
			}
		}
	}

#ifdef OCEAN_DEBUG

	for (size_t n = 0; n < layers_.size(); ++n)
	{
		ocean_assert(memory_.isInside(layers_[n].constdata(), layers_[n].size()));
	}

#endif // OCEAN_DEBUG

	return true;
}

bool FramePyramid::replace8BitPerChannel(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int channels, const FrameType::PixelOrigin pixelOrigin, const unsigned int layers, const unsigned int framePaddingElements, Worker* worker, const Timestamp timestamp)
{
	ocean_assert(frame && width >= 1u && height >= 1u && layers >= 0u);
	ocean_assert(channels >= 1u);

	if (!(layers_.empty() || isOwner()))
	{
		ocean_assert(false && "Only pyramids that are uninitialized or own their frame data may proceeded");
		return false;
	}

	const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, channels);

	if (layers != 0u)
	{
		unsigned int expectedLayers = 0u;
		const size_t bytes = size(width, height, pixelFormat, layers, &expectedLayers);

		if (bytes == 0)
		{
			return false;
		}

		memory_ = Memory(bytes, memoryAlignmentBytes_);

		layers_.clear();
		layers_.reserve(expectedLayers);

		if (FrameShrinker::pyramidByTwo8BitPerChannel11(frame, memory_.data<uint8_t>(), width, height, channels, memory_.size(), expectedLayers, framePaddingElements, worker))
		{
			layers_.push_back(LegacyFrame(FrameType(width, height, pixelFormat, pixelOrigin), timestamp, memory_.data<uint8_t>(), false));

			for (unsigned int n = 1u; n < expectedLayers; ++n)
			{
				ocean_assert(layers_.size() == n);
				ocean_assert(layers_.back() && !layers_.back().isReadOnly());

				const unsigned int layerWidth = layers_.back().width() / 2u;
				const unsigned int layerHeight = layers_.back().height() / 2u;

				layers_.push_back(LegacyFrame(FrameType(layerWidth, layerHeight, pixelFormat, pixelOrigin), timestamp, layers_.back().data() + layers_.back().size(), false));
				ocean_assert(layers_.size() == n + 1u);
				ocean_assert(!layers_[n].isOwner());
			}
		}
		else
		{
			ocean_assert(false && "This should never happen!");
		}
	}

#ifdef OCEAN_DEBUG

	for (size_t n = 0; n < layers_.size(); ++n)
	{
		ocean_assert(memory_.isInside(layers_[n].constdata(), layers_[n].size()));
	}

#endif // OCEAN_DEBUG

	return true;
}

bool FramePyramid::resize(const FrameType& frameType, const unsigned int layers)
{
	ocean_assert(frameType.isValid());
	ocean_assert(layers >= 1u);

	if (!(layers_.empty() || isOwner()))
	{
		ocean_assert(false && "Only pyramids that are uninitialized or own their frame data may proceeded");
		return false;
	}

	unsigned int resultingLayers = 0u;
	const size_t bytes = size(frameType.width(), frameType.height(), frameType.pixelFormat(), layers, &resultingLayers);

	if (bytes == 0)
	{
		return false;
	}

	if (!memory_.isNull())
	{
		ocean_assert(!layers_.empty());

		// in the case the frame pyramid is the correct size and frame type we simply do nothing
		if (layers_.front().frameType() == frameType && resultingLayers <= (unsigned int)layers_.size())
		{
			return true;
		}

		// in the case the frame type is not identical (but we have enough memory already, we simply replace the individual layer frames)
		if (bytes <= memory_.size() && !memory_.isReadOnly())
		{
			layers_.clear();
			layers_.reserve(resultingLayers);

			unsigned int width = frameType.width();
			unsigned int height = frameType.height();
			uint8_t* data = memory_.data<uint8_t>();

			for (unsigned int n = 0u; n < resultingLayers; ++n)
			{
				layers_.push_back(LegacyFrame(FrameType(frameType, width, height), data, false));

				width >>= 1u;
				height >>= 1u;
				data += layers_.back().size();
			}

			return true;
		}

		// we cannot use anything so that we have to re-create a new pyramid

		memory_.free();
		layers_.clear();
	}

	ocean_assert(memory_.isNull() && layers_.empty());

	// in the case this pyramid is empty, we simply reserve enough memory

	if (layers != 0u)
	{
		memory_ = Memory(bytes, memoryAlignmentBytes_);

		layers_.reserve(resultingLayers);

		unsigned int width = frameType.width();
		unsigned int height = frameType.height();
		uint8_t* data = memory_.data<uint8_t>();

		for (unsigned int n = 0u; n < resultingLayers; ++n)
		{
			layers_.push_back(LegacyFrame(FrameType(frameType, width, height), data, false));

			width >>= 1u;
			height >>= 1u;
			data += layers_.back().size();
		}
	}

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
	unsigned int testSize = std::min(width, height);

	unsigned int layers = 1u;

	while (testSize / 2u > invalidWidthOrHeight)
	{
		testSize /= 2u;
		++layers;
	}

	return layers;
}

unsigned int FramePyramid::idealLayers(const unsigned int width, const unsigned int height, const unsigned int invalidWidth, const unsigned int invalidHeight)
{
	unsigned int testWidth = width;
	unsigned int testHeight = height;

	unsigned int layers = 1u;

	while (testWidth / 2u > invalidWidth && testHeight / 2u > invalidHeight)
	{
		testWidth /= 2u;
		testHeight /= 2u;
		++layers;
	}

	return layers;
}

unsigned int FramePyramid::idealLayers(const unsigned int width, const unsigned int height, const unsigned int invalidWidth, const unsigned int invalidHeight, const unsigned int layerFactor, const unsigned int maximalRadius, const unsigned int coarsestLayerRadius)
{
	ocean_assert(width != 0u && height != 0u && layerFactor >= 2u && coarsestLayerRadius >= 2u);

	unsigned int testWidth = width;
	unsigned int testHeight = height;
	unsigned int testFactor = coarsestLayerRadius;

	unsigned int layers = 1u;

	while (testWidth / layerFactor > invalidWidth && testHeight / layerFactor > invalidHeight && testFactor < maximalRadius)
	{
		testWidth /= layerFactor;
		testHeight /= layerFactor;
		testFactor *= layerFactor;
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

size_t FramePyramid::size(const unsigned int width, const unsigned int height, const FrameType::PixelFormat pixelFormat, const unsigned int layers, unsigned int* totalLayers)
{
	ocean_assert(width <= 65535u && height <= 65535u);

	if (width == 0u || height == 0u || width > 65535u || height > 65535u)
	{
		return 0;
	}

	ocean_assert(FrameType::numberPlanes(pixelFormat) == 1u && FrameType::formatIsGeneric(pixelFormat));

	const unsigned int bytesPerPixel = FrameType::channels(pixelFormat) * FrameType::bytesPerDataType(FrameType::dataType(pixelFormat));
	ocean_assert(bytesPerPixel <= 256u);

	if (bytesPerPixel > 256u)
	{
		return 0;
	}

	unsigned int layerWidth = width;
	unsigned int layerHeight = height;

	unsigned int iterations = 0u;
	uint64_t bytes = 0u;

	while (iterations < layers && layerWidth >= 1u && layerHeight >= 1u)
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

		bytes += layerBytes;
		++iterations;

		layerWidth /= 2u;
		layerHeight /= 2u;
	}

	if (totalLayers != nullptr)
	{
		*totalLayers = iterations;
	}

	ocean_assert(NumericT<size_t>::isInsideValueRange(bytes));

	return size_t(bytes);
}

}

}
