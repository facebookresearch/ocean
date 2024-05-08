/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_HEAD_UP_TRANSFORM_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_HEAD_UP_TRANSFORM_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DGroupingNode.h"

#include "ocean/rendering/Scene.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements an ocean specific X3D node to allow head-up nodes.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT HeadUpTransform : virtual public X3DGroupingNode
{
	public:

		/**
		 * Creates a new ocean head-up transform node.
		 * @param environment Node environment
		 */
		explicit HeadUpTransform(const SDXEnvironment* environment);

	protected:

		/**
		 * Destructs an ocean head-up transform node.
		 */
		~HeadUpTransform() override;

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
		 * Event function to inform the node about a changed field.
		 * @see SDXNode::onFieldChanged().
		 */
		void onFieldChanged(const std::string& fieldName) override;

		/**
		 * Returns the address of the most derived object.
		 * @see Node::objectAddress().
		 */
		size_t objectAddress() const override;

	protected:

		/// ScreenPosition Field.
		SingleVector2 screenPosition_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_HEAD_UP_TRANSFORM_H
