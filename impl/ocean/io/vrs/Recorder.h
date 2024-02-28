// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_IO_VRS_RECORDER_H
#define META_OCEAN_IO_VRS_RECORDER_H

#include "ocean/io/vrs/VRS.h"

#include "ocean/base/Lock.h"

#include <vrs/RecordFileWriter.h>

namespace Ocean
{

namespace IO
{

namespace VRS
{

/**
 * This class is the base class for all VRS recorder objects.
 * The recorder comes with several recordables for standard Ocean data types.<br>
 * A customized recorder can be created by deriving from this class.
 * @ingroup iovrs
 */
class OCEAN_IO_VRS_EXPORT Recorder
{
	public:

		/**
		 * Creates a new recorder object.
		 */
		Recorder();

		/**
		 * Destructs this recorder object.
		 */
		virtual ~Recorder();

		/**
		 * Starts recording a VRS file.
		 * @param filename The filename of the VRS file, must be valid
		 * @return True, if succeeded
		 * @see stop().
		 */
		virtual bool start(const std::string& filename);

		/**
		 * Stops recording a VRS file.
		 * @return True, if succeeded
		 * @see start().
		 */
		virtual bool stop();

		/**
		 * Returns whether the recorder is currently started and actively writing.
		 * @return True, if so
		 */
		bool isStarted() const;

		/**
		 * Returns the byte size of the queue of this recorder.
		 * @return The queue's byte size, with range [0, infinity)
		 */
		uint64_t queueByteSize();

		/**
		 * Set number of threads to use for background compression, or none will be used.
		 * @param size: The number of threads to compress records in parallel, with range
		 * [0, infinity), uint32_t(-1) to use as many threads as there are cores in system.
		 */
		void setCompressionThreadPoolSize(uint32_t size);

	protected:

		/**
		 * Disabled copy constructor.
		 * @param recorder The recorder which would be copied
		 */
		Recorder(const Recorder& recorder) = delete;

		/**
		 * Event function called when the writer needs to be configured.
		 */
		virtual bool onConfigureWriter() = 0;

		/**
		 * Releases all resources of this recorder.
		 */
		virtual void release();

		/**
		* Disabled copy operator.
		* @param recorder The recorder which would be assigned
		* @return Reference to this object
		*/
		Recorder& operator=(const Recorder& recorder) = delete;

	protected:

		/// The actual VRS recorder.
		vrs::RecordFileWriter recordFileWriter_;

		/// The recorder lock.
		mutable Lock lock_;
};

}

}

}

#endif // META_OCEAN_IO_VRS_RECORDER_H
