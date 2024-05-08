/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_PROXIMITY_SENSOR_NODE_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_PROXIMITY_SENSOR_NODE_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DEnvironmentalSensorNode.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements a proximity sensor node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT ProximitySensor : virtual public X3DEnvironmentalSensorNode
{
	public:

		/**
		 * Creates an abstract proximity sensor node.
		 * @param environment Node environment
		 */
		explicit ProximitySensor(const SDXEnvironment* environment);

	protected:

		/**
		 * Specifies the node type and the fields of this node.
		 * @return Unique node specification of this node
		 */
		NodeSpecification specifyNode();

		/**
		 * Event function for the new position and orientaiton inside the defined bounding box.
		 * @see EnvironmentalSensorNode::onInsideBoundingBox().
		 */
		void onInsideBoundingBox(const Vector3& translation, const Quaternion& orientation, const Timestamp timestamp) override;

		/**
		 * Returns the address of the most derived object.
		 * @see Node::objectAddress().
		 */
		size_t objectAddress() const override;

	protected:

		/// CenterOfRotation_changed field.
		SingleVector3 centerOfRotationChanged_;

		/// Orientation_changed field.
		SingleRotation orientationChanged_;

		/// Position_changed field.
		SingleVector3 positionChanged_;

		/// Last local orientation.
		Quaternion lastLocalOrientation_;

		/// Last local translation.
		Vector3 lastLocalTranslation_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_PROXIMITY_SENSOR_NODE_H
