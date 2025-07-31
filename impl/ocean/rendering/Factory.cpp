/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/Factory.h"

namespace Ocean
{

namespace Rendering
{

Factory::Factory()
{
	// nothing to do here
}

ObjectRef Factory::createAbsoluteTransform() const
{
	throw NotSupportedException("AbsoluteTransform set is not supported.");
}

ObjectRef Factory::createAttributeSet() const
{
	throw NotSupportedException("Attribute set is not supported.");
}

NodeRef Factory::createBillboard() const
{
	throw NotSupportedException("Billboard is not supported.");
}

ObjectRef Factory::createBlendAttribute() const
{
	throw NotSupportedException("Blend attribute is not supported.");
}

ObjectRef Factory::createBox() const
{
	throw NotSupportedException("Box is not supported.");
}

ObjectRef Factory::createCone() const
{
	throw NotSupportedException("Cone is not supported.");
}

ObjectRef Factory::createCylinder() const
{
	throw NotSupportedException("Cylinder is not supported.");
}

ObjectRef Factory::createDepthAttribute() const
{
	throw NotSupportedException("Depth attribute is not supported.");
}

ObjectRef Factory::createDirectionalLight() const
{
	throw NotSupportedException("Directional light is not supported.");
}

ObjectRef Factory::createFrameTexture2D() const
{
	throw NotSupportedException("FrameTexture2d is not supported.");
}

NodeRef Factory::createGeometry() const
{
	throw NotSupportedException("Geometry is not supported.");
}

NodeRef Factory::createGroup() const
{
	throw NotSupportedException("Group is not supported.");
}

ObjectRef Factory::createLines() const
{
	throw NotSupportedException("Lines is not supported.");
}

ObjectRef Factory::createLineStrips() const
{
	throw NotSupportedException("LineStrips is not supported.");
}

NodeRef Factory::createLOD() const
{
	throw NotSupportedException("LOD is not supported.");
}

ObjectRef Factory::createMaterial() const
{
	throw NotSupportedException("Material is not supported.");
}

ObjectRef Factory::createMediaTexture2D() const
{
	throw NotSupportedException("MediaTexture2D is not supported.");
}

ObjectRef Factory::createParallelView() const
{
	throw NotSupportedException("Parallel view is not supported.");
}

ObjectRef Factory::createPerspectiveView() const
{
	throw NotSupportedException("Perspective view is not supported.");
}

ObjectRef Factory::createPhantomAttribute() const
{
	throw NotSupportedException("Phantom attribute is not supported.");
}

ObjectRef Factory::createPointLight() const
{
	throw NotSupportedException("Point light is not supported.");
}

ObjectRef Factory::createPoints() const
{
	throw NotSupportedException("Points is not supported.");
}

ObjectRef Factory::createPrimitiveAttribute() const
{
	throw NotSupportedException("Primitive attribute is not supported.");
}

ObjectRef Factory::createQuads() const
{
	throw NotSupportedException("Quads is not supported.");
}

ObjectRef Factory::createQuadStrips() const
{
	throw NotSupportedException("Quad strips is not supported.");
}

NodeRef Factory::createScene() const
{
	throw NotSupportedException("Scene is not supported.");
}

ObjectRef Factory::createShaderProgram() const
{
	throw NotSupportedException("ShaderProgram is not supported.");
}

NodeRef Factory::createSkyBackground() const
{
	throw NotSupportedException("Sky background is not supported.");
}

ObjectRef Factory::createSphere() const
{
	throw NotSupportedException("Sphere is not supported.");
}

ObjectRef Factory::createSpotLight() const
{
	throw NotSupportedException("Spot light is not supported.");
}

ObjectRef Factory::createStereoAttribute() const
{
	throw NotSupportedException("Stereo attribute is not supported.");
}

ObjectRef Factory::createStereoView() const
{
	throw NotSupportedException("Stereo view is not supported.");
}

NodeRef Factory::createSwitch() const
{
	throw NotSupportedException("Switch is not supported.");
}

ObjectRef Factory::createText() const
{
	throw NotSupportedException("Text is not supported.");
}

ObjectRef Factory::createTextureFramebuffer() const
{
	throw NotSupportedException("TextureFramebuffer is not supported.");
}

ObjectRef Factory::createTextures() const
{
	throw NotSupportedException("Textures is not supported.");
}

NodeRef Factory::createTransform() const
{
	throw NotSupportedException("Transform is not supported.");
}

ObjectRef Factory::createTriangleFans() const
{
	throw NotSupportedException("Triangle fans is not supported.");
}

ObjectRef Factory::createTriangles() const
{
	throw NotSupportedException("Triangles is not supported.");
}

ObjectRef Factory::createTriangleStrips() const
{
	throw NotSupportedException("Triangle strips is not supported.");
}

NodeRef Factory::createUndistortedBackground() const
{
	throw NotSupportedException("Undistorted background is not supported.");
}

ObjectRef Factory::createVertexSet() const
{
	throw NotSupportedException("Vertex set is not supported.");
}

ObjectRef Factory::createObject(const std::string& type) const
{
	if (type == "AttributeSet")
		return createAttributeSet();
	if (type == "Billboard")
		return createBillboard();
	if (type == "BlendAttribute")
		return createBlendAttribute();
	if (type == "Box")
		return createBox();
	if (type == "Cone")
		return createCone();
	if (type == "Cylinder")
		return createCylinder();
	if (type == "DepthAttribute")
		return createDepthAttribute();
	if (type == "DirectionalLight")
		return createDirectionalLight();
	if (type == "Geometry")
		return createGeometry();
	if (type == "Group")
		return createGroup();
	if (type == "Lines")
		return createLines();
	if (type == "LineStrips")
		return createLineStrips();
	if (type == "LOD")
		return createLOD();
	if (type == "Material")
		return createMaterial();
	if (type == "ParallelView")
		return createParallelView();
	if (type == "PerspectiveView")
		return createPerspectiveView();
	if (type == "PhantomAttribute")
		return createPhantomAttribute();
	if (type == "PointLight")
		return createPointLight();
	if (type == "PrimitiveAttribute")
		return createPrimitiveAttribute();
	if (type == "Quads")
		return createQuads();
	if (type == "QuadStrips")
		return createQuadStrips();
	if (type == "Scene")
		return createScene();
	if (type == "ShaderProgram")
		return createShaderProgram();
	if (type == "SkyBackground")
		return createSkyBackground();
	if (type == "Sphere")
		return createSphere();
	if (type == "SpotLight")
		return createSpotLight();
	if (type == "StereoView")
		return createStereoView();
	if (type == "Switch")
		return createSwitch();
	if (type == "MediaTexture2D")
		return createMediaTexture2D();
	if (type == "Textures")
		return createTextures();
	if (type == "Transform")
		return createTransform();
	if (type == "Triangles")
		return createTriangles();
	if (type == "TriangleFans")
		return createTriangleFans();
	if (type == "TriangleStrips")
		return createTriangleStrips();
	if (type == "UndistortedBackground")
		return createUndistortedBackground();
	if (type == "VertexSet")
		return createVertexSet();

	return ObjectRef();
}

}

}
