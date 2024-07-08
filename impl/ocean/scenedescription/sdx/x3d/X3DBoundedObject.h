/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_BOUNDED_OBJECT_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_BOUNDED_OBJECT_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"

#include "ocean/scenedescription/SDXNode.h"
#include "ocean/scenedescription/Field0D.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements an abstract x3d bounded object.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT X3DBoundedObject : virtual public SDXNode
{
	protected:

		/**
		 * Creates a new x3d bounded object.
		 * @param environment Node environment
		 */
		explicit X3DBoundedObject(const SDXEnvironment* environment);

		/**
		 * Registers the fields of this node.
		 * @param specification Node specification receiving the field informations
		 */
		void registerFields(NodeSpecification& specification);

	protected:

		/// BBoxCenter node.
		SingleVector3 bboxCenter_;

		/// BBoxSize node.
		SingleVector3 bboxSize_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_BOUNDED_OBJECT_H
