/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_SOUND_SOURCE_NODE_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_SOUND_SOURCE_NODE_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DTimeDependentNode.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements an abstract x3d sound source node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT X3DSoundSourceNode : virtual public X3DTimeDependentNode
{
	protected:

		/**
		 * Creates a new abstract x3d sound source node.
		 */
		X3DSoundSourceNode(const SDXEnvironment* environment);

		/**
		 * Registers the fields of this node.
		 * @param specification Node specification receiving the field informations
		 */
		void registerFields(NodeSpecification& specification);

	protected:

		/// Description field.
		SingleString description_;

		/// Pitch field.
		SingleFloat pitch_;

		/// Duration_changed field.
		SingleTime durationChanged_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_SOUND_SOURCE_NODE_H
