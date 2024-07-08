/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/directshow/DSGraphObject.h"

#include "ocean/base/ScopedFunction.h"
#include "ocean/base/Timestamp.h"

namespace Ocean
{

namespace Media
{

namespace DirectShow
{

DSGraphObject::~DSGraphObject()
{
	ocean_assert(!filterGraph_.isValid());
	ocean_assert(!mediaControlInterface_.isValid());
	ocean_assert(!mediaEventInterface_.isValid());
}

bool DSGraphObject::createGraphBuilder()
{
	if (filterGraph_.isValid())
	{
		return true;
	}

	ocean_assert(!mediaControlInterface_.isValid());
	ocean_assert(!mediaEventInterface_.isValid());

	if (S_OK != CoCreateInstance(CLSID_FilterGraph, nullptr, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&(filterGraph_.resetObject())))
	{
		Log::error() << "Could not create a filter graph.";

		return false;
	}

	ScopedFunctionVoid scopedReleaseGraphBuilderFunction(std::bind(&DSGraphObject::releaseGraphBuilder, this)); // scoped function which will be invoked in case we don't reach the both of this function

#ifdef OCEAN_DEBUG
	ocean_assert(addFilterGraphToROT());
#endif

	if (S_OK != filterGraph_->QueryInterface(IID_IMediaControl, (void**)(&mediaControlInterface_.resetObject())))
	{
		Log::error() << "Could not create a media control interface.";

		return false;
	}

	if (S_OK != filterGraph_->QueryInterface(IID_IMediaEvent, (void**)(&mediaEventInterface_.resetObject())))
	{
		Log::error() << "Could not create the media event interface.";

		return false;
	}

	scopedReleaseGraphBuilderFunction.revoke(); // we don't want the release function to be called

	return true;
}

void DSGraphObject::releaseGraphBuilder()
{
	ocean_assert(graphIsRunning_ == false);

	mediaControlInterface_.release();
	mediaEventInterface_.release();

	if (filterGraph_.isValid())
	{

#ifdef OCEAN_DEBUG
		ocean_assert(removeFilterGraphFromROT());
#endif
	}

	filterGraph_.release();
}

bool DSGraphObject::startGraph()
{
	ocean_assert((filterGraph_.isValid() && mediaControlInterface_.isValid()) || (!filterGraph_.isValid() && !mediaControlInterface_.isValid()));

	graphIsRunning_ = false;

	if (mediaControlInterface_.isValid())
	{
		if (S_FALSE == mediaControlInterface_->Run())
		{
			const Timestamp startTimestamp(true);

			while (!graphIsRunning_ && startTimestamp + 1.0 > Timestamp(true))
			{
				FILTER_STATE state = State_Stopped;
				if (E_FAIL != mediaControlInterface_->GetState(1000, (OAFilterState*)(&state)))
				{
					graphIsRunning_ = state == State_Running;
				}
			}
		}
		else
		{
			graphIsRunning_ = true;
		}
	}

	return graphIsRunning_;
}

bool DSGraphObject::pauseGraph()
{
	ocean_assert((filterGraph_.isValid() && mediaControlInterface_.isValid()) || !filterGraph_.isValid() && !mediaControlInterface_.isValid());

	if (mediaControlInterface_.isValid())
	{
		if (S_OK == mediaControlInterface_->Pause())
		{
			return true;
		}

		FILTER_STATE state;
		if (S_OK == mediaControlInterface_->GetState(1000, (OAFilterState*)(&state)))
		{
			return state == State_Paused;
		}
	}

	return false;
}

bool DSGraphObject::stopGraph()
{
	if (mediaControlInterface_.isValid())
	{
		if (S_FALSE == mediaControlInterface_->Stop())
		{
			FILTER_STATE state;
			if (S_OK == mediaControlInterface_->GetState(1000, (OAFilterState*)(&state)))
			{
				graphIsRunning_ = state != State_Stopped;
			}
		}
		else
		{
			graphIsRunning_ = false;
		}
	}

	return graphIsRunning_ == false;
}

ScopedIBaseFilter DSGraphObject::addFilter(const GUID& classIdentifier, const std::wstring& name)
{
	if (!filterGraph_.isValid())
	{
		return ScopedIBaseFilter();
	}

	ScopedIBaseFilter filter;
	if (S_OK != CoCreateInstance(classIdentifier, nullptr, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)(&filter.resetObject())))
	{
		return ScopedIBaseFilter();
	}

	ocean_assert(filter.isValid());

	if (S_OK != filterGraph_->AddFilter(*filter, name.c_str()))
	{
		ScopedIBaseFilter();
	}

	return filter;
}

bool DSGraphObject::connectFilters(IBaseFilter* outputFilter, IBaseFilter* inputFilter, const AM_MEDIA_TYPE* type)
{
	ocean_assert(filterGraph_.isValid());
	ocean_assert(outputFilter != nullptr && inputFilter != nullptr);

	const ScopedIPin outputPin = firstPin(outputFilter, PTYPE_OUTPUT, CTYPE_NOT_CONNECTED);
	if (!outputPin.isValid())
	{
		return false;
	}

	const ScopedIPin inputPin = firstPin(inputFilter, PTYPE_INPUT, CTYPE_NOT_CONNECTED);
	if (!inputPin.isValid())
	{
		return false;
	}

	if (type != nullptr)
	{
		return S_OK == outputPin->Connect(*inputPin, type);
	}

	return S_OK == filterGraph_->Connect(*outputPin, *inputPin);
}

bool DSGraphObject::connectFilter(IPin* outputPin, IBaseFilter* inputFilter, const AM_MEDIA_TYPE* type)
{
	ocean_assert(filterGraph_.isValid());
	ocean_assert(outputPin != nullptr && inputFilter != nullptr);

	const ScopedIPin inputPin = firstPin(inputFilter, PTYPE_INPUT, CTYPE_NOT_CONNECTED);
	if (!inputPin.isValid())
	{
		return false;
	}

	if (type != nullptr)
	{
		return S_OK == outputPin->Connect(*inputPin, type);
	}

	return S_OK == filterGraph_->Connect(outputPin, *inputPin);
}

bool DSGraphObject::connectFilter(IBaseFilter* outputFilter, IPin* inputPin, const AM_MEDIA_TYPE* type)
{
	ocean_assert(filterGraph_.isValid());
	ocean_assert(outputFilter != nullptr && inputPin != nullptr);

	const ScopedIPin outputPin = firstPin(outputFilter, PTYPE_OUTPUT, CTYPE_NOT_CONNECTED);
	if (!outputPin.isValid())
	{
		return false;
	}

	if (type != nullptr)
	{
		return S_OK == outputPin->Connect(inputPin, type);
	}

	return S_OK == filterGraph_->Connect(*outputPin, inputPin);
}

DSGraphObject::Filters DSGraphObject::filters()
{
	ocean_assert(filterGraph_.isValid());

	Filters filters;

	ScopedIEnumFilters enumerator;
	if (S_OK != filterGraph_->EnumFilters(&enumerator.resetObject()))
	{
		return filters;
	}

	ScopedIBaseFilter filter;
	while (S_OK == enumerator->Next(1, &filter.resetObject(), nullptr))
	{
		filters.emplace_back(std::move(filter));
	}

	return filters;
}

#ifdef OCEAN_DEBUG

bool DSGraphObject::addFilterGraphToROT()
{
	if (!filterGraph_.isValid())
	{
		return true;
	}

	ocean_assert(rotId_ == 0);

	ScopedIRunningObjectTable rotInterface;
	if (S_OK != GetRunningObjectTable(0, &rotInterface.resetObject()))
	{
		return false;
	}

	wchar_t name[256];
	StringCchPrintfW(name, 256, L"FilterGraph %08x pid %08x", (DWORD_PTR)(*filterGraph_), GetCurrentProcessId());

	ScopedIMoniker moniker;
	if (S_OK == CreateItemMoniker(L"!", name, &moniker.resetObject()))
	{
		rotInterface->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE, *filterGraph_, *moniker, &rotId_);
	}

	return true;
}

bool DSGraphObject::removeFilterGraphFromROT()
{
	if (rotId_ == 0)
	{
		return true;
	}

	ScopedIRunningObjectTable rotInterface;
	if (S_OK != GetRunningObjectTable(0, &rotInterface.resetObject()))
	{
		return false;
	}

	rotInterface->Revoke(rotId_);

	rotId_ = 0;

	return true;
}

#endif // OCEAN_DEBUG

}

}

}
