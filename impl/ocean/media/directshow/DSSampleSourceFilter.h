/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_DS_SOURCE_FILTER_H
#define META_OCEAN_MEDIA_DS_SOURCE_FILTER_H

#include "ocean/media/directshow/DirectShow.h"
#include "ocean/media/directshow/DSObject.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Signal.h"
#include "ocean/base/Timestamp.h"

DISABLE_WARNINGS_BEGIN
	#include <streams.h>
DISABLE_WARNINGS_END

namespace Ocean
{

namespace Media
{

namespace DirectShow
{

// Forward declaration.
class DSSampleSourceFilter;

/**
 * Definition of a scoped object holding an DSSampleSourceFilter object.
 * The wrapped DSSampleSourceFilter object will be released automatically once the scoped object does not exist anymore.
 * @ingroup mediads
 */
typedef ScopeDirectShowObject<DSSampleSourceFilter> ScopedDSSampleSourceFilter;

/**
 * This class implements a DirectShow sample source filter pushing new samples into a DirectShow filter graph.
 * @ingroup mediads
 */
class OCEAN_MEDIA_DS_EXPORT DSSampleSourceFilter : public CSource
{
	public:

		/**
		 * Class identifier of the sample source filter.
		 * 0x2e4e013a, 0x417, 0x4b26, 0x98, 0x69, 0xcd, 0xcd, 0x61, 0xe, 0x2f, 0xc3
		 */
		struct __declspec(uuid("{2E4E013A-0417-4b26-9869-CDCD610E2FC3}")) CLSID_DSOceanSampleSourceFilter;

	protected:

		/**
		 * This class implements an output pin for the sample source filter.
		 */
		class DSOutputPin : public CSourceStream
		{
			public:

				/**
				 * Creates a new output pin.
				 * @param filter Pin owner filter
				 * @param result Returning result value
				 */
				DSOutputPin(DSSampleSourceFilter* filter, HRESULT* result);

				/**
				 * Destructs an output pin.
				 */
				~DSOutputPin() override;

				/**
				 * Returns a pointer to the most recent buffer to be filled immediately and locks it.
				 * Beware: The buffer has to be unlocked by DSOutputPin::unlockBufferToFill() afterwards.
				 * @param buffer Recent buffer to be filled
				 * @param size Available size of the buffer in bytes
				 * @param respectSampleFrequency Flag determining that a buffer will be returned if it is time for a new sample only
				 * @return True, if the buffer is valid
				 */
				bool lockBufferToFill(void*& buffer, size_t& size, const bool respectSampleFrequency = true);

				/**
				 * Unlocks the filled buffer.
				 * Beware: The buffer has to be locked by DSOutputPin::lockBufferToFill() before.
				 * @param size The size of the recently copied buffer in bytes, has to be set at least if the sample size is not constant
				 */
				void unlockBufferToFill(const size_t size = 0);

				/**
				 * Explicitly specifies the media type of this pin.
				 * @param mediaType Media type of this pin to specify
				 * @param frequency Sample frequency in Hz
				 * @return True, if succeeded
				 */
				bool specifyMediaType(const AM_MEDIA_TYPE& mediaType, const double frequency = 0.0);

				/**
				 * Starts the output pin.
				 * @param start The start time
				 * @return Start result
				 */
				HRESULT Run(REFERENCE_TIME start) override;

				/**
				 * Decides the buffer size of one media sample holding one source sample.
				 * @param allocator Buffer allocator
				 * @param properties Allocation properties
				 * @return Allocation result
				 */
				HRESULT DecideBufferSize(IMemAllocator* allocator, ALLOCATOR_PROPERTIES* properties) override;

				/**
				 * Fills the buffer with a new source sample.
				 * This function returns not before the next sample has been filled.
				 * @param sample Media sample receiving the next source sample
				 * @return Fill result
				 */
				HRESULT FillBuffer(IMediaSample* sample) override;

				/**
				 * Returns the media type of this output pin.
				 * @param mediaType Object receiving the media type
				 * @return S_TRUE, if the media type could be returned
				 */
				HRESULT GetMediaType(CMediaType* mediaType) override;

				/**
				 * Informs this pin about a quality change.
				 * @param pSender Information sender
				 * @param q Quality
				 * @return S_OK, if accepted
				 */
				STDMETHODIMP Notify(IBaseFilter * pSender, Quality q) override;

			protected:

				/// Pattern media type of this pin
				CMediaType mediaType_;

				/// Frequency of the source.
				double sampleFrequency_ = 25.0;

				/// Sample duration of the source in 100 nanoseconds.
				REFERENCE_TIME sampleDuration_ = 0;

				/// Run timestamp of the source filter.
				Timestamp runTimestamp_;

				/// Timestamp of the next sample.
				Timestamp nextSampleTimestamp_;

				/// Sample number
				unsigned int sampleNumber_ = 0u;

				/// Recent media sample to be filled.
				IMediaSample* mediaSample_ = nullptr;

				/// Sample signal.
				Signal sampleSignal_;

				/// Source media sample lock.
				Lock lock_;

#ifdef OCEAN_DEBUG
				/// Flag determining whether a given sample buffer is waiting to be filled.
				bool isWaitingForBuffer_ = false;
#endif
		};

		/**
		 * Definition of a scoped object holding an DSOutputPin object.
		 * The wrapped DSOutputPin object will be released automatically once the scoped object does not exist anymore.
		 * @ingroup mediads
		 */
		typedef ScopeDirectShowObject<DSOutputPin> ScopedDSOutputPin;

	public:

		/**
		 * Creates a new DirectShow sample source filter object.
		 * @param unknown IUnknown of delegating object
		 */
		DSSampleSourceFilter(IUnknown* unknown);

		/**
		 * Destructs a sample source filter object.
		 */
		virtual ~DSSampleSourceFilter();

		/**
		 * Returns a pointer to the most recent buffer to be filled immediately and locks it.
		 * Beware: The buffer has to be unlocked by DSSampleSourceFilter::unlockBufferToFill() afterwards.
		 * @param buffer Recent buffer to be filled
		 * @param size Available size of the buffer in bytes
		 * @param respectSampleFrequency Flag determining that a buffer will be returned if it is time for a new sample only
		 * @return True, if the buffer is valid
		 */
		bool lockBufferToFill(void*& buffer, size_t& size, const bool respectSampleFrequency = true);

		/**
		 * Unlocks the filled buffer.
		 * Beware: The buffer has to be locked by DSSampleSourceFilter::lockBufferToFill() before.
		 * @param size The size of the recently copied buffer in bytes, has to be set at least if the sample size is not constant
		 */
		void unlockBufferToFill(const size_t size = 0);

		/**
		 * Sets the sample type of this filter to a frame format.
		 * Beware: Width and height of the frame type must be dividable by four.
		 * @param type Source frame type
		 * @param frequency Source frame frequency in Hz
		 * @return True, if succeeded
		 */
		bool setFormat(const FrameType& type, const double frequency);

		/**
		 * Sets the sample type of this filter to an encoded frame stream.
		 * Beware: Width and height of the frame type must be dividable by four.
		 * @param compression Four cc compression type
		 * @param type Source frame type
		 * @param frequency Source frame frequency in Hz
		 * @return True, if succeeded
		 */
		bool setFormat(const std::string& compression, const FrameType& type, const double frequency);

	protected:

		/**
		 * Returns the DirectShow filter state.
		 * @return Filter state
		 */
		inline FILTER_STATE filterState() const;

	protected:

		/// Source filter output pin
		ScopedDSOutputPin outputPin_;
};

inline FILTER_STATE DSSampleSourceFilter::filterState() const
{
	return m_State;
}

}

}

}

#endif // META_OCEAN_MEDIA_DS_SOURCE_FILTER_H
