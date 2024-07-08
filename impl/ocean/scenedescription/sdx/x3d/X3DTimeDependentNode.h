/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_TIME_DEPENDENT_NODE_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_TIME_DEPENDENT_NODE_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DChildNode.h"

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
 * This class implements an abstract x3d time dependent node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT X3DTimeDependentNode :
	virtual public X3DChildNode,
	virtual public SDXUpdateNode
{
	protected:

		/**
		 * Creates an abstract x3d time dependent node.
		 * @param environment Node environment
		 */
		explicit X3DTimeDependentNode(const SDXEnvironment* environment);

		/**
		 * Registers the fields of this node.
		 * @param specification Node specification receiving the field informations
		 */
		void registerFields(NodeSpecification& specification);

		/**
		 * Event function to inform the node that it has been initialized and can apply all internal values to corresponding rendering objects.
		 * @see SDXNode::onInitialize().
		 */
		void onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp) override;

		/**
		 * Explicit changing event function for node fields.
		 * @see X3DNode::onFieldChanging().
		 */
		bool onFieldChanging(const std::string& fieldName, const Field& field) override;

		/**
		 * Event function to inform the node about a changed field.
		 * @see SDXNode::onFieldChanged().
		 */
		void onFieldChanged(const std::string& fieldName)override;

		/**
		 * Update event function.
		 * @see SDXUpdateNode::onUpdate().
		 */
		void onUpdate(const Rendering::ViewRef& view, const Timestamp timestamp) override;

		/**
		 * Starts the node explicitly.
		 * The node must not be active before this call.<br>
		 * All necessary events will be generated.<br>
		 * @param valueTimestamp Timestamp value of optional time fields
		 * @param eventTimestamp Timestamp of the event causing the start
		 */
		virtual void startNode(const Timestamp valueTimestamp, const Timestamp eventTimestamp);

		/**
		 * Pauses the node explicitly.
		 * The node must be active at the moment of this call.<br>
		 * All necessary events will be generated.<br>
		 * @param valueTimestamp Timestamp value of optional time fields
		 * @param eventTimestamp Timestamp of the event causing the start
		 */
		virtual void pauseNode(const Timestamp valueTimestamp, const Timestamp eventTimestamp);

		/**
		 * Resumes the node explicitly.
		 * The node must be active but paused at the moment of this call.<br>
		 * All necessary events will be generated.<br>
		 * @param valueTimestamp Timestamp value of optional time fields
		 * @param eventTimestamp Timestamp of the event causing the start
		 */
		virtual void resumeNode(const Timestamp valueTimestamp, const Timestamp eventTimestamp);

		/**
		 * Stops the node explicitly.
		 * The node must be active before this call.<br>
		 * All necessary events will be generated.<br>
		 * @param valueTimestamp Timestamp value of optional time fields
		 * @param eventTimestamp Timestamp of the event causing the start
		 */
		virtual void stopNode(const Timestamp valueTimestamp, const Timestamp eventTimestamp);

		/**
		 * Event function for node start events.
		 * This function can be used in derivated classes.
		 * @param eventTimestamp Event timestamp causing the starting
		 */
		virtual void onStarted(const Timestamp eventTimestamp) = 0;

		/**
		 * Event function for node pause events.
		 * This function can be used in derivated classes.
		 * @param eventTimestamp Event timestamp causing the pausing
		 */
		virtual void onPaused(const Timestamp eventTimestamp) = 0;

		/**
		 * Event function for node resume events.
		 * This function can be used in derivated classes.
		 * @param eventTimestamp Event timestamp causing the resuming
		 */
		virtual void onResumed(const Timestamp eventTimestamp) = 0;

		/**
		 * Event function for node stop events.
		 * This function can be used in derivated classes.
		 * @param eventTimestamp Event timestamp causing the stopping
		 */
		virtual void onStopped(const Timestamp eventTimestamp) = 0;

		/**
		 * Event function for node update events.
		 * This function must be used in derivated classes.
		 * @param timestamp Update timestamp
		 */
		virtual void onUpdated(const Timestamp timestamp) = 0;

	protected:

		/// Loop field.
		SingleBool loop_;

		/// PauseTime field.
		SingleTime pauseTime_;

		/// ResumeTime field.
		SingleTime resumeTime_;

		/// StartTime field.
		SingleTime startTime_;

		/// StopTime field.
		SingleTime stopTime_;

		/// ElapsedTime field.
		SingleTime elapsedTime_;

		/// IsActive field.
		SingleBool isActive_;

		/// IsPaused field.
		SingleBool isPaused_;

		/// The time this sensor was paused, with range [0, infinity)
		double pausedTime_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_TIME_DEPENDENT_NODE_H
