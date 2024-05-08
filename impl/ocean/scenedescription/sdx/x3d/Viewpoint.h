/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_VIEWPOINT_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_VIEWPOINT_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DViewpointNode.h"

#include "ocean/scenedescription/SDXUpdateNode.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements a x3d viewpoint node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT Viewpoint :
	virtual public X3DViewpointNode,
	virtual public SDXUpdateNode
{
	public:

		/**
		 * Creates a new x3d viewpoint node.
		 * @param environment Node environment
		 */
		explicit Viewpoint(const SDXEnvironment* environment);

	protected:

		/**
		 * Specifies the node type and the fields of this node.
		 * @return Unique node specification of this node
		 */
		NodeSpecification specifyNode();

		/**
		 * Returns the address of the most derived object.
		 * @see Node::objectAddress().
		 */
		size_t objectAddress() const override;

		/**
		 * Pre update event function.
		 * @see SDXUpdateNode::onPreUpdate().
		 */
		Timestamp onPreUpdate(const Rendering::ViewRef& view, const Timestamp timestamp) override;

	protected:

		/// FieldOfView field.
		SingleFloat fieldOfView_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_VIEWPOINT_H
