/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_BACKGROUND_NODE_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_BACKGROUND_NODE_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DBindableNode.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements an abstract x3d background node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT X3DBackgroundNode : virtual public X3DBindableNode
{
	protected:

		/**
		 * Creats a new x3d background node.
		 * @param environment Node environment
		 */
		explicit X3DBackgroundNode(const SDXEnvironment* environment);

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
		 * Event function to inform the node about a changed field.
		 * @see SDXNode::onFieldChanged().
		 */
		void onFieldChanged(const std::string& fieldName) override;

	protected:

		/// GroundAngle field.
		MultiFloat groundAngle_;

		/// GroundColor field.
		MultiColor groundColor_;

		/// SkyAngle field.
		MultiFloat skyAngle_;

		/// SkyColor field.
		MultiColor skyColor_;

		/// Transparency field.
		SingleFloat transparency_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_BACKGROUND_NODE_H
