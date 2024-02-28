// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/mediafoundation/MFMovieRecorder.h"
#include "ocean/media/mediafoundation/Utilities.h"

#include <mfapi.h>
#include <mfidl.h>
#include <Mfreadwrite.h>
#include <mferror.h>

namespace Ocean
{

namespace Media
{

namespace MediaFoundation
{

MFMovieRecorder::MFMovieRecorder()
{
	recorderFrameEncoder = "h264";
	recorderFrameFrequency = 30.0;
	recorderFilenameSuffixed = false;
}

MFMovieRecorder::~MFMovieRecorder()
{
	stop();
}

bool MFMovieRecorder::setFilename(const std::string& filename)
{
	const ScopedLock scopedLock(recorderLock);

	if (sinkWriter_.object() != nullptr)
	{
		Log::error() << "The filename cannot be changed after recording has started.";
		return false;
	}

	return MovieRecorder::setFilename(filename);
}

bool MFMovieRecorder::setPreferredFrameType(const FrameType& type)
{
	if (!type.isValid())
	{
		return false;
	}

	const ScopedLock scopedLock(recorderLock);

	if (sinkWriter_.object() != nullptr)
	{
		Log::error() << "The frame type cannot be changed after recording has started.";
		return false;
	}

	if (Utilities::convertPixelFormat(type.pixelFormat()) == GUID_NULL)
	{
		return MovieRecorder::setPreferredFrameType(FrameType(type, FrameType::FORMAT_BGR24));
	}

	return MovieRecorder::setPreferredFrameType(type);
}

bool MFMovieRecorder::start()
{
	const ScopedLock scopedLock(recorderLock);

	if (sinkWriter_.object() != nullptr)
	{
		return false;
	}

	if (!createSinkWriter())
	{
		return false;
	}

	ocean_assert(sinkWriter_.object() != nullptr);

	if (sinkWriter_.object()->BeginWriting() != S_OK)
	{
		return false;
	}

	isRecording_ = true;

	return true;
}

bool MFMovieRecorder::stop()
{
	const ScopedLock scopedLock(recorderLock);

	if (sinkWriter_.object() == nullptr || isRecording_ == false)
	{
		return false;
	}

	if (sinkWriter_.object()->Finalize() != S_OK)
	{
		return false;
	}

	isRecording_ = false;

	releaseSinkWriter();

	return true;
}

bool MFMovieRecorder::isRecording() const
{
	const ScopedLock scopedLock(recorderLock);

	return isRecording_;
}

MFMovieRecorder::Encoders MFMovieRecorder::frameEncoders() const
{
	Encoders encoders;

	encoders.emplace_back("h264");
	encoders.emplace_back("h265");
	encoders.emplace_back("hevc");

	return encoders;
}

bool MFMovieRecorder::lockBufferToFill(Frame& recorderFrame, const bool /*respectFrameFrequency*/)
{
	const ScopedLock scopedLock(recorderLock);

	// **TODO** missing implementation, handle 'respectFrameFrequency'

	if (sinkWriter_.object() == nullptr)
	{
		return false;
	}

	ocean_assert(mediaBuffer_.object() == nullptr);

	const DWORD bufferSize = DWORD(recorderFrameType.frameTypeSize());

	if (bufferSize == DWORD(0))
	{
		return false;
	}

	bool noError = true;

	if (noError && S_OK != MFCreateMemoryBuffer(bufferSize, &mediaBuffer_.resetObject()))
	{
		noError = false;
	}

	if (noError && S_OK != mediaBuffer_.object()->SetCurrentLength(bufferSize))
	{
		noError = false;
	}

	BYTE* data = nullptr;
	if (noError && S_OK != mediaBuffer_.object()->Lock(&data, nullptr, nullptr))
	{
		noError = false;
	}

	if (noError == false)
	{
		mediaBuffer_.release();
		return false;
	}

	ocean_assert(recorderFrameType.numberPlanes() == 1u);

	constexpr unsigned int dataPaddingElements = 0u;

	recorderFrame = Frame(recorderFrameType, data, Frame::CM_USE_KEEP_LAYOUT, dataPaddingElements);

	return true;
}

void MFMovieRecorder::unlockBufferToFill()
{
	const ScopedLock scopedLock(recorderLock);

	ocean_assert(mediaBuffer_.object() != nullptr && sinkWriter_.object() != nullptr);
	if (mediaBuffer_.object() == nullptr || sinkWriter_.object() == nullptr)
	{
		return;
	}

	bool noError = true;

	if (noError && S_OK != mediaBuffer_.object()->Unlock())
	{
		noError = false;
	}

	ScopedIMFSample sample;
	if (noError && S_OK != MFCreateSample(&sample.resetObject()))
	{
		noError = false;
	}

	if (noError && S_OK != sample.object()->AddBuffer(mediaBuffer_.object()))
	{
		noError = false;
	}

	ocean_assert(recorderFrameFrequency > 0.0);
	const double frameDuration = 1.0 / recorderFrameFrequency;

	const LONGLONG sampleTime100ns = LONGLONG(nextFrameTimestamp_ * 10000000.0);

	if (noError && S_OK != sample.object()->SetSampleTime(sampleTime100ns))
	{
		noError = false;
	}

	nextFrameTimestamp_ += frameDuration;

	const LONGLONG frameDuration100ns = LONGLONG(frameDuration * 10000000.0);

	if (noError && S_OK != sample.object()->SetSampleDuration(frameDuration100ns))
	{
		noError = false;
	}

	if (noError && S_OK != sinkWriter_.object()->WriteSample(streamIndex_, sample.object()))
	{
		noError = false;
	}

	mediaBuffer_.release();
}

bool MFMovieRecorder::encoderToVideoFormat(const std::string& encoder, GUID& videoFormat)
{
	ocean_assert(!encoder.empty());

	const std::string lowerEncoder(String::toLower(encoder));

	if (encoder == "h264")
	{
		videoFormat = MFVideoFormat_H264;
	}
	else if (encoder == "h265")
	{
		videoFormat = MFVideoFormat_H265;
	}
	else if (encoder == "hevc")
	{
		videoFormat = MFVideoFormat_HEVC;
	}
	else
	{
		ocean_assert(false && "Invalid encoder!");
		return false;
	}

	return true;
}

bool MFMovieRecorder::createSinkWriter()
{
	const std::string filename = addOptionalSuffixToFilename(recorderFilename, recorderFilenameSuffixed);

	if (filename.empty())
	{
		return false;
	}

	GUID videoFormat = GUID_NULL;
	if (!encoderToVideoFormat(recorderFrameEncoder, videoFormat))
	{
		Log::error() << "Invalid frame encoder!";
		return false;
	}

	const GUID videoInput = Utilities::convertPixelFormat(recorderFrameType.pixelFormat());

	if (videoInput == GUID_NULL)
	{
		Log::error() << "Invalid pixel format!";
		return false;
	}

	constexpr unsigned int frameRateDenominator = 600u;
	const unsigned int frameRateNumerator = (unsigned int)(recorderFrameFrequency * double(frameRateDenominator) + 0.5);

	bool noError = true;

	ocean_assert(sinkWriter_.object() == nullptr);

	if (noError && S_OK != MFCreateSinkWriterFromURL(String::toWString(filename).c_str(), nullptr, nullptr, &sinkWriter_.resetObject()))
	{
		noError = false;
	}

	ScopedIMFMediaType mediaTypeOutput;

	if (noError && S_OK != MFCreateMediaType(&mediaTypeOutput.resetObject()))
	{
		noError = false;
	}
	if (noError && S_OK != mediaTypeOutput.object()->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video))
	{
		noError = false;
	}
	if (noError && S_OK != mediaTypeOutput.object()->SetGUID(MF_MT_SUBTYPE, videoFormat))
	{
		noError = false;
	}
	if (noError && S_OK != mediaTypeOutput.object()->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive))
	{
		noError = false;
	}
	if (noError && S_OK != mediaTypeOutput.object()->SetUINT32(MF_MT_AVG_BITRATE, 10000000u))
	{
		noError = false;
	}
	if (noError && S_OK != MFSetAttributeSize(mediaTypeOutput.object(), MF_MT_FRAME_SIZE, recorderFrameType.width(), recorderFrameType.height()))
	{
		noError = false;
	}
	if (noError && S_OK != MFSetAttributeRatio(mediaTypeOutput.object(), MF_MT_FRAME_RATE, frameRateNumerator, frameRateDenominator))
	{
		noError = false;
	}
	if (noError && S_OK != MFSetAttributeRatio(mediaTypeOutput.object(), MF_MT_PIXEL_ASPECT_RATIO, 1, 1))
	{
		noError = false;
	}

	ocean_assert(streamIndex_ == DWORD(-1));
	if (noError && S_OK != sinkWriter_.object()->AddStream(mediaTypeOutput.object(), &streamIndex_))
	{
		noError = false;
	}

	ScopedIMFMediaType mediaTypeInput;

	if (noError && S_OK != MFCreateMediaType(&mediaTypeInput.resetObject()))
	{
		noError = false;
	}
	if (noError && S_OK != mediaTypeInput.object()->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video))
	{
		noError = false;
	}
	if (noError && S_OK != mediaTypeInput.object()->SetGUID(MF_MT_SUBTYPE, videoInput))
	{
		noError = false;
	}
	if (noError && S_OK != mediaTypeInput.object()->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive))
	{
		noError = false;
	}
	if (noError && S_OK != MFSetAttributeSize(mediaTypeInput.object(), MF_MT_FRAME_SIZE, recorderFrameType.width(), recorderFrameType.height()))
	{
		noError = false;
	}

	ocean_assert(recorderFrameType.numberPlanes() == 1u);
	ocean_assert(FrameType::formatIsGeneric(recorderFrameType.pixelFormat()));

	unsigned int strideBytes = recorderFrameType.width() * recorderFrameType.bytesPerDataType() * recorderFrameType.channels();

	if (recorderFrameType.pixelOrigin() == FrameType::ORIGIN_LOWER_LEFT)
	{
		// the stride for bottom frames is negative (and then casted to an unsigned int)
		strideBytes = (unsigned int)(-int(strideBytes));
	}

	if (noError && S_OK != mediaTypeInput.object()->SetUINT32(MF_MT_DEFAULT_STRIDE, strideBytes))
	{
		noError = false;
	}
	if (noError && S_OK != MFSetAttributeRatio(mediaTypeInput.object(), MF_MT_FRAME_RATE, frameRateNumerator, frameRateDenominator))
	{
		noError = false;
	}
	if (noError && S_OK != MFSetAttributeRatio(mediaTypeInput.object(), MF_MT_PIXEL_ASPECT_RATIO, 1, 1))
	{
		noError = false;
	}

	if (noError && S_OK != sinkWriter_.object()->SetInputMediaType(streamIndex_, mediaTypeInput.object(), nullptr))
	{
		noError = false;
	}

	if (noError == false)
	{
		sinkWriter_.release();
	}

	return noError;
}

void MFMovieRecorder::releaseSinkWriter()
{
	ocean_assert(isRecording_ == false);

	if (mediaBuffer_.object() != nullptr)
	{
		mediaBuffer_.object()->Unlock();
	}

	mediaBuffer_.release();

	nextFrameTimestamp_ = 0.0;

	streamIndex_ = DWORD(-1);

	sinkWriter_.release();
}

}

}

}
