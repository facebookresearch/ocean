/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_TEXTURE_2D_H
#define META_OCEAN_RENDERING_TEXTURE_2D_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Texture.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Utilities.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class Texture2D;

/**
 * Definition of a smart object reference holding a 2D texture.
 * @see SmartObjectRef, Texture2D.
 * @ingroup rendering
 */
typedef SmartObjectRef<Texture2D> Texture2DRef;

/**
 * This class is the base class for all 2D textures.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT Texture2D : virtual public Texture
{
	public:

		/**
		 * Returns the ratio between original image dimension and actual texture dimension in horizontal and vertical dimensions.
		 * The returned value is calculated by: (image width / texture width, image height / texture height).<br>
		 * If the graphic system supports non-power-of-two texture the ratio will normaly be 1.0 for each dimension.
		 * @return Ratio between image and texture dimensions.
		 * @see adjustedToPowerOfTwo().
		 */
		virtual Vector2 imageTextureRatio() const;

		/**
		 * Returns the texture wrap type in s direction.
		 * The default is WRAP_CLAMP.
		 * @return Wrap type in s direction
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see setWrapTypeS(), wrapTypeT().
		 */
		virtual WrapType wrapTypeS() const;

		/**
		 * Returns the texture wrap type in t direction.
		 * The default is WRAP_CLAMP.
		 * @return Wrap type in t direction
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see setWrapTypeT(), wrapTypeS().
		 */
		virtual WrapType wrapTypeT() const;

		/**
		 * Sets the texture wrap type in s direction.
		 * @param type Wrap type to set
		 * @return True, if succeeded
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see wrapTypeS(), setWrapTypeT().
		 */
		virtual bool setWrapTypeS(const WrapType type);

		/**
		 * Sets the texture wrap type in t direction.
		 * @param type Wrap type to set
		 * @return True, if succeeded
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see wrapTypeT(), setWrapTypeS().
		 */
		virtual bool setWrapTypeT(const WrapType type);

		/**
		 * Returns the frame type of this 2D texture.
		 * @return Texture frame type
		 */
		virtual FrameType frameType() const;

		/**
		 * Returns whether this texture contains at least one transparent pixel.
		 * @return True, if so
		 */
		virtual bool hasTransparentPixel() const;

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new 2D texture object.
		 */
		Texture2D();

		/**
		 * Destructs a 2D texture object.
		 */
		~Texture2D() override;

		/**
		 * Calculates the smallest power of two texture in which a given image with arbitrary dimension fits.
		 * @param width The width of the image for which the smallest power-of-two width will be calculated, with range [1, infinity)
		 * @param height The height of the image for which the smallest power-of-two height will be calculated, with range [1, infinity)
		 * @param powerOfTwoWidth Resulting power of two width, with range [width, infinity), will be a power of two
		 * @param powerOfTwoHeight Resulting power of two height, with range [height, infinity), will be a power of two
		 */
		static inline void calculatePowerOfTwoDimension(const unsigned int width, const unsigned int height, unsigned int& powerOfTwoWidth, unsigned int& powerOfTwoHeight);
};

inline void Texture2D::calculatePowerOfTwoDimension(const unsigned int width, const unsigned int height, unsigned int& powerOfTwoWidth, unsigned int& powerOfTwoHeight)
{
	ocean_assert(width != 0u && height != 0u);

	powerOfTwoWidth = Utilities::smallestPowerOfTwo(width);
	powerOfTwoHeight = Utilities::smallestPowerOfTwo(height);
}

}

}

#endif // META_OCEAN_RENDERING_TEXTURE_2D_H
