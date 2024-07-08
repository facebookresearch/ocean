/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/directshow/DSLiveVideo.h"
#include "ocean/media/directshow/DSDeviceEnumerator.h"
#include "ocean/media/directshow/DSSampleSinkFilter.h"

#include "ocean/base/ScopedFunction.h"
#include "ocean/base/String.h"

#include "ocean/io/CameraCalibrationManager.h"

#include "ocean/math/PinholeCamera.h"

namespace Ocean
{

namespace Media
{

namespace DirectShow
{

DSLiveVideo::DSLiveVideo(const std::string& url) :
	Medium(url),
	DSMedium(url),
	FrameMedium(url),
	DSFrameMedium(url),
	ConfigMedium(url),
	DSConfigMedium(url),
	LiveMedium(url),
	DSLiveMedium(url),
	LiveVideo(url)
{
	preferredFrameType_ = MediaFrameType(1280, 720u, FrameType::FORMAT_UNDEFINED, FrameType::ORIGIN_LOWER_LEFT, 30);
	isValid_ = buildGraph();
}

DSLiveVideo::~DSLiveVideo()
{
	releaseGraph();

	ocean_assert(!videoSourceFilter_.isValid());
	ocean_assert(!videoSourceFilterStreamConfigInterface_.isValid());
}

bool DSLiveVideo::buildGraph()
{
	if (filterGraph_.isValid())
	{
		return true;
	}

	bool noError = createGraphBuilder();

	if (noError && false == createVideoSourceFilter())
	{
		return false;
	}

	ScopedFunctionVoid scopedReleaseGraphFunction(std::bind(&DSLiveVideo::releaseGraph, this)); // scoped function which will be invoked in case we don't reach the both of this function

	const ScopedIPin videoSourceFilterOutputPin = firstPin(*videoSourceFilter_, PTYPE_OUTPUT, CTYPE_NOT_CONNECTED);
	if (!videoSourceFilterOutputPin.isValid())
	{
		return false;
	}

	DSSortableFrameTypes sortableFrameTypes;

	if (collectPreferredVideoFormats(sortableFrameTypes))
	{
		bool connectionEstablished = false;

		// Due to incorrect DirectShow camera driver sometime not all possible media types can be enumerated
		// Therefore, the preferred frame type is tested explicitly
		if (preferredFrameType_.width() > 0u && preferredFrameType_.height() > 0u && sortableFrameTypes.empty() == false)
		{
			DSMediaType forcedMediaType(sortableFrameTypes.front().type());
			modifyFrameFormat(forcedMediaType.type(), preferredFrameType_);

			if (S_OK == videoSourceFilterStreamConfigInterface_->SetFormat(&forcedMediaType.type()))
			{
				if (insertFrameSampleSinkFilter(*videoSourceFilterOutputPin, preferredFrameType_.pixelFormat()))
				{
					connectionEstablished = true;
				}
			}
		}

		if (connectionEstablished == false)
		{
			for (const DSSortableFrameType& frameType : sortableFrameTypes)
			{
				if (S_OK == videoSourceFilterStreamConfigInterface_->SetFormat(const_cast<AM_MEDIA_TYPE*>(&frameType.type())))
				{
					if (insertFrameSampleSinkFilter(*videoSourceFilterOutputPin, preferredFrameType_.pixelFormat()))
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
	}

	scopedReleaseGraphFunction.revoke(); // we don't want the release function to be called

	return true;
}

void DSLiveVideo::releaseGraph()
{
	stopGraph();

	releaseVideoSourceFilter();
	releaseFrameSampleSinkFilter();
	releaseGraphBuilder();
}

bool DSLiveVideo::createVideoSourceFilter()
{
	if (videoSourceFilter_.isValid())
	{
		return true;
	}

	DSDeviceEnumerator& enumerator = DSEnumerators::get().enumerator(CLSID_VideoInputDeviceCategory);

	if (!enumerator.exist(url()))
	{
		Log::warning() << "Could not open video source \"" << url() << "\".";

		return false;
	}

	const ScopedIMoniker videoMoniker = enumerator.moniker(url());
	ocean_assert(videoMoniker.isValid());

	ocean_assert(!videoSourceFilter_.isValid());
	if (S_OK != videoMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)(&videoSourceFilter_.resetObject())))
	{
		Log::error() << "Could not create a video source filter for \"" << url() << "\".";

		return false;
	}

	if (S_OK != filterGraph_->AddFilter(*videoSourceFilter_, L"Source Filter"))
	{
		Log::error() << "Could not add the live video source filter to the filter graph.";

		return false;
	}

	const ScopedIPin videoSourceFilterOutputPin = firstPin(*videoSourceFilter_, PTYPE_OUTPUT, CTYPE_NOT_CONNECTED);
	if (!videoSourceFilterOutputPin.isValid())
	{
		return false;
	}

	if (S_OK != videoSourceFilterOutputPin->QueryInterface(IID_IAMStreamConfig, (void**)(&videoSourceFilterStreamConfigInterface_.resetObject())))
	{
		return false;
	}

	return true;
}

bool DSLiveVideo::collectPreferredVideoFormats(DSSortableFrameTypes& sortableFrameTypes)
{
	if (!videoSourceFilterStreamConfigInterface_.isValid())
	{
		return false;
	}

	int count = 0;
	int size = 0;

	if (videoSourceFilterStreamConfigInterface_->GetNumberOfCapabilities(&count, &size) != S_OK)
	{
		return false;
	}

	ocean_assert(sizeof(VIDEO_STREAM_CONFIG_CAPS) >= size_t(size));
	VIDEO_STREAM_CONFIG_CAPS caps;

	ocean_assert(sortableFrameTypes.empty());

	for (int n = 0; n < count; n++)
	{
		AM_MEDIA_TYPE* type = nullptr;
		if (S_OK == videoSourceFilterStreamConfigInterface_->GetStreamCaps(n, &type, (BYTE*)&caps))
		{
			if (type->majortype == MEDIATYPE_Video)
			{
				sortableFrameTypes.emplace_back(DSMediaType(*type), preferredFrameType_);
			}
		}

		FreeMediaType(*type);
		CoTaskMemFree(type);
	}

	std::sort(sortableFrameTypes.begin(), sortableFrameTypes.end());

	return sortableFrameTypes.empty() == false;
}

void DSLiveVideo::releaseVideoSourceFilter()
{
	if (videoSourceFilter_.isValid())
	{
		ocean_assert(filterGraph_.isValid());

		filterGraph_->RemoveFilter(*videoSourceFilter_);
	}

	videoSourceFilterStreamConfigInterface_.release();
	videoSourceFilter_.release();
}

bool DSLiveVideo::configuration(const std::string& name, long long data)
{
	if (!videoSourceFilter_.isValid() || !filterGraph_.isValid())
	{
		return false;
	}

	if (name == "Properties")
	{
		return showPropertyDialog(*videoSourceFilter_, HWND(data), false);
	}

	if (name != "Format")
	{
		return false;
	}

	bool restart = graphIsRunning_;
	stopGraph();

	const PinPairs pinPairs = connections(*videoSourceFilter_);

	for (const PinPair& pinPair : pinPairs)
	{
		pinPair.first->Disconnect();
		pinPair.second->Disconnect();
	}

	releaseFrameSampleSinkFilter();

	if (!showPropertyDialog(*videoSourceFilterStreamConfigInterface_, HWND(data), false))
	{
		return false;
	}

	ocean_assert(videoSourceFilter_.isValid());
	const ScopedIPin videoSourceFilterOutputPin = firstPin(*videoSourceFilter_, PTYPE_OUTPUT, CTYPE_NOT_CONNECTED);

	if (!videoSourceFilterOutputPin.isValid())
	{
		return false;
	}

	if (!insertFrameSampleSinkFilter(*videoSourceFilterOutputPin, preferredFrameType_.pixelFormat()))
	{
		Log::error() << "Could not reconnect video filters.";

		return false;
	}

	// remove unused filter
	const Filters currentFilters(filters());
	for (const ScopedIBaseFilter& filter : currentFilters)
	{
		const ScopedIPin pin = firstPin(*filter, PTYPE_DONT_CARE, CTYPE_CONNECTED);

		if (!pin.isValid())
		{
			filterGraph_->RemoveFilter(*filter);
		}
	}

	DSMediaType establishedMediaType;
	if (!sampleSinkFilter_->establishedMediaType(establishedMediaType))
	{
		return false;
	}
	else
	{
		MediaFrameType type;

		if (extractFrameFormat(establishedMediaType.type(), type))
		{
			recentFrameType_ = FrameType(type.width(), type.height(), type.pixelFormat(), type.pixelOrigin());
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
	}

	if (restart && !start())
	{
		Log::error() << "Could not restart \"" << url() << "\".";

		return false;
	}

	return true;
}

DSLiveVideo::ConfigNames DSLiveVideo::configs() const
{
	ConfigNames names;

	if (showPropertyDialog(*videoSourceFilter_, nullptr, true))
	{
		names.emplace_back("Properties");
	}

	if (showPropertyDialog(*videoSourceFilterStreamConfigInterface_, nullptr, true))
	{
		names.emplace_back("Format");
	}

	return names;
}

}

}

}
