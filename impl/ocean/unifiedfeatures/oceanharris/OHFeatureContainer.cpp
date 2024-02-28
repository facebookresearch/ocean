// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/unifiedfeatures/oceanharris/OHFeatureContainer.h"

#include "ocean/cv/detector/HarrisCornerDetector.h"

#include <arvr/libraries/thrift_if/ufi/gen-cpp2/UnifiedFeatures_types.h>

namespace Ocean
{

namespace UnifiedFeatures
{

namespace OceanHarris
{

Keypoints OHFeatureContainer::convertFeaturesToKeypoints() const
{
	Keypoints keypoints;
	keypoints.reserve(harrisCorners_.size());

	for (const CV::Detector::HarrisCorner& harrisCorner : harrisCorners_)
	{
		const Vector2& observation = harrisCorner.observation();
		keypoints.emplace_back(VectorF2(float(observation.x()), float(observation.y())), float(harrisCorner.strength()));
	}

	return keypoints;
}

size_t OHFeatureContainer::size() const
{
	return harrisCorners_.size();
}

bool OHFeatureContainer::toThrift(Thrift::FeatureContainer& thriftFeatureContainer) const
{
	Thrift::UnifiedObject unifiedObjectInfo;
	unifiedObjectInfo.libraryName_ref() = libraryName();
	unifiedObjectInfo.name_ref() = name();
	unifiedObjectInfo.version_ref() = int32_t(objectVersion());

	Thrift::FeatureContainer localThriftFeatureContainer;
	localThriftFeatureContainer.unifiedObjectInfo_ref() = unifiedObjectInfo;

	std::vector<Thrift::Keypoint> thriftKeypoints;
	thriftKeypoints.reserve(harrisCorners_.size());

	for (const CV::Detector::HarrisCorner& harrisCorner : harrisCorners_)
	{
		Thrift::Keypoint& thriftKeypoint = thriftKeypoints.emplace_back();

		thriftKeypoint.x_ref() = float(harrisCorner.observation().x());
		thriftKeypoint.y_ref() = float(harrisCorner.observation().y());
		thriftKeypoint.strength_ref() = float(harrisCorner.strength());

		// Ignoring orientation, octave, and scale as they are not defined for this feature
	}

	localThriftFeatureContainer.featureUnion_ref()->keypoints_ref() = std::move(thriftKeypoints);
	ocean_assert(localThriftFeatureContainer.featureUnion_ref()->getType() == Thrift::FeatureUnion::Type::keypoints);

	thriftFeatureContainer = std::move(localThriftFeatureContainer);

	return true;
}

bool OHFeatureContainer::fromThrift(const Thrift::FeatureContainer& thriftFeatureContainer)
{
	if (thriftFeatureContainer.get_unifiedObjectInfo().get_libraryName() != libraryName()
		|| thriftFeatureContainer.get_unifiedObjectInfo().get_name() != name()
		|| (unsigned int)(thriftFeatureContainer.get_unifiedObjectInfo().get_version()) != objectVersion()
		|| thriftFeatureContainer.get_featureUnion().getType() != Thrift::FeatureUnion::Type::keypoints)
	{
		ocean_assert(false && "Incompatible feature container");
		return false;
	}

	const std::vector<Thrift::Keypoint>& thriftKeypoints = thriftFeatureContainer.get_featureUnion().get_keypoints();

	harrisCorners_.reserve(thriftKeypoints.size());

	for (const Thrift::Keypoint& thriftKeypoint : thriftKeypoints)
	{
		const float* strength = thriftKeypoint.get_strength();

		if (strength == nullptr || *strength < 0.0f)
		{
			ocean_assert(false && "Invalid Thrift data! This should never happen!");
			return false;
		}

		harrisCorners_.emplace_back(Vector2(Scalar(thriftKeypoint.get_x()), Scalar(thriftKeypoint.get_y())), CV::Detector::HarrisCorner::DS_UNKNOWN, Scalar(*strength));

		// Ignoring all other members of thriftKeypoint because they are not used by this feature
	}

	return true;
}

} // namespace OceanHarris

} // namespace UnifiedFeatures

} // namespace Ocean
