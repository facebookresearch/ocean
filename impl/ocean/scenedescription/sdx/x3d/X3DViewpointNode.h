/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_VIEWPOINT_NODE_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_VIEWPOINT_NODE_H

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
 * This class implements an abstract x3d viewpoint node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT X3DViewpointNode : virtual public X3DBindableNode
{
	public:

		/**
		 * Creates a new abstract x3d viewpoint node.
		 * @param environment Node environment
		 */
		explicit X3DViewpointNode(const SDXEnvironment* environment);

	protected:

		/**
		 * Registers the fields of this node.
		 * @param specification Node specification receiving the field informations
		 */
		void registerFields(NodeSpecification& specification);

	protected:

		/// CenterOfRotation field.
		SingleVector3 centerOfRotation_;

		/// Description field.
		SingleString description_;

		/// Jump field.
		SingleBool jump_;

		/// Orientation field.
		SingleRotation orientation_;

		/// Position field.
		SingleVector3 position_;

		/// RetainUserOffsets field.
		SingleBool retainUserOffsets_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_VIEWPOINT_NODE_H
