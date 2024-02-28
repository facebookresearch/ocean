// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_TRACKING_BLOB_CORRESPONDENCES_H
#define META_OCEAN_TRACKING_BLOB_CORRESPONDENCES_H

#include "ocean/tracking/blob/Blob.h"
#include "ocean/tracking/blob/FeatureMap.h"

#include "ocean/cv/detector/blob/BlobFeature.h"

#include "ocean/geometry/Geometry.h"

#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Quaternion.h"

#include <set>
#include <vector>

namespace Ocean
{

namespace Tracking
{

namespace Blob
{

/**
 * This class implements simple helper functions for feature correspondences.
 * @ingroup trackingblob
 */
class OCEAN_TRACKING_BLOB_EXPORT Correspondences
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
		 * Determines features correspondences for two sets of features without any previous knowledge (applying a brute force search) using the minimal summed square distance (SSD).
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
		 * Determines features correspondences for two sets of features without any previous knowledge (applying a brute force search) using the minimal summed square distance (SSD).
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
		 * Determines features correspondences between sets of 2D and 3D features with a known rough camera pose.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param model_T_camera The rough camera pose defining the transformation between camera and model, must be valid
		 * @param cameraFeatures The 2D features located in the camera, must have valid 2D observation coordinates, must be valid
		 * @param modelFeatures The 3D features defined in the coordinate system of the mode, must have valid 3D position coordinates, must be valid
		 * @param numberCameraFeatures The number of given camera features, with range [4, infinity)
		 * @param numberModelFeatures The number of given model features, with range [4, infinity)
		 * @param threshold The maximal distance between two feature descriptors so that the matching still counts as valid correspondence, with range [0, infinity)
		 * @param maxSqrProjectionError The maximal square projection error between a projected 3D feature and a 2D feature so that both features can be corresponding, with range [0, infinity)
		 * @param earlyReject True, to use SSD calculations with early reject; False, to calculate the full SSD
		 * @param worker Optional worker object to distribute the computation
		 * @return Resulting correspondences
		 * @see determineFeatureCorrespondencesEarlyReject(), CV::Detector::Blob::BlobDescriptor::isDescriptorEqual(), CV::Detector::Blob::BlobDescriptor::isDescriptorEqualEarlyReject().
		 */
		static CorrespondencePairs determineFeatureCorrespondencesGuided(const AnyCamera& camera, const HomogenousMatrix4& model_T_camera, const CV::Detector::Blob::BlobFeature* cameraFeatures, const CV::Detector::Blob::BlobFeature* modelFeatures, const size_t numberCameraFeatures, const size_t numberModelFeatures, const Scalar threshold, const Scalar maxSqrProjectionError, const bool earlyReject = true, Worker* worker = nullptr);

		/**
		 * Deprecated.
		 *
		 * Determines features correspondences between sets of 2D and 3D features with a known rough camera pose.
		 * @param pinholeCamera The pinhole camera profile defining the projection, must be valid
		 * @param model_T_camera The rough camera pose defining the transformation between camera and model, must be valid
		 * @param cameraFeatures The 2D features located in the camera, must have valid 2D observation coordinates, must be valid
		 * @param modelFeatures The 3D features defined in the coordinate system of the mode, must have valid 3D position coordinates, must be valid
		 * @param numberCameraFeatures The number of given camera features, with range [4, infinity)
		 * @param numberModelFeatures The number of given model features, with range [4, infinity)
		 * @param threshold The maximal distance between two feature descriptors so that the matching still counts as valid correspondence, with range [0, infinity)
		 * @param maxSqrProjectionError The maximal square projection error between a projected 3D feature and a 2D feature so that both features can be corresponding, with range [0, infinity)
		 * @param earlyReject True, to use SSD calculations with early reject; False, to calculate the full SSD
		 * @param worker Optional worker object to distribute the computation
		 * @return Resulting correspondences
		 * @see determineFeatureCorrespondencesEarlyReject(), CV::Detector::Blob::BlobDescriptor::isDescriptorEqual(), CV::Detector::Blob::BlobDescriptor::isDescriptorEqualEarlyReject().
		 */
		static inline CorrespondencePairs determineFeatureCorrespondencesGuided(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& model_T_camera, const CV::Detector::Blob::BlobFeature* cameraFeatures, const CV::Detector::Blob::BlobFeature* modelFeatures, const size_t numberCameraFeatures, const size_t numberModelFeatures, const Scalar threshold, const Scalar maxSqrProjectionError, const bool earlyReject = true, Worker* worker = nullptr);

		/**
		 * Extracts object and image positions of Blob features from pairs of feature correspondences.
		 * Beware: The indices of the feature correspondences must not exceed the number of forward or backward features respectively!
		 * @param forwardFeatures Forward or image features
		 * @param backwardFeatures Backward or object features
		 * @param correspondences Pairs of feature correspondences
		 * @param imagePoints Resultling image points taken form the forward features
		 * @param objectPoints Resulting object points taken from the backward features
		 * @param weights Optional resulting weights individual for each feature correspondence calculated by the square of the feature scale inverse
		 */
		static void extractCorrespondingPoints(const CV::Detector::Blob::BlobFeatures& forwardFeatures, const CV::Detector::Blob::BlobFeatures& backwardFeatures, const CorrespondencePairs& correspondences, Geometry::ImagePoints& imagePoints, Geometry::ObjectPoints& objectPoints, Scalars* weights = nullptr);

		/**
		 * Extracts pairs of image positions of Blob features from pairs of feature correspondences.
		 * Beware: The indices of the feature correspondences must not exceed the number of forward or backward features respectively!
		 * @param forwardFeatures Forward or image features
		 * @param backwardFeatures Backward or image features
		 * @param correspondences Pairs of feature correspondences
		 * @param forwardImagePoints Resultling image points taken form the forward features
		 * @param backwardImagePoints Resulting image points taken from the backward features
		 * @param weights Optional resulting weights individual for each feature correspondence calculated by the square of the feature scale inverse
		 */
		static void extractCorrespondingPoints(const CV::Detector::Blob::BlobFeatures& forwardFeatures, const CV::Detector::Blob::BlobFeatures& backwardFeatures, const CorrespondencePairs& correspondences, Geometry::ImagePoints& forwardImagePoints, Geometry::ImagePoints& backwardImagePoints, Scalars* weights = nullptr);

		/**
		 * Extracts object and image features by pairs of feature correspondences all correspondences are expected to be valid.
		 * @param forwardFeatures Forward or image features building the correspondences
		 * @param backwardFeatures Backward or object features building the correspondences
		 * @param correspondencePairs Pairs of correspondence candidates defining a correspondence between forward and backward features
		 * @param forwardCorrespondences Resulting features from the forward correspondneces
		 * @param backwardCorrespondences Resulting feature from the backward correspondences
		 */
		static void extractCorrespondingFeatures(const CV::Detector::Blob::BlobFeatures& forwardFeatures, const CV::Detector::Blob::BlobFeatures& backwardFeatures, const CorrespondencePairs& correspondencePairs, CV::Detector::Blob::BlobFeatures& forwardCorrespondences, CV::Detector::Blob::BlobFeatures& backwardCorrespondences);

		/**
		 * Extracts object and image features by pairs of feature correspondences all correspondences are expected to be candidates.
		 * However, feature correspondences are extracted only if their projection error is below a given threshold.
		 * @param forwardFeatures Forward or image features building the correspondences
		 * @param backwardFeatures Backward or object features building the correspondences
		 * @param correspondencePairs Pairs of correspondence candidates defining a correspondence between forward and backward features
		 * @param pose Pose corresponding to the determined corresponding pairs
		 * @param pinholeCamera The pinhole camera used for tracking
		 * @param sqrDistance Maximal square distance of the projected object feature and the image feature
		 * @param forwardCorrespondences Resulting features from the forward correspondneces
		 * @param backwardCorrespondences Resulting feature from the backward correspondences
		 */
		static void extractCorrespondingFeatures(const CV::Detector::Blob::BlobFeatures& forwardFeatures, const CV::Detector::Blob::BlobFeatures& backwardFeatures, const CorrespondencePairs& correspondencePairs, const HomogenousMatrix4& pose, const PinholeCamera& pinholeCamera, const Scalar sqrDistance, CV::Detector::Blob::BlobFeatures& forwardCorrespondences, CV::Detector::Blob::BlobFeatures& backwardCorrespondences);

		/**
		 * Extracts object and image features by pairs of feature correspondence candidates.
		 * Some candidates are expected to be invalid, thus additional indices specify the valid candidates.
		 * @param forwardFeatures Forward or image features building the correspondences
		 * @param backwardFeatures Backward or object features building the correspondences
		 * @param candidatePairs Pairs of correspondence candidates defining a correspondence between forward and backward features
		 * @param correspondenceIndices Indices specifying the valid pairs of corresponding candidates
		 * @param forwardCorrespondences Resulting features from the forward correspondneces
		 * @param backwardCorrespondences Resulting feature from the backward correspondences
		 */
		static void extractCorrespondingFeatures(const CV::Detector::Blob::BlobFeatures& forwardFeatures, const CV::Detector::Blob::BlobFeatures& backwardFeatures, const CorrespondencePairs& candidatePairs, const CV::Detector::FeatureIndices& correspondenceIndices, CV::Detector::Blob::BlobFeatures& forwardCorrespondences, CV::Detector::Blob::BlobFeatures& backwardCorrespondences);

		/**
		 * Determines candidates for feature correspondences upon their spatial position in the frame.
		 * @param interestFeatures Blob features of interest to find candidates for
		 * @param candidatePool A pool of Blob features holding all possible candidates
		 * @param window Size of the search window for possible candidates
		 * @param multiCandidates Lists of feature candidates for each feature of interest
		 * @param candidates List of all feature candidates
		 */
		static void determineCandidates(const CV::Detector::Blob::BlobFeatures& interestFeatures, const CV::Detector::Blob::BlobFeatures& candidatePool, const Scalar window, MultiCandidates& multiCandidates, CV::Detector::FeatureIndices& candidates);

		/**
		 * Determines a correspondence candiate for a 2D feature inside the 3D feature map.
		 * @param interestFeature 2D interest fature to find a 3D candidate for
		 * @param featureMap Feature map with 3D features
		 * @param poseTranslation Translation of the almost accurate pose to be used to project the 2D feature into the 3D feature map
		 * @param poseOrientation Orientation of the almost accurate pose to be used to project the 2D feature into the 3D feature map
		 * @param pinholeCamera The pinhole camera object used for feature tracking
		 * @param maximalHalfAngle Maximal (half) angle between back projection ray of the 2D position and the real 3D position in radian
		 * @param maximalDescriptorThreshold Maximal threshold for the descriptor to count as valid feature correspondence
		 * @param correspondenceIndex Resulting index of the corresponding 3D feature point inside the feature map if succeeded
		 * @return True, if succeeded
		 */
		static bool determineCorresponding3DFeature(const CV::Detector::Blob::BlobFeature& interestFeature, const FeatureMap& featureMap, const Vector3& poseTranslation, const Quaternion& poseOrientation, const PinholeCamera& pinholeCamera, const Scalar maximalHalfAngle, const Scalar maximalDescriptorThreshold, unsigned int& correspondenceIndex);

	private:

		/**
		 * Determines features correspondences for two sets of features without any previous knowledge (applying a brute force search) using the minimal summed square distance (SSD).
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
		 * Determines features correspondences between sets of 2D and 3D features with a known rough camera pose.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param model_T_camera The rough camera pose defining the transformation between camera and model, must be valid
		 * @param cameraFeatures The 2D features located in the camera, must have valid 2D observation coordinates, must be valid
		 * @param modelFeatures The 3D features defined in the coordinate system of the mode, must have valid 3D position coordinates, must be valid
		 * @param numberCameraFeatures The number of given camera features, with range [4, infinity)
		 * @param numberModelFeatures The number of given model features, with range [4, infinity)
		 * @param threshold The maximal distance between two feature descriptors so that the matching still counts as valid correspondence, with range [0, infinity)
		 * @param maxSqrProjectionError The maximal square projection error between a projected 3D feature and a 2D feature so that both features can be corresponding, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return Resulting correspondences
		 * @tparam tEarlyReject True, to use SSD calculations with early reject; False, to calculate the full SSD
		 */
		template <bool tEarlyReject>
		static CorrespondencePairs determineFeatureCorrespondencesGuided(const AnyCamera& camera, const HomogenousMatrix4& model_T_camera, const CV::Detector::Blob::BlobFeature* cameraFeatures, const CV::Detector::Blob::BlobFeature* modelFeatures, const size_t numberCameraFeatures, const size_t numberModelFeatures, const Scalar threshold, const Scalar maxSqrProjectionError, Worker* worker = nullptr);

		/**
		 * Determines features correspondences for a subset of two sets of features without any previous knowledge (applying a brute force search) using the minimal summed square distance (SSD).
		 * @param forwardFeatures Forward features (e.g. of a live camera)
		 * @param backwardFeatures Backward features (e.g. of a feature map)
		 * @param backwardFeaturesSize Number of backward features, with range [1, infinity)
		 * @param threshold The maximal distance between two feature descriptors so that the matching still counts as valid correspondence, with range (0, infinity)
		 * @param backwardIndicesForForwardFeatures Indices of matching backward features, one for each forward feature, -1 if no match exists
		 * @param firstForward First forward feature to be handled, with range [0, forwardFeaturesSize)
		 * @param numberForwards Number of forward features to be handled, with range [1, forwardFeaturesSize - firstForward]
		 * @tparam tEarlyReject True, to apply a SSD determination with early reject strategy; False, to apply a full SSD determination
		 * @see determineFeatureCorrespondences().
		 */
		template <bool tEarlyReject>
		static void determineFeatureCorrespondencesSubset(const CV::Detector::Blob::BlobFeature* forwardFeatures, const CV::Detector::Blob::BlobFeature* backwardFeatures, const size_t backwardFeaturesSize, const Scalar threshold, Index32* backwardIndicesForForwardFeatures, const unsigned int firstForward, const unsigned int numberForwards);

		/**
		 * Determines features correspondences between for a subset of 2D and 3D features with a known rough camera pose.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param model_T_camera The rough camera pose defining the transformation between camera and model, must be valid
		 * @param cameraFeatures The 2D features located in the camera, must have valid 2D observation coordinates, must be valid
		 * @param modelFeatures The 3D features defined in the coordinate system of the mode, must have valid 3D position coordinates, must be valid
		 * @param numberModelFeatures The number of given model features, with range [4, infinity)
		 * @param threshold The maximal distance between two feature descriptors so that the matching still counts as valid correspondence, with range [0, infinity)
		 * @param maxSqrProjectionError The maximal square projection error between a projected 3D feature and a 2D feature so that both features can be corresponding, with range [0, infinity)
		 * @param modelIndicesForCameraFeatures The resulting indices of model features, one for each camera feature
		 * @param firstCameraFeature The first camera feature to be handled, with range [0, numberCameraFeatures - 1]
		 * @param numberCameraFeatures The number of camera features to be handled, with range [1, numberCameraFeatures - firstCameraFeature]
		 * @tparam tEarlyReject True, to use SSD calculations with early reject; False, to calculate the full SSD
		 */
		template <bool tEarlyReject>
		static void determineFeatureCorrespondencesGuidedSubset(const AnyCamera* camera, const HomogenousMatrix4* model_T_camera, const CV::Detector::Blob::BlobFeature* cameraFeatures, const CV::Detector::Blob::BlobFeature* modelFeatures, const size_t numberModelFeatures, const Scalar threshold, const Scalar maxSqrProjectionError, Index32* modelIndicesForCameraFeatures, const unsigned int firstCameraFeature, const unsigned int numberCameraFeatures);
};

inline Correspondences::CorrespondencePairs Correspondences::determineFeatureCorrespondencesGuided(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& model_T_camera, const CV::Detector::Blob::BlobFeature* cameraFeatures, const CV::Detector::Blob::BlobFeature* modelFeatures, const size_t numberCameraFeatures, const size_t numberModelFeatures, const Scalar threshold, const Scalar maxSqrProjectionError, const bool earlyReject, Worker* worker)
{
	return determineFeatureCorrespondencesGuided(AnyCameraPinhole(pinholeCamera), model_T_camera, cameraFeatures, modelFeatures, numberCameraFeatures, numberModelFeatures, threshold, maxSqrProjectionError, earlyReject, worker);
}

}

}

}

#endif // META_OCEAN_TRACKING_BLOB_CORRESPONDENCES_H
