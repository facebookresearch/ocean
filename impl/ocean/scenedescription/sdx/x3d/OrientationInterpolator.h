/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_ORIENTATION_INTERPOLATOR_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_ORIENTATION_INTERPOLATOR_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DInterpolatorNode.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements an orientation interpolator node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT OrientationInterpolator : virtual public X3DInterpolatorNode
{
	private:

		/**
		 * Definition of a vector holding quaternion rotation values.
		 */
		typedef std::vector<Quaternion> QuaternionValues;

	public:

		/**
		 * Creates a new orientation interpolator node.
		 * @param environment Node environment
		 */
		explicit OrientationInterpolator(const SDXEnvironment* environment);

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
		 * Event function to inform the node about a changed field.
		 * @see SDXNode::onFieldChanged().
		 */
		void onFieldChanged(const std::string& fieldName) override;

		/**
		 * Event function for single value 'interpolates'.
		 * @see X3DInterpolationNode::onSingleValue().
		 */
		void onSingleValue(const size_t index, const Timestamp eventTimestamp) override;

		/**
		 * Event function to interpolate between two values.
		 * @see X3DInterpolationNode::onInterpolate().
		 */
		void onInterpolate(const size_t leftIndex, const size_t rightIndex, const Scalar interpolationFactor, const Timestamp eventTimestamp) override;

		/**
		 * Returns the address of the most derived object.
		 * @see Node::objectAddress().
		 */
		size_t objectAddress() const override;

	protected:

		/// KeyValue field.
		MultiRotation keyValue_;

		/// Value_changed field.
		SingleRotation valueChanged_;

		/// Vector holding the quaternion rotation values of the key values.
		QuaternionValues quaternionRotations_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_ORIENTATION_INTERPOLATOR_H
