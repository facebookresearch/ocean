// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/io/vrs/Recorder.h"

#include "ocean/base/Timestamp.h"

#include "ocean/io/Directory.h"
#include "ocean/io/File.h"

namespace Ocean
{

namespace IO
{

namespace VRS
{

Recorder::Recorder()
{
	// nothing to do here
}

Recorder::~Recorder()
{
	stop();
}

bool Recorder::start(const std::string& filename)
{
	ocean_assert(!filename.empty());
	if (filename.empty())
	{
		return false;
	}

	const IO::File file(filename);
	const IO::Directory directory(file);

	if (!directory.exists())
	{
		if (!directory.create())
		{
			Log::error() << "Failed to create directory \"" << directory() << "\"";

			return false;
		}
	}

	const ScopedLock scopedLock(lock_);

	if (!onConfigureWriter())
	{
		Log::error() << "Failed to configure the VRS recorder";

		return false;
	}

	recordFileWriter_.trackBackgroundThreadQueueByteSize();

	const int result = recordFileWriter_.createFileAsync(filename);

	if (result != 0)
	{
		return false;
	}

	// every second, write-out records older than 1 seconds
	recordFileWriter_.autoWriteRecordsAsync([](){ return double(Timestamp(true)) - 1.0; }, 1);

	return true;
}

bool Recorder::stop()
{
	const ScopedLock scopedLock(lock_);

	if (!isStarted())
	{
		return true;
	}

	const int result = recordFileWriter_.waitForFileClosed();

	release();

	return result == 0;
}

bool Recorder::isStarted() const
{
	const ScopedLock scopedLock(lock_);

	return recordFileWriter_.isWriting();
}

uint64_t Recorder::queueByteSize()
{
	const ScopedLock scopedLock(lock_);

	const uint64_t bytes = recordFileWriter_.getBackgroundThreadQueueByteSize();

	return bytes;
}

void Recorder::setCompressionThreadPoolSize(uint32_t size)
{
	const ScopedLock scopedLock(lock_);
	recordFileWriter_.setCompressionThreadPoolSize(size);
}

void Recorder::release()
{
	// nothing to do here
}

}

}

}
