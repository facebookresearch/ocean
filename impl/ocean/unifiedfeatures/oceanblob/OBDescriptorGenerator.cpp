// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/unifiedfeatures/oceanblob/OBDescriptorGenerator.h"
#include "ocean/base/Base.h"
#include "ocean/unifiedfeatures/FeatureDetector.h"
#include "ocean/unifiedfeatures/Keypoint.h"
#include "ocean/unifiedfeatures/oceanblob/OBDescriptorContainer.h"
#include "ocean/unifiedfeatures/oceanblob/OBFeatureContainer.h"

#include "ocean/base/WorkerPool.h"

#include "ocean/base/Frame.h"
#include "ocean/cv/IntegralImage.h"
#include "ocean/cv/detector/blob/BlobFeature.h"
#include "ocean/cv/detector/blob/BlobFeatureDescriptor.h"
#include "ocean/cv/detector/blob/BlobFeatureDetector.h"
#include "ocean/geometry/SpatialDistribution.h"
#include "ocean/unifiedfeatures/oceanblob/OBFeatureContainer.h"

#include <memory>

namespace Ocean
{

namespace UnifiedFeatures
{

namespace OceanBlob
{

DescriptorGenerator::Result OBDescriptorGenerator::generateDescriptors(
	const Frame& frame,
	const FeatureContainer& featureContainer,
	std::shared_ptr<DescriptorContainer>& descriptorsContainer,
	Worker* worker) const
{
	ocean_assert(frame.isValid());

	if (!FrameType::arePixelFormatsCompatible(frame.pixelFormat(), FrameType::FORMAT_Y8))
	{
		ocean_assert(false && "Invalid pixel format");
		return Result::ERROR_PIXEL_FORMAT_INVALID;
	}

	const unsigned int width = frame.width();
	const unsigned int height = frame.height();

	const OBFeatureContainer* blobFeatureContainer = dynamic_cast<const OBFeatureContainer*>(&featureContainer);

	CV::Detector::Blob::BlobFeatures features;
	Frame linedIntegralFrame;

	// Not the same feture container so we must repopulate features from the keypoints and recompute the integral image.
	if (!blobFeatureContainer)
	{
		// Convert keypoints to blob features.
		for(const Keypoint& kp: featureContainer.keypoints()){
			// Unknown distortion state and laplacian sign is always false.
			features.emplace_back(Vector2(Scalar(kp.observation().x()), Scalar(kp.observation().y())), CV::Detector::Blob::BlobFeature::DS_UNKNOWN, kp.hasScale()? kp.scale(): 1.0f, kp.hasStrength()? kp.strength(): 1.0f, false, kp.hasOrientation()? kp.orientation(): 0.0f);
		}

		// Regenerate the integral image.
		linedIntegralFrame = Frame(FrameType(width + 1u, height + 1u, FrameType::FORMAT_Y32, FrameType::ORIGIN_UPPER_LEFT));
		uint32_t* const linedIntegralFrameData = linedIntegralFrame.data<uint32_t>();
		CV::IntegralImage::createLinedImage<uint8_t, uint32_t, 1u>(frame.constdata<uint8_t>(), linedIntegralFrameData, width, height, frame.paddingElements(), linedIntegralFrame.paddingElements());
	}
	else if (!blobFeatureContainer->checkIfFrameWasUsedForDetection(frame))
	{
		// `featureContainer` is a OBFeatureContainer but the frame address is different. We need to recompute the integral image and reuse the blobFeatures_ in the container.
		features = blobFeatureContainer->blobFeatures();

		linedIntegralFrame = Frame(FrameType(width + 1u, height + 1u, FrameType::FORMAT_Y32, FrameType::ORIGIN_UPPER_LEFT));
		uint32_t* const linedIntegralFrameData = linedIntegralFrame.data<uint32_t>();
		CV::IntegralImage::createLinedImage<uint8_t, uint32_t, 1u>(frame.constdata<uint8_t>(), linedIntegralFrameData, width, height, frame.paddingElements(), linedIntegralFrame.paddingElements());
	}
	else
	{
		// `featureContainer` is a OBFeatureContainer and the frame in feature container is the same as the frame in generator. We can reuse both features and integral image.
		features = blobFeatureContainer->blobFeatures();
		linedIntegralFrame = blobFeatureContainer->integralFrame();
	}

	// Run feature description.
	Ocean::CV::Detector::Blob::BlobFeatureDescriptor::calculateOrientationsAndDescriptors(
		linedIntegralFrame.constdata<unsigned int>(),
		width,
		height,
		Ocean::FrameType::ORIGIN_UPPER_LEFT,
		Ocean::CV::Detector::Blob::BlobFeature::ORIENTATION_SLIDING_WINDOW,
		features,
		true);

	descriptorsContainer = std::make_shared<OBDescriptorContainer>(std::move(features));

	return Result::SUCCESS;
}

bool OBDescriptorGenerator::isSupportedFeatureDetector(const FeatureDetector& featureDetector) const
{
	return featureDetector.featureCategory() == FeatureCategory::KEYPOINTS;
}

} // namespace OceanBlob

} // namespace UnifiedFeatures

} // namespace Ocean
