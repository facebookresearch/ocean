/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_GROUPING_NODE_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_GROUPING_NODE_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DChildNode.h"
#include "ocean/scenedescription/sdx/x3d/X3DBoundedObject.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements an abstract x3d grouping node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT X3DGroupingNode :
	virtual public X3DChildNode,
	virtual public X3DBoundedObject
{
	protected:

		/**
		 * Creates an abstract x3d grouping node.
		 * @param environment Node environment
		 */
		explicit X3DGroupingNode(const SDXEnvironment* environment);

		/**
		 * Destructs a grouping node.
		 */
		~X3DGroupingNode() override;

		/**
		 * Registers the fields of this node.
		 * @param specification Node specification receiving the field informations
		 */
		void registerFields(NodeSpecification& specification);

		/**
		 * Event function to inform the node that it has been initialized and can apply all internal values to corresponding rendering objects.
		 * @see SDXNode::onInitialize().
		 */
		void onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp) override;

		/**
		 * Explicit changing event function for node fields.
		 * @see X3DNode::onFieldChanging().
		 */
		bool onFieldChanging(const std::string& fieldName, const Field& field) override;

	protected:

		/// AddChild field.
		MultiNode addChildren_;

		/// RemoveChildren field.
		MultiNode removeChildren_;

		/// Children field.
		MultiNode children_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_GROUPING_NODE_H
