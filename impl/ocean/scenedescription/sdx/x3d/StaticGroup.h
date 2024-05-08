/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_STATIC_GROUP_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_STATIC_GROUP_H

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
 * This class implements a x3d static group node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT StaticGroup :
	virtual public X3DChildNode,
	virtual public X3DBoundedObject
{
	public:

		/**
		 * Creates a new x3d static group node.
		 * @param environment Node environment
		 */
		explicit StaticGroup(const SDXEnvironment* environment);

		/**
		 * Destructs a static group node.
		 */
		~StaticGroup() override;

	protected:

		/**
		 * Specifies the node type and the fields of this node.
		 * @return Unique node specification of this node
		 */
		NodeSpecification specifyNode();

		/**
		 * Event function to inform the node that it has been initialized and can apply all internal values to corresponding rendering objects.
		 * @see SDXNode::onInitialize().
		 */
		void onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp) override;

		/**
		 * Returns the address of the most derived object.
		 * @see Node::objectAddress().
		 */
		size_t objectAddress() const override;

	protected:

		/// Children field.
		MultiNode children_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_STATIC_GROUP_H
