/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_LOD_H
#define META_OCEAN_RENDERING_LOD_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Group.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class LOD;

/**
 * Definition of a smart object reference holding a LOD node.
 * @see SmartObjectRef, LOD.
 * @ingroup rendering
 */
typedef SmartObjectRef<LOD> LODRef;

/**
 * This class is the base class for all level-of-detail nodes.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT LOD : virtual public Group
{
	public:

		/**
		 * Definition of a vector holding distance ranges.
		 */
		typedef std::vector<Scalar> DistanceRanges;

	public:

		/**
		 * Returns the n+1 distance ranges for n child nodes.
		 * @return Distance ranges specified
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual DistanceRanges distanceRanges() const;

		/**
		 * Sets the n+1 distance ranges for n child nodes.
		 * @param ranges Distance ranges for the LOD node
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setDistanceRanges(const DistanceRanges& ranges);

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new LOD node.
		 */
		LOD();

		/**
		 * Destructs a LOD node.
		 */
		~LOD() override;
};

}

}

#endif // META_OCEAN_RENDERING_LOD_H
