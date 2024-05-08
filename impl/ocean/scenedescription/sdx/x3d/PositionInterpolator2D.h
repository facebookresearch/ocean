/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_POSITION_INTERPOLATOR_2D_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_POSITION_INTERPOLATOR_2D_H

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
 * This class implements a position interpolator 2d node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT PositionInterpolator2D : virtual public X3DInterpolatorNode
{
	public:

		/**
		 * Creates a new position interpolator 2d node.
		 * @param environment Node environment
		 */
		explicit PositionInterpolator2D(const SDXEnvironment* environment);

	protected:

		/**
		 * Specifies the node type and the fields of this node.
		 * @return Unique node specification of this node
		 */
		NodeSpecification specifyNode();

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
		MultiVector2 keyValue_;

		/// Value_changed field.
		SingleVector2 valueChanged_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_POSITION_INTERPOLATOR_2D_H
