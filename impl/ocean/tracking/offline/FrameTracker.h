/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_OFFLINE_FRAME_TRACKER_H
#define META_OCEAN_TRACKING_OFFLINE_FRAME_TRACKER_H

#include "ocean/tracking/offline/Offline.h"
#include "ocean/tracking/offline/OfflineTracker.h"

#include "ocean/base/SmartObjectRef.h"

#include "ocean/cv/FrameProviderInterface.h"
#include "ocean/cv/FramePyramid.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"

namespace Ocean
{

namespace Tracking
{

namespace Offline
{

// Forward declaration.
class FrameTracker;

/**
 * Definition of a smart object reference holding a FrameTracker object.
 * @see FrameTracker.
 * @ingroup trackingoffline
 */
typedef SmartObjectRef<FrameTracker, OfflineTracker> FrameTrackerRef;

/**
 * This class implements the base class for all visual offline tracker using frames to provide the tracking data.
 * @ingroup trackingoffline
 */
class OCEAN_TRACKING_OFFLINE_EXPORT FrameTracker : virtual public OfflineTracker
{
	friend class FrameTrackerComponent;

	protected:

		/**
		 * This class implements the base class for all components of a frame tracker.
		 */
		class TrackerComponent
		{
			protected:

				/**
				 * Definition of individual results for the component iterations.
				 */
				enum IterationResult
				{
					/// The iteration succeeded and thus the component proceeds with the next frame.
					IR_SUCCEEDED,
					/// The iteration failed and thus also the component fails.
					IR_FAILED,
					/// The iteration finished and thus also the component finishes.
					IR_FINISHED
				};

			public:

				/**
				 * Creates a new component object.
				 * @param parent The parent tracker that invokes this component
				 */
				TrackerComponent(FrameTracker& parent);

				/**
				 * Destructs a tracker component object.
				 */
				virtual ~TrackerComponent();

				/**
				 * Invokes the component.
				 * The component starts at the initial frame index and proceeds frames with increasing indices, afterwards the components restarts at the initial frame index and proceeds frames with decreasing indices
				 * @param startEvent Event that is invoked at the beginning of the component
				 * @param breakEvent Event that is invoked if the component breaks
				 * @param finishEvent Event that is invoked if the component finishes
				 * @param lowerFrameIndex Index of the frame defining the lower (including) tracking boundary
				 * @param initialFrameIndex Index of the frame at that the tracking process will start
				 * @param upperFrameIndex Index of the frame defining the upper (including) tracking boundary
				 * @return True, if the component succeeded
				 */
				virtual bool invoke(const TrackerEvent& startEvent, TrackerEvent& breakEvent, TrackerEvent& finishEvent, const unsigned int lowerFrameIndex, const unsigned int initialFrameIndex, const unsigned int upperFrameIndex);

			protected:

				/**
				 * Component start event function.
				 */
				virtual bool onStart(const unsigned int lowerFrameIndex, const unsigned int initialFrameIndex, const unsigned int upperFrameIndex);

				/**
				 * Component increasing start event function.
				 * This function is executed before the increasing component iterations are applied.
				 * @return True, if the start has succeeded
				 */
				virtual bool onStartIncreasing();

				/**
				 * Component decreasing start event function.
				 * This function is executed before the decreasing component iterations are applied.
				 * @return True, if the start has succeeded
				 */
				virtual bool onStartDecreasing();

				/**
				 * Iteration setup event function.
				 * This function is executed before an iteration is applied
				 * @return True, if the setup has succeeded
				 */
				virtual bool onSetupIteration(const unsigned int index);

				/**
				 * Applies one component step.
				 * @param previousIndex Index of the previous frame that has been handled in the previous iteration, -1 for the first iteration
				 * @param currentIndex Index of the current frame that is handled in this iteration
				 * @param iteration Index of the current iteration
				 * @param maximalIterations Number of maximal iterations that will be applied
				 * @return Returns the iteration result
				 */
				virtual IterationResult onFrame(const unsigned int previousIndex, const unsigned int currentIndex, const unsigned int iteration, const unsigned int maximalIterations) = 0;

				/**
				 * Component stop event function.
				 * This function is executed after the component iterations have been applied.
				 * @param lowerFrameIndex Index of the frame defining the lower (including) tracking boundary
				 * @param initialFrameIndex Index of the frame at that the tracking process will start
				 * @param upperFrameIndex Index of the frame defining the upper (including) tracking boundary
				 * @return True, if the start has succeeded
				 */
				virtual bool onStop(const unsigned int lowerFrameIndex, const unsigned int initialFrameIndex, const unsigned int upperFrameIndex);

			protected:

				/// Parent tracker object that invokes this component.
				FrameTracker& parent_;
		};

		/**
		 * This class implements the base class for all components of a frame tracker.
		 */
		class FrameTrackerComponent : public TrackerComponent
		{
			public:

				/**
				 * Creates a new component object.
				 * @param tracker The tracker object for that this component will be executed
				 */
				FrameTrackerComponent(FrameTracker& tracker);

				/**
				 * Destructor
				 */
				inline ~FrameTrackerComponent() override;

			protected:

				/**
				 * Component decreasing start event function.
				 * @see TrackerComponent::onStartDecreasing().
				 */
				bool onStartDecreasing() override;

				/**
				 * Iteration setup event function.
				 * @see TrackerComponent::onSetupIteration().
				 */
				bool onSetupIteration(const unsigned int index) override;

			protected:

				/// Frame that has been created in the previous component iteration.
				Frame previousFrame_;

				/// Frame that has been created for the current component iteration.
				Frame currentFrame_;
		};

		/**
		 * This class implements the base class for all components of a frame tracker.
		 */
		class FramePyramidTrackerComponent : public TrackerComponent
		{
			public:

				/**
				 * Creates a new component object.
				 * @param tracker The tracker object for that this component will be executed
				 */
				FramePyramidTrackerComponent(FrameTracker& tracker);

				/**
				 * Destructor
				 */
				inline ~FramePyramidTrackerComponent() override;

			protected:

				/**
				 * Component increasing start event function.
				 * @see TrackerComponent::onStartIncreasing().
				 */
				bool onStartIncreasing() override;

				/**
				 * Component decreasing start event function.
				 * @see TrackerComponent::onStartDecreasing().
				 */
				bool onStartDecreasing() override;

				/**
				 * Iteration setup event function.
				 * @see TrackerComponent::onSetupIteration().
				 */
				bool onSetupIteration(const unsigned int index) override;

			protected:

				/// Frame pyramid that has been created in the previous component iteration.
				CV::FramePyramid previousFramePyramid_;

				/// Frame pyramid that has been created for the current component iteration.
				CV::FramePyramid currentFramePyramid_;

				/// Number of pyramid layers that should be created in each pyramid.
				unsigned int framePyramidLayers_ = (unsigned int)(-1);
		};

	public:

		/**
		 * Returns whether the offline tracker has finished and succeeded since the last start or has not been started yet.
		 * @return True, if so
		 * @see finished().
		 */
		bool succeeded() const override;

		/**
		 * Returns the camera object this is applied by this tracker.
		 * @return The pinhole camera object
		 */
		inline const PinholeCamera& camera() const;

		/**
		 * Returns the camera precision of this tracker.
		 * @return The pinhole camera precision, in squared pixel errors, -1 if unknown
		 */
		inline Scalar cameraPrecision() const;

		/**
		 * Starts the offline tracker.
		 * Ensure that a frame provider has been set before.<br>
		 * @return True, if succeeded
		 * @see stop(), finished(), setFrameProvider().
		 */
		bool start() override;

		/**
		 * Sets the frame provider interface that allows to request the individual frames for the offline tracker.
		 * @param frameProviderInterface Frame provider interface to be set
		 * @return True, if succeeded, returns false if the interface has been set already
		 */
		virtual bool setFrameProviderInterface(const CV::FrameProviderInterfaceRef& frameProviderInterface);

		/**
		 * Sets the frame range of this frame tracker.
		 * The range specify a subset of the entire frames that are investigated during tracking.<br>
		 * The given parameters should match the number of available frames provided by the frame provider.<br>
		 * If the frame provider provides less frames than specified by the range parameters, the parameters will be adjusted automatically.<br>
		 * The range cannot be adjusted if the tracker is active.<br>
		 * @param lowerFrameIndex Index of the first frame that will be investigated, with range [0, infinity)
		 * @param upperFrameIndex Index of the last frame that will be investigated, with range [lowerFrameIndex, infinity)
		 * @param startFrameIndex Index of the start frame at which the tracking will start, with range [lowerFrameIndex, upperFrameIndex], or -1 to use no specific start frame
		 * @return True, if succeeded
		 * @see setFrameProviderInterface()
		 */
		virtual bool setTrackingFrameRange(const unsigned int lowerFrameIndex, const unsigned int upperFrameIndex, const unsigned int startFrameIndex = (unsigned int)(-1));

		/**
		 * Sets a camera object that will be used by this tracker.
		 * @param pinholeCamera The pinhole camera object to be used
		 * @param cameraPrecision The precision of the given camera object, in squared pixel errors, should be -1 if unknown
		 * @param optimizeCamera True, to take the given camera object as rough estimation only
		 * @return True, if succeeded
		 */
		virtual bool setCamera(const PinholeCamera& pinholeCamera, const Scalar cameraPrecision, const bool optimizeCamera);

	protected:

		/**
		 * Creates a new offline frame tracker object.
		 */
		FrameTracker() = default;

		/**
		 * Creates a new offline frame tracker object.
		 * @param frameProviderInterface Frame provider interface to be set
		 */
		inline FrameTracker(const CV::FrameProviderInterfaceRef& frameProviderInterface);

		/**
		 * Thread run function.
		 */
		void threadRun() override;

		/**
		 * Frame tracker run function.
		 * @param frameType Frame type of the tracking frames
		 * @return True, if the tracking succeeded
		 */
		virtual bool applyFrameTracking(const FrameType& frameType) = 0;

		/**
		 * Applies the initialization of the frame tracker.
		 * The function initializes the frame provider interface and waits until a valid frame type is known.
		 * @return The frame type of the frame provider interface, an invalid frame type if the initialization fails
		 */
		FrameType applyFrameTrackingInitialization();

		/**
		 * Updates the camera of this tracker and invokes the corresponding state event(s).
		 * @param pinholeCamera The new pinhole camera for this tracker
		 */
		virtual void updateCamera(const PinholeCamera& pinholeCamera);

	protected:

		/// Frame provider interface that allows to request the individual frames for this offline tracker.
		CV::FrameProviderInterfaceRef frameProviderInterface_;

		/// Index of the lower frame range.
		unsigned int lowerFrameIndex_ = (unsigned int)(-1);

		/// Index of the frame at that the tracking will start, or -1 if no specific frame is specified
		unsigned int startFrameIndex_ = (unsigned int)(-1);

		/// Index of the upper frame range.
		unsigned int upperFrameIndex_ = (unsigned int)(-1);

		/// The camera object of this tracker.
		PinholeCamera camera_;

		/// The precision of the camera object in squared pixel errors.
		Scalar cameraPrecision_ = Scalar(-1);

		/// True, if the camera needs to be optimized during the tracking process.
		bool optimizeCamera_ = true;

		/// Succeeded state of the tracker.
		bool succeeded_ = true;
};

inline FrameTracker::FrameTrackerComponent::~FrameTrackerComponent()
{
	// Nothing else to do.
}

inline FrameTracker::FramePyramidTrackerComponent::~FramePyramidTrackerComponent()
{
	// Nothing else to do.
}

inline FrameTracker::FrameTracker(const CV::FrameProviderInterfaceRef& frameProviderInterface) :
	OfflineTracker(),
	frameProviderInterface_(frameProviderInterface)
{
	// nothing to do here
}

inline const PinholeCamera& FrameTracker::camera() const
{
	return camera_;
}

inline Scalar FrameTracker::cameraPrecision() const
{
	return cameraPrecision_;
}

}

}

}

#endif // META_OCEAN_TRACKING_OFFLINE_FRAME_TRACKER_H
