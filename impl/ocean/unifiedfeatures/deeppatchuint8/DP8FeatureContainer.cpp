// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/unifiedfeatures/deeppatchuint8/DP8FeatureContainer.h"

#include <arvr/libraries/thrift_if/ufi/gen-cpp2/UnifiedFeatures_types.h>

#include <climits>

namespace Ocean
{

namespace UnifiedFeatures
{

namespace DeepPatchUint8
{

bool DP8FeatureContainer::toThrift(Thrift::FeatureContainer& thriftFeatureContainer) const
{
	Thrift::UnifiedObject unifiedObjectInfo;
	unifiedObjectInfo.set_libraryName(libraryName());
	unifiedObjectInfo.set_name(name());
	unifiedObjectInfo.set_version(int32_t(objectVersion()));

	Thrift::FeatureContainer localThriftFeatureContainer;
	localThriftFeatureContainer.set_unifiedObjectInfo(unifiedObjectInfo);

	std::vector<Thrift::Keypoint> thriftKeypoints;
	thriftKeypoints.reserve(keypoints_.size());

	for (size_t i = 0; i < keypoints_.size(); ++i)
	{
		Thrift::Keypoint& thriftKeypoint = thriftKeypoints.emplace_back();

		const perception::features::Keypoint& keypoint = keypoints_[i];

		thriftKeypoint.set_x(float(keypoint.positionLevel0[0]));
		thriftKeypoint.set_y(float(keypoint.positionLevel0[1]));
		thriftKeypoint.set_strength(float(keypoint.score));
		thriftKeypoint.set_octave(int32_t(keypoint.level));
		thriftKeypoint.set_orientation(float(keypoint.angle));
		thriftKeypoint.set_scale(float(keypoint.scale));
	}

	localThriftFeatureContainer.featureUnion_ref()->set_keypoints(std::move(thriftKeypoints));
	ocean_assert(localThriftFeatureContainer.featureUnion_ref()->getType() == Thrift::FeatureUnion::Type::keypoints);

	thriftFeatureContainer = std::move(localThriftFeatureContainer);

	return true;
}

bool DP8FeatureContainer::fromThrift(const Thrift::FeatureContainer& thriftFeatureContainer)
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

	keypoints_.reserve(thriftKeypoints.size());

	for (const Thrift::Keypoint& thriftKeypoint : thriftKeypoints)
	{
		const int level = thriftKeypoint.get_octave() != nullptr ? int(*thriftKeypoint.get_octave()) : -1;
		const float scale = thriftKeypoint.get_scale() != nullptr ? *thriftKeypoint.get_scale() : -1.0f;
		const float score = thriftKeypoint.get_strength() != nullptr ? *thriftKeypoint.get_strength() : -1.0f;
		const float angle = thriftKeypoint.get_orientation() != nullptr ? *thriftKeypoint.get_orientation() : 0.0f;

		keypoints_.emplace_back(perception::features::Keypoint{Eigen::Vector2f(thriftKeypoint.get_x(), thriftKeypoint.get_y()), level, scale, score, angle});
	}

	return true;
}

Keypoints DP8FeatureContainer::convertFeaturesToKeypoints() const
{
	Keypoints keypoints;
	keypoints.reserve(keypoints_.size());

	for (const perception::features::Keypoint& Keypoint : keypoints_)
	{
		keypoints.emplace_back(VectorF2(Keypoint.positionLevel0[0], Keypoint.positionLevel0[1]), Keypoint.score, Keypoint.angle, (unsigned int)(Keypoint.level), Keypoint.scale);
	}

	return keypoints;
}

} // namespace DeepPatchUint8

} // namespace UnifiedFeatures

} // namespace Ocean
