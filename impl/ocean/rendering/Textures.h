/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_TEXTURES_H
#define META_OCEAN_RENDERING_TEXTURES_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Attribute.h"
#include "ocean/rendering/ObjectRef.h"
#include "ocean/rendering/Texture.h"

#include <vector>

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class Textures;

/**
 * Definition of a smart object reference holding a textures.
 * @see SmartObjectRef, Textures.
 * @ingroup rendering
 */
typedef SmartObjectRef<Textures> TexturesRef;

/**
 * This class is the base class for all textures objects.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT Textures : virtual public Attribute
{
	public:

		/**
		 * Definition of a vector holding textures.
		 */
		typedef std::vector<TextureRef> TextureObjects;

	public:

		/**
		 * Returns the texture for a specified texture layer.
		 * @param layerIndex Index of the texture layer to return the texture for
		 * @return Specified texture object
		 */
		virtual TextureRef texture(const unsigned int layerIndex) const;

		/**
		 * Returns the number of specified texture layers.
		 * @return Number of texture layers
		 */
		inline unsigned int numberTextures() const;

		/**
		 * Sets the texture for a specified texture layer.
		 * @param texture Texture for the defined texture layer
		 * @param layerIndex Index of the texture layer to set the texture attribute for
		 */
		virtual void setTexture(const TextureRef& texture, const unsigned int layerIndex);

		/**
		 * Adds a texture at the next free position of the texture layer.
		 * @param texture Texture to add
		 */
		virtual void addTexture(const TextureRef& texture);

		/**
		 * Returns whether at least one texture has at least one alpha pixel.
		 * @return True, if so
		 */
		virtual bool hasAlphaTexture() const;

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new textures object.
		 */
		Textures();

		/**
		 * Destructs a textures object.
		 */
		~Textures() override;

	protected:

		/// Vector holding texture objects.
		TextureObjects textures;
};

inline unsigned int Textures::numberTextures() const
{
	return (unsigned int)(textures.size());
}

}

}

#endif // META_OCEAN_RENDERING_TEXTURES_H
