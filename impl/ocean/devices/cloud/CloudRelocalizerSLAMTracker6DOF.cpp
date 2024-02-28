// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/cloud/CloudRelocalizerSLAMTracker6DOF.h"

#include "ocean/base/Subset.h"
#include "ocean/base/ThreadPool.h"

#include "ocean/cv/FrameShrinker.h"

#include "ocean/cv/advanced/AdvancedMotion.h"

#include "ocean/devices/GPSTracker.h"
#include "ocean/devices/Manager.h"

#include "ocean/geometry/AbsoluteTransformation.h"
#include "ocean/geometry/NonLinearOptimizationPose.h"
#include "ocean/geometry/RANSAC.h"

#include "ocean/tracking/cloud/CloudRelocalizer.h"

namespace Ocean
{

namespace Devices
{

namespace Cloud
{

CloudRelocalizerSLAMTracker6DOF::CloudRelocalizerSLAMTracker6DOF(const std::string& name, const bool useFrameToFrameTracking) :
	Device(name,  deviceTypeCloudRelocalizerSLAMTracker6DOF()),
	Measurement(name, deviceTypeCloudRelocalizerSLAMTracker6DOF()),
	Tracker(name, deviceTypeCloudRelocalizerSLAMTracker6DOF()),
	OrientationTracker3DOF(name),
	PositionTracker3DOF(name),
	Tracker6DOF(name),
	SceneTracker6DOF(name),
	ObjectTracker(name, deviceTypeCloudRelocalizerSLAMTracker6DOF()),
	VisualTracker(name, deviceTypeCloudRelocalizerSLAMTracker6DOF()),
	CloudDevice(name, deviceTypeCloudRelocalizerSLAMTracker6DOF())
{
	useFrameToFrameTracking_ = useFrameToFrameTracking;

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

	for (const std::string& deviceName : deviceNames)
	{
		worldTracker_ = Manager::get().device(deviceName);

		if (worldTracker_)
		{
			break;
		}
	}

	if (worldTracker_.isNull())
	{
		Log::warning() << "Failed to access world tracker tracker!";

		deviceIsValid = false;
		return;
	}
}

CloudRelocalizerSLAMTracker6DOF::~CloudRelocalizerSLAMTracker6DOF()
{
	stop();

#ifdef OCEAN_DEBUG
	stopThreadExplicitly(15000u); // 15 seconds
#else
	stopThreadExplicitly();
#endif
}

void CloudRelocalizerSLAMTracker6DOF::setInput(Media::FrameMediumRefs&& frameMediums)
{
	ocean_assert(worldTracker_);

	const ScopedLock scopedLock(deviceLock);

	const VisualTrackerRef visualWorldTracker(worldTracker_);
	ocean_assert(visualWorldTracker);

	visualWorldTracker->setInput(Media::FrameMediumRefs(frameMediums));

	VisualTracker::setInput(std::move(frameMediums));
}

CloudRelocalizerSLAMTracker6DOF::ObjectId CloudRelocalizerSLAMTracker6DOF::registerObject(const std::string& description, const Vector3& /*dimension*/)
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

bool CloudRelocalizerSLAMTracker6DOF::isObjectTracked(const ObjectId& objectId) const
{
	ocean_assert(objectId != invalidObjectId());

	const ScopedLock scopedLock(deviceLock);

	return trackedObjectIds_.find(objectId) != trackedObjectIds_.cend();
}

bool CloudRelocalizerSLAMTracker6DOF::isStarted() const
{
	const ScopedLock scopedLock(deviceLock);

	ocean_assert(worldTracker_);

	return worldTracker_->isStarted();
}

bool CloudRelocalizerSLAMTracker6DOF::start()
{
	const ScopedLock scopedLock(deviceLock);

	ocean_assert(worldTracker_);

	if (objectDescription_.empty())
	{
		Log::error() << "CloudPerFrameRelocalizerTracker6DOF needs a valid object (a label of a cloud map)";
		return false;
	}

	ocean_assert(objectId_ != invalidObjectId());

	if (!worldTracker_->start())
	{
		return false;
	}

	if (!worldTrackerSampleEventSubscription_)
	{
		// we register our event functions so that we can receive tracking events from the world tracker

		ocean_assert(!worldTrackerObjectEventSubscription_);

		worldTrackerSampleEventSubscription_ = worldTracker_->subscribeSampleEvent(SampleCallback::create(*this, &CloudRelocalizerSLAMTracker6DOF::onWorldTrackerSample));
		worldTrackerObjectEventSubscription_ = worldTracker_->subscribeTrackerObjectEvent(TrackerObjectCallback::create(*this, &CloudRelocalizerSLAMTracker6DOF::onWorldTrackerObject));
	}

	return true;
}

bool CloudRelocalizerSLAMTracker6DOF::stop()
{
	const ScopedLock scopedLock(deviceLock);

	worldTrackerSampleEventSubscription_.release();
	worldTrackerObjectEventSubscription_.release();

	if (worldTracker_)
	{
		return worldTracker_->stop();
	}

	return true;
}

void CloudRelocalizerSLAMTracker6DOF::onWorldTrackerSample(const Measurement* /*measurement*/, const SampleRef& sample)
{
	const Tracker6DOFSampleRef tracker6DOFSample(sample);

	ocean_assert(tracker6DOFSample);
	ocean_assert(tracker6DOFSample->referenceSystem() == RS_DEVICE_IN_OBJECT);

	if (tracker6DOFSample->objectIds().empty())
	{
		return;
	}

	ocean_assert(tracker6DOFSample->objectIds().size() == 1); // world tracker is always providing one world transformation

	const HomogenousMatrix4 world_T_camera(tracker6DOFSample->positions().front(), tracker6DOFSample->orientations().front());

	const Timestamp& timestamp = tracker6DOFSample->timestamp();

	if (isThreadActive())
	{
		// we keep storing the world poses as long as the relocalization thread is active
		world_T_cameras_.emplace(timestamp, world_T_camera);
	}

	Metadata metadata;

	const Timestamp firstRelocalizationTimestamp(firstRelocalizationTimestamp_);
	if (firstRelocalizationTimestamp.isValid())
	{
		metadata["firstRelocalizationTimestamp"] = Value(double(firstRelocalizationTimestamp));
	}

	const Timestamp lastSuccessfulRelocalizationTimestamp(lastSuccessfulRelocalizationTimestamp_);
	if (lastSuccessfulRelocalizationTimestamp.isValid())
	{
		metadata["lastSuccessfulRelocalizationTimestamp"] = Value(double(lastSuccessfulRelocalizationTimestamp));
	}

	const Timestamp lastFailedRelocalizationTimestamp(lastFailedRelocalizationTimestamp_);
	if (lastFailedRelocalizationTimestamp.isValid())
	{
		metadata["lastFailedRelocalizationTimestamp"] = Value(double(lastFailedRelocalizationTimestamp));
	}

	const size_t currentFeatureNumberFrameToFrame = currentFeatureNumberFrameToFrame_;
	if (currentFeatureNumberFrameToFrame != 0)
	{
		metadata["numberFeaturesFrameToFrameTracking"] = Value(int(currentFeatureNumberFrameToFrame));
	}

	metadata["numberOfRelocalizationRequestsSent"] = Value(numberOfRequestsSent_);
	metadata["numberOfSuccessfulRelocalizationRequestsReceived"] = Value(numberOfSuccessfulRequestsReceived_);

	ObjectIds objectIds;
	Tracker6DOFSample::Positions positions;
	Tracker6DOFSample::Orientations orientations;
	SharedSceneElements sceneElements;

	// we determine the smoothed 7-DOF transformation between world and Cloud anchor (smoothed to avoid exteme jumps when this transformation gets updated)
	const HomogenousMatrix4 anchor_T_world(anchor_T_world_.transformation(timestamp));

	if (anchor_T_world.isValid())
	{
		const Vector3 scale = anchor_T_world.scale();
		ocean_assert(Numeric::isWeakEqual(scale.x(), scale.y()) && Numeric::isWeakEqual(scale.x(), scale.z()));

		metadata["anchorWorldScale"] = Value(float(scale.x()));

		if (!determineSceneElements(world_T_camera, anchor_T_world, timestamp, objectIds, positions, orientations, sceneElements, metadata))
		{
			ocean_assert(false && "This should never happen!");
		}
	}

	const ObjectIdSet trackedObjectIds(objectIds.cbegin(), objectIds.cend());

	TemporaryScopedLock temporaryScopedLock(deviceLock);

		const ObjectIdSet lostObjectIds = determineLostObjects(trackedObjectIds_, trackedObjectIds);
		const ObjectIdSet foundObjectIds = determineFoundObjects(trackedObjectIds_, trackedObjectIds);
		trackedObjectIds_ = std::move(trackedObjectIds);

	temporaryScopedLock.release();

	postLostTrackerObjects(lostObjectIds, timestamp);
	postFoundTrackerObjects(foundObjectIds, timestamp);

	if (!objectIds.empty() || (isThreadActive() && !metadata.empty()))
	{
		postNewSample(SampleRef(new SceneTracker6DOFSample(timestamp, RS_DEVICE_IN_OBJECT, std::move(objectIds), std::move(orientations), std::move(positions), std::move(sceneElements), std::move(metadata))));
	}

	if (objectIds.empty() && !isThreadActive())
	{
#ifdef OCEAN_PLATFORM_BUILD_MOBILE
		constexpr double delayForStableWorldTracking = 0.5;
#else
		constexpr double delayForStableWorldTracking = 0.0;
#endif

		if (worldTrackerInitializedTimestamp_.isInvalid())
		{
			worldTrackerInitializedTimestamp_ = sample->timestamp();
		}

		if (timestamp > worldTrackerInitializedTimestamp_ + delayForStableWorldTracking)
		{
			startThread();
		}
	}
}

void CloudRelocalizerSLAMTracker6DOF::onWorldTrackerObject(const Tracker* /*tracker*/, const bool found, const ObjectIdSet& worldObjectIds, const Timestamp& timestamp)
{
	ocean_assert(worldObjectIds.size() == 1);

	if (found)
	{
		Log::info() << "World tracking initialized";

		ocean_assert(!anchor_T_world_.isValid());

		const ScopedLock scopedLock(deviceLock);

		worldTrackerInitializedTimestamp_ = timestamp;

		ocean_assert(world_T_cameras_.empty());
		ocean_assert(trackedObjectIds_.empty());
	}
	else
	{
		Log::info() << "World tracking lost";

		stopThread();

		anchor_T_world_.reset();

		TemporaryScopedLock scopedLock(deviceLock);
			ObjectIdSet trackedObjectIds(std::move(trackedObjectIds_));

			worldTrackerInitializedTimestamp_.toInvalid();
			world_T_cameras_.clear();
		scopedLock.release();

		postLostTrackerObjects(trackedObjectIds, timestamp);
	}
}

void CloudRelocalizerSLAMTracker6DOF::threadRun()
{
	TemporaryScopedLock temporaryScopedLock(deviceLock);
		if (frameMediums_.size() != 1 || frameMediums_.front().isNull())
		{
			return;
		}

		const Media::FrameMediumRef frameMedium = frameMediums_.front();
	temporaryScopedLock.release();

	constexpr unsigned int maxImageResolution = 1280u * 720u;

	Tracking::Cloud::CloudRelocalizer::Configuration configuration;
	if (!determineConfiguration(configuration))
	{
		Log::error() << "Failed to determine configuration for cloud relocalizer";
		return;
	}

	GPSTrackerRef gpsTracker;

	gpsTracker = Manager::get().device(GPSTracker::deviceTypeGPSTracker());

	if (gpsTracker.isNull())
	{
		Log::error() << "Failed to create GPS Tracker.";
		return;
	}

	gpsTracker->start();

	auto httpClient = Tracking::Cloud::CloudRelocalizer::createClient();

	if (!httpClient)
	{
		Log::error() << "Failed to create HTTP client";
		return;
	}

	// frame to frame tracking thresholds
	constexpr float minimalRelativeCorrespondences = 0.08f; // 8% wrt to initial correspondences
	constexpr size_t minimalAbsoluteCorrespondencesStart = 25; // just 25 correspondences
	constexpr size_t minimalAbsoluteCorrespondencesEstablished = 35; // just 35 correspondences

	constexpr double minimalTimeBetweenRelocalization = 3.0; // 3 seconds

	Timestamp lastInvokedRelocalization(false);

	size_t minimalAbsoluteCorrespondences = minimalAbsoluteCorrespondencesStart;

	RandomGenerator randomGenerator;

	TransformationMap anchor_T_cameras;

	HomogenousMatrix4 anchor_T_world(false); // the local copy of anchor_T_world_, a 7-DOF transformation

	while (!shouldThreadStop())
	{
		sleep(1u);

		Timestamp firstTimestamp(false);
		HomogenousMatrix4 world_T_firstCamera(false);

		{
			const ScopedLock scopedLock(deviceLock);

			if (world_T_cameras_.empty())
			{
				continue;
			}

			firstTimestamp = world_T_cameras_.rbegin()->first; // most recent timestamp
			world_T_firstCamera = world_T_cameras_.rbegin()->second;
		}

		if (lastInvokedRelocalization.isValid() && firstTimestamp < lastInvokedRelocalization + minimalTimeBetweenRelocalization)
		{
			// we need to wait until we can start a new relocalization request
			continue;
		}

		ocean_assert(firstTimestamp.isValid());
		ocean_assert(world_T_firstCamera.isValid());

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

		const VectorD2 gpsLocation = VectorD2(locations.front().latitude(), locations.front().longitude());

		SharedAnyCamera firstCamera;

		FrameRef firstFrame = frameMedium->frame(firstTimestamp, &firstCamera);

		if (!firstFrame || !firstCamera || firstFrame->timestamp() != firstTimestamp)
		{
			continue;
		}

		Frame yFrame;
		if (!CV::FrameConverter::Comfort::convert(*firstFrame, FrameType(*firstFrame, FrameType::FORMAT_Y8), yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE))
		{
			ocean_assert(false && "This should never happen!");
			continue;
		}

		if (firstRelocalizationTimestamp_.load().isInvalid())
		{
			firstRelocalizationTimestamp_ = yFrame.timestamp();
		}

		Frame yFrameToUse(yFrame, Frame::ACM_USE_KEEP_LAYOUT);
		while (yFrameToUse.width() * yFrameToUse.height() > maxImageResolution)
		{
			CV::FrameShrinker::downsampleByTwo11(yFrameToUse);
		}

		SharedAnyCamera cameraToUse = firstCamera->clone(yFrameToUse.width(), yFrameToUse.height());

		ocean_assert(cameraToUse != nullptr);
		if (cameraToUse == nullptr)
		{
			ocean_assert(false && "This should never happen!");
			continue;
		}

		// gravity must be a unit vector, defined in the camera coordinate system (with default camera pointing towards negative z-space), pointing towards ground

		const HomogenousMatrix4 firstCamera_T_world(world_T_firstCamera.inverted());
		const Vector3 firstCameraGravity = -firstCamera_T_world.yAxis();

		HomogenousMatrix4 anchor_T_firstCamera(false);
		Vectors3 currentObjectPoints;
		Vectors2 currentImagePoints;

		++numberOfRequestsSent_;

		if (!invokeRelocalization(*cameraToUse, yFrameToUse, gpsLocation, firstCameraGravity, configuration, *httpClient, anchor_T_firstCamera, currentObjectPoints, currentImagePoints))
		{
			lastFailedRelocalizationTimestamp_ = yFrame.timestamp();
			currentFeatureNumberFrameToFrame_  = 0;

			continue;
		}

		lastInvokedRelocalization = firstTimestamp;

		if (anchor_T_world.isValid())
		{
			// this is the second (or later) successful relocalization, we can make the threshold tighter
			minimalAbsoluteCorrespondences = minimalAbsoluteCorrespondencesEstablished;
		}

		ocean_assert(currentObjectPoints.size() == currentImagePoints.size());

		const size_t initialCorrespondences = currentObjectPoints.size();

		const bool fewCorrespondences = initialCorrespondences < 70;

		if (fewCorrespondences)
		{
			lastFailedRelocalizationTimestamp_ = yFrame.timestamp();
			currentFeatureNumberFrameToFrame_ = 0;

			Log::info() << "Relocalization succeeded but only with " << initialCorrespondences << " correspondences, we will re-try to determine more correspondences in the next frame";

			if (!relocalizeWithFewCorrespondences_) {
				continue;
			}
		}
		else
		{
        	++numberOfSuccessfulRequestsReceived_;
		}

		lastSuccessfulRelocalizationTimestamp_ = yFrame.timestamp();

		// as the anchor and world will have different scales, we need to track the anchor over several frames until we can anchor the anchor in the world

		ocean_assert(currentObjectPoints.size() == currentImagePoints.size());

		ocean_assert(anchor_T_cameras.find(firstTimestamp) == anchor_T_cameras.cend());
		anchor_T_cameras.emplace(firstTimestamp, anchor_T_firstCamera);

		anchor_T_world = calculateAnchor_T_world(anchor_T_cameras);

		{
			// the relocalization may be quite old already (e.g., 1.5seconds), therefore, we use the most recent timestamp to allow a smooth interpolated pose
			FrameRef currentFrame = frameMedium->frame();

			anchor_T_world_.setTransformation(anchor_T_world, currentFrame ? currentFrame->timestamp() : firstTimestamp);
		}

		if (!useFrameToFrameTracking_ || fewCorrespondences)
		{
			// we skip the frame to frame tracking
			continue;
		}

		constexpr unsigned int coarsestLayerRadius = 8u;
		constexpr unsigned int pyramidLayers = 3u;
		constexpr unsigned int subPixelIterations = 4u;

		CV::FramePyramid currentPyramid = CV::FramePyramid::create8BitPerChannel(yFrameToUse.constdata<uint8_t>(), yFrameToUse.width(), yFrameToUse.height(), 1u, FrameType::ORIGIN_UPPER_LEFT, pyramidLayers, yFrameToUse.paddingElements(), nullptr);

		Timestamp lastFrameTimestamp(firstTimestamp);

		// now we track the 2D/3D correspondences until we have lost most of them

		CV::FramePyramid previousPyramid(std::move(currentPyramid));
		Vectors2 previousImagePoints(std::move(currentImagePoints));
		Vectors2 predictedCurrentImagePoints;

		Indices32 validCorrespondences;

		while (!shouldThreadStop())
		{
			sleep(1u);

			SharedAnyCamera currentCamera;
			FrameRef currentFrame = frameMedium->frame(&currentCamera);

			if (!currentFrame || !currentCamera || currentFrame->timestamp() == lastFrameTimestamp)
			{
				continue;
			}

			const Timestamp currentTimestamp = currentFrame->timestamp();
			lastFrameTimestamp = currentFrame->timestamp();

			if (!CV::FrameConverter::Comfort::convert(*currentFrame, FrameType(*currentFrame, FrameType::FORMAT_Y8), yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE))
			{
				ocean_assert(false && "This should never happen!");
				continue;
			}

			yFrameToUse = Frame(yFrame, Frame::ACM_USE_KEEP_LAYOUT);
			while (yFrameToUse.width() * yFrameToUse.height() > maxImageResolution)
			{
				CV::FrameShrinker::downsampleByTwo11(yFrameToUse);
			}

			cameraToUse = currentCamera->clone(yFrameToUse.width(), yFrameToUse.height());

			ocean_assert(cameraToUse != nullptr);
			if (cameraToUse == nullptr)
			{
				ocean_assert(false && "This should never happen!");
				continue;
			}

			currentPyramid.replace8BitPerChannel(yFrameToUse.constdata<uint8_t>(), yFrameToUse.width(), yFrameToUse.height(), 1u, FrameType::ORIGIN_UPPER_LEFT, pyramidLayers, yFrameToUse.paddingElements(), nullptr);


			HomogenousMatrix4 world_T_predictedCamera(false);

			{
				const ScopedLock scopedLock(deviceLock);

				if (world_T_cameras_.empty())
				{
					continue;
				}

				TransformationMap::iterator iMap = world_T_cameras_.lower_bound(currentTimestamp);

				if (iMap == world_T_cameras_.cend())
				{
					// no pose greater or equal to the current timestamp, so our best prediction is the latest pose
					world_T_predictedCamera = world_T_cameras_.rbegin()->second;
				}
				else
				{
					world_T_predictedCamera = iMap->second;
				}
			}

			ocean_assert(anchor_T_world.isValid() && world_T_predictedCamera.isValid());

			const HomogenousMatrix4 anchor_T_predictedCamera(anchor_T_world * world_T_predictedCamera);
			const HomogenousMatrix4 flippedPredictedCamera_T_anchor(AnyCamera::standard2InvertedFlipped(anchor_T_predictedCamera));

			predictedCurrentImagePoints.clear();
			predictedCurrentImagePoints.reserve(currentObjectPoints.size());

			for (const Vector3& currentObjectPoint : currentObjectPoints)
			{
				const Vector2 predictedImagePoint(cameraToUse->projectToImageIF(flippedPredictedCamera_T_anchor, currentObjectPoint));

				predictedCurrentImagePoints.emplace_back(minmax<Scalar>(Scalar(0), predictedImagePoint.x(), Scalar(cameraToUse->width() - 1u)), minmax<Scalar>(Scalar(0), predictedImagePoint.y(), Scalar(cameraToUse->height() - 1u)));
			}

			currentImagePoints.clear();
			validCorrespondences.clear();
			CV::Advanced::AdvancedMotionSSD::trackPointsBidirectionalSubPixelMirroredBorderWithRoughLocations<1u, 15u>(previousPyramid, currentPyramid, coarsestLayerRadius, previousImagePoints, predictedCurrentImagePoints, currentImagePoints, Scalar(0.9 * 0.9), nullptr, &validCorrespondences, subPixelIterations);

			if (validCorrespondences.size() < size_t(float(initialCorrespondences) * minimalRelativeCorrespondences + 0.5f) || validCorrespondences.size() < minimalAbsoluteCorrespondences)
			{
				// we lost too may correspondences from the first frame
				break;
			}

			if (validCorrespondences.size() != currentImagePoints.size())
			{
				currentObjectPoints = Subset::subset(currentObjectPoints, validCorrespondences);
				currentImagePoints = Subset::subset(currentImagePoints, validCorrespondences);
				previousImagePoints = Subset::subset(previousImagePoints, validCorrespondences);
			}

			ocean_assert(currentObjectPoints.size() == currentImagePoints.size());

			HomogenousMatrix4 anchor_T_currentCamera;

			validCorrespondences.clear();
			if (!Geometry::RANSAC::p3p(*cameraToUse, ConstArrayAccessor<Vector3>(currentObjectPoints), ConstArrayAccessor<Vector2>(currentImagePoints), randomGenerator, anchor_T_currentCamera, 25u, true, 25u, Scalar(7 * 7), &validCorrespondences))
			{
				continue;
			}

			if (validCorrespondences.size() < size_t(float(initialCorrespondences) * minimalRelativeCorrespondences + 0.5f) || validCorrespondences.size() < minimalAbsoluteCorrespondences)
			{
				currentFeatureNumberFrameToFrame_ = 0;

				// we lost too may correspondences from the first frame
				break;
			}

			currentFeatureNumberFrameToFrame_ = validCorrespondences.size();

			if (validCorrespondences.size() != currentImagePoints.size())
			{
				currentObjectPoints = Subset::subset(currentObjectPoints, validCorrespondences);
				currentImagePoints = Subset::subset(currentImagePoints, validCorrespondences);
				previousImagePoints = Subset::subset(previousImagePoints, validCorrespondences);
			}

			std::swap(previousPyramid, currentPyramid);
			std::swap(previousImagePoints, currentImagePoints);

#if 0 // for now we use every frame
			constexpr double poseSeparationForScaleEstimation = 0.05; // 50ms seconds

			if (anchor_T_cameras.size() >= 20 && anchor_T_cameras.rbegin()->first + poseSeparationForScaleEstimation > currentTimestamp)
			{
				// the current pose is too close to the previous one, we do not get enough signal out of this pose
				// so we do not add the pose for the scale alignment
				continue;
			}
#endif

			ocean_assert(anchor_T_cameras.find(currentTimestamp) == anchor_T_cameras.cend());
			anchor_T_cameras.emplace(currentTimestamp, anchor_T_currentCamera);

			anchor_T_world = calculateAnchor_T_world(anchor_T_cameras);

			if (shouldThreadStop())
			{
				return;
			}

			anchor_T_world_.setTransformation(anchor_T_world, currentTimestamp);

			// let's remove all transformations which are too old

			constexpr double maximalPoseHistory = 25.0; // 25 seconds

			while (anchor_T_cameras.size() > 100 && anchor_T_cameras.begin()->first + maximalPoseHistory < currentTimestamp)
			{
				anchor_T_cameras.erase(anchor_T_cameras.begin());
			}

			const ScopedLock scopedLock(deviceLock);

			while (world_T_cameras_.size() > 100 && world_T_cameras_.begin()->first + maximalPoseHistory < currentTimestamp)
			{
				world_T_cameras_.erase(world_T_cameras_.begin());
			}
		}
	}
}

HomogenousMatrix4 CloudRelocalizerSLAMTracker6DOF::calculateAnchor_T_world(const TransformationMap& anchor_T_cameras)
{
	// let's find the pairs of corresponding camera transformations
	HomogenousMatrices4 world_T_correspondingCameras;
	HomogenousMatrices4 anchor_T_correspondingCameras;

	{
		const ScopedLock scopedLock(deviceLock);
		Subset::correspondingElements<Timestamp, HomogenousMatrix4>(world_T_cameras_, anchor_T_cameras, world_T_correspondingCameras, anchor_T_correspondingCameras);
	}

	if (world_T_correspondingCameras.empty())
	{
		return HomogenousMatrix4(false);
	}

	HomogenousMatrix4 anchor_T_world;
	Scalar scale;
	if (Geometry::AbsoluteTransformation::calculateTransformation(world_T_correspondingCameras.data(), anchor_T_correspondingCameras.data(), world_T_correspondingCameras.size(), anchor_T_world, Geometry::AbsoluteTransformation::ScaleErrorType::Symmetric, &scale))
	{
		// we do not use `anchor_T_world` directly, as this transformation optimizes the transformation across all corresponding transformations (this result would be a slightly misaligned/swimming AR content
		// instead, we use the scale only - and we apply the scale to the latest corresponding pose - this results in a great content registration with a fair depth/scale

		const HomogenousMatrix4& world_T_lastCorrespondingCamera = world_T_correspondingCameras.back();

		HomogenousMatrix4 anchor_T_lastCorrespondingCamera(anchor_T_correspondingCameras.back());
		anchor_T_lastCorrespondingCamera.applyScale(Vector3(scale, scale, scale));

		return anchor_T_lastCorrespondingCamera * world_T_lastCorrespondingCamera.inverted();
	}

	return HomogenousMatrix4(false);
}

bool CloudRelocalizerSLAMTracker6DOF::determineConfiguration(Tracking::Cloud::CloudRelocalizer::Configuration& configuration)
{
	if (!objectDescription_.empty() && objectDescription_.front() == '{' && objectDescription_.back() == '}')
	{
		// we have a JSON-based description

		std::string errorMessage;
		if (!Tracking::Cloud::CloudRelocalizer::Configuration::parseConfiguration(objectDescription_, configuration, &errorMessage))
		{
			Log::error() << "Failed to parse object description for CloudRelocalizerTracker6DOF: " << errorMessage;

			// we use the default configuration
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
			VectorD2 explicitGPSLocation(NumericD::minValue(), NumericD::minValue());
			mapLabel = Tracking::Cloud::CloudRelocalizer::parseMapLabel(mapLabel, explicitGPSLocation);

			if (explicitGPSLocation.x() != NumericD::minValue())
			{
				Log::warning() << "Explicit GPS location is not supported!";
			}
		}

		configuration = Tracking::Cloud::CloudRelocalizer::Configuration(std::move(mapLabel), std::move(releaseEnvironment));
	}

	return true;
}

}

}

}
