/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/X3DScene.h"

#include "ocean/rendering/Group.h"
#include "ocean/rendering/Scene.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

X3DScene::X3DScene(const std::string& filename, const Library& library, const Rendering::EngineRef& engine) :
	SDXNode(nullptr),
	X3DNode(nullptr),
	X3DChildNode(nullptr),
	X3DBoundedObject(nullptr),
	X3DGroupingNode(nullptr),
	Scene(filename),
	SDXScene(filename, library, engine)
{
	// this node environment pointer will be set in SDXScene()
	ocean_assert(environment_ != nullptr);

	static NodeSpecification specification(specifyNode());
	specification_ = &specification;

	renderingObject_ = engine->factory().createGroup();
	ocean_assert(renderingObject_);

	renderingSceneObject_ = engine->factory().createScene();
	ocean_assert(renderingSceneObject_);

	renderingSceneObject_->addChild(renderingObject_);
}

X3DScene::NodeSpecification X3DScene::specifyNode()
{
	NodeSpecification specification("X3DScene");

	X3DGroupingNode::registerFields(specification);

	return specification;
}

void X3DScene::onInitialize(const Timestamp timestamp)
{
	try
	{
		Rendering::GroupRef renderingGroup(renderingObject_);
		if (renderingGroup)
		{
			for (MultiNode::Values::const_iterator i = children_.values().begin(); i != children_.values().end(); ++i)
			{
				const SDXNodeRef childNode(*i);

				registerThisNodeAsParent(childNode);
				childNode->initialize(renderingSceneObject_, timestamp);

				renderingGroup->addChild(childNode->renderingObject());
			}
		}
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}
}

void X3DScene::onInitialize(const Rendering::SceneRef& /*scene*/, const Timestamp /*timestamp*/)
{
	ocean_assert(false && "This function should never be used!");
}

const Rendering::ObjectRef& X3DScene::renderingObject() const
{
	return renderingSceneObject_;
}

size_t X3DScene::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
