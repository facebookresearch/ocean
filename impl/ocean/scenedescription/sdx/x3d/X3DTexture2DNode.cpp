/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/X3DTexture2DNode.h"

#include "ocean/media/Manager.h"

#include "ocean/rendering/MediaTexture2D.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

X3DTexture2DNode::X3DTexture2DNode(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DAppearanceChildNode(environment),
	X3DTextureNode(environment),
	repeatS_(true),
	repeatT_(true)
{
	renderingObject_ = engine()->factory().createMediaTexture2D();
}

X3DTexture2DNode::~X3DTexture2DNode()
{
	if (initialized_)
	{
		unregisterThisNodeAsParent(textureProperties_.value());
	}
}

void X3DTexture2DNode::registerFields(NodeSpecification& specification)
{
	registerField(specification, "repeatS", repeatS_, ACCESS_NONE);
	registerField(specification, "repeatT", repeatT_, ACCESS_NONE);
	registerField(specification, "textureProperties", textureProperties_, ACCESS_NONE);

	X3DTextureNode::registerFields(specification);
}

void X3DTexture2DNode::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DTextureNode::onInitialize(scene, timestamp);

	try
	{
		const Rendering::Texture2DRef renderingTexture(renderingObject_);

		if (renderingTexture)
		{
			registerThisNodeAsParent(textureProperties_.value());

			const SDXNodeRef texturePropertiesNode(textureProperties_.value());
			if (texturePropertiesNode)
			{
				texturePropertiesNode->initialize(scene, timestamp);
			}

			renderingTexture->setWrapTypeS(repeatS_.value() ? Rendering::Texture::WRAP_REPEAT : Rendering::Texture::WRAP_CLAMP);
			renderingTexture->setWrapTypeT(repeatT_.value() ? Rendering::Texture::WRAP_REPEAT : Rendering::Texture::WRAP_CLAMP);
		}
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}
}

void X3DTexture2DNode::applyUrl(const StringVector& resolvedURLs, const Media::FrameMedium::Type mediumType, const bool start)
{
	Rendering::MediaTexture2DRef renderingTexture2D(renderingObject_);
	if (renderingTexture2D.isNull())
	{
		return;
	}

	Media::FrameMediumRef medium;

	if (!resolvedURLs.empty())
	{
		for (StringVector::const_iterator i = resolvedURLs.begin(); i != resolvedURLs.end(); ++i)
		{
			medium = Media::Manager::get().newMedium(*i, mediumType);

			if (medium)
			{
				break;
			}
		}

		if (medium.isNull())
		{
			Log::warning() << "Failed to load a texture \"" << resolvedURLs[0] << "\".";
		}
	}

	if (medium)
	{
		onMediumChanged(medium);

		if (start)
		{
			medium->start();
		}
	}

	renderingTexture2D->setMedium(medium);
	textureMedium_ = medium;

	// check which texture environment mode has to be chosen
	bool shouldUseModulate = false;
	bool shouldUseReplace = false;

	const NodeRefs parents(parentNodes());
	for (NodeRefs::const_iterator i = parents.begin(); i != parents.end(); ++i)
	{
		ocean_assert(*i);

		if ((*i)->hasField("material") && (*i)->field<SingleNode>("material").value())
		{
			shouldUseModulate = true;
		}
		else
		{
			shouldUseReplace = true;
		}
	}

	if (shouldUseModulate && shouldUseReplace)
	{
		Log::warning() << "The texture environment mode of texture \"" << name() << "\" can not be chosen correctly.";
	}

	if (!shouldUseModulate)
	{
		renderingTexture2D->setEnvironmentMode(Rendering::Texture::MODE_REPLACE);
	}
	else
	{
		renderingTexture2D->setEnvironmentMode(Rendering::Texture::MODE_MODULATE);
	}
}

void X3DTexture2DNode::onMediumChanged(const Media::MediumRef& /*medium*/)
{
	// nothing to do here
}

}

}

}

}
