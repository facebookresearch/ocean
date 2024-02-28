// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/unifiedfeatures/vlfeatsift/VlfeatSiftFeatureDetector.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/unifiedfeatures/vlfeatsift/Utilities.h"

#include <arvr/libraries/thrift_if/ufi/gen-cpp2/UnifiedFeatures_types.h>

namespace Ocean
{

namespace UnifiedFeatures
{

namespace VlfeatSift
{

FeatureDetector::Result VlfeatSiftFeatureDetector::detectFeatures(const Frame& frame, std::shared_ptr<FeatureContainer>& featureContainer, const unsigned int border, Worker* worker) const
{
	ocean_assert(frame.isValid());

	if (2u * border >= frame.width() || 2u * border >= frame.height())
	{
		ocean_assert(false && "Input too small");
		return Result::ERROR_INPUT_TOO_SMALL;
	}

	if (!isSupportedPixelFormat(frame.pixelFormat()))
	{
		ocean_assert(false && "Invalid pixel format");
		return Result::ERROR_PIXEL_FORMAT_INVALID;
	}

	std::unique_ptr<VlCovDet, void (*)(VlCovDet*)> detector(vl_covdet_new(parameters_.detectionMethod), &vl_covdet_delete);

	if (!detector)
	{
		ocean_assert(false && "Failed to create SIFT detector");
		return Result::GENERAL_ERROR;
	}

	// Potentially cast the Y8 frame to float32, and/or make a contiguous copy of the frame if necessary.
	std::unique_ptr<Frame> float32FramePtr;
	const Frame& float32Frame = Utilities::frameAsContiguousF32(frame, float32FramePtr, worker);

	vl_covdet_set_first_octave(detector.get(), parameters_.firstOctave);
	vl_covdet_set_octave_resolution(detector.get(), parameters_.octaveResolution);
	vl_covdet_set_peak_threshold(detector.get(), parameters_.peakThreshold);
	vl_covdet_set_edge_threshold(detector.get(), parameters_.edgeThreshold);

	// Run detection.
	vl_covdet_put_image(detector.get(), float32Frame.constdata<float>(), float32Frame.width(), float32Frame.height());
	vl_covdet_detect(detector.get(), parameters_.maximumNumberFeatures);

	// Note that the detector may return more points than requested.
	// We'll apply a minor hack here and move the extra features out of the image, then use the VLFeat API to remove them.
	// It's not possible to remove them, otherwise.
	const size_t numberFeatures = size_t(vl_covdet_get_num_features(detector.get()));
	VlCovDetFeature* features = vl_covdet_get_features(detector.get());

	if (numberFeatures > parameters_.maximumNumberFeatures)
	{
		for (size_t i = parameters_.maximumNumberFeatures; i < numberFeatures; ++i)
		{
			features[i].frame.x = -1.0f;
			features[i].frame.y = -1.0f;
		}

		vl_covdet_drop_features_outside(detector.get(), border);
	}

	if (parameters_.computeAffineShape)
	{
		vl_covdet_extract_affine_shape(detector.get());
	}

	if (!parameters_.upright)
	{
		// note: if a feature has several orientations, new dedicated features are appended so that we may end with more features than before
		vl_covdet_extract_orientations(detector.get());
	}

	featureContainer = std::make_shared<VlfeatSiftFeatureContainer>(std::move(detector), frame.constdata<void>());

	ocean_assert(featureContainer != nullptr);
	return Result::SUCCESS;
}

} // namespace VlfeatSift

} // namespace UnifiedFeatures

} // namespace Ocean
