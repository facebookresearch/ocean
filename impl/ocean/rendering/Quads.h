/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_QUADS_H
#define META_OCEAN_RENDERING_QUADS_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/IndependentPrimitive.h"
#include "ocean/rendering/QuadFace.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class Quads;

/**
 * Definition of a smart object reference holding a quads object.
 * @see SmartObjectRef, Quads.
 * @ingroup rendering
 */
typedef SmartObjectRef<Quads> QuadsRef;

/**
 * This class is the base class for all rendering quads.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT Quads : virtual public IndependentPrimitive
{
	public:

		/**
		 * Returns the faces of this quads object.
		 * @return Quad faces
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual QuadFaces faces() const;

		/**
		 * Returns the number of quad faces defined in this primitive.
		 * @return Number of quad faces
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual unsigned int numberFaces() const;

		/**
		 * Sets the faces of this triangles object.
		 * @param faces Faces of the quads
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setFaces(const QuadFaces& faces);

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new quads object.
		 */
		Quads();

		/**
		 * Destructs a quads object.
		 */
		~Quads() override;

};

}

}

#endif // META_OCEAN_RENDERING_QUADS_H
