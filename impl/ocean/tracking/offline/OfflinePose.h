/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_OFFLINE_OFFLINE_POSE_H
#define META_OCEAN_TRACKING_OFFLINE_OFFLINE_POSE_H

#include "ocean/tracking/offline/Offline.h"

#include "ocean/base/ShiftVector.h"

#include "ocean/math/HomogenousMatrix4.h"

namespace Ocean
{

namespace Tracking
{

namespace Offline
{

// Forward declaration.
class OfflinePose;

/**
 * Definition of a shift vector holding offline poses.
 * @ingroup trackingoffline
 */
typedef ShiftVector<OfflinePose> OfflinePoses;

/**
 * This class encapsulates the tracking pose data.
 * The pose is combined with a unique id and an abstract quality parameters.<br>
 * @ingroup trackingoffline
 */
class OfflinePose
{
	public:

		/**
		 * Creates a default pose object.
		 */
		OfflinePose() = default;

		/**
		 * Creates a new pose object.
		 * @param id The id of the pose
		 * @param transformation The transformation of this pose
		 * @param quality Abstract quality parameter of the pose
		 */
		inline OfflinePose(const unsigned int id, const HomogenousMatrix4& transformation, const Scalar quality = -1.0);

		/**
		 * Returns the transformation of this pose.
		 * @return Frame pose
		 */
		inline const HomogenousMatrix4& transformation() const;

		/**
		 * Returns the abstract quality parameter of this pose.
		 * This parameter might have individual meanings for individual tracker implementations.<br>
		 * @return Pose quality
		 */
		inline Scalar quality() const;

		/**
		 * Returns the id of this frame.
		 * @return Frame id
		 */
		inline unsigned int id() const;

		/**
		 * Sets or changes the transformation of this pose object.
		 * @param transformation The transformation to be set
		 */
		inline void setTransformation(const HomogenousMatrix4& transformation);

		/**
		 * Sets or changes the abstract quality parameters of this pose object.
		 * @param quality The quality to be set
		 */
		inline void setQuality(const Scalar quality);

		/**
		 * Sets or changes the id of this pose object.
		 * @param id Id to be set
		 */
		inline void setId(const unsigned int id);

		/**
		 * Returns whether this pose holds a valid transformation.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Returns whether this object holds a valid id and a valid pose.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

		/**
		 * Compares two objects and returns whether this object has a higher quality parameter than the second one.
		 * @param object Second pose object
		 * @return True, if so
		 */
		inline bool operator<(const OfflinePose& object) const;

		/**
		 * Extracts the transformations from a set of offline pose objects.
		 * @param offlinePoses Offline pose object from that the transformations will be extracted
		 */
		static inline ShiftVector<HomogenousMatrix4> offlinePoses2transformations(const OfflinePoses& offlinePoses);

	protected:

		/// Pose id.
		unsigned int id_ = (unsigned int)(-1);

		/// The transformation of this pose.
		HomogenousMatrix4 transformation_ = HomogenousMatrix4(false);

		/// Pose quality.
		Scalar quality_ = Scalar(-1);
};

inline OfflinePose::OfflinePose(const unsigned int id, const HomogenousMatrix4& transformation, const Scalar quality) :
	id_(id),
	transformation_(transformation),
	quality_(quality)
{
	// nothing to do here
}

inline const HomogenousMatrix4& OfflinePose::transformation() const
{
	return transformation_;
}

inline unsigned int OfflinePose::id() const
{
	return id_;
}

inline Scalar OfflinePose::quality() const
{
	return quality_;
}

inline void OfflinePose::setTransformation(const HomogenousMatrix4& transformation)
{
	transformation_ = transformation;
}

inline void OfflinePose::setQuality(const Scalar quality)
{
	quality_ = quality;
}

inline void OfflinePose::setId(const unsigned int id)
{
	id_ = id;
}

inline bool OfflinePose::isValid() const
{
	return transformation_.isValid();
}

inline OfflinePose::operator bool() const
{
	return id_ != (unsigned int)(-1) && transformation_.isValid();
}

inline bool OfflinePose::operator<(const OfflinePose& object) const
{
	return quality_ > object.quality_;
}

inline ShiftVector<HomogenousMatrix4> OfflinePose::offlinePoses2transformations(const OfflinePoses& offlinePoses)
{
	ShiftVector<HomogenousMatrix4> result(offlinePoses.firstIndex(), offlinePoses.size());

	for (ShiftVector<HomogenousMatrix4>::Index n = offlinePoses.firstIndex(); n < ShiftVector<HomogenousMatrix4>::Index(offlinePoses.firstIndex() + offlinePoses.size()); ++n)
	{
		result[n] = offlinePoses[n].transformation();
	}

	return result;
}

}

}

}

#endif // META_OCEAN_TRACKING_OFFLINE_OFFLINE_POSE_H
