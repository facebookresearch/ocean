/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_ENVIRONMENTAL_SENSOR_NODE_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_ENVIRONMENTAL_SENSOR_NODE_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DSensorNode.h"

#include "ocean/scenedescription/SDXUpdateNode.h"

#include "ocean/rendering/Node.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements an abstract x3d envrionmental sensor node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT X3DEnvironmentalSensorNode :
	virtual public X3DSensorNode,
	virtual public SDXUpdateNode
{
	protected:

		/**
		 * Creates an abstract x3d environmental sensor node.
		 * @param environment Node environment
		 */
		explicit X3DEnvironmentalSensorNode(const SDXEnvironment* environment);

		/**
		 * Registers the fields of this node.
		 * @param specification Node specification receiving the field informations
		 */
		void registerFields(NodeSpecification& specification);

		/**
		 * Update event function.
		 * @see SDXUpdateNode::onUpdate().
		 */
		void onUpdate(const Rendering::ViewRef& view, const Timestamp timestamp) override;

		/**
		 * Event function for the new position and orientaiton inside the defined bounding box.
		 * This function should be used by derivated classes.
		 * @param translation Local translation inside the sensor bounding box
		 * @param orientation Local orientation inside the sensor bounding box
		 * @param timestamp Event timestamp
		 */
		virtual void onInsideBoundingBox(const Vector3& translation, const Quaternion& orientation, const Timestamp timestamp) = 0;

		/**
		 * Returns the coordinate systems of the sensor node in world.
		 * @return All possible sensor node coordinate systems, which is world_T_sensors
		 */
		HomogenousMatrices4 sensorTransformations() const;

	protected:

		/// Center field.
		SingleVector3 center_;

		/// Size field.
		SingleVector3 size_;

		/// EnterTime field.
		SingleTime enterTime_;

		/// ExitTime field.
		SingleTime exitTime_;

		/// IsActive field.
		SingleBool isActive_;

		/// State determining the enter state.
		bool sensorEntered_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_ENVIRONMENTAL_SENSOR_NODE_H
