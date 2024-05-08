/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_TRACKER_TRANSFORM_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_TRACKER_TRANSFORM_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DGroupingNode.h"

#include "ocean/devices/Tracker.h"

#include "ocean/scenedescription/SDXUpdateNode.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements an ocean specific X3D node to allow specific tracker system transformations.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT TrackerTransform :
	virtual public X3DGroupingNode,
	virtual public SDXUpdateNode
{
	public:

		/**
		 * Creates a new ocean tracker transform node.
		 * @param environment Node environment
		 */
		explicit TrackerTransform(const SDXEnvironment* environment);

	protected:

		/**
		 * Destructs an ocean tracker transform node.
		 */
		~TrackerTransform() override;

		/**
		 * Specifies the node type and the fields of this node.
		 * @return Unique node specification of this node
		 */
		NodeSpecification specifyNode();

		/**
		 * Event function to inform the node that it has been initialized and can apply all internal values to corresponding rendering objects.
		 * @see SDXNode::onInitialize().
		 */
		void onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp) override;

		/**
		 * Event function to inform the node about a changed field.
		 * @see SDXNode::onFieldChanged().
		 */
		void onFieldChanged(const std::string& fieldName) override;

		/**
		 * Pre update event function.
		 * @see SDXUpdateNode::onPreUpdate().
		 */
		Timestamp onPreUpdate(const Rendering::ViewRef& view, const Timestamp timestamp) override;

		/**
		 * Event function of object ids.
		 * @param trackerSender The sender of the event, must be valid
		 * @param found True, if the object ids were found; False if the object ids were lost
		 * @param objectIds The ids of the object which are found or lost, at least one
		 * @param timestamp The timestamp of the event
		 */
		void onTrackerObjectEvent(const Devices::Tracker* trackerSender, const bool found, const Devices::Tracker::ObjectIdSet& objectIds, const Timestamp& timestamp);

		/**
		 * Returns the address of the most derived object.
		 * @see Node::objectAddress().
		 */
		size_t objectAddress() const override;

	protected:

		/// True, if the tracker is an inside-out tracker; False, if the tracker is an outside-in tracker.
		SingleBool insideOut_;

		/// The name of the tracker to be used, the first valid name will be used.
		MultiString tracker_;

		/// The tracker's input.
		MultiString trackerInput_;

		/// True, if the tracker needs multiple inputs; False, if the tracker needs a single input.
		SingleBool multiInput_;

		/// The name of the object to be tracked.
		SingleString object_;

		/// The size of the object to be tracked in 3D space.
		SingleVector3 objectSize_;

		/// True, to keep the child nodes always visible; False, to keep the child nodes only visible when the tracker provides a valid pose (e.g., when actively tracking).
		SingleBool alwaysVisible_;

		/// The tracker which is actually used for tracking.
		Devices::TrackerRef trackerRef_;

		/// Tracker object id.
		Devices::Tracker::ObjectId objectId_;

		/// The subscription object for tracker object events.
		Devices::Tracker::TrackerObjectEventSubscription trackerObjectEventSubscription_;

		/// True, if the tracker object is visible; False, otherwise.
		bool objectVisible_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_TRACKER_TRANSFORM_H
