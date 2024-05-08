/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_ATTRIBUTE_SET_H
#define META_OCEAN_RENDERING_ATTRIBUTE_SET_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Attribute.h"
#include "ocean/rendering/Object.h"
#include "ocean/rendering/ObjectRef.h"

#include <set>

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class AttributeSet;

/**
 * Definition of a smart object reference holding an attribute set.
 * @see SmartObjectRef, AttributeSet.
 * @ingroup rendering
 */
typedef SmartObjectRef<AttributeSet> AttributeSetRef;

/**
 * This class is the base class for all attribute set objects.
 * An attribute set holds several attributes like material, texture or blend attributes.<br>
 * The attributes of this set determine the appearance of a geometry node.<br>
 * Each geometry node can hold at most one attribute set.<br>
 * @see Attribute, Geometry
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT AttributeSet : virtual public Object
{
	protected:

		/**
		 * Definition of a set holding attribute objects.
		 */
		typedef std::set<AttributeRef> Attributes;

	public:

		/**
		 * Returns the number of registered attributes.
		 * @return Number of attributes
		 */
		virtual unsigned int numberAttributes() const;

		/**
		 * Returns an attribute specified by the index.
		 * @param index Index of the attribute to return
		 * @return Requested attribute
		 */
		virtual AttributeRef attribute(const unsigned int index) const;

		/**
		 * Returns the attribute with a specific type.
		 * @param attributeType Attribute type to return
		 * @return Requested attribute, an empty reference if the specific attribute type is not defined
		 */
		virtual AttributeRef attribute(const ObjectType attributeType) const;

		/**
		 * Returns wehether an attribute with a specific type exists in this set.
		 * @param attributeType Attribute type to check
		 * @return True, if at least one attribute with the specified type exists
		 */
		virtual bool hasAttribute(const ObjectType attributeType) const;

		/**
		 * Adds a new attribute to this attribute set.
		 * @param attribute New attribute to add
		 */
		virtual void addAttribute(const AttributeRef& attribute);

		/**
		 * Removes a registered attribute from this attribute ses.
		 * @param attribute Attribute to remove
		 */
		virtual void removeAttribute(const AttributeRef& attribute);

		/**
		 * Removes the first registered attribute having a specified attribute type.
		 * @param attributeType Type of the attribute to be removed
		 * @return True, if an attribute existed and has been removed
		 */
		virtual bool removeAttribute(const ObjectType attributeType);

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new attribute set object.
		 */
		AttributeSet();

		/**
		 * Destructs an attribute set object.
		 */
		~AttributeSet() override;

	protected:

		/// Set holding registered attributes.
		Attributes setAttributes;
};

}

}

#endif // META_OCEAN_RENDERING_ATTRIBUTE_SET_H
