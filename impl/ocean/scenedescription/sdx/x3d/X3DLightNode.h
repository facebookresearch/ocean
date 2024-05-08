/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_LIGHT_NODE_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_LIGHT_NODE_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DChildNode.h"

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
 * This class implements an abstract x3d light node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT X3DLightNode :
	virtual public X3DChildNode,
	virtual public SDXUpdateNode
{
	protected:

		/**
		 * Creates an abstract x3d light node.
		 * @param environment Node environment
		 */
		explicit X3DLightNode(const SDXEnvironment* environment);

		/**
		 * Destructs a light node.
		 */
		~X3DLightNode() override;

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
		 * Update event function.
		 * @see SDXUpdateNode::onUpdate().
		 */
		void onUpdate(const Rendering::ViewRef& view, const Timestamp timestamp) override;

		/**
		 * Event function to update the position or direction of a light source with global state.
		 * @param world_T_light The global light transformation, must be valid
		 */
		virtual void onGlobalLight(const HomogenousMatrix4& world_T_light) = 0;

		/**
		 * Registers the fields of this node.
		 * @param specification Node specification receiving the field informations
		 */
		void registerFields(NodeSpecification& specification);

		/**
		 * Registers this light at the renering objects.
		 * @param willBeGlobal True, if the light will be a global light; False, if the light will be a local light
		 */
		void registerLight(const bool willBeGlobal);

		/**
		 * Unregisters this light at the renering objects.
		 * @param wasGlobal True, if the light was a global light; False, if the light was a local light
		 */
		void unregisterLight(const bool wasGlobal);

	protected:

		/// AmbientIntensity field.
		SingleFloat ambientIntensity_;

		/// Color field.
		SingleColor color_;

		/// Global field.
		SingleBool global_;

		/// Intensity field.
		SingleFloat intensity_;

		/// On field.
		SingleBool on_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_LIGHT_NODE_H
