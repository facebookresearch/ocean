/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/X3DLightNode.h"
#include "ocean/scenedescription/SDXScene.h"

#include "ocean/rendering/LightSource.h"
#include "ocean/rendering/Group.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

X3DLightNode::X3DLightNode(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DChildNode(environment),
	SDXUpdateNode(environment),
	ambientIntensity_(0),
	color_(RGBAColor(1, 1, 1)),
	global_(false),
	intensity_(1),
	on_(true)
{
	// nothing to do here
}

X3DLightNode::~X3DLightNode()
{
	unregisterLight(global_.value());
}

void X3DLightNode::registerFields(NodeSpecification& specification)
{
	registerField(specification, "ambientIntensity", ambientIntensity_, ACCESS_GET_SET);
	registerField(specification, "color", color_, ACCESS_GET_SET);
	registerField(specification, "global", global_, ACCESS_GET_SET);
	registerField(specification, "intensity", intensity_, ACCESS_GET_SET);
	registerField(specification, "on", on_, ACCESS_GET_SET);

	X3DChildNode::registerFields(specification);
}

void X3DLightNode::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DChildNode::onInitialize(scene, timestamp);

	try
	{
		const Rendering::LightSourceRef renderingLightSource(renderingObject_);

		if (renderingLightSource)
		{
			renderingLightSource->setAmbientColor(color_.value().damped(float(ambientIntensity_.value())));
			renderingLightSource->setDiffuseColor(color_.value());
			renderingLightSource->setIntensity(intensity_.value());
			renderingLightSource->setEnabled(on_.value());

			registerLight(global_.value());
		}
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}
}

void X3DLightNode::onFieldChanged(const std::string& fieldName)
{
	try
	{
		const Rendering::LightSourceRef renderingLightSource(renderingObject_);

		if (renderingLightSource)
		{
			if (fieldName == "ambientIntensity")
			{
				renderingLightSource->setAmbientColor(color_.value().damped(float(ambientIntensity_.value())));
			}
			else if (fieldName == "diffuseColor")
			{
				renderingLightSource->setDiffuseColor(color_.value());
			}
			else if (fieldName == "intensity")
			{
				renderingLightSource->setIntensity(intensity_.value());
			}
			else if (fieldName == "on")
			{
				renderingLightSource->setEnabled(on_.value());
			}
			else if (fieldName == "global")
			{
				unregisterLight(!global_.value());
				registerLight(global_.value());
			}
		}
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}

	X3DChildNode::onFieldChanged(fieldName);
}

void X3DLightNode::onUpdate(const Rendering::ViewRef& /*view*/, const Timestamp /*timestamp*/)
{
	if (global_.value())
	{
		const Rendering::LightSourceRef renderingLightSource(renderingObject_);

		if (renderingLightSource)
		{
			const NodeRefs parents(parentNodes());

			for (const NodeRef& parent : parents)
			{
				const SDXNodeRef sdxParent(parent);
				ocean_assert(sdxParent);

				const Rendering::NodeRef parentRenderingNode(sdxParent->renderingObject());
				ocean_assert(parentRenderingNode);

				if (parentRenderingNode && parentRenderingNode->visible())
				{
					const HomogenousMatrices4 transformations(parentRenderingNode->worldTransformations());
					if (!transformations.empty())
					{
						renderingLightSource->setEnabled(on_.value());

						onGlobalLight(transformations.front());
						return;
					}
				}
			}
		}

		/// the parent rendering object is invisible/disabled so we disable the global light as well
		renderingLightSource->setEnabled(false);
	}
}

void X3DLightNode::registerLight(bool willBeGlobal)
{
	const Rendering::LightSourceRef renderingLightSource(renderingObject_);

	if (renderingLightSource)
	{
		if (willBeGlobal)
		{
			renderingLightSource->setTransformationType(Rendering::LightSource::TT_WORLD);

			const SDXSceneRef owningScene(environment_->library()->nodeManager().scene(sceneId()));
			ocean_assert(owningScene);

			owningScene->registerGlobalLight(renderingLightSource);
		}
		else
		{
			renderingLightSource->setTransformationType(Rendering::LightSource::TT_PARENT);

			const NodeRefs parents(parentNodes());

			for (NodeRefs::const_iterator i = parents.begin(); i != parents.end(); ++i)
			{
				const SDXNodeRef parent(*i);
				ocean_assert(parent);

				const Rendering::GroupRef parentGroup(parent->renderingObject());

				if (parentGroup)
				{
					parentGroup->registerLight(renderingLightSource);
				}
			}
		}
	}
}

void X3DLightNode::unregisterLight(const bool wasGlobal)
{
	const Rendering::LightSourceRef renderingLightSource(renderingObject_);

	if (renderingLightSource)
	{
		if (wasGlobal)
		{
			const SDXSceneRef owningScene(environment_->library()->nodeManager().scene(sceneId()));

			if (owningScene)
			{
				owningScene->unregisterGlobalLight(renderingLightSource);
			}
		}
		else
		{
			const NodeRefs parents(parentNodes());

			for (NodeRefs::const_iterator i = parents.begin(); i != parents.end(); ++i)
			{
				SDXNodeRef parent(*i);
				ocean_assert(parent);

				const Rendering::GroupRef parentGroup(parent->renderingObject());

				if (parentGroup)
				{
					parentGroup->unregisterLight(renderingLightSource);
				}
			}
		}
	}
}

}

}

}

}
