// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/unifiedfeatures/oceanorb/OODescriptorGenerator.h"
#include "ocean/unifiedfeatures/oceanorb/OODescriptorContainer.h"

#include "ocean/unifiedfeatures/FeatureDetector.h"

#include "ocean/base/WorkerPool.h"

#include "ocean/cv/IntegralImage.h"

#include "ocean/cv/detector/ORBFeatureDescriptor.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace OceanOrb
{

DescriptorGenerator::Result OODescriptorGenerator::generateDescriptors(const Frame& frame, const FeatureContainer& featureContainer, std::shared_ptr<DescriptorContainer>& descriptors, Worker* worker) const
{
	ocean_assert(frame.isValid());

	if (!FrameType::arePixelFormatsCompatible(frame.pixelFormat(), FrameType::FORMAT_Y8))
	{
		ocean_assert(false && "Invalid pixel format");
		return Result::ERROR_PIXEL_FORMAT_INVALID;
	}

	// Compute the ORB descriptors
	Frame linedIntegralFrame(FrameType(frame.width() + 1u, frame.height() + 1u, FrameType::FORMAT_Y32, FrameType::ORIGIN_UPPER_LEFT));
	uint32_t* const linedIntegralFrameData = linedIntegralFrame.data<uint32_t>();
	CV::IntegralImage::createLinedImage<uint8_t, uint32_t, 1u>(frame.constdata<uint8_t>(), linedIntegralFrameData, frame.width(), frame.height(), frame.paddingElements(), linedIntegralFrame.paddingElements());

	const Keypoints& keypoints = featureContainer.keypoints();

	CV::Detector::ORBFeatures orbFeatures;
	orbFeatures.reserve(featureContainer.size());
	for (size_t i = 0; i < keypoints.size(); ++i)
	{
		const Keypoint& keypoint = keypoints[i];
		const float strength = keypoint.hasStrength() ? keypoint.strength() : 0.0f;
		const float orientation = keypoint.hasOrientation() ? keypoint.orientation() : 0.0f;
		orbFeatures.emplace_back(Vector2(keypoint.observation()), CV::Detector::ORBFeature::DS_UNKNOWN, Scalar(strength), Scalar(orientation));
	}
	ocean_assert(orbFeatures.size() == featureContainer.size());

	CV::Detector::ORBFeatureDescriptor::determineDescriptors(linedIntegralFrameData, frame.width(), frame.height(), orbFeatures, useSubLayers_, worker);

	descriptors = std::make_shared<OODescriptorContainer>(std::move(orbFeatures));

	return Result::SUCCESS;
}

bool OODescriptorGenerator::isSupportedFeatureDetector(const FeatureDetector& featureDetector) const
{
	return featureDetector.featureCategory() == FeatureCategory::KEYPOINTS;
}

} // namespace OceanOrb

} // namespace UnifiedFeatures

} // namespace Ocean
