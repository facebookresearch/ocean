// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_IO_VRS_STREAM_PLAYERS_H
#define META_OCEAN_IO_VRS_STREAM_PLAYERS_H

#include "ocean/io/vrs/VRS.h"

#include "ocean/base/Timestamp.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/SampleMap.h"

#include <vrs/RecordFormatStreamPlayer.h>

namespace Ocean
{

namespace IO
{

namespace VRS
{

/**
 * This class implements a specialization of a `RecordFormatStreamPlayer` object for HomogenousMatrix4 objects.
 * @ingroup iovrs
 */
class OCEAN_IO_VRS_EXPORT PlayableHomogenousMatrix4 : public vrs::RecordFormatStreamPlayer
{
	public:

		/**
		 * Definition of a sample map mapping timestamps to homogeneous matrices (this map allows to determine interpolated transformations for arbitrary timestamps).
		 */
		typedef SampleMap<HomogenousMatrixD4> HomogenousMatrix4Map;

	public:

		/**
		 * Creates a new playable object.
		 */
		PlayableHomogenousMatrix4();

		/**
		 * Data layout read event function of the playable.
		 * @see RecordFormatStreamPlayer::onDataLayoutRead().
		 */
		bool onDataLayoutRead(const vrs::CurrentRecord& header, size_t blockIndex, vrs::DataLayout& dataLayout) override;

		/**
		 * Returns the transformations which are stored in the readable.
		 * @return The moved transformation map
		 */
		inline HomogenousMatrix4Map&& homogenousMatrix4Map();

	protected:

		/// The transformation which have been read.
		HomogenousMatrix4Map homogenousMatrix4Map_;
};

/**
 * This class implements a specialization of a `RecordFormatStreamPlayer` object for HomogenousMatrices4 objects.
 * @ingroup iovrs
 */
class OCEAN_IO_VRS_EXPORT PlayableHomogenousMatrices4 : public vrs::RecordFormatStreamPlayer
{
	public:

		/**
		 * Definition of a map mapping timestamps to groups of homogeneous matrices.
		 */
		typedef std::map<double, HomogenousMatricesD4> HomogenousMatrices4Map;

	public:

		/**
		 * Creates a new playable object.
		 */
		PlayableHomogenousMatrices4();

		/**
		 * Data layout read event function of the playable.
		 * @see RecordFormatStreamPlayer::onDataLayoutRead().
		 */
		bool onDataLayoutRead(const vrs::CurrentRecord& header, size_t blockIndex, vrs::DataLayout& dataLayout) override;

		/**
		 * Returns the transformations which are stored in the readable.
		 * @return The moved transformation map
		 */
		inline HomogenousMatrices4Map&& homogenousMatrices4Map();

	protected:

		/// The groups of transformations which have been read.
		HomogenousMatrices4Map homogenousMatrices4Map_;
};

/**
 * This class implements a specialization of a `RecordFormatStreamPlayer` object for strings objects.
 * @ingroup iovrs
 */
class OCEAN_IO_VRS_EXPORT PlayableString : public vrs::RecordFormatStreamPlayer
{
	public:

		/**
		 * Definition of a map mapping timestamps to strings.
		 */
		typedef std::map<double, std::string> StringMap;

	public:

		/**
		 * Creates a new playable object.
		 */
		PlayableString();

		/**
		 * Data layout read event function of the playable.
		 * @see RecordFormatStreamPlayer::onDataLayoutRead().
		 */
		bool onDataLayoutRead(const vrs::CurrentRecord& header, size_t blockIndex, vrs::DataLayout& dataLayout) override;

		/**
		 * Returns the strings which are stored in the readable.
		 * @return The moved string map
		 */
		inline StringMap&& stringMap();

	protected:

		/// The transformation which have been read.
		StringMap stringMap_;
};

inline PlayableHomogenousMatrix4::HomogenousMatrix4Map&& PlayableHomogenousMatrix4::homogenousMatrix4Map()
{
	return std::move(homogenousMatrix4Map_);
}

inline PlayableHomogenousMatrices4::HomogenousMatrices4Map&& PlayableHomogenousMatrices4::homogenousMatrices4Map()
{
	return std::move(homogenousMatrices4Map_);
}

inline PlayableString::StringMap&& PlayableString::stringMap()
{
	return std::move(stringMap_);
}

}

}

}

#endif // META_OCEAN_IO_VRS_STREAM_PLAYERS_H
