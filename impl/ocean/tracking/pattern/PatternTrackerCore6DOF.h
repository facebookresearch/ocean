/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_PATTERN_PATTERN_TRACKER_CORE_6DOF_H
#define META_OCEAN_TRACKING_PATTERN_PATTERN_TRACKER_CORE_6DOF_H

#include "ocean/tracking/pattern/Pattern.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Lock.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/FramePyramid.h"
#include "ocean/cv/SubRegion.h"

#include "ocean/cv/detector/FREAKDescriptor.h"

#include "ocean/geometry/SpatialDistribution.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/AnyCamera.h"

#include "ocean/tracking/VisualTracker.h"

namespace Ocean
{

namespace Tracking
{

namespace Pattern
{

/**
 * This class implements the core of the 6DOF feature tracker for planar patterns.
 * This 'core' class is separated from the general PatternTracker6DOF class to avoid any virtual functions.<br>
 * Virtual functions may increase the binary size as the compiler/linker may not be able to identify unused virtual functions as 'dead', and thus the linker will not be able to strip such functions.<br>
 * However, if the PatternTracker6DOF's object oriented capability is not needed anyway the usage of this core class is recommended (especially if the binary size matters).
 * @see PatternTracker6DOF.
 * @ingroup trackingpattern
 */
class OCEAN_TRACKING_PATTERN_EXPORT PatternTrackerCore6DOF
{
	public:

		/**
		 * Set of configurable parameters for the tracker.
		 */
		class OCEAN_TRACKING_PATTERN_EXPORT Options
		{
			public:

				/**
				 * Creates a new options object.
				 */
				Options();

			public:

				/// The maximal number of patterns that can be visible concurrently, with range [1, infinity). 0 to allow as many as possible
				unsigned int maxConcurrentlyVisiblePattern_ = 1u;

				/// The maximal time used for pattern recognition for each frame in seconds, with range (0, infinity). 0 to use a default value
				double maxRecognitionTime_ = 0.0;

				/// Time in seconds to wait between recognition attempts when at least one pattern is currently being tracked.
				double recognitionCadenceWithTrackedPatterns_ = 0.5;

				/// Time in seconds to wait between recognition attempts when no patterns are currently being tracked.
				double recognitionCadenceWithoutTrackedPatterns_ = 0.0;

				/// The number of iterations to run RANSAC when attempting to verify a newly recognized target.
				unsigned int recognitionRansacIterations_ = 50u;

				/// True, to skip frame-to-frame tracking and to apply a full re-detection for every frame.
				bool noFrameToFrameTracking_ = false;

#ifdef OCEAN_PLATFORM_BUILD_ANDROID

				/// True, to apply a downsampling on Android devices to improve performance on low end devices.
				bool downsampleInputImageOnAndroid_ = true;
#endif
		};

	protected:

		/**
		 * Definition of the descriptor to be used.
		 */
		using Descriptor = CV::Detector::FREAKDescriptor32;

		/**
		 * Definition of the descriptors to be used.
		 */
		using Descriptors = CV::Detector::FREAKDescriptors32;

		/// The maximal distance between two descriptors to be considered as similar.
		static constexpr unsigned int maximalDescriptorDistance_ = (unsigned int)(Descriptor::size() * 8) * 25u / 100u; // 25% of the descriptor bits

		/**
		 * Definition of a lightweight 3D feature map holding 3D object points and descriptors for all features in the map.
		 */
		class FeatureMap
		{
			public:

				/**
				 * Default constructor.
				 */
				FeatureMap() = default;

				/**
				 * Creates a new feature map for a planar 3D object (an image placed in the x-z plane).
				 * @param yFrame The image for which the feature map will be created, with pixel format FORMAT_Y8, must be valid
				 * @param width The width of the image in pixel, with range [1, infinity)
				 * @param height The height of the image in pixel, with range [1, infinity)
				 * @param yFramePaddingElements The number of padding elements at the end of each image row, in elements, with range [0, infinity)
				 * @param dimension The dimension of the feature map (of the image), with range (0, infinity)x[0, infinity), if the y-value is 0 the image's aspect ratio is used to determine the size of the y-dimension
				 * @param worker Optional worker object to distribute the computation
				 */
				FeatureMap(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const Vector2& dimension, Worker* worker = nullptr);

				/**
				 * Returns the 3D object points of all map features.
				 * @return The map's object points
				 */
				inline const Vectors3& objectPoints() const;

				/**
				 * Returns the descriptor associated with the 3D object points of this map.
				 * @return The map's feature descriptors, one for each 3D object point
				 */
				inline const Descriptors& descriptors() const;

			protected:

				/// The 3D locations of all features in this map.
				Vectors3 objectPoints_;

				/// The descriptors of all features, one for each 3D object point location.
				Descriptors descriptors_;
		};

		/**
		 * This class stores the information necessary for one tracking pattern.
		 */
		class Pattern
		{
			protected:

				/**
				 * Definition of a vector holding 2D feature positions.
				 */
				typedef std::vector<Vectors2> PointLayers;

			public:

				/**
				 * Creates a new invalid pattern object.
				 */
				Pattern() = default;

				/**
				 * Creates a new pattern object by a given frame and pattern dimension.
				 * @param yFrame The 8 bit grayscale frame (with Y8 pixel format, and pixel origin in the upper left corner) specifying the tracking pattern, must be valid
				 * @param width The width of the given grayscale frame in pixel, with range [1, infinity)
				 * @param height The height of the given grayscale frame in pixel, with range [1, infinity)
				 * @param yFramePaddingElements The number of padding elements at the end of each row, in elements, with range [0, infinity)
				 * @param dimension The dimension of the tracking pattern, with range (0, infinity)x(0, infinity)
				 * @param worker Optional worker object to distribute the computation
				 */
				Pattern(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const Vector2& dimension, Worker* worker = nullptr);

				/**
				 * Returns the feature map of this pattern.
				 * @return The pattern's feature map
				 */
				inline const FeatureMap& featureMap() const;

				/**
				 * Returns the frame pyramid of the image defining the tracking pattern.
				 * @return Pattern frame pyramid
				 */
				inline const CV::FramePyramid& pyramid() const;

				/**
				 * Returns the dimension of the tracking object defined in the tracker coordinate system.
				 * @return Pattern dimension
				 */
				inline const Vector2& dimension() const;

				/**
				 * Returns the first 3D corner position of the tracking pattern in the tracker coordinate system. Only to be used for planar patterns.
				 * @return The upper left corner of the tracking pattern (for a planar pattern, identical with the origin of the world coordinate system).
				 */
				inline Vector3 corner0() const;

				/**
				 * Returns the second 3D corner position of the tracking pattern in the tracker coordinate system. Only to be used for planar patterns.
				 * @return The lower left corner of the tracking pattern
				 */
				inline Vector3 corner1() const;

				/**
				 * Returns the third 3D corner position of the tracking pattern in the tracker coordinate system. Only to be used for planar patterns.
				 * @return The lower right corner of the tracking pattern
				 */
				inline Vector3 corner2() const;

				/**
				 * Returns the fourth 3D corner position of the tracking pattern in the tracker coordinate system. Only to be used for planar patterns.
				 * @return The upper right corner of the tracking pattern
				 */
				inline Vector3 corner3() const;

				/**
				 * Returns the two 3D triangles covering the tracking area of this tracking pattern.
				 * @return Two 3D triangles
				 */
				inline Triangles3 triangles3() const;

				/**
				 * Returns the two projected 2D triangles specifying the tracking pattern in the camera frame as seen with the previous pose (which is stored in this object).
				 * Beware: The previous pose must be valid.<br>
				 * @param pinholeCamera The pinhole camera profile defining the projection, must be valid
				 * @return The two projected 2D triangles
				 */
				inline Triangles2 triangles2(const PinholeCamera& pinholeCamera) const;

				/**
				 * Returns the two projected 2D triangles specifying the tracking pattern in the camera frame as seen with a given pose.
				 * @param pinholeCamera The pinhole camera profile defining the projection, must be valid
				 * @param pose The pose defining the camera position and orientation, must be valid
				 * @return The two projected 2D triangles
				 */
				inline Triangles2 triangles2(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose) const;

				/**
				 * Returns the previous camera pose from which this tracking pattern has been seen.
				 * @return Previous camera pose, if any
				 */
				inline const HomogenousMatrix4& previousPose() const;

				/**
				 * Returns the previous camera pose from which this tracking pattern has been seen.
				 * @return Previous camera pose, if any
				 */
				inline HomogenousMatrix4& previousPose();

				/**
				 * Returns the 3D object points of this pattern which have been used in the previous (or current) tracking iteration.
				 * @return 3D object points lying in the pattern plane, each point corresponds with one image point
				 * @see imagePoints().
				 */
				inline const Vectors3& objectPoints() const;

				/**
				 * Returns the 3D object points of this pattern which have been used in the previous (or current) tracking iteration.
				 * @return 3D object points lying in the pattern plane, each point corresponds with one image point
				 * @see imagePoints().
				 */
				inline Vectors3& objectPoints();

				/**
				 * Returns the 2D image points of this pattern which have been used in the previous (or current) tracking iteration.
				 * @return 2D image points, each point corresponds with one object point
				 * @see objectPoints().
				 */
				inline const Vectors2& imagePoints() const;

				/**
				 * Returns the 2D image points of this pattern which have been used in the previous (or current) tracking iteration.
				 * @return 2D image points, each point corresponds with one object point
				 * @see objectPoints().
				 */
				inline Vectors2& imagePoints();

				/**
				 * Returns the 2D feature points from the pyramid frame of the pattern image.
				 * @param layer The index of the pyramid layer, with range [0, layer())
				 * @return The 2D feature points of the specified layer
				 * @see layer().
				 */
				inline const Vectors2& referencePoints(const unsigned int layer) const;

				/**
				 * Returns the number of pyramid layers of the pattern image.
				 * @return The number of layers
				 * @see referencePoints().
				 */
				inline unsigned int layers() const;

				/**
				 * Returns whether this pattern holds a valid/useful rough guess of the camera pose.
				 * @param poseGuess The resulting rough pose guess, if existing
				 * @param maximalAge The maximal age of the rough pose guess in seconds, with range [0, 2]
				 * @return True, if so
				 */
				inline bool hasPoseGuess(HomogenousMatrix4& poseGuess, const double maximalAge = 0.5);

				/**
				 * Returns a guess of the current camera pose for this pattern.
				 * @param timestamp Optional timestamp of the pose guess
				 * @return pose The camera pose guess, if any
				 */
				inline const HomogenousMatrix4& poseGuess(Timestamp* timestamp = nullptr);

				/**
				 * Sets a guess of the current (or next) camera pose for this pattern.
				 * @param pose guess The rough pose guess
				 * @param timestamp The timestamp of this guess
				 */
				inline void setPoseGuess(const HomogenousMatrix4& pose, const Timestamp& timestamp);

				/**
				 * Returns whether this tracking pattern object is valid.
				 * @return True, if so
				 */
				inline bool isValid() const;

				/**
				 * Resets the internal recognition states of this pattern while the actual feature map is untouched.
				 */
				void reset();

				/**
				 * Returns whether this tracking pattern object is valid.
				 * @return True, if so
				 */
				explicit inline operator bool() const;

			protected:

				/// The feature map of this pattern.
				FeatureMap featureMap_;

				/// The frame pyramid of the image specifying the pattern.
				CV::FramePyramid patternPyramid_;

				/// The dimension of the tracking pattern defined in the tracker coordinate system as (x-axis, z-axis).
				Vector2 dimension_ = Vector2(0, 0);

				/// The previous camera pose for this tacking pattern, if any
				HomogenousMatrix4 world_T_previousCamera_ = HomogenousMatrix4(false);

				/// The 3D object points which have been used in the previous (or current) tracking iteration.
				Vectors3 objectPoints_;

				/// The 2D image points which have been used in the previous (or current) tracking iteration.
				Vectors2 imagePoints_;

				/// The point pyramid of the pattern image storing reference feature points for individual pattern resolutions.
				PointLayers pyramidReferencePoints_;

				/// A rough guess of the camera pose for this pattern, if any.
				HomogenousMatrix4 world_T_guessCamera_ = HomogenousMatrix4(false);

				/// The timestamp of the rough camera pose.
				Timestamp poseGuessTimestamp_;
		};

		/**
		 * Definition of a map holding pattern objects.
		 */
		typedef std::map<unsigned int, Pattern> PatternMap;

	public:

		/**
		 * Creates a new feature tracker object.
		 * @param options Set of parameters for the tracker
		 */
		explicit PatternTrackerCore6DOF(const Options& options = Options());

		/**
		 * Destructs a feature tracker object.
		 */
		~PatternTrackerCore6DOF();

		/**
		 * Adds a new 2D tracking pattern (an image) to the tracker.
		 * The origin of the pattern will be located in the upper left corner of the given frame.<br>
		 * The pattern lies inside the x-z-plane with y-axis as up-vector.
		 * @param yFrame The 8 bit grayscale frame (with Y8 pixel format, and pixel origin in the upper left corner) specifying the tracking pattern, must be valid
		 * @param width The width of the given grayscale frame in pixel, with range [1, infinity)
		 * @param height The height of the given grayscale frame in pixel, with range [1, infinity)
		 * @param yFramePaddingElements The number of padding elements at the end of each row, in elements, with range [0, infinity)
		 * @param dimension The dimension of the tracking pattern in the tracker coordinate system, with range (0, infinity)x(-infinity, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return The id of the tracking pattern, -1 if the pattern could not be added
		 * @see removePattern(), numberPattern(), setMaxConcurrentlyVisiblePattern().
		 */
		unsigned int addPattern(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const Vector2& dimension, Worker* worker = nullptr);

		/**
		 * Adds a new 2D tracking pattern (an image) to the tracker.
		 * The origin of the pattern will be located in the upper left corner of the given frame.<br>
		 * The pattern lies inside the x-z-plane with y-axis as up-vector.<br>
		 * This function takes a file in which the pattern is defined, the file can be a simple image or a feature map storing an additional hierarchy of feature points.
		 * @param filename The filename of the file storing the pattern information, must be valid
		 * @param dimension The dimension of the tracking pattern in the tracker coordinate system, with range (0, infinity)x(-infinity, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return The id of the tracking pattern, -1 if the pattern could not be added
		 * @see removePattern(), numberPattern(), setMaxConcurrentlyVisiblePattern().
		 */
		unsigned int addPattern(const std::string& filename, const Vector2& dimension, Worker* worker = nullptr);

		/**
		 * Removes a pattern from this tracker.
		 * @param patternId The id of the pattern to be removed
		 * @return True, if the defined pattern could be removed; False, if e.g., the pattern id is invalid
		 * @see removePatterns().
		 */
		bool removePattern(const unsigned int patternId);

		/**
		 * Removes all patterns from this tracker.
		 * @return True, if all existing patterns could be removed
		 * @see removePattern().
		 */
		bool removePatterns();

		/**
		 * Executes the 6DOF tracking for a given frame.
		 * Beware: The frame type of the input image must not change between successive calls, reset the tracker in case the image resolution changes.
		 * @param yFrame The 8 bit grayscale frame (with Y8 pixel format, and pixel origin in the upper left corner) to be used for tracking (the frame's width and hight will be extracted from the camera profile), must be valid
		 * @param pinholeCamera The pinhole camera object defining the project, with same dimension as the given frame, must be valid
		 * @param yFramePaddingElements The number of padding elements at the end of each row, in elements, with range [0, infinity)
		 * @param frameIsUndistorted True, if the original input frame is undistorted and thus feature must not be undistorted explicitly
		 * @param timestamp The timestamp of the given frame, must be valid
		 * @param transformations Resulting 6DOF poses combined with the tracking ids
		 * @param world_R_camera Optional absolute orientation of the camera in the moment the frame was taken, defined in a coordinate system not related with the tracking objects, an invalid object otherwise
		 * @param worker Optional worker object
		 * @return True, if succeeded
		 */
		bool determinePoses(const uint8_t* yFrame, const PinholeCamera& pinholeCamera, const unsigned int yFramePaddingElements, const bool frameIsUndistorted, const Timestamp& timestamp, VisualTracker::TransformationSamples& transformations, const Quaternion& world_R_camera = Quaternion(false), Worker* worker = nullptr);

		/**
		 * Returns the number of registered/added pattern.
		 * @return The number of existing pattern, with range [0, infinity)
		 * @see addPattern(), setMaxConcurrentlyVisiblePattern().
		 */
		inline unsigned int numberPattern() const;

		/**
		 * Returns the maximal number of pattern that can be tracked concurrently within one frame.
		 * @return The number of pattern that can be tracked concurrently, with range [0, numberPattern()], 0 to track as much as possible, 1 by default
		 * @see setMaxConcurrentlyVisiblePattern(), numberPattern().
		 */
		inline unsigned int maxConcurrentlyVisiblePattern() const;

		/**
		 * Sets the maximal number of pattern that can be tracked concurrently within one frame.
		 * @param maxConcurrentlyVisiblePattern The number of pattern that can be tracked concurrently, with range [0, infinity), 0 to track as much as possible
		 * @see maxConcurrentlyVisiblePattern(), numberPattern().
		 */
		inline void setMaxConcurrentlyVisiblePattern(const unsigned int maxConcurrentlyVisiblePattern);

		/**
		 * Returns the latest 2D/3D correspondences for a pattern which has been used to determine the camera pose.
		 * This function is mainly intended for debugging and visualization purposes.
		 * @param patternId The id of the pattern for which the latest feature correspondences will be returned
		 * @param imagePoints The resulting 2D image points, empty if no pose is known
		 * @param objectPoints The resulting 3D object points, one for each 2D image point
		 * @param pattern_T_camera Optional resulting camera pose associated with the pattern and feature correspondences, nullptr if not of interest
		 * @return True, if succeeded
		 */
		bool recentFeatureCorrespondences(const unsigned int patternId, Vectors2& imagePoints, Vectors3& objectPoints, HomogenousMatrix4* pattern_T_camera = nullptr) const;

		/**
		 * Resets the tracker's states but keeps all registered pattern.
		 * This function should be used e.g., whenever the resolution of the input image changes.<br>
		 * The tracker is simply reset to a state before the first call of determinePoses().
		 */
		void reset();

		/**
		 * Converts a known camera pose determined by this pattern tracker to a corresponding camera pose based on a difference camera profile.
		 * Beware: The conversion is an approximation only and does not reflect a mathematic perfect solution.
		 * @param newCamera The new camera profile for which the new pose will be calculated, must be valid
		 * @param referenceCamera The camera profile for which the known (reference) pose has been determined by this pattern tracker, must be valid
		 * @param referencePose The known (reference) pose which will be converted based on the new camera profile, must be valid
		 * @param newPose The resulting new pose matching with the new camera profile
		 * @return True, if succeeded
		 */
		static bool convertPoseForCamera(const PinholeCamera& newCamera, const PinholeCamera& referenceCamera, const HomogenousMatrix4& referencePose, HomogenousMatrix4& newPose);

	protected:

		/**
		 * Computes the maximum allowed time between recognition attempts, which may depend on whether or not any targets are currently being tracked.
		 * @return Maximum allowed duration in seconds, with range [0, infinity)
		 */
		inline double maximumDurationBetweenRecognitionAttempts() const;

		/**
		 * Determines the 6DOF tracking for a given frame.
		 * @param allowRecognition If false, skip feature extraction and matching for this frame
		 * @param yFrame The 8 bit grayscale frame (with Y8 pixel format) to be used for tracking, must be valid
		 * @param pinholeCamera The pinhole camera object defining the projection, with same dimension as the given frame
		 * @param previousCamera_R_camera Optional relative orientation between the previous frame and the current frame, if known
		 * @param worker Optional worker object
		 * @return True, if succeeded
		 */
		bool determinePoses(const bool allowRecognition, const Frame& yFrame, const PinholeCamera& pinholeCamera, const Quaternion& previousCamera_R_camera = Quaternion(false), Worker* worker = nullptr);

		/**
		 * Determines the 6DOF tracking for a given frame which has been downsampled.
		 * @param allowRecognition If false, skip feature extraction and matching for this frame
		 * @param yFrame The 8 bit grayscale frame (with Y8 pixel format) to be used for tracking, must be valid
		 * @param pinholeCamera The pinhole camera object defining the projection, with same dimension as the given frame
		 * @param previousCamera_R_camera Optional relative orientation between the previous frame and the current frame, if known
		 * @param worker Optional worker object
		 * @return True, if succeeded
		 */
		bool determinePosesWithDownsampledResolution(const bool allowRecognition, const Frame& yFrame, const PinholeCamera& pinholeCamera, const Quaternion& previousCamera_R_camera = Quaternion(false), Worker* worker = nullptr);

		/**
		 * Determines the 6DOF poses for the registered patterns without any a-priori information.
		 * @param pinholeCamera The pinhole camera object associated with the frame
		 * @param yFrame The current camera frame with grayscale pixel format (Y8), must be valid
		 * @param currentFramePyramid The frame pyramid of the current frame
		 * @param previousCamera_R_camera Optional relative orientation between the previous frame and the current frame, if known
		 * @param worker Optional worker object
		 * @return True, if succeeded
		 */
		bool determinePosesWithoutKnowledge(const PinholeCamera& pinholeCamera, const Frame& yFrame, const CV::FramePyramid& currentFramePyramid, const Quaternion& previousCamera_R_camera = Quaternion(false), Worker* worker = nullptr);

		/**
		 * Counts the number of currently visible pattern.
		 * @return The number of visible pattern
		 */
		inline unsigned int internalNumberVisiblePattern() const;

		/**
		 * Returns the maximal number of pattern that can be tracked concurrently within one frame.
		 * This function is an internal helper function providing the number of pattern that are allowed to be visible concurrently.
		 * @return The number of pattern that can be tracked concurrently, with range [1, numberPattern()]
		 */
		inline unsigned int internalMaxConcurrentlyVisiblePattern() const;

		/**
		 * Determine the camera pose for the current camera frame and for a given pattern by application of 2D/3D feature points correspondences determined for the previous camera frame.
		 * @param pinholeCamera The pinhole camera profile to be used, must be valid
		 * @param previousFramePyramid The frame pyramid of the previous frame, must be valid
		 * @param currentFramePyramid The frame pyramid of the current camera frame, with same frame type and number of layers as 'previousFramePyramid', must be valid
		 * @param pattern The tracking pattern object which is needs to be re-found (re-tracked) in the current camera frame
		 * @param previousCamera_R_camera Optional relative orientation between the previous frame and the current frame, if known
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if the pattern could be tracked reliably
		 */
		static bool determinePoseWithPreviousCorrespondences(const PinholeCamera& pinholeCamera, const CV::FramePyramid& previousFramePyramid, const CV::FramePyramid& currentFramePyramid, Pattern& pattern, const Quaternion& previousCamera_R_camera = Quaternion(false), Worker* worker = nullptr);

		/**
		 * Determines the camera pose for extreme camera motion for a given pattern for which a pose guess is known.
		 * The resulting pose will not be free of drift errors as the pose is determine between successive camera frames only.
		 * However, the resulting pose will be better than no pose (in most situations).
		 * @param pinholeCamera The pinhole camera profile to be used, must be valid
		 * @param previousFramePyramid The frame pyramid of the previous frame, must be valid
		 * @param currentFramePyramid The frame pyramid of the current camera frame, with same frame type and number of layers as 'previousFramePyramid', must be valid
		 * @param pattern The tracking pattern object which is needs to be re-found (re-tracked) in the current camera frame
		 * @param previousCamera_R_camera Optional relative orientation between the previous frame and the current frame, if known
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if the pattern could be tracked with a drift-error-based pose
		 */
		static bool determinePoseWithDriftErrors(const PinholeCamera& pinholeCamera, const CV::FramePyramid& previousFramePyramid, const CV::FramePyramid& currentFramePyramid, Pattern& pattern, const Quaternion& previousCamera_R_camera = Quaternion(false), Worker* worker = nullptr);

		/**
		 * Tracks a set of 2D/3D points correspondences from the previous frame to the current frame (in a specified pyramid layer).
		 * @param pinholeCamera The pinhole camera profile to be used
		 * @param previousFramePyramid The frame pyramid of the previous frame
		 * @param currentFramePyramid The frame pyramid of the current frame
		 * @param trackingLayer The pyramid layer which is used for tracking
		 * @param previousPose The camera pose for the previous frame
		 * @param previousObjectPoints The 3D object points which have been used to determine the previous pose
		 * @param previousImagePoints The 2D image points which have been used to determine the previous pose, each points has a corresponding 3D object points, the points are defined in the finest pyramid layer
		 * @param roughPose Resulting pose determined for tracked points in the specified pyramid layer 'trackingLayer'
		 * @param worker Optional worker object to distribute the computation
		 * @param numberFeatures The number of feature points which will be used for tracking, with range [3, infinity)
		 * @param maxError The maximal square error between bidirectional tracked feature points
		 * @return True, if succeeded
		 */
		static bool trackFrame2FrameHierarchy(const PinholeCamera& pinholeCamera, const CV::FramePyramid& previousFramePyramid, const CV::FramePyramid& currentFramePyramid, const unsigned int trackingLayer, const HomogenousMatrix4& previousPose, const Vectors3& previousObjectPoints, const Vectors2& previousImagePoints, HomogenousMatrix4& roughPose, Worker* worker = nullptr, const unsigned int numberFeatures = 20u, const Scalar maxError = Scalar(0.9 * 0.9));

		/**
		 * Tracks a set of 2D/3D points correspondences from the previous frame to the current frame.
		 * @param pinholeCamera The pinhole camera profile to be used
		 * @param previousFramePyramid The frame pyramid of the previous frame
		 * @param currentFramePyramid The frame pyramid of the current frame
		 * @param previousPose The camera pose for the previous frame
		 * @param previousObjectPoints The 3D object points which have been used to determine the previous pose, and resulting 3D object points which could be tracked
		 * @param previousImagePoints The 2D image points which have been used to determine the previous pose, each points has a corresponding 3D object points, and resulting 2D image points which could be tracked
		 * @param currentImagePoints The resulting 2D image points which have been tracked in the current frame
		 * @param currentPose The resulting current camera pose
		 * @param roughCurrentPose Optional rough current camera pose which can be used to improve the tracking performance
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool trackFrame2Frame(const PinholeCamera& pinholeCamera, const CV::FramePyramid& previousFramePyramid, const CV::FramePyramid& currentFramePyramid, const HomogenousMatrix4& previousPose, Vectors3& previousObjectPoints, Vectors2& previousImagePoints, Vectors2& currentImagePoints, HomogenousMatrix4& currentPose, const HomogenousMatrix4& roughCurrentPose = HomogenousMatrix4(false), Worker* worker = nullptr);

		/**
		 * Optimizes a given camera pose by rectification of the current camera frame so that it matches with the original tracking pattern.
		 * @param pinholeCamera The pinhole camera profile to be used
		 * @param currentFramePyramid The frame pyramid of the current camera frame, with pixel format FORMAT_Y8
		 * @param roughPose The rough camera pose which will be optimized
		 * @param pattern The tracking pattern object which is needs to be re-found (re-tracked) in the current camera frame
		 * @param optimizedPose Resulting optimized camera pose
		 * @param worker Optional worker object to distribute the computation
		 * @param occlusionArray Optional resulting occlusion array specifying which parts of the rectified camera frame matched with the tracking pattern and which parts are expected to be occluded by different objects
		 * @return True, if the given rough pose could be optimized
		 */
		static bool optimizePoseByRectification(const PinholeCamera& pinholeCamera, const CV::FramePyramid& currentFramePyramid, const HomogenousMatrix4& roughPose, const Pattern& pattern, HomogenousMatrix4& optimizedPose, Worker* worker = nullptr, Geometry::SpatialDistribution::OccupancyArray* occlusionArray = nullptr);

		/**
		 * Returns a sub region based on a set of given 2D triangles.
		 * Due to numerical instability, triangles may be invalid in extreme tracking situations (especially if 32 bit floating point values are used).<br>
		 * Thus, this function uses a backup sub region which is used whenever one triangle is invalid.<br>
		 * The backup sub region is defined by width and height, with origin at (0, 0).
		 * @param triangles The triangles defining the sub region, at least one triangle
		 * @param backupWidth The width of the backup sub region, with range (0, infinity)
		 * @param backupHeight The height of the backup sub region, with range (0, infinity)
		 * @return The resulting sub region
		 */
		static CV::SubRegion triangles2subRegion(const Triangles2& triangles, const unsigned int backupWidth, const unsigned int backupHeight);

		/**
		 * Determines and describes feature points in an image.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param yFrame The frame in which the feature points will be detected, with pixel format FORMAT_Y8, must be valid
		 * @param imagePoints The resulting 2D image points located in the image
		 * @param imagePointDescriptors The resulting descriptors, one for each image point
		 * @param harrisCornerThreshold The minimal strength value of a Harris corner to be used as feature point, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 */
		static bool detectAndDescribeFeatures(const SharedAnyCamera& camera, const Frame& yFrame, Vectors2& imagePoints, Descriptors& imagePointDescriptors, const unsigned int harrisCornerThreshold = 20u, Worker* worker = nullptr);

		/**
		 * Simple helper function to determine the distance between two feature descriptors.
		 * @param descriptorA The first descriptor
		 * @param descriptorB The second descriptor
		 * @return The distance between both descriptors
		 */
		static OCEAN_FORCE_INLINE unsigned int determineDescriptorDistance(const Descriptor& descriptorA, const Descriptor& descriptorB);

	protected:

		/// Set of options for this tracker.
		Options options_;

		/// Frame pyramid of the current tracking frame.
		CV::FramePyramid currentFramePyramid_;

		/// Frame pyramid of the previous tracking frame.
		CV::FramePyramid previousFramePyramid_;

		/// The map holding all registered pattern object.
		PatternMap patternMap_;

		/// Optional absolute orientation for the previous camera frame (as provided from outside this tracker, e.g., via an IMU sensor).
		Quaternion world_R_previousCamera_ = Quaternion(false);

		/// Random generator object.
		RandomGenerator randomGenerator_;

		/// A counter providing unique pattern ids.
		unsigned int patternMapIdCounter_ = 0u;

		/// Tracker lock object.
		mutable Lock lock_;

		/// The timestamp of the previous frame.
		Timestamp timestampPreviousFrame_;

		/// The last timestamp at which we started an attempt to recognize a new pattern (this timestamp is prior to feature extraction).
		Timestamp lastRecognitionAttemptTimestamp_;

		/// The id of the pattern that has been tried to recognized last.
		unsigned int lastRecognitionPatternId_ = 0u;
};

inline const Vectors3& PatternTrackerCore6DOF::FeatureMap::objectPoints() const
{
	return objectPoints_;
}

inline const PatternTrackerCore6DOF::Descriptors& PatternTrackerCore6DOF::FeatureMap::descriptors() const
{
	return descriptors_;
}

inline const PatternTrackerCore6DOF::FeatureMap& PatternTrackerCore6DOF::Pattern::featureMap() const
{
	return featureMap_;
}

inline const CV::FramePyramid& PatternTrackerCore6DOF::Pattern::pyramid() const
{
	return patternPyramid_;
}

inline const Vector2& PatternTrackerCore6DOF::Pattern::dimension() const
{
	return dimension_;
}

inline Vector3 PatternTrackerCore6DOF::Pattern::corner0() const
{
	ocean_assert(isValid());
	return Vector3(0, 0, 0);
}

inline Vector3 PatternTrackerCore6DOF::Pattern::corner1() const
{
	ocean_assert(isValid());
	return Vector3(0, 0, dimension_.y());
}

inline Vector3 PatternTrackerCore6DOF::Pattern::corner2() const
{
	ocean_assert(isValid());
	return Vector3(dimension_.x(), 0, dimension_.y());
}

inline Vector3 PatternTrackerCore6DOF::Pattern::corner3() const
{
	ocean_assert(isValid());
	return Vector3(dimension_.x(), 0, 0);
}

inline Triangles3 PatternTrackerCore6DOF::Pattern::triangles3() const
{
	Triangles3 result(2);
	result[0] = Triangle3(corner0(), corner1(), corner2());
	result[1] = Triangle3(corner0(), corner2(), corner3());

	return result;
}

inline Triangles2 PatternTrackerCore6DOF::Pattern::triangles2(const PinholeCamera& pinholeCamera) const
{
	ocean_assert(world_T_previousCamera_.isValid());

	return triangles2(pinholeCamera, world_T_previousCamera_);
}

inline Triangles2 PatternTrackerCore6DOF::Pattern::triangles2(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose) const
{
	ocean_assert(pose.isValid());

	Triangles2 triangles;

	triangles =
	{
		pinholeCamera.projectToImage<true>(pose, Triangle3(corner0(), corner1(), corner2()), pinholeCamera.hasDistortionParameters()),
		pinholeCamera.projectToImage<true>(pose, Triangle3(corner0(), corner2(), corner3()), pinholeCamera.hasDistortionParameters())
	};

	return triangles;
}

inline const HomogenousMatrix4& PatternTrackerCore6DOF::Pattern::previousPose() const
{
	return world_T_previousCamera_;
}

inline HomogenousMatrix4& PatternTrackerCore6DOF::Pattern::previousPose()
{
	return world_T_previousCamera_;
}

inline const Vectors3& PatternTrackerCore6DOF::Pattern::objectPoints() const
{
	return objectPoints_;
}

inline Vectors3& PatternTrackerCore6DOF::Pattern::objectPoints()
{
	return objectPoints_;
}

inline const Vectors2& PatternTrackerCore6DOF::Pattern::imagePoints() const
{
	return imagePoints_;
}

inline Vectors2& PatternTrackerCore6DOF::Pattern::imagePoints()
{
	return imagePoints_;
}

inline const Vectors2& PatternTrackerCore6DOF::Pattern::referencePoints(const unsigned int layer) const
{
	ocean_assert(layer < patternPyramid_.layers() && layer < pyramidReferencePoints_.size());
	return pyramidReferencePoints_[layer];
}

inline unsigned int PatternTrackerCore6DOF::Pattern::layers() const
{
	return (unsigned int)(pyramidReferencePoints_.size());
}

inline bool PatternTrackerCore6DOF::Pattern::hasPoseGuess(HomogenousMatrix4& poseGuess, const double maximalAge)
{
	ocean_assert(maximalAge >= 0.0 && maximalAge <= 2.0);

	if (world_T_guessCamera_.isValid() && NumericD::abs(double(Timestamp(true) - poseGuessTimestamp_)) <= maximalAge)
	{
		poseGuess = world_T_guessCamera_;
		return true;
	}

	return false;
}

inline const HomogenousMatrix4& PatternTrackerCore6DOF::Pattern::poseGuess(Timestamp* timestamp)
{
	if (timestamp)
	{
		*timestamp = poseGuessTimestamp_;
	}

	return world_T_guessCamera_;
}

inline void PatternTrackerCore6DOF::Pattern::setPoseGuess(const HomogenousMatrix4& pose, const Timestamp& timestamp)
{
	world_T_guessCamera_ = pose;
	poseGuessTimestamp_ = timestamp;
}

inline bool PatternTrackerCore6DOF::Pattern::isValid() const
{
	return patternPyramid_.isValid();
}

inline PatternTrackerCore6DOF::Pattern::operator bool() const
{
	return isValid();
}

inline unsigned int PatternTrackerCore6DOF::numberPattern() const
{
	const ScopedLock scopedLock(lock_);

	return (unsigned int)(patternMap_.size());
}

inline unsigned int PatternTrackerCore6DOF::maxConcurrentlyVisiblePattern() const
{
	const ScopedLock scopedLock(lock_);

	return options_.maxConcurrentlyVisiblePattern_;
}

inline void PatternTrackerCore6DOF::setMaxConcurrentlyVisiblePattern(const unsigned int maxConcurrentlyVisiblePattern)
{
	const ScopedLock scopedLock(lock_);

	options_.maxConcurrentlyVisiblePattern_ = maxConcurrentlyVisiblePattern;
}

inline double PatternTrackerCore6DOF::maximumDurationBetweenRecognitionAttempts() const
{
	return (internalNumberVisiblePattern() == 0) ? options_.recognitionCadenceWithoutTrackedPatterns_ : options_.recognitionCadenceWithTrackedPatterns_;
}

inline unsigned int PatternTrackerCore6DOF::internalNumberVisiblePattern() const
{
	unsigned int number = 0u;

	for (PatternMap::const_iterator i = patternMap_.begin(); i != patternMap_.end(); ++i)
	{
		if (i->second.previousPose().isValid())
		{
			number++;
		}
	}

	return number;
}

inline unsigned int PatternTrackerCore6DOF::internalMaxConcurrentlyVisiblePattern() const
{
	if (options_.maxConcurrentlyVisiblePattern_ == 0u)
	{
		return (unsigned int)patternMap_.size();
	}
	else
	{
		return min(options_.maxConcurrentlyVisiblePattern_, (unsigned int)(patternMap_.size()));
	}
}

OCEAN_FORCE_INLINE unsigned int PatternTrackerCore6DOF::determineDescriptorDistance(const Descriptor& descriptorA, const Descriptor& descriptorB)
{
	return descriptorA.distance(descriptorB);
}

}

}

}

#endif // META_OCEAN_TRACKING_PATTERN_PATTERN_TRACKER_CORE_6DOF_H
