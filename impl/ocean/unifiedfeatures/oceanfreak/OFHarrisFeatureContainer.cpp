// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/unifiedfeatures/oceanfreak/OFHarrisFeatureContainer.h"

#include "ocean/cv/detector/HarrisCornerDetector.h"

#include <arvr/libraries/thrift_if/ufi/gen-cpp2/UnifiedFeatures_types.h>

namespace Ocean
{

namespace UnifiedFeatures
{

namespace OceanFreak
{

Keypoints OFHarrisFeatureContainer::convertFeaturesToKeypoints() const
{
	ocean_assert(harrisCorners_.size() == harrisCornerPyramidLevels_.size());

	Keypoints keypoints;
	keypoints.reserve(harrisCorners_.size());

	for (size_t i = 0; i < harrisCorners_.size(); ++i)
	{
		const Vector2& observation = harrisCorners_[i].observation();
		keypoints.emplace_back(VectorF2(float(observation.x()), float(observation.y())), float(harrisCorners_[i].strength()), /* orientation */ -1.0f, float(harrisCornerPyramidLevels_[i]), /* scale */ -1.0f);
	}

	return keypoints;
}

size_t OFHarrisFeatureContainer::size() const
{
	ocean_assert(harrisCorners_.size() == harrisCornerPyramidLevels_.size());
	return harrisCorners_.size();
}

bool OFHarrisFeatureContainer::toThrift(Thrift::FeatureContainer& thriftFeatureContainer) const
{
	if (harrisCorners_.size() != harrisCornerPyramidLevels_.size())
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	Thrift::UnifiedObject unifiedObjectInfo;
	unifiedObjectInfo.libraryName_ref() = libraryName();
	unifiedObjectInfo.name_ref() = name();
	unifiedObjectInfo.version_ref() = int32_t(objectVersion());

	Thrift::FeatureContainer localThriftFeatureContainer;
	localThriftFeatureContainer.unifiedObjectInfo_ref() = unifiedObjectInfo;

	std::vector<Thrift::Keypoint> thriftKeypoints;
	thriftKeypoints.reserve(harrisCorners_.size());

	for (size_t i = 0; i < harrisCorners_.size(); ++i)
	{
		Thrift::Keypoint& thriftKeypoint = thriftKeypoints.emplace_back();

		const CV::Detector::HarrisCorner& harrisCorner = harrisCorners_[i];

		thriftKeypoint.x_ref() = float(harrisCorner.observation().x());
		thriftKeypoint.y_ref() = float(harrisCorner.observation().y());
		thriftKeypoint.strength_ref() = float(harrisCorner.strength());
		thriftKeypoint.octave_ref() = int32_t(harrisCornerPyramidLevels_[i]);

		// Ignoring orientation and scale as they are not defined for this feature
	}

	localThriftFeatureContainer.featureUnion_ref()->keypoints_ref() = std::move(thriftKeypoints);
	ocean_assert(localThriftFeatureContainer.featureUnion_ref()->getType() == Thrift::FeatureUnion::Type::keypoints);

	thriftFeatureContainer = std::move(localThriftFeatureContainer);

	return true;
}

bool OFHarrisFeatureContainer::fromThrift(const Thrift::FeatureContainer& thriftFeatureContainer)
{
	if (thriftFeatureContainer.get_unifiedObjectInfo().get_libraryName() != libraryName()
		|| thriftFeatureContainer.get_unifiedObjectInfo().get_name() != name()
		|| thriftFeatureContainer.get_unifiedObjectInfo().get_version() != int32_t(objectVersion())
		|| thriftFeatureContainer.get_featureUnion().getType() != Thrift::FeatureUnion::Type::keypoints)
	{
		ocean_assert(false && "Incompatible feature container");
		return false;
	}

	const std::vector<Thrift::Keypoint>& thriftKeypoints = thriftFeatureContainer.get_featureUnion().get_keypoints();

	harrisCorners_.reserve(thriftKeypoints.size());
	harrisCornerPyramidLevels_.reserve(thriftKeypoints.size());

	for (const Thrift::Keypoint& thriftKeypoint : thriftKeypoints)
	{
		const float* strength = thriftKeypoint.get_strength();
		const int32_t* octave = thriftKeypoint.get_octave();

		if (strength == nullptr || octave == nullptr || *strength < 0.0f || *octave < 0)
		{
			ocean_assert(false && "Invalid Thrift data! This should never happen!");
			return false;
		}

		harrisCorners_.emplace_back(Vector2(Scalar(thriftKeypoint.get_x()), Scalar(thriftKeypoint.get_y())), CV::Detector::HarrisCorner::DS_UNKNOWN, Scalar(*strength));
		harrisCornerPyramidLevels_.emplace_back(Index32(*octave));

		// Ignoring all other members of thriftKeypoint because they are not used by this feature
	}

	return true;
}

} // namespace OceanFreak

} // namespace UnifiedFeatures

} // namespace Ocean
