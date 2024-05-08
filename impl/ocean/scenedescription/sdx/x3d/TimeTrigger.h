/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_TIME_TRIGGER_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_TIME_TRIGGER_H

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
 * This class implements a x3d time trigger node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT TimeTrigger : virtual public X3DChildNode
{
	public:

		/**
		 * Creates a new x3d time trigger node.
		 * @param environment Node environment
		 */
		explicit TimeTrigger(const SDXEnvironment* environment);

	protected:

		/**
		 * Specifies the node type and the fields of this node.
		 * @return Unique node specification of this node
		 */
		NodeSpecification specifyNode();

		/**
		 * Explicit changing event function for node fields.
		 * @see X3DNode::onFieldChanging().
		 */
		bool onFieldChanging(const std::string& fieldName, const Field& field) override;

		/**
		 * Returns the address of the most derived object.
		 * @see Node::objectAddress().
		 */
		size_t objectAddress() const override;

	protected:

		/// Set_boolean field.
		SingleBool setBoolean_;

		/// TriggerTime field.
		SingleTime triggerTime_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_TIME_TRIGGER_H
