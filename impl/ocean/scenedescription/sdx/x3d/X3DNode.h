/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_NODE_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_NODE_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"

#include "ocean/base/Callback.h"

#include "ocean/scenedescription/SDXNode.h"
#include "ocean/scenedescription/Field0D.h"
#include "ocean/scenedescription/Field1D.h"

#include <map>

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

// Forward declaration.
class X3DNode;

/**
 * Definition of a smart object reference for abstract X3D nodes.
 * @see X3DNode, Node.
 * @ingroup scenedescription
 */
typedef SmartObjectRef<X3DNode, Node> X3DNodeRef;

/**
 * This class implements an abstract x3d node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT X3DNode : virtual public SDXNode
{
	protected:

		/**
		 * Definition of a node and field pair.
		 */
		typedef std::pair<NodeId, std::string> ReceiverPair;

		/**
		 * Definition of a map mapping field names to field receiver pairs.
		 */
		typedef std::multimap<std::string, ReceiverPair> FieldConnectionMap;

	public:

		/**
		 * Sets or changes a specified field of this node.
		 * @see SDXNode::setField().
		 */
		bool setField(const std::string& fieldName, const Field& field) override;

		/**
		 * Adds a field connection for a specified field starting at this node.
		 * @param outputField Name of the output field
		 * @param inputNode Id of the input node
		 * @param inputField Name of the input field
		 * @return True, if the connection has not been added before
		 */
		bool addConnection(const std::string& outputField, const NodeId inputNode, const std::string& inputField);

		/**
		 * Tries to translate an alias field name to the original field name.
		 * @see Node::originalFieldName().
		 */
		std::string originalFieldName(const std::string& fieldName) const override;

	protected:

		/**
		 * Creates a new abstract x3d node.
		 * @param environment Node environment
		 */
		explicit X3DNode(const SDXEnvironment* environment);

		/**
		 * Registers the fields of this node.
		 * @param specification Node specification receiving the field informations
		 */
		void registerFields(NodeSpecification& specification);

		/**
		 * Explicit changing event function for node fields.
		 * All node fields will be informed on field changes by this event function only.<br>
		 * The new field value has to be handles explicitly!<br>
		 * Values fields have a different event function.
		 * @param fieldName Name of the field to be changed
		 * @param field Field holding the new value and timestamp
		 * @return True, if the field value has been accepted
		 * @see onFieldChanged().
		 */
		virtual bool onFieldChanging(const std::string& fieldName, const Field& field);

		/**
		 * Informs connected nodes that a field of this node has been changed.
		 * @param fieldName Name of this node field which has been changed
		 * @param field Field of this node which has been changed
		 */
		void forwardThatFieldHasBeenChanged(const std::string& fieldName, const Field& field);

	protected:

		/// Metadata field.
		SingleNode metadata_;

		/// Map holding all field connections.
		FieldConnectionMap fieldConnections_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_NODE_H
