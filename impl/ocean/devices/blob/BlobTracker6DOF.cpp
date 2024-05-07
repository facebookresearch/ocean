// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "ocean/devices/blob/BlobTracker6DOF.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/String.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/IntegralImage.h"

#include "ocean/io/CameraCalibrationManager.h"
#include "ocean/io/File.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Random.h"
#include "ocean/math/Rotation.h"

#include "ocean/media/FrameMedium.h"
#include "ocean/media/Manager.h"

#include "ocean/tracking/blob/Loader.h"

namespace Ocean
{

namespace Devices
{

namespace Blob
{

using namespace Ocean::Tracking::Blob;

BlobTracker6DOF::BlobTracker6DOF() :
	Device(deviceNameBlobTracker6DOF(),  deviceTypeBlobTracker6DOF()),
	BlobDevice(deviceNameBlobTracker6DOF(), deviceTypeBlobTracker6DOF()),
	Measurement(deviceNameBlobTracker6DOF(), deviceTypeBlobTracker6DOF()),
	Tracker(deviceNameBlobTracker6DOF(), deviceTypeBlobTracker6DOF()),
	OrientationTracker3DOF(deviceNameBlobTracker6DOF()),
	PositionTracker3DOF(deviceNameBlobTracker6DOF()),
	Tracker6DOF(deviceNameBlobTracker6DOF()),
	ObjectTracker(deviceNameBlobTracker6DOF(), deviceTypeBlobTracker6DOF()),
	VisualTracker(deviceNameBlobTracker6DOF(), deviceTypeBlobTracker6DOF())
{
	// nothing do to here
}

BlobTracker6DOF::~BlobTracker6DOF()
{
	stopThreadExplicitly();
}

BlobTracker6DOF::ObjectId BlobTracker6DOF::registerObject(const std::string& description, const Vector3& dimension)
{
	const ScopedLock scopedLock(deviceLock);

	const IO::File file(description);

	if (!file.exists())
	{
		return invalidObjectId();
	}

	if (file.extension() == "ofm")
	{
		CV::Detector::Blob::BlobFeatures features;

		if (Tracking::Blob::Loader::loadFile(file(), features))
		{
			const FeatureMap featureMap(features);

			featureTracker_.setFeatureMap(featureMap);

			patternObjectId_ = addUniqueObjectId(file());
			return patternObjectId_;
		}
	}
	else
	{
		ocean_assert(dimension.x() > 0);

		if (dimension.x() <= 0)
		{
			Log::error() << "Invalid feature map dimension!";
			return invalidObjectId();
		}

		Media::FrameMediumRef frameMedium(Media::Manager::get().newMedium(description, Media::Medium::IMAGE));
		if (frameMedium.isNull())
		{
			return invalidObjectId();
		}

		frameMedium->start();

		if (frameMedium->frame().isNull())
		{
			Log::error() << "The defined tracking object \"" << description << "\" holds no image data.";
			return invalidObjectId();
		}

		const FrameRef frame = frameMedium->frame();

		if (frame && *frame)
		{
			const FeatureMap featureMap(*frame, Vector2(dimension.x(), dimension.y()), Scalar(6.5), true, 0, WorkerPool::get().scopedWorker()());

			featureTracker_.setFeatureMap(featureMap);

			patternObjectId_ = addUniqueObjectId(description);
			return patternObjectId_;
		}
	}

	return invalidObjectId();
}

bool BlobTracker6DOF::isStarted() const
{
	const ScopedLock scopedLock(deviceLock);

	return isThreadActive();
}

bool BlobTracker6DOF::start()
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

	Log::info() << "6DOF Blob Feature tracker started using descriptors with " << CV::Detector::Blob::BlobDescriptor::elements() << " elements.";
	return true;
}

bool BlobTracker6DOF::stop()
{
	const ScopedLock scopedLock(deviceLock);

	stopThread();
	return true;
}

bool BlobTracker6DOF::isObjectTracked(const ObjectId& /*objectId*/) const
{
	const ScopedLock scopedLock(deviceLock);

	ocean_assert(false && "Missing implementation!");
	return false;
}

void BlobTracker6DOF::threadRun()
{
	TemporaryScopedLock temporaryScopedLock(deviceLock);
		if (frameMediums_.size() != 1 || frameMediums_.front().isNull())
		{
			return;
		}

		const Media::FrameMediumRef frameMedium = frameMediums_.front();
	temporaryScopedLock.release();

	Log::info() << deviceNameBlobTracker6DOF() << " started...";

	bool trackedPreviousFrame = false;

	try
	{
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

					if (performance.measurements() % 10 == 0)
					{
						Log::info() << "Tracker performance: " << performance.averageMseconds();
					}

					const HomogenousMatrix4& object_T_camera = transformationSamples.front().transformation();

					if (trackedPreviousFrame == false)
					{
						postFoundTrackerObjects({patternObjectId_}, frameTimestamp_);
					}

					trackedPreviousFrame = true;

					postNewSample(SampleRef(new Tracker6DOFSample(frameTimestamp_, RS_DEVICE_IN_OBJECT, ObjectIds(1, patternObjectId_), Quaternions(1, object_T_camera.rotation()), Vectors3(1, object_T_camera.translation()))));
					continue;
				}
				else
				{
					performance.skip();
				}

				if (trackedPreviousFrame)
				{
					postLostTrackerObjects({patternObjectId_}, frameTimestamp_);
					trackedPreviousFrame = false;
				}
			}
			else
			{
				sleep(1);
			}
		}
	}
	catch(const Exception& exception)
	{
		Log::error() << "Exception during Blob feature tracker: " << exception.what();
	}
	catch(...)
	{
		Log::error() << "Unknown exception during Blob feature tracker!";
	}

	postLostTrackerObjects({patternObjectId_}, Timestamp(true));

	Log::info() << deviceNameBlobTracker6DOF() << " stopped...";
}

}

}

}
