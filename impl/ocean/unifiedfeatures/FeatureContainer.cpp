// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/unifiedfeatures/FeatureContainer.h"
#include "ocean/unifiedfeatures/Utilities.h"

namespace Ocean
{

namespace UnifiedFeatures
{

void FeatureContainer::drawFeatures(Frame& frame, const Vector2& offset) const
{
	ocean_assert(frame.isValid());

	if (size() == 0)
	{
		// Nothing to draw.
		return;
	}

	switch (featureCategory())
	{
		case FeatureCategory::KEYPOINTS:
		{
			const Keypoints& localKeypoints = keypoints();
			if (!localKeypoints.empty())
			{
				Utilities::drawKeypoints(frame, localKeypoints.data(), localKeypoints.size(), offset);
			}
			break;
		}

		default:
			ocean_assert(false && "Never be here");
			break;
	}
}

} // namespace UnifiedFeatures

} // namespace Ocean
