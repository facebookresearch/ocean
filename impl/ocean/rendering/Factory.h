/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_FACTORY_H
#define META_OCEAN_RENDERING_FACTORY_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Node.h"
#include "ocean/rendering/ObjectRef.h"

namespace Ocean
{

namespace Rendering
{

/**
 * This class implements a node and object factory.<br>
 * A factory instance can be received from the associated engine instance.<br>
 * However, there is no guarantee that all objects are supported for a specific render engine.<br>
 * Therefore, all factory functions will throw an exception if the selected object is not supported.
 * @see Engine, NotSupportedException
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT Factory
{
	public:

		/**
		 * Destructor
		 */
		inline virtual ~Factory();

		/**
		 * Creates a new absolute transform.
		 * @return New absolute transform
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual ObjectRef createAbsoluteTransform() const;

		/**
		 * Creates a new attribute set object.
		 * @return New attribute set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual ObjectRef createAttributeSet() const;

		/**
		 * Creates a new billboard object.
		 * @return New billboard
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual NodeRef createBillboard() const;

		/**
		 * Creates a new blend attribute object.
		 * @return New blend attribute
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual ObjectRef createBlendAttribute() const;

		/**
		 * Creates a new box object.
		 * @return New box
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual ObjectRef createBox() const;

		/**
		 * Creates a new cone object.
		 * @return New cone
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual ObjectRef createCone() const;

		/**
		 * Creates a new cylinder object.
		 * @return New cylinder
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual ObjectRef createCylinder() const;

		/**
		 * Creates a new depth attribute object.
		 * @return New depth attribute
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual ObjectRef createDepthAttribute() const;

		/**
		 * Creates a new directional light object.
		 * @return New directional light
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual ObjectRef createDirectionalLight() const;

		/**
		 * Creates a new frame texture object.
		 * @return New texture
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual ObjectRef createFrameTexture2D() const;

		/**
		 * Creates a new geometry object.
		 * @return New geometry
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual NodeRef createGeometry() const;

		/**
		 * Creates a new group object.
		 * @return New group
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual NodeRef createGroup() const;

		/**
		 * Creates a new lines object.
		 * @return New lines
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual ObjectRef createLines() const;

		/**
		 * Creates a new line strips object.
		 * @return New line strips
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual ObjectRef createLineStrips() const;

		/**
		 * Creates a new LOD object.
		 * @return new LOD
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual NodeRef createLOD() const;

		/**
		 * Creates a new material object.
		 * @return New material
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual ObjectRef createMaterial() const;

		/**
		 * Creates a new media texture object.
		 * @return New texture
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual ObjectRef createMediaTexture2D() const;

		/**
		 * Creates a new parallel view object.
		 * @return New parallel view
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual ObjectRef createParallelView() const;

		/**
		 * Creates a new perspective view object.
		 * @return New perspective view
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual ObjectRef createPerspectiveView() const;

		/**
		 * Creates a new phantom attribute object.
		 * @return New phantom attribute
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual ObjectRef createPhantomAttribute() const;

		/**
		 * Creates a new point light object.
		 * @return New light
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual ObjectRef createPointLight() const;

		/**
		 * Creates a new points object.
		 * @return New points
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual ObjectRef createPoints() const;

		/**
		 * Creates a new primitive attribute object.
		 * @return New attribute
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual ObjectRef createPrimitiveAttribute() const;

		/**
		 * Creates a new quads object.
		 * @return New quads
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual ObjectRef createQuads() const;

		/**
		 * Creates a new quad strips object.
		 * @return New quad strips
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual ObjectRef createQuadStrips() const;

		/**
		 * Creates a new scene object.
		 * @return New scene
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual NodeRef createScene() const;

		/**
		 * Creates a new shader program object.
		 * @return New shader program
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual ObjectRef createShaderProgram() const;

		/**
		 * Creates a new sky background object.
		 * @return New sky background
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual NodeRef createSkyBackground() const;

		/**
		 * Creates a new sphere object.
		 * @return New sphere
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual ObjectRef createSphere() const;

		/**
		 * Creates a new spot light object.
		 * @return New spot light
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual ObjectRef createSpotLight() const;

		/**
		 * Creates a new stereo attribute object.
		 * @return New stereo attribute
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual ObjectRef createStereoAttribute() const;

		/**
		 * Creates a new stereo view object.
		 * @return New stereo view
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual ObjectRef createStereoView() const;

		/**
		 * Creates a new switch object.
		 * @return New switch
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual NodeRef createSwitch() const;

		/**
		 * Creates a new text object.
		 * @return The new text framebuffer object
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual ObjectRef createText() const;

		/**
		 * Creates a new texture framebuffer object.
		 * @return The new texture framebuffer object
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual ObjectRef createTextureFramebuffer() const;

		/**
		 * Creates a new texture attribute object.
		 * @return New texture attribute
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual ObjectRef createTextures() const;

		/**
		 * Creates a new transform object.
		 * @return New transform
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual NodeRef createTransform() const;

		/**
		 * Creates a new triangles object.
		 * @return New triangles
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual ObjectRef createTriangles() const;

		/**
		 * Creates a new triangle fans object.
		 * @return New triangle fans
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual ObjectRef createTriangleFans() const;

		/**
		 * Creates a new triangle strips object.
		 * @return New triangle strips
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual ObjectRef createTriangleStrips() const;

		/**
		 * Creates a new undistorted background node.
		 * @return New undistorted background
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual NodeRef createUndistortedBackground() const;

		/**
		 * Creates a new vertex set object.
		 * @return New vertex set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual ObjectRef createVertexSet() const;

		/**
		 * Creates a new object specified by the object type name.
		 * This function may be overloaded if a rendering plugin supports more than the standard objects.
		 * @param type The type of the object to be created, must be valid
		 * @return New rendering object if available, otherwise an empty object reference
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual ObjectRef createObject(const std::string& type) const;

	protected:

		/**
		 * Protected default constructor creating a new Factory.
		 */
		Factory();

		/**
		 * Disabled copy constructor.
		 * @param factory Object which would be copied
		 */
		Factory(const Factory& factory) = delete;

		/**
		 * Creates a new object reference by a given object.
		 * @param object The object to create a object reference from, must be valid
		 * @return Created object reference
		 */
		inline ObjectRef createReference(Object* object) const;

		/**
		 * Disabled copy operator.
		 * @param factory Object which would be copied
		 * @return Reference to this object
		 */
		Factory& operator=(const Factory& factory) = delete;
};

inline Factory::~Factory()
{
	// Nothing else to do.
}

inline ObjectRef Factory::createReference(Object* object) const
{
	ocean_assert(object != nullptr);
	return ObjectRefManager::get().registerObject(object);
}

}

}

#endif // META_OCEAN_RENDERING_FACTORY_H
