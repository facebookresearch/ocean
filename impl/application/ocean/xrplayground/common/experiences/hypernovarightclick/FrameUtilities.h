// (c) Meta Platform, Inc. and its affiliates. Confidential and proprietary.
#pragma once

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	#include <ocean/cv/FrameConverter.h>
	#include <ocean/cv/FrameConverterBGR24.h>
	#include <folly/Optional.h>
	#include <memory>
	#include <xplat/common/bufferpool/BufferPool.hpp>


namespace Ocean::XRPlayground
{

struct ComponentData
{
	size_t bufferStride;
	size_t bufferSize;
	std::unique_ptr<facebook::bufferpool::Buffer> pooledBuffer;

	[[nodiscard]] uint8_t* getBufferPtr() const
	{
		return pooledBuffer ? pooledBuffer->getData() : nullptr;
	}

	ComponentData(std::shared_ptr<facebook::bufferpool::BufferPool> bufferPool,
		size_t stride,
		size_t size) :
		bufferStride(stride),
		bufferSize(size), pooledBuffer(bufferPool->getUniqueBuffer(size)) {}
};


class FrameUtilities
{
	public:
	static folly::Optional<Ocean::Frame> convertToBGRA(const Ocean::Frame& frame);

	static folly::Optional<Ocean::Frame> convertFromBGRAToNV12(
		Ocean::Frame bgraFrame,
		std::shared_ptr<facebook::bufferpool::BufferPool> bufferPool);
};


} // namespace Ocean::XRPlayground


#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
