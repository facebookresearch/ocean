/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_ORB_CORRESPONDENCES_H
#define META_OCEAN_TRACKING_ORB_CORRESPONDENCES_H

#include "ocean/tracking/orb/ORB.h"
#include "ocean/tracking/orb/FeatureMap.h"

#include "ocean/cv/detector/ORBFeature.h"

#include "ocean/geometry/Geometry.h"

#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Quaternion.h"

#include <set>
#include <vector>

namespace Ocean
{

namespace Tracking
{

namespace ORB
{

/**
 * This class implements simple helper functions for feature correspondences.
 * @ingroup trackingorb
 */
class OCEAN_TRACKING_ORB_EXPORT Correspondences
{
	public:

		/**
		 * Definition of a pair holding the indices of two corresponding features.
		 */
		typedef std::pair<unsigned int, unsigned int> CorrespondencePair;

		/**
		 * Definition of a vector holding correspondence pairs.
		 */
		typedef std::vector<CorrespondencePair> CorrespondencePairs;

		/**
		 * Definition of a vector holding different feature indices.
		 */
		typedef std::vector<CV::Detector::FeatureIndices> MultiCandidates;

	private:

		/**
		 * Definition of a set holding feature indices.
		 */
		typedef std::set<unsigned int> CandidateSet;

	public:

		/**
		 * Extracts object and image positions of ORB features by pairs of feature correspondences.
		 * Beware: The indices of the feature correspondences must not exceed the number of forward or backward features respectively!
		 * @param forwardFeatures Forward or image features
		 * @param backwardFeatures Backward or object features
		 * @param correspondences Pairs of feature correspondences
		 * @param imagePoints Resultling image points taken form the forward features
		 * @param objectPoints Resulting object points taken from the backward features
		 */
		static void extractCorrespondingPoints(const CV::Detector::ORBFeatures& forwardFeatures, const CV::Detector::ORBFeatures& backwardFeatures, const CorrespondencePairs& correspondences, Geometry::ImagePoints& imagePoints, Geometry::ObjectPoints& objectPoints);

		/**
		 * Extracts object and image features by pairs of feature correspondences all correspondences are expected to be valid.
		 * @param forwardFeatures Forward or image features building the correspondences
		 * @param backwardFeatures Backward or object features building the correspondences
		 * @param correspondencePairs Pairs of correspondence candidates defining a correspondence between forward and backward features
		 * @param forwardCorrespondences Resulting features from the forward correspondences
		 * @param backwardCorrespondences Resulting feature from the backward correspondences
		 */
		static void extractCorrespondingFeatures(const CV::Detector::ORBFeatures& forwardFeatures, const CV::Detector::ORBFeatures& backwardFeatures, const CorrespondencePairs& correspondencePairs, CV::Detector::ORBFeatures& forwardCorrespondences, CV::Detector::ORBFeatures& backwardCorrespondences);

		/**
		 * Extracts object and image features by pairs of feature correspondences all correspondences are expected to be candidates.
		 * However, feature correspondences are extracted only if their projection error is below a given threshold.
		 * @param forwardFeatures Forward or image features building the correspondences
		 * @param backwardFeatures Backward or object features building the correspondences
		 * @param correspondencePairs Pairs of correspondence candidates defining a correspondence between forward and backward features
		 * @param pose Pose corresponding to the determined corresponding pairs
		 * @param pinholeCamera The pinhole camera used for tracking
		 * @param sqrDistance Maximal square distance of the projected object feature and the image feature
		 * @param forwardCorrespondences Resulting features from the forward correspondences
		 * @param backwardCorrespondences Resulting feature from the backward correspondences
		 */
		static void extractCorrespondingFeatures(const CV::Detector::ORBFeatures& forwardFeatures, const CV::Detector::ORBFeatures& backwardFeatures, const CorrespondencePairs& correspondencePairs, const HomogenousMatrix4& pose, const PinholeCamera& pinholeCamera, const Scalar sqrDistance, CV::Detector::ORBFeatures& forwardCorrespondences, CV::Detector::ORBFeatures& backwardCorrespondences);

		/**
		 * Extracts object and image features by pairs of feature correspondence candidates.
		 * Some candidates are expected to be invalid, thus additional indices specify the valid candidates.
		 * @param forwardFeatures Forward or image features building the correspondences
		 * @param backwardFeatures Backward or object features building the correspondences
		 * @param candidatePairs Pairs of correspondence candidates defining a correspondence between forward and backward features
		 * @param correspondenceIndices Indices specifying the valid pairs of corresponding candidates
		 * @param forwardCorrespondences Resulting features from the forward correspondences
		 * @param backwardCorrespondences Resulting feature from the backward correspondences
		 */
		static void extractCorrespondingFeatures(const CV::Detector::ORBFeatures& forwardFeatures, const CV::Detector::ORBFeatures& backwardFeatures, const CorrespondencePairs& candidatePairs, const CV::Detector::FeatureIndices& correspondenceIndices, CV::Detector::ORBFeatures& forwardCorrespondences, CV::Detector::ORBFeatures& backwardCorrespondences);

		/**
		 * Determines candidates for feature correspondences upon their spatial position in the frame.
		 * @param interestFeatures ORB features of interest to find candidates for
		 * @param candidatePool A pool of ORB features holding all possible candidates
		 * @param window Size of the search window for possible candidates
		 * @param multiCandidates Lists of feature candidates for each feature of interest
		 * @param candidates List of all feature candidates
		 */
		static void determineCandidates(const CV::Detector::ORBFeatures& interestFeatures, const CV::Detector::ORBFeatures& candidatePool, const Scalar window, MultiCandidates& multiCandidates, CV::Detector::FeatureIndices& candidates);
};

}

}

}

#endif // META_OCEAN_TRACKING_ORB_CORRESPONDENCES_H
