/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_LIVE_VIDEO_TEXTURE_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_LIVE_VIDEO_TEXTURE_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DTexture2DNode.h"
#include "ocean/scenedescription/sdx/x3d/X3DUrlObject.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements an unspecified x3d live video texture node.
 * The x3d specification does not contain this node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT LiveVideoTexture :
	virtual public X3DTexture2DNode,
	virtual public X3DUrlObject
{
	public:

		/**
		 * Creates a new live video texture node.
		 * @param environment Node environment
		 */
		LiveVideoTexture(const SDXEnvironment* environment);

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
		 * Event function to inform the node about a changed field.
		 * @see SDXNode::onFieldChanged().
		 */
		void onFieldChanged(const std::string& fieldName) override;

		/**
		 * Event function to modify properties of a new medium before it will be started and used as texture.
		 * @see X3DTexture2DNode::onMediumChanged().
		 */
		void onMediumChanged(const Media::MediumRef& medium) override;

		/**
		 * Returns the address of the most derived object.
		 * @see Node::objectAddress().
		 */
		size_t objectAddress() const override;

	protected:

		/// Dimension field.
		SingleVector2 dimension_;

		/// Frequency field.
		SingleFloat frequency_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_LIVE_VIDEO_TEXTURE_H
