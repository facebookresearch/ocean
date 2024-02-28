// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/network/verts/Node.h"

#include <verts/client/capi.h>

#include "verts/shared/ctypes.h"

namespace Ocean
{

namespace Network
{

namespace Verts
{

Node::Node(verts_driver* vertsDriver, const NodeSpecification& nodeSpecification, verts_entity* vertsEntity) :
	nodeSpecification_(nodeSpecification)
{
	ocean_assert(vertsEntity != nullptr);

	nodeSpecification.registerWithDriver(vertsDriver);

	vertsComponent_ = verts_entity_add_component(vertsEntity, verts_make_string(nodeSpecification.name().c_str()), VERTS_COMPONENT_ID_INVALID);
	ocean_assert(vertsComponent_ != nullptr);

	if (vertsComponent_ != nullptr)
	{
		nodeId_ = verts_component_get_id(vertsComponent_);
	}
}

Node::Node(verts_component* vertsComponent, const NodeSpecification& nodeSpecification) :
	nodeSpecification_(nodeSpecification),
	vertsComponent_(vertsComponent)
{
	ocean_assert(vertsComponent_ != nullptr);

#ifdef OCEAN_DEBUG
	const verts_string typeName = verts_component_get_type_name(vertsComponent);
	ocean_assert(typeName.length != 0 && typeName.content != nullptr);

	if (nodeSpecification_.name() != typeName.content)
	{
		Log::error() << "<debug> VERTS: Must never happen: Invalid node component " << nodeSpecification_.name() << " vs. " << typeName.content;
		ocean_assert(false && "Invalid node component");
	}
#endif

	nodeId_ = verts_component_get_id(vertsComponent_);
}

template <>
bool Node::field(const Index32 fieldIndex, const uint64_t timestamp) const
{
	ocean_assert(vertsComponent_ != nullptr);

	if (nodeSpecification_.fieldType(fieldIndex) != NodeSpecification::FT_BOOL)
	{
		ocean_assert(false && "The field has a different type");
		return false;
	}

	uint16_t vertsFieldIndex;
	if (!nodeSpecification_.vertsFieldIndex(fieldIndex, vertsFieldIndex))
	{
		ocean_assert(false && "Invalid field index");
		return false;
	}

	if (timestamp == uint64_t(-1))
	{
		return verts_component_getlatestbool_by_index(vertsComponent_, uint32_t(vertsFieldIndex));
	}
	else
	{
		return verts_component_getbool_by_index(vertsComponent_, uint32_t(vertsFieldIndex), timestamp);
	}
}

template <>
int32_t Node::field(const Index32 fieldIndex, const uint64_t timestamp) const
{
	ocean_assert(vertsComponent_ != nullptr);

	if (nodeSpecification_.fieldType(fieldIndex) != NodeSpecification::FT_INT_32)
	{
		ocean_assert(false && "The field has a different type");
		return 0;
	}

	uint16_t vertsFieldIndex;
	if (!nodeSpecification_.vertsFieldIndex(fieldIndex, vertsFieldIndex))
	{
		ocean_assert(false && "Invalid field index");
		return 0;
	}

	if (timestamp == uint64_t(-1))
	{
		return verts_component_getlatestint_by_index(vertsComponent_, uint32_t(vertsFieldIndex));
	}
	else
	{
		return verts_component_getint_by_index(vertsComponent_, uint32_t(vertsFieldIndex), timestamp);
	}
}

template <>
uint64_t Node::field(const Index32 fieldIndex, const uint64_t timestamp) const
{
	ocean_assert(vertsComponent_ != nullptr);

	if (nodeSpecification_.fieldType(fieldIndex) != NodeSpecification::FT_UINT_64)
	{
		ocean_assert(false && "The field has a different type");
		return 0ull;
	}

	uint16_t vertsFieldIndex;
	if (!nodeSpecification_.vertsFieldIndex(fieldIndex, vertsFieldIndex))
	{
		ocean_assert(false && "Invalid field index");
		return 0ull;
	}

	if (timestamp == uint64_t(-1))
	{
		return verts_component_getlatestuint64_by_index(vertsComponent_, uint32_t(vertsFieldIndex));
	}
	else
	{
		return verts_component_getuint64_by_index(vertsComponent_, uint32_t(vertsFieldIndex), timestamp);
	}
}

template <>
float Node::field(const Index32 fieldIndex, const uint64_t timestamp) const
{
	ocean_assert(vertsComponent_ != nullptr);

	if (nodeSpecification_.fieldType(fieldIndex) != NodeSpecification::FT_FLOAT_32)
	{
		ocean_assert(false && "The field has a different type");
		return false;
	}

	uint16_t vertsFieldIndex;
	if (!nodeSpecification_.vertsFieldIndex(fieldIndex, vertsFieldIndex))
	{
		ocean_assert(false && "Invalid field index");
		return 0.0f;
	}

	if (timestamp == uint64_t(-1))
	{
		return verts_component_getlatestfloat_by_index(vertsComponent_, uint32_t(vertsFieldIndex));
	}
	else
	{
		return verts_component_getfloat_by_index(vertsComponent_, uint32_t(vertsFieldIndex), timestamp);
	}
}

template <>
VectorF3 Node::field(const Index32 fieldIndex, const uint64_t timestamp) const
{
	ocean_assert(vertsComponent_ != nullptr);

	if (nodeSpecification_.fieldType(fieldIndex) != NodeSpecification::FT_VECTOR_32)
	{
		ocean_assert(false && "The field has a different type");
		return VectorF3(0.0f, 0.0f, 0.0f);
	}

	uint16_t vertsFieldIndex;
	if (!nodeSpecification_.vertsFieldIndex(fieldIndex, vertsFieldIndex))
	{
		ocean_assert(false && "Invalid field index");
		return VectorF3(0.0f, 0.0f, 0.0f);
	}

	verts_vec3 vertsVector;

	if (timestamp == uint64_t(-1))
	{
		vertsVector = verts_component_getlatestvec3_by_index(vertsComponent_, uint32_t(vertsFieldIndex));
	}
	else
	{
		vertsVector = verts_component_getvec3_by_index(vertsComponent_, uint32_t(vertsFieldIndex), timestamp);
	}

	return VectorF3(vertsVector.x, vertsVector.y, vertsVector.z);
}

template <>
QuaternionF Node::field(const Index32 fieldIndex, const uint64_t timestamp) const
{
	ocean_assert(vertsComponent_ != nullptr);

	if (nodeSpecification_.fieldType(fieldIndex) != NodeSpecification::FT_QUATERNION_32)
	{
		ocean_assert(false && "The field has a different type");
		return QuaternionF(false);
	}

	uint16_t vertsFieldIndex;
	if (!nodeSpecification_.vertsFieldIndex(fieldIndex, vertsFieldIndex))
	{
		ocean_assert(false && "Invalid field index");
		return QuaternionF(false);
	}

	verts_quat vertsQuaternion;

	if (timestamp == uint64_t(-1))
	{
		vertsQuaternion = verts_component_getlatestquat_by_index(vertsComponent_, uint32_t(vertsFieldIndex));
	}
	else
	{
		vertsQuaternion = verts_component_getquat_by_index(vertsComponent_, uint32_t(vertsFieldIndex), timestamp);
	}

	return QuaternionF(vertsQuaternion.r, vertsQuaternion.i, vertsQuaternion.j, vertsQuaternion.k);
}

template <>
Node::StringPointer Node::field(const Index32 fieldIndex, const uint64_t timestamp) const
{
	ocean_assert(vertsComponent_ != nullptr);

	if (nodeSpecification_.fieldType(fieldIndex) != NodeSpecification::FT_STRING)
	{
		ocean_assert(false && "The field has a different type");
		return StringPointer();
	}

	uint16_t vertsFieldIndex;
	if (!nodeSpecification_.vertsFieldIndex(fieldIndex, vertsFieldIndex))
	{
		ocean_assert(false && "Invalid field index");
		return StringPointer();
	}

	verts_string vertsString;

	if (timestamp == uint64_t(-1))
	{
		vertsString = verts_component_getlateststring_by_index(vertsComponent_, uint32_t(vertsFieldIndex));
	}
	else
	{
		vertsString = verts_component_getstring_by_index(vertsComponent_, uint32_t(vertsFieldIndex), timestamp);
	}

	if (vertsString.length == 0)
	{
		return StringPointer();
	}

	return StringPointer(vertsString.content, vertsString.length);
}

template <>
std::string Node::field(const Index32 fieldIndex, const uint64_t timestamp) const
{
	const StringPointer stringPointer = field<StringPointer>(fieldIndex, timestamp);

	if (stringPointer.size() == 0)
	{
		return std::string();
	}

	ocean_assert(stringPointer.data() != nullptr);

	std::string result(stringPointer.size(), '\0');
	memcpy(&result[0], stringPointer.data(), stringPointer.size());

	return result;
}

template <>
Node::StringBuffer Node::field(const Index32 fieldIndex, const uint64_t timestamp) const
{
	const StringPointer stringPointer = field<StringPointer>(fieldIndex, timestamp);

	if (stringPointer.size() == 0)
	{
		return StringBuffer();
	}

	ocean_assert(stringPointer.data() != nullptr);

	StringBuffer result(stringPointer.size());
	memcpy(result.data(), stringPointer.data(), stringPointer.size());

	return result;
}

template <>
bool Node::setField(const Index32 fieldIndex, const bool& value)
{
	ocean_assert(vertsComponent_ != nullptr);

	if (nodeSpecification_.fieldType(fieldIndex) != NodeSpecification::FT_BOOL)
	{
		ocean_assert(false && "The field has a different type");
		return false;
	}

	uint16_t vertsFieldIndex;
	if (!nodeSpecification_.vertsFieldIndex(fieldIndex, vertsFieldIndex))
	{
		ocean_assert(false && "Invalid field index");
		return false;
	}

	return verts_component_setbool_by_index(vertsComponent_, uint32_t(vertsFieldIndex), value);
}

template <>
bool Node::setField(const Index32 fieldIndex, const int32_t& value)
{
	ocean_assert(vertsComponent_ != nullptr);

	if (nodeSpecification_.fieldType(fieldIndex) != NodeSpecification::FT_INT_32)
	{
		ocean_assert(false && "The field has a different type");
		return false;
	}

	uint16_t vertsFieldIndex;
	if (!nodeSpecification_.vertsFieldIndex(fieldIndex, vertsFieldIndex))
	{
		ocean_assert(false && "Invalid field index");
		return false;
	}

	return verts_component_setint_by_index(vertsComponent_, uint32_t(vertsFieldIndex), value);
}

template <>
bool Node::setField(const Index32 fieldIndex, const uint64_t& value)
{
	ocean_assert(vertsComponent_ != nullptr);

	if (nodeSpecification_.fieldType(fieldIndex) != NodeSpecification::FT_UINT_64)
	{
		ocean_assert(false && "The field has a different type");
		return false;
	}

	uint16_t vertsFieldIndex;
	if (!nodeSpecification_.vertsFieldIndex(fieldIndex, vertsFieldIndex))
	{
		ocean_assert(false && "Invalid field index");
		return false;
	}

	return verts_component_setuint64_by_index(vertsComponent_, uint32_t(vertsFieldIndex), value);
}

template <>
bool Node::setField(const Index32 fieldIndex, const float& value)
{
	ocean_assert(vertsComponent_ != nullptr);

	if (nodeSpecification_.fieldType(fieldIndex) != NodeSpecification::FT_FLOAT_32)
	{
		ocean_assert(false && "The field has a different type");
		return false;
	}

	uint16_t vertsFieldIndex;
	if (!nodeSpecification_.vertsFieldIndex(fieldIndex, vertsFieldIndex))
	{
		ocean_assert(false && "Invalid field index");
		return false;
	}

	return verts_component_setfloat_by_index(vertsComponent_, uint32_t(vertsFieldIndex), value);
}

template <>
bool Node::setField(const Index32 fieldIndex, const VectorF3& value)
{
	ocean_assert(vertsComponent_ != nullptr);

	if (nodeSpecification_.fieldType(fieldIndex) != NodeSpecification::FT_VECTOR_32)
	{
		ocean_assert(false && "The field has a different type");
		return false;
	}

	uint16_t vertsFieldIndex;
	if (!nodeSpecification_.vertsFieldIndex(fieldIndex, vertsFieldIndex))
	{
		ocean_assert(false && "Invalid field index");
		return false;
	}

	const verts_vec3 vertsVector{value.x(), value.y(), value.z()};

	return verts_component_setvec3_by_index(vertsComponent_, uint32_t(vertsFieldIndex), vertsVector);
}

template <>
bool Node::setField(const Index32 fieldIndex, const QuaternionF& value)
{
	ocean_assert(vertsComponent_ != nullptr);

	if (nodeSpecification_.fieldType(fieldIndex) != NodeSpecification::FT_QUATERNION_32)
	{
		ocean_assert(false && "The field has a different type");
		return false;
	}

	uint16_t vertsFieldIndex;
	if (!nodeSpecification_.vertsFieldIndex(fieldIndex, vertsFieldIndex))
	{
		ocean_assert(false && "Invalid field index");
		return false;
	}

	const verts_quat vertsQuaternion{value.w(), value.x(), value.y(), value.z()};

	return verts_component_setquat_by_index(vertsComponent_, uint32_t(vertsFieldIndex), vertsQuaternion);
}

template <>
bool Node::setField(const Index32 fieldIndex, const StringPointer& value)
{
	ocean_assert(vertsComponent_ != nullptr);

	if (nodeSpecification_.fieldType(fieldIndex) != NodeSpecification::FT_STRING)
	{
		ocean_assert(false && "The field has a different type");
		return false;
	}

	uint16_t vertsFieldIndex;
	if (!nodeSpecification_.vertsFieldIndex(fieldIndex, vertsFieldIndex))
	{
		ocean_assert(false && "Invalid field index");
		return false;
	}

	verts_string vertsString;
	vertsString.content = (const char*)(value.data());
	vertsString.length = uint32_t(value.size());

	return verts_component_setstring_by_index(vertsComponent_, uint32_t(vertsFieldIndex), vertsString);
}

template <>
bool Node::setField(const Index32 fieldIndex, const std::string& value)
{
	return setField<StringPointer>(fieldIndex, StringPointer(value.c_str(), value.size()));
}

template <>
bool Node::setField(const Index32 fieldIndex, const StringBuffer& value)
{
	return setField<StringPointer>(fieldIndex, StringPointer(value.data(), value.size()));
}

}

}

}
