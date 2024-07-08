/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_MULTI_TEXTURE_TRANSFORM_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_MULTI_TEXTURE_TRANSFORM_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DTextureTransformNode.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements a x3d multi texture transform node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT MultiTextureTransform : virtual public X3DTextureTransformNode
{
	public:

		/**
		 * Creates a new x3d multi texture transform node.
		 * @param environment Node environment
		 */
		explicit MultiTextureTransform(const SDXEnvironment* environment);

	protected:

		/**
		 * Destructs a multi texture transform node.
		 */
		~MultiTextureTransform() override;

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
		 * Explicit changing event function for node fields.
		 * @see X3DNode::onFieldChanging().
		 */
		bool onFieldChanging(const std::string& fieldName, const Field& field) override;

		/**
		 * Returns the address of the most derived object.
		 * @see Node::objectAddress().
		 */
		size_t objectAddress() const override;

	protected:

		/// TextureTransform field.
		MultiNode textureTransform_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_MULTI_TEXTURE_TRANSFORM_H
