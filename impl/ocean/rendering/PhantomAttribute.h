/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_PHANTOM_ATTRIBUTE_H
#define META_OCEAN_RENDERING_PHANTOM_ATTRIBUTE_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Attribute.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class PhantomAttribute;

/**
 * Definition of a smart object reference holding a phantom attribute.
 * @see SmartObjectRef, PhantomAttribute.
 * @ingroup rendering
 */
typedef SmartObjectRef<PhantomAttribute> PhantomAttributeRef;

/**
 * This class is the base class for all phantom attributes.
 * @see Attribute
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT PhantomAttribute : virtual public Attribute
{
	public:

		/**
		 * Definition of different phantom modes.
		 */
		enum PhantomMode
		{
			/// Invalid phantom mode.
			PM_INVALID,
			/// Default phantom mode.
			PM_DEFAULT,
			/// Background color phantom mode.
			PM_COLOR,
			/// Debug phantom mode.
			PM_DEBUG,
			/// Background view phantom mode.
			PM_VIDEO
		};

		/**
		 * Definition of extenced phantom modes.
		 * **TODO** Merge this enum with enum PhantomMode
		 */
		enum ExtendedPhantomMode
		{
			/// Invalid mode.
			EPM_INVALID = 0,
			// The default phantom video mode.
			EPM_VIDEO = PM_VIDEO,
			/// The fast phantom video mode.
			EPM_VIDEO_FAST,
			/// The default phantom video mode with explicit texture coordinates.
			EPM_VIDEO_TEXTURE_COORDINATES,
			/// The fast phantom video mode with explicit texture coordinates.
			EPM_VIDEO_FAST_TEXTURE_COORDINATES,
		};

	public:

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new phantom attribute.
		 */
		PhantomAttribute();

	protected:

		/// Attribute phantom mode.
		unsigned int attributePhantomMode;
};

}

}

#endif // META_OCEAN_RENDERING_PHANTOM_ATTRIBUTE_H
