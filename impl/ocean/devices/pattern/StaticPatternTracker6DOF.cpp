/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/pattern/StaticPatternTracker6DOF.h"
#include "ocean/devices/pattern/PatternTracker6DOF.h"

#include "ocean/base/Subset.h"

#include "ocean/devices/Manager.h"

#include "ocean/geometry/AbsoluteTransformation.h"

namespace Ocean
{

namespace Devices
{

namespace Pattern
{

bool StaticPatternTracker6DOF::PatternTransformations::addTransformation(const HomogenousMatrix4& pattern_T_camera, const Timestamp& timestamp, const size_t maximalNumberForAlignment, const double maximalIntervalForAlignment)
{
	ocean_assert(timestamp.isValid() && pattern_T_camera.isValid());

	if (pattern_T_camera.translation().length() > maximalDistance_)
	{
		return false;
	}

	transformationMap_.emplace(timestamp, pattern_T_camera);

	// let's remove the old samples
	while (transformationMap_.size() > maximalNumberForAlignment || transformationMap_.begin()->first + maximalIntervalForAlignment < timestamp)
	{
		ocean_assert(transformationMap_.size() < 2 || transformationMap_.cbegin()->first < transformationMap_.crbegin()->first);
		transformationMap_.erase(transformationMap_.begin());
	}

	return true;
}

StaticPatternTracker6DOF::StaticPatternTracker6DOF() :
	Device(deviceNameStaticPatternTracker6DOF(),  deviceTypeStaticPatternTracker6DOF()),
	PatternDevice(deviceNameStaticPatternTracker6DOF(), deviceTypeStaticPatternTracker6DOF()),
	Measurement(deviceNameStaticPatternTracker6DOF(), deviceTypeStaticPatternTracker6DOF()),
	Tracker(deviceNameStaticPatternTracker6DOF(), deviceTypeStaticPatternTracker6DOF()),
	OrientationTracker3DOF(deviceNameStaticPatternTracker6DOF()),
	PositionTracker3DOF(deviceNameStaticPatternTracker6DOF()),
	Tracker6DOF(deviceNameStaticPatternTracker6DOF()),
	ObjectTracker(deviceNameStaticPatternTracker6DOF(), deviceTypeStaticPatternTracker6DOF()),
	VisualTracker(deviceNameStaticPatternTracker6DOF(), deviceTypeStaticPatternTracker6DOF()),
	objectIdMapper_(*this)
{
	// the static pattern tracker is mainly a wrapper around two individual trackers
	// a) the pattern tracker
	// b) the world tracker (either from ARKit or ARCore)

#if defined(_ANDROID)
	worldTracker_ = Manager::get().device("ARCore 6DOF World Tracker");
#elif defined(__APPLE__)
	worldTracker_ = Manager::get().device("ARKit 6DOF World Tracker");
#endif

	if (worldTracker_.isNull())
	{
		Log::warning() << "Failed to access world tracker tracker!";

		deviceIsValid = false;
		return;
	}

	patternTracker_ = Manager::get().device(PatternTracker6DOF::deviceNamePatternTracker6DOF());

	if (patternTracker_.isNull())
	{
		Log::warning() << "Failed to access pattern tracker!";

		deviceIsValid = false;
	}
}

StaticPatternTracker6DOF::~StaticPatternTracker6DOF()
{
	stop();
}

void StaticPatternTracker6DOF::setInput(Media::FrameMediumRefs&& frameMediums)
{
	ocean_assert(patternTracker_ && worldTracker_);

	// we simply forward the input medium to both internal trackers

	{
		const VisualTrackerRef visualPatternTracker(patternTracker_);
		ocean_assert(visualPatternTracker);

		visualPatternTracker->setInput(Media::FrameMediumRefs(frameMediums));
	}

	{
		const VisualTrackerRef visualWorldTracker(worldTracker_);
		ocean_assert(visualWorldTracker);

		visualWorldTracker->setInput(std::move(frameMediums));
	}
}

StaticPatternTracker6DOF::ObjectId StaticPatternTracker6DOF::registerObject(const std::string& description, const Vector3& dimension)
{
	ocean_assert(patternTracker_);

	// we simply forward the pattern registration to the actual pattern tracker

	const ObjectTrackerRef objectTracker(patternTracker_);
	ocean_assert(objectTracker);

	const ObjectId internalObjectId = objectTracker->registerObject(description, dimension);

	if (internalObjectId == invalidObjectId())
	{
		return invalidObjectId();
	}

	const Scalar maximalDimension = max(max(dimension.x(), dimension.y()), dimension.z());

	ocean_assert(patternTrackerTransformationsMap_.find(internalObjectId) == patternTrackerTransformationsMap_.cend());
	patternTrackerTransformationsMap_.emplace(internalObjectId, PatternTransformations(maximalDimension * Scalar(2)));

	// we convert the internal object id of the PatternTracker to a new external of the StaticPatternTracker
	return objectIdMapper_.newInternalObjectId(internalObjectId, "StaticPattern " + description);
}

bool StaticPatternTracker6DOF::isStarted() const
{
	ocean_assert(patternTracker_ && worldTracker_);

	return patternTracker_->isStarted() && worldTracker_->isStarted();
}

bool StaticPatternTracker6DOF::start()
{
	ocean_assert(patternTracker_ && worldTracker_);

	if (!patternTracker_->start() || !worldTracker_->start())
	{
		return false;
	}

	if (!patternTrackerSampleEventSubscription_)
	{
		// we register our event functions so that we can receive tracking events from both trackers

		ocean_assert(!patternTrackerObjectSubscription_);
		ocean_assert(!worldTrackerSampleEventSubscription_);
		ocean_assert(!worldTrackerObjectSubscription_);

		patternTrackerSampleEventSubscription_ = patternTracker_->subscribeSampleEvent(SampleCallback::create(*this, &StaticPatternTracker6DOF::onPatternTrackerSample));
		patternTrackerObjectSubscription_ = patternTracker_->subscribeTrackerObjectEvent(TrackerObjectCallback::create(*this, &StaticPatternTracker6DOF::onPatternTrackerObject));

		worldTrackerSampleEventSubscription_ = worldTracker_->subscribeSampleEvent(SampleCallback::create(*this, &StaticPatternTracker6DOF::onWorldTrackerSample));
		worldTrackerObjectSubscription_ = worldTracker_->subscribeTrackerObjectEvent(TrackerObjectCallback::create(*this, &StaticPatternTracker6DOF::onWorldTrackerObject));
	}

	return true;
}

bool StaticPatternTracker6DOF::stop()
{
	patternTrackerSampleEventSubscription_.release();
	patternTrackerObjectSubscription_.release();

	worldTrackerSampleEventSubscription_.release();
	worldTrackerObjectSubscription_.release();

	bool resultPatternTracker = true;
	bool resultWorldTracker = true;

	if (patternTracker_)
	{
		resultPatternTracker = patternTracker_->stop();
	}

	if (worldTracker_)
	{
		worldTracker_->stop();
	}

	return resultPatternTracker && resultWorldTracker;
}

void StaticPatternTracker6DOF::onPatternTrackerSample(const Measurement* /*measurement*/, const SampleRef& sample)
{
	const Tracker6DOFSampleRef tracker6DOFSample(sample);

	ocean_assert(tracker6DOFSample);
	ocean_assert(tracker6DOFSample->referenceSystem() == RS_DEVICE_IN_OBJECT);

	if (tracker6DOFSample->objectIds().empty())
	{
		return;
	}

	const Timestamp& timestamp = tracker6DOFSample->timestamp();

	TemporaryScopedLock scopedLock(sampleMapLock_);

	for (size_t n = 0; n < tracker6DOFSample->objectIds().size(); ++n)
	{
		const ObjectId& objectId = tracker6DOFSample->objectIds()[n];

		const HomogenousMatrix4 pattern_T_camera(tracker6DOFSample->positions()[n], tracker6DOFSample->orientations()[n]);

		PatternTransformationsMap::iterator iPattern = patternTrackerTransformationsMap_.find(objectId);
		ocean_assert(iPattern != patternTrackerTransformationsMap_.cend());

		iPattern->second.addTransformation(pattern_T_camera, timestamp, maximalNumberForAlignment_, maximalIntervalForAlignment_);
	}

	if (!worldTrackerTransformationMap_.empty())
	{
		return;
	}

	scopedLock.release();

	reportNotAlignedPoses(timestamp);
}

void StaticPatternTracker6DOF::onWorldTrackerSample(const Measurement* /*measurement*/, const SampleRef& sample)
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

	TemporaryScopedLock scopedLock(sampleMapLock_);

	worldTrackerTransformationMap_.emplace(timestamp, world_T_camera);

	// let's remove old samples
	while (worldTrackerTransformationMap_.size() > maximalNumberForAlignment_ && worldTrackerTransformationMap_.begin()->first + maximalIntervalForAlignment_ < timestamp)
	{
		ocean_assert(worldTrackerTransformationMap_.size() < 2 || worldTrackerTransformationMap_.cbegin()->first < worldTrackerTransformationMap_.crbegin()->first);
		worldTrackerTransformationMap_.erase(worldTrackerTransformationMap_.begin());
	}

	scopedLock.release();

	reportAlignedPoses(timestamp);
}

void StaticPatternTracker6DOF::onPatternTrackerObject(const Tracker* /*tracker*/, const bool found, const ObjectIdSet& internalObjectIds, const Timestamp& timestamp)
{
	ocean_assert(!internalObjectIds.empty());

	const ScopedLock scopedLock(sampleMapLock_);

	ObjectIdSet forwardFoundExternalObjectsIds;
	ObjectIdSet forwardLostExternalObjectsIds;

	for (const ObjectId& internalObjectId : internalObjectIds)
	{
		if (found)
		{
			ocean_assert(foundPatternTrackerObjects_.find(internalObjectId) == foundPatternTrackerObjects_.cend());
			foundPatternTrackerObjects_.insert(internalObjectId);

			lostPatternTrackerObjects_.erase(internalObjectId);
		}
		else
		{
			ocean_assert(lostPatternTrackerObjects_.find(internalObjectId) == lostPatternTrackerObjects_.cend());
			lostPatternTrackerObjects_.insert(internalObjectId);

			ocean_assert(foundPatternTrackerObjects_.find(internalObjectId) != foundPatternTrackerObjects_.cend());
			foundPatternTrackerObjects_.erase(internalObjectId);
		}

		const ObjectId externalObjectId = objectIdMapper_.externalObjectIdFromInternalObjectId(internalObjectId);

		if (worldTrackerTransformationMap_.empty())
		{
			// world tracking is not active
			// we directly forward lost/found events

			ocean_assert(worldTrackedPatternObjects_.empty());

			if (found)
			{
				forwardFoundExternalObjectsIds.emplace(externalObjectId);
			}
			else
			{
				forwardLostExternalObjectsIds.emplace(externalObjectId);
			}
		}
		else
		{
			// world tracking is active
			// we forward the found event only if the pattern shows up for the first time since the world tracker initialized

			if (found)
			{
				if (worldTrackedPatternObjects_.find(internalObjectId) == worldTrackedPatternObjects_.cend())
				{
					// the object is currently not rated as world tracked

					worldTrackedPatternObjects_.insert(internalObjectId);
					forwardFoundExternalObjectsIds.emplace(externalObjectId);
				}
			}
			else
			{
				// the pattern object is lost, but world tracking is still active, so we keep the world-tracking state
				ocean_assert(worldTrackedPatternObjects_.find(internalObjectId) != worldTrackedPatternObjects_.cend());
			}
		}
	}

#ifdef OCEAN_DEBUG
	for (const ObjectId& foundObjectId : forwardFoundExternalObjectsIds)
	{
		ocean_assert(forwardLostExternalObjectsIds.find(foundObjectId) == forwardLostExternalObjectsIds.cend());
	}
	for (const ObjectId& lostObjectId : forwardLostExternalObjectsIds)
	{
		ocean_assert(forwardFoundExternalObjectsIds.find(lostObjectId) == forwardFoundExternalObjectsIds.cend());
	}
#endif

	postFoundTrackerObjects(forwardFoundExternalObjectsIds, timestamp);
	postLostTrackerObjects(forwardLostExternalObjectsIds, timestamp);
}

void StaticPatternTracker6DOF::onWorldTrackerObject(const Tracker* /*tracker*/, const bool found, const ObjectIdSet& worldObjectIds, const Timestamp& timestamp)
{
	ocean_assert_and_suppress_unused(worldObjectIds.size() == 1, worldObjectIds);

	const ScopedLock scopedLock(sampleMapLock_);

	if (found)
	{
		Log::info() << "World tracking initialized";

		// now as world tracking is initialized, we can transfer all actively tracked patterns to the world tracker

		ocean_assert(worldTrackedPatternObjects_.empty());

		for (const ObjectId& patternObjectId : foundPatternTrackerObjects_)
		{
			worldTrackedPatternObjects_.insert(patternObjectId);
		}
	}
	else
	{
		Log::info() << "World tracking lost";

		// as world tracking is lost, we need to check which pattern objects needs to be reported as lost as well (all patterns which are currently not actively tracked)

		ObjectIdSet lostExternalObjectIds;

		for (const ObjectId& patternObjectId : worldTrackedPatternObjects_)
		{
			if (lostPatternTrackerObjects_.find(patternObjectId) != lostPatternTrackerObjects_.cend())
			{
				const ObjectId externalObjectId = objectIdMapper_.externalObjectIdFromInternalObjectId(patternObjectId);

				lostExternalObjectIds.emplace(externalObjectId);
			}
		}

		postLostTrackerObjects(lostExternalObjectIds, timestamp);

		worldTrackerTransformationMap_.clear();
		worldTrackedPatternObjects_.clear();
	}
}

void StaticPatternTracker6DOF::reportNotAlignedPoses(const Timestamp& timestamp)
{
	// the world tracker is not yet initialized, so that we have to report all poses solely based on the pattern tracker

	TemporaryScopedLock scopedLock(sampleMapLock_);

	ObjectIds externalObjectIds;
	Tracker6DOFSample::Positions positions;
	Tracker6DOFSample::Orientations orientations;

	externalObjectIds.reserve(patternTrackerTransformationsMap_.size());
	positions.reserve(patternTrackerTransformationsMap_.size());
	orientations.reserve(patternTrackerTransformationsMap_.size());

	// we do not have a world tracker result for the current timestamp (perhaps world tracking is not initialized yet)

	for (PatternTransformationsMap::iterator iPattern = patternTrackerTransformationsMap_.begin(); iPattern != patternTrackerTransformationsMap_.end(); ++iPattern)
	{
		const ObjectId& internalObjectId = iPattern->first;
		const TransformationMap& patternTrackerTransformationMap = iPattern->second.transformations();

		const TransformationMap::const_iterator iPatternTransform = patternTrackerTransformationMap.find(timestamp);

		if (iPatternTransform != patternTrackerTransformationMap.cend())
		{
			const HomogenousMatrix4& pattern_T_camera = iPatternTransform->second;

			const ObjectId externalObjectId = objectIdMapper_.externalObjectIdFromInternalObjectId(internalObjectId);

			externalObjectIds.emplace_back(externalObjectId);
			positions.emplace_back(pattern_T_camera.translation());
			orientations.emplace_back(pattern_T_camera.rotation());
		}

		iPattern->second.pattern_T_world().toNull();
	}

	scopedLock.release();

	postNewSample(SampleRef(new Tracker6DOFSample(timestamp, RS_DEVICE_IN_OBJECT, externalObjectIds, orientations, positions)));
}

void StaticPatternTracker6DOF::reportAlignedPoses(const Timestamp& timestamp)
{
	// world tracker is initialized, so we have to align world tracking results with the pattern tracking results

	TemporaryScopedLock scopedLock(sampleMapLock_);

	ObjectIds externalObjectIds;
	Tracker6DOFSample::Positions positions;
	Tracker6DOFSample::Orientations orientations;

	externalObjectIds.reserve(patternTrackerTransformationsMap_.size());
	positions.reserve(patternTrackerTransformationsMap_.size());
	orientations.reserve(patternTrackerTransformationsMap_.size());

	const TransformationMap::const_iterator iWorldTransformation = worldTrackerTransformationMap_.find(timestamp);

	if (iWorldTransformation == worldTrackerTransformationMap_.cend())
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	const HomogenousMatrix4& world_T_camera = iWorldTransformation->second;

	HomogenousMatrices4 world_T_cameras;
	HomogenousMatrices4 pattern_T_cameras;

	world_T_cameras.reserve(world_T_cameras.size());
	pattern_T_cameras.reserve(world_T_cameras.size());

	for (PatternTransformationsMap::iterator iPattern = patternTrackerTransformationsMap_.begin(); iPattern != patternTrackerTransformationsMap_.end(); ++iPattern)
	{
		const ObjectId& internalObjectId = iPattern->first;
		HomogenousMatrix4& pattern_T_world = iPattern->second.pattern_T_world();
		const TransformationMap& patternTrackerTransformationMap = iPattern->second.transformations();

		world_T_cameras.clear();
		pattern_T_cameras.clear();

		Subset::correspondingElements<Timestamp, HomogenousMatrix4>(worldTrackerTransformationMap_, patternTrackerTransformationMap, world_T_cameras, pattern_T_cameras);

		if (!world_T_cameras.empty())
		{
			Scalar scale;
			if (Geometry::AbsoluteTransformation::calculateTransformation(world_T_cameras.data(), pattern_T_cameras.data(), world_T_cameras.size(), pattern_T_world, Geometry::AbsoluteTransformation::ScaleErrorType::Symmetric, &scale))
			{
				pattern_T_world.applyScale(Vector3(scale, scale, scale));
			}
		}

		if (pattern_T_world.isValid())
		{
			const HomogenousMatrix4 pattern_T_camera = pattern_T_world * world_T_camera;

			const ObjectId externalObjectId = objectIdMapper_.externalObjectIdFromInternalObjectId(internalObjectId);

			externalObjectIds.emplace_back(externalObjectId);
			positions.emplace_back(pattern_T_camera.translation());
			orientations.emplace_back(pattern_T_camera.rotation());
		}
	}

	scopedLock.release();

	postNewSample(SampleRef(new Tracker6DOFSample(timestamp, RS_DEVICE_IN_OBJECT, externalObjectIds, orientations, positions)));
}

}

}

}
