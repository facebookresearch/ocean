/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_MF_SAMPLE_GRABBER_H
#define META_OCEAN_MEDIA_MF_SAMPLE_GRABBER_H

#include "ocean/media/mediafoundation/MediaFoundation.h"

#include "ocean/base/Callback.h"

namespace Ocean
{

namespace Media
{

namespace MediaFoundation
{

// Forward declaration.
class SampleGrabber;

/**
 * Definition of a scoped object holding a SampleGrabber object.
 * The wrapped SampleGrabber object will be released automatically once the scoped object does not exist anymore.
 * @ingroup mediamf
 */
typedef ScopedMediaFoundationObject<SampleGrabber> ScopedSampleGrabber;

/**
 * This class implements a callback interface.
 * @ingroup mediamf
 */
class SampleGrabber final : public IMFSampleGrabberSinkCallback
{
	public:

		/**
		 * Definition of a callback for samples.
		 */
		typedef Callback<void, const GUID&, const unsigned int, const long long, const long long, const void*, const unsigned int> SampleCallback;

	public:

		/**
		 * Destructs this object.
		 */
		virtual ~SampleGrabber() = default;

		/**
		 * Deactivates the callback.
		 */
		void deactivate();

		/**
		 * Returns the time of the most recent sample.
		 * @return The most recent sample time
		 */
		LONGLONG recentSampleTime() const;

		/**
		 * Interface query function.
		 * @param iid The identifier of the interface being requested
		 * @param ppv The address of a pointer variable that receives the interface pointer requested in the riid parameter
		 * @return This method returns S_OK if the interface is supported
		 */
		STDMETHODIMP QueryInterface(REFIID iid, void** ppv) override;

		/**
		 * Increments the reference count for an interface on an object.
		 * @return The method returns the new reference count
		 */
		STDMETHODIMP_(ULONG) AddRef() override;

		/**
		 * Decrements the reference count for an interface on an object.
		 * @return The method returns the new reference count
		 */
		STDMETHODIMP_(ULONG) Release() override;

		/**
		 * Called when the presentation clock starts.
		 * @param hnsSystemTime The system time when the clock started, in 100-nanosecond units
		 * @param llClockStartOffset The new starting time for the clock, in 100-nanosecond units
		 * @return If this method succeeds, it returns S_OK
		 */
		STDMETHODIMP OnClockStart(MFTIME hnsSystemTime, LONGLONG llClockStartOffset) override;

		/**
		 * Called when the presentation clock stops.
		 * @param hnsSystemTime The system time when the clock stopped, in 100-nanosecond units
		 * @return If this method succeeds, it returns S_OK
		 */
		STDMETHODIMP OnClockStop(MFTIME hnsSystemTime) override;

		/**
		 * Called when the presentation clock pauses.
		 * @param hnsSystemTime The system time when the clock was paused, in 100-nanosecond units
		 * @return If this method succeeds, it returns S_OK
		 */
		STDMETHODIMP OnClockPause(MFTIME hnsSystemTime) override;

		/**
		 * Called when the presentation clock restarts from the same position while paused.
		 * @param hnsSystemTime The system time when the clock restarted, in 100-nanosecond units
		 * @return If this method succeeds, it returns S_OK
		 */
		STDMETHODIMP OnClockRestart(MFTIME hnsSystemTime) override;

		/**
		 * Called when the rate changes on the presentation clock.
		 * @param hnsSystemTime The system time when the rate was set, in 100-nanosecond units
		 * @param flRate The new rate, as a multiplier of the normal playback rate
		 * @return If this method succeeds, it returns S_OK
		 */
		STDMETHODIMP OnClockSetRate(MFTIME hnsSystemTime, float flRate) override;

		/**
		 * Called when the presentation clock is set on the sample-grabber sink.
		 * @param pClock Pointer to the presentation clock's IMFPresentationClock interface
		 * @return If this method succeeds, it returns S_OK
		 */
		STDMETHODIMP OnSetPresentationClock(IMFPresentationClock* pClock) override;

		/**
		 * Called when the sample-grabber sink receives a new media sample.
		 * @param guidMajorMediaType The major type that specifies the format of the data
		 * @param dwSampleFlags Reserved
		 * @param llSampleTime The presentation time for this sample, in 100-nanosecond units
		 * @param llSampleDuration The duration of the sample, in 100-nanosecond units
		 * @param pSampleBuffer A pointer to a buffer that contains the sample data
		 * @param dwSampleSize Size of the pSampleBuffer buffer, in bytes
		 */
		STDMETHODIMP OnProcessSample(REFGUID guidMajorMediaType, DWORD dwSampleFlags, LONGLONG llSampleTime, LONGLONG llSampleDuration, const BYTE * pSampleBuffer, DWORD dwSampleSize) override;

		/**
		 * Called when the sample-grabber sink is shut down.
		 * @return If this method succeeds, it returns S_OK
		 */
		STDMETHODIMP OnShutdown() override;

		/**
		 * Creates a new instance of the sample grabber.
		 * @param sampleCallback Sample callback function
		 * @return Resulting sample grabber
		 */
		static ScopedSampleGrabber createInstance(const SampleCallback& sampleCallback);

	private:

		/**
		 * Default constructor.
		 * @param callback Sample callback function
		 */
		SampleGrabber(const SampleCallback& callback);

	private:

		/// The reference counter.
		long referenceCounter_ = 1;

		/// Sample callback function.
		SampleCallback sampleCallback_;

		/// True if the callback is active.
		bool active_ = true;

		/// The time of the most recent sample.
		LONGLONG recentSampleTime_ = LONGLONG(-1);
};

}

}

}

#endif // META_OCEAN_MEDIA_MF_SAMPLE_GRABBER_H
