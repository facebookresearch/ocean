/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_SHADER_PART_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_SHADER_PART_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DNode.h"
#include "ocean/scenedescription/sdx/x3d/X3DUrlObject.h"

#include "ocean/rendering/ShaderProgram.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements a x3d shader part node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT ShaderPart :
	virtual public X3DNode,
	virtual public X3DUrlObject
{
	friend class ComposedShader;

	public:

		/**
		 * Creates a new shader part node.
		 * @param environment Node environment
		 */
		ShaderPart(const SDXEnvironment* environment);

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
		 * Returns the address of the most derived object.
		 * @see Node::objectAddress().
		 */
		size_t objectAddress() const override;

		/**
		 * Returns the pair of filename and shader type defined by this shader part.
		 * @return The shader part's pair of filename and shader type
		 */
		Rendering::ShaderProgram::FilenamePair filenamePair() const;

	protected:

		/// Type field.
		SingleString type_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_SHADER_PART_H
