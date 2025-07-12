/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/mediafoundation/MFLiveVideo.h"
#include "ocean/media/mediafoundation/MFLibrary.h"
#include "ocean/media/mediafoundation/Utilities.h"

namespace Ocean
{

namespace Media
{

namespace MediaFoundation
{

MFLiveVideo::MFLiveVideo(const std::string& url, const std::string& symbolicLink) :
	Medium(url),
	MFMedium(url),
	FrameMedium(url),
	MFFrameMedium(url),
	ConfigMedium(url),
	LiveMedium(url),
	LiveVideo(url)
{
	if (!symbolicLink.empty())
	{
		symbolicLink_ = symbolicLink;
	}
	else
	{
		Library::Definitions definitions;
		if (enumerateVideoDevices(definitions))
		{
			for (const Library::Definition& definition : definitions)
			{
				if (definition.url() == url)
				{
					symbolicLink_ = definition.uniqueIdentifier();
					break;
				}
				else if (definition.uniqueIdentifier() == url)
				{
					url_ = definition.url();
					symbolicLink_ = definition.uniqueIdentifier();
					break;
				}
			}
		}
	}

	if (!symbolicLink_.empty())
	{
		isValid_ = createPipeline(respectPlaybackTime_);
	}
	else
	{
		ocean_assert(!isValid());
	}
}

MFLiveVideo::~MFLiveVideo()
{
	if (mediaSession_.isValid())
	{
		const bool stopResult = stopMediaSession();
		ocean_assert_and_suppress_unused(stopResult, stopResult);
	}

	releasePipeline();
}

MFLiveVideo::StreamTypes MFLiveVideo::supportedStreamTypes() const
{
	const ScopedLock scopedLock(lock_);

	if (!mediaTypeHandler_.isValid())
	{
		return StreamTypes();
	}

	std::unordered_set<StreamType> streamTypes;

	DWORD mediaTypeCount = 0u;
	if (S_OK == mediaTypeHandler_->GetMediaTypeCount(&mediaTypeCount))
	{
		for (DWORD mediaTypeIndex = 0u; mediaTypeIndex < mediaTypeCount; ++mediaTypeIndex)
		{
			ScopedIMFMediaType mediaType;
			if (S_OK == mediaTypeHandler_->GetMediaTypeByIndex(mediaTypeIndex, &mediaType.resetObject()))
			{
				StreamType streamType = ST_INVALID;
				if (MFFrameMedium::extractStreamType(*mediaType, streamType))
				{
					streamTypes.emplace(streamType);
				}
			}
		}
	}

	return StreamTypes(streamTypes.cbegin(), streamTypes.cend());
}

MFLiveVideo::StreamConfigurations MFLiveVideo::supportedStreamConfigurations(const StreamType streamType) const
{
	const ScopedLock scopedLock(lock_);

	if (!mediaTypeHandler_.isValid())
	{
		return StreamConfigurations();
	}

	DWORD mediaTypeCount = 0u;
	if (S_OK != mediaTypeHandler_->GetMediaTypeCount(&mediaTypeCount))
	{
		ocean_assert(false && "This should never happen!");
		return StreamConfigurations();
	}

	StreamPropertyMap streamPropertyMap;

	for (DWORD mediaTypeIndex = 0u; mediaTypeIndex < mediaTypeCount; ++mediaTypeIndex)
	{
		ScopedIMFMediaType mediaType;
		if (S_OK != mediaTypeHandler_->GetMediaTypeByIndex(mediaTypeIndex, &mediaType.resetObject()))
		{
			ocean_assert(false && "This should never happen!");
			continue;
		}

		StreamType mediaStreamType = ST_INVALID;
		CodecType mediaCodecType = CT_INVALID;
		if (!MFFrameMedium::extractStreamType(*mediaType, mediaStreamType, &mediaCodecType))
		{
			continue;
		}

		ocean_assert(mediaStreamType != ST_INVALID);

		if (streamType != ST_INVALID && streamType != mediaStreamType)
		{
			// this stream type is not of interest
			continue;
		}


		MFFrameMedium::MediaFrameType mediaFrameType;
		if (!MFFrameMedium::extractMediaFrameType(*mediaType, mediaFrameType))
		{
			continue;
		}

		const StreamProperty streamProperty(mediaStreamType, mediaFrameType.width(), mediaFrameType.height(), mediaFrameType.pixelFormat(), mediaCodecType);

		streamPropertyMap[streamProperty].emplace_back(mediaFrameType.frequency());
	}

	StreamConfigurations streamConfigurations;
	streamConfigurations.reserve(streamPropertyMap.size());

	for (StreamPropertyMap::value_type& streamPropertyMapValue : streamPropertyMap)
	{
		const StreamProperty& streamProperty = streamPropertyMapValue.first;
		std::vector<double>& frameRates = streamPropertyMapValue.second;

		streamConfigurations.emplace_back(streamProperty, std::move(frameRates));
	}

	return streamConfigurations;
}

bool MFLiveVideo::enumerateVideoDevices(Library::Definitions& definitions)
{
	definitions.clear();

	ScopedIMFAttributes attributes;

	if (S_OK != MFCreateAttributes(&attributes.resetObject(), 1))
	{
		return false;
	}

	ocean_assert(attributes.isValid());

	if (S_OK != attributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID))
	{
		return false;
	}

	IMFActivate** devices = nullptr;
	UINT32 numberDevices = 0u;

	if (S_OK == MFEnumDeviceSources(*attributes, &devices, &numberDevices))
	{
		for (UINT32 n = 0u; n < numberDevices; ++n)
		{
			std::string friendlyName;
			std::string symbolicLink;

			if (MediaFoundation::Utilities::getAllocatedString(devices[n], MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, friendlyName)
					&& MediaFoundation::Utilities::getAllocatedString(devices[n], MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK, symbolicLink))
			{
				definitions.emplace_back(friendlyName, LIVE_VIDEO, nameMediaFoundationLibrary(), symbolicLink);
			}
			else
			{
				Log::error() << "Failed to determine name or link for video capture device";
			}

			release(devices[n]);
		}
	}

	return true;
}

bool MFLiveVideo::createTopology(const bool respectPlaybackTime)
{
	if (topology_.isValid())
	{
		return true;
	}

	if (S_OK != MFCreateTopology(&topology_.resetObject()))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	if (buildFrameTopology(respectPlaybackTime))
	{
		ocean_assert(mediaSession_.isValid());

		if (S_OK == mediaSession_->SetTopology(0, *topology_))
		{
			return true;
		}
	}

	releaseTopology();
	return false;
}

bool MFLiveVideo::createMediaSource()
{
	if (mediaSource_.isValid())
	{
		return true;
	}

	ocean_assert(!symbolicLink_.empty());
	if (symbolicLink_.empty())
	{
		return false;
	}

	ScopedIMFAttributes attributes;

	if (S_OK != MFCreateAttributes(&attributes.resetObject(), 1))
	{
		return false;
	}

	ocean_assert(attributes.isValid());

	if (S_OK != attributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID))
	{
		return false;
	}

	std::wstring symbolicLink(String::toWString(symbolicLink_));

	if (S_OK != attributes->SetString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK, symbolicLink.data()))
	{
		return false;
	}

	if (S_OK != MFCreateDeviceSource(*attributes, &mediaSource_.resetObject()))
	{
		return false;
	}

	return true;
}

void MFLiveVideo::releaseTopology()
{
	releaseFrameTopology();

	MFMedium::releaseTopology();
}

}

}

}
