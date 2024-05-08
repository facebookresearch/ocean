/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/Inline.h"

#include "ocean/rendering/Group.h"

#include "ocean/scenedescription/Manager.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

Inline::Inline(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DChildNode(environment),
	X3DBoundedObject(environment),
	X3DUrlObject(environment),
	load_(true),
	hasScenes_(false)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;

	renderingObject_ = engine()->factory().createGroup();
}

Inline::~Inline()
{
	unloadScenes();
}

Inline::NodeSpecification Inline::specifyNode()
{
	NodeSpecification specification("Inline");

	registerField(specification, "load", load_);

	X3DChildNode::registerFields(specification);
	X3DBoundedObject::registerFields(specification);
	X3DUrlObject::registerFields(specification);

	return specification;
}

void Inline::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DChildNode::onInitialize(scene, timestamp);
	X3DBoundedObject::onInitialize(scene, timestamp);
	X3DUrlObject::onInitialize(scene, timestamp);

	if (load_.value())
	{
		loadScenes(timestamp);
	}
}

void Inline::onFieldChanged(const std::string& fieldName)
{
	if (fieldName == "load")
	{
		if (load_.value() && hasScenes_ == false)
		{
			loadScenes(load_.timestamp());
		}

		return;
	}
	else if (fieldName == "url")
	{
		unloadScenes();

		if (load_.value())
		{
			loadScenes(url_.timestamp());
		}

		return;
	}

	X3DChildNode::onFieldChanged(fieldName);
	X3DBoundedObject::onFieldChanged(fieldName);
	X3DUrlObject::onFieldChanged(fieldName);
}

void Inline::loadScenes(const Timestamp& timestamp)
{
	ocean_assert(timestamp.isValid());

	hasScenes_ = false;

	Rendering::GroupRef renderingGroup(renderingObject_);
	if (renderingGroup.isNull())
	{
		return;
	}

	const IO::Files resolvedFiles(resolveUrls());

	bool fileExisted = false;

	for (IO::Files::const_iterator i = resolvedFiles.begin(); i != resolvedFiles.end(); ++i)
	{
		if (i->exists())
		{
			fileExisted = true;

			SceneRef newScene(Manager::get().load((*i)(), engine(), timestamp));

			if (newScene)
			{
				Rendering::SceneRef renderingScene;

				if (newScene->descriptionType() == TYPE_TRANSIENT)
				{
					const SDLSceneRef sdlScene(newScene);
					ocean_assert(sdlScene);

					renderingScene = sdlScene->apply(engine());
				}
				else
				{
					ocean_assert(newScene->descriptionType() == TYPE_PERMANENT);

					permanentScene_ = newScene;
					ocean_assert(permanentScene_);

					renderingScene = permanentScene_->renderingObject();
				}

				ocean_assert(renderingScene);
				renderingGroup->addChild(renderingScene);
				hasScenes_ = true;
				break;
			}
		}
	}

	if (!hasScenes_ && !resolvedFiles.empty())
	{
		if (fileExisted)
		{
			Log::error() << "Failed to load inline file \"" << resolvedFiles.front()() << "\": Unsupported file type.";
		}
		else
		{
			Log::error() << "Failed to load inline file \"" << resolvedFiles.front()() << "\": File could not be found.";
		}
	}
}

void Inline::unloadScenes()
{
	Rendering::GroupRef renderingGroup(renderingObject_);
	if (renderingGroup.isNull())
	{
		return;
	}

	renderingGroup->clear();

	if (permanentScene_)
	{
		Manager::get().unload(permanentScene_->sceneId());
	}

	hasScenes_ = false;
}

size_t Inline::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
