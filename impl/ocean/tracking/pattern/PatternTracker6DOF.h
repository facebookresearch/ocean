/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_PATTERN_PATTERN_TRACKER_6DOF_H
#define META_OCEAN_TRACKING_PATTERN_PATTERN_TRACKER_6DOF_H

#include "ocean/tracking/pattern/Pattern.h"
#include "ocean/tracking/pattern/PatternTrackerCore6DOF.h"

#include "ocean/math/PinholeCamera.h"

namespace Ocean
{

namespace Tracking
{

namespace Pattern
{

/**
 * This class implements a 6DOF feature tracker for planar patterns.
 * The class is mainly a wrapper for the PatternTrackerCore6DOF class.
 * @see PatternTrackerCore6DOF.
 * @ingroup trackingpattern
 */
class OCEAN_TRACKING_PATTERN_EXPORT PatternTracker6DOF :
	virtual public VisualTracker,
	public PatternTrackerCore6DOF
{
	public:

		/**
		 * Creates a new feature tracker object.
		 */
		explicit PatternTracker6DOF(const Options& options = Options());

		/**
		 * Destructs a feature tracker object.
		 */
		~PatternTracker6DOF() override;

		/**
		 * Adds a new 2D tracking pattern (an image) to the tracker.
		 * The origin of the pattern will be located in the upper left corner of the given frame.<br>
		 * The pattern lies inside the x-z-plane with y-axis as up-vector.
		 * @param frame The frame specifying the tracking pattern, this frame will be converted internally if the pixel format is not FORMAT_Y8, must be valid
		 * @param dimension The dimension of the tracking pattern in the tracker coordinate system, with range (0, infinity)x[0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return The id of the tracking pattern, -1 if the pattern could not be added
		 */
		unsigned int addPattern(const Frame& frame, const Vector2& dimension, Worker* worker = nullptr);

		/**
		 * Adds a new 2D tracking pattern (an image) to the tracker.
		 * The origin of the pattern will be located in the upper left corner of the given frame.<br>
		 * The pattern lies inside the x-z-plane with y-axis as up-vector.<br>
		 * This function takes a file in which the pattern is defined, the file can be a simple image or a feature map storing an additional hierarchy of feature points.
		 * @param filename The filename of the file storing the pattern information, must be valid
		 * @param dimension The dimension of the tracking pattern in the tracker coordinate system, with range (0, infinity)x[0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return The id of the tracking pattern, -1 if the pattern could not be added
		 */
		unsigned int addPattern(const std::string& filename, const Vector2& dimension, Worker* worker = nullptr);

		/**
		 * Removes a pattern from this tracker.
		 * @param patternId The id of the pattern to be removed
		 * @return True, if succeeded
		 * @see removePatterns().
		 */
		bool removePattern(const unsigned int patternId);

		/**
		 * Removes all patterns from this tracker.
		 * @return True, if succeeded
		 * @see removePattern().
		 */
		bool removePatterns();

		/**
		 * Executes the 6DOF tracking for a given frame.
		 * @see VisualTracker::determinePoses().
		 */
		bool determinePoses(const Frame& frame, const PinholeCamera& pinholeCamera, const bool frameIsUndistorted, TransformationSamples& transformations, const Quaternion& world_R_camera = Quaternion(false), Worker* worker = nullptr) override;

	protected:

		/// Intermediate grayscale frame (used to avoid frame buffer re-allocations).
		Frame yFrame_;
};

}

}

}

#endif // META_OCEAN_TRACKING_PATTERN_PATTERN_TRACKER_6DOF_H
