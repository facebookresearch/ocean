// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/unifiedfeatures/vlfeatsift/VlfeatSiftFeatureContainer.h"

#include <arvr/libraries/thrift_if/ufi/gen-cpp2/UnifiedFeatures_types.h>

namespace Ocean
{

namespace UnifiedFeatures
{

namespace VlfeatSift
{

void VlfeatSiftFeatureContainer::computeScaleAndOrientationForFeature(const VlCovDetFeature& feature, float& scale, float& orientation)
{
	// Check that at least the X axis transform is sane.
	// Technically, the Y axis transform should also be checked, but the output values here will be valid as long as the X axis transform is valid.
	if (NumericF::isEqualEps(feature.frame.a11) && NumericF::isEqualEps(feature.frame.a21))
	{
		ocean_assert(false && "Extracted feature has a degenerate transformation");
		scale = 1.0f;
		orientation = 0.0f;
		return;
	}

	// Take the scale as the average "stretch" of the canonical X and Y axes.
	scale = 0.5f * (NumericF::pythagoras(feature.frame.a11, feature.frame.a21) + NumericF::pythagoras(feature.frame.a12, feature.frame.a22));

	// The orientation is taken as the amount that the X axis is rotated.
	orientation = NumericF::atan2(feature.frame.a21, feature.frame.a11);
}

Keypoints VlfeatSiftFeatureContainer::convertFeaturesToKeypoints() const
{
	const size_t numberKeypoints = size();
	Keypoints keypoints;
	keypoints.reserve(numberKeypoints);

	const VlCovDetFeature* features = vl_covdet_get_features(detector_.get());
	for (size_t i = 0; i < numberKeypoints; ++i)
	{
		const VlCovDetFeature& feature = features[i];
		float scale = 1.0f, orientation = 0.0f;
		computeScaleAndOrientationForFeature(feature, scale, orientation);

		std::shared_ptr<SquareMatrixF2> affineShape = std::make_shared<SquareMatrixF2>(feature.frame.a11, feature.frame.a21, feature.frame.a12, feature.frame.a22);

		// VLFeat uses a top-left pixel coordinate of (-0.5, -0.5).
		keypoints.emplace_back(VectorF2(feature.frame.x + 0.5f, feature.frame.y + 0.5f), feature.peakScore, orientation, feature.o < 0 ? 0u : (unsigned int)(feature.o), scale, affineShape);
	}

	return keypoints;
}

size_t VlfeatSiftFeatureContainer::size() const
{
	return (size_t)vl_covdet_get_num_features(detector_.get());
}

bool VlfeatSiftFeatureContainer::toThrift(Thrift::FeatureContainer& thriftFeatureContainer) const
{
	Thrift::UnifiedObject unifiedObjectInfo;
	unifiedObjectInfo.libraryName_ref() = libraryName();
	unifiedObjectInfo.name_ref() = name();
	unifiedObjectInfo.version_ref() = int32_t(objectVersion());

	Thrift::FeatureContainer localThriftFeatureContainer;
	localThriftFeatureContainer.unifiedObjectInfo_ref() = unifiedObjectInfo;

	const size_t numberKeypoints = size();
	std::vector<Thrift::Keypoint> thriftKeypoints;
	thriftKeypoints.reserve(numberKeypoints);

	const VlCovDetFeature* features = vl_covdet_get_features(detector_.get());
	for (size_t i = 0; i < numberKeypoints; ++i)
	{
		const VlCovDetFeature& feature = features[i];
		Thrift::Keypoint& thriftKeypoint = thriftKeypoints.emplace_back();

		// VLFeat uses a top-left pixel coordinate of (-0.5, -0.5).
		thriftKeypoint.x_ref() = feature.frame.x + 0.5f;
		thriftKeypoint.y_ref() = feature.frame.y + 0.5f;
		thriftKeypoint.octave_ref() = feature.o;
		thriftKeypoint.strength_ref() = feature.peakScore;

		// Note that the suboctave (feature.s) and other scores are lost, but these are not used, anyhow.
		float scale = 1.0, orientation = 0.0;
		computeScaleAndOrientationForFeature(feature, scale, orientation);

		// Surreal's kpt scale is simply the scale of image in the pyramid i.e. 1, 2, 4,....
		// Using the actual SIFT kpt scale (~ patch size) can hurt the performance.
		thriftKeypoint.scale_ref() = NumericF::pow(2.0f, float(feature.o));
		thriftKeypoint.orientation_ref() = orientation;

		// Save the affine shape if it is meaningful; if there is no skew or scaling, then we do not need to store it.
		const SquareMatrixF2 affineShape(feature.frame.a11, feature.frame.a21, feature.frame.a12, feature.frame.a22);

		if (!NumericF::isWeakEqualEps(affineShape.xAxis() * affineShape.yAxis()) || !NumericF::isWeakEqual(affineShape.xAxis().sqr(), affineShape.yAxis().sqr()))
		{
			thriftKeypoint.extra_ref() = std::string(reinterpret_cast<const char*>(&affineShape), sizeof(SquareMatrixF2));
		}
	}

	localThriftFeatureContainer.featureUnion_ref()->keypoints_ref() = std::move(thriftKeypoints);
	ocean_assert(localThriftFeatureContainer.featureUnion_ref()->getType() == Thrift::FeatureUnion::Type::keypoints);

	thriftFeatureContainer = std::move(localThriftFeatureContainer);

	return true;
}

bool VlfeatSiftFeatureContainer::fromThrift(const Thrift::FeatureContainer& thriftFeatureContainer)
{
	if (thriftFeatureContainer.get_unifiedObjectInfo().get_libraryName() != libraryName()
		|| thriftFeatureContainer.get_unifiedObjectInfo().get_name() != name()
		|| (unsigned int)(thriftFeatureContainer.get_unifiedObjectInfo().get_version()) != objectVersion()
		|| thriftFeatureContainer.get_featureUnion().getType() != Thrift::FeatureUnion::Type::keypoints)
	{
		ocean_assert(false && "Incompatible feature container");
		return false;
	}

	if (!detector_)
	{
		ocean_assert(false && "Invalid detector; this should never happen!");
		return false;
	}

	const std::vector<Thrift::Keypoint>& thriftKeypoints = thriftFeatureContainer.get_featureUnion().get_keypoints();

	// Clear all existing data from the detector.
	vl_covdet_reset(detector_.get());

	// Since we're loading features, we can no longer assume they came from our existing Frame.
	detectorFrame_ = nullptr;

	for (const Thrift::Keypoint& thriftKeypoint : thriftKeypoints)
	{
		VlCovDetFeature feature;

		// VLFeat uses a top-left pixel coordinate of (-0.5, -0.5).
		feature.frame.x = thriftKeypoint.get_x() - 0.5f;
		feature.frame.y = thriftKeypoint.get_y() - 0.5f;

		// Note that the suboctave (feature.s) and other scores are lost, but these are not used, anyhow; only the octave is used for descriptor generation.
		const auto octaveRef = thriftKeypoint.octave_ref();
		feature.o = octaveRef.has_value() ? *octaveRef : 0; // default to the original image resolution

		const auto strengthRef = thriftKeypoint.strength_ref();
		feature.peakScore = strengthRef.has_value() ? *strengthRef : 0.0f;

		// Either the feature has a full affine transform, or we can derive one from the scale and orientation (if available).
		const auto extraRef = thriftKeypoint.extra_ref();

		ocean_assert(!extraRef.has_value() || extraRef->size() == sizeof(SquareMatrixF2));

		if (extraRef.has_value() && extraRef->size() == sizeof(SquareMatrixF2))
		{
			const SquareMatrixF2& affineShape = *reinterpret_cast<const SquareMatrixF2*>(thriftKeypoint.extra_ref<SquareMatrixF2>()->c_str());
			feature.frame.a11 = affineShape(0, 0);
			feature.frame.a12 = affineShape(0, 1);
			feature.frame.a21 = affineShape(1, 0);
			feature.frame.a22 = affineShape(1, 1);
		}
		else
		{
			// If the scale is unavailable, features will be extracted only at the octave resolution.
			const auto scaleRef = thriftKeypoint.scale_ref();
			const float scale = scaleRef.has_value() ? *scaleRef : 1.0f;

			feature.frame.a11 = scale;
			feature.frame.a12 = scale;

			if (const auto orientationRef = thriftKeypoint.orientation_ref(); orientationRef.has_value())
			{
				feature.frame.a11 *= NumericF::cos(*orientationRef);
				feature.frame.a12 *= -NumericF::sin(*orientationRef);
			}

			feature.frame.a21 = -feature.frame.a12;
			feature.frame.a22 = feature.frame.a11;
		}

		vl_covdet_append_feature(detector_.get(), &feature);
	}

	return true;
}

} // namespace VlfeatSift

} // namespace UnifiedFeatures

} // namespace Ocean
