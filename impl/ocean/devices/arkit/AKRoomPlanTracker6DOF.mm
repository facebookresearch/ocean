/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/arkit/AKRoomPlanTracker6DOF.h"

#include "ocean/devices/arkit/roomplan/native/AKRoomPlanTracker6DOFOwnerContainer.h"

#include "ocean/media/avfoundation/AVFoundation.h"

namespace Ocean
{

namespace Devices
{

namespace ARKit
{

AKRoomPlanTracker6DOF::AKRoomPlanTracker6DOF() :
	Device(deviceNameAKRoomPlanTracker6DOF(), deviceTypeAKRoomPlanTracker6DOF()),
	Measurement(deviceNameAKRoomPlanTracker6DOF(), deviceTypeAKRoomPlanTracker6DOF()),
	Tracker(deviceNameAKRoomPlanTracker6DOF(), deviceTypeAKRoomPlanTracker6DOF()),
	OrientationTracker3DOF(deviceNameAKRoomPlanTracker6DOF()),
	PositionTracker3DOF(deviceNameAKRoomPlanTracker6DOF()),
	Tracker6DOF(deviceNameAKRoomPlanTracker6DOF()),
	SceneTracker6DOF(deviceNameAKRoomPlanTracker6DOF()),
	VisualTracker(deviceNameAKRoomPlanTracker6DOF(), deviceTypeAKRoomPlanTracker6DOF())
{
	worldObjectId_ = addUniqueObjectId("World");
}

AKRoomPlanTracker6DOF::~AKRoomPlanTracker6DOF()
{
	const ScopedLock scopedLock(deviceLock);

	stop();
}

bool AKRoomPlanTracker6DOF::start()
{
	const ScopedLock scopedLock(deviceLock);

	if (isStarted_)
	{
		return true;
	}

	if (frameMediums_.size() == 1 && frameMediums_.front())
	{
		if (akRoomPlanTracker_ == nullptr)
		{
			akRoomPlanTracker_ = [AKRoomPlanTracker6DOF_Objc new];

			AKRoomPlanTracker6DOFOwnerContainer* ownerContainer = [AKRoomPlanTracker6DOFOwnerContainer new];
			ownerContainer.owner_ = this;
			ownerContainer.inputLiveVideo_ = frameMediums_.front();

			isStarted_ = [akRoomPlanTracker_ start:ownerContainer];
		}
	}
	else
	{
		Log::error() << "AKRoomPlanTracker6DOF needs a valid input medium before it can be started";
	}

	return isStarted_;
}

bool AKRoomPlanTracker6DOF::pause()
{
	return stop();
}

bool AKRoomPlanTracker6DOF::stop()
{
	TemporaryScopedLock scopedLock(deviceLock);

	if (!isStarted_)
	{
		return true;
	}

	ocean_assert(akRoomPlanTracker_ != nullptr);

	if (![akRoomPlanTracker_ stop])
	{
		return false;
	}

	if (!worldIsTracked_)
	{
		return true;
	}

	ObjectIdSet lostObjects = {worldObjectId_};
	worldIsTracked_ = false;

	isStarted_ = false;

	scopedLock.release();

	postLostTrackerObjects(lostObjects, Timestamp(true));

	return true;
}

bool AKRoomPlanTracker6DOF::isObjectTracked(const ObjectId& objectId) const
{
	const ScopedLock scopedLock(deviceLock);

	ocean_assert(objectId == worldObjectId_);

	return worldIsTracked_ && objectId == worldObjectId_;
}

void AKRoomPlanTracker6DOF::setInput(Media::FrameMediumRefs&& frameMediums)
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

const std::string& AKRoomPlanTracker6DOF::library() const
{
	return nameARKitLibrary();
}

bool AKRoomPlanTracker6DOF::isSupported()
{
	return [AKRoomPlanTracker6DOF_Objc isSupported];
}

void AKRoomPlanTracker6DOF::onNewSample(const HomogenousMatrix4& world_T_camera, const Timestamp& timestamp, ARFrame* arFrame)
{
	ocean_assert(arFrame != nullptr);

	TemporaryScopedLock scopedLock(deviceLock);

	if (world_T_camera.isValid())
	{
		ocean_assert(worldObjectId_ != invalidObjectId());

		ObjectIdSet foundObjectIds;

		ObjectIds sampleObjectIds(1, worldObjectId_);
		Tracker6DOFSample::Positions samplePositions(1, world_T_camera.translation());
		Tracker6DOFSample::Orientations sampleOrientations(1, world_T_camera.rotation());

		SharedSceneElements sceneElements(1, std::make_shared<SceneElementRoom>(SceneElementRoom::RoomObjectMap(roomObjectMap_), std::move(addedRoomObjects_), std::move(removedRoomObjects_), std::move(changedRoomObjects_), std::move(updatedRoomObjects_)));

		addedRoomObjects_ = SceneElementRoom::RoomObjectIdentifierSet();
		removedRoomObjects_ = SceneElementRoom::RoomObjectIdentifierSet();
		changedRoomObjects_ = SceneElementRoom::RoomObjectIdentifierSet();
		updatedRoomObjects_ = SceneElementRoom::RoomObjectIdentifierSet();

		if (!worldIsTracked_)
		{
			foundObjectIds = {worldObjectId_};
			worldIsTracked_ = true;
		}

		const InstructionValue instructionValue(instructionValue_);
		instructionValue_ = IV_UNKNOWN;

		scopedLock.release();

		postFoundTrackerObjects(foundObjectIds, timestamp);

		Metadata metadata;

		if (instructionValue != IV_UNKNOWN)
		{
			metadata["instruction"] = Value(translateInstruction(instructionValue));
		}

		if (arFrame.lightEstimate != nullptr)
		{
			metadata["ambientColorTemperature"] = Value(arFrame.lightEstimate.ambientColorTemperature);
			metadata["ambientIntensity"] = Value(arFrame.lightEstimate.ambientIntensity);
		}

		postNewSample(SampleRef(new SceneTracker6DOFSample(timestamp, RS_DEVICE_IN_OBJECT, std::move(sampleObjectIds), std::move(sampleOrientations), std::move(samplePositions), std::move(sceneElements), std::move(metadata))));
	}
	else
	{
		ObjectIdSet lostObjectIds;

		if (worldIsTracked_)
		{
			lostObjectIds = {worldObjectId_};
			worldIsTracked_ = false;
		}

		scopedLock.release();

		postLostTrackerObjects(lostObjectIds, timestamp);
	}
}

void AKRoomPlanTracker6DOF::onCaptureSessionStarted()
{
	Log::debug() << "AKRoomPlanTracker6DOF::onCaptureSessionStarted()";
}

void AKRoomPlanTracker6DOF::onCaptureSessionAdded(const SharedPlanarRoomObjects& planarRoomElements, const SharedVolumetricRoomObjects& volumetricRoomElements)
{
	const ScopedLock scopedLock(deviceLock);

	for (const SharedPlanarRoomObject& planarRoomElement : planarRoomElements)
	{
		ocean_assert(roomObjectMap_.find(planarRoomElement->identifier()) == roomObjectMap_.cend());

		roomObjectMap_.emplace(planarRoomElement->identifier(), planarRoomElement);
		addedRoomObjects_.emplace(planarRoomElement->identifier());
	}

	for (const SharedVolumetricRoomObject& volumetricRoomElement : volumetricRoomElements)
	{
		ocean_assert(roomObjectMap_.find(volumetricRoomElement->identifier()) == roomObjectMap_.cend());

		roomObjectMap_.emplace(volumetricRoomElement->identifier(), volumetricRoomElement);
		addedRoomObjects_.emplace(volumetricRoomElement->identifier());
	}
}

void AKRoomPlanTracker6DOF::onCaptureSessionRemoved(const SharedPlanarRoomObjects& planarRoomElements, const SharedVolumetricRoomObjects& volumetricRoomElements)
{
	const ScopedLock scopedLock(deviceLock);

	for (const SharedPlanarRoomObject& planarRoomElement : planarRoomElements)
	{
		ocean_assert(roomObjectMap_.find(planarRoomElement->identifier()) != roomObjectMap_.cend());

		roomObjectMap_.erase(planarRoomElement->identifier());
		removedRoomObjects_.emplace(planarRoomElement->identifier());

		// ensuring that the room has not been added before without forwarding the result
		addedRoomObjects_.erase(planarRoomElement->identifier());
		changedRoomObjects_.erase(planarRoomElement->identifier());
		updatedRoomObjects_.erase(planarRoomElement->identifier());
	}

	for (const SharedVolumetricRoomObject& volumetricRoomElement : volumetricRoomElements)
	{
		ocean_assert(roomObjectMap_.find(volumetricRoomElement->identifier()) != roomObjectMap_.cend());

		roomObjectMap_.erase(volumetricRoomElement->identifier());
		removedRoomObjects_.emplace(volumetricRoomElement->identifier());

		// ensuring that the room has not been added before without forwarding the result
		addedRoomObjects_.erase(volumetricRoomElement->identifier());
		changedRoomObjects_.erase(volumetricRoomElement->identifier());
		updatedRoomObjects_.erase(volumetricRoomElement->identifier());
	}
}

void AKRoomPlanTracker6DOF::onCaptureSessionChanged(const SharedPlanarRoomObjects& planarRoomElements, const SharedVolumetricRoomObjects& volumetricRoomElements)
{
	const ScopedLock scopedLock(deviceLock);

	for (const SharedPlanarRoomObject& planarRoomElement : planarRoomElements)
	{
		ocean_assert(roomObjectMap_.find(planarRoomElement->identifier()) != roomObjectMap_.cend());

		roomObjectMap_[planarRoomElement->identifier()] = planarRoomElement;
		changedRoomObjects_.emplace(planarRoomElement->identifier());
	}

	for (const SharedVolumetricRoomObject& volumetricRoomElement : volumetricRoomElements)
	{
		ocean_assert(roomObjectMap_.find(volumetricRoomElement->identifier()) != roomObjectMap_.cend());

		roomObjectMap_[volumetricRoomElement->identifier()] = volumetricRoomElement;
		changedRoomObjects_.emplace(volumetricRoomElement->identifier());
	}
}

void AKRoomPlanTracker6DOF::onCaptureSessionUpdated(const SharedPlanarRoomObjects& planarRoomElements, const SharedVolumetricRoomObjects& volumetricRoomElements)
{
	const ScopedLock scopedLock(deviceLock);

	for (const SharedPlanarRoomObject& planarRoomElement : planarRoomElements)
	{
		ocean_assert(roomObjectMap_.find(planarRoomElement->identifier()) != roomObjectMap_.cend());

		roomObjectMap_[planarRoomElement->identifier()] = planarRoomElement;
		updatedRoomObjects_.emplace(planarRoomElement->identifier());
	}

	for (const SharedVolumetricRoomObject& volumetricRoomElement : volumetricRoomElements)
	{
		ocean_assert(roomObjectMap_.find(volumetricRoomElement->identifier()) != roomObjectMap_.cend());

		roomObjectMap_[volumetricRoomElement->identifier()] = volumetricRoomElement;
		updatedRoomObjects_.emplace(volumetricRoomElement->identifier());
	}
}

void AKRoomPlanTracker6DOF::onCaptureSessionInstruction(const InstructionValue instructionValue)
{
	const ScopedLock scopedLock(deviceLock);

	instructionValue_ = instructionValue;
}

void AKRoomPlanTracker6DOF::onCaptureSessionStopped()
{
	Log::debug() << "AKRoomPlanTracker6DOF::onCaptureSessionStopped()";
}

}

}

}
