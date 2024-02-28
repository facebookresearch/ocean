// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/unifiedfeatures/deeppatchuint8/DP8FeatureDetector.h"
#include "ocean/unifiedfeatures/deeppatchuint8/DP8FeatureContainer.h"
#include "ocean/unifiedfeatures/deeppatchuint8/DP8DescriptorContainer.h"

#include "ocean/base/WorkerPool.h"

#include <perception/features/deep/DeepUtils.h>

namespace Ocean
{

namespace UnifiedFeatures
{

namespace DeepPatchUint8
{

DescriptorGenerator::Result DP8FeatureDetector::DP8DescriptorGenerator::generateDescriptors(const Frame& /* frame - ignored */, const FeatureContainer& /* featureContainer - ignored */, std::shared_ptr<DescriptorContainer>& descriptorContainer, Worker* /* worker - ignored */) const
{
	descriptorContainer = std::make_shared<DP8DescriptorContainer>(std::move(descriptors_));

	return Result::SUCCESS;
}

FeatureDetector::Result DP8FeatureDetector::detectFeatures(const Frame& frame, std::shared_ptr<FeatureContainer>& featureContainer, const unsigned int border, Worker* worker) const
{
	ocean_assert(frame.isValid());

	if (!FrameType::arePixelFormatsCompatible(frame.pixelFormat(), FrameType::FORMAT_Y8))
	{
		ocean_assert(false && "Invalid pixel format");
		return Result::ERROR_PIXEL_FORMAT_INVALID;
	}

	const perception::ImageSlice<const uint8_t> perceptionImageSlice(const_cast<uint8_t*>(frame.constdata<uint8_t>()), frame.width(), frame.height(), frame.strideElements());

	// The extraction function returns a float array, which has to be converted.
	std::vector<perception::features::Keypoint> keypoints;
	std::vector<std::array<float, dp8Descriptor96Size>> floatDescriptors;
	deepDescriptorPipeline_->extractDeepDescriptors<dp8Descriptor96Size>(perceptionImageSlice, keypoints, floatDescriptors);

	if (keypoints.size() != floatDescriptors.size())
	{
		ocean_assert(false && "This should never happen!");
		return Result::GENERAL_ERROR;
	}

	DP8Descriptors96 uint8Descriptors;
	uint8Descriptors.reserve(floatDescriptors.size());

	for (const std::array<float, dp8Descriptor96Size>& floatDescriptor : floatDescriptors)
	{
		DP8Descriptor96 uint8Descriptor = perception::features::toUint8Descriptor(floatDescriptor);
		uint8Descriptors.emplace_back(std::move(uint8Descriptor));
	}

	featureContainer = std::make_shared<DP8FeatureContainer>(std::move(keypoints));
	descriptorGenerator_->setDescriptors(std::move(uint8Descriptors));

	return Result::SUCCESS;
}

} // namespace OceanFreak

} // namespace UnifiedFeatures

} // namespace Ocean
