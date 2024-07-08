/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_STEREO_ATTRIBUTE_H
#define META_OCEAN_RENDERING_STEREO_ATTRIBUTE_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Attribute.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class StereoAttribute;

/**
 * Definition of a smart object reference holding a stereo attribute.
 * @see SmartObjectRef, StereoAttribute.
 * @ingroup rendering
 */
typedef SmartObjectRef<StereoAttribute> StereoAttributeRef;

/**
 * This class implements a stereo attribute specifying the stereo behavior for associated objects.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT StereoAttribute : virtual public Attribute
{
	public:

		/**
		 * Definition of different stereo visibilities.
		 */
		enum StereoVisibility
		{
			/// The object is neither visible in the left nor in the right stereo view.
			SV_NONE = 0,
			/// The object is visible in the left stereo view.
			SV_LEFT = 1,
			/// The object is visible in the right stereo view.
			SV_RIGHT = 2,
			/// The object is visbile in left and right stereo view, this is the default property.
			SV_LEFT_AND_RIGHT = SV_LEFT | SV_RIGHT
		};

	public:

		/**
		 * Returns the stereo visibility property.
		 * @return Stereo visibility
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual StereoVisibility stereoVisibility() const;

		/**
		 * Sets the stereo visibility property.
		 * @param stereoVisibility The stereo visibility to be set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setStereoVisibility(const StereoVisibility stereoVisibility);

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new stereo attribute object.
		 */
		StereoAttribute();
};

}

}

#endif // META_OCEAN_RENDERING_STEREO_ATTRIBUTE_H
