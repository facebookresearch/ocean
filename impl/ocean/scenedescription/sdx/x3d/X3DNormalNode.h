// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_NORMAL_NODE_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_NORMAL_NODE_H

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
 * This class implements an abstract x3d normal node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT X3DNormalNode : virtual public X3DGeometricPropertyNode
{
	protected:

		/**
		 * Creates a new x3d normal node.
		 * @param environment Node environment
		 */
		explicit X3DNormalNode(const SDXEnvironment* environment);
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_NORMAL_NODE_H
