// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/directshow/DSLiveAudio.h"
#include "ocean/media/directshow/DSDeviceEnumerator.h"

#include "ocean/base/ScopedFunction.h"

DISABLE_WARNINGS_BEGIN
	#include <Streams.h>
DISABLE_WARNINGS_END

namespace Ocean
{

namespace Media
{

namespace DirectShow
{

DSLiveAudio::DSLiveAudio(const std::string& url) :
	Medium(url),
	DSMedium(url),
	ConfigMedium(url),
	DSConfigMedium(url),
	LiveMedium(url),
	DSLiveMedium(url),
	SoundMedium(url),
	DSSoundMedium(url),
	LiveAudio(url)
{
	isValid_ = buildGraph();
}

DSLiveAudio::~DSLiveAudio()
{
	releaseGraph();

	ocean_assert(audioSourceFilter_.object() == nullptr);
	ocean_assert(audioSourceFilterStreamConfigInterface_.object() == nullptr);
}

bool DSLiveAudio::buildGraph()
{
	if (filterGraph_.object() != nullptr)
	{
		return true;
	}

	if (!createGraphBuilder())
	{
		return false;
	}

	ScopedFunctionVoid scopedReleaseGraphFunction(std::bind(&DSLiveAudio::releaseGraph, this)); // scoped function which will be invoked in case we don't reach the both of this function

	if (!createAudioSourceFilter())
	{
		Log::error() << "Could not create the video source filter.";

		return false;
	}

	ocean_assert(audioSourceFilter_.object() != nullptr);

	const ScopedIPin audioSourceFilterOutputPin = firstPin(audioSourceFilter_.object(), PTYPE_OUTPUT, CTYPE_NOT_CONNECTED);

	if (audioSourceFilterOutputPin.object() == nullptr)
	{
		return false;
	}

	ocean_assert(audioSourceFilterStreamConfigInterface_.object() != nullptr);

	DSSortableSoundTypes sortableSoundTypes;
	collectPreferredAudioFormats(sortableSoundTypes);

	bool connectionEstablished = false;
	for (const DSSortableSoundType& soundtype : sortableSoundTypes)
	{
		if (S_OK == audioSourceFilterStreamConfigInterface_.object()->SetFormat(const_cast<AM_MEDIA_TYPE*>(&soundtype.type())))
		{
			if (S_OK == filterGraph_.object()->Render(audioSourceFilterOutputPin.object()))
			{
				if (createSoundInterface())
				{
					connectionEstablished = true;
					break;
				}
			}
		}
	}

	if (!connectionEstablished)
	{
		return false;
	}

	scopedReleaseGraphFunction.revoke(); // we don't call releaseGraph()

	return true;
}

void DSLiveAudio::releaseGraph()
{
	stopGraph();

	releaseAudioSourceFilter();
	releaseSoundInterface();
	releaseGraphBuilder();
}

bool DSLiveAudio::createAudioSourceFilter()
{
	if (audioSourceFilter_.object() != nullptr)
	{
		return true;
	}

	DSDeviceEnumerator& enumerator = DSEnumerators::get().enumerator(CLSID_AudioInputDeviceCategory);

	if (!enumerator.exist(url()))
	{
		Log::error() << "Could not create a video source filter for \"" << url() << "\".";

		return false;
	}

	const ScopedIMoniker audioMoniker = enumerator.moniker(url());
	ocean_assert(audioMoniker.object() != nullptr);

	ocean_assert(audioSourceFilter_.object() == nullptr);
	if (S_OK != audioMoniker.object()->BindToObject(0, 0, IID_IBaseFilter, (void**)(&audioSourceFilter_.resetObject())))
	{
		Log::error() << "Could not create a live source audio filter.";

		return false;
	}

	if (S_OK != filterGraph_.object()->AddFilter(audioSourceFilter_.object(), L"Source Filter"))
	{
		Log::error() << "Could not add the live audio source filter to the filter graph.";

		return false;
	}

	const ScopedIPin audioSourceFilterOutputPin = firstPin(audioSourceFilter_.object(), PTYPE_OUTPUT, CTYPE_NOT_CONNECTED);
	if (audioSourceFilterOutputPin.object() == nullptr)
	{
		return false;
	}

	if (S_OK != audioSourceFilterOutputPin.object()->QueryInterface(IID_IAMStreamConfig, (void**)(&audioSourceFilterStreamConfigInterface_.resetObject())))
	{
		return false;
	}

	return true;
}

bool DSLiveAudio::collectPreferredAudioFormats(DSSortableSoundTypes& sortableSoundTypes)
{
	if (audioSourceFilterStreamConfigInterface_.object() == nullptr)
	{
		return false;
	}

	int count = 0;
	int size = 0;

	if (audioSourceFilterStreamConfigInterface_.object()->GetNumberOfCapabilities(&count, &size) != S_OK)
	{
		return false;
	}

	ocean_assert(sizeof(AUDIO_STREAM_CONFIG_CAPS) >= size_t(size));
	AUDIO_STREAM_CONFIG_CAPS caps;

	ocean_assert(sortableSoundTypes.empty());

	for (int n = 0; n < count; n++)
	{
		AM_MEDIA_TYPE* type = nullptr;
		if (S_OK == audioSourceFilterStreamConfigInterface_.object()->GetStreamCaps(n, &type, (BYTE*)(&caps)))
		{
			sortableSoundTypes.emplace_back(DSMediaType(*type), mediumPreferredSoundType);

			FreeMediaType(*type);
			CoTaskMemFree(type);
		}
	}

	std::sort(sortableSoundTypes.begin(), sortableSoundTypes.end());

	return sortableSoundTypes.empty() == false;
}

void DSLiveAudio::releaseAudioSourceFilter()
{
	if (audioSourceFilter_.object() != nullptr)
	{
		ocean_assert(filterGraph_.object() != nullptr);

		filterGraph_.object()->RemoveFilter(audioSourceFilter_.object());
	}

	audioSourceFilterStreamConfigInterface_.release();
	audioSourceFilter_.release();
}

bool DSLiveAudio::configuration(const std::string& name, long long data)
{
	if (audioSourceFilter_.object() == nullptr || filterGraph_.object() == nullptr)
	{
		return false;
	}

	if (name == "Display")
	{
		return showPropertyDialog(audioSourceFilter_.object(), HWND(data), false);
	}

	if (name != "Format")
	{
		return false;
	}

	bool restart = graphIsRunning_;
	stopGraph();

	const PinPairs pinPairs = connections(audioSourceFilter_.object());

	for (const PinPair& pinPair : pinPairs)
	{
		pinPair.first.object()->Disconnect();
		pinPair.second.object()->Disconnect();
	}

	const bool noError = showPropertyDialog(audioSourceFilterStreamConfigInterface_.object(), HWND(data), false);
	ocean_assert_and_suppress_unused(noError, noError);

	ocean_assert(audioSourceFilter_.object() != nullptr);
	const ScopedIPin audioSourceFilterOutputPin = firstPin(audioSourceFilter_.object(), PTYPE_OUTPUT, CTYPE_NOT_CONNECTED);

	if (audioSourceFilterOutputPin.object() == nullptr)
	{
		return false;
	}

	if (S_OK != filterGraph_.object()->Render(audioSourceFilterOutputPin.object()))
	{
		Log::error() << "Could not reconnect audio filters.";

		return false;
	}

	// remove unused filter
	Filters currentFilters(filters());
	for (const ScopedIBaseFilter& filter : currentFilters)
	{
		const ScopedIPin pin = firstPin(filter.object(), PTYPE_DONT_CARE, CTYPE_CONNECTED);

		if (pin.object() == nullptr)
		{
			filterGraph_.object()->RemoveFilter(filter.object());
		}
	}

	/*if (noError)
	{
		AM_MEDIA_TYPE mediaType;
		if (S_OK != soundSampleSinkInterface->GetConnectedMediaType(&mediaType))
			noError = false;
		else
		{
			extractSoundFormat(mediaType, mediumSoundType);
			mediumSoundTypeTimestamp.toNow();

			freeMediaType(mediaType);
		}
	}*/

	if (restart && !start())
	{
		Log::error() << "Could not restart \"" << url() << "\".";

		return false;
	}

	return true;
}

DSLiveAudio::ConfigNames DSLiveAudio::configs() const
{
	ConfigNames names;

	if (showPropertyDialog(audioSourceFilter_.object(), nullptr, true))
	{
		names.emplace_back("Properties");
	}

	if (showPropertyDialog(audioSourceFilterStreamConfigInterface_.object(), nullptr, true))
	{
		names.emplace_back("Format");
	}

	return names;
}

}

}

}
