/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_PRIMITIVE_ATTRIBUTE_H
#define META_OCEAN_RENDERING_PRIMITIVE_ATTRIBUTE_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Attribute.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class PrimitiveAttribute;

/**
 * Definition of a smart object reference holding a primitive attribute.
 * @see SmartObjectRef, PrimitiveAttribute.
 * @ingroup rendering
 */
typedef SmartObjectRef<PrimitiveAttribute> PrimitiveAttributeRef;

/**
 * This class implements an attribute specifying face and culling properties of primitives.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT PrimitiveAttribute : virtual public Attribute
{
	public:

		/**
		 * Defines different face rendering modes.
		 */
		enum FaceMode
		{
			/// Face rendering mode rendering the entire face, which is the default mode.
			MODE_FACE,
			/// Line rendering mode rendering the face outlines only.
			MODE_LINE,
			/// Point rendering mode rendering the face vertices only.
			MODE_POINT,
			/// Default face mode.
			MODE_DEFAULT
		};

		/**
		 * Defines different face culling modes.
		 */
		enum CullingMode
		{
			/// Neither front nor back faces culling which results in front and back face rendering.
			CULLING_NONE = 0,
			/// Back face culling mode rendering front faces only, which is the default mode.
			CULLING_BACK = 1,
			/// Front face culling mode rendering back faces only.
			CULLING_FRONT = 2,
			/// Front and back face culling mode rendering neither front nor back faces.
			CULLING_BOTH = CULLING_BACK | CULLING_FRONT,
			/// Default culling mode.
			CULLING_DEFAULT
		};

		/**
		 * Definition of different lighting modes.
		 */
		enum LightingMode
		{
			/// Front face is lighted.
			LM_SINGLE_SIDE_LIGHTING,
			/// Both faces are lighted.
			LM_TWO_SIDED_LIGHTING,
			/// Default lighting.
			LM_DEFAULT
		};

	public:

		/**
		 * Returns the face mode of this attribute.
		 * The default is MODE_FACE.
		 * @return Face mode
		 * @see setFaceMode(), Framebuffer::faceMode().
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual FaceMode faceMode() const;

		/**
		 * Returns the culling mode of this attribute.
		 * The default is CULLLING_BACK.
		 * @return Culling mode
		 * @see setCullingMode(), Framebuffer::cullingMode().
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual CullingMode cullingMode() const;

		/**
		 * Returns the lighting mode of this attribute.
		 * If both sides are lighted the back face is lighted using the inversed normal.<br>
		 * The default is LM_SINGLE_LIGHTING.
		 * @return Lighting mode
		 * @see setLightingMode(), Framebuffer::lightingMode().
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual LightingMode lightingMode() const;

		/**
		 * Sets the face mode for this attribute.
		 * @param mode Face mode to set
		 * @see faceMode(), Framebuffer::setFaceMode().
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setFaceMode(const FaceMode mode);

		/**
		 * Sets the culling mode for this attribute.
		 * @param culling Culling mode to set
		 * @see cullingMode(), Framebuffer::setCullingMode().
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setCullingMode(const CullingMode culling);

		/**
		 * Sets the lighting mode for this attribute.
		 * @param lighting Lighting mode to set
		 * @see lightingMode(), Framebuffer::setLightingMode().
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setLightingMode(const LightingMode lighting);

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new primitive attribute.
		 */
		PrimitiveAttribute();
};

}

}

#endif // META_OCEAN_RENDERING_PRIMITIVE_ATTRIBUTE_H
