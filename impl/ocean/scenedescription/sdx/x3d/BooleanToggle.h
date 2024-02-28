// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_BOOLEAN_TOGGLE_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_BOOLEAN_TOGGLE_H

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
 * This class implements a x3d boolean toggle node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT BooleanToggle : virtual public X3DChildNode
{
	public:

		/**
		 * Creates a new x3d boolean toggle node.
		 * @param environment Node environment
		 */
		explicit BooleanToggle(const SDXEnvironment* environment);

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

		/// Toggle field.
		SingleBool toggle_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_BOOLEAN_TOGGLE_H
