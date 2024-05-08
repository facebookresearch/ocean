/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_SHADER_PROGRAM_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_SHADER_PROGRAM_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DNode.h"
#include "ocean/scenedescription/sdx/x3d/X3DUrlObject.h"
#include "ocean/scenedescription/sdx/x3d/X3DProgrammableShaderObject.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements a x3d shader program node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT ShaderProgram :
	virtual public X3DNode,
	virtual public X3DUrlObject,
	virtual public X3DProgrammableShaderObject
{
	public:

		/**
		 * Creates a new shader program node.
		 * @param environment Node environment
		 */
		explicit ShaderProgram(const SDXEnvironment* environment);

	protected:

		/**
		 * Specifies the node type and the fields of this node.
		 * @return Unique node specification of this node
		 */
		NodeSpecification specifyNode();

		/**
		 * Returns the address of the most derived object.
		 * @see Node::objectAddress().
		 */
		size_t objectAddress() const override;

	protected:

		/// Type field.
		SingleString type_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_SHADER_PROGRAM_H
