/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/slam/SLAMTracker6DOF.h"

#include "ocean/devices/GravityTracker3DOF.h"
#include "ocean/devices/Manager.h"
#include "ocean/devices/OrientationTracker3DOF.h"

#include "ocean/media/Utilities.h"

namespace Ocean
{

namespace Devices
{

namespace SLAM
{

SLAMTracker6DOF::SLAMTracker6DOF() :
	Device(deviceNameSLAMTracker6DOF(),  deviceTypeSLAMTracker6DOF()),
	SLAMDevice(deviceNameSLAMTracker6DOF(), deviceTypeSLAMTracker6DOF()),
	Measurement(deviceNameSLAMTracker6DOF(), deviceTypeSLAMTracker6DOF()),
	Tracker(deviceNameSLAMTracker6DOF(), deviceTypeSLAMTracker6DOF()),
	OrientationTracker3DOF(deviceNameSLAMTracker6DOF()),
	PositionTracker3DOF(deviceNameSLAMTracker6DOF()),
	Tracker6DOF(deviceNameSLAMTracker6DOF()),
	VisualTracker(deviceNameSLAMTracker6DOF(), deviceTypeSLAMTracker6DOF())
{
	uniqueObjectId_ = addUniqueObjectId("SLAM World");
}

SLAMTracker6DOF::~SLAMTracker6DOF()
{
	stopThreadExplicitly();
}

bool SLAMTracker6DOF::isStarted() const
{
	const ScopedLock scopedLock(deviceLock);

	return isThreadActive();
}

bool SLAMTracker6DOF::start()
{
	const ScopedLock scopedLock(deviceLock);

	if (frameMediums_.size() != 1 || frameMediums_.front().isNull())
	{
		return false;
	}

	if (isThreadActive())
	{
		return true;
	}

	startThread();

	return true;
}

bool SLAMTracker6DOF::stop()
{
	const ScopedLock scopedLock(deviceLock);

	stopThread();
	return true;
}

bool SLAMTracker6DOF::setParameter(const std::string& name, const Value& value)
{
	if (name != "preferredResolution" || !value.isString())
	{
		return false;
	}

	const ScopedLock scopedLock(deviceLock);

	if (isStarted())
	{
		return false;
	}

	if (Media::Utilities::parseResolution(value.stringValue(), preferredCameraWidth_, preferredCameraHeight_))
	{
		return true;
	}

	Log::error() << "SLAMTracker6DOF: Failed to set preferred frame dimension";
	return false;
}

bool SLAMTracker6DOF::isObjectTracked(const ObjectId& objectId) const
{
	if (isObjectTracked_)
	{
		const ScopedLock scopedLock(deviceLock);

		return objectId == uniqueObjectId_;
	}

	return false;
}

void SLAMTracker6DOF::threadRun()
{
	TemporaryScopedLock temporaryScopedLock(deviceLock);
		if (frameMediums_.size() != 1 || frameMediums_.front().isNull())
		{
			return;
		}

		const Media::FrameMediumRef frameMedium = frameMediums_.front();
	temporaryScopedLock.release();

	ocean_assert(frameMedium);

	if (preferredCameraWidth_ != 0u && preferredCameraHeight_ != 0u)
	{
		if (!frameMedium->setPreferredFrameDimension(preferredCameraWidth_, preferredCameraHeight_))
		{
			Log::warning() << "SLAMTracker6DOF: Failed to set preferred frame dimension";
		}
	}

	if (!frameMedium->start())
	{
		Log::error() << "SLAMTracker6DOF: Failed to start the frame medium";
		return;
	}

	Log::debug() << deviceNameSLAMTracker6DOF() << " started...";

	const Devices::GravityTracker3DOFRef gravityTracker = Devices::Manager::get().device(Devices::GravityTracker3DOF::deviceTypeGravityTracker3DOF());

	if (gravityTracker && gravityTracker->start())
	{
		Log::info() << "SLAMTracker6DOF: Gravity tracker started";
	}
	else
	{
		Log::warning() << "SLAMTracker6DOF: No gravity tracker available";
	}

	const Devices::OrientationTracker3DOFRef orientationTracker = Devices::Manager::get().device(Devices::OrientationTracker3DOF::deviceTypeOrientationTracker3DOF());

	if (orientationTracker && orientationTracker->start())
	{
		Log::info() << "SLAMTracker6DOF: Orientation tracker started";
	}
	else
	{
		Log::warning() << "SLAMTracker6DOF: No orientation tracker available";
	}

	const Quaternion device_Q_camera = Quaternion(frameMedium->device_T_camera().rotation());

	ocean_assert(device_Q_camera.isValid());
	const Quaternion camera_Q_device = device_Q_camera.inverted();

	SharedAnyCamera camera;
	Timestamp frameTimestamp(false);

	HomogenousMatrix4 world_T_previousCamera(false);

	while (shouldThreadStop() == false)
	{
		const FrameRef frame = frameMedium->frame(&camera);

		if (frame.isNull() || !frame->isValid() || frame->timestamp() <= frameTimestamp)
		{
			Thread::sleep(1u);
			continue;
		}

		if (!camera || !camera->isValid())
		{
			ocean_assert(false && "This should never happen!");
			sleep(1u);
			continue;
		}

		frameTimestamp = frame->timestamp();

		Frame yFrame;
		if (!CV::FrameConverter::Comfort::convert(*frame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE))
		{
			ocean_assert(false && "This should never happen!");
			continue;
		}

		if (preferredCameraWidth_ != 0u && preferredCameraHeight_ != 0u)
		{
			if (yFrame.width() != preferredCameraWidth_ || yFrame.height() != preferredCameraHeight_)
			{
				while (yFrame.pixels() / 4u >= preferredCameraWidth_ * preferredCameraHeight_)
				{
					CV::FrameShrinker::downsampleByTwo11(yFrame);
				}
			}
		}

		if (camera->width() != yFrame.width() || camera->height() != yFrame.height())
		{
			camera = camera->clone(yFrame.width(), yFrame.height());

			if (!camera)
			{
				Log::error() << "Failed to adjust the camera profile to the frame resolution";
				continue;
			}
		}

		Vector3 cameraGravity(false);
		Quaternion anyWorld_Q_camera(false);

		if (gravityTracker)
		{
			const Devices::GravityTracker3DOF::GravityTracker3DOFSampleRef sample = gravityTracker->sample(frameTimestamp, Devices::Measurement::IS_TIMESTAMP_INTERPOLATE);

			if (sample && sample->timestamp() == frameTimestamp)
			{
				ocean_assert(sample->gravities().size() == 1);
				ocean_assert(sample->referenceSystem() == Devices::Tracker::RS_OBJECT_IN_DEVICE);

				if (sample->gravities().size() == 1 && sample->referenceSystem() == Devices::Tracker::RS_OBJECT_IN_DEVICE)
				{
					const Vector3 deviceGravity = sample->gravities().front();

					cameraGravity = camera_Q_device * deviceGravity;
				}
			}
		}

		if (orientationTracker)
		{
			const Devices::OrientationTracker3DOF::OrientationTracker3DOFSampleRef sample = orientationTracker->sample(frameTimestamp, Devices::Measurement::IS_TIMESTAMP_INTERPOLATE);

			if (sample && sample->timestamp() == frameTimestamp)
			{
				ocean_assert(sample->orientations().size() == 1);
				ocean_assert(sample->referenceSystem() == Devices::Tracker::RS_DEVICE_IN_OBJECT);

				if (sample->orientations().size() == 1 && sample->referenceSystem() == Devices::Tracker::RS_DEVICE_IN_OBJECT)
				{
					const Quaternion anyWorld_Q_device = sample->orientations().front();

					anyWorld_Q_camera = anyWorld_Q_device * device_Q_camera;
				}
			}
		}

		HomogenousMatrix4 world_T_camera(false);

		trackerMono_.handleFrame(*camera, std::move(yFrame), world_T_camera, cameraGravity, anyWorld_Q_camera);

		if (!world_T_previousCamera.isValid() && world_T_camera.isValid())
		{
			isObjectTracked_ = true;

			postFoundTrackerObjects({uniqueObjectId_}, frameTimestamp);
		}
		else if (world_T_previousCamera.isValid() && !world_T_camera.isValid())
		{
			postLostTrackerObjects({uniqueObjectId_}, frameTimestamp);

			isObjectTracked_ = false;
		}

		if (world_T_camera.isValid())
		{
			postPose(world_T_camera, frameTimestamp);
		}

		world_T_previousCamera = world_T_camera;
	}

	postLostTrackerObjects({uniqueObjectId_}, Timestamp(true));
	isObjectTracked_ = false;

	Log::info() << deviceNameSLAMTracker6DOF() << " stopped...";
}

void SLAMTracker6DOF::postPose(const HomogenousMatrix4& world_T_camera, const Timestamp& timestamp)
{
	const ObjectIds objectIds(1, uniqueObjectId_);
	const Tracker6DOFSample::Positions positions(1, world_T_camera.translation());
	const Tracker6DOFSample::Orientations orientations(1, world_T_camera.rotation());

	postNewSample(SampleRef(new Tracker6DOFSample(timestamp, RS_DEVICE_IN_OBJECT, objectIds, orientations, positions)));
}

}

}

}
