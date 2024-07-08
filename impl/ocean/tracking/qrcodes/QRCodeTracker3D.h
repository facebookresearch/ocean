/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/tracking/qrcodes/QRCodes.h"

#include "ocean/cv/FramePyramid.h"

#include "ocean/cv/detector/qrcodes/QRCodeDetector3D.h"

#include "ocean/geometry/Geometry.h"

namespace Ocean
{

namespace Tracking
{

namespace QRCodes
{

/**
 * This class implements a 6-DOF tracker for QR codes.
 * @ingroup trackingqrcodes
 */
class OCEAN_TRACKING_QRCODES_EXPORT QRCodeTracker3D : public CV::Detector::QRCodes::QRCodeDetector3D
{
	protected:

		/// Forward-declaration
		class ObservationHistory;

		/// Vector of observation histories
		typedef std::vector<ObservationHistory> ObservationHistories;

	public:

		/**
		 * Definition of tracking states.
		 */
		enum TrackingState
		{
			/// Unknown/invalid tracking statue
			TS_UNKNOWN_STATE = 0u,
			/// State for currently tracked codes
			TS_TRACKING,
			/// State when tracking has been lost
			TS_LOST
		};

		/// The unique ID of each tracked code.
		typedef uint32_t ObjectId;

		/// Definition of a pointer to the function that provides new 6DOF detections of QR codes, QRCodeDetector3D::detectQRCodes()
		typedef std::function<bool(const SharedAnyCameras&, const Frames&, const HomogenousMatrix4&, const HomogenousMatrices4&, CV::Detector::QRCodes::QRCodes&, HomogenousMatrices4&, Scalars&, Worker*, const bool)> CallbackQRCodeDetection3D;

		/// Definition of a function pointer that is called in the event a new QR code is detected for the first time.
		typedef std::function<void(const CV::Detector::QRCodes::QRCode&, const HomogenousMatrix4&, const Scalar, const ObjectId)> CallbackNewQRCode;

		/**
		 * Definition of parameters that control the tracker
		 */
		struct Parameters
		{
			/// The number of frames after which the detection will be run, range: [1, infinity)
			unsigned int detectionCadence_ = 15u;

			/// The number of layers of the image pyramid that are used for the frame-to-frame tracking of points, range: [1, infinity)
			unsigned int trackingNumberFramePyramidLayers_ = 3u;

			/// The time for which a code that is no longer tracked is removed from the database, in seconds, range: (0, infinity)
			double trackingLostGraceTimeout_ = NumericT<double>::maxValue();

			/// The maximum projection error that different observations may have to be counted as identical, in pixels, range: (0, infinity)
			Scalar observationHistoryMaxProjectionError = Scalar(0.5);

			/// The maximum amount of outliers (points) that different observations may have to be counted as identical, in percent, range: [0, 1]
			Scalar observationHistoryMaxOutliersPercent = Scalar(0.1);
		};

		/**
		 * A tracked code.
		 */
		class TrackedQRCode
		{
			friend class QRCodeTracker3D;

			public:

				/**
				 * Constructs an invalid tracked code.
				 */
				TrackedQRCode() = default;

				/**
				 * Constructs a tracked code.
				 * @param code The code that is tracked, must be valid.
				 * @param world_T_code The 6DOF pose of the code mapping from object space to world space, must be valid.
				 * @param codeSize The size of the code in the physical world, in meters, range: (0, infinity)
				 * @param trackingObjectPoints The set of object points of this code that should be used for tracking, must have >= 3 elements
				 * @param trackingState The tracking state that this tracked code will be initialized with, must not be `TS_UNKNOWN_STATE`.
				 * @param trackingTimestamp The time at which the code was tracked, must be valid.
				 */
				TrackedQRCode(CV::Detector::QRCodes::QRCode&& code, HomogenousMatrix4&& world_T_code, const Scalar codeSize, Geometry::ObjectPoints&& trackingObjectPoints, const TrackingState trackingState, const Timestamp trackingTimestamp);

				/**
				 * Constructs a tracked code.
				 * @param code The code that is tracked, must be valid.
				 * @param world_T_code The 6DOF pose of the code mapping from object space to world space, must be valid.
				 * @param codeSize The size of the code in the physical world, in meters, range: (0, infinity)
				 * @param trackingObjectPoints The set of object points of this code that should be used for tracking, must have >= 3 elements
				 * @param trackingState The tracking state that this tracked code will be initialized with, must not be `TS_UNKNOWN_STATE`.
				 * @param trackingTimestamp The time at which the code was tracked, must be valid.
				 */
				TrackedQRCode(const CV::Detector::QRCodes::QRCode& code, const HomogenousMatrix4& world_T_code, const Scalar codeSize, const Geometry::ObjectPoints& trackingObjectPoints, const TrackingState trackingState, const Timestamp trackingTimestamp);

				/**
				 * Returns the tracked code.
				 * @return The code.
				 */
				inline const CV::Detector::QRCodes::QRCode& code() const;

				/**
				 * Returns the 6DOF pose of the tracked code.
				 * @return The 6DOF pose.
				 */
				inline const HomogenousMatrix4& world_T_code() const;

				/**
				 * Return the size of the code in the physical world.
				 * @return The size.
				 */
				inline Scalar codeSize() const;

				/**
				 * Returns the tracking state of the tracked code.
				 * @return The tracking state.
				 */
				inline TrackingState trackingState() const;

				/**
				 * Returns the time stamp of the moment when this code was tracked.
				 * @return The time stamp.
				 */
				inline const Timestamp& trackingTimestamp() const;

				/**
				 * Returns if this tracked code is valid.
				 * @return True, if so.
				 */
				inline bool isValid() const;

				/**
				 * Returns the object points that are used to track this code
				 * @return The object points
				 */
				inline const Geometry::ObjectPoints& trackingObjectPoints() const;

			protected:

				/**
				 * Updates the 6DOF pose of the tracked code.
				 * @param world_T_code The new 6DOF pose of the tracked code, must be valid.
				 * @param codeSize The new size estimate of the code, range: (0, infinity)
				 * @param trackingTimestamp The time stamp that the pose corresponds to, must be valid.
				 */
				inline void updateTrackingPose(HomogenousMatrix4&& world_T_code, const Scalar codeSize, const Timestamp trackingTimestamp);

				/**
				 * Sets the tracking state to lost.
				 */
				inline void setTrackingLost();

				/**
				 * Returns the list of observation histories of this code.
				 * @return The list of observation histories.
				 */
				inline ObservationHistories& observationHistories();

			protected:

				/// The tracked QR code.
				CV::Detector::QRCodes::QRCode code_;

				/// The 6DOF pose of the tracked QR code.
				HomogenousMatrix4 world_T_code_ = HomogenousMatrix4(false);

				/// The size of the code in the physical world .
				Scalar codeSize_ = Scalar(0);

				/// The tracking state of the tracked QR code.
				TrackingState trackingState_ = TS_UNKNOWN_STATE;

				/// The time when this code was tracked.
				Timestamp trackingTimestamp_ = Timestamp(false);

				/// The observation histories of the tracked QR code. One observation history per camera.
				ObservationHistories observationHistories_;

				/// The object points that this code can be tracked with.
				Geometry::ObjectPoints trackingObjectPoints_;
		};

		/// The definition of map of tracked QR codes.
		typedef std::unordered_map<ObjectId, TrackedQRCode> TrackedQRCodesMap;

	protected:

		/**
		 * The definition of an observation history.
		 */
		class ObservationHistory
		{
			public:

				/**
				 * Creates an empty observation history.
				 */
				ObservationHistory() = default;

				/**
				 * Adds a new observation to the observation history.
				 * @param sharedAnyCamera The camera what was used to compute this observation, must be valid.
				 * @param world_T_camera The 6DOF pose of the camera in the world, must be valid.
				 * @param objectPoints The object points of the tracked QR code, must have at least 3 elements and the same size as `imagePoints`.
				 * @param imagePoints The image points of the tracked QR code, must have at least 3 elements and the size as `objectPoints`.
				 */
				void addObservation(const SharedAnyCamera& sharedAnyCamera, const HomogenousMatrix4& world_T_camera, Geometry::ObjectPoints&& objectPoints, Geometry::ImagePoints&& imagePoints);

				/**
				 * Removes all previous observations that are taken from a different pose than the specified one.
				 * @param sharedAnyCamera The camera what was used to compute this observation, must be valid.
				 * @param world_T_code The transformation between code and world, must be valid.
				 * @param maxProjectionError The maximum projection error that different observations may have to be counted as identical, in pixels, range: (0, infinity)
				 * @param maxOutliersPercent The maximum amount of outliers (points) that different observations may have to be counted as identical, in percent, range: [0, 1]
				 */
				size_t removeObservations(const SharedAnyCamera& sharedAnyCamera, const HomogenousMatrix4& world_T_code, const Scalar maxProjectionError, const Scalar maxOutliersPercent);

				/**
				 * Returns the latest group of object points.
				 * @note Do not call this function if no observation history exists.
				 * @return The latest object points.
				 */
				const Geometry::ObjectPoints& latestObjectPoints() const;

				/**
				 * Returns the latest group of image points
				 * @note Do not call this function if no observation history exists.
				 * @return The latest image points.
				 */
				const Geometry::ImagePoints& latestImagePoints() const;

				/**
				 * Return the number of observations stored in this history.
				 * @return The number of observations
				 */
				size_t size();

				/**
				 * Removes all stored observations from this history.
				 */
				void clear();

			protected:

				/// The cameras what were used to compute these observations.
				SharedAnyCameras sharedAnyCameras_;

				/// The camera-to-world transformations, one element per observation.
				HomogenousMatrices4 world_T_cameras_;

				/// The object points of a code observation, one element per observation.
				Geometry::ObjectPointGroups objectPointsGroups_;

				/// The image points of a code observation, one element per observation.
				Geometry::ImagePointGroups imagePointsGroups_;
		};

	public:

		/**
		 * Constructs a tracker instance
		 */
		QRCodeTracker3D() = default;

		/**
		 * Constructs a tracker instance with a specific detection function
		 * @note When `forceDetectionOnlyAndAllow2DCodes` is enabled, 2D detections of codes will be reported in addition to 3D detections. (Frame-to-frame 6DOF code tracking is also disabled.) A 2D code will have will have a negative code size and an invalid pose. The caller will have to add corresponding checks for that.
		 * @param parameters The parameters that will be used for tracking, must be valid
		 * @param callbackQRCodeDetection3D An optional pointer to the function that will be used for the detection of QR codes, if `nullptr` the default will be used
		 * @param callbackNewQRCode An optional callback function that is called whenever a QR code has been detected for the very first time, will be ignored if it is set to `nullptr`
		 * @param forceDetectionOnlyAndAllow2DCodes An optional parameter that disables tracking and only runs detection instead; this will also report 2D codes (which will have a negative size and an invalid pose; the user must check for that); use this if you only care about the payload of codes not their locations.
		 */
		QRCodeTracker3D(const Parameters& parameters, CallbackQRCodeDetection3D callbackQRCodeDetection3D = nullptr, CallbackNewQRCode callbackNewQRCode = nullptr, const bool forceDetectionOnlyAndAllow2DCodes = false);

		/**
		 * Tracks QR codes and their 6-DOF pose in two or more 8-bit grayscale images
		 * @note When `forceDetectionOnlyAndAllow2DCodes` is enabled, 2D detections of codes will be reported as well. A 2D code will have will have a negative code size and an invalid pose. The caller will have to add corresponding checks for that.
		 * @param sharedAnyCameras The cameras that produced the input images, must have the same number of elements as `yFrames`, all elements must be valid, number of cameras must remain identical on subsequent calls.
		 * @param yFrames The frames in which QR codes will be detected, must be valid, a minimum 2 frames must be provided, origin must be in the upper left corner, and have a pixel format that is compatible with Y8, minimum size is 29 x 29 pixels
		 * @param world_T_device The transformation that maps points in the device coordinate system points to world points, must be valid
		 * @param device_T_cameras The transformation that converts points in the camera coordinate systems to device coordinates, `devicePoint = device_T_cameras[i] * cameraPoint`, must have the same number of elements as `yFrames`, all elements must be valid
		 * @param worker Optional worker instance for parallelization
		 * @return The map containing all currently tracked QR code
		 */
		const TrackedQRCodesMap& trackQRCodes(const SharedAnyCameras& sharedAnyCameras, const Frames& yFrames, const HomogenousMatrix4& world_T_device, const HomogenousMatrices4& device_T_cameras, Worker* worker = nullptr);

		/**
		 * Returns if the tracker is in detection-only mode and will report 2D codes as well
		 * @note If this function returns true, 2D detections of codes will be reported as well. A 2D code will have will have a negative code size and an invalid pose. The caller will have to add corresponding checks for that.
		 * @return True if so, otherwise false
		 */
		inline bool isForceDetectionOnlyAndAllow2DCodesEnabled() const;

		/**
		 * Returns an invalid object ID.
		 * @return An invalid object ID.
		 */
		static constexpr ObjectId invalidObjectId();

	protected:

		/**
		 * Tracks a single QR code from one frame to the next
		 * @param previousSharedAnyCameraA The first camera that produced the observation in the previous frame, must be valid
		 * @param previousSharedAnyCameraB The second camera that produced the observation in the previous frame, must be valid
		 * @param previousWorld_T_device The transformation from the previous frame that maps points in the device coordinate system points to world points, must be valid
		 * @param previousDevice_T_cameraA The transformation from the previous frame that converts points in the coordinate systems of the first camera to device coordinates, must be valid
		 * @param previousDevice_T_cameraB The transformation from the previous frame converts points in the coordinate systems of the first camera to device coordinates, must be valid
		 * @param sharedAnyCameraA The first camera that produced the observation in the current frame, must be valid
		 * @param sharedAnyCameraB The second camera that produced the observation in the current frame, must be valid
		 * @param world_T_device The transformation from the current frame that maps points in the device coordinate system points to world points, must be valid
		 * @param device_T_cameraA The transformation from the current frame that converts points in the coordinate systems of the first camera to device coordinates, must be valid
		 * @param device_T_cameraB The transformation from the current frame that converts points in the coordinate systems of the first camera to device coordinates, must be valid
		 * @param previousFramePyramidA The first image pyramid from the previous frame, must be valid
		 * @param previousFramePyramidB The second image pyramid from the previous frame, must be valid
		 * @param framePyramidA The first image pyramid from the current frame, must be valid
		 * @param framePyramidB The second image pyramid from the current frame, must be valid
		 * @param trackingTimestamp The time stamp that will be attributed to the current tracking attempt, must be valid
		 * @param trackedCode The code that will be tracked. This instance will be updated by this function, must be valid
		 * @return True if the code was successfully tracked from the previous to the current frame, otherwise false
		 */
		static bool trackQRCode(const SharedAnyCamera& previousSharedAnyCameraA, const SharedAnyCamera& previousSharedAnyCameraB, const HomogenousMatrix4& previousWorld_T_device, const HomogenousMatrix4& previousDevice_T_cameraA, const HomogenousMatrix4& previousDevice_T_cameraB, const SharedAnyCamera& sharedAnyCameraA, const SharedAnyCamera& sharedAnyCameraB, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_cameraA, const HomogenousMatrix4& device_T_cameraB, const CV::FramePyramid& previousFramePyramidA, const CV::FramePyramid& previousFramePyramidB, const CV::FramePyramid& framePyramidA, const CV::FramePyramid& framePyramidB, const Timestamp& trackingTimestamp, TrackedQRCode& trackedCode);

		/**
		 * Checks if a specified code is already stored in the database of tracked QR codes.
		 * @param trackedQRCodesMap The map containing all currently tracked QR codes.
		 * @param code The code for which will be searched for in the database, must be valid.
		 * @param world_T_code The 6DOF pose of the code that will be searched for, must be valid.
		 * @param codeSize The size of the code in the physical world, range: (0, infinity)
		 * @param objectId The returning object ID of the code that is identical to the specified one and which already tracked, will `QRCodeTracker3D::invalidObjectId()` if this function return `false`.
		 * @return True the specified code is already tracked, otherwise false.
		 */
		static bool isAlreadyTracked(const TrackedQRCodesMap& trackedQRCodesMap, const CV::Detector::QRCodes::QRCode& code, const HomogenousMatrix4& world_T_code, const Scalar codeSize, ObjectId& objectId);

		/**
		 * Creates objects points for a code that can be used for tracking
		 * @param code The code for which the tracking object points will be generated, must be valid
		 * @param codeSize The (display) size of the code in the real world (in meters), range: (0, infinity)
		 * @return The object points, will be empty on failure
		 */
		static Geometry::ObjectPoints createTrackingObjectPoints(const CV::Detector::QRCodes::QRCode& code, const Scalar codeSize);

		/**
		 * Creates the object-image point pairs that can be used for tracking
		 * @param sharedAnyCamera The camera that produced the observation in the frame, must be valid
		 * @param yFrame The frame which contains the image of the code, must be valid
		 * @param world_T_camera The 6DOF pose of the camera in world coordinates, must be valid
		 * @param world_T_code The 6DOF pose of the code that will be used to project the code, must be valid
		 * @param refineCorners True, a subpixel refinement will be applied to the image points, otherwise the points will be used as-is
		 * @param potentialObjectPoints The object points that will considered, must be valid
		 * @param objectPoints The resulting object points (a true subset of `potentialObjectPoints`), size is identical to that of `imagePoints`
		 * @param imagePoints The resulting image points that correspond to the object points, size is identical to that of `objectPoints`
		 * @return True if the point pairs have been created, otherwise false
		 * @sa createTrackingObjectPoints()
		 */
		static bool createTrackingImagePoints(const SharedAnyCamera& sharedAnyCamera, const Frame& yFrame, const HomogenousMatrix4& world_T_camera, const HomogenousMatrix4& world_T_code, const bool refineCorners, const Geometry::ObjectPoints& potentialObjectPoints, Geometry::ObjectPoints& objectPoints, Geometry::ImagePoints& imagePoints);

		/**
		 * Returns an invalid map for tracked code
		 * @return The invalid map
		 */
		static const TrackedQRCodesMap& invalidTrackedQRCodesMap();

		/**
		 * Disabled copy constructor.
		 */
		QRCodeTracker3D(const QRCodeTracker3D&) = delete;

		/**
		 * Disabled assign operator.
		 * @return The reference to this object
		 */
		QRCodeTracker3D operator=(const QRCodeTracker3D&) = delete;

	protected:

		/// The function pointer to the function that provides new 6DOF detections of QR codes.
		CallbackQRCodeDetection3D callbackQRCodeDetection3D_ = CV::Detector::QRCodes::QRCodeDetector3D::detectQRCodesWithPyramids;

		/// The function pointer that is called in the event a new QR code is detected for the first time.
		CallbackNewQRCode callbackNewQRCode_ = nullptr;

		/// The tracking parameters.
		Parameters parameters_;

		/// The database of all tracked QR codes.
		TrackedQRCodesMap trackedQRCodesMap_;

		/// The counter that is used for the assignment of ID to new codes.
		ObjectId objectIdCounter_ = 0u;

		/// The counter for frames that have been processed.
		unsigned int frameCounter_ = 0u;

		/// The cameras from the previous frame/time step.
		SharedAnyCameras previousSharedAnyCameras_;

		/// The frames (frame pyramids) from the previous time step.
		std::vector<CV::FramePyramid> previousFramePyramids_;

		/// The device poses from the previous time step.
		HomogenousMatrix4 previousWorld_T_device_ = HomogenousMatrix4(false);

		/// The camera poses from the previous time step.
		HomogenousMatrices4 previousDevice_T_cameras_;

		/// Will disable tracking and run detection only; will also report back 2D codes if a 6-DOF pose is not available.
		bool forceDetectionOnlyAndAllow2DCodes_ = false;
};

inline const CV::Detector::QRCodes::QRCode& QRCodeTracker3D::TrackedQRCode::code() const
{
	return code_;
}

inline const HomogenousMatrix4& QRCodeTracker3D::TrackedQRCode::world_T_code() const
{
	return world_T_code_;
}

inline Scalar QRCodeTracker3D::TrackedQRCode::codeSize() const
{
	return codeSize_;
}

inline QRCodeTracker3D::TrackingState QRCodeTracker3D::TrackedQRCode::trackingState() const
{
	return trackingState_;
}

inline const Timestamp& QRCodeTracker3D::TrackedQRCode::trackingTimestamp() const
{
	return trackingTimestamp_;
}

inline bool QRCodeTracker3D::TrackedQRCode::isValid() const
{
	return code_.isValid() && world_T_code_.isValid() && codeSize_ > Scalar(0) && trackingState_ != TS_UNKNOWN_STATE && trackingTimestamp_.isValid() && trackingObjectPoints_.size() >= 3;
}

inline const Geometry::ObjectPoints& QRCodeTracker3D::TrackedQRCode::trackingObjectPoints() const
{
	return trackingObjectPoints_;
}

void QRCodeTracker3D::TrackedQRCode::updateTrackingPose(HomogenousMatrix4&& world_T_code, const Scalar codeSize, const Timestamp trackingTimestamp)
{
	if (world_T_code.isValid() && codeSize > Scalar(0) && trackingTimestamp.isValid() && (!trackingTimestamp_.isValid() || trackingTimestamp >= trackingTimestamp_))
	{
		world_T_code_ = std::move(world_T_code);
		codeSize_ = codeSize;
		trackingTimestamp_ = trackingTimestamp;

		trackingState_ = TS_TRACKING;
	}
	else
	{
		Log::error() << "Failed to updated the tracking pose!";
		ocean_assert(false && "Failed to updated the tracking pose!");
	}
}

void QRCodeTracker3D::TrackedQRCode::setTrackingLost()
{
	// world_T_code_, codeSize_ - not invalidating since they could still be useful
	trackingState_ = TS_LOST;
}

inline QRCodeTracker3D::ObservationHistories& QRCodeTracker3D::TrackedQRCode::observationHistories()
{
	return observationHistories_;
}

inline bool QRCodeTracker3D::isForceDetectionOnlyAndAllow2DCodesEnabled() const
{
	return forceDetectionOnlyAndAllow2DCodes_;
}

} // namespace QRCodes

} // namespace Tracking

} // namespace Ocean
