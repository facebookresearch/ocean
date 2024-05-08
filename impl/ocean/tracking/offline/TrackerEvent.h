/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_OFFLINE_TRACKER_EVENT_H
#define META_OCEAN_TRACKING_OFFLINE_TRACKER_EVENT_H

#include "ocean/tracking/offline/Offline.h"
#include "ocean/tracking/offline/OfflinePose.h"

#include "ocean/base/Event.h"
#include "ocean/base/SmartObjectRef.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Plane3.h"

namespace Ocean
{

namespace Tracking
{

namespace Offline
{

/**
 * Definition of individual tracker event types.
 * @ingroup trackingoffline
 */
enum EventType
{
	/// An invalid event type.
	ET_INVALID = 0,
	/// A component event.
	ET_COMPONENT,
	/// A progress event.
	ET_PROGRESS,
	/// A state event for changed states.
	ET_STATE
};

/**
 * This class implements the base class for all tracker events.
 * @ingroup trackingoffline
 */
class OCEAN_TRACKING_OFFLINE_EXPORT TrackerEvent : public Event
{
	public:

		/**
		 * Returns the unique id of the tracker that sent this event.
		 * @return Unique tracker id
		 */
		inline unsigned int trackerId() const;

		/**
		 * Returns the type name of this event.
		 * @return Event type name
		 */
		std::string typeName() const;

		/**
		 * Returns the sub-type name of this event object.
		 * @return Event sub-type name
		 */
		virtual std::string subtypeName() const = 0;

	protected:

		/**
		 * Creates a new tracker event object.
		 * @param type The type of the event
		 * @param subtype The subtype of the event
		 * @param trackerId Id of the tracker that is sending this event
		 */
		TrackerEvent(const unsigned int type, const unsigned int subtype, const unsigned int trackerId);

	protected:

		/// The unique tracker id of the tracker that sent this event.
		unsigned int trackerId_ = 0u;
};

/**
 * Definition of a smart object reference for tracker event.
 * @see TrackerEvent.
 * @ingroup trackingoffline
 */
typedef SmartObjectRef<TrackerEvent, Event> TrackerEventRef;

/**
 * Definition of a scoped event for tracker events.
 * @ingroup trackingoffline
 */
typedef ScopedEvent<TrackerEvent> ScopedEvent;

/**
 * This class implements the base class for all component events.
 * Component events are sent whenever an individual component is started, finished or if the component fails.
 * @ingroup trackingoffline
 */
class OCEAN_TRACKING_OFFLINE_EXPORT ComponentEvent : public TrackerEvent
{
	public:

		/**
		 * Definition of individual component event sub-types.
		 */
		enum ComponentEventSubType
		{
			/// An invalid event sub-type.
			CEST_INVALID = 0,
			/// An tracker process sub-type event.
			CEST_TRACKER_PROCESS,
			/// An analysis sub-type event.
			CEST_ANALYSIS,
			/// A tracking sub-type event.
			CEST_TRACKING
		};

		/**
		 * Definition of individual component states.
		 */
		enum ComponentState
		{
			/// The component started.
			CS_STARTED,
			/// The component finished.
			CS_FINISHED,
			/// The component failed.
			CS_FAILED,
			/// The component broke due to an external request.
			CS_BROKE
		};

		/**
		 * Returns the sub-type name of this event object.
		 * @see TrackerEvent::subtypeName().
		 */
		std::string subtypeName() const override;

		/**
		 * Returns the state of the component event.
		 * @return Component state
		 */
		inline ComponentState state() const;

		/**
		 * Returns the state of this event.
		 * @return Component state
		 */
		std::string stateName() const;

		/**
		 * Sets or changes the state of this component event.
		 * @param state New state to be set
		 */
		inline void setState(const ComponentState state);

	protected:

		/**
		 * Creates a new component event object.
		 * @param subtype The subtype of the event
		 * @param trackerId Unique id of the tracker that sent this event
		 * @param state The state of the component that sent this event
		 */
		inline ComponentEvent(const unsigned int subtype, const unsigned int trackerId, const ComponentState state);

	protected:

		/// The state of the component that sent this event.
		ComponentState eventState;
};

/**
 * Definition of a smart object reference for component events.
 * @see ComponentEvent.
 * @ingroup trackingoffline
 */
typedef SmartObjectRef<ComponentEvent, Event> ComponentEventRef;

/**
 * This class implements a component event for the entire tracker process.
 * @ingroup trackingoffline
 */
class OCEAN_TRACKING_OFFLINE_EXPORT TrackerProcessComponentEvent : public ComponentEvent
{
	public:

		/**
		 * Creates a new event object.
		 * @param trackerId Unique id of the tracker that sent this event
		 * @param state The state of the component that sent this event
		 */
		inline TrackerProcessComponentEvent(const unsigned int trackerId, const ComponentState state);

		/**
		 * Creates a new event reference object.
		 * @param trackerId Unique id of the tracker that sent this event
		 * @param state The state of the component that sent this event
		 * @return The new event object
		 */
		static inline EventRef createEvent(const unsigned int trackerId, const ComponentState state);
};

/**
 * Definition of a smart object reference for tracker process events.
 * @see TrackerProcessComponentEvent.
 * @ingroup trackingoffline
 */
typedef SmartObjectRef<TrackerProcessComponentEvent, Event> TrackerProcessComponentEventRef;

/**
 * This class implements a component event for the analysis components
 * @ingroup trackingoffline
 */
class OCEAN_TRACKING_OFFLINE_EXPORT AnalysisComponentEvent : public ComponentEvent
{
	public:

		/**
		 * Creates a new event object.
		 * @param trackerId Unique id of the tracker that sent this event
		 * @param state The state of the component that sent this event
		 */
		inline AnalysisComponentEvent(const unsigned int trackerId, const ComponentState state);

		/**
		 * Creates a new event reference object.
		 * @param trackerId Unique id of the tracker that sent this event
		 * @param state The state of the component that sent this event
		 * @return The new event object
		 */
		static inline EventRef createEvent(const unsigned int trackerId, const ComponentState state);
};

/**
 * Definition of a smart object reference for analysis component events.
 * @see AnalysisComponentEvent.
 * @ingroup trackingoffline
 */
typedef SmartObjectRef<AnalysisComponentEvent, Event> AnalysisComponentEventRef;

/**
 * This class implements a component event for the tracking components
 * @ingroup trackingoffline
 */
class OCEAN_TRACKING_OFFLINE_EXPORT TrackingComponentEvent : public ComponentEvent
{
	public:

		/**
		 * Creates a new event object.
		 * @param trackerId Unique id of the tracker that sent this event
		 * @param state The state of the component that sent this event
		 */
		inline TrackingComponentEvent(const unsigned int trackerId, const ComponentState state);

		/**
		 * Creates a new event reference object.
		 * @param trackerId Unique id of the tracker that sent this event
		 * @param state The state of the component that sent this event
		 * @return The new event object
		 */
		static inline EventRef createEvent(const unsigned int trackerId, const ComponentState state);
};

/**
 * Definition of a smart object reference for tracking component events.
 * @see TrackingComponentEvent.
 * @ingroup trackingoffline
 */
typedef SmartObjectRef<TrackingComponentEvent, Event> TrackingComponentEventRef;

/**
 * This class implements the base class for all progress events.
 * Progress events are sent whenever the progress of a component changes.<br>
 * @ingroup trackingoffline
 */
class OCEAN_TRACKING_OFFLINE_EXPORT ProgressEvent : public TrackerEvent
{
	public:

		/**
		 * Definition of individual progress event sub-types.
		 */
		enum ProgressEventSubType
		{
			/// An invalid event sub-type.
			PEST_INVALID = 0,
			/// An tracker process sub-type event.
			PEST_TRACKER_PROCESS,
			/// An analysis sub-type event.
			PEST_ANALYSIS,
			/// A tracking sub-type event.
			PEST_TRACKING
		};

	public:

		/**
		 * Returns the sub-type name of this event object.
		 * @see TrackerEvent::subtypeName().
		 */
		std::string subtypeName() const override;

		/**
		 * Returns the progress of a component in percent.
		 * @return Component progress, with range [0, 100]
		 */
		inline unsigned int componentProgress() const;

	protected:

		/**
		 * Creates a new progress event object.
		 * @param subtype The subtype of the event
		 * @param trackerId Unique id of the tracker that sent this event
		 * @param componentProgress The progress of the component that sent this event, with range [0, 100]
		 */
		inline ProgressEvent(const unsigned int subtype, const unsigned int trackerId, const unsigned int componentProgress);

	protected:

		/// The component progress in percent.
		unsigned int componentProgress_ = 0u;
};

/**
 * Definition of a smart object reference for progress events.
 * @see ProgressEvent.
 * @ingroup trackingoffline
 */
typedef SmartObjectRef<ProgressEvent, Event> ProgressEventRef;

/**
 * This class implements a progress event for tracker processes.
 * @ingroup trackingoffline
 */
class OCEAN_TRACKING_OFFLINE_EXPORT TrackerProcessProgressEvent : public ProgressEvent
{
	public:

		/**
		 * Creates a new progress event object.
		 * @param trackerId Unique id of the tracker that sent this event
		 * @param componentProgress Progress of the sending component in percent, with range [0, 100]
		 */
		inline TrackerProcessProgressEvent(const unsigned int trackerId, const unsigned int componentProgress);

		/**
		 * Creates a new event reference object.
		 * @param trackerId Unique id of the tracker that sent this event
		 * @param componentProgress Progress of the sending component in percent, with range [0, 100]
		 * @return The new event object
		 */
		static inline EventRef createEvent(const unsigned int trackerId, const unsigned int componentProgress);
};

/**
 * Definition of a smart object reference for tracker process progress events.
 * @see TrackerProcessProgressEvent.
 * @ingroup trackingoffline
 */
typedef SmartObjectRef<TrackerProcessProgressEvent, Event> TrackerProcessProgressEventRef;

/**
 * This class implements a progress event for analysis components.
 * @ingroup trackingoffline
 */
class OCEAN_TRACKING_OFFLINE_EXPORT AnalysisProgressEvent : public ProgressEvent
{
	public:

		/**
		 * Creates a new progress event object.
		 * @param trackerId Unique id of the tracker that sent this event
		 * @param componentProgress Progress of the sending component in percent, with range [0, 100]
		 */
		inline AnalysisProgressEvent(const unsigned int trackerId, const unsigned int componentProgress);

		/**
		 * Creates a new event reference object.
		 * @param trackerId Unique id of the tracker that sent this event
		 * @param componentProgress Progress of the sending component in percent, with range [0, 100]
		 * @return The new event object
		 */
		static inline EventRef createEvent(const unsigned int trackerId, const unsigned int componentProgress);
};

/**
 * Definition of a smart object reference for analysis progress events.
 * @see AnalysisProgressEvent.
 * @ingroup trackingoffline
 */
typedef SmartObjectRef<AnalysisProgressEvent, Event> AnalysisProgressEventRef;

/**
 * This class implements a progress event for tracking components.
 * @ingroup trackingoffline
 */
class OCEAN_TRACKING_OFFLINE_EXPORT TrackingProgressEvent : public ProgressEvent
{
	public:

		/**
		 * Creates a new progress event object.
		 * @param trackerId Unique id of the tracker that sent this event
		 * @param componentProgress Progress of the sending component in percent, with range [0, 100]
		 */
		inline TrackingProgressEvent(const unsigned int trackerId, const unsigned int componentProgress);

		/**
		 * Creates a new event reference object.
		 * @param trackerId Unique id of the tracker that sent this event
		 * @param componentProgress Progress of the sending component in percent, with range [0, 100]
		 * @return The new event object
		 */
		static inline EventRef createEvent(const unsigned int trackerId, const unsigned int componentProgress);
};

/**
 * Definition of a smart object reference for tracking progress events.
 * @see TrackingProgressEvent.
 * @ingroup trackingoffline
 */
typedef SmartObjectRef<TrackingProgressEvent, Event> TrackingProgressEventRef;

/**
 * This class is the base class for all state events.
 * @ingroup trackingoffline
 */
class OCEAN_TRACKING_OFFLINE_EXPORT StateEvent : public TrackerEvent
{
	public:

		/**
		 * Definition of individual state event sub-types.
		 */
		enum StateEventSubType
		{
			/// An invalid event sub-type.
			SEST_INVALID = 0,
			/// A camera calibration sub-type event.
			SEST_CAMERA_CALIBRATION,
			/// A tracker transformation sub-type event.
			SEST_TRACKER_TRANSFORMATION,
			/// A tracker pose sub-type event.
			SEST_TRACKER_POSE,
			/// A tracker poses sub-type event.
			SEST_TRACKER_POSES,
			/// A tracker plane sub-type event.
			SEST_TRACKER_PLANE
		};

	public:

		/**
		 * Returns the sub-type name of this event object.
		 * @see TrackerEvent::subtypeName().
		 */
		std::string subtypeName() const override;

	protected:

		/**
		 * Creates a new state event object.
		 * @param subtype The subtype of the event
		 * @param trackerId Unique id of the tracker that sent this event
		 */
		inline explicit StateEvent(const unsigned int subtype, const unsigned int trackerId);
};

/**
 * Definition of a smart object reference for state events.
 * @see StateEvent.
 * @ingroup trackingoffline
 */
typedef SmartObjectRef<StateEvent, Event> StateEventRef;

/**
 * This class implements a camera state event that provides a camera profile.
 * @ingroup trackingoffline
 */
class OCEAN_TRACKING_OFFLINE_EXPORT CameraCalibrationStateEvent : public StateEvent
{
	public:

		/**
		 * Creates a new camera state event object.
		 * @param trackerId Unique id of the tracker that sent this event
		 * @param pinholeCamera The pinhole camera profile of the sender
		 */
		explicit inline CameraCalibrationStateEvent(const unsigned int trackerId, const PinholeCamera& pinholeCamera);

		/**
		 * Returns the camera profile of this event.
		 * @return The pinhole camera profile
		 */
		inline const PinholeCamera& camera() const;

		/**
		 * Creates a new event reference object.
		 * @param trackerId Unique id of the tracker that sent this event
		 * @param pinholeCamera The pinhole camera profile of the sender
		 * @return The new event object
		 */
		static inline EventRef createEvent(const unsigned int trackerId, const PinholeCamera& pinholeCamera);

	protected:

		/// The camera profile of this event.
		const PinholeCamera camera_;
};

/**
 * Definition of a smart object reference for camera calibration state events.
 * @see CameraCalibrationStateEvent.
 * @ingroup trackingoffline
 */
typedef SmartObjectRef<CameraCalibrationStateEvent, Event> CameraCalibrationStateEventRef;

/**
 * This class implements a tracker pose state event that provides a tracker pose.
 * @ingroup trackingoffline
 */
class OCEAN_TRACKING_OFFLINE_EXPORT TrackerPoseStateEvent : public StateEvent
{
	public:

		/**
		 * Creates a new pose state event object.
		 * @param trackerId Unique id of the tracker that sent this event
		 * @param pose Offline pose of the sender
		 */
		explicit inline TrackerPoseStateEvent(const unsigned int trackerId, const OfflinePose& pose);

		/**
		 * Returns the pose of this event object.
		 * @return Event pose
		 */
		inline const OfflinePose& pose() const;

		/**
		 * Creates a new event reference object.
		 * @param trackerId Unique id of the tracker that sent this event
		 * @param pose Offline pose of the sender
		 * @return The new event object
		 */
		static inline EventRef createEvent(const unsigned int trackerId, const OfflinePose& pose);

	protected:

		/// The pose of this event.
		OfflinePose pose_;
};

/**
 * Definition of a smart object reference for pose state events.
 * @see TrackerPoseStateEvent.
 * @ingroup trackingoffline
 */
typedef SmartObjectRef<TrackerPoseStateEvent, Event> TrackerPoseStateEventRef;

/**
 * This class implements a tracker poses state event that provides the all determined tracker poses.
 * @ingroup trackingoffline
 */
class OCEAN_TRACKING_OFFLINE_EXPORT TrackerPosesStateEvent : public StateEvent
{
	public:

		/**
		 * Creates a new poses state event object.
		 * @param trackerId Unique id of the tracker that sent this event
		 * @param poses Offline poses of the sender that will be copied
		 */
		explicit inline TrackerPosesStateEvent(const unsigned int trackerId, const OfflinePoses& poses);

		/**
		 * Returns the poses of this object.
		 * @return Event poses
		 */
		inline const OfflinePoses& poses() const;

		/**
		 * Creates a new event reference object.
		 * @param trackerId Unique id of the tracker that sent this event
		 * @param poses Offline poses of the sender that will be copied
		 * @return The new event object
		 */
		static inline EventRef createEvent(const unsigned int trackerId, const OfflinePoses& poses);

	protected:

		/// The poses of this event.
		const OfflinePoses poses_;
};

/**
 * Definition of a smart object reference for tracker poses state events.
 * @see TrackerPosesStateEvent.
 * @ingroup trackingoffline
 */
typedef SmartObjectRef<TrackerPosesStateEvent, Event> TrackerPosesStateEventRef;

/**
 * This class implements an tracker transformation state event that provides the transformation between world coordinate system and tracker coordinate system.
 * @ingroup trackingoffline
 */
class OCEAN_TRACKING_OFFLINE_EXPORT TrackerTransformationStateEvent : public StateEvent
{
	public:

		/**
		 * Creates a new tracker transformation event object.
		 * @param trackerId Unique id of the tracker that sent this event
		 * @param transformation Tracker transformation of the sender
		 * @param isSuggestion Flag to indicate whether the transformation is a suggestion
		 */
		explicit inline TrackerTransformationStateEvent(const unsigned int trackerId, const HomogenousMatrix4& transformation, const bool isSuggestion = false);

		/**
		 * Returns the tracker transformation of this object.
		 * @return Event transformation
		 */
		inline const HomogenousMatrix4& transformation() const;

		/**
		 * Returns whether the tracker transformation of this object is a suggestion.
		 * @return Suggestion flag
		 */
		inline bool isTransformationASuggestion() const;

		/**
		 * Creates a new event reference object.
		 * @param trackerId Unique id of the tracker that sent this event
		 * @param transformation Tracker transformation of the sender
		 * @param isSuggestion Flag to indicate whether the transformation is a suggestion
		 * @return The new event object
		 */
		static inline EventRef createEvent(const unsigned int trackerId, const HomogenousMatrix4& transformation, const bool isSuggestion = false);

	protected:

		/// The transformation of this event.
		const HomogenousMatrix4 transformation_;

		/// Flag to indicate whether the transformation of this event is a suggestion.
		const bool transformationSuggestion_;
};

/**
 * Definition of a smart object reference for tracker transformation state events.
 * @see TrackerTransformationStateEvent.
 * @ingroup trackingoffline
 */
typedef SmartObjectRef<TrackerTransformationStateEvent, Event> TrackerTransformationStateEventRef;

/**
 * This class implements a tracker plane state event that provides the detected plane of a tracker.
 * @ingroup trackingoffline
 */
class OCEAN_TRACKING_OFFLINE_EXPORT TrackerPlaneStateEvent : public StateEvent
{
	public:

		/**
		 * Creates a new plane state event object.
		 * @param trackerId Unique id of the tracker that sent this event
		 * @param plane Tracker plane of the sender
		 */
		explicit inline TrackerPlaneStateEvent(const unsigned int trackerId, const Plane3& plane);

		/**
		 * Returns the plane of this object.
		 * @return Event plane
		 */
		inline const Plane3& plane() const;

		/**
		 * Creates a new event reference object.
		 * @param trackerId Unique id of the tracker that sent this event
		 * @param plane Tracker plane of the sender
		 * @return The new event object
		 */
		static inline EventRef createEvent(const unsigned int trackerId, const Plane3& plane);

	protected:

		/// The plane of this event.
		const Plane3 plane_;
};

/**
 * Definition of a smart object reference for tracker plane state events.
 * @see TrackerPlaneStateEvent.
 * @ingroup trackingoffline
 */
typedef SmartObjectRef<TrackerPlaneStateEvent, Event> TrackerPlaneStateEventRef;

inline unsigned int TrackerEvent::trackerId() const
{
	return trackerId_;
}

inline ComponentEvent::ComponentEvent(const unsigned int subtype, const unsigned int trackerId, const ComponentState state) :
	TrackerEvent(ET_COMPONENT, subtype, trackerId),
	eventState(state)
{
	// nothing to do here
}

inline ComponentEvent::ComponentState ComponentEvent::state() const
{
	return eventState;
}

inline void ComponentEvent::setState(const ComponentState state)
{
	eventState = state;
}

inline TrackerProcessComponentEvent::TrackerProcessComponentEvent(const unsigned int trackerId, const ComponentState state) :
	ComponentEvent(CEST_TRACKER_PROCESS, trackerId, state)
{
	// nothing to do here
}

inline EventRef TrackerProcessComponentEvent::createEvent(const unsigned int trackerId, const ComponentState state)
{
	return EventRef(new TrackerProcessComponentEvent(trackerId, state));
}

inline AnalysisComponentEvent::AnalysisComponentEvent(const unsigned int trackerId, const ComponentState state) :
	ComponentEvent(CEST_ANALYSIS, trackerId, state)
{
	// nothing to do here
}

inline EventRef AnalysisComponentEvent::createEvent(const unsigned int trackerId, const ComponentState state)
{
	return EventRef(new AnalysisComponentEvent(trackerId, state));
}

inline TrackingComponentEvent::TrackingComponentEvent(const unsigned int trackerId, const ComponentState state) :
	ComponentEvent(CEST_TRACKING, trackerId, state)
{
	// nothing to do here
}

inline EventRef TrackingComponentEvent::createEvent(const unsigned int trackerId, const ComponentState state)
{
	return EventRef(new TrackingComponentEvent(trackerId, state));
}

inline ProgressEvent::ProgressEvent(const unsigned int subtype, const unsigned int trackerId, const unsigned int componentProgress) :
	TrackerEvent(ET_PROGRESS, subtype, trackerId),
	componentProgress_(componentProgress)
{
	ocean_assert(componentProgress <= 100u);
}

inline unsigned int ProgressEvent::componentProgress() const
{
	return componentProgress_;
}

inline TrackerProcessProgressEvent::TrackerProcessProgressEvent(const unsigned int trackerId, const unsigned int componentProgress) :
	ProgressEvent(PEST_TRACKER_PROCESS, trackerId, componentProgress)
{
	// nothing to do here
}

inline EventRef TrackerProcessProgressEvent::createEvent(const unsigned int trackerId, const unsigned int componentProgress)
{
	return EventRef(new TrackerProcessProgressEvent(trackerId, componentProgress));
}

inline AnalysisProgressEvent::AnalysisProgressEvent(const unsigned int trackerId, const unsigned int componentProgress) :
	ProgressEvent(PEST_ANALYSIS, trackerId, componentProgress)
{
	// nothing to do here
}

inline EventRef AnalysisProgressEvent::createEvent(const unsigned int trackerId, const unsigned int componentProgress)
{
	return EventRef(new AnalysisProgressEvent(trackerId, componentProgress));
}

inline TrackingProgressEvent::TrackingProgressEvent(const unsigned int trackerId, const unsigned int componentProgress) :
	ProgressEvent(PEST_TRACKING, trackerId, componentProgress)
{
	// nothing to do here
}

inline EventRef TrackingProgressEvent::createEvent(const unsigned int trackerId, const unsigned int componentProgress)
{
	return EventRef(new TrackingProgressEvent(trackerId, componentProgress));
}

inline StateEvent::StateEvent(const unsigned int subtype, const unsigned int trackerId) :
	TrackerEvent(ET_STATE, subtype, trackerId)
{
	// nothing to do here
}

inline CameraCalibrationStateEvent::CameraCalibrationStateEvent(const unsigned int trackerId, const PinholeCamera& pinholeCamera) :
	StateEvent(SEST_CAMERA_CALIBRATION, trackerId),
	camera_(pinholeCamera)
{
	// nothing to do here
}

inline const PinholeCamera& CameraCalibrationStateEvent::camera() const
{
	return camera_;
}

inline EventRef CameraCalibrationStateEvent::createEvent(const unsigned int trackerId, const PinholeCamera& pinholeCamera)
{
	return EventRef(new CameraCalibrationStateEvent(trackerId, pinholeCamera));
}

inline TrackerPoseStateEvent::TrackerPoseStateEvent(const unsigned int trackerId, const OfflinePose& pose) :
	StateEvent(SEST_TRACKER_POSE, trackerId),
	pose_(pose)
{
	// nothing to do here
}

inline const OfflinePose& TrackerPoseStateEvent::pose() const
{
	return pose_;
}

inline EventRef TrackerPoseStateEvent::createEvent(const unsigned int trackerId, const OfflinePose& pose)
{
	return EventRef(new TrackerPoseStateEvent(trackerId, pose));
}

inline TrackerPosesStateEvent::TrackerPosesStateEvent(const unsigned int trackerId, const OfflinePoses& poses) :
	StateEvent(SEST_TRACKER_POSES, trackerId),
	poses_(poses)
{
	// nothing to do here
}

inline const OfflinePoses& TrackerPosesStateEvent::poses() const
{
	return poses_;
}

inline EventRef TrackerPosesStateEvent::createEvent(const unsigned int trackerId, const OfflinePoses& poses)
{
	return EventRef(new TrackerPosesStateEvent(trackerId, poses));
}

inline TrackerTransformationStateEvent::TrackerTransformationStateEvent(const unsigned int trackerId, const HomogenousMatrix4& transformation, const bool isSuggestion) :
	StateEvent(SEST_TRACKER_TRANSFORMATION, trackerId),
	transformation_(transformation),
	transformationSuggestion_(isSuggestion)
{
	// nothing to do here
}

inline const HomogenousMatrix4& TrackerTransformationStateEvent::transformation() const
{
	return transformation_;
}

inline bool TrackerTransformationStateEvent::isTransformationASuggestion() const
{
	return transformationSuggestion_;
}

inline EventRef TrackerTransformationStateEvent::createEvent(const unsigned int trackerId, const HomogenousMatrix4& transformation, const bool isSuggestion)
{
	return EventRef(new TrackerTransformationStateEvent(trackerId, transformation, isSuggestion));
}

inline TrackerPlaneStateEvent::TrackerPlaneStateEvent(const unsigned int trackerId, const Plane3& plane) :
	StateEvent(SEST_TRACKER_PLANE, trackerId),
	plane_(plane)
{
	// nothing to do here
}

inline const Plane3& TrackerPlaneStateEvent::plane() const
{
	return plane_;
}

inline EventRef TrackerPlaneStateEvent::createEvent(const unsigned int trackerId, const Plane3& plane)
{
	return EventRef(new TrackerPlaneStateEvent(trackerId, plane));
}

}

}

}

#endif // META_OCEAN_TRACKING_OFFLINE_TRACKER_EVENT_H
