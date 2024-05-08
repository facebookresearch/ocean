/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_DS_FRAME_MEDIUM_H
#define META_OCEAN_MEDIA_DS_FRAME_MEDIUM_H

#include "ocean/media/directshow/DirectShow.h"
#include "ocean/media/directshow/DSMedium.h"
#include "ocean/media/directshow/DSSampleSinkFilter.h"

#include "ocean/math/AnyCamera.h"

#include "ocean/media/FrameMedium.h"

namespace Ocean
{

namespace Media
{

namespace DirectShow
{

/**
 * This is the base class for all DirectShow frame mediums.
 * @ingroup mediads
 */
class OCEAN_MEDIA_DS_EXPORT DSFrameMedium :
	virtual public DSMedium,
	virtual public FrameMedium
{
	protected:

		/**
		 * Class allowing the sorting of several video media types according their popularity.
		 */
		class DSSortableFrameType : public SortableFrameType
		{
			public:

				/**
				 * Creates a new sortable media type.
				 * The given media type has to be freed after the entire sorting process by the caller.
				 * @param dsMediaType The DirectShow media type
				 * @param frameType Preferable frame type
				 */
				DSSortableFrameType(DSMediaType&& dsMediaType, const MediaFrameType& frameType);

				/**
				 * Returns the DirectShow media type.
				 * @return Media type
				 */
				const AM_MEDIA_TYPE& type() const;

			protected:

				/// DirectShow media type
				DSMediaType dsMediaType_;
		};

		/**
		 * Definition of a vector holding sortable media type objects.
		 */
		typedef std::vector<DSSortableFrameType> DSSortableFrameTypes;

	public:

		/**
		 * Sets the preferred frame dimension.
		 * @see FrameMedium::setPreferredFrameDimension().
		 */
		bool setPreferredFrameDimension(const unsigned int width, const unsigned int height) override;

		/**
		 * Sets the preferred frame pixel format.
		 * @see FrameMedium::setPreferredFramePixelFormat().
		 */
		bool setPreferredFramePixelFormat(const FrameType::PixelFormat format) override;

		/**
		 * Sets the preferred frame frequency in Hz.
		 * @see FrameMedium::setPreferredFrameFrequency().
		 */
		bool setPreferredFrameFrequency(const FrameFrequency frequency) override;

		/**
		 * Extracts the video format of a given DirectShow media type.
		 * @param mediaType DirectShow media type
		 * @param frameType Resulting frame type
		 * @return True, if succeeded
		 */
		static bool extractFrameFormat(const AM_MEDIA_TYPE& mediaType, MediaFrameType& frameType);

		/**
		 * Creates the video format of a given DirectShow media type.
		 * @param mediaType Media type to modify
		 * @param frameType Frame type used for modification
		 * @return True, if succeeded
		 */
		static bool createMediaType(CMediaType& mediaType, const MediaFrameType& frameType);

		/**
		 * Modifies the video format of a given DirectShow media type.
		 * @param mediaType Media type to modify
		 * @param frameType Frame type used for modification
		 * @return True, if succeeded
		 */
		static bool modifyFrameFormat(AM_MEDIA_TYPE& mediaType, const MediaFrameType& frameType);

		/**
		 * Collect all supported frame formats of a given pin.
		 * @param pin The pin to collect the formats for
		 * @param frameTypes Resulting valid and supported frame types
		 * @return True, if succeeded
		 */
		static bool collectFrameFormats(IPin* pin, FrameTypes& frameTypes);

		/**
		 * Converts a Directshow media subtype to a pixel format.
		 * @param mediaSubtype DirectShow media type to convert
		 * @return Resulting pixel format
		 */
		static FrameType::PixelFormat convertMediaSubtype(const GUID& mediaSubtype);

		/**
		 * Returns the pixel origin of a Directshow media subtype.
		 * @param mediaSubtype DirectShow media type to return the pixel origin for
		 * @return Pixel origin
		 */
		static FrameType::PixelOrigin extractPixelOrigin(const GUID& mediaSubtype);

		/**
		 * Converts a pixel format to a DirectShow media subtype.
		 * @param pixelFormat Pixel format
		 * @return DirectShow media type
		 */
		static GUID convertPixelFormat(const FrameType::PixelFormat pixelFormat);

	protected:

		/**
		 * Creates a new frame medium by a given url.
		 * @param url Url of the frame medium
		 */
		explicit DSFrameMedium(const std::string& url);

		/**
		 * Destructs a DSFrameMedium object.
		 */
		~DSFrameMedium() override;

		/**
		 * Creates the frame sink filter.
		 * @return True, if succeeded
		 */
		bool createFrameSampleSinkFilter();

		/**
		 * Creates a new sample sink filter.
		 * @param outputPin Output pin to be connected with the sample sink filter
		 * @param pixelFormat Pixel format of the frame sink filter
		 * @return True, if succeeded
		 */
		bool insertFrameSampleSinkFilter(IPin* outputPin, FrameType::PixelFormat pixelFormat = FrameType::FORMAT_UNDEFINED);

		/**
		 * Releases a the frame sample sink filter.
		 */
		void releaseFrameSampleSinkFilter();

		/**
		 * Called by the sample sink filter if a new sample arrived.
		 * @param sample New sample
		 * @param timestamp The sample timestamp
		 * @param relativeTimestamp Relative timestamp of the sample
		 */
		virtual void onNewSample(IMediaSample* sample, const Timestamp timestamp, const Timestamp relativeTimestamp);

		/**
		 * Specifies whether the media playback time will be respected or whether the media content will be provided as fast as possible.
		 * @see DSMedium::setRespectPlaybackTime().
		 */
		bool setRespectPlaybackTime(const bool respectPlaybackTime) override;

	protected:

		/// DirectShow sample sink filter.
		ScopedDSSampleSinkFilter sampleSinkFilter_;

		/// Frame type of the most recent (upcoming frame) frame.
		FrameType recentFrameType_;

		/// The recent camera profile, if known.
		SharedAnyCamera recentAnyCamera_;

		/// DirecShow media sub type for YUV420, 30323449-0000-0010-8000-00AA00389B71 'I420' == MEDIASUBTYPE_I420
		static constexpr GUID MEDIASUBTYPE_I420 = {0x30323449, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71};

		/// True, to deliver the media content based on the presentation time; False, to ignore the presentation clock and to deliver the media content as fast as possible.
		bool respectPlaybackTime_ = true;
};

}

}

}

#endif // META_OCEAN_MEDIA_DS_FRAME_MEDIUM_H
