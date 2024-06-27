/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/UnidirectionalCorrespondences.h"

#include "ocean/geometry/SpatialDistribution.h"

namespace Ocean
{

namespace Tracking
{

UnidirectionalCorrespondences::UnidirectionalCorrespondences(const size_t numberForward, const size_t numberBackward)
{
	forwardCounters_.resize(numberForward);
	backwardCounters_.resize(numberBackward);
}

UnidirectionalCorrespondences::CorrespondencePairs UnidirectionalCorrespondences::correspondences() const
{
	const ScopedLock scopedLock(lock_);

	// let's find unidirectional correspondences

	CorrespondencePairs validCorrespondences;
	validCorrespondences.reserve(std::min(forwardCounters_.size(), backwardCounters_.size()) * 10 / 100);

	for (const CorrespondencePair& candidate : candidates_)
	{
		if (forwardCounters_[candidate.first] == 1u && backwardCounters_[candidate.second] == 1u)
		{
			validCorrespondences.push_back(candidate);
		}
	}

	return validCorrespondences;
}

}

}
