/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/mapbuilding/UnifiedMatching.h"

#include "ocean/base/WorkerPool.h"

namespace Ocean
{

namespace Tracking
{

namespace MapBuilding
{

void UnifiedGuidedMatchingFreakMultiDescriptor256Group::determineGuidedMatchings(const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_camera, Vectors2& matchedImagePoints, Vectors3& matchedObjectPoints, const DistanceValue& maximalDescriptorDistance, Indices32* matchedImagePointIndices, Indices32* matchedObjectPointIds, Worker* worker) const
{
	PoseEstimationT::determineGuidedMatchings<ImagePointDescriptorGroup, ObjectPointDescriptor, DescriptorDistance, DescriptorHandling::determineFreakDistance>(anyCamera, world_T_camera, imagePoints_, imagePointDescriptorGroups_, numberImagePoints_, objectPoints_, objectPointOctree_, objectPointIds_, objectPointDescriptorMap_, matchedImagePoints, matchedObjectPoints, maximalDescriptorDistance.binaryDistance(), matchedImagePointIndices, matchedObjectPointIds, worker);
}

bool UnifiedUnguidedMatchingFreakMultiFeatures256Group::determineUnguidedMatchings(const unsigned int minimalNumberCorrespondences, const DistanceValue& maximalDescriptorDistance, Vectors2& matchedImagePoints, Vectors3& matchedObjectPoints, Worker* worker) const
{
	if (imagePoints_ == nullptr || imagePointDescriptorGroups_ == nullptr || numberImagePoints_ == 0)
	{
		return false;
	}

	BinaryVocabularyForest::Matches matches;
	forestObjectPointDescriptors_.matchMultiDescriptorGroups<ImagePointDescriptorGroup, ImagePointDescriptor, DescriptorHandling::multiDescriptorGroupFunction, DescriptorHandling::multiDescriptorFunction, BinaryVocabularyTree::MM_ALL_GOOD_LEAFS_2>(objectPointDescriptors_, imagePointDescriptorGroups_, numberImagePoints_, maximalDescriptorDistance.binaryDistance(), matches, worker);

	if (matches.size() < size_t(minimalNumberCorrespondences))
	{
		return false;
	}

	ocean_assert(matchedImagePoints.empty());
	ocean_assert(matchedObjectPoints.empty());

	matchedImagePoints.clear();
	matchedObjectPoints.clear();

	for (const BinaryVocabularyTree::Match& match : matches)
	{
		matchedImagePoints.emplace_back(imagePoints_[match.queryDescriptorIndex()]);
		matchedObjectPoints.emplace_back(objectPoints_[objectPointIndices_[match.candidateDescriptorIndex()]]);
	}

	return true;
}

}

}

}
