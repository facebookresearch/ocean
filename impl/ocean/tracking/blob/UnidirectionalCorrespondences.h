/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_BLOB_UNIDIRECTIONAL_CORRESPONDENCES_H
#define META_OCEAN_TRACKING_BLOB_UNIDIRECTIONAL_CORRESPONDENCES_H

#include "ocean/tracking/blob/Blob.h"
#include "ocean/tracking/blob/Correspondences.h"

#include "ocean/base/Lock.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/detector/blob/BlobFeature.h"

#include "ocean/math/PinholeCamera.h"

#include <vector>

namespace Ocean
{

namespace Tracking
{

namespace Blob
{

/**
 * This class provides unidirectional feature correspondences.
 * @ingroup trackingblob
 */
class OCEAN_TRACKING_BLOB_EXPORT UnidirectionalCorrespondences : public Correspondences
{
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
		 * Determines unidirectional features correspondences for two sets of features without any previous knowledge (applying a brute force search) using the minimal summed square distance (SSD).
		 * This function is slower than determineFeatureCorrespondencesEarlyReject() but determines correspondences based on precise SSD calculations.
		 * @param forwardFeatures Forward features (e.g. of a live camera)
		 * @param backwardFeatures Backward features (e.g. of a feature map)
		 * @param numberForward Number of forward features to be used, may be less than the acutal forward features, with range [1, forwardFeatures.size()]
		 * @param threshold The maximal distance between two feature descriptors so that the matching still counts as valid correspondence, with range (0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return Resulting correspondences
		 * @see determineFeatureCorrespondencesEarlyReject(), CV::Detector::Blob::BlobDescriptor::isDescriptorEqual(), CV::Detector::Blob::BlobDescriptor::isDescriptorEqualEarlyReject().
		 */
		static CorrespondencePairs determineFeatureCorrespondences(const CV::Detector::Blob::BlobFeatures& forwardFeatures, const CV::Detector::Blob::BlobFeatures& backwardFeatures, const size_t numberForward, const Scalar threshold, Worker* worker = nullptr);

		/**
		 * Determines unidirectional features correspondences for two sets of features without any previous knowledge (applying a brute force search) using the minimal summed square distance (SSD).
		 * This function measures the ratio between the best SSD and second best SSD and accepts the matching for the best SSD as long as the ratio between best and second is below a provided threshold.<br>
		 * This function is slower than determineFeatureCorrespondencesWithQualityEarlyReject() but determines correspondences based on precise SSD calculations.
		 * @param forwardFeatures Forward features (e.g. of a live camera)
		 * @param backwardFeatures Backward features (e.g. of a feature map)
		 * @param numberForward Number of forward features to be used, may be less than the acutal forward features, with range [1, forwardFeatures.size()]
		 * @param threshold The maximal distance between two feature descriptors so that the matching still counts as valid correspondence, with range (0, infinity)
		 * @param qualityRatio Maximal allowed ratio between best and second best matching SSD, with range (0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return Resulting correspondences
		 * @see determineFeatureCorrespondencesWithQualityEarlyReject(), CV::Detector::Blob::BlobDescriptor::isDescriptorEqual(), CV::Detector::Blob::BlobDescriptor::isDescriptorEqualEarlyReject().
		 */
		static CorrespondencePairs determineFeatureCorrespondencesWithQuality(const CV::Detector::Blob::BlobFeatures& forwardFeatures, const CV::Detector::Blob::BlobFeatures& backwardFeatures, const size_t numberForward, const Scalar threshold, const Scalar qualityRatio, Worker* worker = nullptr);

		/**
		 * Determines unidirectional features correspondences for two sets of features without any previous knowledge (applying a brute force search) using the minimal summed square distance (SSD).
		 * This function is faster than determineFeatureCorrespondences() but applys an early reject strategy which is not guaranteed to be correct in any case.
		 * @param forwardFeatures Forward features (e.g. of a live camera)
		 * @param backwardFeatures Backward features (e.g. of a feature map)
		 * @param numberForward Number of forward features to be used, may be less than the acutal forward features, with range [1, forwardFeatures.size()]
		 * @param threshold The maximal distance between two feature descriptors so that the matching still counts as valid correspondence, with range (0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return Resulting correspondences
		 * @see determineFeatureCorrespondences(), CV::Detector::Blob::BlobDescriptor::isDescriptorEqual(), CV::Detector::Blob::BlobDescriptor::isDescriptorEqualEarlyReject().
		 */
		static CorrespondencePairs determineFeatureCorrespondencesEarlyReject(const CV::Detector::Blob::BlobFeatures& forwardFeatures, const CV::Detector::Blob::BlobFeatures& backwardFeatures, const size_t numberForward, const Scalar threshold, Worker* worker = nullptr);

		/**
		 * Determines unidirectional features correspondences for two sets of features without any previous knowledge (applying a brute force search) using the minimal summed square distance (SSD).
		 * This function measures the ratio between the best SSD and second best SSD and accepts the matching for the best SSD as long as the ratio between best and second is below a provided threshold.<br>
		 * This function is faster than determineFeatureCorrespondences() but applys an early reject strategy which is not guaranteed to be correct in any case.
		 * @param forwardFeatures Forward features (e.g. of a live camera)
		 * @param backwardFeatures Backward features (e.g. of a feature map)
		 * @param numberForward Number of forward features to be used, may be less than the acutal forward features, with range [1, forwardFeatures.size()]
		 * @param threshold The maximal distance between two feature descriptors so that the matching still counts as valid correspondence, with range (0, infinity)
		 * @param qualityRatio Maximal allowed ratio between best and second best matching SSD, with range (0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return Resulting correspondences
		 * @see determineFeatureCorrespondencesWithQuality(), CV::Detector::Blob::BlobDescriptor::isDescriptorEqual(), CV::Detector::Blob::BlobDescriptor::isDescriptorEqualEarlyReject().
		 */
		static CorrespondencePairs determineFeatureCorrespondencesWithQualityEarlyReject(const CV::Detector::Blob::BlobFeatures& forwardFeatures, const CV::Detector::Blob::BlobFeatures& backwardFeatures, const size_t numberForward, const Scalar threshold, const Scalar qualityRatio, Worker* worker = nullptr);

		/**
		 * Determines unidirectional features correspondences from a set of already found candidates using the minimal summed square distance (SSD).
		 * @param interestFeatures Features of interest
		 * @param featurePool Pool of features building the correspondence
		 * @param multiCandidates Set of already found feature candidates
		 * @param threshold Minimal feature descriptor matching threshold
		 * @return correspondences Resulting correspondences
		 */
		static CorrespondencePairs determineFeatureCorrespondences(const CV::Detector::Blob::BlobFeatures& interestFeatures, const CV::Detector::Blob::BlobFeatures& featurePool, const MultiCandidates& multiCandidates, const Scalar threshold);

		/**
		 * Determines unidirectional features correspondences from a set of already found candidates using the minimal summed square distance (SSD).
		 * @param interestFeatures Features of interest
		 * @param featurePool Pool of features building the correspondence
		 * @param multiCandidates Set of already found feature candidates
		 * @param threshold Minimal feature descriptor matching threshold
		 * @param qualityRatio Maximal ratio between best and second best matching SSD
		 * @return correspondences Resulting correspondences
		 */
		static CorrespondencePairs determineFeatureCorrespondences(const CV::Detector::Blob::BlobFeatures& interestFeatures, const CV::Detector::Blob::BlobFeatures& featurePool, const MultiCandidates& multiCandidates, const Scalar threshold, const Scalar qualityRatio);

		/**
		 * Determines unidirectional feature correspondences between a set of given 2D interest features and 2D map features (applies a guided matching).
		 * @param map_H_interest The homography converting 2D interest points to 2D map points, (mapPoint = map_H_interest * interestPoint), must be valid
		 * @param interestFeatures The 2D interest featuress for which corresponding 3D features will be determined
		 * @param featureMapFeatures The 3D feature candidates for correspondences
		 * @param numberInterestFeatures Number of interest features to find 3D features for, with range [1, interestFeatures.size()]
		 * @param featureMapWidth The width of the feature map image in pixel, with range (0, infinity)
		 * @param featureMapHeight The height of the feature map image in pixel, with range (0, infinity)
		 * @param maximalProjectionError The maximal projection error between projected 3D feature candidates and corresponding 2D image features, with range [0, infinity)
		 * @param threshold Minimal feature descriptor matching threshold, with range (0, infinity)
		 * @param qualityRatio Maximal ratio between best and second best matching SSD, with range (0, infinity)
		 * @return Resulting pairs defining correspondences between interest and map features
		 */
		static CorrespondencePairs determineFeatureCorrespondencesWithHomography(const SquareMatrix3& map_H_interest, const CV::Detector::Blob::BlobFeatures& interestFeatures, const CV::Detector::Blob::BlobFeatures& featureMapFeatures, const size_t numberInterestFeatures, const unsigned int featureMapWidth, const unsigned int featureMapHeight, const Scalar maximalProjectionError, const Scalar threshold, const Scalar qualityRatio);

		/**
		 * Determines unidirectional feature correspondences between a set of given 2D interest features and 3D map features using a rough 6-DOF pose (applies a guided matching).
		 * @param anyCamera The camera object defining the projection parameters, must be valid
		 * @param world_P_camera Rough pose to be used to find 3D features for the 2D interest features, must be valid
		 * @param interestFeatures The 2D interest features for which corresponding 3D features will be determined
		 * @param featureMapFeatures The 3D feature candidates for correspondences
		 * @param numberInterestFeatures Number of interest features to find 3D features for, with range [1, interestFeatures.size()]
		 * @param maximalProjectionError The maximal projection error between projected 3D feature candidates and corresponding 2D image features, with range [0, infinity)
		 * @param threshold Minimal feature descriptor matching threshold, with range (0, infinity)
		 * @param qualityRatio Maximal ratio between best and second best matching SSD, with range (0, infinity)
		 * @return Resulting pairs defining correspondences between interest and map features
		 */
		static CorrespondencePairs determineFeatureCorrespondencesWithPose(const AnyCamera& anyCamera, const HomogenousMatrix4& world_P_camera, const CV::Detector::Blob::BlobFeatures& interestFeatures, const CV::Detector::Blob::BlobFeatures& featureMapFeatures, const size_t numberInterestFeatures, const Scalar maximalProjectionError, const Scalar threshold, const Scalar qualityRatio);

	private:

		/**
		 * Determines unidirectional features correspondences for two sets of features without any previous knowledge (applying a brute force search) using the minimal summed square distance (SSD).
		 * @param forwardFeatures Forward features (e.g. of a live camera)
		 * @param backwardFeatures Backward features (e.g. of a feature map)
		 * @param numberForward Number of forward features to be used, may be less than the acutal forward features, with range [1, forwardFeatures.size()]
		 * @param threshold The maximal distance between two feature descriptors so that the matching still counts as valid correspondence, with range (0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return Resulting correspondences
		 * @tparam tEarlyReject True, to apply a SSD determination with early reject strategy; False, to apply a full SSD determination
		 * @see determineFeatureCorrespondencesWithQuality(), CV::Detector::Blob::BlobDescriptor::isDescriptorEqual(), CV::Detector::Blob::BlobDescriptor::isDescriptorEqualEarlyReject().
		 */
		template <bool tEarlyReject>
		static CorrespondencePairs determineFeatureCorrespondences(const CV::Detector::Blob::BlobFeatures& forwardFeatures, const CV::Detector::Blob::BlobFeatures& backwardFeatures, const size_t numberForward, const Scalar threshold, Worker* worker = nullptr);

		/**
		 * Determines unidirectional features correspondences for two sets of features without any previous knowledge (applying a brute force search) using the minimal summed square distance (SSD).
		 * This function measures the ratio between the best SSD and second best SSD and accepts the matching for the best SSD as long as the ratio between best and second is below a provided threshold.
		 * @param forwardFeatures Forward features (e.g. of a live camera)
		 * @param backwardFeatures Backward features (e.g. of a feature map)
		 * @param numberForward Number of forward features to be used, may be less than the acutal forward features, with range [1, forwardFeatures.size()]
		 * @param threshold The maximal distance between two feature descriptors so that the matching still counts as valid correspondence, with range (0, infinity)
		 * @param qualityRatio Maximal allowed ratio between best and second best matching SSD, with range (0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return Resulting correspondences
		 * @tparam tEarlyReject True, to apply a SSD determination with early reject strategy; False, to apply a full SSD determination
		 * @see determineFeatureCorrespondences(), CV::Detector::Blob::BlobDescriptor::isDescriptorEqual(), CV::Detector::Blob::BlobDescriptor::isDescriptorEqualEarlyReject().
		 */
		template <bool tEarlyReject>
		static CorrespondencePairs determineFeatureCorrespondencesWithQuality(const CV::Detector::Blob::BlobFeatures& forwardFeatures, const CV::Detector::Blob::BlobFeatures& backwardFeatures, const size_t numberForward, const Scalar threshold, const Scalar qualityRatio, Worker* worker = nullptr);

		/**
		 * Determines unidirectional features correspondences for a subset of two sets of features without any previous knowledge (applying a brute force search) using the minimal summed square distance (SSD).
		 * @param forwardFeatures Forward features (e.g. of a live camera)
		 * @param backwardFeatures Backward features (e.g. of a feature map)
		 * @param backwardFeaturesSize Number of backward features, with range [1, infinity)
		 * @param threshold The maximal distance between two feature descriptors so that the matching still counts as valid correspondence, with range (0, infinity)
		 * @param forwardIndicesForBackwardFeatures Indices of matching forward features, one for each backward feature, -1 if no match exists, -2 if no unique match exists
		 * @param locks Optional lock objects necessary if this function is executed in a multi core manner, as manny lock objects as defined by tLocks
		 * @param firstForward First forward feature to be handled, with range [0, forwardFeaturesSize)
		 * @param numberForwards Number of forward features to be handled, with range [1, forwardFeaturesSize - firstForward]
		 * @tparam tEarlyReject True, to apply a SSD determination with early reject strategy; False, to apply a full SSD determination
		 * @tparam tLocks The number of lock objects provided via 'locks', 0 if this function is not executed in a multire core manner, with range [0, infinity)
		 * @see determineFeatureCorrespondences().
		 */
		template <bool tEarlyReject, unsigned int tLocks>
		static void determineFeatureCorrespondencesSubset(const CV::Detector::Blob::BlobFeature* forwardFeatures, const CV::Detector::Blob::BlobFeature* backwardFeatures, const unsigned int backwardFeaturesSize, const Scalar threshold, Index32* forwardIndicesForBackwardFeatures, Lock* locks, const unsigned int firstForward, const unsigned int numberForwards);

		/**
		 * Determines unidirectional features correspondences for a subset of two sets of features without any previous knowledge (applying a brute force search) using the minimal summed square distance (SSD).
		 * @param forwardFeatures Forward features (e.g. of a live camera)
		 * @param backwardFeatures Backward features (e.g. of a feature map)
		 * @param backwardFeaturesSize Number of backward features, with range [1, infinity)
		 * @param threshold The maximal distance between two feature descriptors so that the matching still counts as valid correspondence, with range (0, infinity)
		 * @param qualityRatio Maximal allowed ratio between best and second best matching SSD, with range (0, infinity)
		 * @param forwardIndicesForBackwardFeatures Indices of matching forward features, one for each backward feature, -1 if no match exists, -2 if no unique match exists
		 * @param locks Optional lock objects necessary if this function is executed in a multi core manner, as manny lock objects as defined by tLocks
		 * @param firstForward First forward feature to be handled, with range [0, forwardFeaturesSize)
		 * @param numberForwards Number of forward features to be handled, with range [1, forwardFeaturesSize - firstForward]
		 * @tparam tEarlyReject True, to apply a SSD determination with early reject strategy; False, to apply a full SSD determination
		 * @tparam tLocks The number of lock objects provided via 'locks', 0 if this function is not executed in a multire core manner, with range [0, infinity)
		 * @see determineFeatureCorrespondencesWithQuality().
		 */
		template <bool tEarlyReject, unsigned int tLocks>
		static void determineFeatureCorrespondencesWithQualitySubset(const CV::Detector::Blob::BlobFeature* forwardFeatures, const CV::Detector::Blob::BlobFeature* backwardFeatures, const unsigned int backwardFeaturesSize, const Scalar threshold, const Scalar qualityRatio, Index32* forwardIndicesForBackwardFeatures, Lock* locks, const unsigned int firstForward, const unsigned int numberForwards);

		/**
		 * Determines feature correspondences for two sets of Blob features using the minimal summed square distance (SSD).
		 * @param forwardFeatures First set of Blob features
		 * @param backwardFeatures Second set of Blob features
		 * @param threshold Minimal feature descriptor matching threshold
		 * @param correspondencesCandidates Resulting correspondence pairs
		 * @param firstForward Specifies the index of the first forward feature to be handled
		 * @param numberForwards Specifies the number of features to be handled, with startpoint as specified
		 */
		static void determineFeatureCorrespondences(const CV::Detector::Blob::BlobFeatures* forwardFeatures, const CV::Detector::Blob::BlobFeatures* backwardFeatures, const Scalar threshold, UnidirectionalCorrespondences* correspondencesCandidates, const unsigned int firstForward, const unsigned int numberForwards);

		/**
		 * Determines feature correspondences for two sets of Blob features using the minimal summed square distance (SSD).
		 * @param forward First set of Blob features
		 * @param backward Second set of Blob features
		 * @param threshold Minimal feature descriptor matching threshold
		 * @param qualityRatio Maximal ratio between best and second best matching SSD
		 * @param correspondencesCandidates Resulting correspondence pairs
		 * @param firstForward Specifies the index of the first forward feature to be handled
		 * @param numberForwards Specifies the number of features to be handled, with startpoint as specified
		 */
		static void determineFeatureCorrespondences(const CV::Detector::Blob::BlobFeatures* forward, const CV::Detector::Blob::BlobFeatures* backward, const Scalar threshold, const Scalar qualityRatio, UnidirectionalCorrespondences* correspondencesCandidates, const unsigned int firstForward, const unsigned int numberForwards);

	private:

		/// Correspondence counter for forward features.
		Indices32 forwardCounters;

		/// Correspondence counter for backward features.
		Indices32 backwardCounters;

		/// Correspondence candidates.
		CorrespondencePairs candidates;

		/// Lock object.
		Lock lock;
};

}

}

}

#endif // META_OCEAN_TRACKING_BLOB_UNIDIRECTIONAL_CORRESPONDENCES_H
