/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_PRIMITIVE_H
#define META_OCEAN_RENDERING_PRIMITIVE_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Renderable.h"
#include "ocean/rendering/VertexSet.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class Primitive;

/**
 * Definition of a smart object reference holding a primitive.
 * @see SmartObjectRef, Primitive.
 * @ingroup rendering
 */
typedef SmartObjectRef<Primitive> PrimitiveRef;

/**
 * This class is the base class for all primitive objects.
 * A primitive object defines a mesh by the use of a vertex set.<br>
 * Mainly two different types primitives exist: independent primitives and strip primitives.<br>
 * Independent primitives defines a mesh by separated and self-contained triangles or quads.<br>
 * Strip primitives defines a mesh using strips or fans to define consecutive triangles or quads.<br>
 * Different primitive objects can use the same vertex set to share common vertices and to reduce memory usage.
 * @see VertexSet, IndependentPrimitive, StripPrimitive
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT Primitive : virtual public Renderable
{
	public:

		/**
		 * Returns the vertex set of this primitive object.
		 * @return Vertex set object
		 */
		inline const VertexSetRef& vertexSet() const;

		/**
		 * Sets the vertex set of this primitive object.
		 * @param vertexSet New vertex set object
		 */
		virtual void setVertexSet(const VertexSetRef& vertexSet);

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new primitive object.
		 */
		Primitive();

		/**
		 * Destructs a primitive object.
		 */
		~Primitive() override;

	protected:

		/// Vertex set object.
		VertexSetRef primitiveVertexSet;
};

inline const VertexSetRef& Primitive::vertexSet() const
{
	return primitiveVertexSet;
}

}

}

#endif // META_OCEAN_RENDERING_PRIMITIVE_H
