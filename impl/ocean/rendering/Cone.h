/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_CONE_H
#define META_OCEAN_RENDERING_CONE_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/ObjectRef.h"
#include "ocean/rendering/Shape.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class Cone;

/**
 * Definition of a smart object reference holding a cone.
 * @see SmartObjectRef, Cone.
 * @ingroup rendering
 */
typedef SmartObjectRef<Cone> ConeRef;

/**
 * This class is the base class for all cones.
 * The default cone has radius 1 and height 1.
 * The middle of the cone is defined about (0, 0, 0).
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT Cone : virtual public Shape
{
	public:

		/**
		 * Returns the radius of this cone.
		 * @return Radius of this cone
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual Scalar radius() const;

		/**
		 * Returns the height of this cone.
		 * @return Height of this cone
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual Scalar height() const;

		/**
		 * Returns whether the bottom face of the cone is visible.
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @return True, if so
		 */
		virtual bool bottom() const;

		/**
		 * Returns whether the side face of the cone is visible.
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @return True, if so
		 */
		virtual bool side() const;

		/**
		 * Sets the radius of this cone.
		 * @param radius Radius to set
		 * @return True, if the specified radius was valid and could be set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual bool setRadius(const Scalar radius);

		/**
		 * Sets the height of this cone.
		 * @param height The height to set
		 * @return True, if the specified height was valid and could be set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual bool setHeight(const Scalar height);

		/**
		 * Sets whether the side face of the cone is visible.
		 * @param visible True, if the face has to be visible
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @return True, if succeeded
		 */
		virtual bool setSide(const bool visible);

		/**
		 * Sets whether the bottom face of the cone is visible.
		 * @param visible True, if the face has to be visible
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @return True, if succeeded
		 */
		virtual bool setBottom(const bool visible);

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new cone object.
		 */
		Cone();

		/**
		 * Destructs a cone object.
		 */
		~Cone() override;
};

}

}

#endif // META_OCEAN_RENDERING_CONE_H
