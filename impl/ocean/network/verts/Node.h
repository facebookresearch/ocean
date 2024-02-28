// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_NETWORK_VERTS_NODE_H
#define FACEBOOK_NETWORK_VERTS_NODE_H

#include "ocean/network/verts/Verts.h"
#include "ocean/network/verts/NodeSpecification.h"

// Forward declaration
struct verts_component;
struct verts_driver;
struct verts_entity;

namespace Ocean
{

namespace Network
{

namespace Verts
{

// Forward declaration.
class Node;

/**
 * Definition of a shared ptr holding a Node.
 * @see Node.
 * @ingroup networkverts
 */
typedef std::shared_ptr<Node> SharedNode;

/**
 * Definition of a vector holding SharedNode objects.
 * @see Node, SharedNode.
 * @ingroup networkverts
 */
typedef std::vector<SharedNode> SharedNodes;

/**
 * This class implements a node holding fields with individual values.
 * Components hold a part of the data necessary to represent any kind of entity (e.g., a part of an Avatar, or the material of a virtual 3D object, etc.).<br>
 * Several components are stored in an entity.<br>
 * A node mainly wrapps a VERTS component.
 * @ingroup networkverts
 */
class OCEAN_NETWORK_VERTS_EXPORT Node
{
	friend class Driver;
	friend class Entity;

	public:

		/**
		 * Definition of a node id.
		 */
		typedef uint64_t NodeId;

		/**
		 * Usage of StringBuffer.
		 */
		using StringBuffer = NodeSpecification::StringBuffer;

		/**
		 * Usages of StringPointer.
		 */
		using StringPointer = NodeSpecification::StringPointer;

	public:

		/**
		 * Returns the type of the node.
		 * @return The node's type
		 */
		inline const std::string& nodeType() const;

		/**
		 * Returns the specification of this node containing e.g., the number of fields, field types, etc.
		 * @return The node's specification
		 */
		inline const NodeSpecification& nodeSpecification() const;

		/**
		 * Returns the value of a specific field.
		 * @param fieldName The name of the field, must be valid
		 * @param timestamp Optional timestamp for which the value will be returned, -1 to return the latest value
		 * @return The field's value
		 * @tparam T The data type of the field
		 */
		template <typename T>
		T field(const std::string& fieldName, const uint64_t timestamp = uint64_t(-1)) const;

		/**
		 * Returns the value of a specific field.
		 * @param fieldIndex The index of the field, with range [0, nodeSpecification().fields() - 1]
		 * @param timestamp Optional timestamp for which the value will be returned, -1 to return the latest value
		 * @return The field's value
		 * @tparam T The data type of the field
		 */
		template <typename T>
		T field(const Index32 fieldIndex, const uint64_t timestamp = uint64_t(-1)) const;

		/**
		 * Sets the value of a specific field.
		 * @param fieldName The name of the field to set, must be valid
		 * @param value The value to set
		 * @return True, if succeeded
		 * @tparam T The data type of the field
		 */
		template <typename T>
		bool setField(const std::string& fieldName, const T& value);

		/**
		 * Sets the value of a specific field.
		 * @param fieldIndex The index of the field to set, with range [0, nodeSpecification().fields() - 1]
		 * @param value The value to set
		 * @return True, if succeeded
		 * @tparam T The data type of the field
		 */
		template <typename T>
		bool setField(const Index32 fieldIndex, const T& value);

		/**
		 * Returns the unique id of the this node.
		 * @return The node's id
		 */
		inline NodeId nodeId() const;

		/**
		 * Returns whether this node has changed since the last hasChanged() call.
		 * @param changedFieldIndices Optional resulting indices of all fields which have changed, nullptr if not of interest
		 * @return True, if so
		 */
		inline bool hasChanged(UnorderedIndexSet32* changedFieldIndices = nullptr);

		/**
		 * Returns whether the component has been initialized and whether it is ready for use.
		 * @return True, if so
		 */
		inline bool isInitialized() const;

		/**
		 * Returns whether this entity has been deleted and should not be used anymore.
		 * @return True, if so
		 */
		inline bool hasBeenDeleted() const;

		/**
		 * Returns whether the node is valid.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Returns an invalid node id.
		 * @return Invalid node id
		 */
		static constexpr NodeId invalidNodeId();

	protected:

		/**
		 * Adds a new node to an entity.
		 * @param vertsDriver The VERTS driver to which the node will be added, must be valid
		 * @param nodeSpecification The specification of the new node
		 * @param vertsEntity The VERTS entity to which the node (the corresponding VERTS component) will be added
		 */
		Node(verts_driver* vertsDriver, const NodeSpecification& nodeSpecification, verts_entity* vertsEntity);

		/**
		 * Creates a node for an existing VERTS component.
		 * @param vertsComponent The VERTS component for which the node will be created, must be valid
		 * @param nodeSpecification The specification of the node
		 */
		Node(verts_component* vertsComponent, const NodeSpecification& nodeSpecification);

		/**
		 * Informs the node that a field has changed.
		 * @param fieldIndex The index of the field which has changed, with range [0, nodeSpecification().fields() - 1]
		 */
		inline void informChangedField(const Index32 fieldIndex);

		/**
		 * Informs the node that a field has changed.
		 * @param fieldName The name of the field, must be valid
		 */
		inline void informChangedField(const std::string& fieldName);

		/**
		 * Informs the node that it has been initialized.
		 */
		inline void informHasBeenInitialized();

		/**
		 * Informs the node that it has been deleted.
		 */
		inline void informHasBeenDeleted();

		/**
		 * Disabled copy constructor.
		 * @param node The node which would be copied
		 */
		Node(const Node& node) = delete;

		/**
		 * Disabled assign operator.
		 * @param node The node which would be copied
		 * @return Reference to this node
		 */
		Node& operator=(const Node& node) = delete;

	protected:

		/// The specification of this node.
		const NodeSpecification& nodeSpecification_;

		/// The corresponding VERTS component.
		verts_component* vertsComponent_ = nullptr;

		/// The unique id of this node.
		NodeId nodeId_ = invalidNodeId();

		/// The indices of all fields which have changed since the last call of hasChanged().
		UnorderedIndexSet32 changedFieldIndices_;

		/// True, if the node has been initialized.
		bool isInitialized_ = false;

		/// True, if the entity has been deleted.
		bool hasBeenDeleted_ = false;
};

inline const std::string& Node::nodeType() const
{
	return nodeSpecification_.name();
}

inline const NodeSpecification& Node::nodeSpecification() const
{
	return nodeSpecification_;
}

template <typename T>
T Node::field(const std::string& fieldName, const uint64_t timestamp) const
{
	const Index32 fieldIndex = nodeSpecification_.fieldIndex(fieldName);

	if (fieldIndex == NodeSpecification::invalidFieldIndex())
	{
		ocean_assert(false && "Invalid field!");
		return T();
	}

	return field<T>(fieldIndex, timestamp);
}

template <typename T>
bool Node::setField(const std::string& fieldName, const T& value)
{
	const Index32 fieldIndex = nodeSpecification_.fieldIndex(fieldName);

	if (fieldIndex == NodeSpecification::invalidFieldIndex())
	{
		ocean_assert(false && "Invalid field!");
		return false;
	}

	return setField<T>(fieldIndex, value);
}

inline Node::NodeId Node::nodeId() const
{
	return nodeId_;
}

inline bool Node::hasChanged(UnorderedIndexSet32* changedFieldIndices)
{
	const bool result = !changedFieldIndices_.empty();

	if (changedFieldIndices != nullptr)
	{
		std::swap(*changedFieldIndices, changedFieldIndices_);
	}

	changedFieldIndices_.clear();

	return result;
}

inline void Node::informChangedField(const Index32 fieldIndex)
{
	ocean_assert(fieldIndex != NodeSpecification::invalidFieldIndex());

	changedFieldIndices_.emplace(fieldIndex);
}

inline void Node::informChangedField(const std::string& fieldName)
{
	informChangedField(nodeSpecification_.fieldIndex(fieldName));
}

inline bool Node::isInitialized() const
{
	return isInitialized_;
}

inline bool Node::hasBeenDeleted() const
{
	return hasBeenDeleted_;
}

inline bool Node::isValid() const
{
	return vertsComponent_ != nullptr;
}

constexpr Node::NodeId Node::invalidNodeId()
{
	return NodeId(-1);
}

void Node::informHasBeenInitialized()
{
	isInitialized_ = true;
}

void Node::informHasBeenDeleted()
{
	hasBeenDeleted_ = true;
}

}

}

}

#endif // FACEBOOK_NETWORK_VERTS_NODE_H
