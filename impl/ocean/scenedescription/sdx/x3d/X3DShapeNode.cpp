/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/X3DShapeNode.h"

#include "ocean/rendering/Geometry.h"
#include "ocean/rendering/Renderable.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

X3DShapeNode::X3DShapeNode(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DBoundedObject(environment),
	X3DNode(environment),
	X3DChildNode(environment)
{
	// nothing to do here
}

X3DShapeNode::~X3DShapeNode()
{
	if (initialized_)
	{
		unregisterThisNodeAsParent(appearance_.value());
		unregisterThisNodeAsParent(geometry_.value());
	}
}

void X3DShapeNode::registerFields(NodeSpecification& specification)
{
	registerField(specification, "appearance", appearance_, FieldAccessType(ACCESS_GET_SET | ACCESS_EXPLICIT_NOTIFICATION));
	registerField(specification, "geometry", geometry_, FieldAccessType(ACCESS_GET_SET | ACCESS_EXPLICIT_NOTIFICATION));

	X3DBoundedObject::registerFields(specification);
	X3DChildNode::registerFields(specification);
}

void X3DShapeNode::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DBoundedObject::onInitialize(scene, timestamp);
	X3DChildNode::onInitialize(scene, timestamp);

	try
	{
		const Rendering::GeometryRef renderingGeometry(renderingObject_);

		if (renderingGeometry)
		{
			registerThisNodeAsParent(appearance_.value());
			registerThisNodeAsParent(geometry_.value());

			Rendering::AttributeSetRef attributeSet;

			const SDXNodeRef appearanceNode(appearance_.value());
			if (appearanceNode)
			{
				appearanceNode->initialize(scene, timestamp);
				attributeSet = appearanceNode->renderingObject();
			}
			else
			{
				attributeSet = engine()->factory().createAttributeSet();
			}

			const SDXNodeRef geometryNode(geometry_.value());
			if (geometryNode)
			{
				geometryNode->initialize(scene, timestamp);

				if (geometryNode->hasField("solid") && geometryNode->field<SingleBool>("solid").value() == false)
				{
					Rendering::PrimitiveAttributeRef primitiveAttribute;

					if (attributeSet)
					{
						primitiveAttribute = attributeSet->attribute(Rendering::Object::TYPE_PRIMITIVE_ATTRIBUTE);
					}

					if (primitiveAttribute.isNull())
					{
						primitiveAttribute = engine()->factory().createPrimitiveAttribute();

						if (primitiveAttribute)
						{
							primitiveAttribute->setCullingMode(Rendering::PrimitiveAttribute::CULLING_NONE);
							primitiveAttribute->setLightingMode(Rendering::PrimitiveAttribute::LM_TWO_SIDED_LIGHTING);

							if (attributeSet.isNull())
							{
								attributeSet = engine()->factory().createAttributeSet();
							}

							if (attributeSet)
							{
								attributeSet->addAttribute(primitiveAttribute);
							}
						}
					}
				}

				renderingGeometry->addRenderable(geometryNode->renderingObject(), attributeSet);
			}
		}
	}
	catch (const OceanException& exception)
	{
		Log::warning() << exception.what();
	}
}

bool X3DShapeNode::onFieldChanging(const std::string& fieldName, const Field& field)
{
	const Rendering::GeometryRef renderingGeometry(renderingObject_);

	if (renderingGeometry && field.isType(Field::TYPE_NODE, 0u))
	{
		const SingleNode& singleNode = dynamic_cast<const SingleNode&>(field);
		const SDXNodeRef nodeRef(singleNode.value());

		if (nodeRef)
		{
			if (fieldName == "appearance")
			{
				unregisterThisNodeAsParent(appearance_.value());

				if (renderingGeometry->numberRenderables() != 0u)
				{
					ocean_assert(renderingGeometry->numberRenderables() <= 1u);

					const Rendering::RenderableRef renderingRenderable(renderingGeometry->renderable(0u));
					renderingGeometry->removeRenderable(renderingRenderable);

					renderingGeometry->addRenderable(renderingRenderable, nodeRef->renderingObject());
				}

				appearance_.setValue(singleNode.value());
				registerThisNodeAsParent(appearance_.value());

				return true;
			}

			if (fieldName == "geometry")
			{
				unregisterThisNodeAsParent(geometry_.value());

				if (renderingGeometry->numberRenderables() != 0u)
				{
					ocean_assert(renderingGeometry->numberRenderables() <= 1u);

					const Rendering::AttributeSetRef renderingAttributeSet(renderingGeometry->attributeSet(0u));
					const Rendering::RenderableRef renderingRenderable(renderingGeometry->renderable(0u));

					renderingGeometry->removeRenderable(renderingRenderable);

					if (nodeRef->hasField("solid") && nodeRef->field<SingleBool>("solid").value() == false)
					{
						Rendering::PrimitiveAttributeRef primitiveAttribute = renderingAttributeSet->attribute(Rendering::Object::TYPE_PRIMITIVE_ATTRIBUTE);

						if (primitiveAttribute.isNull())
						{
							primitiveAttribute = engine()->factory().createPrimitiveAttribute();
						}

						if (primitiveAttribute)
						{
							primitiveAttribute->setCullingMode(Rendering::PrimitiveAttribute::CULLING_NONE);
							renderingAttributeSet->addAttribute(primitiveAttribute);
						}
					}

					renderingGeometry->addRenderable(nodeRef->renderingObject(), renderingAttributeSet);
				}

				geometry_.setValue(singleNode.value());
				registerThisNodeAsParent(geometry_.value());

				return true;
			}
		}
	}

	return false;
}

}

}

}

}
