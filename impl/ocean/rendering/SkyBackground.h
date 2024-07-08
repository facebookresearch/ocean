/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_SKY_BACKGROUND_H
#define META_OCEAN_RENDERING_SKY_BACKGROUND_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Background.h"
#include "ocean/rendering/Textures.h"

#include "ocean/math/RGBAColor.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class SkyBackground;

/**
 * Definition of a smart object reference holding a sky background.
 * @see SmartObjectRef, SkyBackground.
 * @ingroup rendering
 */
typedef SmartObjectRef<SkyBackground> SkyBackgroundRef;

/**
 * This class is the base class for all sky backgrounds.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT SkyBackground : virtual public Background
{
	public:

		/**
		 * Definition of a pair holding an angle and a color value.
		 * All angles must be defined as radian.
		 */
		typedef std::pair<Scalar, RGBAColor> ColorPair;

		/**
		 * Definition of a vector holding color values.
		 */
		typedef std::vector<ColorPair> ColorPairs;

	public:

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

		/**
		 * Returns the sky angle and color pairs.
		 * If any color values are defined the first pair must specified the color for the zenith (angle 0).<br>
		 * The angle values are defined as radian with range [0, PI], 0 for the zenith, PI/2 for the standard horizon and PI for the nadir
		 * @return Color pairs
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual ColorPairs skyColors() const;

		/**
		 * Returns the ground angle and color pairs.
		 * If any color values are defined the first pair must specified the color for the nadir (angle 0).<br>
		 * The angle values are defined as radian with range [0, PI/2], 0 for the nadir and PI/2 for the standard horizon
		 * @return Color pairs
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual ColorPairs groundColors() const;

		/**
		 * Returns the front textures.
		 * @return Textures object used for the front plane
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual TexturesRef frontTextures() const;

		/**
		 * Returns the back textures.
		 * @return Textures object used for the back plane
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual TexturesRef backTextures() const;

		/**
		 * Returns the left textures.
		 * @return Textures object used for the left plane
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual TexturesRef leftTextures() const;

		/**
		 * Returns the right textures.
		 * @return Textures object used for the right plane
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual TexturesRef rightTextures() const;

		/**
		 * Returns the top textures.
		 * @return Textures object used for the top plane
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual TexturesRef topTextures() const;

		/**
		 * Returns the bottom textures.
		 * @return Textures object used for the bottom plane
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual TexturesRef bottomTextures() const;

		/**
		 * Sets the sky angle and color pairs for the sky background.
		 * If any color values are defined the first pair must specified the color for the zenith (angle 0).<br>
		 * The angle values are defined as radian with range [0, PI], 0 for the zenith, PI/2 for the standard horizon and PI for the nadir
		 * @param colors Sky color pairs to set
		 * @return True, if succeeded
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual bool setSkyColors(const ColorPairs& colors);

		/**
		 * Returns the ground angle and color pairs.
		 * If any color values are defined the first pair must specified the color for the nadir (angle 0).<br>
		 * The angle values are defined as radian with range [0, PI/2], 0 for the nadir and PI/2 for the standard horizon
		 * @param colors Ground color pairs to set
		 * @return True, if succeeded
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual bool setGroundColors(const ColorPairs& colors);

		/**
		 * Sets the textures for the front plane.
		 * @param textures Textures object to be used for the front plane
		 * @return True, if succeeded
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual bool setFrontTextures(const TexturesRef& textures);

		/**
		 * Sets the textures for the back plane.
		 * @param textures Textures object to be used for the back plane
		 * @return True, if succeeded
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual bool setBackTextures(const TexturesRef& textures);

		/**
		 * Sets the textures for the left plane.
		 * @param textures Textures object to be used for the left plane
		 * @return True, if succeeded
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual bool setLeftTextures(const TexturesRef& textures);

		/**
		 * Sets the textures for the right plane.
		 * @param textures Textures object to be uesed for the right plane
		 * @return True, if succeeded
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual bool setRightTextures(const TexturesRef& textures);

		/**
		 * Sets the textures for the top plane.
		 * @param textures Textures object to be used for the top plane
		 * @return True, if succeeded
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual bool setTopTextures(const TexturesRef& textures);

		/**
		 * Sets the textures for the bottom plane.
		 * @param textures Textures object to be used for the bottom plane
		 * @return True, if succeeded
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual bool setBottomTextures(const TexturesRef& textures);

	protected:

		/**
		 * Creates a new sky background object.
		 */
		SkyBackground();

		/**
		 * Destructs a Sky background object.
		 */
		~SkyBackground() override;
};

}

}

#endif // META_OCEAN_RENDERING_SKY_BACKGROUND_H
