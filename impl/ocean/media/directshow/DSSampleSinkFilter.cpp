/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/directshow/DSSampleSinkFilter.h"
#include "ocean/media/directshow/DSObject.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace Media
{

namespace DirectShow
{

DSSampleSinkFilter::DSInputPin::DSInputPin(const std::wstring& pinName, HRESULT *phr, DSSampleSinkFilter *filter, const SampleCallback& callback) :
	CBaseInputPin(L"Input Pin", filter, &filter->interfaceLock_, phr, pinName.c_str()),
	sampleCallback_(callback)
{
	ocean_assert(sampleCallback_);
}

DSSampleSinkFilter::DSInputPin::~DSInputPin()
{
	// nothing to do here
}

bool DSSampleSinkFilter::DSInputPin::respectPlaybackTime() const
{
	return respectPlaybackTime_;
}

bool DSSampleSinkFilter::DSInputPin::setRespectPlaybackTime(const bool state)
{
	respectPlaybackTime_ = state;
	return true;
}

bool DSSampleSinkFilter::DSInputPin::specifyMediaType(const AM_MEDIA_TYPE& mediaType)
{
	mediaTypes_.push_back(mediaType);
	return true;
}

HRESULT DSSampleSinkFilter::DSInputPin::CheckMediaType(const CMediaType* mediaType)
{
	ocean_assert(mediaType);

	for (MediaTypes::const_iterator iMedia = mediaTypes_.begin(); iMedia != mediaTypes_.end(); ++iMedia)
	{
		if (iMedia->majortype != GUID_NULL)
		{
			if (iMedia->majortype != mediaType->majortype)
			{
				continue;
			}
		}

		if (iMedia->subtype != GUID_NULL)
		{
			if (iMedia->subtype != mediaType->subtype)
			{
				continue;
			}
		}

		if (iMedia->formattype != GUID_NULL)
		{
			if (iMedia->formattype != mediaType->formattype)
			{
				continue;
			}
		}

		if (mediaType->formattype == GUID_NULL)
		{
			continue;
		}

		if (iMedia->lSampleSize > 1)
		{
			if (iMedia->lSampleSize != mediaType->lSampleSize)
			{
				continue;
			}
		}

		if (iMedia->cbFormat != 0)
		{
			if (*mediaType != *iMedia)
			{
				continue;
			}
		}

		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT DSSampleSinkFilter::DSInputPin::GetMediaType(int position, CMediaType* mediaType)
{
	ocean_assert(mediaType);

	if (position < 0)
	{
		return E_INVALIDARG;
	}

	if (position >= int(mediaTypes_.size()))
	{
		return VFW_S_NO_MORE_ITEMS;
	}

	*mediaType = mediaTypes_[position];
	return S_OK;
}

STDMETHODIMP DSSampleSinkFilter::DSInputPin::Receive(IMediaSample* mediaSample)
{
	ocean_assert(mediaSample);

	REFERENCE_TIME start = 0, stop = 0;
	if (S_OK == mediaSample->GetTime(&start, &stop))
	{
		if (respectPlaybackTime_)
		{
			CRefTime time;

			if (S_OK == m_pFilter->StreamTime(time) && time < start)
			{
				REFERENCE_TIME waitTime = start - time;
				Sleep(DWORD(waitTime / 10000));
			}
		}
	}

#ifdef OCEAN_DEBUG
	const HRESULT isSyncPoint = mediaSample->IsSyncPoint();
	OCEAN_SUPPRESS_UNUSED_WARNING(isSyncPoint);
#endif

	const double relativeTime = double(start + m_tStart) * 0.0000001;
	ocean_assert(NumericD::round64(relativeTime * 10000000.0) == start + m_tStart);

	if (sampleCallback_)
	{
		sampleCallback_(mediaSample, Timestamp(true), Timestamp(relativeTime));
	}

	return S_OK;
}

STDMETHODIMP DSSampleSinkFilter::DSInputPin::EndOfStream()
{
	const HRESULT result = CBaseInputPin::EndOfStream();

	if (sampleCallback_)
	{
		sampleCallback_(nullptr, Timestamp(true), Timestamp(0.0));
	}

	return result;
}

DSSampleSinkFilter::DSSampleSinkFilter(const std::wstring& filterName, const SampleCallback& callback) :
	CBaseFilter(filterName.c_str(), nullptr, &interfaceLock_, __uuidof(CLSID_DSOceanSampleSinkFilter))
{
	ocean_assert(callback);

	HRESULT result;
	inputPin_ = ScopedDSInputPin(new DSInputPin(L"Input pin", &result, this, callback));
	inputPin_->AddRef();
}

DSSampleSinkFilter::~DSSampleSinkFilter()
{
	// The input pin must not be released
}

bool DSSampleSinkFilter::specifyMediaType(const AM_MEDIA_TYPE& mediaType)
{
	ocean_assert(inputPin_.isValid());
	return inputPin_->specifyMediaType(mediaType);
}

bool DSSampleSinkFilter::establishedMediaType(DSMediaType& mediaType)
{
	ocean_assert(inputPin_.isValid());

	mediaType = DSMediaType(inputPin_->m_mt);

	return inputPin_->m_Connected != nullptr;
}

bool DSSampleSinkFilter::respectPlaybackTime() const
{
	ocean_assert(inputPin_.isValid());
	return inputPin_->respectPlaybackTime();
}

bool DSSampleSinkFilter::setRespectPlaybackTime(const bool state)
{
	ocean_assert(inputPin_.isValid());
	return inputPin_->setRespectPlaybackTime(state);
}

int DSSampleSinkFilter::GetPinCount()
{
	return 1;
}

CBasePin* DSSampleSinkFilter::GetPin(int n)
{
	if (n == 0)
	{
		ocean_assert(inputPin_.isValid());
		return *inputPin_;
	}

	return nullptr;
}

}

}

}
