// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/network/verts/Entity.h"
#include "ocean/network/verts/Driver.h"
#include "ocean/network/verts/NodeSpecification.h"

#include <verts/client/capi.h>

#include "verts/shared/ctypes.h"

namespace Ocean
{

namespace Network
{

namespace Verts
{

Entity::Entity(verts_driver* vertsDriver)
{
	ocean_assert(vertsDriver != nullptr);

	vertsEntity_ = verts_driver_create_entity(vertsDriver, VERTS_ENTITY_ID_INVALID);
	ocean_assert(vertsEntity_ != nullptr);

	entityId_ = verts_entity_get_id(vertsEntity_);
}

Entity::Entity(verts_entity* vertsEntity) :
	vertsEntity_(vertsEntity)
{
	ocean_assert(vertsEntity_ != nullptr);

	entityId_ = verts_entity_get_id(vertsEntity_);
}

Entity::Entity(verts_driver* vertsDriver, const NodeSpecification& nodeSpecification) :
	Entity(vertsDriver)
{
	addNode(vertsDriver, nodeSpecification);
}

Entity::Entity(verts_driver* vertsDriver, const std::vector<std::string>& nodeTypes) :
	Entity(vertsDriver)
{
	ocean_assert(std::unordered_set<std::string>(nodeTypes.cbegin(), nodeTypes.cend()).size() == nodeTypes.size() && "At least one node type appears twice");

	for (const std::string& nodeType : nodeTypes)
	{
		const NodeSpecification* nodeSpecification = NodeSpecification::nodeSpecification(nodeType);

		if (nodeSpecification == nullptr)
		{
			Log::error() << "VERTS: The node type '" << nodeType << "' does not exist";
		}
		else
		{
			addNode(vertsDriver, *nodeSpecification);
		}
	}
}

Entity::Entity(verts_driver* vertsDriver, const std::vector<const NodeSpecification*>& nodeSpecifications)
{
	ocean_assert(std::unordered_set<const NodeSpecification*>(nodeSpecifications.cbegin(), nodeSpecifications.cend()).size() == nodeSpecifications.size() && "At least one node specification twice");

	for (const NodeSpecification* nodeSpecification : nodeSpecifications)
	{
		ocean_assert(nodeSpecification != nullptr);

		if (nodeSpecification != nullptr)
		{
			addNode(vertsDriver, *nodeSpecification);
		}
	}
}

Entity::~Entity()
{
	if (vertsEntity_ != nullptr)
	{
		verts_entity_release(vertsEntity_);
	}
}

SharedNode Entity::node(const std::string& nodeType)
{
	for (const SharedNode& node : nodes_)
	{
		if (nodeType == node->nodeType())
		{
			return node;
		}
	}

	ocean_assert(false && "The node does not exist");
	return nullptr;
}

bool Entity::hasNode(const std::string& nodeType) const
{
	for (const SharedNode& node : nodes_)
	{
		if (nodeType == node->nodeType())
		{
			return true;
		}
	}

	return false;
}

uint32_t Entity::ownerSessionId() const
{
	if (vertsEntity_ != nullptr)
	{
		return 0u;
	}

	return verts_entity_get_creator_session(vertsEntity_);
}

uint64_t Entity::ownerUserId(Driver& driver) const
{
	if (vertsEntity_ != nullptr || driver.isInitialized())
	{
		return 0u;
	}

	const uint32_t sessionId = verts_entity_get_creator_session(vertsEntity_);

	return verts_driver_get_user_for_session(driver.vertsDriver_, sessionId);
}

const SharedNode& Entity::addNode(verts_driver* vertsDriver, const NodeSpecification& nodeSpecification)
{
	ocean_assert(vertsDriver != nullptr);

	nodes_.emplace_back(new Node(vertsDriver, nodeSpecification, vertsEntity_));

	return nodes_.back();
}

const SharedNode& Entity::addNode(verts_component* vertsComponent, const NodeSpecification& nodeSpecification)
{
	nodes_.emplace_back(new Node(vertsComponent, nodeSpecification));

	return nodes_.back();
}

}

}

}
