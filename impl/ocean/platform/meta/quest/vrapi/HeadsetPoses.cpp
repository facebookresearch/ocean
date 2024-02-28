// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/platform/meta/quest/vrapi/HeadsetPoses.h"
#include "ocean/platform/meta/quest/vrapi/Utilities.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace VrApi
{

/// As tracking samples may arrive with 1000Hz, we keep a pose history of 1 second.
constexpr size_t sampleMapCapacity = 1000;

HeadsetPoses::HeadsetPoses() :
	sampleMap_world_T_device_(sampleMapCapacity),
	sampleMapPreviousTimestamp_(false),
	ovrMobile_(nullptr)
{
	// nothing to do here
}

bool HeadsetPoses::start(ovrMobile* ovr)
{
	ocean_assert(ovrMobile_ == nullptr);
	ocean_assert(ovr != nullptr);

	const ScopedLock scopedLock(lock_);

	if (ovrMobile_ != nullptr || ovr == nullptr)
	{
		// the object has been started already or the provided OVR SDK pointer is invalid
		return false;
	}

	ovrMobile_ = ovr;

	startThread();

	return true;
}

void HeadsetPoses::stop()
{
	const ScopedLock scopedLock(lock_);

	if (ovrMobile_ == nullptr)
	{
		return;
	}

	stopThreadExplicitly(100u /* timeout 100ms, while we expect < 1ms */);

	sampleMap_world_T_device_.clear();
	sampleMapPreviousTimestamp_.toInvalid();
	ovrMobile_ = nullptr;
}

bool HeadsetPoses::interpolatedPose(const Timestamp& hostTimestamp, HomogenousMatrix4& world_T_device) const
{
	ocean_assert(ovrMobile_ != nullptr);

	const ScopedLock scopedLock(lock_);

	return sampleMap_world_T_device_.sample(double(hostTimestamp), PosesMap::IS_TIMESTAMP_INTERPOLATE, world_T_device);
}

HomogenousMatrix4 HeadsetPoses::world_T_floor(ovrMobile* ovr)
{
	ocean_assert(ovr != nullptr);

	const ovrTrackingSpace oldTrackingSpace = vrapi_GetTrackingSpace(ovr);

	if (oldTrackingSpace != VRAPI_TRACKING_SPACE_LOCAL)
	{
		vrapi_SetTrackingSpace(ovr, VRAPI_TRACKING_SPACE_LOCAL);
	}

	const ovrPosef pose = vrapi_LocateTrackingSpace(ovr, VRAPI_TRACKING_SPACE_LOCAL_FLOOR);

	if (oldTrackingSpace != VRAPI_TRACKING_SPACE_LOCAL)
	{
		vrapi_SetTrackingSpace(ovr, oldTrackingSpace);
	}

	return Utilities::toHomogenousMatrix4<Scalar>(pose);
}

HomogenousMatrix4 HeadsetPoses::world_T_device(ovrMobile* ovr, const Timestamp& hostTimestamp)
{
	ocean_assert(ovr != nullptr);
	ocean_assert(hostTimestamp.isValid());

	ovrTracking2 tracking = vrapi_GetPredictedTracking2(ovr, double(hostTimestamp));

	constexpr unsigned int validTrackingStatus = VRAPI_TRACKING_STATUS_ORIENTATION_VALID | VRAPI_TRACKING_STATUS_POSITION_VALID;

	if ((tracking.Status & validTrackingStatus) == validTrackingStatus)
	{
		return Utilities::toHomogenousMatrix4<Scalar>(tracking.HeadPose.Pose);
	}

	return HomogenousMatrix4(false);
}

void HeadsetPoses::threadRun()
{
	ocean_assert(ovrMobile_ != nullptr);
	ocean_assert(sampleMapPreviousTimestamp_.isInvalid());

	while (!shouldThreadStop())
	{
		const double absTimeInSeconds = 0.0; // VrApi documentation: Pass absTime value of 0.0 to request the most recent sensor reading
		ovrTracking2 tracking = vrapi_GetPredictedTracking2(ovrMobile_, absTimeInSeconds);

		if (tracking.HeadPose.TimeInSeconds > 0.0 && tracking.HeadPose.TimeInSeconds != double(sampleMapPreviousTimestamp_))
		{
			const HomogenousMatrix4 world_T_device = Utilities::toHomogenousMatrix4<Scalar>(tracking.HeadPose.Pose);

			const ScopedLock scopedLock(lock_);

			sampleMap_world_T_device_.insert(world_T_device, tracking.HeadPose.TimeInSeconds);
			sampleMapPreviousTimestamp_ = Timestamp(tracking.HeadPose.TimeInSeconds);
		}

		// reducing CPU load by sleeping 1ms
		sleep(1u);
	}
}

}

}

}

}

}
