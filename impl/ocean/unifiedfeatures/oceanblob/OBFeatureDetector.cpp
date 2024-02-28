// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/unifiedfeatures/oceanblob/OBFeatureDetector.h"

#include "ocean/base/WorkerPool.h"

#include "ocean/cv/IntegralImage.h"
#include "ocean/cv/detector/blob/BlobFeatureDescriptor.h"
#include "ocean/cv/detector/blob/BlobFeatureDetector.h"
#include "ocean/geometry/SpatialDistribution.h"

#include <arvr/libraries/thrift_if/ufi/gen-cpp2/UnifiedFeatures_types.h>

namespace Ocean
{

namespace UnifiedFeatures
{

namespace OceanBlob
{

FeatureDetector::Result OBFeatureDetector::detectFeatures(
	const Frame& frame,
	std::shared_ptr<FeatureContainer>& featureContainer,
	const unsigned int border,
	Worker* worker) const
{
	ocean_assert(frame.isValid());

	if (!FrameType::arePixelFormatsCompatible(frame.pixelFormat(), FrameType::FORMAT_Y8))
	{
		ocean_assert(false && "Invalid pixel format");
		return Result::ERROR_PIXEL_FORMAT_INVALID;
	}

	const uint32_t width = frame.width();
	const uint32_t height = frame.height();

	CV::Detector::Blob::BlobFeatures features;

	// integral image.
	Frame linedIntegralFrame(FrameType(width + 1u, height + 1u, FrameType::FORMAT_Y32, FrameType::ORIGIN_UPPER_LEFT));
	uint32_t* const linedIntegralFrameData = linedIntegralFrame.data<uint32_t>();
	CV::IntegralImage::createLinedImage<uint8_t, uint32_t, 1u>(
		frame.constdata<uint8_t>(),
		linedIntegralFrameData,
		width, height,
		frame.paddingElements(), linedIntegralFrame.paddingElements());


	// Determine all possible feature points within the given frame.
	CV::Detector::Blob::BlobFeatureDetector::detectFeatures(
		linedIntegralFrame.constdata<unsigned int>(),
		width,
		height,
		0 /* subRegionLeft */,
		0 /* subRegionTop */,
		width /* subRegionWidth */,
		height /* subRegionHeight */,
		CV::Detector::Blob::BlobFeatureDetector::SAMPLING_NORMAL,
		parameters_.minimumStrength,
		true,
		features);

	if (features.size() > parameters_.maximumNumberFeatures)
	{
		// Sort the feature points w.r.t. their strength parameter.
		std::sort(features.begin(), features.end());

		uint32_t horizontalBins = 2u;
		uint32_t verticalBins = 2u;
		Ocean::Geometry::SpatialDistribution::idealBins(
			width, height, parameters_.maximumNumberFeatures, horizontalBins, verticalBins);

		features = Ocean::Geometry::SpatialDistribution::distributeAndFilter<
			Ocean::CV::Detector::Blob::BlobFeature,
			Ocean::CV::Detector::Blob::BlobFeature::feature2imagePoint>(
			features.data(),
			features.size(),
			0 /* subRegionLeft */,
			0 /* subRegionTop */,
			Ocean::Scalar(width) /* subRegionWidth */,
			Ocean::Scalar(height) /* subRegionHeight */,
			horizontalBins,
			verticalBins,
			parameters_.maximumNumberFeatures);
	}


	featureContainer = std::make_shared<OBFeatureContainer>(std::move(features), std::move(linedIntegralFrame), &frame);

	ocean_assert(featureContainer != nullptr);
	return Result::SUCCESS;
}

} // namespace OceanBlob

} // namespace UnifiedFeatures

} // namespace Ocean
