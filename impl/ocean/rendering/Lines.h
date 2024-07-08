/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_LINES_H
#define META_OCEAN_RENDERING_LINES_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/IndependentPrimitive.h"
#include "ocean/rendering/TriangleFace.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class Lines;

/**
 * Definition of a smart object reference holding a lines node.
 * @see SmartObjectRef, Lines.
 * @ingroup rendering
 */
typedef SmartObjectRef<Lines> LinesRef;

/**
 * This class is the base for all lines objects.
 * A lines object define individual lines using a vertex set.<br>
 * Different lines objects can use the same vertex set to share common vertices and to reduce memory usage.
 * @see VertexSet
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT Lines : virtual public IndependentPrimitive
{
	public:

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

		/**
		 * Returns the indices of the used vertex lines.
		 * @return Vertex indices
		 * @see setIndices(), VertexSet
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual VertexIndices indices() const;

		/**
		 * Returns the number of point indices defined in this primitive.
		 * @return Number of point indices
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual unsigned int numberIndices() const;

		/**
		 * Sets the indices of the used vertex lines.
		 * The indices must not extend the number of defined vertices inside the used vertex set
		 * @param indices Vertex indices to be set
		 * @see indices(), VertexSet
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setIndices(const VertexIndices& indices);

		/**
		 * Sets the indices of the used vertex lines.
		 * @param numberImplicitLines The number of lines on points with consecutive indices, with range [0, infinity)
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setIndices(const unsigned int numberImplicitLines);

	protected:

		/**
		 * Creates a new lines object.
		 */
		Lines();

		/**
		 * Destructs a lines object.
		 */
		~Lines() override;
};

}

}

#endif // META_OCEAN_RENDERING_LINES_H
