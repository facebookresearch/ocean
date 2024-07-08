/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_ORB_UNIDIRECTIONAL_CORRESPONDENCES_H
#define META_OCEAN_TRACKING_ORB_UNIDIRECTIONAL_CORRESPONDENCES_H

#include "ocean/tracking/orb/ORB.h"
#include "ocean/tracking/orb/Correspondences.h"

#include "ocean/base/Lock.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/detector/ORBFeature.h"

#include "ocean/math/PinholeCamera.h"

#include <vector>

namespace Ocean
{

namespace Tracking
{

namespace ORB
{

/**
 * This class provides unidirectional feature correspondences.
 * @ingroup trackingorb
 */
class OCEAN_TRACKING_ORB_EXPORT UnidirectionalCorrespondences : public Correspondences
{
	public:

		/**
		 * Definition of a vector counting correspondences.
		 */
		typedef std::vector<unsigned int> CorrespondenceCounters;

	public:

		/**
		 * Creates a new unidirecitonal correspondences object.
		 * @param numberForward Number of forward features
		 * @param numberBackward Number of backward features
		 */
		UnidirectionalCorrespondences(const unsigned int numberForward, const unsigned int numberBackward);

		/**
		 * Adds a new feature correspondence candidate.
		 * The given indices must be below the number of specified features.<br>
		 * Beware: This function is not threadsafe!
		 * @param forwardIndex Index of the forward feature
		 * @param backwardIndex Index of the best matching backward feature
		 */
		void addCandidate(const unsigned int forwardIndex, const unsigned int backwardIndex);

		/**
		 * Adds a new feature correspondence candidate.
		 * The given indices must be below the number of specified features.<br>
		 * This function is threadsafe.
		 * @param forwardIndex Index of the forward feature
		 * @param backwardIndex Index of the best matching backward feature
		 */
		void lockedAddCandidate(const unsigned int forwardIndex, const unsigned int backwardIndex);

		/**
		 * Returns all uni-directional feature correspondences.
		 * Beware: This function is not threadsafe!
		 * @return Unique feature correspondences
		 */
		CorrespondencePairs correspondences() const;

		/**
		 * Determines unidirectional features correspondences between two sets of features without any previous knowledge.
		 * @param forwardFeatures The forward features for which corresponding backward features will be determined, at least one
		 * @param backwardFeatures The backward features to be used
		 * @param threshold The percentage (in relation to the number of descriptor bits) of the maximal hamming distance so that two descriptors count as similar, with range [0, 1]
		 * @param worker Optional worker object to distribute the computation to several CPU cores
		 * @return The determined resulting correspondences
		 */
		static CorrespondencePairs determineBijectiveCorrespondences(const CV::Detector::ORBFeatures& forwardFeatures, const CV::Detector::ORBFeatures& backwardFeatures, const float threshold = 0.15f, Worker* worker = nullptr);

		/**
		 * Determines unidirectional features correspondences of two sets of features with previous knowledge.
		 * Based on a given camera profile and a rough camera pose, feature candidates are rejected if the projected (backward) feature points is not close enough to the corresponding (forward) feature point.
		 * @param forwardFeatures The forward features for which corresponding backward features will be determined, at least one
		 * @param backwardFeatures The backward features to be used
		 * @param pinholeCamera The pinhole camera profile defining the projection, must be valid
		 * @param pose The rough camera pose, e.g., a pose from the previous frame, must be valid
		 * @param threshold The percentage (in relation to the number of descriptor bits) of the maximal hamming distance so that two descriptors count as similar, with range [0, 1]
		 * @return correspondencePairs Resulting correspondences
		 */
		static CorrespondencePairs determineBijectiveCorrespondencesWithPose(const CV::Detector::ORBFeatures& forwardFeatures, const CV::Detector::ORBFeatures& backwardFeatures, const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose, const float threshold = 0.15f);

	private:

		/// Correspondence counter for forward features.
		CorrespondenceCounters correspondenceForwardCounters;

		/// Correspondence counter for backward features.
		CorrespondenceCounters correspondenceBackwardCounters;

		/// Correspondence candidates.
		CorrespondencePairs correspondenceCandidates;

		/// Lock object.
		Lock correspondenceLock;
};

}

}

}

#endif // META_OCEAN_TRACKING_ORB_UNIDIRECTIONAL_CORRESPONDENCES_H
