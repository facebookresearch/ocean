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
				MFFrameMedium::MediaFrameType mediaFrameType;
				if (MFFrameMedium::extractFrameFormat(*mediaType, mediaFrameType))
				{
					// we do not explicitly know whether a media type is based on an uncompressed frame or a encoded video stream, thus we us the number of planes as decision

					if (mediaFrameType.numberPlanes() == 1u)
					{
						streamTypes.emplace(ST_FRAME);
					}
					else
					{
						streamTypes.emplace(ST_CODEC);
					}
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

	StreamConfigurations streamConfigurations;

	DWORD mediaTypeCount = 0u;
	if (S_OK == mediaTypeHandler_->GetMediaTypeCount(&mediaTypeCount))
	{
		streamConfigurations.reserve(mediaTypeCount);

		unsigned int currentWidth = 0u;
		unsigned int currentHeight = 0u;
		StreamType currentStreamType = ST_INVALID;
		std::vector<double> currentFrameRates;
		FrameType::PixelFormat currentPixelFormat = FrameType::FORMAT_UNDEFINED;

		for (DWORD mediaTypeIndex = 0u; mediaTypeIndex < mediaTypeCount; ++mediaTypeIndex)
		{
			ScopedIMFMediaType mediaType;
			if (S_OK == mediaTypeHandler_->GetMediaTypeByIndex(mediaTypeIndex, &mediaType.resetObject()))
			{
				MFFrameMedium::MediaFrameType mediaFrameType;
				if (MFFrameMedium::extractFrameFormat(*mediaType, mediaFrameType))
				{
					const StreamType mediaStreamType = mediaFrameType.numberPlanes() == 1u ? ST_FRAME : ST_CODEC; // we do not explicitly know whether a media type is based on an uncompressed frame or a encoded video stream, thus we us the number of planes as decision

					if (currentWidth == mediaFrameType.width() && currentHeight == mediaFrameType.height() && currentStreamType == mediaStreamType && currentPixelFormat == mediaFrameType.pixelFormat())
					{
						currentFrameRates.push_back(mediaFrameType.frequency());
					}
					else
					{
						if (!currentFrameRates.empty())
						{
							if (streamType == ST_INVALID || streamType == mediaStreamType)
							{
								streamConfigurations.emplace_back(currentStreamType, currentWidth, currentHeight, std::move(currentFrameRates), currentPixelFormat, CT_INVALID);
							}
						}

						currentWidth = mediaFrameType.width();
						currentHeight = mediaFrameType.height();
						currentStreamType = mediaStreamType;
						currentPixelFormat = mediaFrameType.pixelFormat();

						currentFrameRates = std::vector<double>(1, mediaFrameType.frequency());
					}
				}
			}
		}

		if (!currentFrameRates.empty())
		{
			if (streamType == ST_INVALID || streamType == currentStreamType)
			{
				streamConfigurations.emplace_back(currentStreamType, currentWidth, currentHeight, std::move(currentFrameRates), currentPixelFormat, CT_INVALID);
			}
		}
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
