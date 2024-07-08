/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_BINDABLE_NODE_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_BINDABLE_NODE_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DChildNode.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements an abstract x3d bindable node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT X3DBindableNode : virtual public X3DChildNode
{
	public:

		/**
		 * Creates a new abstract x3d bindable node.
		 * @param environment Node environment
		 */
		explicit X3DBindableNode(const SDXEnvironment* environment);

	protected:

		/**
		 * Registers the fields of this node.
		 * @param specification Node specification receiving the field informations
		 */
		void registerFields(NodeSpecification& specification);

	protected:

		/// Set_bind field.
		SingleBool setBind_;

		/// BindTime field.
		SingleTime bindTime_;

		/// IsBound field.
		SingleBool isBound_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_BINDABLE_NODE_H
