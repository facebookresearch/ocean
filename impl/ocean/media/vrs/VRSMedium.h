// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_MEDIA_VRS_MEDIUM_H
#define META_OCEAN_MEDIA_VRS_MEDIUM_H

#include "ocean/media/vrs/VRS.h"

#include "ocean/media/Medium.h"

#include <vrs/RecordFileReader.h>

namespace Ocean
{

namespace Media
{

namespace VRS
{

/**
 * This is the base class for all VRS mediums.
 * @ingroup mediavrs
 */
class OCEAN_MEDIA_VRS_EXPORT VRSMedium : virtual public Medium
{
	public:

		/**
		 * Returns whether the medium is started currently.
		 * @see Medium::isStarted()
		 */
		bool isStarted() const override;

		/**
		 * Returns the start timestamp.
		 * @return Start timestamp
		 */
		Timestamp startTimestamp() const override;

		/**
		 * Returns the pause timestamp.
		 * @return Pause timestamp
		 */
		Timestamp pauseTimestamp() const override;

		/**
		 * Returns the stop timestamp.
		 * @return Stop timestamp
		 */
		Timestamp stopTimestamp() const override;

	protected:

		/**
		 * Creates a new medium by a given url.
		 * @param url Url of the medium
		 */
		explicit VRSMedium(const std::string& url);

		/**
		 * Destructs a VRSMedium object.
		 */
		~VRSMedium() override;

		/**
		 * Specifies whether the media playback time will be respected or whether the media content will be provided as fast as possible.
		 * @param respectPlaybackTime True, to deliver the media content based on the presentation time; False, to ignore the presentation clock and to deliver the media content as fast as possible
		 */
		virtual bool setRespectPlaybackTime(const bool respectPlaybackTime);

		/**
		 * Returns the id of a stream for which the name is known.
		 * @param recordFileReader The reader from which the stream will be returned
		 * @param name The name of the stream for which the id will be returned
		 * @return The id of the stream, an invalid stream if the name does not match
		 */
		static vrs::StreamId findStreamId(const vrs::RecordFileReader& recordFileReader, const std::string& name);

		/**
		 * Returns the timestamp of the first data record for a given stream.
		 * @param recordFileReader The reader holding the stream
		 * @param streamId The id of the stream
		 * @return The timestamp of the first record, if existing, otherwise an invalid timestamp
		 */
		static Timestamp determineFirstFrameTimestamp(const vrs::RecordFileReader& recordFileReader, const vrs::StreamId& streamId);

		/**
		 * Returns the timestamp of the last data record for a given stream.
		 * @param recordFileReader The reader holding the stream
		 * @param streamId The id of the stream
		 * @return The timestamp of the last record, if existing, otherwise an invalid timestamp
		 */
		static Timestamp determineLastFrameTimestamp(const vrs::RecordFileReader& recordFileReader, const vrs::StreamId& streamId);

		/**
		 * Returns the number of data records (frames) a given stream has.
		 * @param recordFileReader The reader holding the stream
		 * @param streamId The id of the stream
		 * @return The number of frames, with range [0, infinity)
		 */
		static size_t determineNumberFrames(const vrs::RecordFileReader& recordFileReader, const vrs::StreamId& streamId);

		/**
		 * Finds the best matching data record info out of a set of record infos based on a desired timestamp.
		 * @param recordInfos The record infos
		 * @param timestamp The desired timestamp
		 * @param perfectMatch Optional resulting whether the resulting index has a perfect match
		 * @return The index of the best matching record info, with range [0, recordInfos.size())
		 */
		static size_t findDataRecordInfoForTimestamp(const std::vector<const vrs::IndexRecord::RecordInfo*>& recordInfos, const Timestamp& timestamp, bool* perfectMatch = nullptr);

	protected:

		/// The vrs file which is used for this medium object.
		std::string vrsFilename_;

		/// The name of the stream (id) which is used for this medium object.
		std::string streamIdName_;

		/// Start timestamp
		Timestamp startTimestamp_;

		/// Pause timestamp
		Timestamp pauseTimestamp_;

		/// Stop timestamp
		Timestamp stopTimestamp_;

		/// The speed of the medium, 1 means normal speed, 2 means two times faster than normal speed, with range (0, infinity), 1 by default
		float speed_ = 1.0f;

		/// The VRS file reader for records.
		std::unique_ptr<vrs::RecordFileReader> recordFileReader_;

		/// The id of the stream to be used.
		vrs::StreamId streamId_;
};

}

}

}

#endif // META_OCEAN_MEDIA_VRS_MEDIUM_H
