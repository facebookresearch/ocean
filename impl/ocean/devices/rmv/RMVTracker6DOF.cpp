/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/rmv/RMVTracker6DOF.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/String.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameShrinker.h"
#include "ocean/cv/IntegralImage.h"

#include "ocean/io/CameraCalibrationManager.h"
#include "ocean/io/File.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Random.h"
#include "ocean/math/Rotation.h"

#include "ocean/media/FrameMedium.h"
#include "ocean/media/Manager.h"
#include "ocean/media/Utilities.h"

#include "ocean/tracking/rmv/RMVFeatureTracker6DOF.h"

namespace Ocean
{

namespace Devices
{

namespace RMV
{

using namespace Ocean::Tracking::RMV;

const Tracking::RMV::RMVFeatureDetector::DetectorType detectorType = Tracking::RMV::RMVFeatureDetector::DT_FAST_FEATURE;

RMVTracker6DOF::RMVTracker6DOF() :
	Device(deviceNameRMVTracker6DOF(), deviceTypeRMVTracker6DOF()),
	RMVDevice(deviceNameRMVTracker6DOF(), deviceTypeRMVTracker6DOF()),
	Measurement(deviceNameRMVTracker6DOF(), deviceTypeRMVTracker6DOF()),
	Tracker(deviceNameRMVTracker6DOF(), deviceTypeRMVTracker6DOF()),
	OrientationTracker3DOF(deviceNameRMVTracker6DOF()),
	PositionTracker3DOF(deviceNameRMVTracker6DOF()),
	Tracker6DOF(deviceNameRMVTracker6DOF()),
	ObjectTracker(deviceNameRMVTracker6DOF(), deviceTypeRMVTracker6DOF()),
	VisualTracker(deviceNameRMVTracker6DOF(), deviceTypeRMVTracker6DOF()),
	featureTracker_(detectorType)
{
	// nothing do to here
}

RMVTracker6DOF::~RMVTracker6DOF()
{
	stopThread();
	sleep(100);

	stopThreadExplicitly();
}

RMVTracker6DOF::ObjectId RMVTracker6DOF::registerObject(const std::string& description, const Vector3& dimension)
{
	const ScopedLock scopedLock(deviceLock);

	const IO::File file(description);

	if (!file.exists())
	{
		return invalidObjectId();
	}

	ocean_assert(dimension.x() > 0);

	if (dimension.x() <= 0)
	{
		Log::error() << "Invalid feature map dimension!";
		return invalidObjectId();
	}

	const Frame pattern = Media::Utilities::loadImage(description);

	if (!pattern.isValid())
	{
		Log::error() << "The defined tracking object \"" << description << "\" holds no image data.";
		return invalidObjectId();
	}

	if (frameMediums_.size() != 1 || frameMediums_.front().isNull())
	{
		return false;
	}

	const Media::FrameMedium& frameMedium = *frameMediums_.front();

	Log::info() << "Creating camera: " << frameMedium.url();

	PinholeCamera pinholeCamera = IO::CameraCalibrationManager::get().camera(frameMedium.url(), pattern.width(), pattern.height());

	Log::info() << "Standard camera resolution: " << pinholeCamera.width() << "x" << pinholeCamera.height();

	if (pinholeCamera)
	{
		const WorkerPool::ScopedWorker worker(WorkerPool::get().scopedWorker());

		Tracking::RMV::RMVFeatureMap featureMap;

#ifdef OCEAN_HARDWARE_REDUCED_PERFORMANCE

		Tracking::RMV::RMVFeatureDetector::DetectorType initilizationDetectorType = Tracking::RMV::RMVFeatureDetector::DT_HARRIS_FEATURE;

		const unsigned int numberMapPoints = 300u;
		const unsigned int numberInitializationPoints = 120u;

#else

		Tracking::RMV::RMVFeatureDetector::DetectorType initilizationDetectorType = Tracking::RMV::RMVFeatureDetector::DT_HARRIS_FEATURE;

		const unsigned int numberMapPoints = 600u;
		const unsigned int numberInitializationPoints = 150u;

#endif

		featureMap.setFeatures(pattern, dimension, pinholeCamera, numberMapPoints, featureTracker_.detectorType(), worker());
		featureMap.setInitializationFeatures(pattern, dimension, pinholeCamera, numberInitializationPoints, initilizationDetectorType, worker());

		ocean_assert(uniqueObjectId_ == invalidObjectId());
		uniqueObjectId_ = addUniqueObjectId("RMV");

		return uniqueObjectId_;
	}

	Log::error() << "FAILED to create a feature map for the RMV tracker";

	return invalidObjectId();
}

bool RMVTracker6DOF::isStarted() const
{
	const ScopedLock scopedLock(deviceLock);

	return isThreadActive();
}

bool RMVTracker6DOF::start()
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

bool RMVTracker6DOF::stop()
{
	const ScopedLock scopedLock(deviceLock);

	stopThread();
	return true;
}

bool RMVTracker6DOF::isObjectTracked(const ObjectId& /*objectId*/) const
{
	const ScopedLock scopedLock(deviceLock);

	ocean_assert(false && "Missing implementation!");
	return false;
}

void RMVTracker6DOF::threadRun()
{
	TemporaryScopedLock temporaryScopedLock(deviceLock);
		if (frameMediums_.size() != 1 || frameMediums_.front().isNull())
		{
			return;
		}

		const Media::FrameMediumRef frameMedium = frameMediums_.front();
	temporaryScopedLock.release();

	Log::info() << deviceNameRMVTracker6DOF() << " started...";

	Random::initialize();
	bool trackedPreviousFrame = false;

	while (shouldThreadStop() == false)
	{
		SharedAnyCamera camera;
		const FrameRef frame = frameMedium->frame(&camera);

		if (frame && *frame && frame->timestamp() > frameTimestamp_ && camera && camera->isValid())
		{
			frameTimestamp_ = frame->timestamp();

			static Ocean::HighPerformanceStatistic performance;
			performance.start();

			const Frames frames(1, Frame(*frame, Frame::ACM_USE_KEEP_LAYOUT));
			const SharedAnyCameras cameras(1, std::move(camera));

			Tracking::VisualTracker::TransformationSamples transformationSamples;
			if (featureTracker_.determinePoses(frames, cameras, transformationSamples, Quaternion(false), WorkerPool::get().scopedWorker()()) && !transformationSamples.empty())
			{
				performance.stop();

				if (performance.measurements() % 50 == 0)
				{
					Log::info() << "Tracker performance: " << performance.averageMseconds();
					performance.reset();
				}

				const HomogenousMatrix4& object_T_camera = transformationSamples.front().transformation();

				if (trackedPreviousFrame == false)
				{
					postFoundTrackerObjects({uniqueObjectId_}, frameTimestamp_);
				}

				trackedPreviousFrame = true;

				postNewSample(SampleRef(new Tracker6DOFSample(frameTimestamp_, RS_DEVICE_IN_OBJECT, {uniqueObjectId_}, {object_T_camera.rotation()}, {object_T_camera.translation()})));
				continue;
			}
			else
			{
				performance.stop();
			}

			if (trackedPreviousFrame)
			{
				postLostTrackerObjects({uniqueObjectId_}, frameTimestamp_);
				trackedPreviousFrame = false;
			}
		}
		else
		{
			sleep(1);
		}
	}

	postLostTrackerObjects({uniqueObjectId_}, Timestamp(true));

	Log::info() << deviceNameRMVTracker6DOF() << " stopped...";
}

}

}

}
