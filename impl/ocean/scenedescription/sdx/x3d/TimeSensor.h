/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_TIME_SENSOR_NODE_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_TIME_SENSOR_NODE_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DTimeDependentNode.h"
#include "ocean/scenedescription/sdx/x3d/X3DSensorNode.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements a time sensor node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT TimeSensor :
	virtual public X3DTimeDependentNode,
	virtual public X3DSensorNode
{
	public:

		/**
		 * Creates an abstract time sensor node.
		 * @param environment Node environment
		 */
		explicit TimeSensor(const SDXEnvironment* environment);

	protected:

		/**
		 * Specifies the node type and the fields of this node.
		 * @return Unique node specification of this node
		 */
		NodeSpecification specifyNode();

		/**
		 * Event function to inform the node that it has been initialized and can apply all internal values to corresponding rendering objects.
		 * @see SDXNode::onInitialize().
		 */
		void onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp) override;

		/**
		 * Explicit changing event function for node fields.
		 * @see X3DNode::onFieldChanging().
		 */
		bool onFieldChanging(const std::string& fieldName, const Field& field) override;

		/**
		 * Event function to inform the node about a changed field.
		 * @see SDXNode::onFieldChanged().
		 */
		void onFieldChanged(const std::string& fieldName) override;

		/**
		 * Event function for node update events.
		 * @see X3DTimeDependentNode::onUpdated().
		 */
		void onUpdated(const Timestamp timestamp) override;

		/**
		 * Event function for node start events.
		 * @see X3DTimeDependentNode::onStarted().
		 */
		void onStarted(const Timestamp eventTimestamp) override;

		/**
		 * Event function for node pause events.
		 * @see X3DTimeDependentNode::onPaused().
		 */
		void onPaused(const Timestamp eventTimestamp) override;

		/**
		 * Event function for node resume events.
		 * @see X3DTimeDependentNode::onResumed().
		 */
		void onResumed(const Timestamp eventTimestamp) override;

		/**
		 * Event function for node stop events.
		 * @see X3DTimeDependentNode::onStopped().
		 */
		void onStopped(const Timestamp eventTimestamp) override;

		/**
		 * Returns the address of the most derived object.
		 * @see Node::objectAddress().
		 */
		size_t objectAddress() const override;

	protected:

		/// CycleInterval field.
		SingleTime cycleInterval_;

		/// CycleTime field.
		SingleTime cycleTime_;

		/// Fraction_changed field.
		SingleFloat fractionChanged_;

		/// Time field.
		SingleTime time_;

		/// Last fraction value.
		Scalar lastFraction_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_TIME_SENSOR_NODE_H
