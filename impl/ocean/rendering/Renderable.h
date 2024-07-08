/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_RENDERABLE_H
#define META_OCEAN_RENDERING_RENDERABLE_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Object.h"
#include "ocean/rendering/ObjectRef.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class Renderable;

/**
 * Definition of a smart object reference holding a renderable node.
 * @see SmartObjectRef, Renderable.
 * @ingroup rendering
 */
typedef SmartObjectRef<Renderable> RenderableRef;

/**
 * This class is the base class for all renderable objects.
 * Renderable objects can be combined with different appearance attributes and then added to a geometry node.<br>
 * @see Geometry, AttributeSet
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT Renderable : virtual public Object
{
	public:

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new renderable object.
		 */
		Renderable();

		/**
		 * Destructs a renderable object.
		 */
		~Renderable() override;
};

}

}

#endif // META_OCEAN_RENDERING_RENDERABLE_H
