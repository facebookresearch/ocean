/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_NORMAL_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_NORMAL_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DNormalNode.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements a x3d normal node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT Normal : virtual public X3DNormalNode
{
	public:

		/**
		 * Creates a new x3d normal node.
		 * @param environment Node environment
		 */
		explicit Normal(const SDXEnvironment* environment);

	protected:

		/**
		 * Specifies the node type and the fields of this node.
		 * @return Unique node specification of this node
		 */
		NodeSpecification specifyNode();

		/**
		 * Event function to inform the node about a changed field.
		 * @see SDXNode::onFieldChanged().
		 */
		void onFieldChanged(const std::string& fieldName) override;

		/**
		 * Returns the address of the most derived object.
		 * @see Node::objectAddress().
		 */
		size_t objectAddress() const override;

	protected:

		/// Vector field.
		MultiVector3 vector_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_NORMAL_H
