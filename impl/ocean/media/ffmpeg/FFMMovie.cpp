/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/ffmpeg/FFMMovie.h"
#include "ocean/media/ffmpeg/FFMLibrary.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/ScopedFunction.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/pixdesc.h>
}

namespace Ocean
{

namespace Media
{

namespace FFmpeg
{

FFMMovie::FFMMovie(const std::string& url) :
	Medium(url),
	FFMMedium(url),
	FiniteMedium(url),
	FrameMedium(url),
	SoundMedium(url),
	Movie(url)
{
	if (isValid_)
	{
		if (!createAndOpenVideoCodec())
		{
			isValid_ = false;
		}
	}
}

FFMMovie::~FFMMovie()
{
	stopThreadExplicitly();

	frameCollection_.clear();

	releaseVideoCodec();
}

MediumRef FFMMovie::clone() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid_);
	if (isValid_)
	{
		return FFMLibrary::newMovie(url_, true);
	}

	return MediumRef();
}

double FFMMovie::duration() const
{
	const ScopedLock scopedLock(lock_);

	const float currentSpeed = speed();

	if (currentSpeed == 0.0f)
	{
		return 0.0;
	}

	return normalDuration() / double(currentSpeed);
}

double FFMMovie::normalDuration() const
{
	const ScopedLock scopedLock(lock_);

	if (normalDuration_ < 0.0)
	{
		Log::error() << "FFmpeg: Unknown duration of movie";
		return 0.0;
	}

	return normalDuration_;
}

double FFMMovie::position() const
{
	return position_.load();
}

float FFMMovie::speed() const
{
	return speed_.load();
}

bool FFMMovie::setSpeed(const float speed)
{
	if (speed < 0.0f)
	{
		return false;
	}

	speed_ = speed;

	return true;
}

float FFMMovie::soundVolume() const
{
	return 0.0f;
}

bool FFMMovie::soundMute() const
{
	return true;
}

bool FFMMovie::setSoundVolume(const float volume)
{
	return false;
}

bool FFMMovie::setSoundMute(const bool mute)
{
	return false;
}

bool FFMMovie::setUseSound(const bool state)
{
	return !state;
}

bool FFMMovie::setPosition(const double position)
{
	if (position < 0.0 || position > duration())
	{
		return false;
	}

	seekPosition_ = position;
	position_ = position;

	return true;
}

bool FFMMovie::internalStart()
{
	ocean_assert(avFormatContext_ != nullptr && avVideoStreamIndex_ >= 0);

	if (isPaused_)
	{
		isPaused_ = false;

		return true;
	}
	else
	{
		if (position_.load() != 0.0)
		{
			// we re-start the movie
			seekPosition_ = 0.0;
		}

		return startThread();
	}
}

bool FFMMovie::internalPause()
{
	isPaused_ = true;

	return true;
}

bool FFMMovie::internalStop()
{
	isPaused_ = false;

	stopThread();

	return true;
}

void FFMMovie::threadRun()
{
	ocean_assert(avFormatContext_ != nullptr);
	ocean_assert(avVideoStream_ != nullptr && avVideoStreamIndex_ >= 0);

	RandomI::initialize();

	const Timestamp startTimestamp(startTimestamp_);
	ocean_assert(startTimestamp.isValid());

	AVFrame* avFrame = av_frame_alloc();
	ocean_assert(avFrame != nullptr);

	const ScopedFunctionVoid scopedFrameFree(std::bind(&av_frame_free, &avFrame));

	// we need an intermediate store for packet timestamps as frames may be delayed

	PacketTimestampMap packetTimestampMap;
	packetTimestampMap.reserve(32);

	AVPacket avPacket;

	const AVRational avTimeBase = avVideoStream_->time_base;

	int64_t frameIndex = 0ll;

	while (shouldThreadStop() == false)
	{
		const double seekPosition = seekPosition_.exchange(-1.0);

		if (seekPosition >= 0.0)
		{
			const int64_t targetTimestamp = int64_t(seekPosition * double(avTimeBase.den) / double(avTimeBase.num) + 0.5);

			const int seekResult = avformat_seek_file(avFormatContext_, avVideoStreamIndex_, 0, targetTimestamp, avFormatContext_->streams[avVideoStreamIndex_]->duration, 0);

			if (seekResult >= 0)
			{
				// Reset the internal codec state / flush internal buffers.
				avcodec_flush_buffers(avVideoCodecContext_);
			}
			else
			{
				Log::error() << "FFmpeg: Failed to change position in movie '" << url() << "': " << av_err2str(seekResult);
			}
		}

		if (isPaused_.load())
		{
			while (isPaused_.load() && shouldThreadStop() == false)
			{
				sleep(1u);
			}
		}

		ScopedFunctionVoid scopedPacketUnref;

		const int readFrameResult = av_read_frame(avFormatContext_, &avPacket);

		if (readFrameResult == 0)
		{
			scopedPacketUnref = ScopedFunctionVoid(std::bind(&av_packet_unref, &avPacket));
		}

		if (readFrameResult == 0 || readFrameResult == AVERROR_EOF) // in case of AVERROR_EOF, we still may receive frames from the codec
		{
			if (avPacket.stream_index == avVideoStreamIndex_)
			{
				if (readFrameResult != AVERROR_EOF)
				{
					packetTimestampMap.emplace(frameIndex++, avPacket.pts);
				}

				const int sendPacketResult = avcodec_send_packet(avVideoCodecContext_, &avPacket);

				if (sendPacketResult == 0)
				{
					int receiveFrameResult = 0;

					while (receiveFrameResult == 0)
					{
						receiveFrameResult = avcodec_receive_frame(avVideoCodecContext_, avFrame);

						if (receiveFrameResult == 0)
						{
							const PacketTimestampMap::const_iterator iPacket = packetTimestampMap.find(int64_t(avFrame->coded_picture_number));

							if (iPacket != packetTimestampMap.cend())
							{
								const int64_t presentationTimestamp = iPacket->second;
								packetTimestampMap.erase(iPacket);

								Frame frame = extractFrame(avFrame, avVideoCodecContext_->pix_fmt, avVideoCodecContext_->color_range);
								ocean_assert(frame.isValid());

								const double relativePresentationTimestamp = double(presentationTimestamp) * double(avTimeBase.num) / double(avTimeBase.den);

								const float speed = speed_.load();
								ocean_assert(speed >= 0.0f);

								Timestamp displayTimestamp;

								if (speed != 0.0f)
								{
									displayTimestamp = Timestamp(startTimestamp + relativePresentationTimestamp / double(speed));

									while (shouldThreadStop() == false && Timestamp(true) < displayTimestamp)
									{
										sleep(1u);
									}
								}
								else
								{
									displayTimestamp.toNow();
								}

								position_ = relativePresentationTimestamp;

								frame.setTimestamp(displayTimestamp);
								frame.setRelativeTimestamp(Timestamp(relativePresentationTimestamp));

								deliverNewFrame(std::move(frame));
							}
							else
							{
								Log::error() << "FFmpeg: Missing packet timestamp for picture " << avFrame->coded_picture_number;
							}
						}
						else if (receiveFrameResult == AVERROR(EAGAIN) || receiveFrameResult == AVERROR_EOF)
						{
							// nothing to receive right now, need to send a new AvPacket first
						}
						else
						{
							Log::error() << "FFmpeg: Failed to receive frame: " << av_err2str(receiveFrameResult);
						}
					}
				}
				else
				{
					Log::error() << "FFmpeg: Failed to send AvPaket to codec: " << av_err2str(sendPacketResult);
				}
			}

			if (readFrameResult == AVERROR_EOF)
			{
				if (!packetTimestampMap.empty())
				{
					Log::warning() << "FFmpeg: Reached end of file but " << packetTimestampMap.size() << " frame(s) have not been delivered";
				}

				const ScopedLock scopedLock(lock_);

				if (loop_)
				{
					const int seekResult = avformat_seek_file(avFormatContext_, avVideoStreamIndex_, 0, 0, avFormatContext_->streams[avVideoStreamIndex_]->duration, 0);

					if (seekResult < 0)
					{
						Log::error() << "FFmpeg: Failed to restart movie '" << url() << "': " << av_err2str(seekResult);
						break;
					}

					// Reset the internal codec state / flush internal buffers.
					avcodec_flush_buffers(avVideoCodecContext_);

					continue;
				}
				else
				{
					// we have reached the end of the movie

					startTimestamp_.toInvalid();
					pauseTimestamp_.toInvalid();
					stopTimestamp_.toNow();

					break;
				}
			}
		}
		else
		{
			Log::error() << "FFmpeg: Failed to read frame for '" << url() << "': " << av_err2str(readFrameResult);
			break;
		}
	}
}

bool FFMMovie::createAndOpenVideoCodec()
{
	ocean_assert(avFormatContext_ != nullptr);

	ocean_assert(avVideoCodecContext_ == nullptr);

	const AVCodec* avCodec = nullptr;

	for (int streamIndex = 0; streamIndex < avFormatContext_->nb_streams; ++streamIndex)
	{
		AVStream* avStream = avFormatContext_->streams[streamIndex];
		AVCodecParameters* avCodecParameters = avStream->codecpar;

		if (avCodecParameters->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			const AVCodec* avCandidateCodec = avcodec_find_decoder(avStream->codecpar->codec_id);

			if (avCandidateCodec != nullptr)
			{
				avVideoCodecContext_ = avcodec_alloc_context3(avCandidateCodec);

				if (avVideoCodecContext_ != nullptr)
				{
					const int result = avcodec_parameters_to_context(avVideoCodecContext_, avCodecParameters);

					if (result < 0)
					{
						Log::error() << "FFmpeg: Failed to copy codec parameters to context for '" << url() << "': " << av_err2str(avVideoStreamIndex_);
						return false;
					}

					avVideoStreamIndex_ = streamIndex;
					avCodec = avCandidateCodec;
					break;
				}
			}
		}
	}

	if (avVideoStreamIndex_ < 0)
	{
		Log::error() << "FFmpeg: Failed to find video codec for '" << url() << "'";
		return false;
	}

	ocean_assert(avVideoCodecContext_ != nullptr);

	ocean_assert(avVideoStream_ == nullptr);
	avVideoStream_ = avFormatContext_->streams[avVideoStreamIndex_];
	ocean_assert(avVideoStream_ != nullptr);

	if (translatePixelFormat(avVideoCodecContext_->pix_fmt, avVideoCodecContext_->color_range) == FrameType::FORMAT_UNDEFINED)
	{
		Log::error() << "FFmpeg: The pixel format " << avVideoCodecContext_->pix_fmt << " is not supported";
		return false;
	}

	const AVRational avTimeBase = avVideoStream_->time_base;

	if (avTimeBase.num < 0 || avTimeBase.den <= 0)
	{
		Log::error() << "FFmpeg: Invalid time base";
		return false;
	}

	normalDuration_ = double(avFormatContext_->streams[avVideoStreamIndex_]->duration) * double(avTimeBase.num) / double(avTimeBase.den);

	const int result = avcodec_open2(avVideoCodecContext_, avCodec, nullptr /*options*/);

	if (result < 0)
	{
		Log::error() << "FFmpeg: Failed to open video stream '" << url() << "': " << av_err2str(result);
		return false;
	}

	return true;
}

void FFMMovie::releaseVideoCodec()
{
	if (avVideoCodecContext_ != nullptr)
	{
		avcodec_free_context(&avVideoCodecContext_);
		avVideoCodecContext_ = nullptr;
	}

	avVideoStream_ = nullptr;
	avVideoStreamIndex_ = -1;

	normalDuration_ = -1.0;
}

Frame FFMMovie::extractFrame(AVFrame* avFrame, const int avPixelFormat, const int avColorRange)
{
	ocean_assert(avFrame != nullptr && avPixelFormat >= 0);

	const FrameType::PixelFormat pixelFormat = translatePixelFormat(avPixelFormat, avColorRange);
	ocean_assert(pixelFormat != FrameType::FORMAT_UNDEFINED);

	if (pixelFormat == FrameType::FORMAT_UNDEFINED)
	{
		return Frame();
	}

	const unsigned int numberPlanes = FrameType::numberPlanes(pixelFormat);
	ocean_assert(numberPlanes >= 1u && numberPlanes <= AV_NUM_DATA_POINTERS);

	Frame::PlaneInitializers<uint8_t> planeInitializers;
	planeInitializers.reserve(numberPlanes);

	const int avWidth = avFrame->width;
	const int avHeight = avFrame->height;

	if (avWidth <= 0 || avHeight <= 0)
	{
		ocean_assert(false && "This should never happen!");
		return Frame();
	}

	const unsigned int width = (unsigned int)(avWidth);
	const unsigned int height = (unsigned int)(avHeight);

	ocean_assert(width % FrameType::widthMultiple(pixelFormat) == 0u);
	ocean_assert(height % FrameType::heightMultiple(pixelFormat) == 0u);

	if (width % FrameType::widthMultiple(pixelFormat) != 0u || height % FrameType::heightMultiple(pixelFormat) != 0u)
	{
		Log::error() << "FFmpeg: Invalid frame resolution";
		return Frame();
	}

	const FrameType frameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT);

	for (unsigned int planeIndex = 0u; planeIndex < numberPlanes; ++planeIndex)
	{
		const int avLineSize = avFrame->linesize[planeIndex];

		if (avLineSize <= 0)
		{
			ocean_assert(false && "This should never happen!");
			return Frame();
		}

		unsigned int planePaddingElements = 0u;
		if (Frame::strideBytes2paddingElements(pixelFormat, width, (unsigned int)(avLineSize), planePaddingElements, planeIndex))
		{
			planeInitializers.emplace_back(avFrame->data[planeIndex], Frame::CM_COPY_REMOVE_PADDING_LAYOUT, planePaddingElements);
		}
		else
		{
			Log::error() << "FFmpeg: Invalid plane stride";
			return Frame();
		}
	}

	return Frame(frameType, planeInitializers);
}

FrameType::PixelFormat FFMMovie::translatePixelFormat(const int avPixelFormat, const int avColorRange)
{
	ocean_assert(avPixelFormat >= 0);

	switch (avPixelFormat)
	{
		case AV_PIX_FMT_YUV420P:
		{
			if (avColorRange == AVCOL_RANGE_JPEG)
			{
				return FrameType::FORMAT_Y_U_V12_FULL_RANGE;
			}

			ocean_assert(avColorRange == AVCOL_RANGE_UNSPECIFIED || avColorRange == AVCOL_RANGE_MPEG);
			return FrameType::FORMAT_Y_U_V12_LIMITED_RANGE;
		}

		case AV_PIX_FMT_YUYV422:
			return FrameType::FORMAT_YUYV16;

		case AV_PIX_FMT_RGB24:
			return FrameType::FORMAT_RGB24;

		case AV_PIX_FMT_BGR24:
			return FrameType::FORMAT_BGR24;

		case AV_PIX_FMT_GRAY8:
			return FrameType::FORMAT_Y8;

		case AV_PIX_FMT_NV12:
		{
			if (avColorRange == AVCOL_RANGE_JPEG)
			{
				return FrameType::FORMAT_Y_UV12_FULL_RANGE;
			}

			ocean_assert(avColorRange == AVCOL_RANGE_UNSPECIFIED || avColorRange == AVCOL_RANGE_MPEG);
			return FrameType::FORMAT_Y_UV12_LIMITED_RANGE;
		}

		case AV_PIX_FMT_NV21:
		{
			if (avColorRange == AVCOL_RANGE_JPEG)
			{
				return FrameType::FORMAT_Y_VU12_FULL_RANGE;
			}

			ocean_assert(avColorRange == AVCOL_RANGE_UNSPECIFIED || avColorRange == AVCOL_RANGE_MPEG);
			return FrameType::FORMAT_Y_VU12_LIMITED_RANGE;
		}

		case AV_PIX_FMT_ARGB:
			return FrameType::FORMAT_ARGB32;

		case AV_PIX_FMT_RGBA:
			return FrameType::FORMAT_RGBA32;

		case AV_PIX_FMT_ABGR:
			return FrameType::FORMAT_ABGR32;

		case AV_PIX_FMT_BGRA:
			return FrameType::FORMAT_BGRA32;

		default:
			break;
	}

#ifdef OCEAN_DEBUG
	const char* readablePixelFormat = av_get_pix_fmt_name(AVPixelFormat(avPixelFormat));

	Log::debug() << "FFMMovie: Pixel format not supported: " << readablePixelFormat;
#endif

	ocean_assert(false && "Pixel format not supported!");
	return FrameType::FORMAT_UNDEFINED;
}

}

}

}
