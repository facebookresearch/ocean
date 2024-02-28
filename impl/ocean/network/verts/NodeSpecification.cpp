// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/network/verts/NodeSpecification.h"

#include <verts/client/capi.h>

#include "verts/shared/ctypes.h"

namespace Ocean
{

namespace Network
{

namespace Verts
{

const NodeSpecification* NodeSpecification::Manager::nodeSpecification(const std::string& name) const
{
	const ScopedLock scopedLock(lock_);

	NodeSpecificationMap::const_iterator i = nodeSpecificationMap_.find(name);

	if (i == nodeSpecificationMap_.cend())
	{
		return nullptr;
	}

	return i->second.get();
}

NodeSpecification& NodeSpecification::Manager::newNodeSpecification(const std::string& name)
{
	const ScopedLock scopedLock(lock_);

	NodeSpecificationMap::const_iterator i = nodeSpecificationMap_.find(name);

	if (i == nodeSpecificationMap_.cend())
	{
		// in case the specification does not exist we return a new instance, otherwise we return the existing instance

		i = nodeSpecificationMap_.emplace(name, new NodeSpecification(name)).first;
	}

	return *i->second.get();
}

const NodeSpecification* NodeSpecification::Manager::newNodeSpecification(verts_driver* vertsDriver, verts_component* vertsComponent, verts_field_set* vertsFieldSet)
{
	ocean_assert(vertsDriver != nullptr);
	ocean_assert(vertsComponent != nullptr);
	ocean_assert(vertsFieldSet != nullptr);

	ocean_assert(!verts_component_is_locally_owned(vertsComponent));

	const verts_string typeName = verts_component_get_type_name(vertsComponent);
	ocean_assert(typeName.length != 0);

	std::string name(typeName.content);

	const ScopedLock scpedLock(lock_);

	NodeSpecificationMap::const_iterator i = nodeSpecificationMap_.find(name);

	if (i == nodeSpecificationMap_.cend())
	{
		// in case the specification does not exist we return a new instance

		i = nodeSpecificationMap_.emplace(name, new NodeSpecification(vertsDriver, name, vertsComponent, vertsFieldSet)).first;
	}
	else
	{
		// the specification exists already, so let's check that the existing specification and the new specification match with each other

		NodeSpecification& nodeSpecification = *i->second.get();

		bool nodeSpecificationIsDifferent = false;
		size_t numberFields = 0;

		for (uint16_t vertsFieldIndex = 0u; !nodeSpecificationIsDifferent && vertsFieldIndex < 256u; ++vertsFieldIndex)
		{
			const verts_string vertsFieldName = verts_component_get_field_name(vertsComponent, vertsFieldIndex);

			if (vertsFieldName.length == 0)
			{
				break;
			}

			ocean_assert(vertsFieldName.content != nullptr);
			const std::string fieldName(vertsFieldName.content);

			const VertsFieldType vertsFieldType = verts_field_set_get_field_type_by_index(vertsFieldSet, vertsFieldIndex);
			const FieldType fieldType = NodeSpecification::translateFieldType(vertsFieldType);

			if (fieldType == FT_UNKNOWN || !nodeSpecification.hasField(fieldName, fieldType))
			{
				nodeSpecificationIsDifferent = true;
			}
			else
			{
				const Index32 fieldIndex = nodeSpecification.fieldIndex(fieldName);
				ocean_assert(fieldIndex != invalidFieldIndex());

				ocean_assert(fieldIndex < nodeSpecification.fieldSpecifications_.size());
				if (fieldIndex < nodeSpecification.fieldSpecifications_.size())
				{
					FieldSpecification& fieldSpecification = nodeSpecification.fieldSpecifications_[fieldIndex];

					ocean_assert(fieldSpecification.name_ == fieldName);
					ocean_assert(fieldSpecification.fieldType_ = fieldType);

					if (fieldSpecification.vertsFieldIndex_ == uint16_t(-1))
					{
						// although the specification already exists, it has not yet been registered with a driver
						// therefore, the VERTS field index is not yet assigned - we do that now

						fieldSpecification.vertsFieldIndex_ = vertsFieldIndex;
					}
					else
					{
						ocean_assert(fieldSpecification.vertsFieldIndex_ == vertsFieldIndex);
					}
				}

			}

			++numberFields;
		}

		if (nodeSpecificationIsDifferent || numberFields != nodeSpecification.fields())
		{
			OCEAN_APPLY_IF_DEBUG(Log::error() << "VERTS: The node specification for '" << name << "' is different from the existing node specification");
			ocean_assert(false && "The node specification is different from the existing node specification");

			return nullptr;
		}
	}

	return i->second.get();
}

void NodeSpecification::Manager::registerAllNodeSpecificationsWithDriver(verts_driver* vertsDriver)
{
	ocean_assert(vertsDriver != nullptr);

	for (NodeSpecificationMap::const_iterator iSpecification = nodeSpecificationMap_.cbegin(); iSpecification != nodeSpecificationMap_.cend(); ++iSpecification)
	{
		iSpecification->second->registerWithDriver(vertsDriver);
	}
}

void NodeSpecification::Manager::release()
{
	const ScopedLock scopedLock(lock_);

	nodeSpecificationMap_.clear();
}

NodeSpecification::NodeSpecification(std::string name) :
	name_(std::move(name))
{
	vertsFieldSet_ = verts_create_field_set(verts_make_string(name_.c_str()), VertsAuthorityModel::VertsAuthorityModel_Anyone, VertsCleanupPolicy::VertsCleanupPolicy_WithCreator);

	id_ = verts_field_set_get_id(vertsFieldSet_);
}

NodeSpecification::NodeSpecification(verts_driver* vertsDriver, std::string name, verts_component* vertsComponent, verts_field_set* vertsFieldSet) :
	name_(std::move(name)),
	vertsFieldSet_(vertsFieldSet)
{
	ocean_assert(vertsDriver != nullptr);
	ocean_assert(!name_.empty());
	ocean_assert(vertsFieldSet_ != nullptr);
	ocean_assert(vertsComponent != nullptr);

	for (uint16_t vertsFieldIndex = 0u; vertsFieldIndex < 256u; ++vertsFieldIndex)
	{
		const verts_string vertsFieldName = verts_component_get_field_name(vertsComponent, vertsFieldIndex);

		if (vertsFieldName.length == 0)
		{
			break;
		}

		ocean_assert(vertsFieldName.content != nullptr);
		std::string fieldName(vertsFieldName.content);

		const VertsFieldType vertsFieldType = verts_field_set_get_field_type_by_index(vertsFieldSet, vertsFieldIndex);
		const FieldType fieldType = translateFieldType(vertsFieldType);

		ocean_assert(fieldType != FT_UNKNOWN);

		registerField(std::move(fieldName), fieldType);
	}

	// now, as the node is finalized, we can determine verts' field indices

	for (FieldSpecification& fieldSpecification : fieldSpecifications_)
	{
		const int vertsFieldIndex = verts_field_set_get_field_index(vertsFieldSet_, verts_make_string(fieldSpecification.name_.c_str()));

		fieldSpecification.vertsFieldIndex_ = uint16_t(vertsFieldIndex);
	}

	id_ = verts_field_set_get_id(vertsFieldSet_);

	isFinalized_ = true;

	registeredVertsDrivers_.emplace(vertsDriver);
}

NodeSpecification::~NodeSpecification()
{
	ocean_assert(vertsFieldSet_ != nullptr);
	verts_delete_field_set(vertsFieldSet_);
}

Index32 NodeSpecification::registerField(std::string fieldName, const FieldType fieldType)
{
	ocean_assert(vertsFieldSet_ != nullptr);
	ocean_assert(!fieldName.empty());

	if (fieldName.empty())
	{
		return invalidFieldIndex();
	}

	ocean_assert(fieldName[0] != '_'); // internal fields start with '_'

	const FieldMap::const_iterator iField = fieldMap_.find(fieldName);

	if (isFinalized_)
	{
		// the node specification is already finalized (e.g., already used or created via a remote host)
		// therefore, we just ensure that the field exists and has the expected field type

		if (iField != fieldMap_.cend())
		{
			if (iField->second.second == fieldType)
			{
				return iField->second.first;
			}
			else
			{
				OCEAN_APPLY_IF_DEBUG(Log::error() << "<debug> VERTS: The field '" << fieldName << "' exist already and has a different type");
				ocean_assert(false && "The field exist already and has a different type");
			}
		}
		else
		{
			OCEAN_APPLY_IF_DEBUG(Log::error() << "<debug> VERTS: The field '" << fieldName << "' does not exist although the node is already finalized");
			ocean_assert(false && "The field does not exist although the node is already finalized");
		}

		return invalidFieldIndex();
	}

	if (iField != fieldMap_.cend())
	{
		OCEAN_APPLY_IF_DEBUG(Log::error() << "<debug> VERTS: The field '" << fieldName << "' exist already");
		ocean_assert(false && "The field exist already");

		return invalidFieldIndex();
	}

	const VertsFieldType vertsFieldType = VertsFieldType(translateFieldType(fieldType));
	ocean_assert(vertsFieldType != VertsFieldType(-1));

	verts_add_field_set_field_def(vertsFieldSet_, verts_make_string(fieldName.c_str()), vertsFieldType);

	const Index32 fieldIndex = Index32(fieldSpecifications_.size());

	fieldSpecifications_.emplace_back(fieldName, fieldType);
	fieldMap_.emplace(std::move(fieldName), FieldPair(fieldIndex, fieldType));

	return fieldIndex;
}

bool NodeSpecification::fieldHasIndex(const std::string& name, const Index32 fieldIndex, const FieldType fieldType) const
{
	const FieldMap::const_iterator iField = fieldMap_.find(name);

	if (iField == fieldMap_.cend())
	{
		ocean_assert(false && "Unknown field");
		return FT_UNKNOWN;
	}

	const FieldPair& fieldPair = iField->second;

	if (fieldIndex != fieldPair.first)
	{
		return false;
	}

	return fieldType == FT_UNKNOWN || fieldType == fieldPair.second;
}

VertsFieldType NodeSpecification::translateFieldType(const FieldType fieldType)
{
	static_assert(std::is_same<int, std::underlying_type<VertsFieldType>::type>::value, "Invalid data type!");

	switch (fieldType)
	{
		case FT_UNKNOWN:
			ocean_assert(false && "Invalid field type!");
			return VertsFieldType(-1);

		case FT_BOOL:
			return VertsFieldType::VertsFieldType_Bool;

		case FT_INT_32:
			return VertsFieldType::VertsFieldType_Int;

		case FT_UINT_64:
			return VertsFieldType::VertsFieldType_UInt64;

		case FT_FLOAT_32:
			return VertsFieldType::VertsFieldType_Float;

		case FT_VECTOR_32:
			return VertsFieldType::VertsFieldType_Vec3;

		case FT_QUATERNION_32:
			return VertsFieldType::VertsFieldType_Quat;

		case FT_STRING:
			return VertsFieldType::VertsFieldType_String;

		case FT_RPC_BROADCAST:
			return VertsFieldType::VertsFieldType_RpcBroadcast;

		case FT_RPC_OWNER:
			return VertsFieldType::VertsFieldType_RpcOwner;

		case FT_ENTITY_REFERENCE:
			return VertsFieldType::VertsFieldType_EntityRef;
	}

	ocean_assert(false && "Invalid field type!");
	return VertsFieldType(-1);
}

NodeSpecification::FieldType NodeSpecification::translateFieldType(const VertsFieldType vertsFieldType)
{
	static_assert(std::is_same<int, std::underlying_type<VertsFieldType>::type>::value, "Invalid data type!");

	switch (vertsFieldType)
	{
		case VertsFieldType::VertsFieldType_Bool:
			return FT_BOOL;

		case VertsFieldType::VertsFieldType_Int:
			return FT_INT_32;

		case VertsFieldType::VertsFieldType_UInt64:
			return FT_UINT_64;

		case VertsFieldType::VertsFieldType_Float:
			return FT_FLOAT_32;

		case VertsFieldType::VertsFieldType_Vec3:
			return FT_VECTOR_32;

		case VertsFieldType::VertsFieldType_Quat:
			return FT_QUATERNION_32;

		case VertsFieldType::VertsFieldType_String:
			return FT_STRING;

		case VertsFieldType::VertsFieldType_RpcBroadcast:
			return FT_RPC_BROADCAST;

		case VertsFieldType::VertsFieldType_RpcOwner:
			return FT_RPC_OWNER;

		case VertsFieldType::VertsFieldType_EntityRef:
			return FT_ENTITY_REFERENCE;
	}

	ocean_assert(false && "Invalid field type!");
	return FT_UNKNOWN;
}

void NodeSpecification::registerWithDriver(verts_driver* vertsDriver) const
{
	ocean_assert(vertsDriver != nullptr);
	ocean_assert(vertsFieldSet_ != nullptr);

	isFinalized_ = true;

	if (registeredVertsDrivers_.emplace(vertsDriver).second)
	{
		// verts changes the field's indices whenever a new field is added (fields are sorted)
		// now, as the node is finalized, we can determine verts' field indices

		for (FieldSpecification& fieldSpecification : fieldSpecifications_)
		{
			const int vertsFieldIndex = verts_field_set_get_field_index(vertsFieldSet_, verts_make_string(fieldSpecification.name_.c_str()));

			fieldSpecification.vertsFieldIndex_ = uint16_t(vertsFieldIndex);
		}

		verts_driver_register_field_set_cstruct(vertsDriver, vertsFieldSet_);
	}
}

void NodeSpecification::registerAllNodeSpecificationsWithDriver(verts_driver* vertsDriver)
{
	Manager::get().registerAllNodeSpecificationsWithDriver(vertsDriver);
}

void NodeSpecification::registerInternalNodeSpecifications()
{
	{
		ocean_assert(isInternalNodeType(internalNodeTypeSession_));

		NodeSpecification& internalUserNode = NodeSpecification::newNodeSpecification(internalNodeTypeSession_);
		internalUserNode.registerField<uint64_t>("sessionId");
		internalUserNode.registerField<uint64_t>("userId");
		internalUserNode.registerField<uint64_t>("appId");
	}

	{
		ocean_assert(isInternalNodeType(internalNodeTypeContainer_));

		NodeSpecification& internalUserNode = NodeSpecification::newNodeSpecification(internalNodeTypeContainer_);
		internalUserNode.registerField<uint64_t>("uniqueId");
		internalUserNode.registerField<std::string>("identifier");
		internalUserNode.registerField<uint64_t>("version");
		internalUserNode.registerField<uint64_t>("numberSequences");
	}
}

bool NodeSpecification::isInternalNodeType(const std::string& nodeType)
{
	ocean_assert(!nodeType.empty());

	// internal nodes have the prefix "_OCEAN_"

	return nodeType.size() >= 8 && nodeType[0] == '_' && nodeType[1] == 'O' && nodeType[2] == 'C' && nodeType[3] == 'E' && nodeType[4] == 'A' && nodeType[5] == 'N' && nodeType[6] == '_';
}

}

}

}
