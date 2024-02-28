// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/unifiedfeatures/deepdescriptor/DDHarrisDeepPatchFloatFeatureDetector.h"
#include "ocean/unifiedfeatures/deepdescriptor/DDFloatDescriptorContainer.h"
#include "ocean/unifiedfeatures/deepdescriptor/DDHarrisFeatureContainer.h"

#include "ocean/base/WorkerPool.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace DeepDescriptor
{

DescriptorGenerator::Result DDHarrisDeepPatchFloatFeatureDetector::IntegratedHelperDescriptorGenerator::generateDescriptors(const Frame& /* frame - ignored */, const FeatureContainer& /* featureContainer - ignored */, std::shared_ptr<DescriptorContainer>& descriptorContainer, Worker* /* worker - ignored */) const
{
	descriptorContainer = std::make_shared<DDFloatDescriptorContainer>(std::move(deepPatchFloatDescriptors_));

	return Result::SUCCESS;
}

FeatureDetector::Result DDHarrisDeepPatchFloatFeatureDetector::detectFeatures(const Frame& frame, std::shared_ptr<FeatureContainer>& featureContainer, const unsigned int border, Worker* worker) const
{
	ocean_assert(frame.isValid());

	if (!FrameType::arePixelFormatsCompatible(frame.pixelFormat(), FrameType::FORMAT_Y8))
	{
		ocean_assert(false && "Invalid pixel format");
		return Result::ERROR_PIXEL_FORMAT_INVALID;
	}

	if (deepDescriptorFloatPipeline_ == nullptr)
	{
		ocean_assert(false && "Deep descriptor pipeline is not initialized. This should never happen!");
		return Result::GENERAL_ERROR;
	}

	const perception::ImageSlice<const uint8_t> perceptionImageSlice(const_cast<uint8_t*>(frame.constdata<uint8_t>()), frame.width(), frame.height(), frame.strideElements()); // ImageSlice requires non-const pointer

	std::vector<perception::features::Keypoint> perceptionKeypoints;
	DeepPatchFloatDescriptors deepPatchFloatDescriptors;

	deepDescriptorFloatPipeline_->extractDeepDescriptors<deepPatchFloatDescriptorSizeInElements>(perceptionImageSlice, perceptionKeypoints, deepPatchFloatDescriptors);

	if (perceptionKeypoints.size() != deepPatchFloatDescriptors.size())
	{
		ocean_assert(false && "This should never happen!");
		return Result::GENERAL_ERROR;
	}

	featureContainer = std::make_shared<DDHarrisFeatureContainer>(std::move(perceptionKeypoints));
	descriptorGenerator_->setDescriptors(std::move(deepPatchFloatDescriptors));

	return Result::SUCCESS;
}

} // namespace DeepDescriptor

} // namespace UnifiedFeatures

} // namespace Ocean
