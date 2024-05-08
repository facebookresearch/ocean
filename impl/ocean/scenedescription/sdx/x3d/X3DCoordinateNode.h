/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_COORDINATE_NODE_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_COORDINATE_NODE_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DGeometricPropertyNode.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements an abstract x3d coordinate node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT X3DCoordinateNode : virtual public X3DGeometricPropertyNode
{
	protected:

		/**
		 * Creates a new x3d coordinate node.
		 * @param environment Node environment
		 */
		explicit X3DCoordinateNode(const SDXEnvironment* environment);
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_COORDINATE_NODE_H
