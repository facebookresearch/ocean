/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_BILLBOARD_H
#define META_OCEAN_RENDERING_BILLBOARD_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Group.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class Billboard;

/**
 * Definition of a smart object reference holding a billboard.
 * @see SmartObjectRef, Billboard.
 * @ingroup rendering
 */
typedef SmartObjectRef<Billboard> BillboardRef;

/**
 * This class is the base class for all billboard objects.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT Billboard : virtual public Group
{
	public:

		/**
		 * Definition of different billboard alignment types.
		 */
		enum AlignmentType
		{
			/// Invalid alignment type.
			TYPE_INVALID,
			/// The object is aligned to specified axis.
			TYPE_AXIS,
			/// The object is aligned to the viewer.
			TYPE_VIEWER
		};

	public:

		/**
		 * Returns the alignment type of the billboard.
		 * @return Alignment type
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual AlignmentType alignmentType() const;

		/**
		 * Returns the alignment axis of the billboard.
		 * @return Alignment axis
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual Vector3 alignmentAxis() const;

		/**
		 * Sets the alignment type of the billboard.
		 * @param type Alignment type to set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setAlignmentType(const AlignmentType type);

		/**
		 * Sets the alignment axis of the billboard.
		 * @param axis Alignment axis to set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setAlignmentAxis(const Vector3& axis);

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new billboard object.
		 */
		Billboard();

		/**
		 * Destructs a billboard object.
		 */
		~Billboard() override;
};

}

}

#endif // META_OCEAN_RENDERING_BILLBOARD_H
