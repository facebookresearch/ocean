/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_OCULUSTAGS_OCULUSTAGTRACKER_H
#define META_OCEAN_TRACKING_OCULUSTAGS_OCULUSTAGTRACKER_H

#include "ocean/tracking/oculustags/OculusTags.h"
#include "ocean/tracking/oculustags/OculusTag.h"

#include "ocean/base/Frame.h"

#include "ocean/cv/FramePyramid.h"

#include "ocean/math/AnyCamera.h"

#include "ocean/geometry/Geometry.h"

#include "ocean/tracking/oculustags/QuadDetector.h"

namespace Ocean
{

namespace Tracking
{

namespace OculusTags
{

/**
 * This class implements a detector and tracker for Oculus Tags.
 * @ingroup trackingoculustags
 */
class OCEAN_TRACKING_OCULUSTAGS_EXPORT OculusTagTracker
{
	public:

		/**
		 * Definition of the states of a tracked tag
		 */
		enum TrackingState
		{
			/// Unknown or invalid state
			TS_UNKNOWN_STATE = 0,

			// A newly detected tag
			TS_NEW_DETECTION,

			/// A tag that is currently being tracked
			TS_TRACKING,

			/// Denotes known tag which are currently not tracked, e.g., because they occluded or out of view
			TS_NOT_TRACKING
		};

		/**
		 * Definition of the motion types of a tracked tag
		 */
		enum MotionType
		{
			/// The motion is not known
			MT_UNKNOWN = 0,

			// The motion dynamic, i.e., the tag is moving relative to the camera(s) (e.g., a tag on a moving object)
			MT_DYNAMIC,

			/// The motion is static, i.e., the tag does not move relative to the camera(s) (e.g., a tag that is glued to a wall)
			MT_STATIC,
		};

		/**
		 * Definition of groups of object corners on a tag.
		 *
		 * The corners points of a tag are defined as follows:
		 * @code
		 * 0---+---+---+---+---+---+---+---3
		 * | # | # | # | # | # | # | # | # |
		 * +---4---+---+---+---+---D---+---+
		 * | # |                   | # | # |
		 * +---+   X---X---X---X---C---B---+
		 * | # |   | = | = | = | = |   | # |
		 * +---+   X---X---X---X---X   +---+
		 * | # |   | = | = | = | = |   | # |
		 * +---+   X---X---X---X---X   +---+
		 * | # |   | = | = | = | = |   | # |
		 * +---+   X---X---X---X---X   +---+
		 * | # |   | = | = | = | = |   | # |
		 * +---5---6---X---X---X---9---A---+
		 * | # | # |               | # | # |
		 * +---+---7---+---+---+---8---+---+
		 * | # | # | # | # | # | # | # | # |
		 * 1---+---+---+---+---+---+---+---2
		 * @endcode
		 *
		 * There are additional corners (denoted by `X`) in the data matrix (denoted by `=`); their number and locations
		 * depend on the specific ID of the tag at hand.
		 *
		 * @sa getTagObjectPoints()
		 */
		enum TagPointGroup : uint8_t
		{
			/// The corners 0 through 3
			TPG_CORNERS_0_TO_3 = 1u << 0u,

			/// The corners 4 through D
			TPG_CORNERS_4_TO_D = 1u << 1u,

			/// The corners 0 through D
			TPG_CORNERS_0_TO_D = TPG_CORNERS_0_TO_3 | TPG_CORNERS_4_TO_D,

			/// The corners in the data matrix of a tag (including the corners 6, 9, and C)
			TPG_CORNERS_IN_DATA_MATRIX = 1u << 2u,

			/// All available corners
			TPG_CORNERS_ALL_AVAILABLE = TPG_CORNERS_0_TO_D | TPG_CORNERS_IN_DATA_MATRIX,

			/// The center locations of the modules in the data matrix (`=` in the figure above)
			TPG_DATA_MATRIX_MODULE_CENTERS = 1u << 3u,

			/// The centers of those modules on the outermost border of a tag, which are guaranteed to always have foreground values
			TPG_FOREGROUND_MODULE_CENTERS = 1u << 4u,

			/// The centers of those modules between the outermost border and a tag and the data matrix, which are guaranteed to always have background values
			TPG_BACKGROUND_MODULE_CENTERS = 1u << 5u,

			/// The centers of those modules that are required to determine the orientation of a tag
			TPG_ORIENTATION_MODULE_CENTERS = 1u << 6u,
		};


		/**
		 * A history of tag observations for one camera
		 * Each observation consists of a camera pose, a set of object points, and a set of corresponding image points
		 */
		class OCEAN_TRACKING_OCULUSTAGS_EXPORT TagObservationHistory
		{
			public:

				/**
				 * Adds a single observation of a tag to the collection
				 * @param world_T_camera The world-to-camera transformations, must be valid
				 * @param objectPoints The object points of the tag (in reference frame of the tag), must have at least one element
				 * @param imagePoints The image points corresponding to the object points at the given camera pose, size must be identical to number of object points
				 * @param trackingImagePoints The image points that should be used to track this tag from frame to frame (temporally), must have at least the four outer corners of the tag and the same size as `trackingObjectPoints`
				 * @param trackingObjectPoints The object points that should be used to track this tag from frame to frame (temporally), must have at least the four outer corners of the tag and the same size as `trackingImagePoints`
				 */
				inline void addObservation(const HomogenousMatrix4& world_T_camera, Vectors3&& objectPoints, Vectors2&& imagePoints, Vectors2&& trackingImagePoints, Vectors3&& trackingObjectPoints);

				/**
				 * Appends a collection of observations
				 * @param observationHistory The observations that will be appended, note: the other instance will be empty afterwards
				 */
				inline void append(TagObservationHistory& observationHistory);

				/**
				 * Remove all observations in that do not match the specified pose
				 * This function starts with the newest observation stored in this instance and works its way back in time. It stops
				 * at the first observation that exceeds the maximum projection error and removes it as well as everything before it
				 * (older observation).
				 * @param anyCamera The camera that was use for capturing, must be valid
				 * @param tag_T_world The pose of tag as the transformation which converts points in the world reference frame to the tag reference frame, must be valid
				 * @param maximalError Maximum projection error when applying the above tag pose to all stored observations, range: [0, infinity)
				 * @param maximalOutliersPercent Maximum percentage of points that may exceed the maximum projection error before the entire observation is rejected, range: [0, 1]
				 * @return The number of observations that have been removed, range: [0, size()]
				 */
				size_t removeObservations(const AnyCamera& anyCamera, const HomogenousMatrix4& tag_T_world, const Scalar maximalError, const Scalar maximalOutliersPercent = Scalar(0.1));

				/**
				 * Returns the number of observations
				 * @return The number of observations stored in this instance
				 */
				inline size_t size() const;

				/**
				 * Returns true if no observations are stored in this instance
				 * @return True if empty, otherwise false
				 */
				inline bool empty() const;

				/**
				 * Removes all stored observations stored in this instance
				 */
				inline void clear();

				/**
				 * Returns the world-to-camera transformations
				 * @return A reference to the world-to-camera transformations
				 */
				inline const HomogenousMatrices4& cameraPoses_world_T_camera() const;

				/**
				 * Returns the world-to-camera transformations
				 * @return A reference to the world-to-camera transformations
				 */
				inline HomogenousMatrices4& cameraPoses_world_T_camera();

				/**
				 * Returns the objects points stored in the instance
				 * @return A reference to the object points
				 */
				inline const Geometry::ObjectPointGroups& objectPointsGroups() const;

				/**
				 * Returns the objects points stored in the instance
				 * @return A reference to the object points
				 */
				inline Geometry::ObjectPointGroups& objectPointsGroups();

				/**
				 * Returns the image points stored in this instance
				 * @return A reference to the image points
				 */
				inline const Geometry::ImagePointGroups& imagePointsGroups() const;

				/**
				 * Returns the image points stored in this instance
				 * @return A reference to the image points
				 */
				inline Geometry::ImagePointGroups& imagePointsGroups();

				/**
				 * Returns the tracking object points stored in this instance
				 * @return A reference to the tracking object points
				 */
				inline const Geometry::ObjectPointGroups& trackingObjectPointsGroups() const;

				/**
				 * Returns the tracking points stored in this instance
				 * @return A reference to the tracking points
				 */
				inline Geometry::ObjectPointGroups& trackingObjectPointsGroups();

				/**
				 * Returns the tracking image points stored in this instance
				 * @return A reference to the tracking image points
				 */
				inline const Geometry::ImagePointGroups& trackingImagePointsGroups() const;

				/**
				 * Returns the tracking image points stored in this instance
				 * @return A reference to the tracking image points
				 */
				inline Geometry::ImagePointGroups& trackingImagePointsGroups();

				/**
				 * Returns the latest object points that should be used for tracking
				 * @return A reference to the latest tracking object points
				 */
				inline const Vectors3& latestTrackingObjectPoints() const;

				/**
				 * Returns the latest image points that should be used for tracking
				 * @return A reference to the latest tracking image points
				 */
				inline const Vectors2& latestTrackingImagePoints() const;

				/**
				 * Monoscopic optimization of the world-to-tag transformation using the stored object-image point correspondences
				 * @param anyCamera The camera that was use for capturing, must be valid
				 * @param unoptimized_world_T_tag The initial estimate of the world-to-tag transformation, must valid
				 * @param optimized_world_T_tag The optimized world-to-tag transformation
				 * @return True if the pose optimization was successful, otherwise false
				 */
				bool optimizePose(const AnyCamera& anyCamera, const HomogenousMatrix4& unoptimized_world_T_tag, HomogenousMatrix4& optimized_world_T_tag);

				/**
				 * Stereoscopic optimization of the world-to-tag transformation using the stored object-image point correspondences
				 * @param anyCameraA The first camera that captured the first set of observations, must be valid
				 * @param anyCameraB The second camera that captured the second set of observations, must be valid
				 * @param tagObservationHistoryA The first set of observations, must not be empty
				 * @param tagObservationHistoryB The second set of observation, must not be empty
				 * @param unoptimized_world_T_tag The initial estimate of the world-to-tag transformation, must valid
				 * @param optimized_world_T_tag The optimized world-to-tag transformation
				 * @return True if the pose optimization was successful, otherwise false
				 */
				static bool optimizePose(const AnyCamera& anyCameraA, const AnyCamera& anyCameraB, const TagObservationHistory& tagObservationHistoryA, const TagObservationHistory& tagObservationHistoryB, const HomogenousMatrix4& unoptimized_world_T_tag, HomogenousMatrix4& optimized_world_T_tag);

			public:

				/// The world-to-camera transformations
				HomogenousMatrices4 cameraPoses_world_T_camera_;

				/// The object points of the tag (in reference frame of the tag)
				Geometry::ObjectPointGroups objectPointsGroups_;

				/// The image points corresponding to the object points at the given camera pose
				Geometry::ImagePointGroups imagePointsGroups_;

				/// The image points that should be used to track the tag from frame to frame (temporally)
				Geometry::ImagePointGroups trackingImagePointsGroups_;

				/// The image points that should be used to track the tag from frame to frame (temporally)
				Geometry::ObjectPointGroups trackingObjectPointsGroups_;
		};

		/// A vector of tag observation histories
		typedef std::vector<TagObservationHistory> TagObservationHistories;

		/**
		 * This class organizes the information of tracked tags
		 */
		class TrackedTag
		{
			public:

				/**
				 * Copy constructor
				 * @param otherTrackedTag The other instance that will be cloned
				 */
				inline TrackedTag(const TrackedTag& otherTrackedTag);

				/**
				 * Move constructor
				 * @param otherTrackedTag The other instance that will be moved
				 */
				inline TrackedTag(TrackedTag&& otherTrackedTag);

				/**
				 * Constructor
				 * @param tag The tag that will be stored and managed, must be valid
				 * @param tagObservationHistoryA The observations from the first camera, must not be empty if `tagObservationsB` is empty
				 * @param tagObservationHistoryB The observations from the second camera, must not be empty if `tagObservationsA` is empty
				 * @param trackingState The tracking state of the tag
				 * @param motionType The motion type of the tag
				 */
				inline TrackedTag(OculusTag&& tag, TagObservationHistory&& tagObservationHistoryA, TagObservationHistory&& tagObservationHistoryB, const TrackingState trackingState, const MotionType motionType);

				/**
				 * Smaller-than comparator for tracked tag instances
				 * @param otherTrackedTag The other instance that will be compared against instance, must be valid
				 * @return True if the ID of tag stored in this instance is smaller than the ID of the tag stored in the other instance, otherwise false
				 */
				inline bool operator<(const TrackedTag& otherTrackedTag) const;

				/**
				 * Move-Assign operator
				 * @param otherTrackedTag The other instance of which all data will transferred to this instance; the other instance will be invalid afterwards
				 */
				inline TrackedTag& operator=(TrackedTag&& otherTrackedTag);

			public: // TODO Make the section below protected and add setters/getters (later once API is more stable)

				/// The tag that is managed
				OculusTag tag_;

				// The observations of `tag_` in the first camera
				TagObservationHistory tagObservationHistoryA_;

				/// The observations of `tag_` in the second camera
				TagObservationHistory tagObservationHistoryB_;

				/// The tracking state of the managed tag
				TrackingState trackingState_;

				/// The motion type of the managed tag
				MotionType motionType_;
		};

		/// A map to store all tracked tags
		typedef std::unordered_map<uint32_t, TrackedTag> TrackedTagMap;

		/// A vector of tracked tags.
		typedef std::vector<TrackedTag> TrackedTags;

	public:

		/**
		 * Default constructor.
		 */
		OculusTagTracker();

		/**
		 * Move constructor.
		 * @param oculusTagTracker Other instance that will be moved to this instance; the other instance will be invalid afterwards
		 */
		OculusTagTracker(OculusTagTracker&& oculusTagTracker);

		/**
		 * Destructor
		 */
		virtual ~OculusTagTracker();

		/**
		 * Move-Assign operator
		 * @param oculusTagTracker The other instance of which all data will transferred to this instance; the other instance will be invalid afterwards
		 */
		OculusTagTracker& operator=(OculusTagTracker&& oculusTagTracker);

		/**
		 * Tracks tags to the current stereo frames
		 * @param anyCameraA The first camera with which the first input image has been recorded, must be valid
		 * @param anyCameraB The second camera with which the second input image has been recorded, must be valid
		 * @param yFrameA The first input image in which Oculus tags will be searched, must be valid
		 * @param yFrameB The first input image in which Oculus tags will be searched, must be valid
		 * @param world_T_device The transformation that converts device points to world points, must be valid
		 * @param device_T_cameraA The transformation that converts points in the first camera to device points, must be valid
		 * @param device_T_cameraB The transformation that converts points in the second camera to device points, must be valid
		 * @param tags The vectors of tags that are visible in the provided stereo frame
		 * @return True if tags found in the provided stereo frame, otherwise false
		 */
		bool trackTagsStereo(const AnyCamera& anyCameraA, const AnyCamera& anyCameraB, const Frame& yFrameA, const Frame& yFrameB, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_cameraA, const HomogenousMatrix4& device_T_cameraB, OculusTags& tags);

		/**
		 * Returns the map of tracked tags
		 * @return A reference to the map of tracked tags
		 */
		inline const TrackedTagMap& trackedTagMap() const;

		/**
		 * Creates a rectified image of a tag for visualization
		 * @param anyCameraA The first camera with which the first input image has been recorded, must be valid
		 * @param anyCameraB The second camera with which the second input image has been recorded, must be valid
		 * @param yFrameA The first input image in which Oculus tags will be searched, must be valid
		 * @param yFrameB The first input image in which Oculus tags will be searched, must be valid
		 * @param world_T_device The transformation that converts device points to world points, must be valid
		 * @param device_T_cameraA The transformation that converts points in the first camera to device points, must be valid
		 * @param device_T_cameraB The transformation that converts points in the second camera to device points, must be valid
		 * @param tag The tag for which the image will be rectified
		 * @param rectifiedFrame The frame will hold the rectified image of the tag, will have the same pixel format as the input and have the size `rectifiedFrameSize` x `rectifiedFrameSize`
		 * @param rectifiedFrameSize Optional size of the rectified frame, range: [1, infinity)
		 * @return True if the creation of the rectified frame was successful, otherwise false
		 */
		static bool extractRectifiedTagImage(const AnyCamera& anyCameraA, const AnyCamera& anyCameraB, const Frame& yFrameA, const Frame& yFrameB, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_cameraA, const HomogenousMatrix4& device_T_cameraB, const OculusTag& tag, Frame& rectifiedFrame, const uint32_t rectifiedFrameSize = 128u);

		/**
		 * Detect tags in a grayscale frame
		 * @param anyCamera The camera with which the input image has been recorded, must be valid
		 * @param yFrame The input image in which Oculus tag will be search, must be valid
		 * @param world_T_device The transformation that converts device points to world points, must be valid
		 * @param device_T_camera The transformation that converts points camera to device points, must be valid
		 * @param defaultTagSize The edge length of all detected tags that are not specified in `tagSizeMap`, range: (0, infinity)
		 * @param tagSizeMap Optional mapping of tag IDs to specific tag sizes, range of tag IDs (key): [0, 1024), range of tag sizes (value): (0, infinity)
		 * @param tagObservationHistories Optional return value holding the tag observations (2D-3D point correspondences)
		 * @return The detected tags
		 */
		static OculusTags detectTagsMono(const AnyCamera& anyCamera, const Frame& yFrame, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_camera, const Scalar defaultTagSize, const TagSizeMap& tagSizeMap = TagSizeMap(), TagObservationHistories* tagObservationHistories = nullptr);

		/**
		 * Locates a detected tag in a different camera image, e.g., the second camera of a stereo camera
		 * @note The camera specified here must be mounted to the same device, i.e., use the `world_T_device` transformation as the tag
		 * @param anyCamera The camera with which the input image has been recorded, must be valid
		 * @param yFrame The input image in which Oculus tag will be search, must be valid
		 * @param world_T_device The transformation that converts device points to world points, must be valid
		 * @param device_T_camera The transformation that converts points camera to device points, must be valid
		 * @param tag The tag that has been detected in a different camera and which will be located in the given camera if possible, must be valid
		 * @param tagObservationHistory The result holding the observation the tag for the provided camera
		 * @return True if the tag has been located in the provided camera, otherwise false
		 */
		static bool locateTagInCamera(const AnyCamera& anyCamera, const Frame& yFrame, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_camera, const OculusTag& tag, TagObservationHistory& tagObservationHistory);

		/**
		 * Assembles specific points on a tag in the tag object space
		 * @param tagPointGroup Indicates which points should be returned
		 * @param tagSize The size of the tag for which the corners are computed, range: (0, infinity)
		 * @param dataMatrix The data matrix of the matrix; only needs to be specified if corners inside the area of the data matrix should be generated (can otherwise be ignored)
		 * @return The generated corners in the object space
		 */
		static Vectors3 getTagObjectPoints(const TagPointGroup tagPointGroup, const Scalar tagSize, const OculusTag::DataMatrix dataMatrix = 0u);

		/**
		* Expands the data matrix of a tag to a vector of binary module values
		* @param dataMatrix The data matrix of a tag that will be expanded
		* @return A vector containing the values of all 64 modules of a tag
		*/
		static std::vector<uint8_t> generateModuleValues(const OculusTag::DataMatrix& dataMatrix);

		/**
		* Checks if a valid tag is fully visible in the frame of a camera
		* @param anyCamera The camera for which will be checked if the tag is fully visible for it, must be valid
		* @param tag_T_camera The transformation that maps tag object points to camera object coordinates, must be valid
		* @param tagSize The size of that tag, range: (0, infinity)
		* @param signedBorder The optional border increasing or decreasing the rectangle in which the tag corners must be located inside the camera frame, in pixels, range: (-infinity, std::min(anyCamera.width(), anyCamera.height()) / 2)
		* @return True if the tag is valid and fully visible in the frame of the camera, otherwise false
		*/
		static bool isTagVisible(const AnyCamera& anyCamera, const HomogenousMatrix4& tag_T_camera, const Scalar tagSize, const Scalar signedBorder);

	protected:

		/**
		 * Tracks 2D-3D correspondences to temporally from frame to frame
		 * @param anyCamera The camera with which the current and previous frame (pyramids) have been recorded, must be valid
		 * @param framePyramid The frame pyramid of the current frame, must be valid
		 * @param previousFramePyramid The frame pyramid of the previous frame, must be valid
		 * @param world_T_camera The transformation that converts device points to world points at the current time stamp, must be valid
		 * @param previous_world_T_tag The transformation that converts device points to world points at the previous time stamp, must be valid
		 * @param objectPoints The object points that corresponds to the previous image points, must have the same size as `previousImagePoints`
		 * @param previousImagePoints The image location of the tag at the previous time stamp, must have at least four elements (the outer corners)
		 * @param imagePoints The image locations of the tracked points at the current time stamp, will have the same size as `previousImagePoints`
		 * @return True if the points were tracked successfully, otherwise false
		 */
		static bool trackTagCornersTemporally(const AnyCamera& anyCamera, const CV::FramePyramid& framePyramid, const CV::FramePyramid& previousFramePyramid, const HomogenousMatrix4& world_T_camera, const HomogenousMatrix4& previous_world_T_tag, const Vectors3& objectPoints, const Vectors2& previousImagePoints, Vectors2& imagePoints);

		/**
		 * Detects Oculus Tags in stereo images.
		 * @param anyCameras The camera models that correspond to the input images, must have two valid elements
		 * @param yFrames The 8-bit grayscale images in which the tags will be detected, must have two valid elements
		 * @param world_T_device The world pose of the device, must be valid
		 * @param device_T_cameras The device poses of the all cameras, must have two valid elements
		 * @return The detected tags.
		 */
		static TrackedTags detectTagsStereo(const SharedAnyCameras& anyCameras, const Frames& yFrames, const HomogenousMatrix4& world_T_device, const HomogenousMatrices4& device_T_cameras);

		/**
		 * Reads the tag information from an image given the locations of its four outer corners
		 * @param anyCamera The camera with which the input image has been recorded, must be valid
		 * @param yFrame The frame from which the tag will be read, must be valid
		 * @param unorientedQuad The four corners of the tag, must be in counter-clockwise order and fully inside image
		 * @param world_T_device The transformation that converts device points to world points, must be valid
		 * @param device_T_camera The transformation from the camera to the device, must be valid
		 * @param defaultTagSize The default edge length of the detected tag, range: (0, infinity)
		 * @param tag The tag instance initialized with the information from the tag in the image, only valid if this function returns `true`
		 * @param quad The four outer corners of the tag in counter-clockwise order starting with the top-left corner
		 * @param tagSizeMap Optional mapping of tag IDs to specific tag sizes, range of tag IDs (key): [0, 1024), range of tag sizes (value): (0, infinity)
		 * @return True, if a tag at the specified location could be read, otherwise false
		 */
		static bool readTag(const AnyCamera& anyCamera, const Frame& yFrame, const QuadDetector::Quad& unorientedQuad, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_camera, const Scalar defaultTagSize, OculusTag& tag, QuadDetector::Quad& quad, const TagSizeMap& tagSizeMap = TagSizeMap());

		/**
		 * Determines the reflectance type of an tag candidate and the intensity threshold between foreground and background
		 * @param anyCamera The camera with which the input image has been recorded, must be valid
		 * @param yFrame The grayscale image in which the tag candidate has been found, must be valid
		 * @param tag_T_camera The 6DOF pose of the tag relative to the camera coordinates at detection time
		 * @param tagSize The size of the corresponding tag, range: (0, infinity)
		 * @param reflectanceType The reflectance type of the tag candidate that has been determined
		 * @param intensityThreshold The intensity value that can be used to separate foreground and background pixel values
		 * @param moduleValueDark The binary value of dark modules, can be `1` or `0`
		 * @return True if the computation was successful, otherwise false
		 */
		static bool determineReflectanceTypeAndIntensityThreshold(const AnyCamera& anyCamera, const Frame& yFrame, const HomogenousMatrix4& tag_T_camera, const Scalar tagSize, OculusTag::ReflectanceType& reflectanceType, uint8_t& intensityThreshold, uint8_t& moduleValueDark);

		/**
		 * Determines the top-left corner of the tag candidate
		 * @param anyCamera The camera with which the input image has been recorded, must be valid
		 * @param yFrame The grayscale image in which the tag candidate has been found, must be valid
		 * @param unorientedQuad The boundary pattern/tag candidate
		 * @param unorientedTag_T_camera The 6DOF pose of the tag relative to the camera coordinates at detection time
		 * @param tagSize The size of the corresponding tag, range: (0, infinity)
		 * @param orientedQuad Rotated version of `boundaryPattern` where the first element corresponds to the top-left corner of the tag candidate
		 * @param orientedTag_T_camera The correctly rotated (oriented) 6DOF pose of the tag relative to the camera so that the origin is the in top-left corner of the tag
		 * @param intensityThreshold The intensity value that can be used to separate foreground and background pixel values
		 * @param binaryModuleValueDark The binary value of dark modules, can be `1` or `0`
		 * @return True if the computation was successful, otherwise false
		 */
		static bool determineOrientation(const AnyCamera& anyCamera, const Frame& yFrame, const QuadDetector::Quad& unorientedQuad, const HomogenousMatrix4& unorientedTag_T_camera, const Scalar tagSize, QuadDetector::Quad& orientedQuad, HomogenousMatrix4& orientedTag_T_camera, const uint8_t& intensityThreshold, const uint8_t& binaryModuleValueDark);

		/**
		 * Reads the modules from the data matrix of a tag
		 * @param anyCamera The camera with which the input image has been recorded, must be valid
		 * @param yFrame The grayscale image in which the tag candidate has been found, must be valid
		 * @param tag_T_camera The 6DOF pose of the tag relative to the camera coordinates at detection time
		 * @param tagSize The size of the corresponding tag, range: (0, infinity)
		 * @param intensityThreshold The intensity value that can be used to separate foreground and background pixel values
		 * @param binaryModuleValueDark The binary value of dark modules, can be `1` or `0`
		 * @param binaryModuleValueLight binary value of light modules, can be `1` or `0`
		 * @param dataMatrix Will store the modules that were read from a tag
		 * @return True if the computation was successful, otherwise false
		 */
		static bool readDataMatrix(const AnyCamera& anyCamera, const Frame& yFrame, const HomogenousMatrix4& tag_T_camera, const Scalar tagSize, const uint8_t& intensityThreshold, const uint8_t& binaryModuleValueDark, const uint8_t& binaryModuleValueLight, OculusTag::DataMatrix& dataMatrix);

		/**
		 * Determines a set 2D-3D corner correspondences of a tag
		 * @param anyCamera The camera with which the input image has been recorded, must be valid
		 * @param yFrame The grayscale image in which the tag candidate has been found, must be valid
		 * @param world_T_device The transformation that converts device points to world points, must be valid
		 * @param device_T_camera The transformation that converts points in the camera to device points, must be valid
		 * @param tag The tag for which the observation corners will be determined, must be valid
		 * @param quad The image locations of the four outer corners of the tag, must be fully inside the image
		 * @param tagObservationHistory The observations to which the new observation will be added
		 * @return True if a set of correspondences was added, otherwise false
		 */
		static bool addTagObservation(const AnyCamera& anyCamera, const Frame& yFrame, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_camera, const OculusTag& tag, const QuadDetector::Quad& quad, TagObservationHistory& tagObservationHistory);

		/**
		 * Determines 2D-3D corner correspondences of a tag and optimizes the tag pose based on them
		 * @note The optimization will use all the observations stored in `tagObservations` for the pose optimization
		 * @param anyCamera The camera with which the input image has been recorded, must be valid
		 * @param yFrame The grayscale image in which the tag candidate has been found, must be valid
		 * @param world_T_device The transformation that converts device points to world points, must be valid
		 * @param device_T_camera The transformation that converts points in the camera to device points, must be valid
		 * @param tag The tag for which the observation corners will be determined, must be valid
		 * @param quad The image locations of the four outer corners of the tag, must be fully inside the image
		 * @param tagObservationHistory The observations to which the new observation will be added
		 * @return True if a set of correspondences was added, otherwise false
		 */
		static bool addTagObservationAndOptimize(const AnyCamera& anyCamera, const Frame& yFrame, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_camera, OculusTag& tag, const QuadDetector::Quad& quad, TagObservationHistory& tagObservationHistory);

		/**
		 * Computes the 6DOF pose of the tag relative to the location of the camera using 3D-to-2D point correspondences
		 * @param anyCamera The camera with which the input image has been recorded, must be valid
		 * @param imagePoints The 2D image points, size: [4, infinity)
		 * @param objectPoints The corresponding 3D object points in the reference frame of the tag, size: must be identical to `imagePoints`
		 * @param object_T_camera The 6DOF pose of the tag relative to the camera
		 * @param minPoints Minimum number of points used in each RANSAC iteration, range: [4, imagePoints.size())
		 * @return True if the pose has been successfully computed, otherwise false
		 */
		static bool computePose(const AnyCamera& anyCamera, const Vectors2& imagePoints, const Vectors3& objectPoints, HomogenousMatrix4& object_T_camera, const uint32_t minPoints = 4u);

		/**
		 * Confirms tag in image given a valid tag instance
		 * @param anyCamera The camera with which the input image has been recorded, must be valid
		 * @param yFrame he grayscale image in which the tag will be confirmed, must be valid
		 * @param world_T_device The transformation that converts device points to world points, must be valid
		 * @param device_T_camera The transformation that converts points in the camera to device points, must be valid
		 * @param tag The tag which will be confirmed in the input image, must be valid
		 * @return True if the tag was found at the specified location in the image, otherwise false
		 */
		static bool confirmDetectionInFrame(const AnyCamera& anyCamera, const Frame& yFrame, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_camera, const OculusTag& tag);

		/**
		 * Creates an image pyramid from a frame with padding
		 * @param yFrame The frame for which a frame pyramid will be created, must be valid
		 * @param layers The number of layers that the newly created pyramid should have, range: [1, infinity)
		 * @return The newly created image pyramid
		 */
		static CV::FramePyramid createFramePyramid(const Frame& yFrame, const uint32_t layers);

	protected:

		/// A frame counter
		uint32_t frameCounter_;

		/// A map tags that are (known and) tracked
		TrackedTagMap trackedTagMap_;

		/// The input frames of the previous tracking iteration
		Frame previousYFrames_[2];

		/// The frame pyramids of the previous tracking iteration
		CV::FramePyramid previousFramePyramids_[2];

		/// The previous pose of the device
		HomogenousMatrix4 previous_world_T_device_;

		/// The poses of the input cameras of the previous tracking iteration
		HomogenousMatrix4 previousDevice_T_cameras_[2];

		/// The border area along the inside of the image which will be ignored completely (in pixels), range: [0, min(imageWidth, imageHeight))
		static constexpr uint32_t frameBorder_ = 10u;

		/// The minimum absolute difference between foreground and background color in order to count as a transition
		static constexpr uint32_t minIntensityThreshold_ = 10u;

		/// The minimum required number of observations before the motion of a tag can be declared static (i.e. it doesn't move in the world), range: [1, infinity)
		static constexpr size_t numberRequiredObservationForStatic_ = 5;

		/// The maximum number of observations per tag that will be stored, range: [1, infinity)
		static constexpr size_t numberMaxAllowedObservations_ = 15;

		/// The number of frames after which the detector is run to detect new tags, range: [1, infinity)
		static constexpr unsigned int detectionCadence_ = 15u;

		/// The maximum projection error in pixels, range: [0, infinit)
		static constexpr Scalar maxAllowedProjectionError_ = Scalar(0.5);

		/// The number of layers used for the frame pyramids, maximum supported pixel motion: 2^LAYERS, range: [1, infinity)
		static constexpr unsigned int numberFrameLayers_ = 6u;
};

inline void OculusTagTracker::TagObservationHistory::addObservation(const HomogenousMatrix4& world_T_camera, Vectors3&& objectPoints, Vectors2&& imagePoints, Vectors2&& trackingImagePoints, Vectors3&& trackingObjectPoints)
{
	ocean_assert(world_T_camera.isValid());
	ocean_assert(objectPoints.empty() == false);
	ocean_assert(objectPoints.size() == imagePoints.size());
	ocean_assert(trackingImagePoints.size() >= 4);
	ocean_assert(trackingImagePoints.size() == trackingObjectPoints.size());

	cameraPoses_world_T_camera_.emplace_back(world_T_camera);
	objectPointsGroups_.emplace_back(std::move(objectPoints));
	imagePointsGroups_.emplace_back(std::move(imagePoints));
	trackingImagePointsGroups_.emplace_back(std::move(trackingImagePoints));
	trackingObjectPointsGroups_.emplace_back(std::move(trackingObjectPoints));
}

inline void OculusTagTracker::TagObservationHistory::append(TagObservationHistory& otherObservationHistory)
{
	if (otherObservationHistory.empty())
	{
		return;
	}

	HomogenousMatrices4 otherCameraPoses_world_T_camera = std::move(otherObservationHistory.cameraPoses_world_T_camera());
	Geometry::ObjectPointGroups otherObjectPointsGroup = std::move(otherObservationHistory.objectPointsGroups());
	Geometry::ImagePointGroups otherImagePointsGroup = std::move(otherObservationHistory.imagePointsGroups());
	Geometry::ImagePointGroups otherTrackingImagePointsGroup = std::move(otherObservationHistory.trackingImagePointsGroups());
	Geometry::ObjectPointGroups otherTrackingObjectPointsGroup = std::move(otherObservationHistory.trackingObjectPointsGroups());

	ocean_assert(otherObservationHistory.empty());
	ocean_assert(otherCameraPoses_world_T_camera.size() != 0);
	ocean_assert(otherCameraPoses_world_T_camera.size() == otherObjectPointsGroup.size());
	ocean_assert(otherCameraPoses_world_T_camera.size() == otherImagePointsGroup.size());
	ocean_assert(otherCameraPoses_world_T_camera.size() == otherTrackingImagePointsGroup.size());
	ocean_assert(otherCameraPoses_world_T_camera.size() == otherTrackingObjectPointsGroup.size());

	for (size_t i = 0; i < otherCameraPoses_world_T_camera.size(); ++i)
	{
		addObservation(otherCameraPoses_world_T_camera[i], std::move(otherObjectPointsGroup[i]), std::move(otherImagePointsGroup[i]), std::move(otherTrackingImagePointsGroup[i]), std::move(otherTrackingObjectPointsGroup[i]));
	}
}

inline size_t OculusTagTracker::TagObservationHistory::size() const
{
	ocean_assert(cameraPoses_world_T_camera_.size() == objectPointsGroups_.size());
	ocean_assert(cameraPoses_world_T_camera_.size() == imagePointsGroups_.size());
	ocean_assert(cameraPoses_world_T_camera_.size() == trackingImagePointsGroups_.size());
	ocean_assert(cameraPoses_world_T_camera_.size() == trackingObjectPointsGroups_.size());

	return cameraPoses_world_T_camera_.size();
}

inline bool OculusTagTracker::TagObservationHistory::empty() const
{
	ocean_assert(cameraPoses_world_T_camera_.size() == objectPointsGroups_.size());
	ocean_assert(cameraPoses_world_T_camera_.size() == imagePointsGroups_.size());
	ocean_assert(cameraPoses_world_T_camera_.size() == trackingImagePointsGroups_.size());
	ocean_assert(cameraPoses_world_T_camera_.size() == trackingObjectPointsGroups_.size());

	return cameraPoses_world_T_camera_.empty();
}

inline void OculusTagTracker::TagObservationHistory::clear()
{
	cameraPoses_world_T_camera_.clear();
	objectPointsGroups_.clear();
	imagePointsGroups_.clear();
	trackingImagePointsGroups_.clear();
	trackingObjectPointsGroups_.clear();
}

inline const HomogenousMatrices4& OculusTagTracker::TagObservationHistory::cameraPoses_world_T_camera() const
{
	return cameraPoses_world_T_camera_;
}

inline HomogenousMatrices4& OculusTagTracker::TagObservationHistory::cameraPoses_world_T_camera()
{
	return cameraPoses_world_T_camera_;
}

inline const Geometry::ObjectPointGroups& OculusTagTracker::TagObservationHistory::objectPointsGroups() const
{
	return objectPointsGroups_;
}

inline Geometry::ObjectPointGroups& OculusTagTracker::TagObservationHistory::objectPointsGroups()
{
	return objectPointsGroups_;
}

inline const Geometry::ImagePointGroups& OculusTagTracker::TagObservationHistory::imagePointsGroups() const
{
	return imagePointsGroups_;
}

inline Geometry::ImagePointGroups& OculusTagTracker::TagObservationHistory::imagePointsGroups()
{
	return imagePointsGroups_;
}

inline const Geometry::ObjectPointGroups& OculusTagTracker::TagObservationHistory::trackingObjectPointsGroups() const
{
	return trackingObjectPointsGroups_;
}

inline Geometry::ObjectPointGroups& OculusTagTracker::TagObservationHistory::trackingObjectPointsGroups()
{
	return trackingObjectPointsGroups_;
}

inline const Geometry::ImagePointGroups& OculusTagTracker::TagObservationHistory::trackingImagePointsGroups() const
{
	return trackingImagePointsGroups_;
}

inline Geometry::ImagePointGroups& OculusTagTracker::TagObservationHistory::trackingImagePointsGroups()
{
	return trackingImagePointsGroups_;
}

inline const Vectors3& OculusTagTracker::TagObservationHistory::latestTrackingObjectPoints() const
{
	ocean_assert(empty() == false);
	ocean_assert(trackingObjectPointsGroups_.back().size() == trackingImagePointsGroups_.back().size());

	return trackingObjectPointsGroups_.back();
}

inline const Vectors2& OculusTagTracker::TagObservationHistory::latestTrackingImagePoints() const
{
	ocean_assert(empty() == false);
	ocean_assert(trackingObjectPointsGroups_.back().size() == trackingImagePointsGroups_.back().size());

	return trackingImagePointsGroups_.back();
}

inline OculusTagTracker::TrackedTag::TrackedTag(const TrackedTag& otherTrackedTag)
{
	tag_ = otherTrackedTag.tag_;
	tagObservationHistoryA_ = otherTrackedTag.tagObservationHistoryA_;
	tagObservationHistoryB_ = otherTrackedTag.tagObservationHistoryB_;
	trackingState_ = otherTrackedTag.trackingState_;
	motionType_ = otherTrackedTag.motionType_;
}

inline OculusTagTracker::TrackedTag::TrackedTag(TrackedTag&& otherTrackedTag)
{
	*this = std::move(otherTrackedTag);
}

inline OculusTagTracker::TrackedTag::TrackedTag(OculusTag&& tag, TagObservationHistory&& tagObservationHistoryA, TagObservationHistory&& tagObservationHistoryB, const TrackingState trackingState, const MotionType motionType) :
	tag_(std::move(tag)),
	tagObservationHistoryA_(std::move(tagObservationHistoryA)),
	tagObservationHistoryB_(std::move(tagObservationHistoryB)),
	trackingState_(trackingState),
	motionType_(motionType)
{
	ocean_assert(tag_.isValid());
	ocean_assert(tagObservationHistoryA_.size() != 0 || tagObservationHistoryB_.size() != 0);
}

inline bool OculusTagTracker::TrackedTag::operator<(const TrackedTag& otherTrackedTag) const
{
	ocean_assert(tag_.isValid() && otherTrackedTag.tag_.isValid());
	return tag_.tagID() < otherTrackedTag.tag_.tagID();
}

inline OculusTagTracker::TrackedTag& OculusTagTracker::TrackedTag::operator=(TrackedTag&& otherTrackedTag)
{
	if (this != &otherTrackedTag)
	{
		tag_ = std::move(otherTrackedTag.tag_);
		tagObservationHistoryA_ = std::move(otherTrackedTag.tagObservationHistoryA_);
		tagObservationHistoryB_ = std::move(otherTrackedTag.tagObservationHistoryB_);
		trackingState_ = otherTrackedTag.trackingState_;
		motionType_ = otherTrackedTag.motionType_;
	}

	return *this;
}

inline const OculusTagTracker::TrackedTagMap& OculusTagTracker::trackedTagMap() const
{
	return trackedTagMap_;
}

}  // namespace OculusTags

}  // namespace Tracking

}  // namespace Ocean

#endif // META_OCEAN_TRACKING_OCULUSTAGS_OCULUSTAGTRACKER_H
