/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_MOVIE_TEXTURE_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_MOVIE_TEXTURE_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DTexture2DNode.h"
#include "ocean/scenedescription/sdx/x3d/X3DSoundSourceNode.h"
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
 * This class implements a x3d movie texture node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT MovieTexture :
	virtual public X3DTexture2DNode,
	virtual public X3DSoundSourceNode,
	virtual public X3DUrlObject
{
	public:

		/**
		 * Creates a new movie texture node.
		 * @param environment Node environment
		 */
		MovieTexture(const SDXEnvironment* environment);

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
		 * Event function for node update events.
		 * @see X3DTimeDependentNode::onUpdated().
		 */
		void onUpdated(const Timestamp timestamp) override;

		/**
		 * Event function for node start events.
		 * @see X3DTimeDependentNode::onStarted().
		 */
		void onStarted(const Timestamp eventTimestamp) override;

		/**
		 * Event function for node pause events.
		 * @see X3DTimeDependentNode::onPaused().
		 */
		void onPaused(const Timestamp eventTimestamp) override;

		/**
		 * Event function for node resume events.
		 * @see X3DTimeDependentNode::onStarted().
		 */
		void onResumed(const Timestamp eventTimestamp) override;

		/**
		 * Event function for node stop events.
		 * @see X3DTimeDependentNode::onStarted().
		 */
		void onStopped(const Timestamp eventTimestamp) override;

		/**
		 * Returns the address of the most derived object.
		 * @see Node::objectAddress().
		 */
		size_t objectAddress() const override;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDL_X3D_MOVIE_TEXTURE_H
