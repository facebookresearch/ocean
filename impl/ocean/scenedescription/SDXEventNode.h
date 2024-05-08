/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_EVENT_NODE_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_EVENT_NODE_H

#include "ocean/scenedescription/SceneDescription.h"
#include "ocean/scenedescription/SDXNode.h"

namespace Ocean
{

namespace SceneDescription
{

/**
 * This class implements the base class for all nodes receiving event calls regularly.
 * @ingroup scenedescription
 */
class OCEAN_SCENEDESCRIPTION_EXPORT SDXEventNode : virtual public SDXNode
{
	friend class Manager;

	public:

		/**
		 * Creates a new event node and registers it at the scene description manager.
		 * @param environment Node environment
		 */
		SDXEventNode(const SDXEnvironment* environment);

		/**
		 * Destructs an event node and unregisters it from the scene description manager.
		 */
		~SDXEventNode() override;

	protected:

		/**
		 * Mouse event function.
		 * @param button Device type
		 * @param buttonEvent Type of the event
		 * @param screenPosition 2D screen position
		 * @param objectPosition 3D object position
		 * @param objectId Id of a rendering object associated with the event
		 * @param timestamp Event timestamp
		 */
		virtual void onMouse(const ButtonType button, const ButtonEvent buttonEvent, const Vector2& screenPosition, const Vector3& objectPosition, const Rendering::ObjectId objectId, const Timestamp timestamp) = 0;

		/**
		 * Key event function.
		 * @param key Key type
		 * @param buttonEvent Type of the event
		 * @param objectId Id of a rendering object associated with the event
		 * @param timestamp Event timestamp
		 */
		virtual void onKey(const int key, const ButtonEvent buttonEvent, const Rendering::ObjectId objectId, const Timestamp timestamp) = 0;
};

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_EVENT_NODE_H
