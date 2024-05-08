/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// clang-format off
// @nolint

#include "application/ocean/demo/tracking/pointtracker/meta/quest/openxr/PointTrackerApplication.h"

#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/geometry/RANSAC.h"

#include "ocean/media/Manager.h"

#include "ocean/rendering/Scene.h"
#include "ocean/rendering/Utilities.h"

#include "ocean/tracking/Utilities.h"

PointTrackerApplication::PointTrackerApplication(struct android_app* androidApp) :
	VRNativeApplicationAdvanced(androidApp)
{
	requestAndroidPermission("com.oculus.permission.ACCESS_MR_SENSOR_DATA");
}

void PointTrackerApplication::onAndroidPermissionGranted(const std::string& permission)
{
	VRNativeApplicationAdvanced::onAndroidPermissionGranted(permission);

	if (permission == "com.oculus.permission.ACCESS_MR_SENSOR_DATA")
	{
		frameProvider_.initialize(Platform::Meta::Quest::Sensors::FrameProvider::FCM_MAKE_COPY);

		OSSDK::Sensors::v3::FrameType cameraFrameType;

		if (deviceType() == Platform::Meta::Quest::Device::DT_QUEST_PRO || deviceType() == Platform::Meta::Quest::Device::DT_QUEST_3)
		{
			cameraFrameType = OSSDK::Sensors::v3::FrameType::GenericSlot0;
			imageRotation_ = -Numeric::pi_2();
		}
		else if (deviceType() == Platform::Meta::Quest::Device::DT_QUEST_2)
		{
			cameraFrameType = OSSDK::Sensors::v3::FrameType::Headset;
			imageRotation_ = Numeric::pi_2();
		}
		else
		{
			Log::error() << "Unsupported device type: " << deviceName();
			return;
		}

		constexpr Platform::Meta::Quest::Sensors::FrameProvider::CameraType cameraType = Platform::Meta::Quest::Sensors::FrameProvider::CT_LOWER_STEREO_CAMERAS;

		if (frameProvider_.isCameraFrameTypeAvailable(cameraFrameType))
		{
			frameProvider_.startReceivingCameraFrames(cameraFrameType, cameraType);
			startThread();
		}
	}
}

void PointTrackerApplication::threadRun()
{
	ocean_assert(frameProvider_.isValid());

	RandomGenerator randomGenerator;

	while (!shouldThreadStop())
	{
		if (!frameProvider_.isValid())
		{
			break;
		}

		Frames frames;
		SharedAnyCamerasD cameras;
		HomogenousMatrixD4 ossdkWorld_T_device;
		HomogenousMatricesD4 device_T_cameras;

		OSSDK::Sensors::v3::FrameType cameraFrameType = OSSDK::Sensors::v3::FrameType::Invalid;

		if (!frameProvider_.latestFrames(frames, &cameras, &ossdkWorld_T_device, &device_T_cameras, &cameraFrameType))
		{
			Thread::sleep(1u);
			continue;
		}

		const Frame& frame = frames.front();
		ocean_assert(frame.isValid());

		SharedAnyCamera camera;
		if (cameras.front()->width() == frame.width() && cameras.front()->height() == frame.height())
		{
			camera = AnyCamera::convert(cameras.front());
		}
		else
		{
			// Resize the camera to the frame dimensions. This wouldn't be necessary if the camera and frame always matched, like they should.
			Log::warning() << "Mismatch of frame size and camera calibration: device type = " << Platform::Meta::Quest::Device::deviceName(deviceType()) << ", camera = (" << cameras.front()->width() << " x " << cameras.front()->height() << "), frame = (" << frame.width() << " x " << frame.height() << ")";

			camera = AnyCamera::convert(cameras.front())->clone(frame.width(), frame.height());
		}

		ocean_assert(camera && camera->isValid());

		const HomogenousMatrix4 world_T_device = HomogenousMatrix4(ossdkWorld_T_device);
		const HomogenousMatrix4 device_T_camera = HomogenousMatrix4(device_T_cameras.front());
		const HomogenousMatrix4 world_T_camera = world_T_device * device_T_camera;

		if (!world_T_device.isValid())
		{
			Log::warning() << "Invalid OpenXR device pose";
			continue;
		}

		Frame displayLeft;

		if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_RGB24, displayLeft, true))
		{
			Log::error() << "Failed to convert frame!";
			ocean_assert(false && "This should never happen!");
			return;
		}

		Frame displayRight(displayLeft, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

		constexpr unsigned int maximalTrackLength = 40u;

		if (objectPoints_.empty())
		{
			// in case we do not have any 3D object points yet, we track 2D image points from frame to frame

			const unsigned int poseIndex = pointTracker_.newFrame(frame, WorkerPool::get().scopedWorker()());

			if (poseIndex != Tracking::Point::PointTracker::invalidFrameIndex)
			{
				pointTracker_.database().setPose<false>(poseIndex, world_T_camera);

				// we draw all Tracks (paths of connected image points) belonging to feature points visible in the current frame
				// very short point paths have a red color, while longer point paths have a green color, with interpolation between red and green for intermediate lengths

				const Tracking::Point::PointTracker::PointTracks pointTracks = pointTracker_.pointTracks(poseIndex, maximalTrackLength);

				unsigned int numberLongTracks = 0u;

				Scalars factors;
				factors.reserve(pointTracks.size());

				for (const Vectors2& pointTrack : pointTracks)
				{
					factors.push_back(Scalar(pointTrack.size()) / Scalar(maximalTrackLength));

					if (pointTrack.size() >= 25)
					{
						++numberLongTracks;
					}
				}

				// we draw the detected point paths
				Tracking::Utilities::paintPaths<3u>(displayLeft, pointTracks.data(), pointTracks.size(), CV::Canvas::red(), CV::Canvas::green(), factors.data(), WorkerPool::get().scopedWorker()());

				if (numberLongTracks >= 25u)
				{
					// we have more than 25 tracked feature points with enough observations
					// let's try to determine 3D object points for those observations

					Vectors3 objectPoints;

					const Tracking::Database& database = pointTracker_.database();

					const Tracking::Database::IdIdPointPairsMap featurePointsMap = database.imagePoints<false>(poseIndex, true, size_t(maximalTrackLength - 5u), size_t(maximalTrackLength));

					Vectors2 imagePoints;
					imagePoints.reserve(maximalTrackLength);

					HomogenousMatrices4 poses_world_T_camera;
					poses_world_T_camera.reserve(maximalTrackLength);

					for (Tracking::Database::IdIdPointPairsMap::const_iterator i = featurePointsMap.begin(); i != featurePointsMap.end(); ++i)
					{
						imagePoints.clear();
						poses_world_T_camera.clear();

						for (Tracking::Database::IdPointPairs::const_iterator iI = i->second.begin(); iI != i->second.end(); ++iI)
						{
							const Index32 imagePointId = iI->first;
							const Vector2& imagePoint = iI->second;

							imagePoints.push_back(imagePoint);

							const unsigned int poseIndexOfImagePoint = database.poseFromImagePoint<false>(imagePointId);

							const HomogenousMatrix4& pose = database.pose<false>(poseIndexOfImagePoint);
							poses_world_T_camera.push_back(pose);
						}

						ocean_assert(imagePoints.size() == poses_world_T_camera.size());

						Vector3 objectPoint;
						if (Geometry::RANSAC::objectPoint(*camera, ConstArrayAccessor<HomogenousMatrix4>(poses_world_T_camera), ConstArrayAccessor<Vector2>(imagePoints), randomGenerator, objectPoint, 40u))
						{
							objectPoints.push_back(objectPoint);
						}
					}

					if (objectPoints.size() >= 20)
					{
						// we have enough 3D object points, so that we accept all of them
						objectPoints_ = std::move(objectPoints);
					}
				}
			}

			if (poseIndex >= 40u)
			{
				// we prevent the database from growing infinitely
				pointTracker_.clearUpTo(poseIndex - 40u);
			}
		}
		else
		{
			const HomogenousMatrix4 flippedCamera_T_world = PinholeCamera::standard2InvertedFlipped(world_T_camera);

			for (const Vector3& objectPoint : objectPoints_)
			{
				if (PinholeCamera::isObjectPointInFrontIF(flippedCamera_T_world, objectPoint))
				{
					const Vector2 imagePoint = camera->projectToImageIF(flippedCamera_T_world, objectPoint);

					if (camera->isInside(imagePoint))
					{
						CV::Canvas::point<7u>(displayRight, imagePoint, CV::Canvas::green());
					}
				}
			}

			if (++frameCounter_ >= 30 * 10u)
			{
				// let's determine a new set of 3D object points

				objectPoints_.clear();
				pointTracker_.clear();

				frameCounter_ = 0u;
			}
		}

		const ScopedLock scopedLock(resultLock_);

		haveResults_ = true;
		displayLeft_ = std::move(displayLeft);
		displayRight_ = std::move(displayRight);
	}
}

void PointTrackerApplication::onPreRender(const XrTime& xrPredictedDisplayTime, const Timestamp& predictedDisplayTime)
{
	VRNativeApplicationAdvanced::onPreRender(xrPredictedDisplayTime, predictedDisplayTime);

	TemporaryScopedLock scopedResultLock(resultLock_);

	const bool haveResults = haveResults_;
	haveResults_ = false;

	const Frame displayLeft = std::move(displayLeft_);
	const Frame displayRight = std::move(displayRight_);

	scopedResultLock.release();

	if (haveResults)
	{
		ocean_assert(displayLeft.isValid() && displayRight.isValid() && displayLeft.frameType() == displayRight.frameType());
		vrImageVisualizer_.visualizeImageInView(0u /* a unique id*/, HomogenousMatrix4(Vector3(-0.16, 0, -0.5)) * HomogenousMatrix4(Rotation(0, 0, 1, imageRotation_)), displayLeft, Scalar(0.4) /* 40cm */);
		vrImageVisualizer_.visualizeImageInView(1u /* a unique id*/, HomogenousMatrix4(Vector3(0.16, 0, -0.5)) * HomogenousMatrix4(Rotation(0, 0, 1, imageRotation_)), displayRight, Scalar(0.4) /* 40cm */);
	}
}
