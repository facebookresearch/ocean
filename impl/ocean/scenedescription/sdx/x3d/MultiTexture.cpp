/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/MultiTexture.h"

#include "ocean/rendering/Textures.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

MultiTexture::MultiTexture(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DAppearanceChildNode(environment),
	X3DTextureNode(environment),
	alpha_(1),
	color_(RGBAColor(1, 1, 1))
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;

	renderingObject_ = engine()->factory().createTextures();
}

MultiTexture::~MultiTexture()
{
	if (initialized_)
	{
		for (MultiNode::Values::const_iterator i = texture_.values().begin(); i != texture_.values().end(); ++i)
		{
			unregisterThisNodeAsParent(*i);
		}
	}
}

MultiTexture::NodeSpecification MultiTexture::specifyNode()
{
	NodeSpecification specification("MultiTexture");

	registerField(specification, "alpha", alpha_, ACCESS_GET_SET);
	registerField(specification, "color", color_, ACCESS_GET_SET);
	registerField(specification, "function", function_, ACCESS_GET_SET);
	registerField(specification, "mode", mode_, ACCESS_GET_SET);
	registerField(specification, "source", source_, ACCESS_GET_SET);
	registerField(specification, "texture", texture_, ACCESS_GET_SET);

	X3DTextureNode::registerFields(specification);

	return specification;
}

void MultiTexture::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DTextureNode::onInitialize(scene, timestamp);

	Rendering::TexturesRef renderingTextures(renderingObject());

	Rendering::Texture::EnvironmentMode environmentMode(Rendering::Texture::MODE_MODULATE);
	if (!mode_.values().empty())
	{
		environmentMode = translateEnvironmentMode(mode_.values().front());
	}

	unsigned int n = 0u;
	for (MultiNode::Values::const_iterator i = texture_.values().begin(); i != texture_.values().end(); ++i)
	{
		const SDXNodeRef child(*i);
		ocean_assert(child);

		registerThisNodeAsParent(child);
		child->initialize(scene, timestamp);

		const Rendering::TextureRef renderingTexture(child->renderingObject());
		ocean_assert(renderingTexture);

		if (mode_.values().size() > n)
		{
			environmentMode = translateEnvironmentMode(mode_.values()[n]);
		}

		renderingTexture->setEnvironmentMode(environmentMode);

		if (renderingTextures)
		{
			renderingTextures->addTexture(renderingTexture);
		}

		++n;
	}
}

void MultiTexture::onFieldChanged(const std::string& /*fieldName*/)
{
	ocean_assert(false && "Missing implementation.");
}

Rendering::Texture::EnvironmentMode MultiTexture::translateEnvironmentMode(const std::string& mode)
{
	if (mode == "MODULATE")
	{
		return Rendering::Texture::MODE_MODULATE;
	}
	else if (mode == "REPLACE")
	{
		return Rendering::Texture::MODE_REPLACE;
	}
	else if (mode == "ADD")
	{
		return Rendering::Texture::MODE_ADD;
	}
	else if (mode == "ADDSIGNED")
	{
		return Rendering::Texture::MODE_ADD_SIGNED;
	}
	else if (mode == "SUBTRACT")
	{
		return Rendering::Texture::MODE_SUBTRACT;
	}
	else if (mode == "BLENDDIFFUSEALPHA")
	{
		return Rendering::Texture::MODE_BLEND;
	}

	Log::warning() << "Unsupported X3D MultiTexture mode value: \"" << mode << "\" using MODULATE instead.";

	return Rendering::Texture::MODE_MODULATE;
}

size_t MultiTexture::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
