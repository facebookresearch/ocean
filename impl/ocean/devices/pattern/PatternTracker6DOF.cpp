/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/pattern/PatternTracker6DOF.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/String.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/devices/Manager.h"

#include "ocean/io/CameraCalibrationManager.h"
#include "ocean/io/File.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Random.h"
#include "ocean/math/Rotation.h"

#include "ocean/media/FrameMedium.h"
#include "ocean/media/Manager.h"
#include "ocean/media/Utilities.h"

namespace Ocean
{

namespace Devices
{

namespace Pattern
{

using namespace Ocean::Tracking::Pattern;

PatternTracker6DOF::PatternTracker6DOF() :
	Device(deviceNamePatternTracker6DOF(),  deviceTypePatternTracker6DOF()),
	PatternDevice(deviceNamePatternTracker6DOF(), deviceTypePatternTracker6DOF()),
	Measurement(deviceNamePatternTracker6DOF(), deviceTypePatternTracker6DOF()),
	Tracker(deviceNamePatternTracker6DOF(), deviceTypePatternTracker6DOF()),
	OrientationTracker3DOF(deviceNamePatternTracker6DOF()),
	PositionTracker3DOF(deviceNamePatternTracker6DOF()),
	Tracker6DOF(deviceNamePatternTracker6DOF()),
	ObjectTracker(deviceNamePatternTracker6DOF(), deviceTypePatternTracker6DOF()),
	VisualTracker(deviceNamePatternTracker6DOF(), deviceTypePatternTracker6DOF()),
	objectIdMapper_(*this)
{
	// nothing do to here
}

PatternTracker6DOF::~PatternTracker6DOF()
{
	stopThreadExplicitly();
}

PatternTracker6DOF::ObjectId PatternTracker6DOF::registerObject(const std::string& description, const Vector3& dimension)
{
	ocean_assert(dimension.x() > 0);

	if (dimension.x() <= 0)
	{
		Log::error() << "Invalid feature map dimension!";
		return invalidObjectId();
	}

	const IO::File file(description);

	if (!file.exists())
	{
		Log::info() << "The pattern tracker needs an existing file as pattern.";
		return invalidObjectId();
	}

	const ScopedLock scopedLock(deviceLock);

	ensureTrackerExists();

	if (String::toLower(file.extension()) == std::string("opfm"))
	{
		const unsigned int internalObjectId = visualTracker_.force<Tracking::Pattern::PatternTracker6DOF>().addPattern(description, dimension.xy(), WorkerPool::get().scopedWorker()());

		if (internalObjectId == (unsigned int)(-1))
		{
			return invalidObjectId();
		}

		return objectIdMapper_.newInternalObjectId(internalObjectId, description);
	}
	else
	{
		Frame frame(Media::Utilities::loadImage(description));

		if (!frame.isValid())
		{
			Log::error() << "The defined tracking object \"" << description << "\" holds no image data.";
			return invalidObjectId();
		}

		if (!CV::FrameConverter::Comfort::change(frame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, false))
		{
			Log::error() << "The defined tracking object \"" << description << "\" holds no usable image data.";
			return invalidObjectId();
		}

		const unsigned int internalObjectId = visualTracker_.force<Tracking::Pattern::PatternTracker6DOF>().addPattern(frame, dimension.xy(), WorkerPool::get().scopedWorker()());

		if (internalObjectId == (unsigned int)(-1))
		{
			return invalidObjectId();
		}

		return objectIdMapper_.newInternalObjectId(internalObjectId, description);
	}
}

bool PatternTracker6DOF::isStarted() const
{
	return isThreadActive();
}

bool PatternTracker6DOF::start()
{
	const ScopedLock scopedLock(deviceLock);

	if (frameMediums_.size() != 1 || frameMediums_.front().isNull())
	{
		return false;
	}

	if (orientationTracker3DOF_.isNull())
	{
#ifdef __APPLE__
		orientationTracker3DOF_ = Manager::get().device("IOS 3DOF Orientation Tracker");
#elif defined(_ANDROID)
		orientationTracker3DOF_ = Manager::get().device("Android 3DOF Orientation Tracker");
#endif
	}

	if (orientationTracker3DOF_)
	{
		orientationTracker3DOF_->start();
	}

	if (isThreadActive())
	{
		return true;
	}

	startThread();

	Log::info() << "6DOF Pattern Feature tracker started.";

	return true;
}

bool PatternTracker6DOF::stop()
{
	if (orientationTracker3DOF_)
	{
		orientationTracker3DOF_->stop();
	}

	stopThread();
	return true;
}

bool PatternTracker6DOF::isObjectTracked(const ObjectId& objectId) const
{
	const ScopedLock scopedLock(deviceLock);

	return visiblePatterns_.find(objectId) != visiblePatterns_.cend();
}

bool PatternTracker6DOF::setParameter(const std::string& parameter, const Value& value)
{
	const ScopedLock scopedLock(deviceLock);

	if (visualTracker_)
	{
		Log::warning() << "Pattern tracker exists already, cannot change parameters anymore";
		return false;
	}

	if (value.isBool())
	{
		if (parameter == "noFrameToFrameTracking")
		{
			noFrameToFrameTracking_ = value.boolValue();
			return true;
		}
		else if (parameter == "noDownsamplingOnAndroid")
		{
			noDownsamplingOnAndroid_ = value.boolValue();
			return true;
		}
	}

	return false;
}

bool PatternTracker6DOF::parameter(const std::string& parameter, Value& value)
{
	const ScopedLock scopedLock(deviceLock);

	if (parameter == "noFrameToFrameTracking")
	{
		value = Value(noFrameToFrameTracking_);
		return true;
	}
	else if (parameter == "noDownsamplingOnAndroid")
	{
		value = Value(noDownsamplingOnAndroid_);
		return true;
	}

	return false;
}

void PatternTracker6DOF::ensureTrackerExists()
{
	// this->deviceLock is locked

	if (!visualTracker_)
	{
		Tracking::Pattern::PatternTracker6DOF::Options options;

		options.noFrameToFrameTracking_ = noFrameToFrameTracking_;

#ifdef OCEAN_PLATFORM_BUILD_ANDROID
		options.downsampleInputImageOnAndroid_ = !noDownsamplingOnAndroid_;
#endif

		visualTracker_ = Tracking::VisualTrackerRef(new Tracking::Pattern::PatternTracker6DOF(options));
	}
}

void PatternTracker6DOF::threadRun()
{
	if (noFrameToFrameTracking_)
	{
		// reducing the thread's priority as tracking will be quite expensive and real-time execution is not expected

		if (!Thread::setThreadPriority(Thread::PRIORITY_IDLE))
		{
			Log::debug() << "PatternTracker6DOF: Failed to set thread priority";
		}
	}

	TemporaryScopedLock temporaryScopedLock(deviceLock);

		ensureTrackerExists();

		if (frameMediums_.size() != 1 || frameMediums_.front().isNull())
		{
			return;
		}

		const Media::FrameMediumRef frameMedium = frameMediums_.front();
	temporaryScopedLock.release();

	Log::info() << deviceNamePatternTracker6DOF() << " started...";

	while (shouldThreadStop() == false)
	{
		SharedAnyCamera anyCamera;
		const FrameRef frame = frameMedium->frame(&anyCamera);

		if (anyCamera && frame && *frame && frame->timestamp() > frameTimestamp_)
		{
			frameTimestamp_ = frame->timestamp();

			Quaternion world_Q_camera(false);
			if (orientationTracker3DOF_)
			{
				const Devices::OrientationTracker3DOF::OrientationTracker3DOFSampleRef sample = orientationTracker3DOF_->sample(frameTimestamp_, Devices::Measurement::IS_TIMESTAMP_INTERPOLATE);

				if (sample && sample->orientations().size() == 1)
				{
					const Quaternion& world_Q_device = sample->orientations().front();

					world_Q_camera = world_Q_device * Quaternion(frameMedium->device_T_camera().rotation());
				}
			}

			static HighPerformanceStatistic performance;
			performance.start();

			ObjectIdSet currentPatterns;

			const SharedAnyCameras anyCameras = { anyCamera };
			const Frames frames = { *frame };

			Tracking::VisualTracker::TransformationSamples transformationSamples;
			if (visualTracker_->determinePoses(frames, anyCameras, transformationSamples, world_Q_camera, WorkerPool::get().scopedWorker()()) && !transformationSamples.empty())
			{
				performance.stop();

				ObjectIds objectIds;
				Tracker6DOFSample::Positions positions;
				Tracker6DOFSample::Orientations orientations;

				for (const Tracking::VisualTracker::TransformationSample& transformSample : transformationSamples)
				{
					const ObjectId objectId = objectIdMapper_.externalObjectIdFromInternalObjectId(transformSample.id());

					currentPatterns.emplace(objectId);

					objectIds.push_back(objectId);
					positions.push_back(transformSample.transformation().translation());
					orientations.push_back(transformSample.transformation().rotation());
				}

				postFoundTrackerObjects(determineFoundObjects(visiblePatterns_, currentPatterns), frameTimestamp_);

				postNewSample(SampleRef(new Tracker6DOFSample(frameTimestamp_, RS_DEVICE_IN_OBJECT, std::move(objectIds), std::move(orientations), std::move(positions))));
			}
			else
			{
				performance.stop();
			}

			if (performance.measurements() % 100u == 0u)
			{
				Log::info() << "Pattern Tracker performance: " << performance.averageMseconds();
				performance.reset();
			}

			postLostTrackerObjects(determineLostObjects(visiblePatterns_, currentPatterns), frameTimestamp_);

			visiblePatterns_ = currentPatterns;
		}
		else
		{
			sleep(1);
		}
	}

	const Timestamp stopTimestamp(true);

	postLostTrackerObjects(visiblePatterns_, stopTimestamp);

	Log::info() << deviceNamePatternTracker6DOF() << " stopped...";
}

}

}

}
