// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/PrototypeDevices.h"

#include "ocean/base/Subset.h"
#include "ocean/base/ThreadPool.h"

#include "ocean/cv/FrameShrinker.h"

#include "ocean/cv/advanced/AdvancedMotion.h"

#include "ocean/devices/GPSTracker.h"
#include "ocean/devices/Manager.h"
#include "ocean/devices/SceneTracker6DOF.h"

#include "ocean/geometry/AbsoluteTransformation.h"
#include "ocean/geometry/NonLinearOptimizationPose.h"
#include "ocean/geometry/RANSAC.h"

#include "ocean/io/FileResolver.h"

#include "ocean/math/SophusUtilities.h"

#include "ocean/tracking/blob/BlobTracker6DOF.h"
#include "ocean/tracking/pattern/PatternTrackerCore6DOF.h"

#include <location_platform/location/geoanchor/GeoAnchor.hpp>
#include <location_platform/location/geoanchor/GeoAnchorManager.hpp>

#include <sophus/se3.hpp>

namespace Ocean
{

using namespace Devices;

namespace XRPlayground
{

bool PrototypeDevices::registerPrototypeDevices()
{
	if (!Manager::get().registerAdhocDevice(OfflineRelocalizationTracker6DOF::deviceName(), OfflineRelocalizationTracker6DOF::deviceType(), Manager::AdhocInstanceFunction::createStatic(&OfflineRelocalizationTracker6DOF::create)))
	{
		return false;
	}

	if (!Manager::get().registerAdhocDevice(FloorTracker6DOF::deviceName(), FloorTracker6DOF::deviceType(), Manager::AdhocInstanceFunction::createStatic(&FloorTracker6DOF::create)))
	{
		return false;
	}

	if (!Manager::get().registerAdhocDevice(GeoAnchorTracker6DOF::deviceName(), GeoAnchorTracker6DOF::deviceType(), Manager::AdhocInstanceFunction::createStatic(&GeoAnchorTracker6DOF::create)))
	{
		return false;
	}

	return true;
}

OfflineRelocalizationTracker6DOF::OfflineRelocalizationTracker6DOF() :
	Device(deviceName(),  deviceType()),
	Measurement(deviceName(), deviceType()),
	Tracker(deviceName(), deviceType()),
	OrientationTracker3DOF(deviceName()),
	PositionTracker3DOF(deviceName()),
	Tracker6DOF(deviceName()),
	ObjectTracker(deviceName(), deviceType()),
	VisualTracker(deviceName(), deviceType()),
	libraryName_(Manager::nameAdhocFactory())
{
	const Strings deviceNames =
	{
#if defined(OCEAN_PLATFORM_BUILD_ANDROID)
		"ARCore 6DOF World Tracker",
#elif defined(OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY)
		"ARKit 6DOF World Tracker",
#endif
		"VRS ARCore 6DOF World Tracker",
		"VRS ARKit 6DOF World Tracker"
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

OfflineRelocalizationTracker6DOF::~OfflineRelocalizationTracker6DOF()
{
	stop();

#ifdef OCEAN_DEBUG
	stopThreadExplicitly(15000u); // 15 seconds
#else
	stopThreadExplicitly();
#endif
}

void OfflineRelocalizationTracker6DOF::setInput(Media::FrameMediumRefs&& frameMediums)
{
	if (frameMediums.size() != 1)
	{
		Log::error() << "OfflineRelocalizationTracker needs one valid frame medium as input.";
		return;
	}

	ocean_assert(worldTracker_);

	const ScopedLock scopedLock(lock_);

	const VisualTrackerRef visualWorldTracker(worldTracker_);
	ocean_assert(visualWorldTracker);

	visualWorldTracker->setInput(Media::FrameMediumRefs(frameMediums));

	VisualTracker::setInput(std::move(frameMediums));
}

OfflineRelocalizationTracker6DOF::ObjectId OfflineRelocalizationTracker6DOF::registerObject(const std::string& description, const Vector3& dimension)
{
	const ScopedLock scopedLock(lock_);

	if (modelObjectId_ != invalidObjectId())
	{
		Log::info() << "A model has already been registered";
		return invalidObjectId();
	}

	if (!modelFeatures_.empty())
	{
		Log::info() << "We have a registered object already.";
	}

	const IO::Files files = IO::FileResolver::get().resolve(IO::File(description), true /*checkExistence*/);

	if (files.empty())
	{
		Log::info() << "The object description '" << description << "' must be a file with features";
		return invalidObjectId();
	}

	std::ifstream inputFileStream(files.front()(), std::ifstream::binary);
	IO::InputBitstream inputBitstream(inputFileStream);

	CV::Detector::Blob::BlobFeatures mapFeatures;

	if (!Tracking::Pattern::PatternTrackerCore6DOF::readFeatures(inputBitstream, modelFeatures_))
	{
		Log::info() << "The object description '" << description << "' is not a file with features";
		return invalidObjectId();
	}

	modelObjectId_ = addUniqueObjectId(description);

	return modelObjectId_;
}

const std::string& OfflineRelocalizationTracker6DOF::library() const
{
	return libraryName_;
}

bool OfflineRelocalizationTracker6DOF::isStarted() const
{
	ocean_assert(worldTracker_);

	return worldTracker_->isStarted();
}

bool OfflineRelocalizationTracker6DOF::start()
{
	ocean_assert(worldTracker_);

	if (!worldTracker_->start())
	{
		return false;
	}

	if (!worldTrackerSampleSubscription_)
	{
		// we register our event functions so that we can receive tracking events from the world tracker

		ocean_assert(!worldTrackerObjectSubscription_);

		worldTrackerSampleSubscription_ = worldTracker_->subscribeSampleEvent(SampleCallback::create(*this, &OfflineRelocalizationTracker6DOF::onWorldTrackerSample));
		worldTrackerObjectSubscription_ = worldTracker_->subscribeTrackerObjectEvent(TrackerObjectCallback::create(*this, &OfflineRelocalizationTracker6DOF::onWorldTrackerObject));
	}

	return true;
}

bool OfflineRelocalizationTracker6DOF::stop()
{
	worldTrackerSampleSubscription_.release();
	worldTrackerObjectSubscription_.release();

	if (worldTracker_)
	{
		return worldTracker_->stop();
	}

	return true;
}

void OfflineRelocalizationTracker6DOF::onWorldTrackerSample(const Measurement* /*measurement*/, const SampleRef& sample)
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

	TemporaryScopedLock temporaryScopedLock(lock_);

	if (model_T_world_.isValid())
	{
		if (isThreadActive())
		{
			// we keep storing the world poses as long as the relocalization thread is active
			world_T_cameras_.emplace(timestamp, world_T_camera);
		}

		const HomogenousMatrix4 model_T_camera = model_T_world_ * world_T_camera;

		ObjectId foundModelObjectId = invalidObjectId();

		if (!modelIsTracked_)
		{
			// first time we have a valid world pose and model pose, so we report the object to be found

			foundModelObjectId = modelObjectId_;
			modelIsTracked_ = true;
		}

		ObjectIds objectIds(1, modelObjectId_);
		Tracker6DOFSample::Positions positions(1, model_T_camera.translation());
		Tracker6DOFSample::Orientations orientations(1, model_T_camera.rotation());

		temporaryScopedLock.release();

		if (foundModelObjectId != invalidObjectId())
		{
			postFoundTrackerObjects({foundModelObjectId}, timestamp);
		}

		postNewSample(SampleRef(new Tracker6DOFSample(timestamp, RS_DEVICE_IN_OBJECT, objectIds, orientations, positions)));
	}
	else
	{
		if (!isThreadActive())
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

		world_T_cameras_.emplace(timestamp, world_T_camera);
	}
}

void OfflineRelocalizationTracker6DOF::onWorldTrackerObject(const Tracker* /*tracker*/, const bool found, const ObjectIdSet& worldObjectIds, const Timestamp& timestamp)
{
	ocean_assert(worldObjectIds.size() == 1);

	if (found)
	{
		Log::info() << "World tracking initialized";

		const ScopedLock scopedLock(lock_);

		worldTrackerInitializedTimestamp_ = timestamp;

		ocean_assert(!model_T_world_.isValid());
		ocean_assert(world_T_cameras_.empty());
		ocean_assert(!modelIsTracked_);
	}
	else
	{
		Log::info() << "World tracking lost";

		const ScopedLock scopedLock(lock_);

		stopThread();

		const ObjectId lostModelObjectId = model_T_world_.isValid() ? modelObjectId_ : invalidObjectId();

		worldTrackerInitializedTimestamp_.toInvalid();

		world_T_cameras_.clear();
		model_T_world_.toNull();
		modelIsTracked_ = false;

		if (lostModelObjectId != invalidObjectId())
		{
			// we have lost world tracking and have an active relocalization pose, so we have to report this object to be lost

			postLostTrackerObjects({lostModelObjectId}, timestamp);
		}
	}
}

void OfflineRelocalizationTracker6DOF::threadRun()
{
	TemporaryScopedLock temporaryScopedLock(deviceLock);
		if (frameMediums_.size() != 1 || frameMediums_.front().isNull())
		{
			return;
		}

		const Media::FrameMediumRef frameMedium = frameMediums_.front();
	temporaryScopedLock.release();

	constexpr unsigned int maxImageResolution = 1280u * 720u;

	RandomGenerator randomGenerator;

	HomogenousMatrix4 model_T_previousCamera(false);
	TransformationMap model_T_cameras;

	while (!shouldThreadStop())
	{
		sleep(1u);

		Timestamp firstTimestamp(false);
		HomogenousMatrix4 world_T_firstCamera(false);

		{
			const ScopedLock scopedLock(lock_);

 			ocean_assert(!modelFeatures_.empty());

			if (world_T_cameras_.empty())
			{
				continue;
			}

			firstTimestamp = world_T_cameras_.rbegin()->first; // most recent timestamp
			world_T_firstCamera = world_T_cameras_.rbegin()->second;
		}

		ocean_assert(firstTimestamp.isValid());
		ocean_assert(world_T_firstCamera.isValid());

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

		Frame yFrameToUse(yFrame, Frame::ACM_USE_KEEP_LAYOUT);
		while (yFrameToUse.width() * yFrameToUse.height() > maxImageResolution)
		{
			CV::FrameShrinker::downsampleByTwo11(yFrameToUse);
		}

		SharedAnyCamera cameraToUse = firstCamera->clone(yFrameToUse.width(), yFrameToUse.height());

		HomogenousMatrix4 model_T_firstCamera(false);
		size_t initialCorrespondences = 0;
		Vectors3 currentObjectPoints;
		Vectors2 currentImagePoints;
		Indices32 validCorrespondences;

		const Scalar faultyRate = model_T_previousCamera.isValid() ? Scalar(0.5) : Scalar(0.9); // we expect 10% inliers/matches only  if we do not have a rough pose, 50% if we have a rough pose

		if (!Tracking::Blob::BlobTracker6DOF::determinePose(yFrameToUse, *cameraToUse, model_T_firstCamera, modelFeatures_, 20, &initialCorrespondences, nullptr, faultyRate, &currentObjectPoints, &currentImagePoints, model_T_previousCamera))
		{
			continue;
		}

		if (initialCorrespondences < 70)
		{
			model_T_previousCamera.toNull();

			Log::info() << "Relocalization succeeded but only with " << initialCorrespondences << " we will re-try to determine more correspondences in the next frame";
			continue;
		}

		model_T_previousCamera = model_T_firstCamera;

		// as the model and world will have different scales, we need to track the model over several frames until we can anchor the model in the world

		ocean_assert(currentObjectPoints.size() == currentImagePoints.size());

		ocean_assert(model_T_cameras.find(firstTimestamp) == model_T_cameras.cend());
		model_T_cameras.emplace(firstTimestamp, model_T_firstCamera);

		constexpr unsigned int coarsestLayerRadius = 8u;
		constexpr unsigned int pyramidLayers = 5u;
		constexpr unsigned int subPixelIterations = 2u;

		CV::FramePyramid currentPyramid(yFrameToUse, pyramidLayers, true /*copyFirstLayer*/, nullptr /*worker*/);

		Timestamp lastFrameTimestamp(firstTimestamp);

		// now we track the 2D/3D correspondences until we have lost most of them

		CV::FramePyramid previousPyramid(std::move(currentPyramid));
		Vectors2 previousImagePoints(std::move(currentImagePoints));
		Vectors2 predictedCurrentImagePoints;

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

			currentPyramid.replace8BitPerChannel11(yFrameToUse.constdata<uint8_t>(), yFrameToUse.width(), yFrameToUse.height(), 1u, FrameType::ORIGIN_UPPER_LEFT, pyramidLayers, yFrameToUse.paddingElements(), true /*copyfirstLayer*/, nullptr);


			HomogenousMatrix4 world_T_predictedCamera(false);

			{
				const ScopedLock scopedLock(lock_);

				if (world_T_cameras_.empty())
				{
					continue;
				}

				TransformationMap::iterator iMap = world_T_cameras_.lower_bound(currentTimestamp);

				if (iMap == world_T_cameras_.cend())
				{
					world_T_predictedCamera = iMap->second;
				}
				else
				{
					world_T_predictedCamera = world_T_cameras_.rbegin()->second;
				}
			}

			ocean_assert(model_T_firstCamera.isValid() && world_T_firstCamera.isValid() && world_T_predictedCamera.isValid());

			const HomogenousMatrix4 model_T_predictedCamera(model_T_firstCamera * world_T_firstCamera.inverted() * world_T_predictedCamera);
			const HomogenousMatrix4 flippedPredictedCamera_T_model(PinholeCamera::standard2InvertedFlipped(model_T_predictedCamera));

			predictedCurrentImagePoints.clear();
			predictedCurrentImagePoints.reserve(currentObjectPoints.size());

			for (const Vector3& currentObjectPoint : currentObjectPoints)
			{
				const Vector2 predictedImagePoint(cameraToUse->projectToImageIF(flippedPredictedCamera_T_model, currentObjectPoint));

				predictedCurrentImagePoints.emplace_back(minmax<Scalar>(Scalar(0), predictedImagePoint.x(), Scalar(cameraToUse->width() - 1u)), minmax<Scalar>(Scalar(0), predictedImagePoint.y(), Scalar(cameraToUse->height() - 1u)));
			}

			currentImagePoints.clear();
			validCorrespondences.clear();
			CV::Advanced::AdvancedMotionZeroMeanSSD::trackPointsBidirectionalSubPixelMirroredBorderWithRoughLocations<1u, 31u>(previousPyramid, currentPyramid, coarsestLayerRadius, previousImagePoints, predictedCurrentImagePoints, currentImagePoints, Scalar(0.9 * 0.9), nullptr, &validCorrespondences, subPixelIterations);

			if (validCorrespondences.size() < initialCorrespondences * 5 / 100 || validCorrespondences.size() < 25) // at least 5% and 25 features
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

			ocean_assert(!model_T_cameras.empty());
			const HomogenousMatrix4 model_T_lastCamera =  model_T_cameras.rbegin()->second;

			ocean_assert(currentObjectPoints.size() == currentImagePoints.size());

			HomogenousMatrix4 model_T_currentCamera;

			validCorrespondences.clear();
			if (!Geometry::RANSAC::p3p(*cameraToUse, ConstArrayAccessor<Vector3>(currentObjectPoints), ConstArrayAccessor<Vector2>(currentImagePoints), randomGenerator, model_T_currentCamera, 25u, true, 25u, Scalar(4 * 4), &validCorrespondences))
			{
				continue;
			}

			if (validCorrespondences.size() < initialCorrespondences * 5 / 100 || validCorrespondences.size() < 25) // at least 5% and 25 features
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

			model_T_previousCamera = model_T_currentCamera;

			std::swap(previousPyramid, currentPyramid);
			std::swap(previousImagePoints, currentImagePoints);

			ocean_assert(model_T_cameras.find(currentTimestamp) == model_T_cameras.cend());
			model_T_cameras.emplace(currentTimestamp, model_T_currentCamera);


			// let's find the pairs of corresponding camera transformations
			HomogenousMatrices4 world_T_correspondingCameras;
			HomogenousMatrices4 model_T_correspondingCameras;

			{
				const ScopedLock scopedLock(lock_);
				Subset::correspondingElements<Timestamp, HomogenousMatrix4>(world_T_cameras_, model_T_cameras, world_T_correspondingCameras, model_T_correspondingCameras);
			}

			if (world_T_correspondingCameras.empty())
			{
				continue;
			}

			HomogenousMatrix4 model_T_world(false);

			Scalar scale;
			if (Geometry::AbsoluteTransformation::calculateTransformation(world_T_correspondingCameras.data(), model_T_correspondingCameras.data(), world_T_correspondingCameras.size(), model_T_world, Geometry::AbsoluteTransformation::ScaleErrorType::Symmetric, &scale))
			{
				// we do not use `model_T_world` directly, as this transformation optimizes the transformation across all corresponding transformations (this result would be a slightly misaligned/swimming AR content
				// instead, we use the scale only - and we apply the scale to the latest corresponding pose - this results in a great content registration with a fair depth/scale

				const HomogenousMatrix4& world_T_correspondingCamera = world_T_correspondingCameras.back();

				HomogenousMatrix4 model_T_lastCorrespondingCamera(model_T_correspondingCameras.back());
				model_T_lastCorrespondingCamera.applyScale(Vector3(scale, scale, scale));

				model_T_world = model_T_lastCorrespondingCamera * world_T_correspondingCamera.inverted();

				const ScopedLock scopedLock(lock_);

				if (shouldThreadStop())
				{
					return;
				}

				model_T_world_ = model_T_world;
			}
		}
	}
}

Device* OfflineRelocalizationTracker6DOF::create(const std::string& /*name*/, const Device::DeviceType& /*deviceType*/)
{
	return new OfflineRelocalizationTracker6DOF();
}


FloorTracker6DOF::FloorTracker6DOF() :
	Device(deviceName(),  deviceType()),
	Measurement(deviceName(), deviceType()),
	Tracker(deviceName(), deviceType()),
	OrientationTracker3DOF(deviceName()),
	PositionTracker3DOF(deviceName()),
	Tracker6DOF(deviceName()),
	VisualTracker(deviceName(), deviceType()),
	libraryName_(Manager::nameAdhocFactory())
{
#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	tracker6DOF_ = Devices::Manager::get().device("Floor 6DOF Tracker"); // we simply use Quest's floor tracker

#else

	#if defined(OCEAN_PLATFORM_BUILD_ANDROID)
		tracker6DOF_ = Devices::Manager::get().device("ARCore 6DOF Plane Tracker");
	#elif defined(OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY)
		tracker6DOF_ = Devices::Manager::get().device("ARKit 6DOF Plane Tracker");
	#endif

#endif

	if (tracker6DOF_)
	{
		floorObjectId_ = addUniqueObjectId("Floor");
	}
	else
	{
		deviceIsValid = false;
	}
}

FloorTracker6DOF::~FloorTracker6DOF()
{
	// nothing to do here
}

void FloorTracker6DOF::setInput(Media::FrameMediumRefs&& frameMediums)
{
	if (frameMediums.size() != 1)
	{
		Log::error() << "XRPlayground FloorTracker needs one valid frame medium as input.";
		return;
	}

	ocean_assert(tracker6DOF_);

	const ScopedLock scopedLock(deviceLock);

	const VisualTrackerRef visualTracker(tracker6DOF_);

	if (visualTracker)
	{
		visualTracker->setInput(Media::FrameMediumRefs(frameMediums));
		VisualTracker::setInput(std::move(frameMediums));
	}
	else
	{
		Log::warning() << "XRPlayground FloorTracker does not need a frame medium as input";
	}
}

const std::string& FloorTracker6DOF::library() const
{
	return libraryName_;
}

bool FloorTracker6DOF::isStarted() const
{
	const ScopedLock scopedLock(deviceLock);

	ocean_assert(tracker6DOF_);

	return tracker6DOF_->isStarted();
}

bool FloorTracker6DOF::isObjectTracked(const ObjectId& objectId) const
{
	const ScopedLock scopedLock(deviceLock);

	ocean_assert(objectId == floorObjectId_);

	return objectId == floorObjectId_ && floorIsTracked_;
}

bool FloorTracker6DOF::start()
{
	const ScopedLock scopedLock(deviceLock);

	ocean_assert(tracker6DOF_);

	if (tracker6DOF_)
	{
		if (tracker6DOF_->start())
		{
			trackerSampleEventSubscription_ = tracker6DOF_->subscribeSampleEvent(Devices::Measurement::SampleCallback::create(*this, &FloorTracker6DOF::onTrackerSample));
			return true;
		}
	}

	Log::error() << "Failed to start XRPlayground Floor Tracker";

	return false;
}

bool FloorTracker6DOF::stop()
{
	const ScopedLock scopedLock(deviceLock);

	trackerSampleEventSubscription_.release();

	if (tracker6DOF_)
	{
		return tracker6DOF_->stop();
	}

	return true;
}

void FloorTracker6DOF::onTrackerSample(const Measurement* measurement, const SampleRef& sample)
{
	const ScopedLock scopedLock(deviceLock);

	HomogenousMatrix4 transformation(false); // either world_T_floor, or floor_T_camera;

	ReferenceSystem referenceSystem = RS_OBJECT_IN_DEVICE;

	if (measurement->name() == "Floor 6DOF Tracker")
	{
		const Devices::Tracker6DOF::Tracker6DOFSampleRef trackerSample(measurement->sample());

		if (sample && !trackerSample->objectIds().empty())
		{
			world_T_recentFloor_ = HomogenousMatrix4(trackerSample->positions().front(), trackerSample->orientations().front());
			transformation = world_T_recentFloor_;
		}
	}
	else
	{
		using SceneElementPlanes = Devices::SceneTracker6DOF::SceneElementPlanes;

		const Devices::SceneTracker6DOF::SceneTracker6DOFSampleRef sceneTrackerSample(measurement->sample());

		if (sceneTrackerSample && !sceneTrackerSample->sceneElements().empty())
		{
			const HomogenousMatrix4 world_T_camera(sceneTrackerSample->positions().front(), sceneTrackerSample->orientations().front());

			if (floorTimestamp_ + 2.0 <= sample->timestamp())
			{
				const Devices::SceneTracker6DOF::SharedSceneElement& sceneElement = sceneTrackerSample->sceneElements().front();

				if (sceneElement && sceneElement->sceneElementType() == Devices::SceneTracker6DOF::SceneElement::SET_PLANES)
				{
					const Scalar yCameraTranslation = world_T_camera.translation().y();
					const Scalar yPlaneTranslationLower = yCameraTranslation - Scalar(2); // we expect the plane to be at with range [-2, -1] below the camera
					const Scalar yPlaneTranslationUpper = yCameraTranslation - Scalar(1);

					const SceneElementPlanes& sceneElementPlanes = dynamic_cast<const SceneElementPlanes&>(*sceneElement);

					const SceneElementPlanes::Planes& planes = sceneElementPlanes.planes();

					Scalar bestPlaneArea = Scalar(2 * 2); // a plane candidate must have at least 4 square meter
					Plane3 floorPlane;

					for (const SceneElementPlanes::Plane& plane : planes)
					{
						if (plane.planeType() != SceneElementPlanes::Plane::PT_HORIZONTAL)
						{
							continue;
						}

						const Scalar yPlaneTranslation = plane.world_T_plane().translation().y();

						if (yPlaneTranslation >= yPlaneTranslationLower && yPlaneTranslation <= yPlaneTranslationUpper)
						{
							ocean_assert(plane.boundingBox().yDimension() < Scalar(0.01));

							const Scalar planeArea = plane.boundingBox().xDimension() * plane.boundingBox().zDimension();

							if (planeArea > bestPlaneArea)
							{
								bestPlaneArea = planeArea;

								floorPlane = plane.worldPlane();
							}
						}
					}

					if (floorPlane.isValid())
					{
						const Vector3 translation = floorPlane.pointOnPlane();

						const Vector3 yAxis = floorPlane.normal();
						Vector3 xAxis = Vector3(1, 0, 0);//yAxis.perpendicular().normalized();
						const Vector3 zAxis = xAxis.cross(yAxis).normalized();
						xAxis = yAxis.cross(zAxis).normalized();

						const HomogenousMatrix4 world_T_floor(xAxis, yAxis, zAxis, translation);

						world_T_recentFloor_ = world_T_floor;

						floorTimestamp_ = sample->timestamp();
					}
				}
			}

			if (!world_T_recentFloor_.isValid())
			{
				return;
			}

			transformation = world_T_recentFloor_.inverted() * world_T_camera;

			referenceSystem = RS_DEVICE_IN_OBJECT;
		}
	}

	if (!transformation.isValid())
	{
		return;
	}

	ObjectIds sampleObjectIds(1, floorObjectId_);
	Tracker6DOFSample::Positions samplePositions(1, transformation.translation());
	Tracker6DOFSample::Orientations sampleOrientations(1, transformation.rotation());

	if (!floorIsTracked_)
	{
		postFoundTrackerObjects({floorObjectId_}, sample->timestamp());
	}

	postNewSample(SampleRef(new Tracker6DOFSample(sample->timestamp(), referenceSystem, std::move(sampleObjectIds), std::move(sampleOrientations), std::move(samplePositions))));
}

Device* FloorTracker6DOF::create(const std::string& /*name*/, const Device::DeviceType& /*deviceType*/)
{
	return new FloorTracker6DOF();
}


GeoAnchorTracker6DOF::GeoAnchorTracker6DOF() :
	Device(deviceName(), deviceType()),
	Measurement(deviceName(), deviceType()),
	Tracker(deviceName(), deviceType()),
	OrientationTracker3DOF(deviceName()),
	PositionTracker3DOF(deviceName()),
	Tracker6DOF(deviceName()),
	ObjectTracker(deviceName(), deviceType()),
	VisualTracker(deviceName(), deviceType()),
	libraryName_(Manager::nameAdhocFactory())
{
	const Strings deviceNames =
	{
#if defined(OCEAN_PLATFORM_BUILD_ANDROID)
		"ARCore 6DOF World Tracker",
#elif defined(OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY)
		"ARKit 6DOF World Tracker",
#endif
		"VRS ARCore 6DOF World Tracker",
		"VRS ARKit 6DOF World Tracker"
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
		Log::warning() << "GeoAnchor: Failed to access world tracker tracker!";

		deviceIsValid = false;
		return;
	}

	gpsTracker_ = Manager::get().device(Devices::GPSTracker::deviceTypeGPSTracker());

	if (gpsTracker_.isNull())
	{
		Log::warning() << "GeoAnchor: Failed to access GPS tracker!";

		deviceIsValid = false;
	}

	Log::info() << "GeoAnchor <init>";
}

GeoAnchorTracker6DOF::~GeoAnchorTracker6DOF()
{
	stop();
}

void GeoAnchorTracker6DOF::setInput(Media::FrameMediumRefs&& frameMediums)
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(worldTracker_);

	const VisualTrackerRef visualWorldTracker(worldTracker_);
	ocean_assert(visualWorldTracker);

	visualWorldTracker->setInput(Media::FrameMediumRefs(frameMediums));

	VisualTracker::setInput(std::move(frameMediums));
}

GeoAnchorTracker6DOF::ObjectId GeoAnchorTracker6DOF::registerObject(const std::string& description, const Vector3& /*dimension*/)
{
	const ScopedLock scopedLock(lock_);

	const ObjectId existingObjectId = objectId(description);

	if (existingObjectId != invalidObjectId())
	{
		return existingObjectId;
	}

	double latitude;
	double longitude;
	double altitude = NumericD::minValue();

	if (GPSTracker::parseGPSLocation(description, latitude, longitude, &altitude))
	{
		const ObjectId objectId = addUniqueObjectId(description);

		bool hasAltitude = (altitude != NumericD::minValue());

		// GeoAnchor identifier stores the objectId value
		facebook::location_platform::location::geoanchor::GeoAnchor geoAnchor =
			facebook::location_platform::location::geoanchor::GeoAnchor(
				objectId,
				"OCEAN",
				latitude,
				longitude,
				hasAltitude,
				hasAltitude ? altitude : 0.0,
				0.0,
				0.0,
				facebook::location_platform::location::algorithms::ElevationType::CAMERA);

		geoAnchorManager_->addGeoAnchor(geoAnchor);
		geoAnchorsToAdd.insert(objectId);

#ifdef OCEAN_DEBUG
		Log::info() << "GeoAnchor: registerObject: id=" << geoAnchor.identifier
					<< ", location: [" << geoAnchor.latitude << ", " << geoAnchor.longitude
					<< ", " << geoAnchor.altitude << ", " << geoAnchor.bearing << "]";
#endif

		return geoAnchor.identifier;
	}

	Log::warning() << "Failed to register GPS loation '" << description << "'";

	return invalidObjectId();
}

const std::string& GeoAnchorTracker6DOF::library() const
{
	return libraryName_;
}

bool GeoAnchorTracker6DOF::isStarted() const
{
	ocean_assert(gpsTracker_ && worldTracker_);

	return worldTracker_->isStarted() && gpsTracker_->isStarted();
}

bool GeoAnchorTracker6DOF::start()
{
	ocean_assert(gpsTracker_ && worldTracker_);

	Log::info() << "GeoAnchor start! Override Location? " << shouldOverrideGPS;

	if (!worldTracker_->start() || !gpsTracker_->start())
	{
		return false;
	}

	if (!worldTrackerSampleSubscription_)
	{
		worldTrackerSampleSubscription_ = worldTracker_->subscribeSampleEvent(SampleCallback::create(*this, &GeoAnchorTracker6DOF::onWorldTrackerSample));
	}

	if (!gpsTrackerSampleSubscription_ && !shouldOverrideGPS)
	{
		gpsTrackerSampleSubscription_ = gpsTracker_->subscribeSampleEvent(SampleCallback::create(*this, &GeoAnchorTracker6DOF::onGPSTrackerSample));
	}

	return true;
}

bool GeoAnchorTracker6DOF::stop()
{
    ocean_assert(gpsTracker_ && worldTracker_);

	Log::info() << "GeoAnchor stop!";

	gpsTrackerSampleSubscription_.release();
	worldTrackerSampleSubscription_.release();

	if (gpsTracker_)
	{
		if (!gpsTracker_->stop())
		{
			return false;
		}
	}

	if (worldTracker_)
	{
		if (!worldTracker_->stop())
		{
			return  false;
		}
	}

    return true;
}

void GeoAnchorTracker6DOF::overrideLocation(const VectorD3& location, Timestamp timestamp)
{
	if (!shouldOverrideGPS)
	{
        Log::error() << "Attempting to override location when shouldOverrideGPS is set to false.";
		return;
	}

	const ScopedLock scopedLock(lock_);
	const Timestamp& unixTimestamp = timestamp;

	facebook::location::service::LocationSignal locationSignal = facebook::location::service::createEmptySignal();
	locationSignal.latitude = location[0];
	locationSignal.longitude = location[1];
	locationSignal.horizontalAccuracy = 2.0;
	locationSignal.altitude = 0.0;
	// locationSignal.altitudeAccuracy = 0.0;
	locationSignal.bearing = location[2];
	// locationSignal.bearingAccuracy = 0.0;
	locationSignal.declination = 0.0;

#ifdef OCEAN_DEBUG
	Log::info() << "GeoAnchor: Overriden Location ==> sample: [" << locationSignal.latitude << ", " << locationSignal.longitude << " bearing: " << locationSignal.bearing.value_or(0.0) << ", t=" << double(unixTimestamp) << "]";
#endif

	geoAnchorManager_->updateGpsData(locationSignal);
	geoAnchorManager_->updateTransformerStates(cameraPose_, double(unixTimestamp));

}

void GeoAnchorTracker6DOF::onGPSTrackerSample(const Measurement* /*sender*/, const SampleRef& sample)
{
	const ScopedLock scopedLock(lock_);

	const GPSTracker::GPSTrackerSampleRef gpsTrackerSample(sample);
	ocean_assert(gpsTrackerSample);

	if (gpsTrackerSample->locations().size() != 1 || shouldOverrideGPS)
	{
		return;
	}

	const GPSTracker::Location& gpsSampleLocation = gpsTrackerSample->locations().front();

    const double longitude = gpsSampleLocation.longitude();
    const double latitude = gpsSampleLocation.latitude();
    const double altitude = gpsSampleLocation.altitude();
	double horizontalAccuracy = gpsSampleLocation.accuracy();
	double altitudeAccuracy = gpsSampleLocation.altitudeAccuracy();
	const Timestamp& unixTimestamp = gpsTrackerSample->timestamp();

#ifdef OCEAN_DEBUG
	Log::info() << "GeoAnchor: GPS ==> Location sample: [" << latitude << ", " << longitude << ", " << altitude << ", t=" << double(unixTimestamp) << "]";
#endif

	facebook::location::service::LocationSignal locationSignal = facebook::location::service::createEmptySignal();
	locationSignal.latitude = latitude;
	locationSignal.longitude = longitude;
	locationSignal.horizontalAccuracy = horizontalAccuracy;
	locationSignal.altitude = altitude;
	locationSignal.altitudeAccuracy = altitudeAccuracy;

	geoAnchorManager_->updateGpsData(locationSignal);
	geoAnchorManager_->updateTransformerStates(cameraPose_, double(unixTimestamp));
}

void GeoAnchorTracker6DOF::onWorldTrackerSample(const Measurement* /*sensor*/, const SampleRef& sample)
{
	TemporaryScopedLock temporaryScopedLock(lock_);

	const Tracker6DOFSampleRef tracker6DOFSample(sample);
	ocean_assert(tracker6DOFSample);

	if (tracker6DOFSample->objectIds().size() != 1)
	{
		return;
	}

	HomogenousMatrix4 world_T_camera(tracker6DOFSample->positions().front(), tracker6DOFSample->orientations().front());
	world_T_camera_ = world_T_camera;
	Timestamp timestamp = tracker6DOFSample->timestamp();

	cameraPose_ = SophusUtilities::toSE3<Scalar, float>(world_T_camera);

#ifdef OCEAN_DEBUG
	Log::info() << "GeoAnchor: World tracker sample: " << tracker6DOFSample->positions().front();
#endif

	temporaryScopedLock.release();

    if (shouldOverrideGPS && hasFirstReloc) {
        reportAnchorPoses(timestamp);
    } else if (!shouldOverrideGPS) {
        reportAnchorPoses(timestamp);
    }
}

void GeoAnchorTracker6DOF::reportAnchorPoses(const Timestamp& timestamp /* TODO some location service data*/)
{
	TemporaryScopedLock temporaryScopedLock(lock_);

	ObjectIds objectIds;
	Tracker6DOFSample::Positions positions;
	Tracker6DOFSample::Orientations orientations;

	bool cameraHasMoved = geoAnchorManager_->updateCameraPosition(cameraPose_);

	for (auto & [identifier, geoAnchor] : geoAnchorManager_->getGeoAnchors()) {
		// Always update Geoanchor pose for VPS (overrides GPS)
		if (shouldOverrideGPS || geoAnchorManager_->shouldUpdateGeoAnchorHorizontal(geoAnchor, cameraHasMoved)) {
			geoAnchorManager_->updateGeoAnchorPose(geoAnchor, cameraPose_, double(timestamp));
		} else {
			geoAnchorManager_->updateGeoAnchorHeightOnly(geoAnchor, cameraPose_, double(timestamp));
		}

		auto T_world_anchor = geoAnchorManager_->getGeoAnchorInterpolatedPose(
          identifier, int64_t(double(timestamp) * 1000.0));

		geoAnchorManager_->setGeoAnchorLastArWorld(identifier, T_world_anchor);

		Sophus::SE3f T_anchor_world = T_world_anchor.inverse();
		HomogenousMatrix4 anchor_T_world = SophusUtilities::toHomogenousMatrix4<float, Scalar>(T_anchor_world);

		HomogenousMatrix4 anchor_T_camera = anchor_T_world * world_T_camera_;
		objectIds.push_back(identifier);
		positions.push_back(anchor_T_camera.translation());
		orientations.push_back(anchor_T_camera.rotation());

 #ifdef OCEAN_DEBUG
		Log::info() << "GeoAnchor: Anchor id=" << geoAnchor.identifier << " pose: "
					<< anchor_T_camera.translation() << ";; " << anchor_T_camera.rotation();
		Log::info() << "GeoAnchor: Anchor id=" << geoAnchor.identifier << " anchor_T_world pose: "
					<< anchor_T_world.translation() << ";; " << anchor_T_world.rotation();
		Log::info() << "GeoAnchor: Anchor id=" << geoAnchor.identifier << " world_T_anchor pose: "
					<< anchor_T_world.inverted().translation() << ";; " << anchor_T_world.inverted().rotation();
 #endif

		if (geoAnchorsToAdd.find(identifier) != geoAnchorsToAdd.end())
		{
			postFoundTrackerObjects({identifier}, timestamp);
			geoAnchorsToAdd.erase(identifier);
		}
	}

	temporaryScopedLock.release();

	postNewSample(SampleRef(new Tracker6DOFSample(timestamp, RS_DEVICE_IN_OBJECT, objectIds, orientations, positions)));
}

Device* GeoAnchorTracker6DOF::create(const std::string& /*name*/, const Device::DeviceType& /*deviceType*/)
{
	return new GeoAnchorTracker6DOF();
}

}

}
