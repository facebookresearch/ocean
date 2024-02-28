// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/mrroomplan/MRRoomPlan.h"

namespace Ocean
{

using namespace Devices;

namespace XRPlayground
{

bool MRRoomPlan::writeObjectPointsToStream(const Vectors3& objectPoints, const Indices32& objectPointIds, IO::OutputBitstream& bitstream)
{
	ocean_assert(objectPoints.size() == objectPointIds.size());

	if (objectPoints.empty() || objectPoints.size() != objectPointIds.size())
	{
		return false;
	}

	if (!bitstream.write<unsigned long long>(objectPointsTag_))
	{
		return false;
	}

	constexpr unsigned long long version = 1ull;

	if (!bitstream.write<unsigned long long>(version))
	{
		return false;
	}

	if (!bitstream.write<unsigned int>((unsigned int)(objectPoints.size())))
	{
		return false;
	}

	if (std::is_same<Scalar, float>::value)
	{
		if (!bitstream.write(objectPoints.data(), objectPoints.size() * sizeof(VectorF3)))
		{
			return false;
		}
	}
	else
	{
		ocean_assert((std::is_same<Scalar, double>::value));

		VectorsF3 objectPointsF;
		objectPointsF.reserve(objectPoints.size());
		for (const Vector3& objectPoint : objectPoints)
		{
			objectPointsF.emplace_back(float(objectPoint.x()), float(objectPoint.y()), float(objectPoint.z()));
		}

		if (!bitstream.write(objectPointsF.data(), objectPointsF.size() * sizeof(VectorF3)))
		{
			return  false;
		}
	}

	if (!bitstream.write<unsigned int>((unsigned int)(objectPointIds.size())))
	{
		return false;
	}

	if (!bitstream.write(objectPointIds.data(), objectPointIds.size() * sizeof(Index32)))
	{
		return false;
	}

	return true;
}

bool MRRoomPlan::writeRoomObjectsToStream(const SceneTracker6DOF::SceneElementRoom::RoomObjectMap& roomObjectMap, IO::OutputBitstream& bitstream)
{
	if (!bitstream.write<unsigned long long>(roomObjectsTag_))
	{
		return false;
	}

	constexpr unsigned long long version = 1ull;
	if (!bitstream.write<unsigned long long>(version))
	{
		return false;
	}

	const unsigned int numberRoomObjects = (unsigned int)(roomObjectMap.size());

	if (!bitstream.write<unsigned int>(numberRoomObjects))
	{
		return false;
	}

	for (SceneTracker6DOF::SceneElementRoom::RoomObjectMap::const_iterator iObject = roomObjectMap.cbegin(); iObject != roomObjectMap.cend(); ++iObject)
	{
		ocean_assert(iObject->second);
		const SceneTracker6DOF::SceneElementRoom::RoomObject& roomObject = *iObject->second;

		if (!bitstream.write<std::string>(roomObject.identifier()))
		{
			return false;
		}

		if (!bitstream.write<float>(roomObject.confidence()))
		{
			return false;
		}

		const HomogenousMatrixD4 world_T_object(roomObject.world_T_object());
		if (!bitstream.write(world_T_object.data(), sizeof(HomogenousMatrixD4)))
		{
			return false;
		}

		const VectorD3 dimension(roomObject.dimension());
		if (!bitstream.write(dimension.data(), sizeof(VectorD3)))
		{
			return false;
		}

		const std::string objectType = SceneTracker6DOF::SceneElementRoom::RoomObject::translateObjectType(roomObject.objectType());
		if (!bitstream.write<std::string>(objectType))
		{
			return false;
		}

		if (iObject->second->objectType() == SceneTracker6DOF::SceneElementRoom::RoomObject::ObjectType::OT_PLANAR)
		{
			const SceneTracker6DOF::SceneElementRoom::PlanarRoomObject& planarRoomObject = (const SceneTracker6DOF::SceneElementRoom::PlanarRoomObject&)(*iObject->second);

			const std::string planarType = SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::translatePlanarType(planarRoomObject.planarType());

			if (!bitstream.write<std::string>(planarType))
			{
				return false;
			}
		}
		else
		{
			ocean_assert(iObject->second->objectType() == SceneTracker6DOF::SceneElementRoom::RoomObject::ObjectType::OT_VOLUMETRIC);

			const SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject& volumetricRoomObject = (const SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject&)(*iObject->second);

			const std::string volumetricType = SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::translateVolumetricType(volumetricRoomObject.volumetricType());

			if (!bitstream.write<std::string>(volumetricType))
			{
				return false;
			}
		}
	}

	return true;
}

bool MRRoomPlan::readObjectPointsFromStream(IO::InputBitstream& bitstream, Vectors3& objectPoints, Indices32& objectPointIds)
{
	constexpr unsigned int maximalObjectPoints = 100u * 1000u;

	unsigned long long tag = 0ull;
	if (!bitstream.read<unsigned long long>(tag) || tag != objectPointsTag_)
	{
		return false;
	}

	unsigned long long version = 0ull;
	if (!bitstream.read<unsigned long long>(version) || version != 1ull)
	{
		return false;
	}

	unsigned int numberObjectPoints = 0u;
	if (!bitstream.read<unsigned int>(numberObjectPoints))
	{
		return false;
	}

	if (numberObjectPoints > maximalObjectPoints)
	{
		return false;
	}

	if (std::is_same<Scalar, float>::value)
	{
		objectPoints.resize(numberObjectPoints);

		if (!bitstream.read(objectPoints.data(), objectPoints.size() * sizeof(VectorF3)))
		{
			return false;
		}
	}
	else
	{
		ocean_assert((std::is_same<Scalar, double>::value));

		VectorsF3 objectPointsF(numberObjectPoints);
		if (!bitstream.read(objectPointsF.data(), objectPointsF.size() * sizeof(VectorF3)))
		{
			return false;
		}

		objectPoints.clear();
		objectPoints.reserve(objectPointsF.size());

		for (VectorF3& objectPointF : objectPointsF)
		{
			objectPoints.emplace_back(Scalar(objectPointF.x()), Scalar(objectPointF.y()), Scalar(objectPointF.z()));
		}
	}

	unsigned int numberObjectPointIds = 0u;
	if (!bitstream.read<unsigned int>(numberObjectPointIds))
	{
		return false;
	}

	if (numberObjectPoints != numberObjectPointIds)
	{
		return false;
	}

	objectPointIds.resize(numberObjectPointIds);

	if (!bitstream.read(objectPointIds.data(), objectPointIds.size() * sizeof(Index32)))
	{
		return false;
	}

	return true;
}

bool MRRoomPlan::readRoomObjectsToStream(IO::InputBitstream& bitstream, SceneTracker6DOF::SceneElementRoom::RoomObjectMap& roomObjectMap)
{
	roomObjectMap.clear();

	unsigned long long tag = 0ull;
	if (!bitstream.read<unsigned long long>(tag) || tag != roomObjectsTag_)
	{
		return false;
	}

	unsigned long long version = 0ull;
	if (!bitstream.read<unsigned long long>(version) || version != 1ull)
	{
		return false;
	}

	unsigned int numberRoomObjects = 0u;
	if (!bitstream.read<unsigned int>(numberRoomObjects) || numberRoomObjects >= 1024u * 1024u)
	{
		return false;
	}

	roomObjectMap.reserve(numberRoomObjects);

	for (unsigned int n = 0u; n < numberRoomObjects; ++n)
	{
		std::string identifier;
		if (!bitstream.read<std::string>(identifier))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		if (roomObjectMap.find(identifier) != roomObjectMap.cend())
		{
			return false;
		}

		float confidence = -1.0f;
		if (!bitstream.read<float>(confidence) || confidence < 0.0f || confidence > 1.0f)
		{
			return false;
		}

		HomogenousMatrixD4 world_T_object;
		if (!bitstream.read(world_T_object.data(), sizeof(HomogenousMatrixD4)) || !world_T_object.isValid())
		{
			return false;
		}

		VectorD3 dimension;
		if (!bitstream.read(dimension.data(), sizeof(VectorD3)))
		{
			return false;
		}

		std::string objectTypeString;
		if (!bitstream.read<std::string>(objectTypeString))
		{
			return false;
		}

		const SceneTracker6DOF::SceneElementRoom::RoomObject::ObjectType objectType = SceneTracker6DOF::SceneElementRoom::RoomObject::translateObjectType(objectTypeString);

		if (objectType == SceneTracker6DOF::SceneElementRoom::RoomObject::OT_PLANAR)
		{
			std::string planarTypeString;
			if (!bitstream.read<std::string>(planarTypeString))
			{
				return false;
			}

			const SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::PlanarType planarType = SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::translatePlanarType(planarTypeString);

			if (planarType == SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::PT_UNKNOWN)
			{
				ocean_assert(false && "This should never happen!");
				return false;
			}

			SceneTracker6DOF::SceneElementRoom::SharedRoomObject planarObject = std::make_shared<SceneTracker6DOF::SceneElementRoom::PlanarRoomObject>(std::string(identifier), planarType, confidence, HomogenousMatrix4(world_T_object), Vector3(dimension));

			roomObjectMap.emplace(std::move(identifier), std::move(planarObject));
		}
		else if (objectType == SceneTracker6DOF::SceneElementRoom::RoomObject::OT_VOLUMETRIC)
		{
			std::string volumetricTypeString;
			if (!bitstream.read<std::string>(volumetricTypeString))
			{
				return false;
			}

			const SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VolumetricType volumetricType = SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::translateVolumetricType(volumetricTypeString);

			if (volumetricType == SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_UNKNOWN)
			{
				ocean_assert(false && "This should never happen!");
				return false;
			}

			SceneTracker6DOF::SceneElementRoom::SharedRoomObject volumetricObject = std::make_shared<SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject>(std::string(identifier), volumetricType, confidence, HomogenousMatrix4(world_T_object), Vector3(dimension));

			roomObjectMap.emplace(std::move(identifier), std::move(volumetricObject));
		}
		else
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}
	}

	return true;
}

RGBAColor MRRoomPlan::planarColor(const SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::PlanarType planarType, const float alpha)
{
	switch (planarType)
	{
		case SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::PT_UNKNOWN:
			break;

		case SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::PT_WALL:
			return RGBAColor(0.7f, 0.7f, 0.7f, alpha);

		case SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::PT_DOOR:
			return RGBAColor(0.9f, 0.25f, 0.25f, alpha);

		case SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::PT_WINDOW:
			return RGBAColor(0.25f, 0.25f, 0.9f, alpha);

		case SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::PT_OPENING:
			return RGBAColor(0.25f, 0.9f, 0.25f, alpha);

		case SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::PT_FLOOR:
			return RGBAColor(0.25f, 0.25f, 0.25f, alpha);

		case SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::PT_END:
			ocean_assert(false && "Must not be used!");
			break;
	}

	ocean_assert(false && "Invalid type!");
	return RGBAColor(0.7f, 0.7f, 0.7f, alpha);
}

Scalar MRRoomPlan::planarThickness(const Devices::SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::PlanarType planarType)
{
	switch (planarType)
	{
		case Devices::SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::PT_UNKNOWN:
			break;

		case Devices::SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::PT_WALL:
		case Devices::SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::PT_FLOOR:
			return Scalar(0);

		case Devices::SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::PT_DOOR:
			return Scalar(0.1);

		case Devices::SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::PT_WINDOW:
			return Scalar(0.05);

		case Devices::SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::PT_OPENING:
			return Scalar(0.025);

		case Devices::SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::PT_END:
			ocean_assert(false && "Must not be used!");
			break;
	}

	ocean_assert(false && "Invalid type!");
	return Scalar(0);
}

RGBAColor MRRoomPlan::volumetricColor(const SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VolumetricType volumetricType, const float alpha)
{
	switch (volumetricType)
	{
		case SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_UNKNOWN:
			break;

		case SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_STORAGE:
			return RGBAColor(0.7f, 0.25f, 0.25f, alpha);

		case SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_REFRIGERATOR:
			return RGBAColor(0.7f, 0.7f, 0.7f, alpha);

		case SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_STOVE:
			return RGBAColor(0.25f, 0.7f, 0.25f, alpha);

		case SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_BED:
			return RGBAColor(0.25f, 0.25f, 0.7f, alpha);

		case SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_SINK:
			return RGBAColor(0.9f, 0.9f, 0.9f, alpha);

		case SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_WASHER_DRYER:
			return RGBAColor(0.0f, 0.0f, 1.0f, alpha);

		case SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_TOILET:
			return RGBAColor(1.0f, 1.0f, 1.0f, alpha);

		case SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_BATHTUB:
			return RGBAColor(1.0f, 1.0f, 1.0f, alpha);

		case SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_OVEN:
			return RGBAColor(0.25f, 0.7f, 0.25f, alpha);

		case SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_DISHWASHER:
			return RGBAColor(0.0f, 0.0f, 1.0f, alpha);

		case SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_TABLE:
			return RGBAColor(0.5f, 0.5f, 0.5f, alpha);

		case SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_SOFA:
			return RGBAColor(0.7f, 0.7f, 0.25f, alpha);

		case SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_CHAIR:
			return RGBAColor(0.25f, 0.7f, 0.7f, alpha);

		case SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_FIREPLACE:
			return RGBAColor(0.7f, 0.25f, 0.7f, alpha);

		case SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_TELEVISION:
			return RGBAColor(0.2f, 0.2f, 0.2f, alpha);

		case SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_STAIRS:
			return RGBAColor(0.6f, 0.6f, 0.6f, alpha);

		case SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_END:
			ocean_assert(false && "Must not be used!");
			break;
	}

	ocean_assert(false && "Invalid type!");
	return RGBAColor(0.7f, 0.7f, 0.7f, alpha);
}

Vector3 MRRoomPlan::adjustedVolumetricDimension(const Devices::SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VolumetricType volumetricType, const Vector3& dimension)
{
	switch (volumetricType)
	{
		case SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_UNKNOWN:
			ocean_assert(false && "Invalid type!");
			break;

		case SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_SINK:
		case SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_STOVE:
			return Vector3(dimension.x(), dimension.y() + Scalar(0.01), dimension.z());

		case SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_REFRIGERATOR:
		case SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_OVEN:
		case SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_DISHWASHER:
			return Vector3(dimension.x(), dimension.y(), dimension.z() + Scalar(0.01));

		default:
			break;
	}

	return dimension;
}

}

}
