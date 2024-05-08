/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_APPEARANCE_CHILD_NODE_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_APPEARANCE_CHILD_NODE_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DNode.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements an abstract x3d appearance child node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT X3DAppearanceChildNode : virtual public X3DNode
{
	protected:

		/**
		 * Creates a new x3d appearance child node.
		 * @param environment Node environment
		 */
		explicit X3DAppearanceChildNode(const SDXEnvironment* environment);
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_APPEARANCE_CHILD_NODE_H
