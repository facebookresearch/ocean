/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/Utilities.h"

#include "ocean/math/Rotation.h"

#include "ocean/rendering/BlendAttribute.h"
#include "ocean/rendering/Cone.h"
#include "ocean/rendering/Cylinder.h"
#include "ocean/rendering/FrameTexture2D.h"
#include "ocean/rendering/MediaTexture2D.h"
#include "ocean/rendering/Points.h"
#include "ocean/rendering/Textures.h"
#include "ocean/rendering/Triangles.h"
#include "ocean/rendering/VertexSet.h"

namespace Ocean
{

namespace Rendering
{

TransformRef Utilities::createBox(const EngineRef& engine, const Vector3& dimension, BoxRef* box, AttributeSetRef* attributeSet, GeometryRef* geometry)
{
	if (engine.isNull() || dimension.x() < 0 || dimension.y() < 0 || dimension.z() < 0)
	{
		return TransformRef();
	}

	try
	{
		const AttributeSetRef internalAttributeSet(engine->factory().createAttributeSet());

		const BoxRef internalBox(engine->factory().createBox());
		internalBox->setSize(dimension);

		const GeometryRef internalGeometry(engine->factory().createGeometry());
		internalGeometry->addRenderable(internalBox, internalAttributeSet);

		const TransformRef transform(engine->factory().createTransform());
		transform->addChild(internalGeometry);

		if (box)
		{
			*box = internalBox;
		}

		if (attributeSet)
		{
			*attributeSet = internalAttributeSet;
		}

		if (geometry)
		{
			*geometry = internalGeometry;
		}

		return transform;
	}
	catch (...)
	{
		// nothing to do here
	}

	return TransformRef();
}

TransformRef Utilities::createBox(const EngineRef& engine, const Vector3& dimension, const RGBAColor& color, BoxRef* box, AttributeSetRef* attributeSet, MaterialRef* material, GeometryRef* geometry)
{
	if (engine.isNull() || dimension.x() < 0 || dimension.y() < 0 || dimension.z() < 0)
	{
		return TransformRef();
	}

	try
	{
		const MaterialRef internalMaterial(engine->factory().createMaterial());
		internalMaterial->setDiffuseColor(color);
		internalMaterial->setTransparency(1.0f - color.alpha());

		const AttributeSetRef internalAttributeSet(engine->factory().createAttributeSet());
		internalAttributeSet->addAttribute(internalMaterial);

		if (internalMaterial->transparency() != 0)
		{
			internalAttributeSet->addAttribute(engine->factory().createBlendAttribute());
		}

		const BoxRef internalBox(engine->factory().createBox());
		internalBox->setSize(dimension);

		const GeometryRef internalGeometry(engine->factory().createGeometry());
		internalGeometry->addRenderable(internalBox, internalAttributeSet);

		const TransformRef transform(engine->factory().createTransform());
		transform->addChild(internalGeometry);

		if (box)
		{
			*box = internalBox;
		}

		if (attributeSet)
		{
			*attributeSet = internalAttributeSet;
		}

		if (material)
		{
			*material = internalMaterial;
		}

		if (geometry)
		{
			*geometry = internalGeometry;
		}

		return transform;
	}
	catch (...)
	{
		// nothing to do here
	}

	return TransformRef();
}

TransformRef Utilities::createBox(const EngineRef& engine, const Vector3& dimension, const Media::FrameMediumRef textureMedium, MediaTexture2DRef* texture, const bool createMipmaps, AttributeSetRef* attributeSet, GeometryRef* geometry, const RGBAColor* color)
{
	if (engine.isNull() || dimension.isNull())
	{
		return TransformRef();
	}

	try
	{
		const Rendering::BoxRef box(engine->factory().createBox());
		box->setSize(dimension);

		const Rendering::AttributeSetRef internalAttributeSet(engine->factory().createAttributeSet());

		const Rendering::MediaTexture2DRef internalTexture = engine->factory().createMediaTexture2D();
		internalTexture->setMedium(textureMedium);

		const Rendering::TexturesRef textures = engine->factory().createTextures();
		textures->addTexture(internalTexture);

		internalAttributeSet->addAttribute(textures);

		if (color != nullptr)
		{
			const MaterialRef internalMaterial(engine->factory().createMaterial());
			internalMaterial->setDiffuseColor(*color);
			internalMaterial->setTransparency(1.0f - color->alpha());

			internalAttributeSet->addAttribute(internalMaterial);

			if (internalMaterial->transparency() != 0)
			{
				internalAttributeSet->addAttribute(engine->factory().createBlendAttribute());
			}
		}

		const Rendering::GeometryRef internalGeometry(engine->factory().createGeometry());
		internalGeometry->addRenderable(box, internalAttributeSet);

		Rendering::TransformRef transform = engine->factory().createTransform();
		transform->addChild(internalGeometry);

		internalTexture->setMagnificationFilterMode(Texture::MAG_MODE_LINEAR);

		if (createMipmaps)
		{
			internalTexture->setMinificationFilterMode(Texture::MIN_MODE_LINEAR_MIPMAP_LINEAR);
		}

		internalTexture->setUseMipmaps(createMipmaps);

		if (texture)
		{
			*texture = internalTexture;
		}

		if (attributeSet)
		{
			*attributeSet = internalAttributeSet;
		}

		if (geometry)
		{
			*geometry = internalGeometry;
		}

		return transform;
	}
	catch (...)
	{
		// nothing to do here
	}

	return TransformRef();
}

TransformRef Utilities::createBox(const EngineRef& engine, const Vector3& dimension, Frame&& frame, FrameTexture2DRef* texture, const bool createMipmaps, AttributeSetRef* attributeSet, GeometryRef* geometry, const RGBAColor* color)
{
	if (engine.isNull() || dimension.isNull())
	{
		ocean_assert(false && "Invalid input!");
		return TransformRef();
	}

	if (!frame.isValid() || !frame.isOwner())
	{
		ocean_assert(false && "Frame must be valid and must own the memory");
		return TransformRef();
	}

	try
	{
		const Rendering::BoxRef box(engine->factory().createBox());
		box->setSize(dimension);

		const Rendering::AttributeSetRef internalAttributeSet(engine->factory().createAttributeSet());

		const Rendering::FrameTexture2DRef internalTexture = engine->factory().createFrameTexture2D();
		internalTexture->setTexture(std::move(frame));

		const Rendering::TexturesRef textures = engine->factory().createTextures();
		textures->addTexture(internalTexture);

		internalAttributeSet->addAttribute(textures);

		if (color != nullptr)
		{
			const MaterialRef internalMaterial(engine->factory().createMaterial());
			internalMaterial->setDiffuseColor(*color);
			internalMaterial->setTransparency(1.0f - color->alpha());

			internalAttributeSet->addAttribute(internalMaterial);

			if (internalMaterial->transparency() != 0)
			{
				internalAttributeSet->addAttribute(engine->factory().createBlendAttribute());
			}
		}

		const Rendering::GeometryRef internalGeometry(engine->factory().createGeometry());
		internalGeometry->addRenderable(box, internalAttributeSet);

		Rendering::TransformRef transform = engine->factory().createTransform();
		transform->addChild(internalGeometry);

		internalTexture->setMagnificationFilterMode(Texture::MAG_MODE_LINEAR);

		if (createMipmaps)
		{
			internalTexture->setMinificationFilterMode(Texture::MIN_MODE_LINEAR_MIPMAP_LINEAR);
		}

		internalTexture->setUseMipmaps(createMipmaps);

		if (texture)
		{
			*texture = internalTexture;
		}

		if (attributeSet)
		{
			*attributeSet = internalAttributeSet;
		}

		if (geometry)
		{
			*geometry = internalGeometry;
		}

		return transform;
	}
	catch (...)
	{
		// nothing to do here
	}

	return TransformRef();
}

TransformRef Utilities::createSphere(const EngineRef& engine, const Scalar radius, SphereRef* sphere, AttributeSetRef* attributeSet, GeometryRef* geometry)
{
	if (engine.isNull() || radius < 0)
	{
		return TransformRef();
	}

	try
	{
		const AttributeSetRef internalAttributeSet(engine->factory().createAttributeSet());

		const SphereRef internalSphere(engine->factory().createSphere());
		internalSphere->setRadius(radius);

		const GeometryRef internalGeometry(engine->factory().createGeometry());
		internalGeometry->addRenderable(internalSphere, internalAttributeSet);

		const TransformRef transform(engine->factory().createTransform());
		transform->addChild(internalGeometry);

		if (sphere)
		{
			*sphere = internalSphere;
		}

		if (attributeSet)
		{
			*attributeSet = internalAttributeSet;
		}

		if (geometry)
		{
			*geometry = internalGeometry;
		}

		return transform;
	}
	catch (...)
	{
		// nothing to do here
	}

	return TransformRef();
}

TransformRef Utilities::createSphere(const EngineRef& engine, const Scalar radius, const RGBAColor& color, SphereRef* sphere, AttributeSetRef* attributeSet, MaterialRef* material, GeometryRef* geometry)
{
	if (engine.isNull() || radius < 0)
	{
		return TransformRef();
	}

	try
	{
		const MaterialRef internalMaterial(engine->factory().createMaterial());
		internalMaterial->setDiffuseColor(color);
		internalMaterial->setTransparency(1.0f - color.alpha());

		const AttributeSetRef internalAttributeSet(engine->factory().createAttributeSet());
		internalAttributeSet->addAttribute(internalMaterial);

		if (internalMaterial->transparency() != 0)
		{
			internalAttributeSet->addAttribute(engine->factory().createBlendAttribute());
		}

		const SphereRef internalSphere(engine->factory().createSphere());
		internalSphere->setRadius(radius);

		const GeometryRef internalGeometry(engine->factory().createGeometry());
		internalGeometry->addRenderable(internalSphere, internalAttributeSet);

		const TransformRef transform(engine->factory().createTransform());
		transform->addChild(internalGeometry);

		if (sphere)
		{
			*sphere = internalSphere;
		}

		if (attributeSet)
		{
			*attributeSet = internalAttributeSet;
		}

		if (material)
		{
			*material = internalMaterial;
		}

		if (geometry)
		{
			*geometry = internalGeometry;
		}

		return transform;
	}
	catch(...)
	{
		// nothing to do here
	}

	return TransformRef();
}

TransformRef Utilities::createSphere(const EngineRef& engine, const Scalar radius, Frame&& frame, FrameTexture2DRef* texture, const bool createMipmaps, AttributeSetRef* attributeSet, GeometryRef* geometry, const RGBAColor* color)
{
	if (engine.isNull() || radius < 0)
	{
		ocean_assert(false && "Invalid input!");
		return TransformRef();
	}

	if (!frame.isValid() || !frame.isOwner())
	{
		ocean_assert(false && "Frame must be valid and must own the memory");
		return TransformRef();
	}

	try
	{
		const Rendering::SphereRef sphere(engine->factory().createSphere());
		sphere->setRadius(radius);

		const Rendering::AttributeSetRef internalAttributeSet(engine->factory().createAttributeSet());

		const Rendering::FrameTexture2DRef internalTexture = engine->factory().createFrameTexture2D();
		internalTexture->setTexture(std::move(frame));

		const Rendering::TexturesRef textures = engine->factory().createTextures();
		textures->addTexture(internalTexture);

		internalAttributeSet->addAttribute(textures);

		if (color != nullptr)
		{
			const MaterialRef internalMaterial(engine->factory().createMaterial());
			internalMaterial->setDiffuseColor(*color);
			internalMaterial->setTransparency(1.0f - color->alpha());

			internalAttributeSet->addAttribute(internalMaterial);

			if (internalMaterial->transparency() != 0)
			{
				internalAttributeSet->addAttribute(engine->factory().createBlendAttribute());
			}
		}

		const Rendering::GeometryRef internalGeometry(engine->factory().createGeometry());
		internalGeometry->addRenderable(sphere, internalAttributeSet);

		Rendering::TransformRef transform = engine->factory().createTransform();
		transform->addChild(internalGeometry);

		internalTexture->setMagnificationFilterMode(Texture::MAG_MODE_LINEAR);

		if (createMipmaps)
		{
			internalTexture->setMinificationFilterMode(Texture::MIN_MODE_LINEAR_MIPMAP_LINEAR);
		}

		internalTexture->setUseMipmaps(createMipmaps);

		if (texture)
		{
			*texture = internalTexture;
		}

		if (attributeSet)
		{
			*attributeSet = internalAttributeSet;
		}

		if (geometry)
		{
			*geometry = internalGeometry;
		}

		return transform;
	}
	catch (...)
	{
		// nothing to do here
	}

	return TransformRef();
}

TransformRef Utilities::createCylinder(const EngineRef& engine, const Scalar radius, const Scalar height, const RGBAColor& color, CylinderRef* cylinder, AttributeSetRef* attributeSet, MaterialRef* material, GeometryRef* geometry)
{
	if (engine.isNull() || radius <= Numeric::eps() || height <= Numeric::eps())
	{
		return TransformRef();
	}

	try
	{
		const MaterialRef internalMaterial(engine->factory().createMaterial());
		internalMaterial->setDiffuseColor(color);
		internalMaterial->setTransparency(1.0f - color.alpha());

		const AttributeSetRef internalAttributeSet(engine->factory().createAttributeSet());
		internalAttributeSet->addAttribute(internalMaterial);

		if (internalMaterial->transparency() != 0)
		{
			internalAttributeSet->addAttribute(engine->factory().createBlendAttribute());
		}

		const CylinderRef internalCylinder(engine->factory().createCylinder());
		internalCylinder->setRadius(radius);
		internalCylinder->setHeight(height);

		const GeometryRef internalGeometry(engine->factory().createGeometry());
		internalGeometry->addRenderable(internalCylinder, internalAttributeSet);

		const TransformRef transform(engine->factory().createTransform());
		transform->addChild(internalGeometry);

		if (cylinder)
		{
			*cylinder = internalCylinder;
		}

		if (attributeSet)
		{
			*attributeSet = internalAttributeSet;
		}

		if (material)
		{
			*material = internalMaterial;
		}

		if (geometry)
		{
			*geometry = internalGeometry;
		}

		return transform;
	}
	catch(...)
	{
		// nothing to do here
	}

	return TransformRef();
}

TransformRef Utilities::createCone(const EngineRef& engine, const Scalar radius, const Scalar height, const RGBAColor& color, ConeRef* cone, AttributeSetRef* attributeSet, MaterialRef* material, GeometryRef* geometry)
{
	if (engine.isNull() || radius <= Numeric::eps() || height <= Numeric::eps())
	{
		return TransformRef();
	}

	try
	{
		const MaterialRef internalMaterial(engine->factory().createMaterial());
		internalMaterial->setDiffuseColor(color);
		internalMaterial->setTransparency(1.0f - color.alpha());

		const AttributeSetRef internalAttributeSet(engine->factory().createAttributeSet());
		internalAttributeSet->addAttribute(internalMaterial);

		if (internalMaterial->transparency() != 0)
		{
			internalAttributeSet->addAttribute(engine->factory().createBlendAttribute());
		}

		const ConeRef internalCone(engine->factory().createCone());
		internalCone->setRadius(radius);
		internalCone->setHeight(height);

		const GeometryRef internalGeometry(engine->factory().createGeometry());
		internalGeometry->addRenderable(internalCone, internalAttributeSet);

		const TransformRef transform(engine->factory().createTransform());
		transform->addChild(internalGeometry);

		if (cone)
		{
			*cone = internalCone;
		}

		if (attributeSet)
		{
			*attributeSet = internalAttributeSet;
		}

		if (material)
		{
			*material = internalMaterial;
		}

		if (geometry)
		{
			*geometry = internalGeometry;
		}

		return transform;
	}
	catch(...)
	{
		// nothing to do here
	}

	return TransformRef();
}

TransformRef Utilities::createCoordinateSystem(const EngineRef& engine, const Scalar length, const Scalar topLength, const Scalar radius)
{
	ocean_assert(engine);
	ocean_assert(length > 0 && topLength > 0 && radius > 0);

	if (engine.isNull())
	{
		return TransformRef();
	}

	try
	{
		// x-axis
		const TransformRef xArrow(createArrow(engine, length, topLength, radius, RGBAColor(1, 0, 0)));
		xArrow->setTransformation(HomogenousMatrix4(Rotation(0, 0, 1, -Numeric::pi_2())));

		// y-axis
		const TransformRef yArrow(createArrow(engine, length, topLength, radius, RGBAColor(0, 1, 0)));

		// z-axis
		const TransformRef zArrow(createArrow(engine, length, topLength, radius, RGBAColor(0, 0, 1)));
		zArrow->setTransformation(HomogenousMatrix4(Rotation(1, 0, 0, Numeric::pi_2())));

		const TransformRef coordinateSystem(engine->factory().createTransform());
		coordinateSystem->addChild(xArrow);
		coordinateSystem->addChild(yArrow);
		coordinateSystem->addChild(zArrow);

		return coordinateSystem;
	}
	catch(...)
	{
		// nothing to do here
	}

	return TransformRef();
}

TransformRef Utilities::createCoordinateSystems(const Engine& engine, const HomogenousMatrices4& world_T_coordinateSystems, const Scalar length, VertexSetRef* vertexSet)
{
	ocean_assert(length > 0);
	ocean_assert(!world_T_coordinateSystems.empty());

	try
	{
		Vectors3 vertices;
		vertices.reserve(world_T_coordinateSystems.size() * 6);

		RGBAColors colorsPerVertex;
		colorsPerVertex.reserve(world_T_coordinateSystems.size() * 6);

		for (const HomogenousMatrix4& world_T_coordinateSystem : world_T_coordinateSystems)
		{
			const Vector3 translation = world_T_coordinateSystem.translation();

			vertices.emplace_back(translation);
			vertices.emplace_back(world_T_coordinateSystem * Vector3(length, 0, 0));

			colorsPerVertex.emplace_back(1.0f, 0.0f, 0.0f);
			colorsPerVertex.emplace_back(1.0f, 0.0f, 0.0f);

			vertices.emplace_back(translation);
			vertices.emplace_back(world_T_coordinateSystem * Vector3(0, length, 0));

			colorsPerVertex.emplace_back(0.0f, 1.0f, 0.0f);
			colorsPerVertex.emplace_back(0.0f, 1.0f, 0.0f);

			vertices.emplace_back(translation);
			vertices.emplace_back(world_T_coordinateSystem * Vector3(0, 0, length));

			colorsPerVertex.emplace_back(0.0f, 0.0f, 1.0f);
			colorsPerVertex.emplace_back(0.0f, 0.0f, 1.0f);
		}

		VertexSetRef internalVertexSet = engine.factory().createVertexSet();
		internalVertexSet->setVertices(vertices);
		internalVertexSet->setColors(colorsPerVertex);

		const LinesRef internalLines = engine.factory().createLines();
		internalLines->setVertexSet(internalVertexSet);
		internalLines->setIndices((unsigned int)(vertices.size()));

		const GeometryRef internalGeometry = engine.factory().createGeometry();
		internalGeometry->addRenderable(internalLines, engine.factory().createAttributeSet());

		const TransformRef transform = engine.factory().createTransform();
		transform->addChild(internalGeometry);

		if (vertexSet != nullptr)
		{
			*vertexSet = std::move(internalVertexSet);
		}

		return transform;
	}
	catch (...)
	{
		// nothing to do here
	}

	return TransformRef();
}

TransformRef Utilities::createArrow(const EngineRef& engine, const Scalar length, const Scalar topLength, const Scalar radius, const RGBAColor& color)
{
	if (engine.isNull() || length < 0 || topLength < 0 || radius < 0)
	{
		return TransformRef();
	}

	try
	{
		const MaterialRef material(engine->factory().createMaterial());
		material->setDiffuseColor(color);
		material->setTransparency(1.0f - color.alpha());

		const AttributeSetRef attributeSet(engine->factory().createAttributeSet());
		attributeSet->addAttribute(material);

		if (material->transparency() != 0)
		{
			attributeSet->addAttribute(engine->factory().createBlendAttribute());
		}

		// axis
		const CylinderRef cylinderAxis(engine->factory().createCylinder());
		cylinderAxis->setRadius(radius);
		cylinderAxis->setHeight(length);

		const GeometryRef geometryAxis(engine->factory().createGeometry());
		geometryAxis->addRenderable(cylinderAxis, attributeSet);

		const TransformRef transformAxis(engine->factory().createTransform());
		transformAxis->addChild(geometryAxis);
		transformAxis->setTransformation(HomogenousMatrix4(Vector3(0, length * Scalar(0.5), 0)));


		// top
		const ConeRef cylinderTop(engine->factory().createCone());
		cylinderTop->setHeight(topLength);
		cylinderTop->setRadius(2 * radius);

		const GeometryRef geometryTop(engine->factory().createGeometry());
		geometryTop->addRenderable(cylinderTop, attributeSet);

		const TransformRef transformTop(engine->factory().createTransform());
		transformTop->addChild(geometryTop);
		transformTop->setTransformation(HomogenousMatrix4(Vector3(0, length + topLength * Scalar(0.5), 0)));


		const TransformRef arrow(engine->factory().createTransform());
		arrow->addChild(transformAxis);
		arrow->addChild(transformTop);

		return arrow;
	}
	catch(...)
	{
		// nothing to do here
	}

	return TransformRef();
}

TransformRef Utilities::createMesh(const EngineRef& engine, const Vectors3& vertices, const TriangleFaces& triangleFaces, const RGBAColor& color, const Normals& normals, const TextureCoordinates& textureCoordinates, Frame&& textureFrame, const Media::FrameMediumRef& textureMedium, AttributeSetRef* attributeSet, GeometryRef* geometry, VertexSetRef* vertexSet, Texture2DRef* texture)
{
	if (engine.isNull() || vertices.empty() || triangleFaces.empty())
	{
		return TransformRef();
	}

	ocean_assert(textureCoordinates.empty() || vertices.size() == textureCoordinates.size());

	try
	{
		const VertexSetRef internalVertexSet = engine->factory().createVertexSet();
		internalVertexSet->setVertices(vertices);

		if (!textureCoordinates.empty())
		{
			internalVertexSet->setTextureCoordinates(textureCoordinates, 0);
		}

		if (!normals.empty())
		{
			internalVertexSet->setNormals(normals);
		}

		const TrianglesRef triangles = engine->factory().createTriangles();
		triangles->setVertexSet(internalVertexSet);
		triangles->setFaces(triangleFaces);

		const GeometryRef internalGeometry = engine->factory().createGeometry();

		const AttributeSetRef internalAttributeSet = engine->factory().createAttributeSet();

		Texture2DRef internalTexture;

		ocean_assert(!textureFrame.isValid() || !textureMedium);

		if (textureFrame.isValid())
		{
			const FrameTexture2DRef frameTexture = engine->factory().createFrameTexture2D();
			frameTexture->setTexture(std::move(textureFrame));

			internalTexture = frameTexture;
		}
		else if (textureMedium)
		{
			const MediaTexture2DRef mediaTexture = engine->factory().createMediaTexture2D();
			mediaTexture->setMedium(textureMedium);

			internalTexture = mediaTexture;
		}

		if (internalTexture)
		{
			const TexturesRef textures = engine->factory().createTextures();
			textures->addTexture(internalTexture);

			internalAttributeSet->addAttribute(textures);
		}

		if (color.isValid())
		{
			const MaterialRef material = engine->factory().createMaterial();
			material->setDiffuseColor(color);
			material->setTransparency(1.0f - color.alpha());

			if (material->transparency() != 0.0f)
			{
				internalAttributeSet->addAttribute(engine->factory().createBlendAttribute());
			}

			internalAttributeSet->addAttribute(material);
		}

		internalGeometry->addRenderable(triangles, internalAttributeSet);

		const TransformRef transform = engine->factory().createTransform();
		transform->addChild(internalGeometry);

		if (attributeSet != nullptr)
		{
			*attributeSet = std::move(internalAttributeSet);
		}

		if (geometry != nullptr)
		{
			*geometry = std::move(internalGeometry);
		}

		if (vertexSet != nullptr)
		{
			*vertexSet = std::move(internalVertexSet);
		}

		if (texture != nullptr)
		{
			*texture = std::move(internalTexture);
		}

		return transform;
	}
	catch (...)
	{
		// nothing to do here
	}

	return TransformRef();
}

TransformRef Utilities::createTexturedPlane(const EngineRef& engine, const Scalar width, const Scalar height, const Media::FrameMediumRef textureMedium, MediaTexture2DRef* texture, const bool createMipmaps, AttributeSetRef* attributeSet, GeometryRef* geometry)
{
	return createBox(engine, Vector3(width, height, Scalar(0.0001)), textureMedium, texture, createMipmaps, attributeSet, geometry);
}

TransformRef Utilities::createText(const Engine& engine, const std::string& textString, const RGBAColor& foregroundColor, const RGBAColor& backgroundColor, const bool shaded, const Scalar fixedWidth, const Scalar fixedHeight, const Scalar fixedLineHeight, const Text::AlignmentMode alignmentMode, const Text::HorizontalAnchor horizontalAnchor, const Text::VerticalAnchor verticalAnchor, const std::string& fontFamily, const std::string& styleName, TextRef* text, MaterialRef* foregroundMaterial, MaterialRef* backgroundMaterial)
{
	try
	{
		const Rendering::AttributeSetRef attributeSet = engine.factory().createAttributeSet();

		const MaterialRef internForegroundMaterial = engine.factory().createMaterial();
		const MaterialRef internBackgroundMaterial = engine.factory().createMaterial();

		if (shaded)
		{
			internForegroundMaterial->setDiffuseColor(foregroundColor);
			internBackgroundMaterial->setDiffuseColor(backgroundColor);
		}
		else
		{
			internForegroundMaterial->setDiffuseColor(RGBAColor(0, 0, 0));
			internForegroundMaterial->setEmissiveColor(foregroundColor);

			internBackgroundMaterial->setDiffuseColor(RGBAColor(0, 0, 0));
			internBackgroundMaterial->setEmissiveColor(backgroundColor);
		}

		internForegroundMaterial->setTransparency(1.0f - foregroundColor.alpha());
		internBackgroundMaterial->setTransparency(1.0f - backgroundColor.alpha());

		if (foregroundColor.isOpaque() && backgroundColor.isOpaque())
		{
			// fully opaque, nothing to do here
		}
		else if (backgroundColor.alpha() == 0.0f)
		{
			// fully transparent

			const BlendAttributeRef blendAttribute = engine.factory().createBlendAttribute();

			blendAttribute->setSourceFunction(BlendAttribute::BlendingFunction::FUNCTION_ONE);
			blendAttribute->setDestinationFunction(BlendAttribute::BlendingFunction::FUNCTION_ONE_MINUS_SOURCE_ALPHA);

			attributeSet->addAttribute(blendAttribute);
		}
		else
		{
			// translucent

			const BlendAttributeRef blendAttribute = engine.factory().createBlendAttribute();

			blendAttribute->setSourceFunction(BlendAttribute::BlendingFunction::FUNCTION_SOURCE_ALPHA);
			blendAttribute->setDestinationFunction(BlendAttribute::BlendingFunction::FUNCTION_ONE_MINUS_SOURCE_ALPHA); // translucent with standard blending

			attributeSet->addAttribute(blendAttribute);
		}

		attributeSet->addAttribute(internForegroundMaterial);

		const TextRef internalText = engine.factory().createText();

		internalText->setText(textString);

		if (fontFamily.empty())
		{
			std::string internalStyleName;
			const std::string defaultFont = internalText->availableDefaultFont(&internalStyleName);

			internalText->setFont(defaultFont, styleName.empty() ? internalStyleName : styleName);
		}
		else
		{
			internalText->setFont(fontFamily, styleName);
		}

		internalText->setBackgroundMaterial(internBackgroundMaterial);

		internalText->setSize(fixedWidth, fixedHeight, fixedLineHeight);

		internalText->setAlignmentMode(alignmentMode);
		internalText->setHorizontalAnchor(horizontalAnchor);
		internalText->setVerticalAnchor(verticalAnchor);

		const GeometryRef geometry = engine.factory().createGeometry();
		geometry->addRenderable(internalText, attributeSet);

		TransformRef transform = engine.factory().createTransform();
		transform->addChild(geometry);

		if (text != nullptr)
		{
			*text = internalText;
		}

		if (foregroundMaterial != nullptr)
		{
			*foregroundMaterial = std::move(internForegroundMaterial);
		}

		if (backgroundMaterial != nullptr)
		{
			*backgroundMaterial = std::move(internBackgroundMaterial);
		}

		return transform;
	}
	catch (...)
	{
		return TransformRef();
	}
}

TransformRef Utilities::createPoints(const Engine& engine, const Vectors3& vertices, const RGBAColor& emissiveColor, const Scalar pointSize, const RGBAColors& colorsPerVertex, MaterialRef* material, AttributeSetRef* attributeSet, PointsRef* points, VertexSetRef* vertexSet)
{
	ocean_assert(!vertices.empty());
	ocean_assert(emissiveColor.isValid() || colorsPerVertex.size() == vertices.size());
	ocean_assert(pointSize >= 1);

	try
	{
		const VertexSetRef internalVertexSet = engine.factory().createVertexSet();
		internalVertexSet->setVertices(vertices);

		if (!colorsPerVertex.empty())
		{
			if (vertices.size() == colorsPerVertex.size())
			{
				internalVertexSet->setColors(colorsPerVertex);
			}
			else
			{
				Log::error() << "Invalid number of per-vertex colors";
			}
		}

		const PointsRef internalPoints = engine.factory().createPoints();
		internalPoints->setVertexSet(internalVertexSet);
		internalPoints->setIndices((unsigned int)(vertices.size()));
		internalPoints->setPointSize(pointSize);

		const GeometryRef internalGeometry = engine.factory().createGeometry();

		const AttributeSetRef internalAttributeSet = engine.factory().createAttributeSet();

		if (colorsPerVertex.empty())
		{
			const MaterialRef internalMaterial = engine.factory().createMaterial();
			internalMaterial->setDiffuseColor(RGBAColor(0, 0, 0));
			internalMaterial->setEmissiveColor(emissiveColor);
			internalMaterial->setTransparency(1.0f - emissiveColor.alpha());

			if (internalMaterial->transparency() != 0.0f)
			{
				internalAttributeSet->addAttribute(engine.factory().createBlendAttribute());
			}

			internalAttributeSet->addAttribute(internalMaterial);

			if (material != nullptr)
			{
				*material = internalMaterial;
			}
		}

		internalGeometry->addRenderable(internalPoints, internalAttributeSet);

		const TransformRef transform = engine.factory().createTransform();
		transform->addChild(internalGeometry);

		if (attributeSet != nullptr)
		{
			*attributeSet = internalAttributeSet;
		}

		if (points != nullptr)
		{
			*points = internalPoints;
		}

		if (vertexSet != nullptr)
		{
			*vertexSet = internalVertexSet;
		}

		return transform;
	}
	catch (...)
	{
		// nothing to do here
	}

	return TransformRef();
}

TransformRef Utilities::createLines(const Engine& engine, const Vectors3& vertices, const VertexIndexGroups& lineIndexGroups, const RGBAColor& emissiveColor, const RGBAColors& colorsPerVertex, MaterialRef* material, AttributeSetRef* attributeSet, LineStripsRef* lineStrips, VertexSetRef* vertexSet)
{
	try
	{
		ocean_assert(!vertices.empty());
		ocean_assert(!lineIndexGroups.empty());
		ocean_assert(emissiveColor.isValid() || colorsPerVertex.size() == vertices.size());

		const VertexSetRef internalVertexSet = engine.factory().createVertexSet();
		internalVertexSet->setVertices(vertices);

		if (!colorsPerVertex.empty())
		{
			internalVertexSet->setColors(colorsPerVertex);
		}

		const LineStripsRef internalLineStrips = engine.factory().createLineStrips();
		internalLineStrips->setVertexSet(internalVertexSet);
		internalLineStrips->setStrips(lineIndexGroups);

		const GeometryRef internalGeometry = engine.factory().createGeometry();

		const AttributeSetRef internalAttributeSet = engine.factory().createAttributeSet();

		if (colorsPerVertex.empty())
		{
			const MaterialRef internalMaterial = engine.factory().createMaterial();
			internalMaterial->setDiffuseColor(RGBAColor(0, 0, 0));
			internalMaterial->setEmissiveColor(emissiveColor);
			internalMaterial->setTransparency(1.0f - emissiveColor.alpha());

			if (internalMaterial->transparency() != 0.0f)
			{
				internalAttributeSet->addAttribute(engine.factory().createBlendAttribute());
			}

			internalAttributeSet->addAttribute(internalMaterial);

			if (material != nullptr)
			{
				*material = internalMaterial;
			}
		}

		internalGeometry->addRenderable(internalLineStrips, internalAttributeSet);

		const TransformRef transform = engine.factory().createTransform();
		transform->addChild(internalGeometry);

		if (attributeSet != nullptr)
		{
			*attributeSet = internalAttributeSet;
		}

		if (lineStrips != nullptr)
		{
			*lineStrips = internalLineStrips;
		}

		if (vertexSet != nullptr)
		{
			*vertexSet = internalVertexSet;
		}

		return transform;
	}
	catch (...)
	{
		// nothing to do here
	}

	return TransformRef();
}

TransformRef Utilities::createLines(const Engine& engine, const Vectors3& vertices, const TriangleFaces& triangleFaces, const RGBAColor& emissiveColor, const RGBAColors& colorsPerVertex, MaterialRef* material, AttributeSetRef* attributeSet, LinesRef* lines, VertexSetRef* vertexSet)
{
	try
	{
		ocean_assert(!vertices.empty());
		ocean_assert(!triangleFaces.empty());
		ocean_assert(emissiveColor.isValid() || colorsPerVertex.size() == vertices.size());

		const VertexSetRef internalVertexSet = engine.factory().createVertexSet();

		Vectors3 lineVertices;
		lineVertices.reserve(triangleFaces.size() * 2);

		RGBAColors lineColors;

		UnorderedIndexSet64 existingEdges;
		existingEdges.reserve(triangleFaces.size());

		if (colorsPerVertex.empty())
		{
			for (const TriangleFace& triangleFace : triangleFaces)
			{
				if (triangleFace[0] < vertices.size() && triangleFace[1] < vertices.size() && triangleFace[2] < vertices.size())
				{
					for (unsigned int n = 0u; n < 3u; ++n)
					{
						const Index32 indexA = triangleFace[n];
						const Index32 indexB = triangleFace[n < 2u ? n + 1u : 0u];

						if (existingEdges.emplace(makeSortedIndex64(indexA, indexB)).second)
						{
							lineVertices.emplace_back(vertices[indexA]);
							lineVertices.emplace_back(vertices[indexB]);
						}
					}
				}
				else
				{
					Log::error() << "Invalid vertex index";
				}
			}
		}
		else
		{
			lineColors.reserve(triangleFaces.size() * 2);

			for (const TriangleFace& triangleFace : triangleFaces)
			{
				if (triangleFace[0] < vertices.size() && triangleFace[1] < vertices.size() && triangleFace[2] < vertices.size()
						&& triangleFace[0] < colorsPerVertex.size() && triangleFace[1] < colorsPerVertex.size() && triangleFace[2] < colorsPerVertex.size())
				{
					for (unsigned int n = 0u; n < 3u; ++n)
					{
						const Index32 indexA = triangleFace[n];
						const Index32 indexB = triangleFace[n < 2u ? n + 1u : 0u];

						if (existingEdges.emplace(makeSortedIndex64(indexA, indexB)).second)
						{
							lineVertices.emplace_back(vertices[indexA]);
							lineVertices.emplace_back(vertices[indexB]);
							lineColors.emplace_back(colorsPerVertex[indexA]);
							lineColors.emplace_back(colorsPerVertex[indexB]);
						}
					}
				}
				else
				{
					Log::error() << "Invalid vertex index";
				}
			}
		}

		internalVertexSet->setVertices(lineVertices);
		internalVertexSet->setColors(lineColors);

		const LinesRef internalLines = engine.factory().createLines();
		internalLines->setVertexSet(internalVertexSet);
		internalLines->setIndices((unsigned int)(lineVertices.size()));

		const GeometryRef internalGeometry = engine.factory().createGeometry();

		const AttributeSetRef internalAttributeSet = engine.factory().createAttributeSet();

		if (colorsPerVertex.empty())
		{
			const MaterialRef internalMaterial = engine.factory().createMaterial();
			internalMaterial->setDiffuseColor(RGBAColor(0, 0, 0));
			internalMaterial->setEmissiveColor(emissiveColor);
			internalMaterial->setTransparency(1.0f - emissiveColor.alpha());

			if (internalMaterial->transparency() != 0.0f)
			{
				internalAttributeSet->addAttribute(engine.factory().createBlendAttribute());
			}

			internalAttributeSet->addAttribute(internalMaterial);

			if (material != nullptr)
			{
				*material = internalMaterial;
			}
		}

		internalGeometry->addRenderable(internalLines, internalAttributeSet);

		const TransformRef transform = engine.factory().createTransform();
		transform->addChild(internalGeometry);

		if (attributeSet != nullptr)
		{
			*attributeSet = internalAttributeSet;
		}

		if (lines != nullptr)
		{
			*lines = internalLines;
		}

		if (vertexSet != nullptr)
		{
			*vertexSet = internalVertexSet;
		}

		return transform;
	}
	catch (...)
	{
		// nothing to do here
	}

	return TransformRef();
}

TransformRef Utilities::createLines(const Engine& engine, const Vector3* points, const Vector3* normals, const size_t size, const Scalar scale, const RGBAColor& emissiveColor, MaterialRef* material, VertexSetRef* vertexSet)
{
	ocean_assert(points != nullptr && normals != nullptr);
	ocean_assert(size > 0);
	ocean_assert(emissiveColor.isValid());

	try
	{
		Vectors3 vertices;
		vertices.reserve(size * 2);

		for (size_t n = 0; n < size; ++n)
		{
			vertices.emplace_back(points[n]);
			vertices.emplace_back(points[n] + normals[n] * scale);
		}

		VertexSetRef internalVertexSet = engine.factory().createVertexSet();
		internalVertexSet->setVertices(vertices);

		const LinesRef internalLines = engine.factory().createLines();
		internalLines->setVertexSet(internalVertexSet);
		internalLines->setIndices((unsigned int)(vertices.size()));

		const GeometryRef internalGeometry = engine.factory().createGeometry();

		Rendering::AttributeSetRef attributeSet = engine.factory().createAttributeSet();

		const MaterialRef internalMaterial = engine.factory().createMaterial();
		internalMaterial->setDiffuseColor(RGBAColor(0, 0, 0));
		internalMaterial->setEmissiveColor(emissiveColor);
		internalMaterial->setTransparency(1.0f - emissiveColor.alpha());

		if (internalMaterial->transparency() != 0.0f)
		{
			attributeSet->addAttribute(engine.factory().createBlendAttribute());
		}

		attributeSet->addAttribute(internalMaterial);

		if (material != nullptr)
		{
			*material = internalMaterial;
		}

		internalGeometry->addRenderable(internalLines, std::move(attributeSet));

		const TransformRef transform = engine.factory().createTransform();
		transform->addChild(internalGeometry);

		if (vertexSet != nullptr)
		{
			*vertexSet = std::move(internalVertexSet);
		}

		return transform;
	}
	catch (...)
	{
		// nothing to do here
	}

	return TransformRef();
}

AttributeSetRef Utilities::findAttributeSet(const NodeRef& node)
{
	if (node.isNull())
	{
		return AttributeSetRef();
	}

	if (node->type() == Node::TYPE_GEOMETRY)
	{
		const GeometryRef geometry(node);
		ocean_assert(geometry);

		for (unsigned int n = 0u; n < geometry->numberRenderables(); ++n)
		{
			const AttributeSetRef attributeSet(geometry->attributeSet(n));

			if (attributeSet)
			{
				return attributeSet;
			}
		}
	}
	else
	{
		const GroupRef group(node);

		if (group)
		{
			for (unsigned int n = 0u; n < group->numberChildren(); ++n)
			{
				const AttributeSetRef attributeSet(findAttributeSet(group->child(n)));

				if (attributeSet)
				{
					return attributeSet;
				}
			}
		}
	}

	return AttributeSetRef();
}

}

}
