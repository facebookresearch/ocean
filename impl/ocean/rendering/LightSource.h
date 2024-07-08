/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_LIGHT_SOURCE_H
#define META_OCEAN_RENDERING_LIGHT_SOURCE_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Object.h"
#include "ocean/rendering/ObjectRef.h"

#include "ocean/math/RGBAColor.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class LightSource;

/**
 * Definition of a smart object reference holding a light source object.
 * @see SmartObjectRef, LightSource.
 * @ingroup rendering
 */
typedef SmartObjectRef<LightSource> LightSourceRef;

/**
 * This is the base class for all lights.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT LightSource : virtual public Object
{
	public:

		/**
		 * Defines different transformation types.
		 */
		enum TransformationType
		{
			/// The light is located in the coordinate system of the parent node, this is the default type.
			TT_PARENT,
			/// The light is located in the world coordinate system (an absolute location).
			TT_WORLD
		};

	public:

		/**
		 * Returns the ambient color of this light source.
		 * @return Ambient color
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual RGBAColor ambientColor() const;

		/**
		 * Returns the diffuse color of this light source.
		 * @return Diffuse color
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual RGBAColor diffuseColor() const;

		/**
		 * Returns the specular color of this light source.
		 * @return Specular color
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual RGBAColor specularColor() const;

		/**
		 * Returns the intensity of this light source.
		 * @return Light intensity with range [0.0, 1.0]
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual Scalar intensity() const;

		/**
		 * Returns the transformation type of this light.
		 * @return The light's transformation type, TT_PARENT by default
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual TransformationType transformationType() const;

		/**
		 * Returns whether this light source is enabled.
		 * @return True, if so
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual bool enabled() const;

		/**
		 * Gets all color values of the light at once.
		 * @param ambient Resulting ambient color of this light source
		 * @param diffuse Resulting diffuse color of this light source
		 * @param specular Resulting specular color of this light source
		 * @param intensity Resulting intensity of this light source
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void get(RGBAColor& ambient, RGBAColor& diffuse, RGBAColor& specular, Scalar& intensity);

		/**
		 * Sets the ambient color of the light source.
		 * @param color Ambient color
		 * @return True, if the specified color is valid and could be set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual bool setAmbientColor(const RGBAColor& color);

		/**
		 * Sets the diffuse color of the light source.
		 * @param color Diffuse color
		 * @return True, if the specified color is valid and could be set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual bool setDiffuseColor(const RGBAColor& color);

		/**
		 * Sets the specular color of the light source.
		 * @param color Specular color
		 * @return True, if the specified color is valid and could be set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual bool setSpecularColor(const RGBAColor& color);

		/**
		 * Sets the specular exponent of this material.
		 * @param intensity Light intensity to set with range [0.0, 1.0]
		 * @return True, if the valud is valid and could be set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual bool setIntensity(const Scalar intensity);

		/**
		 * Sets all color values of the light source at once.
		 * @param ambient Ambient color to set
		 * @param diffuse Diffuse color to set
		 * @param specular Specular color to set
		 * @param intensity Intensity value to set
		 * @return True, if at least one value is valid and could be set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual bool set(const RGBAColor& ambient, const RGBAColor& diffuse, const RGBAColor& specular, const Scalar intensity);

		/**
		 * Sets the transformation type of this light.
		 * @param transformationType The transformation type to set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setTransformationType(const TransformationType transformationType);

		/**
		 * Sets whether the light source is enabled.
		 * @param state True, if the light source has to be enabled
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setEnabled(const bool state);

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new light source object.
		 */
		LightSource();

		/**
		 * Destructs a light source object.
		 */
		~LightSource() override;
};

}

}

#endif // META_OCEAN_RENDERING_LIGHT_SOURCE_H
