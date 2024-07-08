/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/mediafoundation/SampleGrabber.h"

#include <Shlwapi.h>

namespace Ocean
{

namespace Media
{

namespace MediaFoundation
{

SampleGrabber::SampleGrabber(const SampleCallback& callback) :
	sampleCallback_(callback)
{
	ocean_assert(sampleCallback_);
}

void SampleGrabber::deactivate()
{
	active_ = false;
}

LONGLONG SampleGrabber::recentSampleTime() const
{
	return recentSampleTime_;
}

STDMETHODIMP SampleGrabber::QueryInterface(REFIID riid, void** ppv)
{
	static const QITAB qit[] =
	{
		{
			&__uuidof(IMFSampleGrabberSinkCallback), OFFSETOFCLASS(IMFSampleGrabberSinkCallback, SampleGrabber),
		},
		{
			&__uuidof(IMFClockStateSink), OFFSETOFCLASS(IMFClockStateSink, SampleGrabber)
		},
		{nullptr, 0}
	};

	return QISearch(this, qit, riid, ppv);
}

STDMETHODIMP_(ULONG) SampleGrabber::AddRef()
{
	return InterlockedIncrement(&referenceCounter_);
}

STDMETHODIMP_(ULONG) SampleGrabber::Release()
{
	const ULONG counter = InterlockedDecrement(&referenceCounter_);

	if (counter == 0)
	{
		delete this;
	}

	return counter;
}

STDMETHODIMP SampleGrabber::OnClockStart(MFTIME /*hnsSystemTime*/, LONGLONG /*llClockStartOffset*/)
{
	return S_OK;
}

STDMETHODIMP SampleGrabber::OnClockStop(MFTIME /*hnsSystemTime*/)
{
	return S_OK;
}

STDMETHODIMP SampleGrabber::OnClockPause(MFTIME /*hnsSystemTime*/)
{
	return S_OK;
}

STDMETHODIMP SampleGrabber::OnClockRestart(MFTIME /*hnsSystemTime*/)
{
	return S_OK;
}

STDMETHODIMP SampleGrabber::OnClockSetRate(MFTIME /*hnsSystemTime*/, float /*flRate*/)
{
	return S_OK;
}

STDMETHODIMP SampleGrabber::OnSetPresentationClock(IMFPresentationClock* /*pClock*/)
{
	return S_OK;
}

STDMETHODIMP SampleGrabber::OnProcessSample(REFGUID guidMajorMediaType, DWORD dwSampleFlags, LONGLONG llSampleTime, LONGLONG llSampleDuration, const BYTE * pSampleBuffer, DWORD dwSampleSize)
{
	if (active_)
	{
		recentSampleTime_ = llSampleTime;

		sampleCallback_(guidMajorMediaType, dwSampleFlags, llSampleTime, llSampleDuration, pSampleBuffer, dwSampleSize);
		return S_OK;
	}

	return S_FALSE;
}

STDMETHODIMP SampleGrabber::OnShutdown()
{
	active_ = false;
	return S_OK;
}

ScopedSampleGrabber SampleGrabber::createInstance(const SampleCallback& callback)
{
	return ScopedSampleGrabber(new (std::nothrow) SampleGrabber(callback));
}

}

}

}
