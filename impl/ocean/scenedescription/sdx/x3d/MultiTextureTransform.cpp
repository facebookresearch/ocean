/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/MultiTextureTransform.h"
#include "ocean/scenedescription/sdx/x3d/TextureTransform.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

MultiTextureTransform::MultiTextureTransform(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DAppearanceChildNode(environment),
	X3DTextureTransformNode(environment)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;
}

MultiTextureTransform::~MultiTextureTransform()
{
	if (initialized_)
	{
		for (MultiNode::Values::const_iterator i = textureTransform_.values().begin(); i != textureTransform_.values().end(); ++i)
		{
			unregisterThisNodeAsParent(*i);
		}
	}
}

MultiTextureTransform::NodeSpecification MultiTextureTransform::specifyNode()
{
	NodeSpecification specification("MultiTextureTransform");

	registerField(specification, "textureTransform", textureTransform_, ACCESS_GET_SET);

	X3DTextureTransformNode::registerFields(specification);

	return specification;
}

void MultiTextureTransform::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DTextureTransformNode::onInitialize(scene, timestamp);

	try
	{
		for (MultiNode::Values::const_iterator i = textureTransform_.values().begin(); i != textureTransform_.values().end(); ++i)
		{
			const SDXNodeRef childNode(*i);

			registerThisNodeAsParent(childNode);
			childNode->initialize(scene, timestamp);
		}
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}
}

bool MultiTextureTransform::onFieldChanging(const std::string& fieldName, const Field& field)
{
	if (fieldName == "textureTransform" && field.isType(Field::TYPE_NODE, 1))
	{
		const MultiNode& multiNode = Field::cast<const MultiNode>(field);

		for (MultiNode::Values::const_iterator i = textureTransform_.values().begin(); i != textureTransform_.values().end(); ++i)
		{
			unregisterThisNodeAsParent(*i);
		}

		textureTransform_.setValues(multiNode.values(), field.timestamp());

		for (MultiNode::Values::const_iterator i = textureTransform_.values().begin(); i != textureTransform_.values().end(); ++i)
		{
			registerThisNodeAsParent(*i);

			SmartObjectRef<TextureTransform, Node> textureTransformNode(*i);
			ocean_assert(textureTransformNode);
			textureTransformNode->applyTextureTransform();
		}

		return true;
	}

	return X3DTextureTransformNode::onFieldChanging(fieldName, field);
}

size_t MultiTextureTransform::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
