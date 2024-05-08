/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_SHADER_NODE_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_SHADER_NODE_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DAppearanceChildNode.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements an abstract x3d shader node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT X3DShaderNode : virtual public X3DAppearanceChildNode
{
	protected:

		/**
		 * Creates a new abstract x3d shader node.
		 * @param environment Node environment
		 */
		X3DShaderNode(const SDXEnvironment* environment);

		/**
		 * Registers the fields of this node.
		 * @param specification Node specification receiving the field informations
		 */
		void registerFields(NodeSpecification& specification);

	protected:

		/// Activate field.
		SingleBool activate_;

		/// IsSelected.
		SingleBool isSelected_;

		/// IsValid field.
		SingleBool isValid_;

		/// Language field.
		SingleString language_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_SHADER_NODE_H
