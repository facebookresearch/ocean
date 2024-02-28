// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/cloud/CloudPerFrameRelocalizerTracker6DOF.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/devices/GPSTracker.h"
#include "ocean/devices/Manager.h"

#include "ocean/tracking/cloud/CloudRelocalizer.h"

namespace Ocean
{

namespace Devices
{

namespace Cloud
{

CloudPerFrameRelocalizerTracker6DOF::CloudPerFrameRelocalizerTracker6DOF() :
	Device(deviceNameCloudPerFrameRelocalizerTracker6DOF(), deviceTypeCloudPerFrameRelocalizerTracker6DOF()),
	Measurement(deviceNameCloudPerFrameRelocalizerTracker6DOF(), deviceTypeCloudPerFrameRelocalizerTracker6DOF()),
	Tracker(deviceNameCloudPerFrameRelocalizerTracker6DOF(), deviceTypeCloudPerFrameRelocalizerTracker6DOF()),
	OrientationTracker3DOF(deviceNameCloudPerFrameRelocalizerTracker6DOF()),
	PositionTracker3DOF(deviceNameCloudPerFrameRelocalizerTracker6DOF()),
	Tracker6DOF(deviceNameCloudPerFrameRelocalizerTracker6DOF()),
	SceneTracker6DOF(deviceNameCloudPerFrameRelocalizerTracker6DOF()),
	ObjectTracker(deviceNameCloudPerFrameRelocalizerTracker6DOF(), deviceTypeCloudPerFrameRelocalizerTracker6DOF()),
	VisualTracker(deviceNameCloudPerFrameRelocalizerTracker6DOF(), deviceTypeCloudPerFrameRelocalizerTracker6DOF()),
	CloudDevice(deviceNameCloudPerFrameRelocalizerTracker6DOF(), deviceTypeCloudPerFrameRelocalizerTracker6DOF())
{
	// nothing to do here
}

CloudPerFrameRelocalizerTracker6DOF::~CloudPerFrameRelocalizerTracker6DOF()
{
	stop();
}

CloudPerFrameRelocalizerTracker6DOF::ObjectId CloudPerFrameRelocalizerTracker6DOF::registerObject(const std::string& description, const Vector3& /*dimension*/)
{
	const ScopedLock scopedLock(deviceLock);

	if (objectId_ != invalidObjectId())
	{
		Log::error() << "The tracker has been configured already for map '" << objectDescription_ << "'";

		// for now, we do not support more than one location
		return invalidObjectId();
	}

	objectDescription_ = description;

	objectId_ = addUniqueObjectId(description);

	return objectId_;
}

bool CloudPerFrameRelocalizerTracker6DOF::isObjectTracked(const ObjectId& objectId) const
{
	ocean_assert(objectId != invalidObjectId());

	const ScopedLock scopedLock(deviceLock);

	return objectId == objectId_ && objectIsTracked_;
}

bool CloudPerFrameRelocalizerTracker6DOF::isStarted() const
{
	const ScopedLock scopedLock(deviceLock);

	return isThreadActive();
}

bool CloudPerFrameRelocalizerTracker6DOF::start()
{
	const ScopedLock scopedLock(deviceLock);

	if (frameMediums_.size() != 1 || frameMediums_.front().isNull())
	{
		Log::error() << "CloudPerFrameRelocalizerTracker6DOF needs a valid input medium";
		return false;
	}

	if (objectDescription_.empty())
	{
		Log::error() << "CloudPerFrameRelocalizerTracker6DOF needs a valid object (a label of a cloud map)";
		return false;
	}

	ocean_assert(objectId_ != invalidObjectId());

	if (!isThreadActive())
	{
		startThread();
	}

	return true;
}

bool CloudPerFrameRelocalizerTracker6DOF::stop()
{
	const ScopedLock scopedLock(deviceLock);

	stopThreadExplicitly();

	return true;
}

void CloudPerFrameRelocalizerTracker6DOF::threadRun()
{
	TemporaryScopedLock temporaryScopedLock(deviceLock);
		if (frameMediums_.size() != 1 || frameMediums_.front().isNull())
		{
			return;
		}

		const Media::FrameMediumRef frameMedium = frameMediums_.front();
	temporaryScopedLock.release();

	VectorD2 explicitGPSLocation(Numeric::minValue(), Numeric::minValue());

	Tracking::Cloud::CloudRelocalizer::Configuration configuration;

	if (!objectDescription_.empty() && objectDescription_.front() == '{' && objectDescription_.back() == '}')
	{
		// we have a JSON-based description

		std::string errorMessage;
		if (!Tracking::Cloud::CloudRelocalizer::Configuration::parseConfiguration(objectDescription_, configuration, &errorMessage))
		{
			Log::error() << "Failed to parse object description for CloudRelocalizerTracker6DOF: " << errorMessage;
		}
	}
	else
	{
		// we have a legacy description

		std::string mapLabel;
		std::string releaseEnvironment;

		if (!Tracking::Cloud::CloudRelocalizer::parseMapParameters(objectDescription_, mapLabel, releaseEnvironment))
		{
			Log::info() << "Failed to parse map parameters, got '" << objectDescription_ << "'";
		}

		if (mapLabel == "any")
		{
			// if the map label is not defined, any suitable map will be used
			mapLabel.clear();
		}
		else if (!mapLabel.empty())
		{
			mapLabel = Tracking::Cloud::CloudRelocalizer::parseMapLabel(mapLabel, explicitGPSLocation); // we allow an explicit GPS location as part of the object description, with pattern "<MapLabel>@GPS<Latitude>,<Longitude>"
		}

		configuration = Tracking::Cloud::CloudRelocalizer::Configuration(std::move(mapLabel), std::move(releaseEnvironment));
	}

	GPSTrackerRef gpsTracker;

	if (explicitGPSLocation == VectorD2(Numeric::minValue(), Numeric::minValue()))
	{
		gpsTracker = Manager::get().device(GPSTracker::deviceTypeGPSTracker());

		if (gpsTracker.isNull())
		{
			Log::error() << "Failed to create GPS Tracker.";
			return;
		}

		if (!gpsTracker->start())
		{
			Log::error() << "Failed to start world tracker or GPS tracker";
			return;
		}
	}

	const Strings deviceNames =
	{
#if defined(OCEAN_PLATFORM_BUILD_ANDROID)
		"ARCore 6DOF World Tracker",
#elif defined(OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY)
		"ARKit 6DOF World Tracker",
#endif
		"VRS ARCore 6DOF World Tracker",
		"VRS ARKit 6DOF World Tracker",
		"VRS Rover 6DOF World Tracker"
	};

	Tracker6DOFRef worldTracker;

	for (const std::string& deviceName : deviceNames)
	{
		worldTracker = Manager::get().device(deviceName);

		if (worldTracker)
		{
			break;
		}
	}

	if (worldTracker.isNull())
	{
		Log::error() << "Failed to create World Tracker.";
		return;
	}

	auto httpClient = Tracking::Cloud::CloudRelocalizer::createClient();

	if (!httpClient)
	{
		Log::error() << "Failed to create HTTP client";
		return;
	}

	if (!worldTracker->start())
	{
		Log::error() << "Failed to start world tracker";
		return;
	}

	Timestamp lastFrameTimestamp(false);

	SharedAnyCamera camera;
	Frame yFrame;

	TemporaryScopedLock temporaryScopeLock(deviceLock);
	objectIsTracked_ = false;
	temporaryScopeLock.release();

	ocean_assert(objectId_ != invalidObjectId());

	while (!shouldThreadStop())
	{
		const FrameRef frame = frameMedium->frame(&camera);

		if (frame && *frame && camera && frame->timestamp() != lastFrameTimestamp)
		{
			if (camera->anyCameraType() != AnyCameraType::PINHOLE)
			{
				Log::error() << "CloudPerFrameRelocalizerTracker6DOF needs pinhole camera model";
				return;
			}

			lastFrameTimestamp = frame->timestamp();

			if (CV::FrameConverter::Comfort::convert(*frame, FrameType(*frame, FrameType::FORMAT_Y8), yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE))
			{
				const Tracker6DOFSampleRef tracker6DOFSample = worldTracker->sample(lastFrameTimestamp);

				if (tracker6DOFSample.isNull())
				{
					continue;
				}

				HomogenousMatrix4 world_T_camera(false);
				if (tracker6DOFSample->objectIds().size() == 1)
				{
					world_T_camera = HomogenousMatrix4(tracker6DOFSample->positions().front(), tracker6DOFSample->orientations().front());
				}

				if (!world_T_camera.isValid())
				{
					continue;
				}

				// gravity must be a unit vector, defined in the camera coordinate system (with default camera pointing towards negative z-space), pointing towards ground

				const HomogenousMatrix4 camera_T_world(world_T_camera.inverted());
				const Vector3 gravity = -camera_T_world.yAxis();

				// we allow an explicit GPS location
				VectorD2 gpsLocation(explicitGPSLocation);

				if (gpsLocation == VectorD2(Numeric::minValue(), Numeric::minValue()))
				{
					const GPSTracker::GPSTrackerSampleRef gpsSample = gpsTracker->sample();

					if (gpsSample.isNull())
					{
						sleep(1u);
						continue;
					}

					const GPSTracker::Locations& locations = gpsSample->locations();

					if (locations.empty())
					{
						sleep(1u);
						continue;
					}

					gpsLocation = VectorD2(locations.front().latitude(), locations.front().longitude());
				}

				temporaryScopeLock.relock(deviceLock);
				bool objectIsTracked = objectIsTracked_;
				temporaryScopeLock.release();

				HomogenousMatrix4 model_T_camera(false);
				Vectors3 objectPoints;
				Vectors2 imagePoints;
				if (Tracking::Cloud::CloudRelocalizer::relocalize(*camera, yFrame, gpsLocation, gravity, configuration, *httpClient, model_T_camera, &objectPoints, &imagePoints))
				{
					if (!objectIsTracked)
					{
						objectIsTracked = true;

						postFoundTrackerObjects({objectId_}, frame->timestamp());
					}

					ocean_assert(objectPoints.size() == imagePoints.size());

					SharedSceneElements sceneElements;

					if (objectPoints.empty())
					{
						sceneElements.emplace_back(std::make_shared<SceneElementFeatureCorrespondences>(std::move(objectPoints), std::move(imagePoints)));
					}
					else
					{
						sceneElements.emplace_back(nullptr);
					}

					postNewSample(SampleRef(new SceneTracker6DOFSample(frame->timestamp(), RS_DEVICE_IN_OBJECT, ObjectIds(1, objectId_), Quaternions(1, model_T_camera.rotation()), Vectors3(1, model_T_camera.translation()), std::move(sceneElements))));

					continue;
				}

				if (objectIsTracked)
				{
					objectIsTracked = false;
					postLostTrackerObjects({objectId_}, frame->timestamp());
				}

				temporaryScopeLock.relock(deviceLock);
				objectIsTracked_ = objectIsTracked;
			}
		}
		else
		{
			sleep(1u);
		}
	}

	const FrameRef frame = frameMedium->frame();

	temporaryScopeLock.relock(deviceLock);
	if (objectIsTracked_ && frame)
	{
		temporaryScopeLock.release();

		postLostTrackerObjects({objectId_}, frame->timestamp());
	}
}

}

}

}
