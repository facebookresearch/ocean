// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_SPOT_LIGHT_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_SPOT_LIGHT_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DLightNode.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements a x3d spot light node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT SpotLight : virtual public X3DLightNode
{
	public:

		/**
		 * Creates a new x3d spot light node.
		 * @param environment Node environment
		 */
		explicit SpotLight(const SDXEnvironment* environment);

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
		 * Event function to update the position or direction of a light source with global state.
		 * @see SDXNode::onGlobalLight().
		 */
		void onGlobalLight(const HomogenousMatrix4& world_T_light) override;

		/**
		 * Returns the address of the most derived object.
		 * @see Node::objectAddress().
		 */
		size_t objectAddress() const override;

	protected:

		/// Attenuation field.
		SingleVector3 attenuation_;

		/// BeamWidth field.
		SingleFloat beamWidth_;

		/// CutOffAngle field.
		SingleFloat cutOffAngle_;

		/// Direction field.
		SingleVector3 direction_;

		/// Location field.
		SingleVector3 location_;

		/// Radius field.
		SingleFloat radius_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_SPOT_LIGHT_H
