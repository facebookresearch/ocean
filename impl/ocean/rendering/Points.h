/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_POINTS_H
#define META_OCEAN_RENDERING_POINTS_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/IndependentPrimitive.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class Points;

/**
 * Definition of a smart object reference holding a points node.
 * @see SmartObjectRef, Points.
 * @ingroup rendering
 */
typedef SmartObjectRef<Points> PointsRef;

/**
 * This class is the base for all points objects.
 * A points object define individual points using a vertex set.<br>
 * Different points objects can use the same vertex set to share common vertices and to reduce memory usage.
 * @see VertexSet
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT Points : virtual public IndependentPrimitive
{
	public:

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

		/**
		 * Returns the indices of the used vertex points.
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
		 * Returns the size in pixels at which all points will be rendered.
		 * The size of a point is defined by the diameter of the point.
		 * @return The size of all points, in pixels, with range [1, infinity)
		 */
		virtual Scalar pointSize() const;

		/**
		 * Sets the indices of the used vertex points.
		 * The indices must not extend the number of defined vertices inside the used vertex set
		 * @param indices Vertex indices to be set
		 * @see indices(), VertexSet
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setIndices(const VertexIndices& indices);

		/**
		 * Sets the indices of the used vertex points.
		 * @param numberImplicitPoints The number of points on points with consecutive indices, with range [0, infinity)
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setIndices(const unsigned int numberImplicitPoints);

		/**
		 * Sets the size in pixels at which all points will be rendered.
		 * @param size The size of all points, in pixels, with range [1, infinity)
		 */
		virtual void setPointSize(const Scalar size);

	protected:

		/**
		 * Creates a new points object.
		 */
		Points();

		/**
		 * Destructs a points object.
		 */
		~Points() override;
};

}

}

#endif // META_OCEAN_RENDERING_POINTS_H
