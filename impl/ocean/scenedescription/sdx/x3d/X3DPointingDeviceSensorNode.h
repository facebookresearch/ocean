/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_POINTING_DEVICE_SENSOR_NODE_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_POINTING_DEVICE_SENSOR_NODE_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DSensorNode.h"

#include "ocean/scenedescription/SDXEventNode.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements an abstract x3d pointing device sensor node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT X3DPointingDeviceSensorNode :
	virtual public X3DSensorNode,
	virtual public SDXEventNode
{
	protected:

		/**
		 * Creates an abstract x3d pointing device sensor node.
		 * @param environment Node environment
		 */
		explicit X3DPointingDeviceSensorNode(const SDXEnvironment* environment);

		/**
		 * Registers the fields of this node.
		 * @param specification Node specification receiving the field informations
		 */
		void registerFields(NodeSpecification& specification);

	protected:

		/// Description field.
		SingleString description_;

		/// IsOver field.
		SingleBool isOver_;

		/// IsActive field.
		SingleBool isActive_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_POINTING_DEVICE_SENSOR_NODE_H
