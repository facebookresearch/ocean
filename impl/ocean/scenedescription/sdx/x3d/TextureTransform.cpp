/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/TextureTransform.h"

#include "ocean/rendering/AttributeSet.h"
#include "ocean/rendering/Texture.h"
#include "ocean/rendering/Textures.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

TextureTransform::TextureTransform(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DAppearanceChildNode(environment),
	X3DTextureTransformNode(environment),
	center_(Vector2(0, 0)),
	rotation_(0),
	scale_(Vector2(1, 1)),
	translation_(Vector2(0, 0))
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;
}

TextureTransform::NodeSpecification TextureTransform::specifyNode()
{
	NodeSpecification specification("TextureTransform");

	registerField(specification, "center", center_, ACCESS_GET_SET);
	registerField(specification, "rotation", rotation_, ACCESS_GET_SET);
	registerField(specification, "scale", scale_, ACCESS_GET_SET);
	registerField(specification, "translation", translation_, ACCESS_GET_SET);

	X3DTextureTransformNode::registerFields(specification);

	return specification;
}

void TextureTransform::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DTextureTransformNode::onInitialize(scene, timestamp);

	try
	{
		applyTextureTransform();
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}
}

void TextureTransform::onFieldChanged(const std::string& fieldName)
{
	try
	{
		if (fieldName == "center" || fieldName == "rotation" || fieldName == "scale" || fieldName == "translation")
		{
			applyTextureTransform();
		}
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}

	if (fieldName == "anisotropicDegree" || fieldName == "borderColor" || fieldName == "borderWidth" || fieldName == "boundaryModeS"
			|| fieldName == "boundaryModeT" || fieldName == "boundaryModeR" || fieldName == "textureCompression" || fieldName == "texturePriority")
		Log::warning() << "TextureProperties::" << fieldName << " is not supported";

	X3DNode::onFieldChanged(fieldName);
}

size_t TextureTransform::objectAddress() const
{
	return size_t(this);
}

void TextureTransform::applyTextureTransform()
{
	/**
	 * The following transformtion will be applied:
	 * t' = -C * S * R * C * T * p
	 * With: C(enter) vector, S(cale) vector, R(otation) vector and T(ranslation) vector
	 */

	const HomogenousMatrix4 nc(-Vector3(center_.value().x(), center_.value().y(), 0));
	const HomogenousMatrix4 c(Vector3(center_.value().x(), center_.value().y(), 0));
	const HomogenousMatrix4 s(Vector3(scale_.value().x(), 0, 0), Vector3(0, scale_.value().y(), 0), Vector3(0, 0, 1));
	const HomogenousMatrix4 r(Rotation(0, 0, 1, rotation_.value()));
	const HomogenousMatrix4 t(Vector3(translation_.value().x(), translation_.value().y(), 0));

	const HomogenousMatrix4 transformation(nc * s * r * c * t);

	const NodeRefs parents(parentNodes());

	for (NodeRefs::const_iterator i = parents.begin(); i != parents.end(); ++i)
	{
		const SDXNodeRef parent(*i);
		ocean_assert(parent);

		if (parent->type() == "Appearance")
		{
			const Rendering::AttributeSetRef renderingAttributeSet(parent->renderingObject());
			if (renderingAttributeSet)
			{
				const Rendering::TexturesRef renderingTextures = renderingAttributeSet->attribute(Rendering::Object::TYPE_TEXTURES);

				if (renderingTextures)
				{
					for (unsigned int n = 0u; n < renderingTextures->numberTextures(); ++n)
					{
						const Rendering::TextureRef renderingTexture(renderingTextures->texture(n));
						renderingTexture->setTransformation(transformation);
					}
				}
			}
		}
		else if (parent->type() == "MultiTextureTransform")
		{
			const MultiNode& textureTransform = parent->field<MultiNode>("textureTransform");

			for (size_t n = 0u; n < textureTransform.values().size(); ++n)
			{
				if (dynamic_cast<TextureTransform*>(&*textureTransform.values()[n]) == this)
				{
					const NodeRefs multiTextureTransformParents(parent->parentNodes());

					for (NodeRefs::const_iterator iM = multiTextureTransformParents.begin(); iM != multiTextureTransformParents.end(); ++iM)
					{
						const SDXNodeRef multiTextureParent(*iM);
						ocean_assert(multiTextureParent);

						if (multiTextureParent->type() == "Appearance")
						{
							const Rendering::AttributeSetRef renderingAttributeSet(multiTextureParent->renderingObject());
							if (renderingAttributeSet)
							{
								const Rendering::TexturesRef renderingTextures = renderingAttributeSet->attribute(Rendering::Object::TYPE_TEXTURES);

								if (renderingTextures)
								{
									const Rendering::TextureRef renderingTexture(renderingTextures->texture((unsigned int)(n)));

									if (renderingTexture)
									{
										renderingTexture->setTransformation(transformation);
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

}

}

}

}
