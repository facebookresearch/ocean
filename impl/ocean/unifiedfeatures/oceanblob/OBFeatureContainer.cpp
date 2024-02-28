// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/unifiedfeatures/oceanblob/OBFeatureContainer.h"

#include "ocean/cv/detector/HarrisCornerDetector.h"
#include "ocean/cv/detector/blob/BlobFeature.h"

#include <arvr/libraries/thrift_if/ufi/gen-cpp2/UnifiedFeatures_types.h>

namespace Ocean
{

namespace UnifiedFeatures
{

namespace OceanBlob
{

Keypoints OBFeatureContainer::convertFeaturesToKeypoints() const
{
	Keypoints keypoints;
	keypoints.reserve(blobFeatures_.size());

	for (const CV::Detector::Blob::BlobFeature& feature : blobFeatures_)
	{
		keypoints.emplace_back(
			VectorF2(feature.observation().x(), feature.observation().y()),
			float(feature.strength()),
			float(feature.orientation()),
			0u, // No octave for blob features exist
			// Surreal's kpt scale is the scale of image in the pyramid i.e. 1, 2, 4,....
			// Using the actual blob kpt scale (~ patch size) can hurt the performance.
			1.0f);
	}

	return keypoints;
}

size_t OBFeatureContainer::size() const
{
	return blobFeatures_.size();
}

bool OBFeatureContainer::toThrift(Thrift::FeatureContainer& thriftFeatureContainer) const
{
	Thrift::UnifiedObject unifiedObjectInfo;
	unifiedObjectInfo.libraryName_ref() = libraryName();
	unifiedObjectInfo.name_ref() = name();
	unifiedObjectInfo.version_ref() = int32_t(objectVersion());


	Thrift::FeatureContainer localThriftFeatureContainer;
	localThriftFeatureContainer.unifiedObjectInfo_ref() = unifiedObjectInfo;

	std::vector<Thrift::Keypoint> thriftKeypoints;
	thriftKeypoints.reserve(blobFeatures_.size());

	for (const CV::Detector::Blob::BlobFeature& feature : blobFeatures_)
	{
		Thrift::Keypoint& thriftKeypoint = thriftKeypoints.emplace_back();

		thriftKeypoint.x_ref() = float(feature.observation().x());
		thriftKeypoint.y_ref() = float(feature.observation().y());

		thriftKeypoint.strength_ref() = float(feature.strength());
		thriftKeypoint.scale_ref() = float(feature.scale());
		thriftKeypoint.orientation_ref() = float(feature.orientation());
		thriftKeypoint.extra_ref<int>() = feature.laplace() ? 1 : 0;

		// Ignoring octave as it is not defined for this feature
	}

	localThriftFeatureContainer.featureUnion_ref()->set_keypoints(std::move(thriftKeypoints));
	ocean_assert(localThriftFeatureContainer.featureUnion_ref()->getType() == Thrift::FeatureUnion::Type::keypoints);

	thriftFeatureContainer = std::move(localThriftFeatureContainer);

	return true;
}

bool OBFeatureContainer::fromThrift(const Thrift::FeatureContainer& thriftFeatureContainer)
{
	if (thriftFeatureContainer.get_unifiedObjectInfo().get_libraryName() != libraryName() ||
		thriftFeatureContainer.get_unifiedObjectInfo().get_name() != name() ||
		(unsigned int)(thriftFeatureContainer.get_unifiedObjectInfo().get_version()) != objectVersion() ||
		thriftFeatureContainer.get_featureUnion().getType() != Thrift::FeatureUnion::Type::keypoints)
	{
		ocean_assert(false && "Incompatible feature container");
		return false;
	}

	const std::vector<Thrift::Keypoint>& thriftKeypoints = thriftFeatureContainer.get_featureUnion().get_keypoints();
	blobFeatures_.reserve(thriftKeypoints.size());

	detectorFrame_ = nullptr;

	for (const Thrift::Keypoint& thriftKeypoint : thriftKeypoints)
	{
		const float* strength = thriftKeypoint.get_strength();
		const float* scale = thriftKeypoint.get_scale();
		const float* orientation = thriftKeypoint.get_orientation();
		const std::string* extra = thriftKeypoint.get_extra();

		if (strength == nullptr || *strength < 0.0f || scale == nullptr || orientation == nullptr)
		{
			ocean_assert(false && "Invalid Thrift data! This should never happen!");
			return false;
		}

		const bool laplace = *reinterpret_cast<const int*>(extra) == 1;

		blobFeatures_.emplace_back(Vector2(Scalar(thriftKeypoint.get_x()), Scalar(thriftKeypoint.get_y())), CV::Detector::Blob::BlobFeature::DS_UNKNOWN, Scalar(*scale), Scalar(*strength), laplace, *orientation);
	}

	return true;
}

} // namespace OceanBlob

} // namespace UnifiedFeatures

} // namespace Ocean
