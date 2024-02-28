// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_NETWORK_VERTS_ENTITY_H
#define FACEBOOK_NETWORK_VERTS_ENTITY_H

#include "ocean/network/verts/Verts.h"
#include "ocean/network/verts/Node.h"

// Forward declaration
struct verts_entity;
struct verts_driver;

namespace Ocean
{

namespace Network
{

namespace Verts
{

/// Forward declaration.
class Driver;

/// Forward declaration.
class Entity;

/**
 * Definition of a shared ptr holding a Entity.
 * @see Entity.
 * @ingroup networkverts
 */
typedef std::shared_ptr<Entity> SharedEntity;

/**
 * Definition of a vector holding SharedEntity objects.
 * @see Entity, SharedEntity.
 * @ingroup networkverts
 */
typedef std::vector<SharedEntity> SharedEntities;

/**
 * This class implements an entity able to hold several nodes with different node types.
 * Entities hold all the data necessary to represent any kind of entity (e.g., an Avatar, virtual 3D object, etc.) in a distributed system.<br>
 * An entity mainly wrapps a VERTS entity.
 * @ingroup networkverts
 */
class OCEAN_NETWORK_VERTS_EXPORT Entity
{
	friend class Driver;

	public:

		/**
		 * Definition of unique id for all entities.
		 */
		typedef uint64_t EntityId;

	public:

		/**
		 * Destructs an entity object.
		 */
		~Entity();

		/**
		 * Returns a specific node of this entity.
		 * @param nodeType The type of the node to return, must be valid
		 * @return The specified node, nullptr if the node does not exist
		 */
		SharedNode node(const std::string& nodeType);

		/**
		 * Returns the nodes of this entity.
		 * @return The entity's nodes
		 */
		inline const SharedNodes& nodes() const;

		/**
		 * Returns whether this entity has a specific node.
		 * @param nodeType The type of the node to check, must be valid
		 * @return True, if so
		 */
		bool hasNode(const std::string& nodeType) const;

		/**
		 * Returns the unique id of this entity.
		 * @return The entity's unique id
		 */
		inline EntityId entityId() const;

		/**
		 * Returns whether this entity has been deleted and should not be used anymore.
		 * @return True, if so
		 */
		inline bool hasBeenDeleted() const;

		/**
		 * Returns the id of the session owning this entity.
		 * @return The owner's session id
		 */
		uint32_t ownerSessionId() const;

		/**
		 * Returns the id of the user owning this entity.
		 * @return The owner's user id
		 */
		uint64_t ownerUserId(Driver& driver) const;

		/**
		 * Returns an invalid entity id.
		 * @return Invalid entity id
		 */
		static constexpr EntityId invalidEntityId();

	protected:

		/**
		 * Creates a new entity object not yet holding any components.
		 * @param vertsDriver The VERTS driver to which the entity will be added, must be valid
		 */
		explicit Entity(verts_driver* vertsDriver);

		/**
		 * Creates a new entity object for an existing VERTS entity.
		 * @param vertsEntity The existing VERTS entity, must be valid
		 */
		explicit Entity(verts_entity* vertsEntity);

		/**
		 * Creates a new entity object holding one node.
		 * @param vertsDriver The VERTS driver to which the entity will be added, must be valid
		 * @param nodeSpecification The specification of the node which will be added to the entity
		 */
		Entity(verts_driver* vertsDriver, const NodeSpecification& nodeSpecification);

		/**
		 * Creates a new entity object holding several different nodes.
		 * @param vertsDriver The VERTS driver to which the entity will be added, must be valid
		 * @param nodeTypes The types of the nodes which will be added to the entity, each node type must be different
		 */
		Entity(verts_driver* vertsDriver, const std::vector<std::string>& nodeTypes);

		/**
		 * Creates a new entity object holding several different nodes.
		 * @param vertsDriver The VERTS driver to which the entity will be added, must be valid
		 * @param nodeSpecifications The specifications of the nodes which will be added to the entity, each node specification must be different
		 */
		Entity(verts_driver* vertsDriver, const std::vector<const NodeSpecification*>& nodeSpecifications);

		/**
		 * Adds a new node to this entity.
		 * @param vertsDriver The VERTS driver owning this entity, must be valid
		 * @param nodeSpecification The specification of the new node
		 * @return The resulting node
		 */
		const SharedNode& addNode(verts_driver* vertsDriver, const NodeSpecification& nodeSpecification);

		/**
		 * Adds an existing node (represented by a VERTS component) to this entity.
		 * @param vertsComponent The VERTS component representing the node, must be valid
		 * @param nodeSpecification The specification of the new node
		 * @return The resulting node
		 */
		const SharedNode& addNode(verts_component* vertsComponent, const NodeSpecification& nodeSpecification);

		/**
		 * Informs the entity that it has been deleted.
		 */
		inline void informHasBeenDeleted();

	protected:

		/// The entity's nodes.
		SharedNodes nodes_;

		/// The corresponding VERTS entity.
		verts_entity* vertsEntity_ = nullptr;

		/// The unique id of this entity.
		EntityId entityId_ = invalidEntityId();

		/// True, if the entity has been deleted.
		bool hasBeenDeleted_ = false;
};

inline const SharedNodes& Entity::nodes() const
{
	return nodes_;
}

inline Entity::EntityId Entity::entityId() const
{
	return entityId_;
}

inline bool Entity::hasBeenDeleted() const
{
	return hasBeenDeleted_;
}

constexpr Entity::EntityId Entity::invalidEntityId()
{
	return EntityId(-1);
}

void Entity::informHasBeenDeleted()
{
	hasBeenDeleted_ = true;
}

}

}

}

#endif // FACEBOOK_NETWORK_VERTS_ENTITY_H
