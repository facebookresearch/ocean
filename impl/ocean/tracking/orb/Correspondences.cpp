/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/orb/Correspondences.h"

#include "ocean/geometry/Quadric.h"

namespace Ocean
{

namespace Tracking
{

namespace ORB
{

using namespace CV;
using namespace CV::Detector;

void Correspondences::extractCorrespondingPoints(const ORBFeatures& forwardFeatures, const ORBFeatures& backwardFeatures, const CorrespondencePairs& correspondences, Geometry::ImagePoints& imagePoints, Geometry::ObjectPoints& objectPoints)
{
	ocean_assert(objectPoints.empty());
	ocean_assert(imagePoints.empty());

	objectPoints.reserve(correspondences.size());
	imagePoints.reserve(correspondences.size());

	for (CorrespondencePairs::const_iterator i = correspondences.begin(); i != correspondences.end(); ++i)
	{
		ocean_assert(i->first < forwardFeatures.size());
		ocean_assert(i->second < backwardFeatures.size());

		const ORBFeature& forwardFeature = forwardFeatures[i->first];

		imagePoints.push_back(forwardFeature.observation());
		objectPoints.push_back(backwardFeatures[i->second].position());
	}
}

void Correspondences::extractCorrespondingFeatures(const ORBFeatures& forwardFeatures, const ORBFeatures& backwardFeatures, const CorrespondencePairs& correspondencePairs, ORBFeatures& forwardCorrespondences, ORBFeatures& backwardCorrespondences)
{
	ocean_assert(forwardCorrespondences.empty());
	ocean_assert(backwardCorrespondences.empty());

	forwardCorrespondences.reserve(correspondencePairs.size());
	backwardCorrespondences.reserve(correspondencePairs.size());

	for (CorrespondencePairs::const_iterator i = correspondencePairs.begin(); i != correspondencePairs.end(); ++i)
	{
		ocean_assert(i->first < forwardFeatures.size());
		ocean_assert(i->second < backwardFeatures.size());

		forwardCorrespondences.push_back(forwardFeatures[i->first]);
		backwardCorrespondences.push_back(backwardFeatures[i->second]);
	}
}

void Correspondences::extractCorrespondingFeatures(const ORBFeatures& forwardFeatures, const ORBFeatures& backwardFeatures, const CorrespondencePairs& correspondencePairs, const HomogenousMatrix4& pose, const PinholeCamera& pinholeCamera, const Scalar sqrDistance, ORBFeatures& forwardCorrespondences, ORBFeatures& backwardCorrespondences)
{
	ocean_assert(forwardCorrespondences.empty());
	ocean_assert(backwardCorrespondences.empty());

	forwardCorrespondences.reserve(correspondencePairs.size());
	backwardCorrespondences.reserve(correspondencePairs.size());

	const HomogenousMatrix4 flippedPose(PinholeCamera::flippedTransformationRightSide(pose));
	const HomogenousMatrix4 iFlippedPose(flippedPose.inverted());

	for (CorrespondencePairs::const_iterator i = correspondencePairs.begin(); i != correspondencePairs.end(); ++i)
	{
		ocean_assert(i->first < forwardFeatures.size());
		ocean_assert(i->second < backwardFeatures.size());

		const ORBFeature& forwardFeature(forwardFeatures[i->first]);
		const ORBFeature& backwardFeature(backwardFeatures[i->second]);

		const Vector2 projectedObject(pinholeCamera.projectToImageIF<true>(iFlippedPose, backwardFeature.position(), false));

		if ((forwardFeature.observation() - projectedObject).sqr() < sqrDistance)
		{
			forwardCorrespondences.push_back(forwardFeature);
			backwardCorrespondences.push_back(backwardFeature);
		}
	}
}

void Correspondences::extractCorrespondingFeatures(const ORBFeatures& forwardFeatures, const ORBFeatures& backwardFeatures, const CorrespondencePairs& candidatePairs, const FeatureIndices& correspondenceIndices, ORBFeatures& forwardCorrespondences, ORBFeatures& backwardCorrespondences)
{
	ocean_assert(forwardCorrespondences.empty());
	ocean_assert(backwardCorrespondences.empty());

	forwardCorrespondences.reserve(correspondenceIndices.size());
	backwardCorrespondences.reserve(correspondenceIndices.size());

	for (FeatureIndices::const_iterator i = correspondenceIndices.begin(); i != correspondenceIndices.end(); ++i)
	{
		ocean_assert(*i < candidatePairs.size());

		const CorrespondencePair& candidatePair(candidatePairs[*i]);

		ocean_assert(candidatePair.first < forwardFeatures.size());
		ocean_assert(candidatePair.second < backwardFeatures.size());

		forwardCorrespondences.push_back(forwardFeatures[candidatePair.first]);
		backwardCorrespondences.push_back(backwardFeatures[candidatePair.second]);
	}
}

void Correspondences::determineCandidates(const ORBFeatures& interestFeatures, const ORBFeatures& candidatePool, const Scalar window, MultiCandidates& multiCandidates, FeatureIndices& candidates)
{
	ocean_assert(window > 0);
	const Scalar windowHalf = window * Scalar(0.5);

	multiCandidates.clear();
	multiCandidates.resize(interestFeatures.size());

	CandidateSet candidateSet;

	for (unsigned int i = 0; i < interestFeatures.size(); ++i)
	{
		FeatureIndices& indices = multiCandidates[i];
		const ORBFeature& interest = interestFeatures[i];

		const Scalar minX = interest.observation().x() - windowHalf;
		const Scalar maxX = interest.observation().x() + windowHalf;
		const Scalar minY = interest.observation().y() - windowHalf;
		const Scalar maxY = interest.observation().y() + windowHalf;

		for (unsigned int c = 0; c < candidatePool.size(); ++c)
		{
			const ORBFeature& candidate = candidatePool[c];

			if (candidate.observation().x() >= minX && candidate.observation().x() <= maxX && candidate.observation().y() >= minY && candidate.observation().y() <= maxY)
			{
				indices.push_back(c);
				candidateSet.insert(c);
			}
		}
	}

	candidates.clear();
	candidates.reserve(candidateSet.size());

	for (CandidateSet::const_iterator i = candidateSet.begin(); i != candidateSet.end(); ++i)
		candidates.push_back(*i);
}

}

}

}
