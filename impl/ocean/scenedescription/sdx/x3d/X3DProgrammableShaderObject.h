/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_PROGRAMMABLE_SHADER_OBJECT_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_PROGRAMMABLE_SHADER_OBJECT_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"

#include "ocean/scenedescription/SDXNode.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements an abstract x3d programmable shader object.
 * @ingroup x3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT X3DProgrammableShaderObject : virtual public SDXNode
{
	protected:

		/**
		 * Creates a new x3d programmable shader object.
		 * @param environment Node environment
		 */
		X3DProgrammableShaderObject(const SDXEnvironment* environment);

		/**
		 * Registers the fields of this node.
		 * @param specification Node specification receiving the field informations
		 */
		void registerFields(NodeSpecification& specification);
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_PROGRAMMABLE_SHADER_OBJECT_H
