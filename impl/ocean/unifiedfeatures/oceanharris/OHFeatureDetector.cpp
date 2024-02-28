// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/unifiedfeatures/oceanharris/OHFeatureDetector.h"

#include "ocean/base/WorkerPool.h"

#include "ocean/cv/detector/HarrisCornerDetector.h"

#include <arvr/libraries/thrift_if/ufi/gen-cpp2/UnifiedFeatures_types.h>

namespace Ocean
{

namespace UnifiedFeatures
{

namespace OceanHarris
{

FeatureDetector::Result OHFeatureDetector::detectFeatures(const Frame& frame, std::shared_ptr<FeatureContainer>& featureContainer, const unsigned int border, Worker* worker) const
{
	ocean_assert(frame.isValid());

	if (2u * border + 10u >= frame.width() || 2u * border + 7u >= frame.height())
	{
		ocean_assert(false && "Input too small");
		return Result::ERROR_INPUT_TOO_SMALL;
	}

	if (!isSupportedPixelFormat(frame.pixelFormat()))
	{
		ocean_assert(false && "Invalid pixel format");
		return Result::ERROR_PIXEL_FORMAT_INVALID;
	}

	// Detect corners but ignore the border area; point locations will need to be offset later
	const Frame subFrame = frame.subFrame(border, border, frame.width() - 2u * border, frame.height() - 2u * border);
	ocean_assert(subFrame.isValid() && subFrame.width() >= 10u && subFrame.height() >= 7u);

	CV::Detector::HarrisCorners harrisCorners;
	if (CV::Detector::HarrisCornerDetector::detectCorners(subFrame.constdata<uint8_t>(), subFrame.width(), subFrame.height(), subFrame.paddingElements(), parameters_.threshold, /* isFrameUndistorted */ false, harrisCorners, parameters_.determineExactPosition, worker))
	{
		// Apply the border offsets
		if (border != 0u)
		{
			for (CV::Detector::HarrisCorner& harrisCorner : harrisCorners)
			{
				harrisCorner.setObservation(harrisCorner.observation() + Vector2(Scalar(border), Scalar(border)), harrisCorner.distortionState());
			}
		}
	}

	featureContainer = std::make_shared<OHFeatureContainer>(std::move(harrisCorners));

	ocean_assert(featureContainer != nullptr);
	return Result::SUCCESS;
}

} // namespace OceanHarris

} // namespace UnifiedFeatures

} // namespace Ocean
