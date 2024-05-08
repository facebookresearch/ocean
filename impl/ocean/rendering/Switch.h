/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_SWITCH_H
#define META_OCEAN_RENDERING_SWITCH_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Group.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class Switch;

/**
 * Definition of a smart object reference holding a switch.
 * @see SmartObjectRef, Switch.
 * @ingroup rendering
 */
typedef SmartObjectRef<Switch> SwitchRef;

/**
 * This class is the base class for all switch nodes.
 * A switch node holds several sub nodes like a group. However, at most one of the sub nodes is visible.<br>
 * An index is used to define which sub node is visible.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT Switch : virtual public Group
{
	public:

		/**
		 * Definitio of an invalid index.
		 */
		static constexpr Index32 invalidIndex = Index32(-1);

	public:

		/**
		 * Returns the index of the active node.
		 * @return Index of active node
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual Index32 activeNode() const;

		/**
		 * Sets the index of the active node.
		 * @param index Index of the active node
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setActiveNode(const Index32 index);

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new switch object.
		 */
		Switch();

		/**
		 * Destructs a switch object.
		 */
		~Switch() override;
};

}

}

#endif // META_OCEAN_RENDERING_SWITCH_H
