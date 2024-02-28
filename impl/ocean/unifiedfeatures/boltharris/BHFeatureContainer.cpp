// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/unifiedfeatures/boltharris/BHFeatureContainer.h"

#include <arvr/libraries/thrift_if/ufi/gen-cpp2/UnifiedFeatures_types.h>

namespace Ocean
{

namespace UnifiedFeatures
{

namespace BoltHarris
{

Keypoints BHFeatureContainer::convertFeaturesToKeypoints() const
{
	ocean_assert(xy_.size() == 2 * cornerResponse_.size());

	Keypoints keypoints;
	keypoints.reserve(cornerResponse_.size());

	for (size_t i = 0; i < cornerResponse_.size(); ++i)
	{
		keypoints.emplace_back(VectorF2(float(xy_[2 * i + 0]), float(xy_[2 * i + 1])), float(cornerResponse_[i]));
	}

	return keypoints;
}

size_t BHFeatureContainer::size() const
{
	ocean_assert(xy_.size() == 2 * cornerResponse_.size());
	return cornerResponse_.size();
}

bool BHFeatureContainer::toThrift(Thrift::FeatureContainer& thriftFeatureContainer) const
{
	if (xy_.size() != 2 * cornerResponse_.size())
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	Thrift::UnifiedObject unifiedObjectInfo;
	unifiedObjectInfo.set_libraryName(libraryName());
	unifiedObjectInfo.set_name(name());
	unifiedObjectInfo.set_version(int32_t(objectVersion()));

	Thrift::FeatureContainer localThriftFeatureContainer;
	localThriftFeatureContainer.set_unifiedObjectInfo(std::move(unifiedObjectInfo));

	std::vector<Thrift::Keypoint> thriftKeypoints;
	thriftKeypoints.reserve(cornerResponse_.size());

	for (size_t i = 0; i < cornerResponse_.size(); ++i)
	{
		Thrift::Keypoint& thriftKeypoint = thriftKeypoints.emplace_back();

		thriftKeypoint.set_x(float(xy_[2 * i + 0]));
		thriftKeypoint.set_y(float(xy_[2 * i + 1]));
		thriftKeypoint.set_strength(float(cornerResponse_[i]));

		// Ignoring orientation, octave, and scale as they are not defined for this feature
	}

	localThriftFeatureContainer.featureUnion_ref()->set_keypoints(std::move(thriftKeypoints));
	ocean_assert(localThriftFeatureContainer.get_featureUnion().getType() == Thrift::FeatureUnion::Type::keypoints);

	thriftFeatureContainer = std::move(localThriftFeatureContainer);

	return true;
}

bool BHFeatureContainer::fromThrift(const Thrift::FeatureContainer& thriftFeatureContainer)
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

	cornerResponse_.reserve(thriftKeypoints.size());
	xy_.reserve(2 * thriftKeypoints.size());

	for (const Thrift::Keypoint& thriftKeypoint : thriftKeypoints)
	{
		xy_.emplace_back(NumericF::round32(thriftKeypoint.get_x()));
		xy_.emplace_back(NumericF::round32(thriftKeypoint.get_y()));

		const float* strength = thriftKeypoint.get_strength();

		if (strength == nullptr)
		{
			ocean_assert(false && "Invalid Thrift data: missing feature strength! This should never happen!");
			return false;
		}

		cornerResponse_.emplace_back(*strength);
	}

	return true;
}

} // namespace BoltHarris

} // namespace UnifiedFeatures

} // namespace Ocean
