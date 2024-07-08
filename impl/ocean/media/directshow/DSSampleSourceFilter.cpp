/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/directshow/DSSampleSourceFilter.h"
#include "ocean/media/directshow/DSFrameMedium.h"

#include "ocean/base/Exception.h"
#include "ocean/base/String.h"

namespace Ocean
{

namespace Media
{

namespace DirectShow
{

DSSampleSourceFilter::DSOutputPin::DSOutputPin(DSSampleSourceFilter* filter, HRESULT* result) :
	CSourceStream(L"OutputPin", result, filter, L"Output")
{
	ocean_assert(sampleFrequency_ > 0.0);
	sampleDuration_ = REFERENCE_TIME(10000000.0 / sampleFrequency_);
}

DSSampleSourceFilter::DSOutputPin::~DSOutputPin()
{
	// nothing to do here
}

HRESULT DSSampleSourceFilter::DSOutputPin::DecideBufferSize(IMemAllocator* allocator, ALLOCATOR_PROPERTIES* properties)
{
	if (allocator == nullptr || properties == nullptr)
	{
		return S_FALSE;
	}

	CAutoLock cAutoLock(m_pFilter->pStateLock());

	VIDEOINFO* videoInfo = (VIDEOINFO*)(m_mt.Format());

	properties->cBuffers = 1;
	properties->cbBuffer = videoInfo->bmiHeader.biSizeImage;

	ALLOCATOR_PROPERTIES actual;
	if (S_OK != allocator->SetProperties(properties, &actual))
	{
		return S_FALSE;
	}

	if (actual.cbBuffer < properties->cbBuffer)
	{
		return S_FALSE;
	}

	return S_OK;
}

HRESULT DSSampleSourceFilter::DSOutputPin::FillBuffer(IMediaSample* sample)
{
	ocean_assert(sample != nullptr);

	mediaSample_ = sample;

	while (true)
	{
		if (sampleSignal_.wait(100))
		{
			break;
		}

		ocean_assert(m_pFilter && dynamic_cast<DSSampleSourceFilter*>(m_pFilter) != nullptr);
		if (((DSSampleSourceFilter*)(m_pFilter))->filterState() != State_Running)
		{
			return S_FALSE;
		}
	}

	const ScopedLock scopedLock(lock_);

	sampleNumber_++;

	REFERENCE_TIME startTime = sampleNumber_ * sampleDuration_;
	REFERENCE_TIME stopTime = startTime + sampleDuration_;

	if (sampleFrequency_ > 0.0)
	{
		nextSampleTimestamp_ = runTimestamp_ + double(sampleNumber_) / sampleFrequency_;
	}
	else
	{
		nextSampleTimestamp_ = runTimestamp_;
	}

	sample->SetTime(&startTime, &stopTime);
	sample->SetSyncPoint(TRUE);

	return S_OK;
}

bool DSSampleSourceFilter::DSOutputPin::lockBufferToFill(void*& buffer, size_t& size, const bool respectSampleFrequency)
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isWaitingForBuffer_ == false);

	if (respectSampleFrequency && sampleFrequency_ > 0.0 && Timestamp(true) < nextSampleTimestamp_ - 0.25 / sampleFrequency_)
	{
		return false;
	}

	if (mediaSample_ != nullptr)
	{
		size = size_t(mediaSample_->GetSize());

		BYTE* sampleBuffer = nullptr;

		if (S_OK == mediaSample_->GetPointer(&sampleBuffer))
		{
			buffer = sampleBuffer;

#ifdef OCEAN_DEBUG
			isWaitingForBuffer_ = true;
#endif
			return true;
		}
	}

	size = 0;

	return false;
}

void DSSampleSourceFilter::DSOutputPin::unlockBufferToFill(const size_t size)
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isWaitingForBuffer_ == true);

	if (size != 0u)
	{
		mediaSample_->SetActualDataLength(long(size));
	}

	sampleSignal_.pulse();
	mediaSample_ = nullptr;

#ifdef OCEAN_DEBUG
	isWaitingForBuffer_ = false;
#endif
}

bool DSSampleSourceFilter::DSOutputPin::specifyMediaType(const AM_MEDIA_TYPE& mediaType, const double frequency)
{
	if (frequency < 0.0)
	{
		return false;
	}

	mediaType_ = mediaType;

	sampleFrequency_ = frequency;

	if (sampleFrequency_ == 0.0)
	{
		sampleDuration_ = REFERENCE_TIME(0);
	}
	else
	{
		sampleDuration_ = REFERENCE_TIME(10000000.0 / sampleFrequency_);
	}

	return true;
}

HRESULT DSSampleSourceFilter::DSOutputPin::Run(REFERENCE_TIME start)
{
	sampleNumber_ = 0u;

	runTimestamp_.toNow();
	nextSampleTimestamp_ = runTimestamp_;

	return CBasePin::Run(start);
}

HRESULT DSSampleSourceFilter::DSOutputPin::GetMediaType(CMediaType* mediaType)
{
	if (mediaType == nullptr)
	{
		return S_FALSE;
	}

	*mediaType = mediaType_;

	return S_OK;
}

STDMETHODIMP DSSampleSourceFilter::DSOutputPin::Notify(IBaseFilter * /*pSender*/, Quality /*q*/)
{
	return S_OK;
}

DSSampleSourceFilter::DSSampleSourceFilter(IUnknown* unknown) :
	CSource(L"Ocean sample source filter", unknown, __uuidof(CLSID_DSOceanSampleSourceFilter))
{
	HRESULT result;

	outputPin_ = ScopedDSOutputPin(new DSOutputPin(this, &result));
}

DSSampleSourceFilter::~DSSampleSourceFilter()
{
	/// the output pin must not be released explicitly
}

bool DSSampleSourceFilter::lockBufferToFill(void*& buffer, size_t& size, const bool respectFrameFrequency)
{
	ocean_assert(outputPin_.isValid());

	return outputPin_->lockBufferToFill(buffer, size, respectFrameFrequency);
}

void DSSampleSourceFilter::unlockBufferToFill(const size_t size)
{
	ocean_assert(outputPin_.isValid());

	outputPin_->unlockBufferToFill(size);
}

bool DSSampleSourceFilter::setFormat(const FrameType& type, const double frequency)
{
	return setFormat(std::string(), type, frequency);
}

bool DSSampleSourceFilter::setFormat(const std::string& compression, const FrameType& type, const double frequency)
{
	if (type.pixelFormat() != FrameType::FORMAT_BGR24 && type.pixelFormat() != FrameType::FORMAT_BGR32)
	{
		return false;
	}

	if ((type.width() % 4) != 0 || (type.height() % 4) != 0)
	{
		return false;
	}

	if (compression.empty() == false && compression.length() < 4)
	{
		return false;
	}

	try
	{
		CMediaType mediaType;

		mediaType.SetType(&MEDIATYPE_Video);
		mediaType.SetFormatType(&FORMAT_VideoInfo);

		VIDEOINFOHEADER& videoInfo = *(VIDEOINFOHEADER*)mediaType.AllocFormatBuffer(sizeof(VIDEOINFOHEADER));

		// Initialize the VideoInfo structure before configuring its members
		ZeroMemory(&videoInfo, sizeof(VIDEOINFOHEADER));

		ocean_assert(type.pixelFormat() == FrameType::FORMAT_BGR24 || type.pixelFormat() == FrameType::FORMAT_BGR32);

		videoInfo.bmiHeader.biCompression = BI_RGB;
		videoInfo.bmiHeader.biBitCount = WORD(type.pixelFormat() == FrameType::FORMAT_BGR24 ? 24 : 32);
		videoInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		videoInfo.bmiHeader.biWidth = LONG(type.width());
		videoInfo.bmiHeader.biHeight = LONG(type.pixelOrigin() == FrameType::ORIGIN_LOWER_LEFT ? type.height() : -LONG(type.height()));
		videoInfo.bmiHeader.biPlanes = 1;
		videoInfo.bmiHeader.biSizeImage = GetBitmapSize(&videoInfo.bmiHeader);
		videoInfo.bmiHeader.biClrImportant = 0;
		if (frequency == 0)
		{
			videoInfo.AvgTimePerFrame = 0;
		}
		else
		{
			videoInfo.AvgTimePerFrame = REFERENCE_TIME(10000000.0f / frequency);
		}

		mediaType.SetSampleSize(videoInfo.bmiHeader.biSizeImage);

		if (compression.empty())
		{
			mediaType.SetTemporalCompression(false);

			const GUID subType = DSFrameMedium::convertPixelFormat(type.pixelFormat());

			mediaType.SetSubtype(&subType);
		}
		else
		{
			ocean_assert(compression.length() >= 4);

			mediaType.SetTemporalCompression(true);
			mediaType.SetVariableSize();

			// Blank video format with default fourChar value
			GUID format = {0x00000000, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9B, 0x71}};
			format.Data1 = (compression[0] << 0) | (compression[1] << 8) | (compression[2] << 16) | (compression[3] << 24);

			videoInfo.bmiHeader.biCompression = format.Data1;

			mediaType.SetSubtype(&format);
		}

		ocean_assert(outputPin_.isValid());
		return outputPin_->specifyMediaType(mediaType, frequency);
	}
	catch (const OceanException& exception)
	{
		Log::error() << exception.what();
	}

	return false;
}

}

}

}
