/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/mediafoundation/MFMedium.h"
#include "ocean/media/mediafoundation/Utilities.h"

#include "ocean/base/String.h"
#include "ocean/base/Thread.h"

namespace Ocean
{

namespace Media
{

namespace MediaFoundation
{

MFMedium::EventCallback::EventCallback(IMFMediaEventGenerator& eventGenerator, const TopologyEventFunction& topologySet, const EventFunction& sessionStarted, const EventFunction& sessionStopped, const EventFunction& sessionEnded, const TopologyNodeEventFunction& formatTypeChanged) :
	eventGenerator_(eventGenerator),
	callbackTopologySet_(topologySet),
	callbackSessionStarted_(sessionStarted),
	callbackSessionStopped_(sessionStopped),
	callbackSessionEnded_(sessionEnded),
	callbackFormatTypeChanged_(formatTypeChanged)
{
	// nothing to do here
}

void MFMedium::EventCallback::deactivate()
{
	active_ = false;
}

bool MFMedium::EventCallback::isMediaSessionClosed() const
{
	return isMediaSessionClosed_.load();
}

HRESULT STDMETHODCALLTYPE MFMedium::EventCallback::QueryInterface(REFIID /*riid*/, void __RPC_FAR* __RPC_FAR* /*ppvObject*/)
{
	return S_FALSE;
}

ULONG STDMETHODCALLTYPE MFMedium::EventCallback::AddRef()
{
	return InterlockedIncrement(&referenceCounter_);
}

ULONG STDMETHODCALLTYPE MFMedium::EventCallback::Release()
{
	const ULONG counter = InterlockedDecrement(&referenceCounter_);

	if (counter == 0)
	{
		delete this;
	}

	return counter;
}

HRESULT STDMETHODCALLTYPE MFMedium::EventCallback::GetParameters(DWORD* /*pdwFlags*/, DWORD* /*pdwQueue*/)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MFMedium::EventCallback::Invoke(IMFAsyncResult* pAsyncResult)
{
	if (!active_)
	{
		return S_FALSE;
	}

	ScopedIMFMediaEvent mediaEvent;

	if (S_OK == eventGenerator_.EndGetEvent(pAsyncResult, &mediaEvent.resetObject()))
	{
		MediaEventType eventType = MEUnknown;
		HRESULT eventStatus = S_OK;

		mediaEvent->GetType(&eventType);
		mediaEvent->GetStatus(&eventStatus);

		switch (eventType)
		{
			case MESessionTopologySet:
			{
				ScopedMediaFoundationObject<IMFTopology> topology = Utilities::eventObject<IMFTopology>(*mediaEvent);
				ocean_assert(topology.isValid());

				if (*topology && callbackTopologySet_)
				{
					callbackTopologySet_(*topology);
				}

				break;
			}

			case MESessionStarted:
			{
				if (callbackSessionStarted_)
				{
					callbackSessionStarted_();
				}

				break;
			}

			case MESessionStopped:
			{
				if (callbackSessionStopped_)
				{
					callbackSessionStopped_();
				}

				break;
			}

			case MESessionEnded:
			{
				if (callbackSessionEnded_)
				{
					callbackSessionEnded_();
				}

				break;
			}

			case MESessionClosed:
			{
				isMediaSessionClosed_ = true;
				break;
			}

			case MESessionStreamSinkFormatChanged:
			{
				TOPOID nodeId = 0ull;
				if (S_OK == mediaEvent->GetUINT64(MF_EVENT_OUTPUT_NODE, &nodeId) && callbackFormatTypeChanged_)
				{
					callbackFormatTypeChanged_(nodeId);
				}

				break;
			}
		}
	}

	eventGenerator_.BeginGetEvent(this, nullptr);

	return S_OK;
}

MFMedium::MFMedium(const std::string& url) :
	Medium(url)
{
	libraryName_ = nameMediaFoundationLibrary();
}

MFMedium::~MFMedium()
{
	stopMediaSession();

	if (eventCallback_.isValid())
	{
		eventCallback_->deactivate();
	}

	releaseTopology();
	releaseMediaSource();
	releaseMediaSession();
}

bool MFMedium::isStarted() const
{
	const ScopedLock lock(lock_);

	return startTimestamp_.isValid();
}

Timestamp MFMedium::startTimestamp() const
{
	const ScopedLock lock(lock_);

	return startTimestamp_;
}

Timestamp MFMedium::pauseTimestamp() const
{
	const ScopedLock lock(lock_);

	return pauseTimestamp_;
}

Timestamp MFMedium::stopTimestamp() const
{
	const ScopedLock lock(lock_);

	return stopTimestamp_;
}

bool MFMedium::start()
{
	const ScopedLock lock(lock_);

	if (!createMediaSession())
	{
		return false;
	}

	if (!createMediaSource())
	{
		return false;
	}

	if (!createTopology(respectPlaybackTime_))
	{
		return false;
	}

	if (!startMediaSession())
	{
		return false;
	}

	return true;
}

bool MFMedium::pause()
{
	const ScopedLock lock(lock_);

	return pauseMediaSession();
}

bool MFMedium::stop()
{
	const ScopedLock lock(lock_);

	return stopMediaSession();
}

bool MFMedium::createMediaSession()
{
	if (mediaSession_.isValid())
	{
		return true;
	}

	if (S_OK != MFCreateMediaSession(nullptr, &mediaSession_.resetObject()))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	ocean_assert(mediaSession_.isValid());
	ocean_assert(!eventCallback_.isValid());

	eventCallback_ = ScopedEventCallback(new EventCallback(*mediaSession_.object(), EventCallback::TopologyEventFunction(*this, &MFMedium::onTopologySet), EventCallback::EventFunction::create(*this, &MFMedium::onSessionStarted), EventCallback::EventFunction::create(*this, &MFMedium::onSessionStopped), EventCallback::EventFunction::create(*this, &MFMedium::onSessionEnded), EventCallback::TopologyNodeEventFunction::create(*this, &MFMedium::onFormatTypeChanged)));

	mediaSession_->BeginGetEvent(*eventCallback_, nullptr);

	return true;
}

void MFMedium::releaseMediaSession()
{
	if (mediaSession_.isValid())
	{
		if (S_OK == mediaSession_->Close())
		{
			const Timestamp startTimestamp(true);

			while (!eventCallback_->isMediaSessionClosed())
			{
				if (startTimestamp + 5.0 > Timestamp(true))
				{
					Thread::sleep(1u);
				}
				else
				{
					Log::warning() << "MediaFoundatoin: Failed to wait for closed media session";
					break;
				}
			}
		}

		mediaSession_.release();
	}

	if (eventCallback_.isValid())
	{
		eventCallback_->deactivate();
	}

	eventCallback_.release();
}

void MFMedium::releaseTopology()
{
	if (mediaSession_.isValid())
	{
		mediaSession_->ClearTopologies();
	}

	topology_.release();
}

bool MFMedium::createMediaSource()
{
	if (mediaSource_.isValid())
	{
		return true;
	}

	mediaSource_ = Media::MediaFoundation::Utilities::createMediaSourceByUrl(String::toWString(url_));

	return mediaSource_.isValid();
}

void MFMedium::releaseMediaSource()
{
	mediaSource_.release();
}

bool MFMedium::startMediaSession()
{
	ocean_assert(mediaSession_.isValid());

	if (!mediaSession_.isValid())
	{
		return false;
	}

	PROPVARIANT startPosition;
	PropVariantInit(&startPosition);
	if (S_OK != mediaSession_->Start(&GUID_NULL, &startPosition))
	{
		return false;
	}

	startTimestamp_.toNow();
	pauseTimestamp_.toInvalid();
	stopTimestamp_.toInvalid();

	return true;
}

bool MFMedium::pauseMediaSession()
{
	ocean_assert(mediaSession_.isValid());

	if (!mediaSession_.isValid())
	{
		return false;
	}

	if (pauseTimestamp_.isValid())
	{
		return true;
	}

	if (S_OK != mediaSession_->Pause())
	{
		return false;
	}

	startTimestamp_.toInvalid();
	pauseTimestamp_.toNow();
	stopTimestamp_.toInvalid();

	return true;
}

bool MFMedium::stopMediaSession()
{
	if (!mediaSession_.isValid())
	{
		return false;
	}

	if (stopTimestamp_.isValid())
	{
		return true;
	}

	if (S_OK != mediaSession_->Stop())
	{
		return false;
	}

	startTimestamp_.toInvalid();
	pauseTimestamp_.toInvalid();
	stopTimestamp_.toNow();

	return true;
}

bool MFMedium::recentMediaSampleTimestamp(LONGLONG& /*timestamp*/, LONGLONG* /*nextTimestamp*/) const
{
	return false;
}

void MFMedium::onTopologySet(IMFTopology* /*topology*/)
{
	// nothing to do here
}

void MFMedium::onSessionStarted()
{
	// nothing to do here
}

void MFMedium::onSessionStopped()
{
	// nothing to do here
}

void MFMedium::onSessionEnded()
{
	// nothing to do here
}

void MFMedium::onFormatTypeChanged(const TOPOID /*nodeId*/)
{
	// nothing to do here
}

bool MFMedium::createPipeline(const bool respectPlaybackTime)
{
	return createMediaSession() && createMediaSource() && createTopology(respectPlaybackTime);
}

void MFMedium::releasePipeline()
{
	releaseTopology();
	releaseMediaSource();
	releaseMediaSession();
}

}

}

}
