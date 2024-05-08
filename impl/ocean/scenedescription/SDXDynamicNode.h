/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_DYNAMIC_NODE_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_DYNAMIC_NODE_H

#include "ocean/scenedescription/SceneDescription.h"
#include "ocean/scenedescription/DynamicNode.h"
#include "ocean/scenedescription/SDXNode.h"

namespace Ocean
{

namespace SceneDescription
{

// Forward declaration.
class SDXDynamicNode;

/**
 * Definition of a smart object reference for SDX dynamic scene description nodes.
 * @see SDXDynamicNode, Node.
 * @ingroup scenedescription
 */
typedef SmartObjectRef<SDXDynamicNode, Node> SDXDynamicNodeRef;

/**
 * This class implements the base class for all dynamic SDX scene description nodes.
 * @ingroup scenedescription
 */
class OCEAN_SCENEDESCRIPTION_EXPORT SDXDynamicNode :
	virtual public DynamicNode,
	virtual public SDXNode
{
	public:

		/**
		 * Sets or changes a specified standard or dynamic field of this node.
		 * @param fieldName Name of the field to set
		 * @param field Field to replace the current field
		 * @return True, if succeeded
		 */
		bool setAnyField(const std::string& fieldName, const Field& field) override;

		/**
		 * Sets or changes a specified dynamic field of this node.
		 * @param fieldName Name of the field to set
		 * @param field Field to replace the current field
		 * @return True, if succeeded
		 */
		virtual bool setDynamicField(const std::string& fieldName, const Field& field);

	protected:

		/**
		 * Creates a new dynamic SDX node object.
		 * @param environment Node environment
		 */
		explicit SDXDynamicNode(const SDXEnvironment* environment);

		/**
		 * Destructs a dynamic SDX node object.
		 */
		~SDXDynamicNode() override;
};

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_DYNAMIC_NODE_H
