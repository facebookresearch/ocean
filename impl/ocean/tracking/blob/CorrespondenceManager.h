// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_TRACKING_BLOB_CORRESPONDENCE_MANAGER_H
#define META_OCEAN_TRACKING_BLOB_CORRESPONDENCE_MANAGER_H

#include "ocean/tracking/blob/Blob.h"
#include "ocean/tracking/blob/Correspondences.h"

#include "ocean/base/Timestamp.h"

#include "ocean/cv/detector/blob/BlobFeature.h"

#include "ocean/geometry/Geometry.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"

namespace Ocean
{

namespace Tracking
{

namespace Blob
{

/**
 * This class implements a feature correspondence manager.
 * All function are not threadsafe.
 * @ingroup trackingblob
 */
class OCEAN_TRACKING_BLOB_EXPORT CorrespondenceManager : public Correspondences
{
	private:

		/**
		 * Definition of a container combining two corresponding features with the most recent detection timestamp.
		 */
		class CorrespondenceObject
		{
			public:

				/**
				 * Creates a new correspondence object.
				 * @param imageFeature Imgage feature of the correspondence
				 * @param objectFeature Object feature of the correspondence
				 * @param timestamp Correspondence timestamp
				 */
				inline CorrespondenceObject(const CV::Detector::Blob::BlobFeature& imageFeature, const CV::Detector::Blob::BlobFeature& objectFeature, const Timestamp timestamp);

				/**
				 * Returns the image feature.
				 * @return Image feature
				 */
				inline const CV::Detector::Blob::BlobFeature& imageFeature() const;

				/**
				 * Returns the object feature.
				 * @return Object feature
				 */
				inline const CV::Detector::Blob::BlobFeature& objectFeature() const;

				/**
				 * Returns the most recent timestamp of this correspondence.
				 * @return Correspondence timestamp
				 */
				inline const Timestamp timestamp() const;

				/**
				 * Updates the image feature.
				 * @param imageFeature New image feature updating the correspondence
				 * @param timestamp New correspondence timestamp
				 */
				inline void updateImageFeature(const CV::Detector::Blob::BlobFeature& imageFeature, const Timestamp timestamp);

				/**
				 * Updates the 2D position of the image feature.
				 * The new position must be undistorted.
				 * @param position New image feature position
				 */
				inline void updateImageFeaturePosition(const Vector2& position);

				/**
				 * Invalidates this correspondence object.
				 */
				inline void invalidate();

			private:

				/// Image feature.
				CV::Detector::Blob::BlobFeature correspondenceImageFeature;

				/// Object feature.
				CV::Detector::Blob::BlobFeature correspondenceObjectFeature;

				/// Timestamp.
				Timestamp correspondenceTimestamp;
		};

		/**
		 * Definition of a vector holding correspondence objects.
		 */
		typedef std::vector<CorrespondenceObject> CorrespondenceObjects;

	public:

		/**
		 * Returns the object and image positions of all stored feature correspondences.
		 * @param objectPoints Resulting object points
		 * @param imagePoints Resulting image points
		 */
		void points(Geometry::ObjectPoints& objectPoints, Geometry::ImagePoints& imagePoints);

		/**
		 * Returns the object and image positions for a subset of the stored feature correspondences.
		 * @param indices Correspondence indices specifying the subset, indices must not exceed the number of stored correspondences
		 * @param objectPoints Resulting object points
		 * @param imagePoints Resulting image points
		 */
		void points(const CV::Detector::FeatureIndices& indices, Geometry::ObjectPoints& objectPoints, Geometry::ImagePoints& imagePoints);

		/**
		 * Replaces previous correspondences by a new set of correspondences.
		 * The number of given object features must match the number of given image features.
		 * @param imageFeatures Image features building the correspondences, each feature must match to exactly one object feature
		 * @param objectFeatures Object features building the correspondences, each feature must match to exactly one image feature
		 * @param timestamp Timestamp of the new correspondences (should be the image timestamp)
		 */
		void setCorrespondences(const CV::Detector::Blob::BlobFeatures& imageFeatures, const CV::Detector::Blob::BlobFeatures& objectFeatures, const Timestamp timestamp);

		/**
		 * Adds a new correspondence, while the already stored correspondences are untouched.
		 * @param imageFeature Image feature of the new correspondence
		 * @param objectFeature Object feature of the new correspondence
		 * @param timestamp Timestamp of the new correspondences (should be the image timestamp)
		 * @return Returns the index of the new correspondence
		 */
		unsigned int addCorrespondence(const CV::Detector::Blob::BlobFeature& imageFeature, const CV::Detector::Blob::BlobFeature& objectFeature, const Timestamp timestamp);

		/**
		 * Replaces previous correspondences by a new set of correspondences.
		 * @param forwardFeatures Forward or image features building the correspondences
		 * @param backwardFeatures Backward or object features building the correspondences
		 * @param candidatePairs Pairs of correspondence candidates defining a correspondence between forward and backward features
		 * @param correspondenceIndices Indices specifying the valid pairs of corresponding candidates
		 * @param timestamp Timestamp of the new correspondences (should be the image timestamp)
		 */
		void setCorrespondences(const CV::Detector::Blob::BlobFeatures& forwardFeatures, const CV::Detector::Blob::BlobFeatures& backwardFeatures, const CorrespondencePairs& candidatePairs, const CV::Detector::FeatureIndices& correspondenceIndices, const Timestamp timestamp);

		/**
		 * Updates the image feature of a specified correspondence.
		 * The specific correspondence is defined by the index as stored in this manager.<br>
		 * Beware: The feature indices may changed after explicit execution of the garbage collector.
		 * @param imageFeature Image feature to be used for correspondence updating
		 * @param index Index of the correspondence to be updated
		 * @param timestamp New timestamp (should be the image timestamp)
		 */
		void updateCorrespondence(const CV::Detector::Blob::BlobFeature& imageFeature, const unsigned int index, const Timestamp timestamp);

		/**
		 * Invalidates a correspondence which will be removed during the next garbage collection call.
		 * @param index Index of the correspondence to be marked as invalid
		 */
		void invalidateCorrespondence(const unsigned int index);

		/**
		 * Removes correspondences of features if they have been unrecovered for a specified time.
		 * Fresh unrecovered correspondences (younger than the specified time) receive a chance for the next tracking iteration because their image position is updated w.r.t. the current pose.
		 * @param timestamp Timestmap of the current tracking iteration (should be the image timestamp)
		 * @param timeout Defines the maximal age an unrecovered correspondence may reach before it will be removed
		 * @param pinholeCamera The pinhole camera object used to update 2D position of unrecovered features
		 * @param pose Extrinsic camera matrix to update 2D position of unrecovered features
		 */
		void garbageCollector(const Timestamp timestamp, const double timeout, const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose);

		/**
		 * Returns the image features of this manager.
		 * @return Image features
		 */
		CV::Detector::Blob::BlobFeatures imageFeatures() const;

		/**
		 * Returns the object features of this manager.
		 * @return Object features
		 */
		CV::Detector::Blob::BlobFeatures objectFeatures() const;

		/**
		 * Returns the number of stored correspondences.
		 * @return Number of correspondence
		 */
		inline unsigned int correspondences() const;

		/**
		 * Returns the positions of specific object features of this manager.
		 * @param correspondences Returns the positions of all features stored in the first correspondence elements
		 * @return Object positions
		 */
		Geometry::ObjectPoints objectPositions(const CorrespondencePairs& correspondences) const;

		/**
		 * Clears all correspondences.
		 */
		inline void clear();

		/**
		 * Returns whether this manager does not hold any feature correspondences.
		 * @return True, if so
		 */
		inline bool isNull() const;

		/**
		 * Returns whether this manager holds at least one feature correspondence.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

	private:

		/// Recent correspondence objects from e.g. previous tracking iterations.
		CorrespondenceObjects managerCorrespondenceObjects;
};

inline CorrespondenceManager::CorrespondenceObject::CorrespondenceObject(const CV::Detector::Blob::BlobFeature& imageFeature, const CV::Detector::Blob::BlobFeature& objectFeature, const Timestamp timestamp) :
	correspondenceImageFeature(imageFeature),
	correspondenceObjectFeature(objectFeature),
	correspondenceTimestamp(timestamp)
{
	// nothing to do here
}

inline const CV::Detector::Blob::BlobFeature& CorrespondenceManager::CorrespondenceObject::imageFeature() const
{
	return correspondenceImageFeature;
}

inline const CV::Detector::Blob::BlobFeature& CorrespondenceManager::CorrespondenceObject::objectFeature() const
{
	return correspondenceObjectFeature;
}

inline const Timestamp CorrespondenceManager::CorrespondenceObject::timestamp() const
{
	return correspondenceTimestamp;
}

inline void CorrespondenceManager::CorrespondenceObject::updateImageFeature(const CV::Detector::Blob::BlobFeature& imageFeature, const Timestamp timestamp)
{
	correspondenceImageFeature = imageFeature;
	correspondenceTimestamp = timestamp;
}

inline void CorrespondenceManager::CorrespondenceObject::updateImageFeaturePosition(const Vector2& position)
{
	correspondenceImageFeature.setObservation(position, CV::Detector::Blob::BlobFeature::DS_UNDISTORTED);
}

inline void CorrespondenceManager::CorrespondenceObject::invalidate()
{
	correspondenceTimestamp = Numeric::minValue();
}

inline unsigned int  CorrespondenceManager::correspondences() const
{
	return (unsigned int)managerCorrespondenceObjects.size();
}

inline bool CorrespondenceManager::isNull() const
{
	return managerCorrespondenceObjects.empty();
}

inline CorrespondenceManager::operator bool() const
{
	return !managerCorrespondenceObjects.empty();
}

void CorrespondenceManager::clear()
{
	managerCorrespondenceObjects.clear();
}

}

}

}

#endif // META_OCEAN_TRACKING_BLOB_CORRESPONDENCE_MANAGER_H
