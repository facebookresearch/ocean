/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_COMPOSED_SHADER_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_COMPOSED_SHADER_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DProgrammableShaderObject.h"
#include "ocean/scenedescription/sdx/x3d/X3DShaderNode.h"

#include "ocean/rendering/ShaderProgram.h"

#include "ocean/scenedescription/SDXDynamicNode.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements a x3d composed shader node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT ComposedShader :
	virtual public X3DProgrammableShaderObject,
	virtual public X3DShaderNode,
	virtual public SDXDynamicNode
{
	public:

		/**
		 * Creates a new packaged shader node.
		 * @param environment Node environment
		 */
		ComposedShader(const SDXEnvironment* environment);

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
		 * Applies a given parameter to the shader.
		 * @param shaderProgram Shader program holding the parameter to be updated
		 * @param fieldName Name of the field to apply
		 * @return True, if succeeded
		 */
		bool applyParameter(const Rendering::ShaderProgramRef& shaderProgram, const std::string& fieldName);

		/**
		 * Returns the address of the most derived object.
		 * @see Node::objectAddress().
		 */
		size_t objectAddress() const override;

	protected:

		/// The parts field.
		MultiNode parts_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_COMPOSED_SHADER_H
