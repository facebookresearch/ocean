// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/unifiedfeatures/Utilities.h"
#include "ocean/unifiedfeatures/Manager.h"

#include "ocean/cv/Canvas.h"

#include <arvr/libraries/thrift_if/ufi/gen-cpp2/UnifiedFeatures_types.h>

namespace Ocean
{

namespace UnifiedFeatures
{

void Utilities::drawKeypoint(Frame& frame, const Keypoint& keypoint, const Vector2& offset)
{
	ocean_assert(frame.isValid());

	const unsigned int octave = keypoint.octave();
	const Vector2 observation = Vector2(keypoint.observation()) + offset;

	switch (octave)
	{
		case 0u:
			CV::Canvas::point<7u>(frame, observation, CV::Canvas::red(frame.pixelFormat()));
			break;

		case 1u:
			CV::Canvas::point<9u>(frame, observation, CV::Canvas::red(frame.pixelFormat()));
			break;

		case 2u:
			CV::Canvas::point<11u>(frame, observation, CV::Canvas::red(frame.pixelFormat()));
			break;

		case 3u:
			CV::Canvas::point<13u>(frame, observation, CV::Canvas::red(frame.pixelFormat()));
			break;

		case 4u:
			CV::Canvas::point<15u>(frame, observation, CV::Canvas::red(frame.pixelFormat()));
			break;

		case 5u:
			CV::Canvas::point<17u>(frame, observation, CV::Canvas::red(frame.pixelFormat()));
			break;

		case 6u:
			CV::Canvas::point<19u>(frame, observation, CV::Canvas::red(frame.pixelFormat()));
			break;

		default:
			CV::Canvas::point<21u>(frame, observation, CV::Canvas::red(frame.pixelFormat()));
			break;
	}
}

void Utilities::drawKeypoints(Frame& frame, const Keypoint* keypoints, const size_t keypointsSize, const Vector2& offset)
{
	ocean_assert(keypoints != nullptr);

	for (size_t i = 0; i < keypointsSize; ++i)
	{
		drawKeypoint(frame, keypoints[i], offset);
	}
}

void Utilities::drawFeatureCorrespondences(Frame& frame, const FeatureContainer& featureContainer0, const FeatureContainer& featureContainer1, const IndexPairs32& correspondences, const Vector2& offset0, const Vector2& offset1)
{
	const UnifiedFeatures::Keypoints& keypoints0 = featureContainer0.keypoints();
	const UnifiedFeatures::Keypoints& keypoints1 = featureContainer1.keypoints();

	for (const IndexPair32& correspondence : correspondences)
	{
		ocean_assert(size_t(correspondence.first) < keypoints0.size());
		ocean_assert(size_t(correspondence.second) < keypoints1.size());

		const Vector2 observation0 = Vector2(keypoints0[correspondence.first].observation()) + offset0;
		const Vector2 observation1 = Vector2(keypoints1[correspondence.second].observation()) + offset1;

		CV::Canvas::line<1u>(frame, Numeric::round32(observation0.x()), Numeric::round32(observation0.y()), Numeric::round32(observation1.x()), Numeric::round32(observation1.y()), CV::Canvas::green(frame.pixelFormat()));
	}

	featureContainer0.drawFeatures(frame, offset0);
	featureContainer1.drawFeatures(frame, offset1);
}

bool Utilities::toThrift(const FeatureContainer& featureContainer, const DescriptorContainer& descriptorContainer, Thrift::ContainerPair& thriftContainerPair)
{
	if (featureContainer.size() != descriptorContainer.size())
	{
		ocean_assert(false && "Container sizes must match!");
		return false;
	}

	if (!featureContainer.toThrift(*thriftContainerPair.featuresContainer_ref()))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	if (!descriptorContainer.toThrift(*thriftContainerPair.descriptorContainer_ref()))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	return true;
}

bool Utilities::fromThrift(const Thrift::ContainerPair& thriftContainerPair, std::shared_ptr<FeatureContainer>& featureContainer, std::shared_ptr<DescriptorContainer>& descriptorContainer)
{
	featureContainer = std::static_pointer_cast<FeatureContainer>(Manager::get().create(thriftContainerPair.get_featuresContainer().get_unifiedObjectInfo().get_name()));

	if (featureContainer == nullptr)
	{
		ocean_assert(false && "Unknown feature container - library not registered?");
		return false;
	}

	if (!featureContainer->fromThrift(thriftContainerPair.get_featuresContainer()))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	descriptorContainer = std::static_pointer_cast<DescriptorContainer>(Manager::get().create(thriftContainerPair.get_descriptorContainer().get_unifiedObjectInfo().get_name()));

	if (descriptorContainer == nullptr)
	{
		ocean_assert(false && "Unknown descriptor container - library not registered?");
		return false;
	}

	if (!descriptorContainer->fromThrift(thriftContainerPair.get_descriptorContainer(), featureContainer))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	return true;
}

} // namespace UnifiedFeatures

} // namespace Ocean
