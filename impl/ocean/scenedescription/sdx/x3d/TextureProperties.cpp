/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/TextureProperties.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

TextureProperties::TextureProperties(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	anisotropicDegree_(1),
	borderColor_(RGBAColor(0.0f, 0.0f, 0.0f, 0.0f)),
	borderWidth_(0),
	boundaryModeS_("REPEAT"),
	boundaryModeT_("REPEAT"),
	boundaryModeR_("REPEAT"),
	magnificationFilter_("FASTEST"),
	minificationFilter_("FASTEST"),
	textureCompression_("FASTEST"),
	texturePriority_(0.0f),
	generateMipMaps_(false)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;
}

TextureProperties::NodeSpecification TextureProperties::specifyNode()
{
	NodeSpecification specification("TextureProperties");

	registerField(specification, "anisotropicDegree", anisotropicDegree_, ACCESS_GET_SET);
	registerField(specification, "borderColor", borderColor_, ACCESS_GET_SET);
	registerField(specification, "borderWidth", borderWidth_, ACCESS_GET_SET);
	registerField(specification, "boundaryModeS", boundaryModeS_, ACCESS_GET_SET);
	registerField(specification, "boundaryModeT", boundaryModeT_, ACCESS_GET_SET);
	registerField(specification, "boundaryModeR", boundaryModeR_, ACCESS_GET_SET);
	registerField(specification, "magnificationFilter", magnificationFilter_, ACCESS_GET_SET);
	registerField(specification, "minificationFilter", minificationFilter_, ACCESS_GET_SET);
	registerField(specification, "textureCompression", textureCompression_, ACCESS_GET_SET);
	registerField(specification, "texturePriority", texturePriority_, ACCESS_GET_SET);
	registerField(specification, "generateMipMaps", generateMipMaps_, ACCESS_NONE);

	X3DNode::registerFields(specification);

	return specification;
}

void TextureProperties::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DNode::onInitialize(scene, timestamp);

	try
	{
		const NodeRefs parents(parentNodes());

		for (NodeRefs::const_iterator i = parents.begin(); i != parents.end(); ++i)
		{
			const SDXNodeRef parent(*i);
			ocean_assert(parent);

			Rendering::TextureRef texture(parent->renderingObject());
			if (texture)
			{
				texture->setMagnificationFilterMode(translateMagnification(magnificationFilter_.value()));
				texture->setMinificationFilterMode(translateMinification(minificationFilter_.value()));
				texture->setUseMipmaps(generateMipMaps_.value());
			}
		}
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}

	if (!anisotropicDegree_.timestamp().isInvalid())
	{
		Log::warning() << "TextureProperties::anisotropicDegree is not supported";
	}

	if (!borderColor_.timestamp().isInvalid())
	{
		Log::warning() << "TextureProperties::borderColor is not supported";
	}

	if (!borderWidth_.timestamp().isInvalid())
	{
		Log::warning() << "TextureProperties::borderWidth is not supported";
	}

	if (!boundaryModeS_.timestamp().isInvalid())
	{
		Log::warning() << "TextureProperties::boundaryModeS is not supported";
	}

	if (!boundaryModeT_.timestamp().isInvalid())
	{
		Log::warning() << "TextureProperties::boundaryModeT is not supported";
	}

	if (!boundaryModeR_.timestamp().isInvalid())
	{
		Log::warning() << "TextureProperties::boundaryModeR is not supported";
	}

	if (!textureCompression_.timestamp().isInvalid())
	{
		Log::warning() << "TextureProperties::textureCompression is not supported";
	}

	if (!texturePriority_.timestamp().isInvalid())
	{
		Log::warning() << "TextureProperties::texturePriority is not supported";
	}
}

void TextureProperties::onFieldChanged(const std::string& fieldName)
{
	try
	{
		if (fieldName == "magnificationFilter" || fieldName == "minificationFilter" || fieldName == "generateMipMaps")
		{
			const NodeRefs parents(parentNodes());

			for (NodeRefs::const_iterator i = parents.begin(); i != parents.end(); ++i)
			{
				const SDXNodeRef parent(*i);
				ocean_assert(parent);

				Rendering::TextureRef texture(parent->renderingObject());
				if (texture)
				{
					if (fieldName == "magnificationFilter")
					{
						texture->setMagnificationFilterMode(translateMagnification(magnificationFilter_.value()));
					}
					else if (fieldName == "minificationFilter")
					{
						texture->setMinificationFilterMode(translateMinification(minificationFilter_.value()));
					}
				}
			}
		}
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}

	if (fieldName == "anisotropicDegree" || fieldName == "borderColor" || fieldName == "borderWidth" || fieldName == "boundaryModeS"
			|| fieldName == "boundaryModeT" || fieldName == "boundaryModeR" || fieldName == "textureCompression" || fieldName == "texturePriority")
	{
		Log::warning() << "TextureProperties::" << fieldName << " is not supported";
	}

	X3DNode::onFieldChanged(fieldName);
}

size_t TextureProperties::objectAddress() const
{
	return size_t(this);
}

Rendering::Texture::MinFilterMode TextureProperties::translateMinification(const std::string parameter)
{
	Rendering::Texture::MinFilterMode minFilterMode(Rendering::Texture::MIN_MODE_LINEAR);

	if (parameter == "FASTEST" || parameter == "NEAREST_PIXEL")
	{
		minFilterMode = Rendering::Texture::MIN_MODE_NEAREST;
	}
	else if (parameter == "NEAREST_PIXEL_NEAREST_MIPMAP")
	{
		minFilterMode = Rendering::Texture::MIN_MODE_NEAREST_MIPMAP_NEAREST;
	}
	else if (parameter == "NEAREST_PIXEL_AVG_MIPMAP")
	{
		minFilterMode = Rendering::Texture::MIN_MODE_NEAREST_MIPMAP_LINEAR;
	}
	else if (parameter == "AVG_PIXEL_NEAREST_MIPMAP")
	{
		minFilterMode = Rendering::Texture::MIN_MODE_LINEAR_MIPMAP_NEAREST;
	}
	else if (parameter == "AVG_PIXEL_AVG_MIPMAP" || parameter == "NICEST")
	{
		minFilterMode = Rendering::Texture::MIN_MODE_LINEAR_MIPMAP_LINEAR;
	}

	return minFilterMode;
}

Rendering::Texture::MagFilterMode TextureProperties::translateMagnification(const std::string parameter)
{
	Rendering::Texture::MagFilterMode magFilterMode(Rendering::Texture::MAG_MODE_LINEAR);

	if (parameter == "FASTEST" || parameter == "NEAREST_PIXEL")
	{
		magFilterMode = Rendering::Texture::MAG_MODE_NEAREST;
	}

	return magFilterMode;
}

}

}

}

}
