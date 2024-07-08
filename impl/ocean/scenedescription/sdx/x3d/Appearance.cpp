/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/Appearance.h"

#include "ocean/rendering/AttributeSet.h"
#include "ocean/rendering/BlendAttribute.h"
#include "ocean/rendering/PhantomAttribute.h"
#include "ocean/rendering/Textures.h"
#include "ocean/rendering/Texture2D.h"
#include "ocean/rendering/ShaderProgram.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

Appearance::Appearance(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DAppearanceNode(environment),
	isPhantom_(false)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;

	renderingObject_ = engine()->factory().createAttributeSet();
}

Appearance::~Appearance()
{
	if (initialized_)
	{
		unregisterThisNodeAsParent(fillProperties_.value());
		unregisterThisNodeAsParent(lineProperties_.value());
		unregisterThisNodeAsParent(material_.value());

		for (MultiNode::Values::const_iterator i = shaders_.values().begin(); i != shaders_.values().end(); ++i)
		{
			unregisterThisNodeAsParent(*i);
		}

		unregisterThisNodeAsParent(texture_.value());
		unregisterThisNodeAsParent(textureTransform_.value());
	}
}

Appearance::NodeSpecification Appearance::specifyNode()
{
	NodeSpecification specification("Appearance");

	registerField(specification, "fillProperties", fillProperties_, FieldAccessType(ACCESS_GET_SET | ACCESS_EXPLICIT_NOTIFICATION));
	registerField(specification, "lineProperties", lineProperties_, FieldAccessType(ACCESS_GET_SET | ACCESS_EXPLICIT_NOTIFICATION));
	registerField(specification, "material", material_, FieldAccessType(ACCESS_GET_SET | ACCESS_EXPLICIT_NOTIFICATION));
	registerField(specification, "shaders", shaders_, FieldAccessType(ACCESS_GET_SET | ACCESS_EXPLICIT_NOTIFICATION));
	registerField(specification, "texture", texture_, FieldAccessType(ACCESS_GET_SET | ACCESS_EXPLICIT_NOTIFICATION));
	registerField(specification, "textureTransform", textureTransform_, FieldAccessType(ACCESS_GET_SET | ACCESS_EXPLICIT_NOTIFICATION));
	registerField(specification, "isPhantom", isPhantom_, ACCESS_NONE);

	X3DAppearanceNode::registerFields(specification);

	return specification;
}

void Appearance::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DAppearanceNode::onInitialize(scene, timestamp);

	try
	{
		Rendering::AttributeSetRef renderingAttributeSet(renderingObject_);
		if (renderingAttributeSet)
		{
			registerThisNodeAsParent(fillProperties_.value());
			registerThisNodeAsParent(lineProperties_.value());
			registerThisNodeAsParent(material_.value());

			for (MultiNode::Values::const_iterator i = shaders_.values().begin(); i != shaders_.values().end(); ++i)
			{
				registerThisNodeAsParent(*i);
			}

			registerThisNodeAsParent(texture_.value());
			registerThisNodeAsParent(textureTransform_.value());


			if (isPhantom_.value())
			{
				try
				{
					Rendering::PhantomAttributeRef renderingPhantomAttribute(engine()->factory().createPhantomAttribute());

					if (renderingPhantomAttribute)
					{
						renderingAttributeSet->addAttribute(renderingPhantomAttribute);
						return;
					}
				}
				catch(...)
				{
					Log::warning() << "The rendering engine does not support phantom objects";
				}
			}


			const SDXNodeRef materialNode(material_.value());
			if (materialNode)
			{
				materialNode->initialize(scene, timestamp);
				renderingAttributeSet->addAttribute(materialNode->renderingObject());
			}

			const SDXNodeRef textureNode(texture_.value());
			Rendering::TexturesRef renderingTextures;

			if (textureNode)
			{
				textureNode->initialize(scene, timestamp);

				renderingTextures = textureNode->renderingObject();

				if (renderingTextures.isNull())
				{
					ocean_assert(explicitRenderingTextures_.isNull());

					explicitRenderingTextures_ = engine()->factory().createTextures();
					explicitRenderingTextures_->addTexture(textureNode->renderingObject());
					renderingTextures = explicitRenderingTextures_;
				}

				if (renderingTextures->numberTextures() != 0)
				{
					renderingAttributeSet->addAttribute(renderingTextures);
				}
			}

			const SDXNodeRef textureTransformNode(textureTransform_.value());
			if (textureTransformNode)
			{
				textureTransformNode->initialize(scene, timestamp);
			}

			if (shaders_.values().empty() == false)
			{
				for (MultiNode::Values::const_iterator i = shaders_.values().begin(); i != shaders_.values().end(); ++i)
				{
					const SDXNodeRef shader(*i);
					ocean_assert(shader);

					shader->initialize(scene, timestamp);
					Rendering::ShaderProgramRef renderingShaderProgram(shader->renderingObject());

					if (renderingShaderProgram && renderingTextures)
					{
						for (unsigned int n = 0u; n < renderingTextures->numberTextures(); ++n)
						{
							renderingShaderProgram->setSampler(n, renderingTextures->texture(n));
						}
					}

					if (shader)
					{
						renderingAttributeSet->addAttribute(shader->renderingObject());
					}
				}
			}

			// adds a blend attribute to the rendering object if necessary
			checkTransparencyState();
		}
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}
}

bool Appearance::onFieldChanging(const std::string& fieldName, const Field& field)
{
	Rendering::AttributeSetRef renderingAttributeSet(renderingObject_);

	if (fieldName == "fillProperties" || fieldName == "lineProperties" || fieldName == "shaders" || fieldName == "textureTransform")
	{
		Log::warning() << "Appearance::" << fieldName << " is not supported";
		return false;
	}

	if (renderingAttributeSet.isNull() || !field.isType(Field::TYPE_NODE))
	{
		return false;
	}

	if (field.is0D())
	{
		const SingleNode& singleNode = dynamic_cast<const SingleNode&>(field);
		SDXNodeRef nodeRef(singleNode.value());

		if (fieldName == "material")
		{
			unregisterThisNodeAsParent(material_.value());

			SDXNodeRef materialNode(material_.value());
			if (materialNode)
			{
				renderingAttributeSet->removeAttribute(materialNode->renderingObject());
			}

			if (nodeRef)
			{
				renderingAttributeSet->addAttribute(nodeRef->renderingObject());
			}

			material_.setValue(singleNode.value());
			registerThisNodeAsParent(material_.value());

			return true;
		}

		if (fieldName == "texture")
		{
			unregisterThisNodeAsParent(texture_.value());

			if (explicitRenderingTextures_)
			{
				renderingAttributeSet->removeAttribute(explicitRenderingTextures_);
				explicitRenderingTextures_.release();
			}
			else
			{
				SDXNodeRef textureNode(texture_.value());
				if (textureNode)
				{
					renderingAttributeSet->removeAttribute(textureNode->renderingObject());
				}
			}

			ocean_assert(explicitRenderingTextures_.isNull());

			registerThisNodeAsParent(texture_.value());

			if (nodeRef)
			{
				Rendering::TexturesRef renderingTextures(nodeRef->renderingObject());

				if (renderingTextures.isNull())
				{
					ocean_assert(explicitRenderingTextures_.isNull());

					explicitRenderingTextures_ = engine()->factory().createTextures();
					explicitRenderingTextures_->addTexture(nodeRef->renderingObject());
					renderingTextures = explicitRenderingTextures_;
				}

				renderingAttributeSet->addAttribute(renderingTextures);
			}

			texture_.setValue(singleNode.value());

			return true;
		}
	}

	return false;
}

bool Appearance::checkTransparencyState()
{
	const Rendering::AttributeSetRef renderingAttributeSet(renderingObject_);

	if (renderingAttributeSet.isNull())
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	bool shouldContainBlendAttribute = false;

	const SDXNodeRef materialNode(material_.value());
	if (materialNode)
	{
		if (!shouldContainBlendAttribute && materialNode->hasField("transparency"))
		{
			ocean_assert(materialNode->field("transparency").isType(Field::TYPE_FLOAT, 0));
			shouldContainBlendAttribute = shouldContainBlendAttribute || materialNode->field<SingleFloat>("transparency").value() != 0;
		}

		if (!shouldContainBlendAttribute && materialNode->hasField("backTransparency"))
		{
			ocean_assert(materialNode->field("backTransparency").isType(Field::TYPE_FLOAT, 0));
			shouldContainBlendAttribute = shouldContainBlendAttribute || materialNode->field<SingleFloat>("BackTransparency").value() != 0;
		}
	}

	if (!shouldContainBlendAttribute)
	{
		const SDXNodeRef textureNode(texture_.value());
		if (textureNode)
		{
			Rendering::TexturesRef renderingTextures = textureNode->renderingObject();

			if (renderingTextures.isNull())
			{
				renderingTextures = explicitRenderingTextures_;
			}

			shouldContainBlendAttribute = shouldContainBlendAttribute || (renderingTextures && renderingTextures->hasAlphaTexture());
		}
	}

	if (shouldContainBlendAttribute)
	{
		if (!renderingAttributeSet->hasAttribute(Rendering::Object::TYPE_BLEND_ATTRIBUTE))
		{
			renderingAttributeSet->addAttribute(engine()->factory().createBlendAttribute());
		}
	}
	else
	{
		renderingAttributeSet->removeAttribute(Rendering::Object::TYPE_BLEND_ATTRIBUTE);
	}

	return shouldContainBlendAttribute;
}

size_t Appearance::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
