// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_IO_VRS_READER_H
#define META_OCEAN_IO_VRS_READER_H

#include "ocean/io/vrs/VRS.h"
#include "ocean/io/vrs/StreamPlayers.h"

namespace Ocean
{

namespace IO
{

namespace VRS
{

/**
 * This class is a reader for VRS streams.
 * The values of individual streams can be read with corresponding playables.<br>
 * The reader will read all values of the streams at once.<br>
 * Therefore, the reader is intended for streams with small memory footprint like e.g., transformations, and not for stream with e.g., image content.
 * @ingroup iovrs
 */
class OCEAN_IO_VRS_EXPORT Reader
{
	protected:

		/**
		 * Definition of a map mapping names of playables to playable objects.
		 */
		typedef std::map<std::string, vrs::RecordFormatStreamPlayer*> PlayableNameReaderMap;

		/**
		 * Definition of a map mapping recordable type ids and flavors to playable objects.
		 */
		typedef std::map<std::pair<vrs::RecordableTypeId, std::string>, vrs::RecordFormatStreamPlayer*> PlayableRecordableTypeIdReaderMap;

	public:

		/**
		 * Creates a new file reader object.
		 */
		Reader();

		/**
		 * Destructs this reader object.
		 */
		virtual ~Reader();

		/**
		 * Adds a new playable for reading.
		 * @param playable The playable for reading, must be valid
		 * @param name The name of the playable in the VRS stream, must be valid
		 * @return True, if succeeded
		 */
		bool addPlayable(vrs::RecordFormatStreamPlayer* playable, const std::string& name);

		/**
		 * Adds a new playable for reading.
		 * @param playable The playable for reading, must be valid
		 * @param recordableTypeId The type id of the recordable from which the playable will be read, must be valid
		 * @param flavor The flavor of the recordable type id, empty if the recordable has no flavor
		 * @return True, if succeeded
		 */
		bool addPlayable(vrs::RecordFormatStreamPlayer* playable, const vrs::RecordableTypeId& recordableTypeId, const std::string& flavor);

		/**
		 * Reads a VRS file synchronous.
		 * Beware: All playables must be added before reading the file.
		 * @param filename The filename of the VRS file, must be valid
		 * @return The number of successfully read streams, range: [0, infinity)
		 * @see addPlayable().
		 */
		virtual size_t read(const std::string& filename);

	protected:

		/**
		 * Disabled copy constructor.
		 * @param reader The reader which would be copied
		 */
		Reader(const Reader& reader) = delete;

		/**
		* Disabled copy operator.
		* @param reader The reader which would be assigned
		* @return Reference to this object
		*/
		Reader& operator=(const Reader& reader) = delete;

	protected:

		/// The map mapping names of playables to playable objects.
		PlayableNameReaderMap playableNameReaderMap_;

		/// The map mapping recordable type ids and flavors to playable objects.
		PlayableRecordableTypeIdReaderMap playableRecordableTypeIdReaderMap_;

		/// The reader lock.
		Lock lock_;
};

}

}

}

#endif // META_OCEAN_IO_VRS_READER_H
