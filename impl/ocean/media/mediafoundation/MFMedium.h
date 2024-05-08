/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_MF_MEDIUM_H
#define META_OCEAN_MEDIA_MF_MEDIUM_H

#include "ocean/media/mediafoundation/MediaFoundation.h"

#include "ocean/base/Signal.h"

#include "ocean/media/Medium.h"

namespace Ocean
{

namespace Media
{

namespace MediaFoundation
{

/**
 * This is the base class for all MediaFoundation mediums.
 * @ingroup mediamf
 */
class OCEAN_MEDIA_MF_EXPORT MFMedium : virtual public Medium
{
	protected:

		/**
		 * This class implements a callback class allowing to determine Media Foundation events.
		 */
		class EventCallback : public IMFAsyncCallback
		{
			public:

				/**
				 * Definition of a callback function without parameter.
				 */
				typedef Callback<void> EventFunction;

				/**
				 * Definition of a callback function with topology as parameter.
				 */
				typedef Callback<void, IMFTopology*> TopologyEventFunction;

				/**
				 * Definition of a callback function with node as parameter.
				 */
				typedef Callback<void, const TOPOID> TopologyNodeEventFunction;

			public:

				/**
				 * Creates a new callback object.
				 * @param eventGenerator Event generator on that this callback object will listen
				 * @param topologySet Callback function for topology-set events
				 * @param sessionStarted Callback function for session-started events
				 * @param sessionStopped Callback function for session-stopped events
				 * @param sessionEnded Callback function for session-ended events
				 * @param frameTypeChanged Callback function for format-changed events
				 */
				EventCallback(IMFMediaEventGenerator& eventGenerator, const TopologyEventFunction& topologySet, const EventFunction& sessionStarted, const EventFunction& sessionStopped, const EventFunction& sessionEnded, const TopologyNodeEventFunction& frameTypeChanged);

				/**
				 * Destructs this object.
				 */
				 virtual ~EventCallback() = default;

				/**
				 * Deactivates the callback.
				 */
				void deactivate();

				/**
				 * Returns whether the media session is closed.
				 * @return True, if so
				 */
				bool isMediaSessionClosed() const;

				/**
				 * Interface query function.
				 * @param riid The identifier of the interface being requested
				 * @param ppvObject The address of a pointer variable that receives the interface pointer requested in the riid parameter
				 * @return This method returns S_OK if the interface is supported
				 */
				HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject) override;

				/**
				 * Increments the reference count for an interface on an object.
				 * @return The method returns the new reference count
				 */
				ULONG STDMETHODCALLTYPE AddRef() override;

				/**
				 * Decrements the reference count for an interface on an object.
				 * @return The method returns the new reference count
				 */
				ULONG STDMETHODCALLTYPE Release() override;

				/**
				 * Provides configuration information to the dispatching thread for a callback.
				 * @param pdwFlags Receives a flag indicating the behavior of the callback object's IMFAsyncCallback::Invoke method
				 * @param pdwQueue Receives the identifier of the work queue on which the callback is dispatched
				 * @return The method returns an HRESULT
				 */
				HRESULT STDMETHODCALLTYPE GetParameters(DWORD* pdwFlags, DWORD* pdwQueue) override;

				/**
				 * Called when an asynchronous operation is completed.
				 * @param pAsyncResult Pointer to the IMFAsyncResult interface
				 * @return The method returns an HRESULT
				 */
				HRESULT STDMETHODCALLTYPE Invoke(IMFAsyncResult *pAsyncResult) override;

			protected:

				/// Event generator on that this callback object will listen.
				IMFMediaEventGenerator& eventGenerator_;

				/// True, if the callback is active.
				bool active_ = true;

				/// Reference counter.
				long referenceCounter_ = 1;

				/// Callback function for topology-set events.
				TopologyEventFunction callbackTopologySet_;

				/// Callback function for session started events.
				EventFunction callbackSessionStarted_;

				/// Callback function for session started events.
				EventFunction callbackSessionStopped_;

				/// Callback function for session ended events.
				EventFunction callbackSessionEnded_;

				/// Callback function for format type changed events.
				TopologyNodeEventFunction callbackFormatTypeChanged_;

				/// True, if the session is closed.
				std::atomic<bool> isMediaSessionClosed_ = false;
		};

		/**
		 * Definition of a scoped object holding a EventCallback object.
		 * The wrapped EventCallback object will be released automatically once the scoped object does not exist anymore.
		 */
		typedef ScopedMediaFoundationObject<EventCallback> ScopedEventCallback;

	public:

		/**
		 * Returns whether the medium is started currently.
		 * @see Medium::isStarted().
		 */
		bool isStarted() const override;

		/**
		 * Returns the start timestamp.
		 * @see FiniteMedium::startTimestamp().
		 */
		Timestamp startTimestamp() const override;

		/**
		 * Returns the pause timestamp.
		 * @see FiniteMedium::pauseTimestamp().
		 */
		Timestamp pauseTimestamp() const override;

		/**
		 * Returns the stop timestamp.
		 * @see FiniteMedium::stopTimestamp().
		 */
		Timestamp stopTimestamp() const override;

	protected:

		/**
		 * Creates a new medium by a given URL.
		 * @param url The URL of the medium
		 */
		explicit MFMedium(const std::string& url);

		/**
		 * Destructs a medium object.
		 */
		~MFMedium() override;

		/**
		 * Starts the medium.
		 * @see Medium::start().
		 */
		bool start() override;

		/**
		 * Pauses the medium.
		 * @see Medium::pause().
		 */
		bool pause() override;

		/**
		 * Stops the medium.
		 * @see Medium::stop().
		 */
		bool stop() override;

		/**
		 * Creates the media session object.
		 * @return True, if succeeded or if the media session exists already
		 */
		bool createMediaSession();

		/**
		 * Releases the media session object.
		 */
		void releaseMediaSession();

		/**
		 * Creates and builds the topology of this object.
		 * @param respectPlaybackTime True, to deliver the media content based on the presentation time; False, to ignore the presentation clock and to deliver the media content as fast as possible
		 * @return True, if succeeded
		 */
		virtual bool createTopology(const bool respectPlaybackTime) = 0;

		/**
		 * Releases the topology.
		 */
		virtual void releaseTopology();

		/**
		 * Creates the media source object.
		 * @return True, if succeeded
		 */
		virtual bool createMediaSource();

		/**
		 * Releases the media source.
		 */
		void releaseMediaSource();

		/**
		 * Starts the media session.
		 * @return True, if succeeded or if the session is already started
		 */
		virtual bool startMediaSession();

		/**
		 * Pauses the media session.
		 * @return True, if succeeded or if the session is already paused
		 */
		virtual bool pauseMediaSession();

		/**
		 * Stops the media session.
		 * @return True, if the succeeded or if the session is already stopped
		 */
		virtual bool stopMediaSession();

		/**
		 * Returns the timestamp of the most recent media sample.
		 * @param timestamp The resulting timestamp of the most recent media sample
		 * @param nextTimestamp Optional resulting timestamp of the next media sample; nullptr if not of interest
		 * @return True, if succeeded
		 */
		virtual bool recentMediaSampleTimestamp(LONGLONG& timestamp, LONGLONG* nextTimestamp) const;

		/**
		 * Topology set event function.
		 * @param topology New topology that has been set
		 */
		virtual void onTopologySet(IMFTopology* topology);

		/**
		 * Session started event function.
		 */
		virtual void onSessionStarted();

		/**
		 * Session stopped event function.
		 */
		virtual void onSessionStopped();

		/**
		 * Session ended event function.
		 */
		virtual void onSessionEnded();

		/**
		 * Format type changed event function.
		 * @param nodeId Id of the topology node that holds the changed format type
		 */
		virtual void onFormatTypeChanged(const TOPOID nodeId);

		/**
		 * Creates the pipeline.
		 * @param respectPlaybackTime True, to deliver the media content based on the presentation time; False, to ignore the presentation clock and to deliver the media content as fast as possible
		 * @return True, if succeeded
		 */
		virtual bool createPipeline(const bool respectPlaybackTime);

		/**
		 * Releases the pipeline.
		 */
		virtual void releasePipeline();

	protected:

		/// Start timestamp.
		Timestamp startTimestamp_;

		/// Pause timestamp.
		Timestamp pauseTimestamp_;

		/// Stop timestamp.
		Timestamp stopTimestamp_;

		/// Media session of this object.
		ScopedIMFMediaSession mediaSession_;

		/// Media topology of this object.
		ScopedIMFTopology topology_;

		/// Media source object.
		ScopedIMFMediaSource mediaSource_;

		/// Event callback object for this medium.
		ScopedEventCallback eventCallback_;

		/// True, to deliver the media content based on the presentation time; False, to ignore the presentation clock and to deliver the media content as fast as possible.
		bool respectPlaybackTime_ = true;
};

}

}

}

#endif // META_OCEAN_MEDIA_MF_MEDIUM_H
