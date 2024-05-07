// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "ocean/media/mediafoundation/MFFrameMedium.h"
#include "ocean/media/mediafoundation/MFFiniteMedium.h"
#include "ocean/media/mediafoundation/Utilities.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/math/Numeric.h"

#include "ocean/system/Performance.h"

#include <Dvdmedia.h>

namespace Ocean
{

namespace Media
{

namespace MediaFoundation
{

MFFrameMedium::MFFrameMedium(const std::string& url) :
	Medium(url),
	MFMedium(url),
	FrameMedium(url)
{
	if (System::Performance::get().performanceLevel() > System::Performance::LEVEL_MEDIUM)
	{
		frameCollection_ = FrameCollection(10);
	}
	else
	{
		frameCollection_ = FrameCollection(5);
	}
}

MFFrameMedium::~MFFrameMedium()
{
	frameCollection_.release();

	releaseFrameTopology();

	ocean_assert(frameSampleGrabber_.object() == nullptr);
}

bool MFFrameMedium::setPreferredFrameDimension(const unsigned int width, const unsigned int height)
{
	const ScopedLock scopedLock(lock_);

	if (startTimestamp_.isValid())
	{
		return false;
	}

	if (width == preferredFrameType_.width() && height == preferredFrameType_.height())
	{
		return true;
	}

	releaseFrameTopology();
	releaseTopology();

	preferredFrameType_ = MediaFrameType(preferredFrameType_, width, height);

	ocean_assert(topology_.object() == nullptr);
	return createTopology(respectPlaybackTime_);
}

bool MFFrameMedium::setPreferredFramePixelFormat(const FrameType::PixelFormat format)
{
	const ScopedLock scopedLock(lock_);

	if (startTimestamp_.isValid())
	{
		return false;
	}

	if (format == preferredFrameType_.pixelFormat())
	{
		return true;
	}

	releaseFrameTopology();
	releaseTopology();

	preferredFrameType_ = MediaFrameType(preferredFrameType_, format);

	ocean_assert(topology_.object() == nullptr);
	return createTopology(respectPlaybackTime_);
}

bool MFFrameMedium::setPreferredFrameFrequency(const FrameFrequency frequency)
{
	const ScopedLock scopedLock(lock_);

	if (startTimestamp_.isValid())
	{
		return false;
	}

	if (frequency < 0.0f || frequency > 10000.0f)
	{
		return false;
	}

	if (frequency == preferredFrameType_.frequency())
	{
		return true;
	}

	releaseFrameTopology();
	releaseTopology();

	preferredFrameType_.setFrequency(frequency);

	ocean_assert(topology_.object() == nullptr);
	return createTopology(respectPlaybackTime_);
}

bool MFFrameMedium::extractFrameFormat(IMFMediaType* mediaType, MediaFrameType& frameType)
{
	ocean_assert(mediaType);

	GUID majorType;
	if (S_OK != mediaType->GetMajorType(&majorType) || majorType != MFMediaType_Video)
	{
		return false;
	}

	GUID mediaSubType;
	if (S_OK != mediaType->GetGUID(MF_MT_SUBTYPE, &mediaSubType))
	{
		return false;
	}

	const FrameType::PixelFormat pixelFormat = Utilities::convertMediaSubtype(mediaSubType);
	if (pixelFormat == FrameType::FORMAT_UNDEFINED)
	{
		return false;
	}

	frameType = MediaFrameType(frameType, pixelFormat, Utilities::extractPixelOrigin(mediaSubType));

	unsigned int width = 0u;
	unsigned int height = 0u;
	if (S_OK == MFGetAttributeSize(mediaType, MF_MT_FRAME_SIZE, &width, &height))
	{
		frameType = MediaFrameType(frameType, width, height);
	}

	unsigned int numerator = 0u;
	unsigned int denominator = 0u;
	if (S_OK == MFGetAttributeRatio(mediaType, MF_MT_FRAME_RATE, &numerator, &denominator))
	{
		ocean_assert(denominator != 0u);
		frameType.setFrequency(FrameMedium::FrameFrequency(numerator) / FrameMedium::FrameFrequency(denominator));
	}

	MFVideoArea videoArea;
	UINT32 blobSize = 0;
	if (S_OK == mediaType->GetBlob(MF_MT_GEOMETRIC_APERTURE, (UINT8*)(&videoArea), sizeof(videoArea), &blobSize) && sizeof(videoArea) == blobSize)
	{
		const int offsetX = NumericD::round32(double(videoArea.OffsetX.value) + double(videoArea.OffsetX.fract) / 65536.0);
		const int offsetY = NumericD::round32(double(videoArea.OffsetY.value) + double(videoArea.OffsetY.fract) / 65536.0);

		ocean_assert(offsetX >= 0 && offsetY >= 0);

		if (offsetX != 0 || offsetY != 0 || videoArea.Area.cx != LONG(width) || videoArea.Area.cy != LONG(height))
		{
			frameType.setCroppingArea((unsigned int)(offsetX), (unsigned int)(offsetY), (unsigned int)(videoArea.Area.cx), (unsigned int)(videoArea.Area.cy));
		}
	}

	return true;
}

ScopedIMFMediaType MFFrameMedium::createMediaType(const MediaFrameType& frameType)
{
	ocean_assert(frameType.isValid());

	ScopedIMFMediaType mediaType;

	HRESULT result = MFCreateMediaType(&mediaType.resetObject());
	ocean_assert(result == S_OK);

	// set the major media type
	mediaType.object()->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);

	if (frameType.pixelFormat() != FrameType::FORMAT_UNDEFINED)
	{
		const GUID mediaSubType = Utilities::convertPixelFormat(frameType.pixelFormat());
		if (mediaSubType == GUID_NULL)
		{
			return ScopedIMFMediaType();
		}

		result = mediaType.object()->SetGUID(MF_MT_SUBTYPE, mediaSubType);
		ocean_assert(result == S_OK);
	}

	if (frameType.width() != 0u && frameType.height() != 0u)
	{
		result = MFSetAttributeSize(mediaType.object(), MF_MT_FRAME_SIZE, frameType.width(), frameType.height());
		ocean_assert(result == S_OK);

		mediaType.object()->SetUINT32(MF_MT_AVG_BITRATE, frameType.frameTypeSize());
	}

	if (frameType.frequency() > 0)
	{
		unsigned int numerator = 0u;
		unsigned int denominator = 0u;

		const UINT64 averageTimePerFrame = UINT64(FrameFrequency(1.0e+7) / frameType.frequency()); // in 100ns

		result = MFAverageTimePerFrameToFrameRate(averageTimePerFrame, &numerator, &denominator);
		ocean_assert(result == S_OK);

		MFSetAttributeRatio(mediaType.object(), MF_MT_FRAME_RATE, numerator, denominator);
	}

	// we have uncompressed media types
	mediaType.object()->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, TRUE);

	// aspect ratio: 1-1
	MFSetAttributeRatio(mediaType.object(), MF_MT_PIXEL_ASPECT_RATIO, 1u, 1u);

	mediaType.object()->SetUINT32(MF_MT_INTERLACE_MODE, 7);

	return mediaType;
}

bool MFFrameMedium::determineMediaType(IMFTopology* topology, FrameMedium::MediaFrameType& mediaFrameType)
{
	ocean_assert(topology != nullptr);

	bool success = false;

	WORD numberNodes = 0;
	if (topology->GetNodeCount(&numberNodes) == S_OK)
	{
		for (WORD i = 0; i < numberNodes; i++)
		{
			ScopedIMFTopologyNode node;
			if (topology->GetNode(i, &node.resetObject()) == S_OK)
			{
				MF_TOPOLOGY_TYPE type;
				if (node.object()->GetNodeType(&type) == S_OK)
				{
					if (type == MF_TOPOLOGY_OUTPUT_NODE && determineMediaType(node.object(), mediaFrameType))
					{
						i = numberNodes;
						success = true;
					}
				}
			}
		}
	}

	return success;
}

bool MFFrameMedium::determineMediaType(IMFTopologyNode* node, FrameMedium::MediaFrameType& mediaFrameType)
{
	ocean_assert(node != nullptr);

	bool success = false;
	ScopedIMFStreamSink streamSink = Utilities::topologyNodeObject<IMFStreamSink>(node);

	if (streamSink.object() != nullptr)
	{
		ScopedIMFMediaTypeHandler mediaTypeHandler;

		if (S_OK == streamSink.object()->GetMediaTypeHandler(&mediaTypeHandler.resetObject()))
		{
			ScopedIMFMediaType mediaType;

			if (S_OK == mediaTypeHandler.object()->GetCurrentMediaType(&mediaType.resetObject()))
			{
				if (extractFrameFormat(mediaType.object(), mediaFrameType))
				{
					success = true;
				}
			}
		}
	}

	return success;
}

bool MFFrameMedium::recentMediaSampleTimestamp(LONGLONG& timestamp, LONGLONG* nextTimestamp) const
{
	timestamp = frameSampleGrabber_.object()->recentSampleTime();

	if (timestamp == LONGLONG(-1))
	{
		return false;
	}

	if (nextTimestamp != nullptr)
	{
		const double frequency = double(frameFrequency());

		if (frequency <= 0.0)
		{
			return false;
		}

		const LONGLONG durationPerFrame = LONGLONG(1.0e+7 / frequency);

		*nextTimestamp = timestamp + durationPerFrame;
	}

	return true;
}

void MFFrameMedium::onNewSample(const GUID& /*guidMajorMediaType*/, const unsigned int /*dwSampleFlags*/, const long long llSampleTime, const long long llSampleDuration, const void* pSampleBuffer, const unsigned int /*dwSampleSize*/)
{
	// check whether the duration indicates an invalid frame, sometimes the sample duration is 1 during a seeking operation so that the real frame is coming in the next sample
	if (llSampleDuration == 1)
	{
		return;
	}

	// the event for a changed frame type is not synchronous with the sample event
	// therefore the event for a changed frame type is too late for this sample
	// here we use a workaround: for the very first frame we check the frame type again
	if (waitingForFirstFrame_)
	{
		onTopologySet(topology_.object());
		waitingForFirstFrame_ = false;
	}

	if (recentFrameType_.isValid())
	{
		if (pSampleBuffer == nullptr)
		{
			MFFiniteMedium* finiteMedium = dynamic_cast<MFFiniteMedium*>(this);

			if (finiteMedium)
			{
				finiteMedium->mediumHasStopped();
			}
		}
		else
		{
			const Timestamp timestamp(true);

			Frame::PlaneInitializers<void> planeInitializers;
			planeInitializers.reserve(internalRecentFrameType_.numberPlanes());

			unsigned int memoryOffset = 0u;

			for (unsigned int planeIndex = 0u; planeIndex < internalRecentFrameType_.numberPlanes(); ++planeIndex)
			{
				unsigned int planeWidth = 0u;
				unsigned int planeHeight = 0u;
				unsigned int planeChannels = 0u;

				if (FrameType::planeLayout(internalRecentFrameType_, planeIndex, planeWidth, planeHeight, planeChannels))
				{
					const void* planePointer = (const void*)((const uint8_t*)(pSampleBuffer) + memoryOffset);

					constexpr unsigned int planePaddingElements = 0u;
					planeInitializers.emplace_back(planePointer, Frame::CM_USE_KEEP_LAYOUT, planePaddingElements);

					const unsigned int planeSizeBytes = planeWidth * planeChannels * planeHeight * internalRecentFrameType_.bytesPerDataType();
					memoryOffset += planeSizeBytes;
				}
				else
				{
					ocean_assert(false && "This should never happen!");
					return;
				}
			}

			Frame internalFrame(internalRecentFrameType_, planeInitializers, timestamp);

			if (recentFrameType_ != internalRecentFrameType_)
			{
				ocean_assert(croppingWidth_ != 0u);
				ocean_assert(croppingHeight_ != 0u);

				Frame externalFrame(internalFrame.subFrame(croppingLeft_, croppingTop_, croppingWidth_, croppingHeight_, Frame::CM_COPY_REMOVE_PADDING_LAYOUT));

				internalFrame = std::move(externalFrame);
			}
			else
			{
				ocean_assert(croppingWidth_ == 0u);
				ocean_assert(croppingHeight_ == 0u);

				internalFrame.makeOwner();
			}

			const double relativeTimestamp(double(llSampleTime) / 1.0e+7);
			internalFrame.setRelativeTimestamp(Timestamp(relativeTimestamp));

			ocean_assert(internalFrame.isOwner());

			deliverNewFrame(std::move(internalFrame));
		}
	}
}

void MFFrameMedium::onTopologySet(IMFTopology* topology)
{
	ocean_assert(topology);

	waitingForFirstFrame_ = true;

	MediaFrameType mediaFrameType;
	if (determineMediaType(topology, mediaFrameType))
	{
		recentFrameType_ = mediaFrameType;
		internalRecentFrameType_ = mediaFrameType;

		recentFrameFrequency_ = mediaFrameType.frequency();

		if (mediaFrameType.croppingWidth() != 0u && mediaFrameType.croppingHeight() != 0u)
		{
			recentFrameType_ = FrameType(recentFrameType_, mediaFrameType.croppingWidth(), mediaFrameType.croppingHeight());

			croppingLeft_ = mediaFrameType.croppingLeft();
			croppingTop_ = mediaFrameType.croppingTop();
			croppingWidth_ = mediaFrameType.croppingWidth();
			croppingHeight_ = mediaFrameType.croppingHeight();
		}
	}
}

void MFFrameMedium::onFormatTypeChanged(const TOPOID nodeId)
{
	ocean_assert(topology_.object() != nullptr);

	ScopedIMFTopologyNode topologyNode;
	if (S_OK == topology_.object()->GetNodeByID(nodeId, &topologyNode.resetObject()))

	{
		MediaFrameType mediaFrameType;
		if (determineMediaType(topologyNode.object(), mediaFrameType))
		{
			recentFrameType_ = mediaFrameType;
			internalRecentFrameType_ = mediaFrameType;

			recentFrameFrequency_ = mediaFrameType.frequency();

			if (mediaFrameType.croppingWidth() != 0u && mediaFrameType.croppingHeight() != 0u)
			{
				recentFrameType_ = FrameType(recentFrameType_, mediaFrameType.croppingWidth(), mediaFrameType.croppingHeight());

				croppingLeft_ = mediaFrameType.croppingLeft();
				croppingTop_ = mediaFrameType.croppingTop();
				croppingWidth_ = mediaFrameType.croppingWidth();
				croppingHeight_ = mediaFrameType.croppingHeight();
			}
		}
	}
}

bool MFFrameMedium::buildFrameTopology(const bool respectPlaybackTime)
{
	ocean_assert(topology_.object() != nullptr && mediaSession_.object() != nullptr && mediaSource_.object() != nullptr);

	waitingForFirstFrame_ = true;

	ScopedIMFMediaType mediaType;

	if (preferredFrameType_.pixelFormat() != FrameType::FORMAT_UNDEFINED)
	{
		mediaType = createMediaType(preferredFrameType_);
	}
	else
	{
		MFCreateMediaType(&mediaType.resetObject());

		mediaType.object()->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
		mediaType.object()->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, TRUE);
	}

	ocean_assert(frameSampleGrabber_.object() == nullptr);
	frameSampleGrabber_ = Media::MediaFoundation::SampleGrabber::createInstance(Media::MediaFoundation::SampleGrabber::SampleCallback::create(*this, &MFFrameMedium::onNewSample));

	if (frameSampleGrabber_.object() == nullptr)
	{
		return false;
	}

	bool noError = true;

	ScopedIMFActivate sinkActivate;
	if (noError && S_OK != MFCreateSampleGrabberSinkActivate(mediaType.object(), frameSampleGrabber_.object(), &sinkActivate.resetObject()))
	{
		noError = false;
	}

	if (noError && S_OK != sinkActivate.object()->SetUINT32(MF_SAMPLEGRABBERSINK_IGNORE_CLOCK, respectPlaybackTime ? FALSE : TRUE))
	{
		noError = false;
	}

	if (noError && !Media::MediaFoundation::Utilities::connectSelectedStream(topology_.object(), mediaSource_.object(), sinkActivate.object(), MFMediaType_Video))
	{
		noError = false;
	}

	if (!noError)
	{
		frameSampleGrabber_.release();
	}

	return noError;
}

void MFFrameMedium::releaseFrameTopology()
{
	if (frameSampleGrabber_.object() != nullptr)
	{
		frameSampleGrabber_.object()->deactivate();
	}

	frameSampleGrabber_.release();
}

}

}

}
