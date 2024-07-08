/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_UPDATE_NODE_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_UPDATE_NODE_H

#include "ocean/scenedescription/SceneDescription.h"
#include "ocean/scenedescription/SDXNode.h"

namespace Ocean
{

namespace SceneDescription
{

/**
 * This class implements the base class for all nodes needing update calls regularly.
 * @ingroup scenedescription
 */
class OCEAN_SCENEDESCRIPTION_EXPORT SDXUpdateNode : virtual public SDXNode
{
	friend class Manager;

	public:

		/**
		 * Creates a new update node and registers it at the scene description manager.
		 * @param environment Node environment
		 */
		SDXUpdateNode(const SDXEnvironment* environment);

		/**
		 * Destructs an update node and unregisters it from the scene description manager.
		 */
		~SDXUpdateNode() override;

	protected:

		/**
		 * Pre update event function.
		 * A derivated object must re-implement this function to receive pre-update events.
		 * @param view Rendering view to be used for rendering
		 * @param timestamp Preferred update timestamp
		 * @return Actually used update timestamp
		 */
		virtual Timestamp onPreUpdate(const Rendering::ViewRef& view, const Timestamp timestamp);

		/**
		 * Update event function.
		 * A derivated object must re-implement this function to receive update events.
		 * @param view Rendering view to be used for rendering
		 * @param timestamp Preferred update timestamp
		 */
		virtual void onUpdate(const Rendering::ViewRef& view, const Timestamp timestamp);
};

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_UPDATE_NODE_H
