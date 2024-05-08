/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_DYNAMIC_OBJECT_H
#define META_OCEAN_RENDERING_DYNAMIC_OBJECT_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Object.h"
#include "ocean/rendering/View.h"

#include "ocean/base/Timestamp.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class DynamicObject;

/**
 * Definition of a smart object reference holding a dynamic object.
 * @see SmartObjectRef, DynamicObject.
 * @ingroup rendering
 */
typedef SmartObjectRef<DynamicObject> DynamicObjectRef;


/**
 * This class is the base class for all dynamic scene graph objects.<br>
 * The class holds an internal update function which is called by the framebuffer.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT DynamicObject : virtual public Object
{
	friend class Engine;

	protected:

		/**
		 * Creates a new dynamic object.
		 */
		DynamicObject();

		/**
		 * Destructs a dynamic object.
		 */
		~DynamicObject() override;

		/**
		 * Registers this dynamic object at the framebuffer update queue.
		 */
		void registerDynamicUpdateObject();

		/**
		 * Unregisteres this dynamic object at the frambuffer update queue.
		 */
		void unregisterDynamicUpdateObject();

		/**
		 * Update function called by the framebuffer.
		 * @param view Associated view
		 * @param timestamp Update timestamp
		 */
		virtual void onDynamicUpdate(const ViewRef& view, const Timestamp timestamp) = 0;
};

}

}

#endif // META_OCEAN_RENDERING_DYNAMIC_OBJECT_H
