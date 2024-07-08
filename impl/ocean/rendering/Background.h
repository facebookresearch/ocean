/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_BACKGROUND_H
#define META_OCEAN_RENDERING_BACKGROUND_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Node.h"

#include "ocean/math/Quaternion.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class Background;

/**
 * Definition of a smart object reference holding a background.
 * @see SmartObjectRef, Background.
 * @ingroup rendering
 */
typedef SmartObjectRef<Background> BackgroundRef;

/**
 * This class is the base class for all backgrounds.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT Background : virtual public Node
{
	public:

		/**
		 * Returns the distance between the world coordinate system and the background object.
		 * The default value is 1000.
		 * @return Background distance
		 * @see setDistance().
		 */
		virtual Scalar distance() const;

		/**
		 * Returns the position used for this background.
		 * The position is defined in relation to the center of projection.
		 * @return Background position
		 */
		virtual Vector3 position() const;

		/**
		 * Returns the orientation used for this background.
		 * @return Background orientation
		 */
		virtual Quaternion orientation() const;

		/**
		 * Sets the distance between world coordinate system and the background object.
		 * @param distance Distance to set, must be positive
		 * @return True, if succeeded
		 * @see distance().
		 */
		virtual bool setDistance(const Scalar distance);

		/**
		 * Sets the position used for this background.
		 * The position is defined in relation to the center of projection.
		 * @param position Background position to set
		 */
		virtual void setPosition(const Vector3& position);

		/**
		 * Sets the orientation used for this background.
		 * @param orientation Background orientation to set
		 */
		virtual void setOrientation(const Quaternion& orientation);

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/// Background distance;
		Scalar backgroundDistance;

		/**
		 * Creates a new background object.
		 */
		Background();

		/**
		 * Destructs a background object.
		 */
		~Background() override;
};

}

}

#endif // META_OCEAN_RENDERING_BACKGROUND_H
