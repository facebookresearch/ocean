/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_DS_SAMPLE_SINK_H
#define META_OCEAN_MEDIA_DS_SAMPLE_SINK_H

#include "ocean/media/directshow/DirectShow.h"
#include "ocean/media/directshow/DSMediaType.h"
#include "ocean/media/directshow/DSObject.h"

#include "ocean/base/Callback.h"
#include "ocean/base/Timestamp.h"

#include "ocean/media/FrameMedium.h"

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
class DSSampleSinkFilter;

/**
 * Definition of a scoped object holding an DSSampleSinkFilter object.
 * The wrapped DSSampleSinkFilter object will be released automatically once the scoped object does not exist anymore.
 * @ingroup mediads
 */
typedef ScopeDirectShowObject<DSSampleSinkFilter> ScopedDSSampleSinkFilter;

/**
 * This class implements a sample sink filter for DirectShow.
 * @ingroup mediads
 */
class OCEAN_MEDIA_DS_EXPORT DSSampleSinkFilter : public CBaseFilter
{
	friend class DSInputPin;

	public:

		/**
		 * Definition of a callback function informing about a new media sample.
		 */
		typedef Callback<void, IMediaSample*, const Timestamp, const Timestamp> SampleCallback;

	protected:

		/**
		 * Unique identifier of the Ocean sample sink filter.
		 * 0xd3d7d4c4, 0xb235, 0x4853, 0x91, 0x08, 0x88, 0xc3, 0x34, 0xa1, 0x9b, 0x43
		 */
		struct __declspec(uuid("{D3D7D4C4-B235-4853-9108-88C334A19B43}")) CLSID_DSOceanSampleSinkFilter;

		/**
		 * This class implements a base input filter.
		 */
		class DSInputPin : public CBaseInputPin
		{
			friend class DSSampleSinkFilter;

			protected:

				/**
				 * Definition of a vector holding media types.
				 */
				typedef std::vector<CMediaType> MediaTypes;

			public:

				/**
				 * Constructor for the input pin.
				 * @param pinName Name of the pin
				 * @param phr Pointer to a result value.
				 * @param filter Pointer to the parent filter.
				 * @param callback New media sample callback function
				 */
				DSInputPin(const std::wstring& pinName, HRESULT* phr, DSSampleSinkFilter* filter, const SampleCallback& callback);

				/**
				 * Destructs a input pin.
				 */
				virtual ~DSInputPin();

				/**
				 * Returns whether the sink respects the media playback time or whether the samples are provided as fast as possible.
				 * @return True, if the media playback time is respected
				 */
				bool respectPlaybackTime() const;

				/**
				 * Specifies whether the media playback time will be respected or whether the samples are provided as fast as possible.
				 * @param state True, to respect the playback time
				 * @return True, if succeeded
				 */
				bool setRespectPlaybackTime(const bool state);

				/**
				 * Adds a new media type of the input pin.
				 * The input pin may support more than one media type.
				 * @param mediaType Media type to set
				 * @return True, if succeeded
				 */
				bool specifyMediaType(const AM_MEDIA_TYPE& mediaType);

			protected:

				/**
				 * Checks if the given media type is supported by the input pin.
				 * @param mediaType Media type to check
				 * @return S_OK if succeeded
				 */
				HRESULT CheckMediaType(const CMediaType* mediaType) override;

				/**
				 * Returns the supported media types of the input pin.
				 * This function is called to connected the input pin with a matching output pin.
				 * @param position Index of the media type.
				 * @param mediaType Media type to receive
				 * @return S_OK if succeeded
				 */
				HRESULT GetMediaType(int position, CMediaType* mediaType) override;

				/**
				 * Receives a media sample from the connected direct show filter.
				 * Here the filter a connected with a video encoder.
				 * @param mediaSample Pointer to a media sample with one encoded frame
				 * @return S_OK if succeeded
				 */
				STDMETHODIMP Receive(IMediaSample* mediaSample) override;

				/**
				 * End of stream event.
				 */
				STDMETHODIMP EndOfStream() override;

			protected:

				/// Pattern media types of this pin
				MediaTypes mediaTypes_;

				/// Callback function informing about a new media sample.
				SampleCallback sampleCallback_;

				/// True, if the media playback time is respected.
				bool respectPlaybackTime_ = true;
		};

		/**
		 * Definition of a scoped object holding an DSInputPin object.
		 * The wrapped DSInputPin object will be released automatically once the scoped object does not exist anymore.
		 * @ingroup mediads
		 */
		typedef ScopeDirectShowObject<DSInputPin> ScopedDSInputPin;

	public:

		/**
		 * Creates a new sample sink filter.
		 * @param filterName Name of the filter
		 * @param callback New media sample callback function
		 */
		DSSampleSinkFilter(const std::wstring& filterName, const SampleCallback& callback);

		/**
		 * Destructs a sample sink filter.
		 */
		~DSSampleSinkFilter() override;

		/**
		 * Adds a new media type of the input pin.
		 * The input pin may support more than one media type.
		 * @param mediaType Media type to set
		 * @return True, if succeeded
		 */
		bool specifyMediaType(const AM_MEDIA_TYPE& mediaType);

		/**
		 * Gets the established media type.
		 * @param dsMediaType The resulting established media type
		 * @return True, if the input pin of the filter is connected
		 */
		bool establishedMediaType(DSMediaType& dsMediaType);

		/**
		 * Returns whether the sink respects the media playback time or whether the samples are provided as fast as possible.
		 * @return True, if the media playback time is respected
		 */
		bool respectPlaybackTime() const;

		/**
		 * Specifies whether the media playback time will be respected or whether the samples are provided as fast as possible.
		 * @param state True, to respect the playback time
		 * @return True, if succeeded
		 */
		bool setRespectPlaybackTime(const bool state);

		/**
		 * Returns the count of pins of this filter.
		 * @return Count of pins
		 */
		int GetPinCount() override;

		/**
		 * Returns a pin given by its index.
		 * @param n Index of the pin
		 * @return Pointer to a pin
		 */
		CBasePin* GetPin(int n) override;

	protected:

		/// Input pin of the filter
		ScopedDSInputPin inputPin_;

		/// Critical section for interfaces
		CCritSec interfaceLock_;
};

}

}

}

#endif // META_OCEAN_MEDIA_DS_SAMPLE_SINK_H
