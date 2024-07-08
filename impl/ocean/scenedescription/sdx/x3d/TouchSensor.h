/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_TOUCH_SENSOR_NODE_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_TOUCH_SENSOR_NODE_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DTouchSensorNode.h"
#include "ocean/scenedescription/sdx/x3d/X3DGroupingNode.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements a touch sensor node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT TouchSensor : virtual public X3DTouchSensorNode
{
	public:

		/**
		 * Creates an abstract touch sensor node.
		 * @param environment Node environment
		 */
		explicit TouchSensor(const SDXEnvironment* environment);

	protected:

		/**
		 * Specifies the node type and the fields of this node.
		 * @return Unique node specification of this node
		 */
		NodeSpecification specifyNode();

		/**
		 * Mouse event function.
		 * @see SDXEventNode().
		 */
		void onMouse(const ButtonType button, const ButtonEvent buttonEvent, const Vector2& screenPosition, const Vector3& objectPosition, const Rendering::ObjectId objectId, const Timestamp timestamp) override;

		/**
		 * Key event function.
		 * @see SDXEventNode().
		 */
		void onKey(const int key, const ButtonEvent buttonEvent, const Rendering::ObjectId objectId, const Timestamp timestamp) override;

		/**
		 * Returns whether the selected rendering object is an object of the sibling nodes.
		 * @param objectId Rendering object id to check
		 * @return True, if so
		 */
		bool isSiblingNode(const Rendering::ObjectId objectId) const;

		/**
		 * Returns whether a specified group object holds a specific rendering object id.
		 * @param group X3D grouping node
		 * @param objectId Rendering object id to check
		 * @return True, if so
		 */
		static bool hasRenderingObject(const SmartObjectRef<X3DGroupingNode, Node>& group, const Rendering::ObjectId objectId);

		/**
		 * Returns the address of the most derived object.
		 * @see Node::objectAddress().
		 */
		size_t objectAddress() const override;

	protected:

		/// HitNormal_changed field.
		SingleVector3 hitNormalChanged_;

		/// HitPoint_changed field.
		SingleVector3 hitPointChanged_;

		/// HitTexCoord_changed field.
		SingleVector2 hitTexCoordChanged_;

		/// Last object hit position.
		Vector3 lastObjectPosition_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_TOUCH_SENSOR_NODE_H
