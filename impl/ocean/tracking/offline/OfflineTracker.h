/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_OFFLINE_OFFLINE_TRACKER_H
#define META_OCEAN_TRACKING_OFFLINE_OFFLINE_TRACKER_H

#include "ocean/tracking/offline/Offline.h"
#include "ocean/tracking/offline/OfflinePose.h"
#include "ocean/tracking/offline/TrackerEvent.h"

#include "ocean/base/ObjectRef.h"
#include "ocean/base/Thread.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"

namespace Ocean
{

namespace Tracking
{

namespace Offline
{

// Forward declaration.
class OfflineTracker;

/**
 * Definition of an object reference holding an offline tracker object.
 * @ingroup trackingoffline
 */
typedef ObjectRef<OfflineTracker> OfflineTrackerRef;

/**
 * This class is the base class for all offline tracker objects.
 * @see OfflineTrackerRef.
 * @ingroup trackingoffline
 */
class OCEAN_TRACKING_OFFLINE_EXPORT OfflineTracker : protected Thread
{
	friend class EventStackLayer;

	public:

		/**
		 * Definition of individual tracking qualities.
		 */
		enum TrackingQuality
		{
			/// A low tracking quality with high performance.
			TQ_LOW,
			/// A moderate tracking quality with moderate performance.
			TQ_MODERATE,
			/// A high tracking quality with low performance.
			TQ_HIGH,
			/// A very high tracking quality with very low performance.
			TQ_ULTRA,
			/// An insane tracking quality with very low performance.
			TQ_INSANE,
			/// Automatic tracking quality.
			TQ_AUTOMATIC,
			/// Coverage with coarse grid
			TQ_COARSE,
			/// Coverage with fine grid
			TQ_FINE
		};

		/**
		 * Definition of individual abstract camera motion types.
		 */
		enum AbstractMotionType
		{
			/// An unknown abstract camera motion.
			AMT_UNKNOWN,
			/// A pure rotational camera motion.
			AMT_PURE_ROTATIONAL,
			/// A complex camera motion (may include rotational and translational motion).
			AMT_COMPLEX
		};

		/**
		 * Definition of individual camera models.
		 */
		enum CameraModel
		{
			/// Invalid camera model parameter.
			CM_INVALID,
			/// Low quality camera, equivalent to a state-of-the-art webcams.
			CM_LOW_QUALITY,
			/// Medium camera model, equivalent to an amateur camera.
			CM_MEDIUM_QUALITY,
			/// High quality camera, equivalent to a professional cinema camera.
			CM_HIGH_QUALITY
		};

		/**
		 * Definition of a component event callback function.
		 * The first parameter provides the event.
		 */
		typedef Callback<void, const TrackerEvent&> EventCallback;

		/**
		 * Frees the offline tracker object.
		 */
		virtual ~OfflineTracker();

	protected:

		/**
		 * This class implements one layer in a progress event stack allowing to define the start and stop progress value.
		 * The start and stop progress values are relative progress values in relation to the next older layer of the stack.<br>
		 * Thus, by application of individual layers an hierarchy of progress states can be created.<br>
		 */
		class EventStackLayer
		{
			public:

				/**
				 * Creates a new layer object.
				 * @param startProgress The start progress value defined relative to the next older layer of the stack, with range [0, 1]
				 * @param stopProgress The stop progress value defined relative to the next older layer of the stack, with range [startProgress, 1]
				 */
				inline EventStackLayer(const Scalar startProgress, const Scalar stopProgress);

				/**
				 * Returns the relative start progress value of this object.
				 * @return Relative start progress value
				 */
				inline Scalar startProgress() const;

				/**
				 * Returns the relative stop progress value of this object.
				 * @return Relative stop progress value
				 */
				inline Scalar stopProgress() const;

				/**
				 * Returns whether two objects have identical start and stop values.
				 * @param layer The second value to compare
				 * @return True, if so
				 */
				inline bool operator==(const EventStackLayer& layer) const;

			protected:

				/// The relative start progress value of this object.
				Scalar startProgress_ = Scalar(0);

				/// the relative stop progress value of this object.
				Scalar stopProgress_ = Scalar(0);
		};

		/**
		 * This class implements an event stack layer object that pushes the relative progress parameters on the stack during creation and pops the layer if the object is disposed.
		 * Further, the relative progress values of this object can be changed after the creation.<br>
		 */
		class ScopedEventStackLayer : public EventStackLayer
		{
			public:

				/**
				 * Move constructor.
				 * @param scopedLayer Scoped layer to move
				 */
				inline ScopedEventStackLayer(ScopedEventStackLayer&& scopedLayer) noexcept;

				/**
				 * Creates a new object by the given owner of the event stack and two relative progress parameters.
				 * @param owner The owner of the stack in which the progress values are pushed
				 * @param startProgress The start progress value defined relative to the next older layer of the stack, with range [0, 1]
				 * @param stopProgress The stop progress value defined relative to the next older layer of the stack, with range [startProgress, 1]
				 */
				inline ScopedEventStackLayer(OfflineTracker& owner, const Scalar startProgress, const Scalar stopProgress);

				/**
				 * Destructs the scoped layer object and pops the relative progress values from the stack.
				 */
				inline ~ScopedEventStackLayer();

				/**
				 * Releases this scoped layer object explicitly before the scope ends.
				 */
				inline void release();

				/**
				 * Modifies the relative start and stop progress values of this object.
				 * @param startProgress The start progress value defined relative to the next older layer of the stack, with range [0, 1]
				 * @param stopProgress The stop progress value defined relative to the next older layer of the stack, with range [startProgress, 1]
				 */
				inline void modify(const Scalar startProgress, const Scalar stopProgress);

				/**
				 * Modifies the relative start and stop progress values of this object by using the previous stop progress value as new start progress value and taking the new value as new stop progress value.
				 * @param stopProgress The stop progress value defined relative to the next older layer of the stack, with range [stopProgress(), 1]
				 */
				inline void modify(const Scalar stopProgress);

				/**
				 * Move operator.
				 * @param scopedLayer The scoped layer object to move
				 * @return Reference to this object
				 */
				inline ScopedEventStackLayer& operator=(ScopedEventStackLayer&& scopedLayer) noexcept;

			private:

				/**
				 * Copy constructor.
				 */
				ScopedEventStackLayer(const ScopedEventStackLayer&) = delete;

				/**
				 * Assign operator.
				 * @return Reference to this object
				 */
				ScopedEventStackLayer& operator=(const ScopedEventStackLayer&) = delete;

			protected:

				/// The owner of the stack that is associated with this object.
				OfflineTracker* owner_ = nullptr;
		};

		/**
		 * Definition of a vector holding EventStackLayer objects which actually is used to implement a stack of these objects.
		 */
		typedef std::vector<EventStackLayer> EventStack;

		/**
		 * Definition of a vector holding event callback functions.
		 */
		typedef ConcurrentCallbacks<EventCallback> EventCallbacks;

	public:

		/**
		 * Starts the offline tracker.
		 * @return True, if succeeded
		 * @see stop(), finished().
		 */
		virtual bool start();

		/**
		 * Stops the offline tracker.
		 * All poses that have been determined since the start of the tracker will be untouched.<br>
		 * @param timeout if 0, this call is asynchronous and will return immediately. Otherwise wait time in ms or -1 for infinite wait
		 * @return True, if succeeded
		 * @see start().
		 */
		virtual bool stop(const unsigned int timeout = 0u);

		/**
		 * Returns the unique id of this tracker object.
		 * Each tracker has an own unique id allowing to separate individual trackers or their events.
		 * @return Unique tracker id
		 */
		inline unsigned int id() const;

		/**
		 * Returns whether the tracker has been started (and is currently tracking).
		 * @return True, if so
		 */
		virtual bool running() const;

		/**
		 * Returns whether the offline tracker has finished since the last start or has not been started yet.
		 * @return True, if so
		 * @see start().
		 */
		virtual bool finished() const;

		/**
		 * Returns whether the offline tracker has finished and succeeded since the last start or has not been started yet.
		 * @return True, if so
		 * @see finished().
		 */
		virtual bool succeeded() const = 0;

		/**
		 * Returns the currently determined poses of this tracker.
		 * @return Tracker poses
		 */
		virtual OfflinePoses poses() const;

		/**
		 * Returns one pose of this tracker.
		 * @param index The index of the frame for that the pose will be returned
		 * @return Tracker pose
		 */
		virtual OfflinePose pose(const unsigned int index) const;

		/**
		 * Adds a state event callback function.
		 * @param callback The callback function to be added
		 */
		inline void addEventCallback(const EventCallback& callback);

		/**
		 * Removes a state event callback function.
		 * @param callback The callback function to be removed
		 */
		inline void removeEventCallback(const EventCallback& callback);

	protected:

		/**
		 * Creates a new offline tracker object.
		 */
		OfflineTracker();

		/**
		 * Checks whether the camera parameters of a given camera profiles matches to the specified camera model.
		 * @param pinholeCamera The pinhole camera profile that will be checked
		 * @param model The model to that the camera profiles must match
		 * @return True, if so
		 */
		static bool isPlausibleCamera(const PinholeCamera& pinholeCamera, const CameraModel model);

		/**
		 * Updates the tracker process progress value of this tracker.
		 * This tracker stores a stack with EventStackLayer objects defining a hierarchy of individual relative progress ranges.<br>
		 * The given progress value should be independent of any stack layer and should simply give the progress state of e.g. a component.<br>
		 * @param localProgress The local progress value of e.g. a component, with range [0, 1]
		 */
		void updateTrackerProgress(const Scalar localProgress);

		/**
		 * Updates all poses of this tracker and invokes the corresponding state event(s).
		 * @param poses New poses for this tracker
		 */
		void updatePoses(const OfflinePoses& poses);

		/**
		 * Removes irregular poses at the boundary between valid and invalid poses.
		 * A pose is voted as irregular if the rotation angle between two successive frame is larger than the median rotation angle (between successive frame) multiplied by a factor.
		 * @param factor The factor that is multiplied with the media angle
		 */
		void removeIrregularPoses(const Scalar factor = Scalar(5));

		/**
		 * Extrapolates poses at the boundary between valid and invalid poses.
		 * @param number The number of poses that will be extrapolated at each border
		 * @param base The maximal number of poses that are used as interpolation base
		 */
		void extrapolatePoses(const unsigned int number, const unsigned int base);

		/**
		 * Interpolates the pose of a single invalid pose which is enclosed by two valid poses.
		 * @param invalidIndex The index of the (single) invalid pose
		 * @param offlinePoses The offline poses on which the poses are extrapolated
		 */
		static void extrapolateSinglePose(const OfflinePoses::Index invalidIndex, OfflinePoses& offlinePoses);

		/**
		 * Interpolates some poses to the left of a gap with invalid poses.
		 * @param invalidIndex The index of the invalid pose with valid poses on the right
		 * @param number The number of poses that will be extrapolated in the left area
		 * @param base The maximal number of poses that are used as interpolation base
		 * @param offlinePoses The offline poses on which the poses are extrapolated
		 */
		static void extrapolateLeftPoses(const OfflinePoses::Index invalidIndex, const unsigned int number, const unsigned int base, OfflinePoses& offlinePoses);

		/**
		 * Interpolates some poses to the right of a gap with invalid poses.
		 * @param invalidIndex The index of the invalid pose with valid poses on the left
		 * @param number The number of poses that will be extrapolated in the right area
		 * @param base The maximal number of poses that are used as interpolation base
		 * @param offlinePoses The offline poses on which the poses are extrapolated
		 */
		static void extrapolateRightPoses(const OfflinePoses::Index invalidIndex, const unsigned int number, const unsigned int base, OfflinePoses& offlinePoses);

		/**
		 * Interpolates some poses to the left or to the right of a gap with invalid poses.
		 * @param invalidIndex The index of the invalid pose with valid poses
		 * @param number The number of poses that will be extrapolated in the area
		 * @param base The maximal number of poses that are used as interpolation base
		 * @param offlinePoses The offline poses on which the poses are extrapolated
		 */
		static void extrapolateCenterPoses(const OfflinePoses::Index invalidIndex, const unsigned int number, const unsigned int base, OfflinePoses& offlinePoses);

		/**
		 * Returns a reference to the lock for the tracker id counter.
		  * @return The lock for the tracker id counter
		 */
		static Lock& idCounterLock();

	private:

		/**
		 * Pushes a new progress event stack layer on the stack of this tracker.
		 * Do not call this function, this function will be called by the ScopedEventStackLayer object only.
		 * @param layer New layer to be pushed
		 * @see ScopedEventStackLayer
		 */
		void pushProgressEventStackLayer(const EventStackLayer& layer);

		/**
		 * Pops a progress event stack layer from the stack of this tracker.
		 * Do not call this function, this function will be called by the ScopedEventStackLayer object only.
		 * @param layer New layer to be pushed
		 * @see ScopedEventStackLayer
		 */
		void popProgressEventStackLayer(const EventStackLayer& layer);

	protected:

		/// The unique id that identifies this tracker.
		unsigned int id_ = 0u;

		/// Finished state of the tracker.
		bool finished_ = true;

		/// Stop-request state of the tracker, this state should have the same state as the threadShouldStop variable but will be accessible in all trackers.
		bool shouldStop_ = false;

		/// Offline poses of this tracker.
		OfflinePoses offlinePoses_;

		/// State event callback functions.
		EventCallbacks eventCallbacks_;

		/// Tracker lock object.
		mutable Lock lock_;

		/// The stack with progress event layers allowing for a convenient process progress event handling.
		EventStack eventStack_;

		/// The lock for the progress event layer stack.
		Lock eventStackLock_;

		/// The previous process progress.
		Scalar previousProcessProgress_ = Scalar(0);

		/// The tracker id counter.
		static unsigned int idCounter_;
};

inline OfflineTracker::EventStackLayer::EventStackLayer(const Scalar startProgress, const Scalar stopProgress) :
	startProgress_(startProgress),
	stopProgress_(stopProgress)
{
	ocean_assert(startProgress <= stopProgress);
	ocean_assert(startProgress >= Scalar(0) && stopProgress <= Scalar(1));
}

inline Scalar OfflineTracker::EventStackLayer::startProgress() const
{
	return startProgress_;
}

inline Scalar OfflineTracker::EventStackLayer::stopProgress() const
{
	return stopProgress_;
}

inline bool OfflineTracker::EventStackLayer::operator==(const EventStackLayer& layer) const
{
	return startProgress_ == layer.startProgress_ && stopProgress_ == layer.stopProgress_;
}

inline OfflineTracker::ScopedEventStackLayer::ScopedEventStackLayer(ScopedEventStackLayer&& scopedLayer) noexcept :
	EventStackLayer(scopedLayer.startProgress_, scopedLayer.stopProgress_),
	owner_(scopedLayer.owner_)
{
	scopedLayer.owner_ = nullptr;
}

inline OfflineTracker::ScopedEventStackLayer::ScopedEventStackLayer(OfflineTracker& owner, const Scalar startProgress, const Scalar stopProgress) :
	EventStackLayer(startProgress, stopProgress),
	owner_(&owner)
{
	owner_->pushProgressEventStackLayer(*this);
}

inline OfflineTracker::ScopedEventStackLayer::~ScopedEventStackLayer()
{
	release();
}

inline void OfflineTracker::ScopedEventStackLayer::release()
{
	if (owner_)
	{
		owner_->popProgressEventStackLayer(*this);
		owner_ = nullptr;
	}
}

void OfflineTracker::ScopedEventStackLayer::modify(const Scalar startProgress, const Scalar stopProgress)
{
	ocean_assert(owner_);

	if (owner_)
	{
		ocean_assert(startProgress <= stopProgress);
		ocean_assert(startProgress >= Scalar(0) && stopProgress <= Scalar(1));

		owner_->popProgressEventStackLayer(*this);

		startProgress_ = startProgress;
		stopProgress_ = stopProgress;

		owner_->pushProgressEventStackLayer(*this);
	}
}

inline void OfflineTracker::ScopedEventStackLayer::modify(const Scalar stopProgress)
{
	modify(stopProgress_, stopProgress);
}
inline OfflineTracker::ScopedEventStackLayer& OfflineTracker::ScopedEventStackLayer::operator=(ScopedEventStackLayer&& scopedLayer) noexcept
{
	if (this != &scopedLayer)
	{
		release();

		owner_ = scopedLayer.owner_;
		scopedLayer.owner_ = nullptr;
	}

	return *this;
}

inline unsigned int OfflineTracker::id() const
{
	return id_;
}

inline void OfflineTracker::addEventCallback(const EventCallback& callback)
{
	eventCallbacks_.addCallback(callback);
}

inline void OfflineTracker::removeEventCallback(const EventCallback& callback)
{
	eventCallbacks_.removeCallback(callback);
}

}

}

}

#endif // META_OCEAN_TRACKING_OFFLINE_OFFLINE_TRACKER_H
