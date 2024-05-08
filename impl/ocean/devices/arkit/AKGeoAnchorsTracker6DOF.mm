/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/arkit/AKGeoAnchorsTracker6DOF.h"

#include "ocean/base/StringApple.h"

#include "ocean/devices/GPSTracker.h"

#include "ocean/media/avfoundation/AVFLiveVideo.h"

namespace Ocean
{

namespace Devices
{

namespace ARKit
{

AKGeoAnchorsTracker6DOF::AKGeoAnchorsTracker6DOF() :
	Device(deviceNameAKGeoAnchorsTracker6DOF(), deviceTypeAKGeoAnchorsTracker6DOF()),
	AKDevice(TrackerCapabilities(TC_SLAM | TC_GEO_ANCHORS), deviceNameAKGeoAnchorsTracker6DOF(), deviceTypeAKGeoAnchorsTracker6DOF()),
	Measurement(deviceNameAKGeoAnchorsTracker6DOF(), deviceTypeAKGeoAnchorsTracker6DOF()),
	Tracker(deviceNameAKGeoAnchorsTracker6DOF(), deviceTypeAKGeoAnchorsTracker6DOF()),
	OrientationTracker3DOF(deviceNameAKGeoAnchorsTracker6DOF()),
	PositionTracker3DOF(deviceNameAKGeoAnchorsTracker6DOF()),
	Tracker6DOF(deviceNameAKGeoAnchorsTracker6DOF()),
	ObjectTracker(deviceNameAKGeoAnchorsTracker6DOF(), deviceTypeAKGeoAnchorsTracker6DOF()),
	VisualTracker(deviceNameAKGeoAnchorsTracker6DOF(), deviceTypeAKGeoAnchorsTracker6DOF())
{
	// nothing to do here
}

AKGeoAnchorsTracker6DOF::~AKGeoAnchorsTracker6DOF()
{
	const ScopedLock scopedLock(deviceLock);

	stop();
}

bool AKGeoAnchorsTracker6DOF::start()
{
	const ScopedLock scopedLock(deviceLock);

	if (frameMediums_.size() != 1 || frameMediums_.front().isNull())
	{
		Log::error() << "AKGeoAnchorsTracker6DOF needs one valid input medium before it can be started";
		return false;
	}

	if (isStarted_ && intermediateLocations_.empty())
	{
		return true;
	}

	isStarted_ = ARSessionManager::get().start(this, frameMediums_.front());

	for (const Location& location : intermediateLocations_)
	{
		if (!ARSessionManager::get().registerGeoAnchor(location.objectId_, location.latitude_, location.longitude_, location.altitude_))
		{
			Log::warning() << "Failed to post-register anchor '" << objectDescription(location.objectId_) << "'";
		}
	}
	intermediateLocations_.clear();

	return isStarted_;
}

bool AKGeoAnchorsTracker6DOF::pause()
{
	TemporaryScopedLock scopedLock(deviceLock);

	if (!ARSessionManager::get().pause(this))
	{
		return false;
	}

	ObjectIdSet lostObjects(std::move(trackedObjectIds_));
	trackedObjectIds_ = ObjectIdSet();

	scopedLock.release();

	postLostTrackerObjects(lostObjects, Timestamp(true));

	return true;
}

bool AKGeoAnchorsTracker6DOF::stop()
{
	TemporaryScopedLock scopedLock(deviceLock);

	if (!isStarted_)
	{
		return true;
	}

	if (!ARSessionManager::get().stop(this))
	{
		return false;
	}

	ObjectIdSet lostObjects(std::move(trackedObjectIds_));
	trackedObjectIds_ = ObjectIdSet();

	isStarted_ = false;

	scopedLock.release();

	postLostTrackerObjects(lostObjects, Timestamp(true));

	return true;
}

bool AKGeoAnchorsTracker6DOF::isObjectTracked(const ObjectId& objectId) const
{
	const ScopedLock scopedLock(deviceLock);

	return trackedObjectIds_.find(objectId) != trackedObjectIds_.cend();
}

void AKGeoAnchorsTracker6DOF::setInput(Media::FrameMediumRefs&& frameMediums)
{
	if (frameMediums.size() == 1 && frameMediums.front())
	{
		const Media::FrameMediumRef& frameMedium = frameMediums.front();

		if (frameMedium->library() == Media::AVFoundation::nameAVFLibrary() && (frameMedium->type() & Media::Medium::LIVE_VIDEO) == Media::Medium::LIVE_VIDEO)
		{
			if (frameMedium->url() == "Back Camera" || frameMedium->url() == "Front Camera")
			{
				VisualTracker::setInput(std::move(frameMediums));
				return;
			}
		}
	}

	ocean_assert(false && "Invalid frameMedium!");
}

AKGeoAnchorsTracker6DOF::ObjectId AKGeoAnchorsTracker6DOF::registerObject(const std::string& description, const Vector3& /*dimension*/)
{
	ocean_assert(!description.empty());

	const ScopedLock scopedLock(deviceLock);

	const ObjectId existingObjectId = objectId(description);

	if (existingObjectId != invalidObjectId())
	{
		return existingObjectId;
	}

	double latitude = NumericD::minValue();
	double longitude = NumericD::minValue();
	double altitude = NumericD::minValue();

	if (!GPSTracker::parseGPSLocation(description, latitude, longitude, &altitude))
	{
		Log::warning() << "Failed to register object '" << description << "' as it is not a valid GPS location with pattern 'GPS Location <latitude>, <longitude>', or 'GPS Location <latitude>, <longitude>, <altitude>'";
		return invalidObjectId();
	}

	if (latitude < -90.0 || latitude > 90.0 || longitude < -180.0 || longitude > 180.0)
	{
		Log::warning() << "Failed to register object '" << description << "' as the location is outside of a valid range [-90, 90], [-180, 180]";
		return invalidObjectId();
	}

	if (altitude != NumericD::minValue() && (altitude < -1000.0 || altitude > 30000.0))
	{
		Log::warning() << "Failed to register object '" << description << "' as the location's altitude is outside of a valid range [-1.000, 30.000]";
		return invalidObjectId();
	}

	const ObjectId newObjectId = addUniqueObjectId(description);

	if (isStarted_)
	{
		if (ARSessionManager::get().registerGeoAnchor(newObjectId, latitude, longitude, altitude))
		{
			return newObjectId;
		}
	}
	else
	{
		intermediateLocations_.emplace_back(newObjectId, latitude, longitude, altitude);

		return newObjectId;
	}

	Log::warning() << "Failed to register GPS location '" << description << "'";

	return invalidObjectId();
}

bool AKGeoAnchorsTracker6DOF::unregisterObject(const ObjectId objectId)
{
	const ScopedLock scopedLock(deviceLock);

	return ARSessionManager::get().unregisterGeoAnchor(objectId);
}

void AKGeoAnchorsTracker6DOF::onNewSample(const HomogenousMatrix4& world_T_camera, const HomogenousMatrix4& world_T_rotatedWorld, const Timestamp& timestamp, ARFrame* arFrame)
{
	static_assert(ARGeoTrackingAccuracyHigh >= ARGeoTrackingAccuracyMedium, "Invalid accuracy");

	ocean_assert(arFrame != nullptr);

	Metadata metadata;

	if (arFrame.lightEstimate != nullptr)
	{
		metadata["ambientColorTemperature"] = Value(arFrame.lightEstimate.ambientColorTemperature);
		metadata["ambientIntensity"] = Value(arFrame.lightEstimate.ambientIntensity);
	}

	if (world_T_camera.isValid() && arFrame.geoTrackingStatus.accuracy >= ARGeoTrackingAccuracyMedium)
	{
		ocean_assert(arFrame.camera.trackingState == ARTrackingStateNormal);

		simd_float4x4 simdTransform = arFrame.camera.transform;
		HomogenousMatrixF4 world_T_anchor;

		AKGeoAnchorsTracker6DOF::TransformationMap anchors_T_camera;

		for (ARAnchor* anchor in arFrame.anchors)
		{
			if (![anchor isKindOfClass:[ARGeoAnchor class]])
			{
				continue;
			}

			ARGeoAnchor* geoAnchor = (ARGeoAnchor*)(anchor);

			const std::string anchorName = StringApple::toUTF8(geoAnchor.name);

			int objectId = -1;
			if (String::isInteger32(anchorName, &objectId) && objectId >= 0)
			{
				simdTransform = geoAnchor.transform;

				memcpy(world_T_anchor.data() +  0, &simdTransform.columns[0], sizeof(float) * 4);
				memcpy(world_T_anchor.data() +  4, &simdTransform.columns[1], sizeof(float) * 4);
				memcpy(world_T_anchor.data() +  8, &simdTransform.columns[2], sizeof(float) * 4);
				memcpy(world_T_anchor.data() + 12, &simdTransform.columns[3], sizeof(float) * 4);

				ocean_assert(world_T_anchor.isValid());

				if (!world_T_anchor.isIdentity())
				{
					ocean_assert(anchors_T_camera.find(Devices::Measurement::ObjectId(objectId)) == anchors_T_camera.cend());

					const HomogenousMatrix4 anchor_T_world((world_T_rotatedWorld * HomogenousMatrix4(world_T_anchor)).inverted());

					anchors_T_camera.emplace(Devices::Measurement::ObjectId(objectId), anchor_T_world * world_T_camera);
				}
			}
		}

		onNewSample(anchors_T_camera, timestamp, std::move(metadata));
	}
	else
	{
		onNewSample({}, timestamp, std::move(metadata));
	}
}

void AKGeoAnchorsTracker6DOF::onNewSample(const TransformationMap& anchors_T_camera, const Timestamp& timestamp, Metadata&& metadata)
{
	TemporaryScopedLock scopedLock(deviceLock);

	ObjectIdSet lostObjectIds;
	ObjectIdSet foundObjectIds;

	ObjectIds sampleObjectIds;
	Tracker6DOFSample::Positions samplePositions;
	Tracker6DOFSample::Orientations sampleOrientations;

	sampleObjectIds.reserve(anchors_T_camera.size());
	samplePositions.reserve(anchors_T_camera.size());
	sampleOrientations.reserve(anchors_T_camera.size());

	for (TransformationMap::const_iterator i = anchors_T_camera.cbegin(); i != anchors_T_camera.cend(); ++i)
	{
		const ObjectId& objectId = i->first;
		const HomogenousMatrix4& anchor_T_camera = i->second;
		ocean_assert(anchor_T_camera.isValid());

		if (trackedObjectIds_.find(objectId) == trackedObjectIds_.cend())
		{
			foundObjectIds.insert(objectId);
			trackedObjectIds_.insert(objectId);
		}

		sampleObjectIds.emplace_back(objectId);
		samplePositions.emplace_back(anchor_T_camera.translation());
		sampleOrientations.emplace_back(anchor_T_camera.rotation());
	}

	for (ObjectIdSet::iterator iObject = trackedObjectIds_.begin(); iObject != trackedObjectIds_.end(); /* noop*/)
	{
		if (anchors_T_camera.find(*iObject) == anchors_T_camera.cend())
		{
			lostObjectIds.insert(*iObject);

			iObject = trackedObjectIds_.erase(iObject);
		}
		else
		{
			++iObject;
		}
	}

	scopedLock.release();

	postFoundTrackerObjects(foundObjectIds, timestamp);

	if (!sampleObjectIds.empty())
	{
		postNewSample(SampleRef(new Tracker6DOFSample(timestamp, RS_DEVICE_IN_OBJECT, std::move(sampleObjectIds), std::move(sampleOrientations), std::move(samplePositions), std::move(metadata))));
	}

	postLostTrackerObjects(lostObjectIds, timestamp);
}

}

}

}
