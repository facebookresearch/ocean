/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_SHAPE_NODE_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_SHAPE_NODE_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DBoundedObject.h"
#include "ocean/scenedescription/sdx/x3d/X3DChildNode.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements an abstract x3d shape node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT X3DShapeNode :
	virtual public X3DBoundedObject,
	virtual public X3DChildNode
{
	protected:

		/**
		 * Creates a new abstract x3d shape node.
		 * @param environment Node environment
		 */
		explicit X3DShapeNode(const SDXEnvironment* environment);

	protected:

		/**
		 * Destructs a x3d shape node.
		 */
		~X3DShapeNode() override;

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

		/// Appearance field.
		SingleNode appearance_;

		/// Geometry field.
		SingleNode geometry_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_SHAPE_NODE_H
