/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/avfoundation/AVFMovieRecorder.h"
#include "ocean/media/avfoundation/AVFLibrary.h"

#include "ocean/base/String.h"
#include "ocean/base/StringApple.h"

#include "ocean/io/File.h"

#include <CoreVideo/CoreVideo.h>

#import <Foundation/Foundation.h>

namespace Ocean
{

namespace Media
{

namespace AVFoundation
{

AVFMovieRecorder::AVFMovieRecorder()
{
	recorderFrameEncoder = "h264";
	recorderFrameFrequency = 30.0;
	recorderFilenameSuffixed = false;
}

AVFMovieRecorder::~AVFMovieRecorder()
{
	stop();

	while (isStopped_ == false)
	{
		Thread::sleep(1u);
	}
}

bool AVFMovieRecorder::setFilename(const std::string& filename)
{
	if (assetWriter_ != nullptr)
	{
		Log::error() << "The filename cannot be changed after recording has started.";
		return false;
	}

	return MovieRecorder::setFilename(filename);
}

bool AVFMovieRecorder::start()
{
	const ScopedLock scopedLock(recorderLock);

	if (assetWriter_ != nullptr || isRecording_)
	{
		return false;
	}

	if (createNewAssetWriter())
	{
		nextFrameTimestamp_ = 0.0;
		previousFrameTimestamp_ = -1.0;

		if ([assetWriter_ startWriting])
		{
			[assetWriter_ startSessionAtSourceTime:time(nextFrameTimestamp_)];

			if (assetWriter_.status != AVAssetWriterStatusFailed)
			{
				isRecording_ = true;
				isStopped_ = false;
				return true;
			}
		}
	}

	release();

	return false;
}

bool AVFMovieRecorder::stop()
{
	const ScopedLock scopedLock(recorderLock);

	if (assetWriter_ == nullptr || isRecording_ == false)
	{
		return false;
	}

	ocean_assert(nextFrameTimestamp_ >= 0.0);
	[assetWriter_ endSessionAtSourceTime:time(nextFrameTimestamp_)];

	ocean_assert(assetWriterInput_ != nullptr);
	[assetWriterInput_ markAsFinished];

	[assetWriter_ finishWritingWithCompletionHandler:^{
		release();
	}];

	isRecording_ = false;

	return true;
}

bool AVFMovieRecorder::isRecording() const
{
	const ScopedLock scopedLock(recorderLock);

	return isRecording_;
}

AVFMovieRecorder::Encoders AVFMovieRecorder::frameEncoders() const
{
	Encoders encoders;

	if (@available(macOS 10.13, *))
	{
		encoders.emplace_back("H264");
		encoders.emplace_back("HEVC");
		encoders.emplace_back("JPEG");
	}

	return encoders;
}

bool AVFMovieRecorder::lockBufferToFill(Frame& recorderFrame, const bool respectFrameFrequency)
{
	ocean_assert(respectFrameFrequency == false && "currently not supported!");

	const ScopedLock scopedLock(recorderLock);

	if (assetWriter_ == nullptr)
	{
		return false;
	}

	ocean_assert(assetWriterInputPixelBufferAdaptor_ != nullptr);
	ocean_assert(assetWriterInput_ != nullptr);
	ocean_assert(!pixelBufferAccessor_);
	ocean_assert(recorderFrameType.isValid());

	const FrameType::PixelFormat pixelFormat = bestMatchingPixelFormat(recorderFrameType.pixelFormat());

	const OSType pixelFormatType = PixelBufferAccessor::translatePixelFormat(pixelFormat);

	if (pixelFormatType == OSType(0))
	{
		return false;
	}

	if (CVPixelBufferCreate(nullptr, recorderFrameType.width(), recorderFrameType.height(), pixelFormatType, nullptr, &pixelBuffer_) != kCVReturnSuccess)
	{
		ocean_assert(false && "This should never happen!");
		ocean_assert(pixelBuffer_ == nullptr);
		return false;
	}

	pixelBufferAccessor_ = PixelBufferAccessor(pixelBuffer_, false);

	ocean_assert(pixelBufferAccessor_);
	if (!pixelBufferAccessor_)
	{
		return false;
	}

	recorderFrame = Frame(pixelBufferAccessor_.frame(), Frame::ACM_USE_KEEP_LAYOUT);

	return true;
}

void AVFMovieRecorder::unlockBufferToFill()
{
	ocean_assert(assetWriterInputPixelBufferAdaptor_ != nullptr);
	ocean_assert(pixelBuffer_ != nullptr);

	const ScopedLock scopedLock(recorderLock);

	if (assetWriterInputPixelBufferAdaptor_ != nullptr && pixelBuffer_ != nullptr)
	{
		while (assetWriterInput_.isReadyForMoreMediaData == NO)
		{
			Thread::sleep(1u);
		}

		const BOOL result = [assetWriterInputPixelBufferAdaptor_ appendPixelBuffer:pixelBuffer_ withPresentationTime:time(nextFrameTimestamp_)];
		ocean_assert_and_suppress_unused(result == YES, result);

		previousFrameTimestamp_ = nextFrameTimestamp_;

		ocean_assert(recorderFrameFrequency > 0.0);
		nextFrameTimestamp_ += 1.0 / recorderFrameFrequency;
	}

	pixelBufferAccessor_.release();

	if (pixelBuffer_ != nullptr)
	{
		CVPixelBufferRelease(pixelBuffer_);
		pixelBuffer_ = nullptr;
	}
}

AVVideoCodecType AVFMovieRecorder::frameEncoderToVideoCodecType(const std::string& frameEncoder)
{
	const std::string lowerFrameEncoder = String::toLower(frameEncoder);

#if defined(OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY)

	if (lowerFrameEncoder == "h264")
	{
		if (@available(ios 11.0, *))
		{
			return AVVideoCodecTypeH264;
		}
	}

	if (lowerFrameEncoder == "hevc")
	{
		if (@available(ios 11.0, *))
		{
			return AVVideoCodecTypeHEVC;
		}
	}

	if (lowerFrameEncoder == "jpeg")
	{
		if (@available(ios 11.0, *))
		{
			return AVVideoCodecTypeJPEG;
		}
	}

#else

	if (lowerFrameEncoder == "h264")
	{
		if (@available(macOS 10.13, *))
		{
			return AVVideoCodecTypeH264;
		}
	}

	if (lowerFrameEncoder == "hevc")
	{
		if (@available(macOS 10.13, *))
		{
			return AVVideoCodecTypeHEVC;
		}
	}

	if (lowerFrameEncoder == "jpeg")
	{
		if (@available(macOS 10.13, *))
		{
			return AVVideoCodecTypeJPEG;
		}
	}

#endif // defined(TARGET_IPHONE_SIMULATOR)

	return nullptr;
}

AVFileType AVFMovieRecorder::fileExtensionToFileType(const std::string& fileExtension)
{
	const std::string lowerFileExtension = String::toLower(fileExtension);

	if (lowerFileExtension == "mp4")
	{
		return AVFileTypeMPEG4;
	}

	if (lowerFileExtension == "mov")
	{
		return AVFileTypeQuickTimeMovie;
	}

	return nullptr;
}

bool AVFMovieRecorder::createNewAssetWriter()
{
	if (!recorderFrameType.isValid())
	{
		Log::error() << "The frame type of the recorder is not configured yet.";
		return false;
	}

	ocean_assert(assetWriter_ == nullptr);

	const IO::File file(recorderFilename);

	const AVFileType avFileType = fileExtensionToFileType(file.extension());

	if (avFileType == nullptr)
	{
		Log::error() << "The file extension (\"" << file.extension() << "\") of the movie is not supported.";
		return false;
	}

	const std::string filename = addOptionalSuffixToFilename(recorderFilename, recorderFilenameSuffixed);

	if (IO::File(filename).exists())
	{
		Log::error() << "The movie file \"" << filename << "\" exists already.";
		return false;
	}

	NSURL* nsUrl = [NSURL fileURLWithPath: StringApple::toNSString(filename)];

	assetWriter_ = [[AVAssetWriter alloc] initWithURL:nsUrl fileType:avFileType error:nil];

	if (assetWriter_ == nullptr)
	{
		Log::error() << "Failed to create an asset writer.";
		return false;
	}

	ocean_assert(assetWriterInput_ == nullptr);
	ocean_assert(assetWriterInputPixelBufferAdaptor_ == nullptr);

	AVVideoCodecType avVideoCodecType = frameEncoderToVideoCodecType(recorderFrameEncoder);

	if (avVideoCodecType == nullptr)
	{
		Log::error() << "The frame encoder \"" << recorderFrameEncoder << "\" is not supported.";
		return false;
	}

	NSDictionary* outputSettings = @{
		AVVideoCodecKey:avVideoCodecType,
		AVVideoWidthKey:@(int(recorderFrameType.width())),
		AVVideoHeightKey:@(int(recorderFrameType.height()))
	};

	assetWriterInput_ = [[AVAssetWriterInput alloc] initWithMediaType:AVMediaTypeVideo outputSettings:outputSettings];

	if (assetWriterInput_ == nullptr)
	{
		Log::error() << "Failed to create an input asset writer.";
		return false;
	}

	assetWriterInput_.expectsMediaDataInRealTime = YES;

	assetWriterInputPixelBufferAdaptor_ = [AVAssetWriterInputPixelBufferAdaptor assetWriterInputPixelBufferAdaptorWithAssetWriterInput:assetWriterInput_ sourcePixelBufferAttributes:nil];

	if (assetWriterInputPixelBufferAdaptor_ == nullptr)
	{
		return false;
	}

	[assetWriter_ addInput:assetWriterInput_];

	return true;
}

void AVFMovieRecorder::release()
{
	nextFrameTimestamp_ = 0.0;
	previousFrameTimestamp_ = -1.0;

	pixelBufferAccessor_.release();

	if (pixelBuffer_ != nullptr)
	{
		CVPixelBufferRelease(pixelBuffer_);
		pixelBuffer_ = nullptr;
	}

	assetWriterInputPixelBufferAdaptor_ = nullptr;
	assetWriterInput_ = nullptr;
	assetWriter_ = nullptr;

	isStopped_ = true; // very last
}

FrameType::PixelFormat AVFMovieRecorder::bestMatchingPixelFormat(const FrameType::PixelFormat preferredPixelFormat)
{
	if (preferredPixelFormat == FrameType::FORMAT_UNDEFINED)
	{
		return FrameType::FORMAT_RGB24;
	}

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY
	return preferredPixelFormat;
#else

	if (FrameType::arePixelFormatsCompatible(preferredPixelFormat, FrameType::genericPixelFormat<uint8_t, 3u>()))
	{
		return FrameType::FORMAT_RGB24; // for kCVPixelFormatType_24RGB, k24RGBPixelFormat
	}
	else if (FrameType::arePixelFormatsCompatible(preferredPixelFormat, FrameType::genericPixelFormat<uint8_t, 4u>()))
	{
		return FrameType::FORMAT_ARGB32; // for kCVPixelFormatType_32ARGB, k32ARGBPixelFormat
	}
	else
	{
		if (FrameType::formatHasAlphaChannel(preferredPixelFormat))
		{
			return FrameType::FORMAT_ARGB32;
		}
		else
		{
			return FrameType::FORMAT_RGB24;
		}
	}

#endif
}

}

}

}
