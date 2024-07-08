/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/SceneTracker6DOF.h"

namespace Ocean
{

namespace Devices
{

std::string SceneTracker6DOF::SceneElementRoom::RoomObject::translateObjectType(const ObjectType objectType)
{
	switch (objectType)
	{
		case OT_UNDEFINED:
			return "UNDEFINED";
			break;

		case OT_PLANAR:
			return "PLANAR";

		case OT_VOLUMETRIC:
			return "VOLUMETRIC";
	}

	ocean_assert(false && "Invalid type");
	return "UNDEFINED";
}

SceneTracker6DOF::SceneElementRoom::RoomObject::ObjectType SceneTracker6DOF::SceneElementRoom::RoomObject::translateObjectType(const std::string& objectType)
{
	if (objectType == "UNDEFINED")
	{
		return OT_UNDEFINED;
	}
	else if (objectType == "PLANAR")
	{
		return OT_PLANAR;
	}
	else if (objectType == "VOLUMETRIC")
	{
		return OT_VOLUMETRIC;
	}

	ocean_assert(false && "Invalid type");
	return OT_UNDEFINED;
}

std::string SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::translatePlanarType(const PlanarType planarType)
{
	switch (planarType)
	{
		case PT_UNKNOWN:
			break;

		case PT_WALL:
			return "wall";

		case PT_DOOR:
			return "door";

		case PT_WINDOW:
			return "window";

		case PT_OPENING:
			return "opening";

		case PT_FLOOR:
			return "floor";

		case PT_END:
			ocean_assert(false && "Must not be used");
			break;
	}

	ocean_assert(false && "Invalid type!");
	return "UNKNOWN";
}

SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::PlanarType SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::translatePlanarType(const std::string& planarType)
{
	if (planarType == "wall")
	{
		return PT_WALL;
	}
	else if (planarType == "door")
	{
		return PT_DOOR;
	}
	else if (planarType == "window")
	{
		return PT_WINDOW;
	}
	else if (planarType == "opening")
	{
		return PT_OPENING;
	}
	else if (planarType == "floor")
	{
		return PT_FLOOR;
	}
	else if (planarType == "UNKNOWN")
	{
		return PT_UNKNOWN;
	}

	ocean_assert(false && "Invalid type!");
	return PT_UNKNOWN;
}

std::string SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::translateVolumetricType(const VolumetricType volumetricType)
{
	switch (volumetricType)
	{
		case VT_UNKNOWN:
			break;

		case VT_STORAGE:
			return "storage";

		case VT_REFRIGERATOR:
			return "refrigerator";

		case VT_STOVE:
			return "stove";

		case VT_BED:
			return "bed";

		case VT_SINK:
			return "sink";

		case VT_WASHER_DRYER:
			return "washer_dryer";

		case VT_TOILET:
			return "toilet";

		case VT_BATHTUB:
			return "bathtub";

		case VT_OVEN:
			return "oven";

		case VT_DISHWASHER:
			return "dishwasher";

		case VT_TABLE:
			return "table";

		case VT_SOFA:
			return "sofa";

		case VT_CHAIR:
			return "chair";

		case VT_FIREPLACE:
			return "fireplace";

		case VT_TELEVISION:
			return "television";

		case VT_STAIRS:
			return "stairs";

		case VT_END:
			ocean_assert(false && "Must not be used");
			break;
	}

	ocean_assert(false && "Invalid type!");
	return "UNKNOWN";
}

SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VolumetricType SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::translateVolumetricType(const std::string& volumetricType)
{
	static const VolumetricTypeMap volumetricTypeMap =
	{
		{"UNKNOWN", VT_UNKNOWN},
		{"storage", VT_STORAGE},
		{"refrigerator", VT_REFRIGERATOR},
		{"stove", VT_STOVE},
		{"bed", VT_BED},
		{"sink", VT_SINK},
		{"washer_dryer", VT_WASHER_DRYER},
		{"toilet", VT_TOILET},
		{"bathtub", VT_BATHTUB},
		{"oven", VT_OVEN},
		{"dishwasher", VT_DISHWASHER},
		{"table", VT_TABLE},
		{"sofa", VT_SOFA},
		{"chair", VT_CHAIR},
		{"fireplace", VT_FIREPLACE},
		{"television", VT_TELEVISION},
		{"stairs", VT_STAIRS}
	};

	const VolumetricTypeMap::const_iterator i = volumetricTypeMap.find(volumetricType);

	if (i != volumetricTypeMap.cend())
	{
		return i->second;
	}

	ocean_assert(false && "Invalid type!");
	return VT_UNKNOWN;
}

SceneTracker6DOF::SceneTracker6DOFSample::SceneTracker6DOFSample(const Timestamp& timestamp, const ReferenceSystem referenceSystem, const ObjectIds& objectIds, const Orientations& orientations, const Positions& positions, const SharedSceneElements& sceneElements, const Metadata& metadata) :
	Sample(timestamp, objectIds, metadata),
	TrackerSample(timestamp, referenceSystem, objectIds, metadata),
	OrientationTracker3DOFSample(timestamp, referenceSystem, objectIds, orientations, metadata),
	PositionTracker3DOFSample(timestamp, referenceSystem, objectIds, positions, metadata),
	Tracker6DOFSample(timestamp, referenceSystem, objectIds, orientations, positions, metadata),
	sceneElements_(sceneElements)
{
	// nothing to do here
}

SceneTracker6DOF::SceneTracker6DOFSample::SceneTracker6DOFSample(const Timestamp& timestamp, const ReferenceSystem referenceSystem, ObjectIds&& objectIds, Orientations&& orientations, Positions&& positions, SharedSceneElements&& sceneElements, Metadata&& metadata) :
	Sample(timestamp, std::move(objectIds), std::move(metadata)),
	TrackerSample(timestamp, referenceSystem, std::move(objectIds), std::move(metadata)),
	OrientationTracker3DOFSample(timestamp, referenceSystem, std::move(objectIds), std::move(orientations), std::move(metadata)),
	PositionTracker3DOFSample(timestamp, referenceSystem, std::move(objectIds), std::move(positions), std::move(metadata)),
	Tracker6DOFSample(timestamp, referenceSystem, std::move(objectIds), std::move(orientations), std::move(positions), std::move(metadata)),
	sceneElements_(std::move(sceneElements))
{
	// nothing to do here
}

SceneTracker6DOF::SceneTracker6DOF(const std::string& name) :
	Device(name, deviceTypeTracker6DOF()),
	Measurement(name, deviceTypeTracker6DOF()),
	Tracker(name, deviceTypeTracker6DOF()),
	OrientationTracker3DOF(name),
	PositionTracker3DOF(name),
	Tracker6DOF(name)
{
	// nothing to do here
}

SceneTracker6DOF::~SceneTracker6DOF()
{
	// nothing to do here
}

bool SceneTracker6DOF::exportSceneElements(const std::string& /*format*/, std::ostream& /*outputStream*/, const std::string& /*options*/) const
{
	return false;
}

}

}
