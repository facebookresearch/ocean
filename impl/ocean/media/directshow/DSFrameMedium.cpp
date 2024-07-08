/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/directshow/DSFrameMedium.h"
#include "ocean/media/directshow/DSFiniteMedium.h"

#include "ocean/base/ScopedFunction.h"

#include "ocean/io/CameraCalibrationManager.h"

#include "ocean/math/PinholeCamera.h"

#include "ocean/system/Performance.h"

DISABLE_WARNINGS_BEGIN
	#include <Dvdmedia.h>
DISABLE_WARNINGS_END

namespace Ocean
{

namespace Media
{

namespace DirectShow
{

DSFrameMedium::DSSortableFrameType::DSSortableFrameType(DSMediaType&& dsMediaType, const MediaFrameType& frameType) :
	SortableFrameType(frameType),
	dsMediaType_(std::move(dsMediaType))
{
	const bool result = DSFrameMedium::extractFrameFormat(dsMediaType_.type(), actualFrameType_);
	ocean_assert_and_suppress_unused(result == true, result);
}

const AM_MEDIA_TYPE& DSFrameMedium::DSSortableFrameType::type() const
{
	return dsMediaType_.type();
}

DSFrameMedium::DSFrameMedium(const std::string& url) :
	Medium(url),
	DSMedium(url),
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

DSFrameMedium::~DSFrameMedium()
{
	frameCollection_.clear();

	ocean_assert(!sampleSinkFilter_.isValid());
}

bool DSFrameMedium::setPreferredFrameDimension(const unsigned int width, const unsigned int height)
{
	if (graphIsRunning_)
	{
		return false;
	}

	if (width == preferredFrameType_.width() && height == preferredFrameType_.height())
	{
		return true;
	}

	releaseGraph();

	preferredFrameType_ = MediaFrameType(preferredFrameType_, width, height);

	return buildGraph();
}

bool DSFrameMedium::setPreferredFramePixelFormat(const FrameType::PixelFormat format)
{
	if (graphIsRunning_)
	{
		return false;
	}

	if (format == preferredFrameType_.pixelFormat())
	{
		return true;
	}

	releaseGraph();

	preferredFrameType_ = MediaFrameType(preferredFrameType_, format);

	return buildGraph();
}

bool DSFrameMedium::setPreferredFrameFrequency(const FrameFrequency frequency)
{
	if (frequency < 0.0 || frequency > 10000.0)
	{
		return false;
	}

	if (frequency == preferredFrameType_.frequency())
	{
		return true;
	}

	releaseGraph();

	preferredFrameType_.setFrequency(frequency);

	return buildGraph();
}

bool DSFrameMedium::extractFrameFormat(const AM_MEDIA_TYPE& mediaType, MediaFrameType& frameType)
{
	frameType = MediaFrameType(frameType, convertMediaSubtype(mediaType.subtype), extractPixelOrigin(mediaType.subtype));

	if (mediaType.formattype == FORMAT_VideoInfo)
	{
		ocean_assert(sizeof(VIDEOINFOHEADER) <= mediaType.cbFormat);
		const VIDEOINFOHEADER& videoInfo = *(VIDEOINFOHEADER*)mediaType.pbFormat;

		frameType = MediaFrameType(frameType, (unsigned int)(videoInfo.bmiHeader.biWidth), (unsigned int)(videoInfo.bmiHeader.biHeight));

		if (videoInfo.AvgTimePerFrame <= 0)
		{
			frameType.setFrequency(0.0);
		}
		else
		{
			frameType.setFrequency(10000000.0 / double(videoInfo.AvgTimePerFrame));
		}
	}
	else if (mediaType.formattype == FORMAT_VideoInfo2)
	{
		ocean_assert(sizeof(VIDEOINFOHEADER2) <= mediaType.cbFormat);
		const VIDEOINFOHEADER2& videoInfo = *(VIDEOINFOHEADER2*)mediaType.pbFormat;

		frameType = MediaFrameType(frameType, (unsigned int)(videoInfo.bmiHeader.biWidth), (unsigned int)(videoInfo.bmiHeader.biHeight));

		if (videoInfo.AvgTimePerFrame <= 0)
		{
			frameType.setFrequency(0.0);
		}
		else
		{
			frameType.setFrequency(10000000.0 / double(videoInfo.AvgTimePerFrame));
		}
	}
	else
	{
		return false;
	}

	return true;
}

bool DSFrameMedium::createMediaType(CMediaType& mediaType, const MediaFrameType& frameType)
{
	unsigned int planeWidth;
	unsigned int planeHeight;
	unsigned int planeChannels;
	if (frameType.numberPlanes() != 1u || !FrameType::planeLayout(frameType, 0u, planeWidth, planeHeight, planeChannels))
	{
		ocean_assert(false && "Not supported frame type!");
		return false;
	}

	const unsigned int bitsPerPixel = planeChannels * frameType.bytesPerDataType() * 8u;

	mediaType.SetType(&MEDIATYPE_Video);
	mediaType.SetFormatType(&FORMAT_VideoInfo);

	VIDEOINFOHEADER& videoInfo = *(VIDEOINFOHEADER*)mediaType.AllocFormatBuffer(sizeof(VIDEOINFOHEADER));
	// Initialize the VideoInfo structure before configuring its members
	ZeroMemory(&videoInfo, sizeof(VIDEOINFOHEADER));

	videoInfo.bmiHeader.biCompression = BI_RGB;
	videoInfo.bmiHeader.biBitCount = WORD(bitsPerPixel);
	videoInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	videoInfo.bmiHeader.biWidth = LONG(frameType.width());
	videoInfo.bmiHeader.biHeight = LONG(frameType.pixelOrigin() == FrameType::ORIGIN_LOWER_LEFT ? frameType.height() : -LONG(frameType.height()));
	videoInfo.bmiHeader.biPlanes = 1;
	videoInfo.bmiHeader.biSizeImage = GetBitmapSize(&videoInfo.bmiHeader);
	videoInfo.bmiHeader.biClrImportant = 0;

	if (frameType.frequency() <= 0.0)
	{
		videoInfo.AvgTimePerFrame = 0;
	}
	else
	{
		videoInfo.AvgTimePerFrame = REFERENCE_TIME(10000000.0 / frameType.frequency());
	}

	mediaType.SetSampleSize(videoInfo.bmiHeader.biSizeImage);

	mediaType.SetTemporalCompression(false);
	mediaType.bFixedSizeSamples = FALSE;

	if (frameType.pixelFormat() != FrameType::FORMAT_UNDEFINED)
	{
		const GUID subType = convertPixelFormat(frameType.pixelFormat());
		mediaType.SetSubtype(&subType);
	}

	return true;
}

bool DSFrameMedium::modifyFrameFormat(AM_MEDIA_TYPE& mediaType, const MediaFrameType& frameType)
{
	if (frameType.pixelFormat() != FrameType::FORMAT_UNDEFINED)
	{
		mediaType.subtype = convertPixelFormat(frameType.pixelFormat());
	}

	FrameType::PixelOrigin possiblePixelOrigin = extractPixelOrigin(mediaType.subtype);
	if (possiblePixelOrigin != FrameType::ORIGIN_INVALID && frameType.pixelOrigin() != frameType.pixelOrigin())
	{
		return false;
	}

	if (mediaType.formattype == FORMAT_VideoInfo)
	{
		ocean_assert(sizeof(VIDEOINFOHEADER) <= mediaType.cbFormat);
		VIDEOINFOHEADER& videoInfo = *(VIDEOINFOHEADER*)mediaType.pbFormat;

		videoInfo.bmiHeader.biWidth = LONG(frameType.width());
		videoInfo.bmiHeader.biHeight = LONG(frameType.height());

		videoInfo.AvgTimePerFrame = REFERENCE_TIME(10000000.0 / double(frameType.frequency()));
	}
	else if (mediaType.formattype == FORMAT_VideoInfo2)
	{
		ocean_assert(sizeof(VIDEOINFOHEADER2) <= mediaType.cbFormat);
		VIDEOINFOHEADER2& videoInfo = *(VIDEOINFOHEADER2*)mediaType.pbFormat;

		videoInfo.bmiHeader.biWidth = LONG(frameType.width());
		videoInfo.bmiHeader.biHeight = LONG(frameType.height());

		videoInfo.AvgTimePerFrame = REFERENCE_TIME(10000000.0 / double(frameType.frequency()));
	}
	else
	{
		return false;
	}

	return true;
}

bool DSFrameMedium::collectFrameFormats(IPin* pin, FrameTypes& frameTypes)
{
	if (pin == nullptr)
	{
		return false;
	}

	ScopeDirectShowObject<IEnumMediaTypes> enumerator;
	if (S_OK != pin->EnumMediaTypes(&enumerator.resetObject()))
	{
		return false;
	}

	MediaFrameType frameType;
	AM_MEDIA_TYPE* type = nullptr;

	while (S_OK == enumerator->Next(1, &type, nullptr))
	{
		if (extractFrameFormat(*type, frameType) && frameType.pixelFormat() != FrameType::FORMAT_UNDEFINED)
		{
			frameTypes.emplace_back(frameType);
		}

		FreeMediaType(*type);
		CoTaskMemFree(type);
	}

	return true;
}

FrameType::PixelFormat DSFrameMedium::convertMediaSubtype(const GUID& mediaSubtype)
{
	if (mediaSubtype == MEDIASUBTYPE_ARGB32)
		return FrameType::FORMAT_BGRA32;

	if (mediaSubtype == MEDIASUBTYPE_I420)
		return FrameType::FORMAT_Y_U_V12;

	if (mediaSubtype == MEDIASUBTYPE_IYUV)
		return FrameType::FORMAT_UNDEFINED;

	if (mediaSubtype == MEDIASUBTYPE_RGB24)
		return FrameType::FORMAT_BGR24;

	if (mediaSubtype == MEDIASUBTYPE_RGB32)
		return FrameType::FORMAT_BGR32;

	if (mediaSubtype == MEDIASUBTYPE_YUY2)
		return FrameType::FORMAT_YUYV16;

	if (mediaSubtype == MEDIASUBTYPE_NV12)
		return FrameType::FORMAT_Y_UV12;

	if (mediaSubtype == MEDIASUBTYPE_YV12)
		return FrameType::FORMAT_UNDEFINED;

	return FrameType::FORMAT_UNDEFINED;
}

FrameType::PixelOrigin DSFrameMedium::extractPixelOrigin(const GUID& mediaSubtype)
{
	if (mediaSubtype == MEDIASUBTYPE_ARGB32 || mediaSubtype == MEDIASUBTYPE_RGB24 || mediaSubtype == MEDIASUBTYPE_RGB32)
	{
		return FrameType::ORIGIN_LOWER_LEFT;
	}

	if (mediaSubtype == MEDIASUBTYPE_I420 || mediaSubtype == MEDIASUBTYPE_IYUV || mediaSubtype == MEDIASUBTYPE_YUY2 || mediaSubtype == MEDIASUBTYPE_YV12 || mediaSubtype == MEDIASUBTYPE_NV12)
	{
		return FrameType::ORIGIN_UPPER_LEFT;
	}

	return FrameType::ORIGIN_INVALID;
}

GUID DSFrameMedium::convertPixelFormat(const FrameType::PixelFormat pixelFormat)
{
	switch (pixelFormat)
	{
		case FrameType::FORMAT_BGR24:
			return MEDIASUBTYPE_RGB24;

		case FrameType::FORMAT_BGR32:
			return MEDIASUBTYPE_RGB32;

		case FrameType::FORMAT_BGRA32:
			return MEDIASUBTYPE_ARGB32;

		case FrameType::FORMAT_Y_U_V12:
			return MEDIASUBTYPE_I420;

		case FrameType::FORMAT_YUYV16:
			return MEDIASUBTYPE_YUY2;

		default:
			break;
	}

	return GUID_NULL;
}

bool DSFrameMedium::createFrameSampleSinkFilter()
{
	ocean_assert(filterGraph_.isValid());
	ocean_assert(graphIsRunning_ == false);

	ScopedIBaseFilter videoRendererFilter;

	// seeks for the DirectShow Video Renderer filter and replaces it with our own media sink filter
	if (S_OK != filterGraph_->FindFilterByName(L"Video Renderer", &videoRendererFilter.resetObject()))
	{
		Log::error() << "\"" << url_ << "\" holds no video.";

		return false;
	}

	const ScopedIPin videoRendererFilterInputPin = firstPin(*videoRendererFilter, PTYPE_INPUT, CTYPE_CONNECTED);
	const ScopedIPin outputPin = connectedPin(*videoRendererFilterInputPin);

	if (!videoRendererFilterInputPin.isValid() || !outputPin.isValid())
	{
		Log::error() << "Could not get the connected video pin.";

		return false;
	}

	if (S_OK != filterGraph_->RemoveFilter(*videoRendererFilter))
	{
		Log::error() << "Could not remove the video renderer filter.";

		return false;
	}

	if (!insertFrameSampleSinkFilter(*outputPin, preferredFrameType_.pixelFormat()))
	{
		Log::error() << "Could not establish the sample sink filter.";

		return false;
	}

	return true;
}

bool DSFrameMedium::insertFrameSampleSinkFilter(IPin* outputPin, FrameType::PixelFormat /*pixelFormat*/)
{
	ocean_assert(filterGraph_.isValid());
	ocean_assert(outputPin != nullptr);

	if (sampleSinkFilter_.isValid())
	{
		return true;
	}

	sampleSinkFilter_ = ScopedDSSampleSinkFilter(new DSSampleSinkFilter(L"Frame sample sink filter", DSSampleSinkFilter::SampleCallback(*this, &DSFrameMedium::onNewSample)));
	sampleSinkFilter_->AddRef();

	if (!sampleSinkFilter_.isValid())
	{
		Log::error() << "Could not create a sample sink filter.";

		return false;
	}

	ScopedFunctionVoid scopedReleaseFrameSampleSinkFilterFunction(std::bind(&DSFrameMedium::releaseFrameSampleSinkFilter, this)); // scoped function which will be invoked in case we don't reach the both of this function

	if (S_OK != filterGraph_->AddFilter(*sampleSinkFilter_, L"Frame sample sink filter"))
	{
		Log::error() << "Could not insert the frame sample sink filter.";

		return false;
	}

	sampleSinkFilter_->setRespectPlaybackTime(respectPlaybackTime_);

	/*if (sinkRespectPlaybackTime == false)
	{
		ScopedIMediaFilter mediaFilter;

		if (S_OK == filterGraph->QueryInterface(IID_IMediaFilter, (void**)(&mediaFilter.resetObject())))
		{
			const HRESULT result = mediaFilter->SetSyncSource(nullptr);
			ocean_assert(result == S_OK);
		}
	}*/

	DSMediaType requestedMediaType;
	requestedMediaType.type().majortype = MEDIATYPE_Video;

	if (preferredFrameType_.pixelFormat() != FrameType::FORMAT_UNDEFINED)
	{
		requestedMediaType.type().subtype = convertPixelFormat(preferredFrameType_.pixelFormat());
		sampleSinkFilter_->specifyMediaType(requestedMediaType.type());
	}

	requestedMediaType.type().subtype = MEDIASUBTYPE_RGB24;
	sampleSinkFilter_->specifyMediaType(requestedMediaType.type());

	//mediaType.subtype = MEDIASUBTYPE_YUYV;
	//mediaType.subtype = MEDIASUBTYPE_YV12;
	requestedMediaType.type().subtype = MEDIASUBTYPE_NV12;
	sampleSinkFilter_->specifyMediaType(requestedMediaType.type());

	if (!connectFilter(outputPin, *sampleSinkFilter_))
	{
		if (!connectFilter(outputPin, *sampleSinkFilter_))
		{
			requestedMediaType.type().subtype = MEDIASUBTYPE_YUY2;
			sampleSinkFilter_->specifyMediaType(requestedMediaType.type());

			if (!connectFilter(outputPin, *sampleSinkFilter_))
			{
				requestedMediaType.type().subtype = MEDIASUBTYPE_I420;
				sampleSinkFilter_->specifyMediaType(requestedMediaType.type());

				if (!connectFilter(outputPin, *sampleSinkFilter_))
				{
					Log::error() << "Could not connect the sample sink filter with the given output pin.";

					return false;
				}
			}
		}
	}

	DSMediaType establishedMediaType;
	if (sampleSinkFilter_->establishedMediaType(establishedMediaType) == false)
	{
		Log::error() << "Could not get the media type of the sample sink filter.";

		return false;
	}

	MediaFrameType type;
	if (extractFrameFormat(establishedMediaType.type(), type))
	{
		recentFrameType_ = type;
		recentFrameFrequency_ = type.frequency();

		recentAnyCamera_ = nullptr;

		if (recentFrameType_.isValid())
		{
			// we try to create a (static) camera profile based on the CameraCalibrationManager

			IO::CameraCalibrationManager::Quality quality = IO::CameraCalibrationManager::QUALITY_DEFAULT;
			const PinholeCamera camera = IO::CameraCalibrationManager::get().camera(url(), recentFrameType_.width(), recentFrameType_.height(), &quality);

			if (!recentAnyCamera_ || quality != IO::CameraCalibrationManager::QUALITY_DEFAULT)
			{
				recentAnyCamera_ = std::make_shared<AnyCameraPinhole>(camera);

				if (quality == IO::CameraCalibrationManager::QUALITY_DEFAULT)
				{
					Log::warning() << "Used default camera calibration for '" << url() << "'";
				}
			}
		}
	}
	else
	{
		Log::error() << "Could not extract a valid frame format from the sample sink.";

		return false;
	}

	scopedReleaseFrameSampleSinkFilterFunction.revoke(); // we don't want the release function to be called

	return true;
}

void DSFrameMedium::releaseFrameSampleSinkFilter()
{
	if (sampleSinkFilter_.isValid())
	{
		ocean_assert(filterGraph_.isValid());

		filterGraph_->RemoveFilter(*sampleSinkFilter_);
	}

	sampleSinkFilter_.release();
}

void DSFrameMedium::onNewSample(IMediaSample* sample, const Timestamp timestamp, const Timestamp relativeTimestamp)
{
	if (sample == nullptr)
	{
		DSFiniteMedium* finiteMedium = dynamic_cast<DSFiniteMedium*>(this);

		if (finiteMedium)
		{
			finiteMedium->hasStopped();
		}
	}
	else
	{
		ocean_assert(recentFrameType_.isValid());

		uint8_t* buffer = nullptr;
		if (S_OK == sample->GetPointer(&buffer))
		{
			const long bytes = sample->GetSize();

			if (NumericT<unsigned int>::isInsideValueRange(bytes))
			{
				if ((unsigned int)(bytes) <= recentFrameType_.frameTypeSize())
				{
					constexpr unsigned int planePaddingElements = 0u;

					Frame::PlaneInitializers<uint8_t> planeInitializers;

					for (unsigned int planeIndex = 0u; planeIndex < recentFrameType_.numberPlanes(); ++planeIndex)
					{
						planeInitializers.push_back(Frame::PlaneInitializer<uint8_t>((const uint8_t*)(buffer), Frame::CM_COPY_REMOVE_PADDING_LAYOUT, planePaddingElements));

						if (planeIndex + 1u < recentFrameType_.numberPlanes())
						{
							unsigned int planeWidth = 0u;
							unsigned int planeHeight = 0u;
							unsigned int planeChannels = 0u;
							if (FrameType::planeLayout(recentFrameType_, planeIndex, planeWidth, planeHeight, planeChannels))
							{
								const unsigned int planeSize = (planeWidth * planeChannels + planePaddingElements) * planeHeight;

								buffer += planeSize;
							}
							else
							{
								planeInitializers.clear();
							}
						}
					}

					if (!planeInitializers.empty())
					{
						Frame frame(recentFrameType_, planeInitializers, timestamp);
						frame.setRelativeTimestamp(relativeTimestamp);

						deliverNewFrame(std::move(frame), SharedAnyCamera(recentAnyCamera_));

						return;
					}
				}
			}

			ocean_assert(false && "Invalid sample buffer!");
		}
	}
}

bool DSFrameMedium::setRespectPlaybackTime(const bool respectPlaybackTime)
{
	if (respectPlaybackTime_ == respectPlaybackTime)
	{
		return true;
	}

	if (sampleSinkFilter_.isValid() && !sampleSinkFilter_->setRespectPlaybackTime(respectPlaybackTime))
	{
		return false;
	}

	respectPlaybackTime_ = respectPlaybackTime;

	if (respectPlaybackTime_ == false && filterGraph_.isValid())
	{
		ScopedIMediaFilter mediaFilter;

		if (S_OK == filterGraph_->QueryInterface(IID_IMediaFilter, (void**)(&mediaFilter.resetObject())))
		{
			const HRESULT result = mediaFilter->SetSyncSource(nullptr);
			ocean_assert_and_suppress_unused(result == S_OK, result);
		}
	}

	return true;
}

}

}

}
