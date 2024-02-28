// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/unifiedfeatures/boltharris/BHFeatureDetector.h"
#include "ocean/unifiedfeatures/boltharris/BHFeatureContainer.h"

#include <sdk/api/harris_corner_detector/HarrisCorners5x5-ref.h>

namespace Ocean
{

namespace UnifiedFeatures
{

namespace BoltHarris
{

FeatureDetector::Result BHFeatureDetector::detectFeatures(const Frame& frame, std::shared_ptr<FeatureContainer>& featureContainer, const unsigned int border, Worker* worker) const
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

	BoltImage boltImage = {(void*)subFrame.constdata<void>(), int(subFrame.width()), int(subFrame.height()), int(subFrame.strideBytes()), int(subFrame.planeBytesPerPixel(0u)), nullptr};
	const gsl::span<const BoltImage> boltImages = gsl::span<const BoltImage>(&boltImage, 1);
	const BoltIndexedImageSlice boltImageSlice = {0, 0, 0, static_cast<uint32_t>(boltImage.width), static_cast<uint32_t>(boltImage.height)};
	const uint32_t outOffset = 0;

	std::vector<int> xy(2u * parameters_.maxCornersToExtract);
	std::vector<int> cornerResponse(parameters_.maxCornersToExtract);

	uint32_t numberDetectedCorners = 0u;
	const uint32_t maxCornersToExtract = parameters_.maxCornersToExtract;

	bolt::HarrisCorners5x5Ref harrisCorners5x5Ref(int(subFrame.width()));
	harrisCorners5x5Ref.sync(
		boltImages,
		{&boltImageSlice, 1},
		{&outOffset, 1},
		{&maxCornersToExtract, 1},
		xy,
		cornerResponse,
		{&numberDetectedCorners, 1},
		int(parameters_.threshold),
		false /* adaptive threshold */,
		parameters_.extractEdges);

	if (numberDetectedCorners != 0u)
	{
		ocean_assert(size_t(numberDetectedCorners) <= cornerResponse.size());

		// Apply the border offsets
		if (border != 0u)
		{
			for (uint32_t i = 0u; i < numberDetectedCorners; ++i)
			{
				xy[2 * i + 0] += int(border);
				xy[2 * i + 1] += int(border);
			}
		}

		if (size_t(numberDetectedCorners) < cornerResponse.size())
		{
			cornerResponse.erase(cornerResponse.begin() + numberDetectedCorners, cornerResponse.end());
			xy.erase(xy.begin() + 2u * numberDetectedCorners, xy.end());
		}

		ocean_assert(xy.size() == 2 * cornerResponse.size());
		ocean_assert(xy.size() == size_t(numberDetectedCorners));

		featureContainer = std::make_shared<BHFeatureContainer>(std::move(xy), std::move(cornerResponse));
	}
	else
	{
		// Return an empty container
		featureContainer = std::make_shared<BHFeatureContainer>();
	}

	ocean_assert(featureContainer != nullptr);
	return Result::SUCCESS;
}

} // namespace BoltHarris

} // namespace UnifiedFeatures

} // namespace Ocean
