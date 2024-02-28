// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_IO_VRS_FLEXIBLE_RECORDER_H
#define META_OCEAN_IO_VRS_FLEXIBLE_RECORDER_H

#include "ocean/io/vrs/VRS.h"
#include "ocean/io/vrs/Recordables.h"
#include "ocean/io/vrs/Recorder.h"

namespace Ocean
{

namespace IO
{

namespace VRS
{

/**
 * This class implements a flexible recorder for an arbitrary number of streams.
 * An arbitrary number of streams can be added before recording starts.
 * @ingroup iovrs
 */
class OCEAN_IO_VRS_EXPORT FlexibleRecorder : public Recorder
{
	protected:

		/**
		 * Definition of an unordered map mapping indices to recordables.
		 */
		typedef std::unordered_map<unsigned int, std::shared_ptr<Recordable>> RecordableMap;

	public:

		/**
		 * Creates a new recorder object.
		 * @param name The name of the recorder
		 */
		explicit FlexibleRecorder(const std::string& name = "Ocean::FlexibleRecorder");

		/**
		 * Destructs the recorder object.
		 */
		~FlexibleRecorder() override;

		/**
		 * Adds a new recordable with specific type.
		 * This function generate the redordable index automatically and will return this index.
		 * @param recordableTypeId The id of the new recordable, must be valid
		 * @param flavor The optional flavor of the new recordable, can be empty
		 * @param vrsRecordStartingTimestamp Optional timestamp of the first configuration and state record, invalid to use the current time instead
		 * @return The index of the new recordable
		 * @tparam T The type of the new recordable, e.g., `RecordableCamera`, or `RecordableHomogenousMatrix4`.
		 */
		template <typename T>
		[[nodiscard]] unsigned int addRecordable(const vrs::RecordableTypeId& recordableTypeId = T::defaultRecordableTypeId(), const std::string& flavor = T::defaultFlavor(), const Timestamp& vrsRecordStartingTimestamp = Timestamp(false));

		/**
		 * Adds a new recordable with specific type.
		 * This function does not automatically generate the recordable index but takes a pre-defined index instead.<br>
		 * The given index must be unique and most not be used already.
		 * @param recordableIndex The explicit unique index of the new recordable, must not be invalidIndex(), must not be used already, must be valid
		 * @param recordableTypeId The id of the new recordable, must be valid
		 * @param flavor The optional flavor of the new recordable, can be empty
		 * @param vrsRecordStartingTimestamp Optional timestamp of the first configuration and state record, invalid to use the current time instead
		 * @return True, if succeeded (if the provided recordable index was not already in use); False, if the recording was not added
		 * @tparam T The type of the new recordable, e.g., `RecordableCamera`, or `RecordableHomogenousMatrix4`.
		 * @see hasRecordable().
		 */
		template <typename T>
		bool addRecordable(const unsigned int recordableIndex, const vrs::RecordableTypeId& recordableTypeId = T::defaultRecordableTypeId(), const std::string& flavor = T::defaultFlavor(), const Timestamp& vrsRecordStartingTimestamp = Timestamp(false));

		/**
		 * Returns a recordable with specific type.
		 * @param recordableIndex The index of the recordable, must be valid
		 * @return The requested recordable
		 * @tparam T The known type of the recordable must match the actual type of the recording, e.g., `RecordableCamera`, or `RecordableHomogenousMatrix4`.
		 * @see isRecordableOfType().
		 */
		template <typename T>
		[[nodiscard]] T& recordable(const unsigned int recordableIndex);

		/**
		 * Checks whether a recordable has a specific type.
		 * @param recordableIndex The index of the recordable to check, must be valid
		 * @tparam T The type to check e.g., `RecordableCamera`, or `RecordableHomogenousMatrix4`.
		 * @return True, if so
		 */
		template <typename T>
		[[nodiscard]] bool isRecordableOfType(const unsigned int recordableIndex) const;

		/**
		 * Checks whether a recordable exists with a specific index.
		 * @param recordableIndex The index of the recordable to check, must be valid
		 * @return True, if so
		 */
		[[nodiscard]] inline bool hasRecordable(const unsigned int recordableIndex) const;

		/**
		 * Returns the an invalid index of a recordable.
		 * @return Invalid index of a recordable
		 */
		static constexpr unsigned int invalidIndex();

	protected:

		/**
		 * Event function called when the writer needs to be configured.
		 * @see FileRecorder::onConfigureFileWriter().
		 */
		bool onConfigureWriter() override;

		/**
		 * Releases this recorder.
		 */
		void release() override;

	protected:

		/// The name of the recorder.
		std::string name_;

		/// All recordables of this recorder.
		RecordableMap recordableMap_;
};

template <typename T>
unsigned int FlexibleRecorder::addRecordable(const vrs::RecordableTypeId& recordableTypeId, const std::string& flavor, const Timestamp& vrsRecordStartingTimestamp)
{
	const ScopedLock scopedLock(lock_);

	unsigned int newIndex = newIndex = (unsigned int)(recordableMap_.size());

	while (recordableMap_.find(newIndex) != recordableMap_.cend())
	{
		++newIndex;
	}

	Recordable* newRecordable = new T(recordableTypeId, flavor, vrsRecordStartingTimestamp);

	recordFileWriter_.addRecordable(newRecordable);
	recordableMap_.emplace(newIndex, std::shared_ptr<Recordable>(newRecordable));

	return newIndex;
}

template <typename T>
bool FlexibleRecorder::addRecordable(const unsigned int recordableIndex, const vrs::RecordableTypeId& recordableTypeId, const std::string& flavor, const Timestamp& vrsRecordStartingTimestamp)
{
	ocean_assert(recordableIndex != invalidIndex());

	const ScopedLock scopedLock(lock_);

	const RecordableMap::const_iterator i = recordableMap_.find(recordableIndex);

	if (i != recordableMap_.cend())
	{
		ocean_assert(false && "The stream id exists already!");
		return false;
	}

	Recordable* newRecordable = new T(recordableTypeId, flavor, vrsRecordStartingTimestamp);

	recordFileWriter_.addRecordable(newRecordable);
	recordableMap_.emplace(recordableIndex, std::shared_ptr<Recordable>(newRecordable));

	return true;
}

template <typename T>
T& FlexibleRecorder::recordable(const unsigned int recordableIndex)
{
	const ScopedLock scopedLock(lock_);

	RecordableMap::const_iterator i = recordableMap_.find(recordableIndex);
	ocean_assert(i != recordableMap_.cend());

	T* recordable = dynamic_cast<T*>(i->second.get());
	ocean_assert(recordable != nullptr);

	return *recordable;
}

template <typename T>
bool FlexibleRecorder::isRecordableOfType(const unsigned int recordableIndex) const
{
	const ScopedLock scopedLock(lock_);

	const RecordableMap::const_iterator i = recordableMap_.find(recordableIndex);

	if (i == recordableMap_.cend())
	{
		return false;
	}

	return dynamic_cast<T*>(i->second.get()) != nullptr;
}

inline bool FlexibleRecorder::hasRecordable(const unsigned int recordableIndex) const
{
	const ScopedLock scopedLock(lock_);

	return recordableMap_.find(recordableIndex) != recordableMap_.cend();
}

constexpr unsigned int FlexibleRecorder::invalidIndex()
{
	return (unsigned int)(-1);
}

}

}

}

#endif // META_OCEAN_IO_VRS_FLEXIBLE_RECORDER_H
