/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_ATTRIBUTE_H
#define META_OCEAN_RENDERING_ATTRIBUTE_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Object.h"
#include "ocean/rendering/ObjectRef.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class Attribute;

/**
 * Definition of a smart object reference holding an attribute.
 * @see SmartObjectRef, Attribute.
 * @ingroup rendering
 */
typedef SmartObjectRef<Attribute> AttributeRef;

/**
 * This class is the base class for all rendering attribute objects.
 * Attribute objects determine the appearance of geometry nodes like e.g. a material attribute.<br>
 * It's possible to combine several different attributes to an attribute set and assign that set to a geometry.<br>
 * @see AttributeSet, BlendAttribute, Material, PrimitiveAttribute, Textures.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT Attribute : virtual public Object
{
	public:

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new attribute object.
		 */
		Attribute();

		/**
		 * Destructs an attribute object.
		 */
		~Attribute() override;
};

}

}

#endif // META_OCEAN_RENDERING_ATTRIBUTE_H
