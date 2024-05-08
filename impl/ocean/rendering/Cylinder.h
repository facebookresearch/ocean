/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_CYLINDER_H
#define META_OCEAN_RENDERING_CYLINDER_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/ObjectRef.h"
#include "ocean/rendering/Shape.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class Cylinder;

/**
 * Definition of a smart object reference holding a cylinder.
 * @see SmartObjectRef, Cylinder.
 * @ingroup rendering
 */
typedef SmartObjectRef<Cylinder> CylinderRef;

/**
 * This class is the base class for all cylinders.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT Cylinder : virtual public Shape
{
	public:

		/**
		 * Returns the height of this cylinder.
		 * @return Height of this cylinder
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual Scalar height() const;

		/**
		 * Returns the radius of this cylinder.
		 * @return Radius of this cylinder
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual Scalar radius() const;

		/**
		 * Returns whether the top face of the cylinder is visible.
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @return True, if so
		 */
		virtual bool top() const;

		/**
		 * Returns whether the bottom face of the cylinder is visible.
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @return True, if so
		 */
		virtual bool bottom() const;

		/**
		 * Reutrn whether the side face of the cylinder is visible.
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @return True, if so
		 */
		virtual bool side() const;

		/**
		 * Sets the height of this cylinder.
		 * @param height The height to set
		 * @return True, if the specified height was valid and could be set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual bool setHeight(const Scalar height);

		/**
		 * Sets the radius of this cylinder.
		 * @param radius Radius to set
		 * @return True, if the specified radius was valid and could be set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual bool setRadius(const Scalar radius);

		/**
		 * Sets whether the top face of the cylinder is visible.
		 * @param visible True, if the face has to be visible
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @return True, if succeeded
		 */
		virtual bool setTop(const bool visible);

		/**
		 * Sets whether the side face of the cylinder is visible.
		 * @param visible True, if the face has to be visible
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @return True, if succeeded
		 */
		virtual bool setSide(const bool visible);

		/**
		 * Sets whether the bottom face of the cylinder is visible.
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
		 * Creates a new cylinder object.
		 */
		Cylinder();

		/**
		 * Destructs a cylinder object.
		 */
		~Cylinder() override;
};

}

}

#endif // META_OCEAN_RENDERING_CYLINDER_H
