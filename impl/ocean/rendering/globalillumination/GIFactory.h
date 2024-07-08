/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_FACTORY_H
#define META_OCEAN_RENDERING_GI_FACTORY_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"

#include "ocean/rendering/Factory.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

/**
 * This class implements a class factory for all Global Illumination objects.<br>
 * Only the factory is able to create any Global Illumination object.
 * @ingroup renderinggi
 */
class OCEAN_RENDERING_GI_EXPORT GIFactory : public Factory
{
	public:

		/**
		 * Creates a new attribute set object.
		 * @see Factory::createAbsoluteTransform().
		 */
		ObjectRef createAbsoluteTransform() const override;

		/**
		 * Creates a new attribute set object.
		 * @see Factory::createAttributeSet().
		 */
		ObjectRef createAttributeSet() const override;

		/**
		 * Creates a new billboard object.
		 * @see Factory::createBillboard().
		 */
		NodeRef createBillboard() const override;

		/**
		 * Creates a new blend attribute object.
		 * @return New blend attribute
		 */
		ObjectRef createBlendAttribute() const override;

		/**
		 * Creates a new box object.
		 * @see Factory::createBox().
		 */
		ObjectRef createBox() const override;

		/**
		 * Creates a new cone object.
		 * @see Factory::createCone().
		 */
		ObjectRef createCone() const override;

		/**
		 * Creates a new cylinder object.
		 * @see Factory::createCylinder().
		 */
		ObjectRef createCylinder() const override;

		/**
		 * Creates a new depth attribute object.
		 * @see Factory::createDepthAttribute().
		 */
		ObjectRef createDepthAttribute() const override;

		/**
		 * Creates a new directional light object.
		 * @see Factory::createDirectionalLight().
		 */
		ObjectRef createDirectionalLight() const override;

		/**
		 * Creates a new geometry object.
		 * @see Factory::createGeometry().
		 */
		NodeRef createGeometry() const override;

		/**
		 * Creates a new group object.
		 * @see Factory::createGroup().
		 */
		NodeRef createGroup() const override;

		/**
		 * Creates a new LOD object.
		 * @see Factory::createLOD().
		 */
		NodeRef createLOD() const override;

		/**
		 * Creates a new material object.
		 * @see Factory::createMaterial().
		 */
		ObjectRef createMaterial() const override;

		/**
		 * Creates a new parallel view object.
		 * @see Factory::createParallelView().
		 */
		ObjectRef createParallelView() const override;

		/**
		 * Creates a new perspective view object.
		 * @see Factory::createPerspectiveView().
		 */
		ObjectRef createPerspectiveView() const override;

		/**
		 * Creates a new phantom attribute object.
		 * @see Factory::createPhantomAttribute().
		 */
		ObjectRef createPhantomAttribute() const override;

		/**
		 * Creates a new point light object.
		 * @see Factory::createPointLight().
		 */
		ObjectRef createPointLight() const override;

		/**
		 * Creates a new points object.
		 * @see Factory::createPoints().
		 */
		ObjectRef createPoints() const override;

		/**
		 * Creates a new point light object.
		 * @see Factory::createPrimitiveAttribute().
		 */
		ObjectRef createPrimitiveAttribute() const override;

		/**
		 * Creates a new quads object.
		 * @see Factory::createQuads().
		 */
		ObjectRef createQuads() const override;

		/**
		 * Creates a new quad strips object.
		 * @see Factory::createQuadStrips().
		 */
		ObjectRef createQuadStrips() const override;

		/**
		 * Creates a new scene object.
		 * @see Factory::createScene().
		 */
		NodeRef createScene() const override;

		/**
		 * Creates a new shader program cg object.
		 * @see Factory::createShaderProgram().
		 */
		ObjectRef createShaderProgram() const override;

		/**
		 * Creates a new sky background object.
		 * @see Factory::createSkyBackground().
		 */
		NodeRef createSkyBackground() const override;

		/**
		 * Creates a new sphere object.
		 * @see Factory::createSphare().
		 */
		ObjectRef createSphere() const override;

		/**
		 * Creates a new switch object.
		 * @see Factory::createSwitch().
		 */
		NodeRef createSwitch() const override;

		/**
		 * Creates a new spot light object.
		 * @see Factroy::createSpotLight().
		 */
		ObjectRef createSpotLight() const override;

		/**
		 * Creates a new stereo view object.
		 * @see Factory::createStereoView().
		 */
		ObjectRef createStereoView() const override;

		/**
		 * Creates a new texture object.
		 * @see Factory::createMediaTexture2D().
		 */
		ObjectRef createMediaTexture2D() const override;

		/**
		 * Creates a new texture attribute object.
		 * @see Factory::createTextures().
		 */
		ObjectRef createTextures() const override;

		/**
		 * Creates a new transform object.
		 * @see Factory::createTransform().
		 */
		NodeRef createTransform() const override;

		/**
		 * Creates a new triangle fans object.
		 * @see Factory::createTriangleFans().
		 */
		ObjectRef createTriangleFans() const override;

		/**
		 * Creates a new triangles object.
		 * @see Factory::createTriangles().
		 */
		ObjectRef createTriangles() const override;

		/**
		 * Creates a new triangle strips object.
		 * @see Factory::createTriangleStrips().
		 */
		ObjectRef createTriangleStrips() const override;

		/**
		 * Creates a new undistorted background object.
		 * @see Factory::createUndistortedBackground().
		 */
		NodeRef createUndistortedBackground() const override;

		/**
		 * Creates a new vertex set object.
		 * @see Factory::createVertexSet().
		 */
		ObjectRef createVertexSet() const override;
};

}

}

}

#endif // META_OCEAN_RENDERING_GI_FACTORY_H
