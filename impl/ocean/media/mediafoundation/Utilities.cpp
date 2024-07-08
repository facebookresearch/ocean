/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/mediafoundation/Utilities.h"

#include "ocean/base/String.h"

#include <mfreadwrite.h>
#include <mferror.h>
#include <uuids.h>
#include <wmcodecdsp.h>
#include <Evr.h>
#include <Wmcontainer.h>
#include <Mfmp2dlna.h>

#if defined(OCEAN_COMPILER_MSC) && OCEAN_MSC_VERSION >= 1700
	#include <Mfcaptureengine.h>
	#include <Mfmediaengine.h>
#endif

namespace Ocean
{

namespace Media
{

namespace MediaFoundation
{

std::string Utilities::guid2String(const GUID& value)
{
	const static IdMap map(guidMap());

	const IdMap::const_iterator i = map.find(value);
	if (i != map.end())
	{
		return i->second;
	}

	if (IsEqualGUID(value, FORMAT_WaveFormatEx))
	{
		return "FORMAT_WaveFormatEx";
	}

	if (IsEqualGUID(value, MEDIASUBTYPE_H264))
	{
		return "MEDIASUBTYPE_H264";
	}

	if (IsEqualGUID(value, FORMAT_MPEG2_VIDEO))
	{
		return "FORMAT_MPEG2_VIDEO";
	}

	if (IsEqualGUID(value, MEDIASUBTYPE_MPEG_HEAAC))
	{
		return "MEDIASUBTYPE_MPEG_HEAAC";
	}

	const std::string videoSubtype(unregisteredVideoSubtype2String(value));

	if (!videoSubtype.empty())
	{
		return videoSubtype;
	}

	return String::toAStringHex(value.Data1) + std::string("-") + String::toAStringHex(value.Data2) + std::string("-") + String::toAStringHex(value.Data3) + std::string("-") + String::toAStringHex(value.Data4, 2) + std::string("-") + String::toAStringHex(value.Data4 + 2, 6);
}

bool Utilities::dumpAttributes(IMFAttributes* attributes, std::string& result)
{
	ocean_assert(attributes);

	UINT32 count = 0;
	if (attributes->GetCount(&count) != S_OK)
	{
		return false;
	}

	for (UINT32 n = 0u; n < count; ++n)
	{
		GUID key;
		MF_ATTRIBUTE_TYPE type = MF_ATTRIBUTE_IUNKNOWN;

		if (attributes->GetItemByIndex(n, &key, nullptr) == S_OK && attributes->GetItemType(key, &type) == S_OK)
		{
			result += "Attribute ";
			result += String::toAString(n);
			result += " (";
			result += guid2String(key);
			result += "): ";

			switch (type)
			{
				case MF_ATTRIBUTE_UINT32:
				{
					UINT32 value = 0;
					attributes->GetUINT32(key, &value);
					result += String::toAString(value);
					break;
				}

				case MF_ATTRIBUTE_UINT64:
				{
					UINT64 value = 0;
					attributes->GetUINT64(key, &value);
					result += String::toAString(value);
					break;
				}

				case MF_ATTRIBUTE_DOUBLE:
				{
					double value = 0;
					attributes->GetDouble(key, &value);
					result += String::toAString(value);
					break;
				}

				case MF_ATTRIBUTE_GUID:
				{
					GUID value;
					attributes->GetGUID(key, &value);
					result += guid2String(value);
					break;
				}

				case MF_ATTRIBUTE_STRING:
				{
					UINT32 length = 0;
					attributes->GetStringLength(key, &length);

					if (length <= 4095)
					{
						wchar_t value[4096];
						attributes->GetString(key, value, 4095, nullptr);
						result += String::toAString(value);
					}

					break;
				}

				case MF_ATTRIBUTE_BLOB:
				{
					result += "**BLOB**";

					break;
				}

				default:
					ocean_assert(false && "Invalid attribute type!");
			}

			result += "\n";
		}
	}

	return true;
}

std::string Utilities::attribute2String(const GUID& attribute)
{
	const static IdMap map(attributeMap());

	const IdMap::const_iterator i = map.find(attribute);
	if (i != map.end())
	{
		return i->second;
	}

	return std::string();
}

std::string Utilities::majorMediaType2String(const GUID& type)
{
	const static IdMap map(majorMediaTypeMap());

	const IdMap::const_iterator i = map.find(type);
	if (i != map.end())
	{
		return i->second;
	}

	return std::string();
}

std::string Utilities::videoSubtype2String(const GUID& type)
{
	const static IdMap map(videoSubtypeMap());

	const IdMap::const_iterator i = map.find(type);
	if (i != map.end())
	{
		return i->second;
	}

	return std::string();
}

std::string Utilities::unregisteredVideoSubtype2String(const GUID& type)
{
	// ????-0000-0010-8000-00AA00389B71
	constexpr uint8_t videoFormatBase[] = {0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71};

	const uint8_t* fourCC = (const uint8_t*)(&type);

	if (memcmp(fourCC + 4, videoFormatBase, sizeof(videoFormatBase)) == 0)
	{
		for (unsigned int n = 0u; n < 4u; ++n)
		{
			if (!isalpha(fourCC[n]) && !isalnum(fourCC[n]))
			{
				return std::string();
			}
		}

		return std::string("MFVideoFormat_") + std::string((char*)fourCC, 4);
	}

	return std::string();
}

std::string Utilities::audioSubtype2String(const GUID& type)
{
	const static IdMap map(audioSubtypeMap());

	const IdMap::const_iterator i = map.find(type);
	if (i != map.end())
	{
		return i->second;
	}

	return std::string();
}

std::string Utilities::transformCategory2String(const GUID& type)
{
	const static IdMap map(transformCategoryMap());

	const IdMap::const_iterator i = map.find(type);
	if (i != map.end())
	{
		return i->second;
	}

	return std::string();
}

bool Utilities::enumerateTransforms(std::string& result)
{
	IMFActivate** activates = nullptr;
	UINT32 numberActivates = 0;

	using CategoryPair = std::pair<GUID, std::string>;

	static std::vector<CategoryPair> categories =
	{
		CategoryPair(MFT_CATEGORY_AUDIO_DECODER, "Audio decoders"),
		CategoryPair(MFT_CATEGORY_AUDIO_EFFECT, "Audio effects"),
		CategoryPair(MFT_CATEGORY_AUDIO_ENCODER, "Audio encoders"),
		CategoryPair(MFT_CATEGORY_DEMULTIPLEXER, "Demultiplexers"),
		CategoryPair(MFT_CATEGORY_MULTIPLEXER, "Multiplexers"),
		CategoryPair(MFT_CATEGORY_OTHER, "Miscellaneous MFTs"),
		CategoryPair(MFT_CATEGORY_VIDEO_DECODER, "Video decoders"),
		CategoryPair(MFT_CATEGORY_VIDEO_EFFECT, "Video effects"),
		CategoryPair(MFT_CATEGORY_VIDEO_ENCODER, "Video encoders"),
		CategoryPair(MFT_CATEGORY_VIDEO_PROCESSOR, "Video processors")
	};

	bool first = true;

	for (const CategoryPair& categoryPair : categories)
	{
		if (!first)
		{
			result += "\n\n";
		}

		first = false;

		result += "Transform category: ";
		result += categoryPair.second;
		result += ":\n";

		const HRESULT enumResult = MFTEnumEx(categoryPair.first, MFT_ENUM_FLAG_ALL, nullptr, nullptr, &activates, &numberActivates);
		ocean_assert_and_suppress_unused(enumResult == S_OK, enumResult);

		for (size_t n = 0; n < numberActivates; ++n)
		{
			if (activates[n])
			{
				ScopedMediaFoundationObject<IMFTransform> transform;
				activates[n]->ActivateObject(IID_PPV_ARGS(&transform.resetObject()));

				dumpAttributes(activates[n], result);
				result += "\n";

				activates[n]->Release();
			}
		}

		if (activates)
		{
			CoTaskMemFree(activates);
		}
	}

	return true;
}

FrameType::PixelFormat Utilities::convertMediaSubtype(const GUID& mediaSubtype)
{
	if (mediaSubtype == MFVideoFormat_I420)
	{
		return FrameType::FORMAT_Y_U_V12;
	}

	if (mediaSubtype == MFVideoFormat_IYUV)
	{
		return FrameType::FORMAT_UNDEFINED;
	}

	if (mediaSubtype == MFVideoFormat_RGB24)
	{
		return FrameType::FORMAT_BGR24;
	}

	if (mediaSubtype == MFVideoFormat_RGB32)
	{
		return FrameType::FORMAT_BGR32;
	}

	if (mediaSubtype == MFVideoFormat_YUY2)
	{
		return FrameType::FORMAT_YUYV16;
	}

	if (mediaSubtype == MFVideoFormat_NV12)
	{
		return FrameType::FORMAT_Y_UV12;
	}

	if (mediaSubtype == MFVideoFormat_YV12)
	{
		return FrameType::FORMAT_UNDEFINED;
	}

	if (mediaSubtype == MFVideoFormat_MJPG)
	{
		return FrameType::FORMAT_UNDEFINED;
	}

	return FrameType::FORMAT_UNDEFINED;
}

FrameType::PixelOrigin Utilities::extractPixelOrigin(const GUID& mediaSubtype)
{
	if (mediaSubtype == MFVideoFormat_RGB24 || mediaSubtype == MFVideoFormat_RGB32)
	{
		return FrameType::ORIGIN_LOWER_LEFT;
	}

	if (mediaSubtype == MFVideoFormat_I420 || mediaSubtype == MFVideoFormat_IYUV || mediaSubtype == MFVideoFormat_YUY2 || mediaSubtype == MFVideoFormat_NV12 || mediaSubtype == MFVideoFormat_YV12)
	{
		return FrameType::ORIGIN_UPPER_LEFT;
	}

	return FrameType::ORIGIN_INVALID;
}

GUID Utilities::convertPixelFormat(const FrameType::PixelFormat pixelFormat)
{
	switch (pixelFormat)
	{
		case FrameType::FORMAT_BGR24:
			return MFVideoFormat_RGB24;

		case FrameType::FORMAT_BGR32:
			return MFVideoFormat_RGB32;

		case FrameType::FORMAT_Y_U_V12:
			return MFVideoFormat_I420;

		case FrameType::FORMAT_YUYV16:
			return MFVideoFormat_YUY2;

		case FrameType::FORMAT_Y_UV12:
			return MFVideoFormat_NV12;

		default:
			break;
	}

	return GUID_NULL;
}

ScopedIMFMediaSource Utilities::createMediaSourceByUrl(const std::wstring& url)
{
	ocean_assert(!url.empty());

	if (url.empty())
	{
		return ScopedIMFMediaSource();
	}

	ScopedMediaFoundationObject<IMFSourceResolver> sourceResolver;
	if (S_OK != MFCreateSourceResolver(&sourceResolver.resetObject()))
	{
		return ScopedIMFMediaSource();
	}

	ScopedMediaFoundationObject<IUnknown> source;
	MF_OBJECT_TYPE objectType;

	if (S_OK != sourceResolver->CreateObjectFromURL(url.c_str(), MF_RESOLUTION_MEDIASOURCE, nullptr, &objectType, &source.resetObject()))
	{
		return ScopedIMFMediaSource();
	}

	ScopedIMFMediaSource mediaSource;
	if (S_OK != source->QueryInterface(IID_PPV_ARGS(&mediaSource.resetObject())))
	{
		return ScopedIMFMediaSource();
	}

	return mediaSource;
}

ScopedIMFTopologyNode Utilities::addSourceNodeToTopology(IMFTopology* topology, IMFMediaSource* source, IMFPresentationDescriptor* presentationDiscriptor, IMFStreamDescriptor* streamDescriptor)
{
	ocean_assert(topology != nullptr && source != nullptr);
	ocean_assert(presentationDiscriptor != nullptr && streamDescriptor != nullptr);

	ScopedIMFTopologyNode node;
	if (S_OK != MFCreateTopologyNode(MF_TOPOLOGY_SOURCESTREAM_NODE, &node.resetObject()))
	{
		return ScopedIMFTopologyNode();
	}

	if (S_OK != node->SetUnknown(MF_TOPONODE_SOURCE, source))
	{
		return ScopedIMFTopologyNode();
	}

	if (S_OK != node->SetUnknown(MF_TOPONODE_PRESENTATION_DESCRIPTOR, presentationDiscriptor))
	{
		return ScopedIMFTopologyNode();
	}

	if (S_OK != node->SetUnknown(MF_TOPONODE_STREAM_DESCRIPTOR, streamDescriptor))
	{
		return ScopedIMFTopologyNode();
	}

	if (S_OK != topology->AddNode(*node))
	{
		return ScopedIMFTopologyNode();
	}

	return node;
}

ScopedIMFTopologyNode Utilities::addOutputNodeToTopology(IMFTopology* topology, IMFActivate* sinkActivate, const DWORD streamIndex)
{
	ocean_assert(topology != nullptr && sinkActivate != nullptr);

	ScopedIMFTopologyNode node;
	if (S_OK != MFCreateTopologyNode(MF_TOPOLOGY_OUTPUT_NODE, &node.resetObject()))
	{
		return ScopedIMFTopologyNode();
	}

	if (S_OK != node->SetObject(sinkActivate))
	{
		return ScopedIMFTopologyNode();
	}

	if (S_OK != node->SetUINT32(MF_TOPONODE_STREAMID, streamIndex))
	{
		return ScopedIMFTopologyNode();
	}

	if (S_OK != node->SetUINT32(MF_TOPONODE_NOSHUTDOWN_ON_REMOVE, FALSE))
	{
		return ScopedIMFTopologyNode();
	}

	if (S_OK != topology->AddNode(*node))
	{
		return ScopedIMFTopologyNode();
	}

	return node;
}

bool Utilities::connectSelectedStream(IMFTopology* topology, IMFMediaSource* mediaSource, IMFActivate* sinkActivate, const GUID& majorMediaType)
{
	ocean_assert(topology != nullptr && mediaSource != nullptr && sinkActivate != nullptr);

	ScopedMediaFoundationObject<IMFPresentationDescriptor> presentationDescriptor;
	if (S_OK != mediaSource->CreatePresentationDescriptor(&presentationDescriptor.resetObject()))
	{
		return false;
	}

	DWORD number = 0;
	if (S_OK != presentationDescriptor->GetStreamDescriptorCount(&number))
	{
		return false;
	}

	for (DWORD i = 0; i < number; i++)
	{
		BOOL selected = FALSE;
		ScopedMediaFoundationObject<IMFStreamDescriptor> streamDescriptor;

		if (S_OK == presentationDescriptor->GetStreamDescriptorByIndex(i, &selected, &streamDescriptor.resetObject()) && selected == TRUE)
		{
			ScopedMediaFoundationObject<IMFMediaTypeHandler> mediaTypeHandler;

			GUID type;
			if (S_OK == streamDescriptor->GetMediaTypeHandler(&mediaTypeHandler.resetObject()) && S_OK == mediaTypeHandler->GetMajorType(&type) && type == majorMediaType)
			{
				ScopedIMFTopologyNode inputNode = addSourceNodeToTopology(topology, mediaSource, *presentationDescriptor, *streamDescriptor);
				ScopedIMFTopologyNode outputNode = addOutputNodeToTopology(topology, sinkActivate);

				if (inputNode.isValid() && outputNode.isValid())
				{
					if (S_OK == inputNode->ConnectOutput(0, *outputNode, 0))
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool Utilities::getAllocatedString(IMFActivate* activate, const GUID& key, std::string& value)
{
	ocean_assert(activate != nullptr);

	LPWSTR stringValue = nullptr;
	UINT32 stringLength = 0u;

	if (activate != nullptr && S_OK == activate->GetAllocatedString(key, &stringValue, &stringLength))
	{
		ocean_assert(stringLength == 0u || stringValue != nullptr);

		if (stringLength > 0u && stringValue != nullptr)
		{
			const std::wstring intermediateString(stringValue, size_t(stringLength));

			value = String::toAString(intermediateString);
		}
		else
		{
			value.clear();
		}

		CoTaskMemFree(stringValue);

		return true;
	}

	return false;
}

Utilities::IdMap Utilities::guidMap()
{
	IdMap map;

	const IdMap map0(attributeMap());
	const IdMap map1(majorMediaTypeMap());
	const IdMap map2(videoSubtypeMap());
	const IdMap map3(audioSubtypeMap());
	const IdMap map4(transformCategoryMap());

	map.insert(map0.begin(), map0.end());
	map.insert(map1.begin(), map1.end());
	map.insert(map2.begin(), map2.end());
	map.insert(map3.begin(), map3.end());
	map.insert(map4.begin(), map4.end());

	return map;
}

Utilities::IdMap Utilities::attributeMap()
{
	IdMap map;

	map[EVRConfig_AllowBatching] = "EVRConfig_AllowBatching";
	map[EVRConfig_AllowDropToBob] = "EVRConfig_AllowDropToBob";
	map[EVRConfig_AllowDropToHalfInterlace] = "EVRConfig_AllowDropToHalfInterlace";
	map[EVRConfig_AllowDropToThrottle] = "EVRConfig_AllowDropToThrottle";
	map[EVRConfig_AllowScaling] = "EVRConfig_AllowScaling";
	map[EVRConfig_ForceBatching] = "EVRConfig_ForceBatching";
	map[EVRConfig_ForceBob] = "EVRConfig_ForceBob";
	map[EVRConfig_ForceHalfInterlace] = "EVRConfig_ForceHalfInterlace";
	map[EVRConfig_ForceScaling] = "EVRConfig_ForceScaling";
	map[EVRConfig_ForceThrottle] = "EVRConfig_ForceThrottle";
	map[MF_ACTIVATE_CUSTOM_VIDEO_MIXER_ACTIVATE] = "MF_ACTIVATE_CUSTOM_VIDEO_MIXER_ACTIVATE";
	map[MF_ACTIVATE_CUSTOM_VIDEO_MIXER_CLSID] = "MF_ACTIVATE_CUSTOM_VIDEO_MIXER_CLSID";
	map[MF_ACTIVATE_CUSTOM_VIDEO_MIXER_FLAGS] = "MF_ACTIVATE_CUSTOM_VIDEO_MIXER_FLAGS";
	map[MF_ACTIVATE_CUSTOM_VIDEO_PRESENTER_ACTIVATE] = "MF_ACTIVATE_CUSTOM_VIDEO_PRESENTER_ACTIVATE";
	map[MF_ACTIVATE_CUSTOM_VIDEO_PRESENTER_CLSID] = "MF_ACTIVATE_CUSTOM_VIDEO_PRESENTER_CLSID";
	map[MF_ACTIVATE_CUSTOM_VIDEO_PRESENTER_FLAGS] = "MF_ACTIVATE_CUSTOM_VIDEO_PRESENTER_FLAGS";
	map[MF_ACTIVATE_MFT_LOCKED] = "MF_ACTIVATE_MFT_LOCKED";
	map[MF_ACTIVATE_VIDEO_WINDOW] = "MF_ACTIVATE_VIDEO_WINDOW";
	map[MF_ASFPROFILE_MAXPACKETSIZE] = "MF_ASFPROFILE_MAXPACKETSIZE";
	map[MF_ASFPROFILE_MINPACKETSIZE] = "MF_ASFPROFILE_MINPACKETSIZE";
	map[MF_ASFSTREAMCONFIG_LEAKYBUCKET1] = "MF_ASFSTREAMCONFIG_LEAKYBUCKET1";
	map[MF_ASFSTREAMCONFIG_LEAKYBUCKET2] = "MF_ASFSTREAMCONFIG_LEAKYBUCKET2";
	map[MF_AUDIO_RENDERER_ATTRIBUTE_ENDPOINT_ID] = "MF_AUDIO_RENDERER_ATTRIBUTE_ENDPOINT_ID";
	map[MF_AUDIO_RENDERER_ATTRIBUTE_ENDPOINT_ROLE] = "MF_AUDIO_RENDERER_ATTRIBUTE_ENDPOINT_ROLE";
	map[MF_AUDIO_RENDERER_ATTRIBUTE_FLAGS] = "MF_AUDIO_RENDERER_ATTRIBUTE_FLAGS";
	map[MF_AUDIO_RENDERER_ATTRIBUTE_SESSION_ID] = "MF_AUDIO_RENDERER_ATTRIBUTE_SESSION_ID";

	map[MF_BYTESTREAM_CONTENT_TYPE] = "MF_BYTESTREAM_CONTENT_TYPE";
	map[MF_BYTESTREAM_DURATION] = "MF_BYTESTREAM_DURATION";

	map[MF_BYTESTREAM_IFO_FILE_URI] = "MF_BYTESTREAM_IFO_FILE_URI";
	map[MF_BYTESTREAM_LAST_MODIFIED_TIME] = "MF_BYTESTREAM_LAST_MODIFIED_TIME";
	map[MF_BYTESTREAM_ORIGIN_NAME] = "MF_BYTESTREAM_ORIGIN_NAME";
	map[MF_BYTESTREAMHANDLER_ACCEPTS_SHARE_WRITE] = "MF_BYTESTREAMHANDLER_ACCEPTS_SHARE_WRITE";

	map[MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME] = "MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME";
	map[MF_DEVSOURCE_ATTRIBUTE_MEDIA_TYPE] = "MF_DEVSOURCE_ATTRIBUTE_MEDIA_TYPE";
	map[MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE] = "MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE";
	map[MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_ENDPOINT_ID] = "MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_ENDPOINT_ID";
	map[MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_ROLE] = "MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_ROLE";
	map[MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_CATEGORY] = "MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_CATEGORY";
	map[MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_HW_SOURCE] = "MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_HW_SOURCE";
	map[MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_MAX_BUFFERS] = "MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_MAX_BUFFERS";
	map[MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK] = "MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK";

	map[MF_EVENT_DO_THINNING] = "MF_EVENT_DO_THINNING";
	map[MF_EVENT_MFT_CONTEXT] = "MF_EVENT_MFT_CONTEXT";
	map[MF_EVENT_MFT_INPUT_STREAM_ID] = "MF_EVENT_MFT_INPUT_STREAM_ID";
	map[MF_EVENT_OUTPUT_NODE] = "MF_EVENT_OUTPUT_NODE";
	map[MF_EVENT_PRESENTATION_TIME_OFFSET] = "MF_EVENT_PRESENTATION_TIME_OFFSET";
	map[MF_EVENT_SCRUBSAMPLE_TIME] = "MF_EVENT_SCRUBSAMPLE_TIME";
	map[MF_EVENT_SESSIONCAPS] = "MF_EVENT_SESSIONCAPS";
	map[MF_EVENT_SESSIONCAPS_DELTA] = "MF_EVENT_SESSIONCAPS_DELTA";
	map[MF_EVENT_SOURCE_ACTUAL_START] = "MF_EVENT_SOURCE_ACTUAL_START";
	map[MF_EVENT_SOURCE_CHARACTERISTICS] = "MF_EVENT_SOURCE_CHARACTERISTICS";
	map[MF_EVENT_SOURCE_CHARACTERISTICS_OLD] = "MF_EVENT_SOURCE_CHARACTERISTICS_OLD";
	map[MF_EVENT_SOURCE_FAKE_START] = "MF_EVENT_SOURCE_FAKE_START";
	map[MF_EVENT_SOURCE_PROJECTSTART] = "MF_EVENT_SOURCE_PROJECTSTART";
	map[MF_EVENT_SOURCE_TOPOLOGY_CANCELED] = "MF_EVENT_SOURCE_TOPOLOGY_CANCELED";
	map[MF_EVENT_START_PRESENTATION_TIME] = "MF_EVENT_START_PRESENTATION_TIME";
	map[MF_EVENT_START_PRESENTATION_TIME_AT_OUTPUT] = "MF_EVENT_START_PRESENTATION_TIME_AT_OUTPUT";
	map[MF_EVENT_TOPOLOGY_STATUS] = "MF_EVENT_TOPOLOGY_STATUS";

	map[MF_MP2DLNA_AUDIO_BIT_RATE] = "MF_MP2DLNA_AUDIO_BIT_RATE";
	map[MF_MP2DLNA_ENCODE_QUALITY] = "MF_MP2DLNA_ENCODE_QUALITY";
	map[MF_MP2DLNA_STATISTICS] = "MF_MP2DLNA_STATISTICS";
	map[MF_MP2DLNA_USE_MMCSS] = "MF_MP2DLNA_USE_MMCSS";
	map[MF_MP2DLNA_VIDEO_BIT_RATE] = "MF_MP2DLNA_VIDEO_BIT_RATE";

	map[MF_MT_AAC_AUDIO_PROFILE_LEVEL_INDICATION] = "MF_MT_AAC_AUDIO_PROFILE_LEVEL_INDICATION";
	map[MF_MT_AAC_PAYLOAD_TYPE] = "MF_MT_AAC_PAYLOAD_TYPE";
	map[MF_MT_ALL_SAMPLES_INDEPENDENT] = "MF_MT_ALL_SAMPLES_INDEPENDENT";
	map[MF_MT_AM_FORMAT_TYPE] = "MF_MT_AM_FORMAT_TYPE";
	map[MF_MT_ARBITRARY_FORMAT] = "MF_MT_ARBITRARY_FORMAT";
	map[MF_MT_ARBITRARY_HEADER] = "MF_MT_ARBITRARY_HEADER";
	map[MF_MT_AUDIO_AVG_BYTES_PER_SECOND] = "MF_MT_AUDIO_AVG_BYTES_PER_SECOND";
	map[MF_MT_AUDIO_BITS_PER_SAMPLE] = "MF_MT_AUDIO_BITS_PER_SAMPLE";
	map[MF_MT_AUDIO_BLOCK_ALIGNMENT] = "MF_MT_AUDIO_BLOCK_ALIGNMENT";
	map[MF_MT_AUDIO_CHANNEL_MASK] = "MF_MT_AUDIO_CHANNEL_MASK";
	map[MF_MT_AUDIO_FLOAT_SAMPLES_PER_SECOND] = "MF_MT_AUDIO_FLOAT_SAMPLES_PER_SECOND";
	map[MF_MT_AUDIO_FOLDDOWN_MATRIX] = "MF_MT_AUDIO_FOLDDOWN_MATRIX";
	map[MF_MT_AUDIO_NUM_CHANNELS] = "MF_MT_AUDIO_NUM_CHANNELS";
	map[MF_MT_AUDIO_PREFER_WAVEFORMATEX] = "MF_MT_AUDIO_PREFER_WAVEFORMATEX";
	map[MF_MT_AUDIO_SAMPLES_PER_BLOCK] = "MF_MT_AUDIO_SAMPLES_PER_BLOCK";
	map[MF_MT_AUDIO_SAMPLES_PER_SECOND] = "MF_MT_AUDIO_SAMPLES_PER_SECOND";
	map[MF_MT_AUDIO_VALID_BITS_PER_SAMPLE] = "MF_MT_AUDIO_VALID_BITS_PER_SAMPLE";
	map[MF_MT_AUDIO_WMADRC_AVGREF] = "MF_MT_AUDIO_WMADRC_AVGREF";
	map[MF_MT_AUDIO_WMADRC_AVGTARGET] = "MF_MT_AUDIO_WMADRC_AVGTARGET";
	map[MF_MT_AUDIO_WMADRC_PEAKREF] = "MF_MT_AUDIO_WMADRC_PEAKREF";
	map[MF_MT_AUDIO_WMADRC_PEAKTARGET] = "MF_MT_AUDIO_WMADRC_PEAKTARGET";
	map[MF_MT_AVG_BIT_ERROR_RATE] = "MF_MT_AVG_BIT_ERROR_RATE";
	map[MF_MT_AVG_BITRATE] = "MF_MT_AVG_BITRATE";
	map[MF_MT_COMPRESSED] = "MF_MT_COMPRESSED";
	map[MF_MT_CUSTOM_VIDEO_PRIMARIES] = "MF_MT_CUSTOM_VIDEO_PRIMARIES";
	map[MF_MT_DEFAULT_STRIDE] = "MF_MT_DEFAULT_STRIDE";
	map[MF_MT_DRM_FLAGS] = "MF_MT_DRM_FLAGS";
	map[MF_MT_DV_AAUX_CTRL_PACK_0] = "MF_MT_DV_AAUX_CTRL_PACK_0";
	map[MF_MT_DV_AAUX_CTRL_PACK_1] = "MF_MT_DV_AAUX_CTRL_PACK_1";
	map[MF_MT_DV_AAUX_SRC_PACK_0] = "MF_MT_DV_AAUX_SRC_PACK_0";
	map[MF_MT_DV_AAUX_SRC_PACK_1] = "MF_MT_DV_AAUX_SRC_PACK_1";
	map[MF_MT_DV_VAUX_CTRL_PACK] = "MF_MT_DV_VAUX_CTRL_PACK";
	map[MF_MT_DV_VAUX_SRC_PACK] = "MF_MT_DV_VAUX_SRC_PACK";
	map[MF_MT_FIXED_SIZE_SAMPLES] = "MF_MT_FIXED_SIZE_SAMPLES";
	map[MF_MT_FRAME_RATE] = "MF_MT_FRAME_RATE";
	map[MF_MT_FRAME_RATE_RANGE_MAX] = "MF_MT_FRAME_RATE_RANGE_MAX";
	map[MF_MT_FRAME_RATE_RANGE_MIN] = "MF_MT_FRAME_RATE_RANGE_MIN";
	map[MF_MT_FRAME_SIZE] = "MF_MT_FRAME_SIZE";
	map[MF_MT_GEOMETRIC_APERTURE] = "MF_MT_GEOMETRIC_APERTURE";

	map[MF_MT_IMAGE_LOSS_TOLERANT] = "MF_MT_IMAGE_LOSS_TOLERANT";
	map[MF_MT_INTERLACE_MODE] = "MF_MT_INTERLACE_MODE";
	map[MF_MT_MAJOR_TYPE] = "MF_MT_MAJOR_TYPE";
	map[MF_MT_MAX_KEYFRAME_SPACING] = "MF_MT_MAX_KEYFRAME_SPACING";
	map[MF_MT_MINIMUM_DISPLAY_APERTURE] = "MF_MT_MINIMUM_DISPLAY_APERTURE";
	map[MF_MT_MPEG_SEQUENCE_HEADER] = "MF_MT_MPEG_SEQUENCE_HEADER";
	map[MF_MT_MPEG_START_TIME_CODE] = "MF_MT_MPEG_START_TIME_CODE";

	map[MF_MT_MPEG2_FLAGS] = "MF_MT_MPEG2_FLAGS";
	map[MF_MT_MPEG2_LEVEL] = "MF_MT_MPEG2_LEVEL";
	map[MF_MT_MPEG2_PROFILE] = "MF_MT_MPEG2_PROFILE";

	map[MF_MT_MPEG4_CURRENT_SAMPLE_ENTRY] = "MF_MT_MPEG4_CURRENT_SAMPLE_ENTRY";
	map[MF_MT_MPEG4_SAMPLE_DESCRIPTION] = "MF_MT_MPEG4_SAMPLE_DESCRIPTION";
	map[MF_MT_ORIGINAL_4CC] = "MF_MT_ORIGINAL_4CC";
	map[MF_MT_ORIGINAL_WAVE_FORMAT_TAG] = "MF_MT_ORIGINAL_WAVE_FORMAT_TAG";
	map[MF_MT_PAD_CONTROL_FLAGS] = "MF_MT_PAD_CONTROL_FLAGS";
	map[MF_MT_PALETTE] = "MF_MT_PALETTE";
	map[MF_MT_PAN_SCAN_APERTURE] = "MF_MT_PAN_SCAN_APERTURE";
	map[MF_MT_PAN_SCAN_ENABLED] = "MF_MT_PAN_SCAN_ENABLED";
	map[MF_MT_PIXEL_ASPECT_RATIO] = "MF_MT_PIXEL_ASPECT_RATIO";
	map[MF_MT_SAMPLE_SIZE] = "MF_MT_SAMPLE_SIZE";
	map[MF_MT_SOURCE_CONTENT_HINT] = "MF_MT_SOURCE_CONTENT_HINT";
	map[MF_MT_SUBTYPE] = "MF_MT_SUBTYPE";

	map[MF_MT_TRANSFER_FUNCTION] = "MF_MT_TRANSFER_FUNCTION";
	map[MF_MT_USER_DATA] = "MF_MT_USER_DATA";

	map[MF_MT_VIDEO_CHROMA_SITING] = "MF_MT_VIDEO_CHROMA_SITING";
	map[MF_MT_VIDEO_LIGHTING] = "MF_MT_VIDEO_LIGHTING";
	map[MF_MT_VIDEO_NOMINAL_RANGE] = "MF_MT_VIDEO_NOMINAL_RANGE";
	map[MF_MT_VIDEO_PRIMARIES] = "MF_MT_VIDEO_PRIMARIES";

	map[MF_MT_WRAPPED_TYPE] = "MF_MT_WRAPPED_TYPE";
	map[MF_MT_YUV_MATRIX] = "MF_MT_YUV_MATRIX";

	map[MF_PD_APP_CONTEXT] = "MF_PD_APP_CONTEXT";
	map[MF_PD_ASF_CODECLIST] = "MF_PD_ASF_CODECLIST";
	map[MF_PD_ASF_CONTENTENCRYPTION_KEYID] = "MF_PD_ASF_CONTENTENCRYPTION_KEYID";
	map[MF_PD_ASF_CONTENTENCRYPTION_LICENSE_URL] = "MF_PD_ASF_CONTENTENCRYPTION_LICENSE_URL";
	map[MF_PD_ASF_CONTENTENCRYPTION_SECRET_DATA] = "MF_PD_ASF_CONTENTENCRYPTION_SECRET_DATA";
	map[MF_PD_ASF_CONTENTENCRYPTION_TYPE] = "MF_PD_ASF_CONTENTENCRYPTION_TYPE";
	map[MF_PD_ASF_CONTENTENCRYPTIONEX_ENCRYPTION_DATA] = "MF_PD_ASF_CONTENTENCRYPTIONEX_ENCRYPTION_DATA";
	map[MF_PD_ASF_DATA_LENGTH] = "MF_PD_ASF_DATA_LENGTH";
	map[MF_PD_ASF_DATA_START_OFFSET] = "MF_PD_ASF_DATA_START_OFFSET";
	map[MF_PD_ASF_FILEPROPERTIES_CREATION_TIME] = "MF_PD_ASF_FILEPROPERTIES_CREATION_TIME";
	map[MF_PD_ASF_FILEPROPERTIES_FILE_ID] = "MF_PD_ASF_FILEPROPERTIES_FILE_ID";
	map[MF_PD_ASF_FILEPROPERTIES_FLAGS] = "MF_PD_ASF_FILEPROPERTIES_FLAGS";
	map[MF_PD_ASF_FILEPROPERTIES_MAX_BITRATE] = "MF_PD_ASF_FILEPROPERTIES_MAX_BITRATE";
	map[MF_PD_ASF_FILEPROPERTIES_MAX_PACKET_SIZE] = "MF_PD_ASF_FILEPROPERTIES_MAX_PACKET_SIZE";
	map[MF_PD_ASF_FILEPROPERTIES_MIN_PACKET_SIZE] = "MF_PD_ASF_FILEPROPERTIES_MIN_PACKET_SIZE";
	map[MF_PD_ASF_FILEPROPERTIES_PACKETS] = "MF_PD_ASF_FILEPROPERTIES_PACKETS";
	map[MF_PD_ASF_FILEPROPERTIES_PLAY_DURATION] = "MF_PD_ASF_FILEPROPERTIES_PLAY_DURATION";
	map[MF_PD_ASF_FILEPROPERTIES_PREROLL] = "MF_PD_ASF_FILEPROPERTIES_PREROLL";
	map[MF_PD_ASF_FILEPROPERTIES_SEND_DURATION] = "MF_PD_ASF_FILEPROPERTIES_SEND_DURATION";
	map[MF_PD_ASF_INFO_HAS_AUDIO] = "MF_PD_ASF_INFO_HAS_AUDIO";
	map[MF_PD_ASF_INFO_HAS_NON_AUDIO_VIDEO] = "MF_PD_ASF_INFO_HAS_NON_AUDIO_VIDEO";
	map[MF_PD_ASF_INFO_HAS_VIDEO] = "MF_PD_ASF_INFO_HAS_VIDEO";
	map[MF_PD_ASF_LANGLIST] = "MF_PD_ASF_LANGLIST";
	map[MF_PD_ASF_LANGLIST_LEGACYORDER] = "MF_PD_ASF_LANGLIST_LEGACYORDER";
	map[MF_PD_ASF_MARKER] = "MF_PD_ASF_MARKER";
	map[MF_PD_ASF_METADATA_IS_VBR] = "MF_PD_ASF_METADATA_IS_VBR";
	map[MF_PD_ASF_METADATA_LEAKY_BUCKET_PAIRS] = "MF_PD_ASF_METADATA_LEAKY_BUCKET_PAIRS";
	map[MF_PD_ASF_METADATA_V8_BUFFERAVERAGE] = "MF_PD_ASF_METADATA_V8_BUFFERAVERAGE";
	map[MF_PD_ASF_METADATA_V8_VBRPEAK] = "MF_PD_ASF_METADATA_V8_VBRPEAK";
	map[MF_PD_ASF_SCRIPT] = "MF_PD_ASF_SCRIPT";
	map[MF_PD_AUDIO_ENCODING_BITRATE] = "MF_PD_AUDIO_ENCODING_BITRATE";
	map[MF_PD_AUDIO_ISVARIABLEBITRATE] = "MF_PD_AUDIO_ISVARIABLEBITRATE";
	map[MF_PD_DURATION] = "MF_PD_DURATION";
	map[MF_PD_LAST_MODIFIED_TIME] = "MF_PD_LAST_MODIFIED_TIME";
	map[MF_PD_MIME_TYPE] = "MF_PD_MIME_TYPE";
	map[MF_PD_PLAYBACK_BOUNDARY_TIME] = "MF_PD_PLAYBACK_BOUNDARY_TIME";
	map[MF_PD_PLAYBACK_ELEMENT_ID] = "MF_PD_PLAYBACK_ELEMENT_ID";
	map[MF_PD_PMPHOST_CONTEXT] = "MF_PD_PMPHOST_CONTEXT";
	map[MF_PD_PREFERRED_LANGUAGE] = "MF_PD_PREFERRED_LANGUAGE";
	map[MF_PD_SAMI_STYLELIST] = "MF_PD_SAMI_STYLELIST";
	map[MF_PD_TOTAL_FILE_SIZE] = "MF_PD_TOTAL_FILE_SIZE";
	map[MF_PD_VIDEO_ENCODING_BITRATE] = "MF_PD_VIDEO_ENCODING_BITRATE";

	map[MF_READWRITE_DISABLE_CONVERTERS] = "MF_READWRITE_DISABLE_CONVERTERS";
	map[MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS] = "MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS";

	map[MF_SA_D3D_AWARE] = "MF_SA_D3D_AWARE";

	map[MF_SA_REQUIRED_SAMPLE_COUNT] = "MF_SA_REQUIRED_SAMPLE_COUNT";

	map[MF_SAMPLEGRABBERSINK_IGNORE_CLOCK] = "MF_SAMPLEGRABBERSINK_IGNORE_CLOCK";
	map[MF_SAMPLEGRABBERSINK_SAMPLE_TIME_OFFSET] = "MF_SAMPLEGRABBERSINK_SAMPLE_TIME_OFFSET";
	map[MF_SD_ASF_EXTSTRMPROP_AVG_BUFFERSIZE] = "MF_SD_ASF_EXTSTRMPROP_AVG_BUFFERSIZE";
	map[MF_SD_ASF_EXTSTRMPROP_AVG_DATA_BITRATE] = "MF_SD_ASF_EXTSTRMPROP_AVG_DATA_BITRATE";
	map[MF_SD_ASF_EXTSTRMPROP_LANGUAGE_ID_INDEX] = "MF_SD_ASF_EXTSTRMPROP_LANGUAGE_ID_INDEX";
	map[MF_SD_ASF_EXTSTRMPROP_MAX_BUFFERSIZE] = "MF_SD_ASF_EXTSTRMPROP_MAX_BUFFERSIZE";
	map[MF_SD_ASF_EXTSTRMPROP_MAX_DATA_BITRATE] = "MF_SD_ASF_EXTSTRMPROP_MAX_DATA_BITRATE";
	map[MF_SD_ASF_METADATA_DEVICE_CONFORMANCE_TEMPLATE] = "MF_SD_ASF_METADATA_DEVICE_CONFORMANCE_TEMPLATE";
	map[MF_SD_ASF_STREAMBITRATES_BITRATE] = "MF_SD_ASF_STREAMBITRATES_BITRATE";
	map[MF_SD_LANGUAGE] = "MF_SD_LANGUAGE";
	map[MF_SD_MUTUALLY_EXCLUSIVE] = "MF_SD_MUTUALLY_EXCLUSIVE";
	map[MF_SD_PROTECTED] = "MF_SD_PROTECTED";
	map[MF_SD_SAMI_LANGUAGE] = "MF_SD_SAMI_LANGUAGE";
	map[MF_SD_STREAM_NAME] = "MF_SD_STREAM_NAME";
	map[MF_SESSION_APPROX_EVENT_OCCURRENCE_TIME] = "MF_SESSION_APPROX_EVENT_OCCURRENCE_TIME";
	map[MF_SESSION_CONTENT_PROTECTION_MANAGER] = "MF_SESSION_CONTENT_PROTECTION_MANAGER";
	map[MF_SESSION_GLOBAL_TIME] = "MF_SESSION_GLOBAL_TIME";
	map[MF_SESSION_QUALITY_MANAGER] = "MF_SESSION_QUALITY_MANAGER";
	map[MF_SESSION_REMOTE_SOURCE_MODE] = "MF_SESSION_REMOTE_SOURCE_MODE";
	map[MF_SESSION_SERVER_CONTEXT] = "MF_SESSION_SERVER_CONTEXT";
	map[MF_SESSION_TOPOLOADER] = "MF_SESSION_TOPOLOADER";
	map[MF_SINK_WRITER_ASYNC_CALLBACK] = "MF_SINK_WRITER_ASYNC_CALLBACK";

	map[MF_SINK_WRITER_DISABLE_THROTTLING] = "MF_SINK_WRITER_DISABLE_THROTTLING";

	map[MF_SOURCE_READER_ASYNC_CALLBACK] = "MF_SOURCE_READER_ASYNC_CALLBACK";
	map[MF_SOURCE_READER_D3D_MANAGER] = "MF_SOURCE_READER_D3D_MANAGER";

	map[MF_SOURCE_READER_DISABLE_DXVA] = "MF_SOURCE_READER_DISABLE_DXVA";
	map[MF_SOURCE_READER_DISCONNECT_MEDIASOURCE_ON_SHUTDOWN] = "MF_SOURCE_READER_DISCONNECT_MEDIASOURCE_ON_SHUTDOWN";

	map[MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING] = "MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING";
	map[MF_SOURCE_READER_MEDIASOURCE_CHARACTERISTICS] = "MF_SOURCE_READER_MEDIASOURCE_CHARACTERISTICS";
	map[MF_SOURCE_READER_MEDIASOURCE_CONFIG] = "MF_SOURCE_READER_MEDIASOURCE_CONFIG";

	map[MF_TOPOLOGY_DXVA_MODE] = "MF_TOPOLOGY_DXVA_MODE";
	map[MF_TOPOLOGY_DYNAMIC_CHANGE_NOT_ALLOWED] = "MF_TOPOLOGY_DYNAMIC_CHANGE_NOT_ALLOWED";
	map[MF_TOPOLOGY_ENUMERATE_SOURCE_TYPES] = "MF_TOPOLOGY_ENUMERATE_SOURCE_TYPES";
	map[MF_TOPOLOGY_HARDWARE_MODE] = "MF_TOPOLOGY_HARDWARE_MODE";
	map[MF_TOPOLOGY_NO_MARKIN_MARKOUT] = "MF_TOPOLOGY_NO_MARKIN_MARKOUT";
	map[MF_TOPOLOGY_PLAYBACK_FRAMERATE] = "MF_TOPOLOGY_PLAYBACK_FRAMERATE";
	map[MF_TOPOLOGY_PLAYBACK_MAX_DIMS] = "MF_TOPOLOGY_PLAYBACK_MAX_DIMS";
	map[MF_TOPOLOGY_PROJECTSTART] = "MF_TOPOLOGY_PROJECTSTART";
	map[MF_TOPOLOGY_PROJECTSTOP] = "MF_TOPOLOGY_PROJECTSTOP";
	map[MF_TOPOLOGY_RESOLUTION_STATUS] = "MF_TOPOLOGY_RESOLUTION_STATUS";
	map[MF_TOPOLOGY_START_TIME_ON_PRESENTATION_SWITCH] = "MF_TOPOLOGY_START_TIME_ON_PRESENTATION_SWITCH";
	map[MF_TOPOLOGY_STATIC_PLAYBACK_OPTIMIZATIONS] = "MF_TOPOLOGY_STATIC_PLAYBACK_OPTIMIZATIONS";
	map[MF_TOPONODE_CONNECT_METHOD] = "MF_TOPONODE_CONNECT_METHOD";
	map[MF_TOPONODE_D3DAWARE] = "MF_TOPONODE_D3DAWARE";
	map[MF_TOPONODE_DECODER] = "MF_TOPONODE_DECODER";
	map[MF_TOPONODE_DECRYPTOR] = "MF_TOPONODE_DECRYPTOR";
	map[MF_TOPONODE_DISABLE_PREROLL] = "MF_TOPONODE_DISABLE_PREROLL";
	map[MF_TOPONODE_DISCARDABLE] = "MF_TOPONODE_DISCARDABLE";
	map[MF_TOPONODE_DRAIN] = "MF_TOPONODE_DRAIN";
	map[MF_TOPONODE_ERROR_MAJORTYPE] = "MF_TOPONODE_ERROR_MAJORTYPE";
	map[MF_TOPONODE_ERROR_SUBTYPE] = "MF_TOPONODE_ERROR_SUBTYPE";
	map[MF_TOPONODE_ERRORCODE] = "MF_TOPONODE_ERRORCODE";
	map[MF_TOPONODE_FLUSH] = "MF_TOPONODE_FLUSH";
	map[MF_TOPONODE_LOCKED] = "MF_TOPONODE_LOCKED";
	map[MF_TOPONODE_MARKIN_HERE] = "MF_TOPONODE_MARKIN_HERE";
	map[MF_TOPONODE_MARKOUT_HERE] = "MF_TOPONODE_MARKOUT_HERE";
	map[MF_TOPONODE_MEDIASTART] = "MF_TOPONODE_MEDIASTART";
	map[MF_TOPONODE_MEDIASTOP] = "MF_TOPONODE_MEDIASTOP";
	map[MF_TOPONODE_NOSHUTDOWN_ON_REMOVE] = "MF_TOPONODE_NOSHUTDOWN_ON_REMOVE";
	map[MF_TOPONODE_PRESENTATION_DESCRIPTOR] = "MF_TOPONODE_PRESENTATION_DESCRIPTOR";
	map[MF_TOPONODE_PRIMARYOUTPUT] = "MF_TOPONODE_PRIMARYOUTPUT";
	map[MF_TOPONODE_RATELESS] = "MF_TOPONODE_RATELESS";
	map[MF_TOPONODE_SEQUENCE_ELEMENTID] = "MF_TOPONODE_SEQUENCE_ELEMENTID";
	map[MF_TOPONODE_SOURCE] = "MF_TOPONODE_SOURCE";
	map[MF_TOPONODE_STREAM_DESCRIPTOR] = "MF_TOPONODE_STREAM_DESCRIPTOR";
	map[MF_TOPONODE_STREAMID] = "MF_TOPONODE_STREAMID";
	map[MF_TOPONODE_TRANSFORM_OBJECTID] = "MF_TOPONODE_TRANSFORM_OBJECTID";
	map[MF_TOPONODE_WORKQUEUE_ID] = "MF_TOPONODE_WORKQUEUE_ID";

	map[MF_TOPONODE_WORKQUEUE_MMCSS_CLASS] = "MF_TOPONODE_WORKQUEUE_MMCSS_CLASS";

	map[MF_TOPONODE_WORKQUEUE_MMCSS_TASKID] = "MF_TOPONODE_WORKQUEUE_MMCSS_TASKID";
	map[MF_TRANSCODE_ADJUST_PROFILE] = "MF_TRANSCODE_ADJUST_PROFILE";
	map[MF_TRANSCODE_CONTAINERTYPE] = "MF_TRANSCODE_CONTAINERTYPE";
	map[MF_TRANSCODE_DONOT_INSERT_ENCODER] = "MF_TRANSCODE_DONOT_INSERT_ENCODER";
	map[MF_TRANSCODE_ENCODINGPROFILE] = "MF_TRANSCODE_ENCODINGPROFILE";
	map[MF_TRANSCODE_QUALITYVSSPEED] = "MF_TRANSCODE_QUALITYVSSPEED";
	map[MF_TRANSCODE_SKIP_METADATA_TRANSFER] = "MF_TRANSCODE_SKIP_METADATA_TRANSFER";
	map[MF_TRANSCODE_TOPOLOGYMODE] = "MF_TRANSCODE_TOPOLOGYMODE";
	map[MF_TRANSFORM_ASYNC] = "MF_TRANSFORM_ASYNC";
	map[MF_TRANSFORM_ASYNC_UNLOCK] = "MF_TRANSFORM_ASYNC_UNLOCK";
	map[MF_TRANSFORM_CATEGORY_Attribute] = "MF_TRANSFORM_CATEGORY_Attribute";
	map[MF_TRANSFORM_FLAGS_Attribute] = "MF_TRANSFORM_FLAGS_Attribute";

	map[MFASFSPLITTER_PACKET_BOUNDARY] = "MFASFSPLITTER_PACKET_BOUNDARY";

	map[MFSampleExtension_BottomFieldFirst] = "MFSampleExtension_BottomFieldFirst";
	map[MFSampleExtension_CleanPoint] = "MFSampleExtension_CleanPoint";

	map[MFSampleExtension_DerivedFromTopField] = "MFSampleExtension_DerivedFromTopField";
	map[MFSampleExtension_DeviceTimestamp] = "MFSampleExtension_DeviceTimestamp";
	map[MFSampleExtension_Discontinuity] = "MFSampleExtension_Discontinuity";

	map[MFSampleExtension_Interlaced] = "MFSampleExtension_Interlaced";
	map[MFSampleExtension_PacketCrossOffsets] = "MFSampleExtension_PacketCrossOffsets";
	map[MFSampleExtension_RepeatFirstField] = "MFSampleExtension_RepeatFirstField";
	map[MFSampleExtension_SingleField] = "MFSampleExtension_SingleField";
	map[MFSampleExtension_Token] = "MFSampleExtension_Token";

	map[MFT_CODEC_MERIT_Attribute] = "MFT_CODEC_MERIT_Attribute";
	map[MFT_CONNECTED_STREAM_ATTRIBUTE] = "MFT_CONNECTED_STREAM_ATTRIBUTE";
	map[MFT_CONNECTED_TO_HW_STREAM] = "MFT_CONNECTED_TO_HW_STREAM";

	map[MFT_ENUM_HARDWARE_URL_Attribute] = "MFT_ENUM_HARDWARE_URL_Attribute";

	map[MFT_ENUM_TRANSCODE_ONLY_ATTRIBUTE] = "MFT_ENUM_TRANSCODE_ONLY_ATTRIBUTE";
	map[MFT_FIELDOFUSE_UNLOCK_Attribute] = "MFT_FIELDOFUSE_UNLOCK_Attribute";
	map[MFT_FRIENDLY_NAME_Attribute] = "MFT_FRIENDLY_NAME_Attribute";
	map[MFT_HW_TIMESTAMP_WITH_QPC_Attribute] = "MFT_HW_TIMESTAMP_WITH_QPC_Attribute";
	map[MFT_INPUT_TYPES_Attributes] = "MFT_INPUT_TYPES_Attributes";
	map[MFT_OUTPUT_TYPES_Attributes] = "MFT_OUTPUT_TYPES_Attributes";
	map[MFT_PREFERRED_ENCODER_PROFILE] = "MFT_PREFERRED_ENCODER_PROFILE";
	map[MFT_PREFERRED_OUTPUTTYPE_Attribute] = "MFT_PREFERRED_OUTPUTTYPE_Attribute";
	map[MFT_PROCESS_LOCAL_Attribute] = "MFT_PROCESS_LOCAL_Attribute";

	map[MFT_SUPPORT_DYNAMIC_FORMAT_CHANGE] = "MFT_SUPPORT_DYNAMIC_FORMAT_CHANGE";
	map[MFT_TRANSFORM_CLSID_Attribute] = "MFT_TRANSFORM_CLSID_Attribute";
	map[VIDEO_ZOOM_RECT] = "VIDEO_ZOOM_RECT";

#if defined(OCEAN_COMPILER_MSC) && OCEAN_MSC_VERSION >= 1700

	map[MF_AUDIO_RENDERER_ATTRIBUTE_STREAM_CATEGORY] = "MF_AUDIO_RENDERER_ATTRIBUTE_STREAM_CATEGORY";

	map[MF_BYTESTREAM_EFFECTIVE_URL] = "MF_BYTESTREAM_EFFECTIVE_URL";

	map[MF_CAPTURE_ENGINE_D3D_MANAGER] = "MF_CAPTURE_ENGINE_D3D_MANAGER";
	map[MF_CAPTURE_ENGINE_DECODER_MFT_FIELDOFUSE_UNLOCK_Attribute] = "MF_CAPTURE_ENGINE_DECODER_MFT_FIELDOFUSE_UNLOCK_Attribute";
	map[MF_CAPTURE_ENGINE_DISABLE_DXVA] = "MF_CAPTURE_ENGINE_DISABLE_DXVA";
	map[MF_CAPTURE_ENGINE_DISABLE_HARDWARE_TRANSFORMS] = "MF_CAPTURE_ENGINE_DISABLE_HARDWARE_TRANSFORMS";
	map[MF_CAPTURE_ENGINE_ENCODER_MFT_FIELDOFUSE_UNLOCK_Attribute] = "MF_CAPTURE_ENGINE_ENCODER_MFT_FIELDOFUSE_UNLOCK_Attribute";
	map[MF_CAPTURE_ENGINE_EVENT_GENERATOR_GUID] = "MF_CAPTURE_ENGINE_EVENT_GENERATOR_GUID";
	map[MF_CAPTURE_ENGINE_EVENT_STREAM_INDEX] = "MF_CAPTURE_ENGINE_EVENT_STREAM_INDEX";
	map[MF_CAPTURE_ENGINE_MEDIASOURCE_CONFIG] = "MF_CAPTURE_ENGINE_MEDIASOURCE_CONFIG";
	map[MF_CAPTURE_ENGINE_RECORD_SINK_AUDIO_MAX_PROCESSED_SAMPLES] = "MF_CAPTURE_ENGINE_RECORD_SINK_AUDIO_MAX_PROCESSED_SAMPLES";
	map[MF_CAPTURE_ENGINE_RECORD_SINK_AUDIO_MAX_UNPROCESSED_SAMPLES] = "MF_CAPTURE_ENGINE_RECORD_SINK_AUDIO_MAX_UNPROCESSED_SAMPLES";
	map[MF_CAPTURE_ENGINE_RECORD_SINK_VIDEO_MAX_PROCESSED_SAMPLES] = "MF_CAPTURE_ENGINE_RECORD_SINK_VIDEO_MAX_PROCESSED_SAMPLES";
	map[MF_CAPTURE_ENGINE_RECORD_SINK_VIDEO_MAX_UNPROCESSED_SAMPLES] = "MF_CAPTURE_ENGINE_RECORD_SINK_VIDEO_MAX_UNPROCESSED_SAMPLES";
	map[MF_CAPTURE_ENGINE_USE_AUDIO_DEVICE_ONLY] = "MF_CAPTURE_ENGINE_USE_AUDIO_DEVICE_ONLY";
	map[MF_CAPTURE_ENGINE_USE_VIDEO_DEVICE_ONLY] = "MF_CAPTURE_ENGINE_USE_VIDEO_DEVICE_ONLY";
	map[MF_DEVICESTREAM_EXTENSION_PLUGIN_CLSID] = "MF_DEVICESTREAM_EXTENSION_PLUGIN_CLSID";
	map[MF_DEVICESTREAM_EXTENSION_PLUGIN_CONNECTION_POINT] = "MF_DEVICESTREAM_EXTENSION_PLUGIN_CONNECTION_POINT";
	map[MF_DEVICESTREAM_IMAGE_STREAM] = "MF_DEVICESTREAM_IMAGE_STREAM";
	map[MF_DEVICESTREAM_INDEPENDENT_IMAGE_STREAM] = "MF_DEVICESTREAM_INDEPENDENT_IMAGE_STREAM";
	map[MF_DEVICESTREAM_MAX_FRAME_BUFFERS] = "MF_DEVICESTREAM_MAX_FRAME_BUFFERS";
	map[MF_DEVICESTREAM_STREAM_CATEGORY] = "MF_DEVICESTREAM_STREAM_CATEGORY";
	map[MF_DEVICESTREAM_STREAM_ID] = "MF_DEVICESTREAM_STREAM_ID";
	map[MF_DEVICESTREAM_TAKEPHOTO_TRIGGER] = "MF_DEVICESTREAM_TAKEPHOTO_TRIGGER";
	map[MF_DEVICESTREAM_TRANSFORM_STREAM_ID] = "MF_DEVICESTREAM_TRANSFORM_STREAM_ID";

	map[MF_DISABLE_LOCALLY_REGISTERED_PLUGINS] = "MF_DISABLE_LOCALLY_REGISTERED_PLUGINS";
	map[MF_ENABLE_3DVIDEO_OUTPUT] = "MF_ENABLE_3DVIDEO_OUTPUT";

	map[MF_LOCAL_PLUGIN_CONTROL_POLICY] = "MF_LOCAL_PLUGIN_CONTROL_POLICY";
	map[MF_LOW_LATENCY] = "MF_LOW_LATENCY";
	map[MF_MEDIA_ENGINE_AUDIO_CATEGORY] = "MF_MEDIA_ENGINE_AUDIO_CATEGORY";
	map[MF_MEDIA_ENGINE_AUDIO_ENDPOINT_ROLE] = "MF_MEDIA_ENGINE_AUDIO_ENDPOINT_ROLE";
	map[MF_MEDIA_ENGINE_BROWSER_COMPATIBILITY_MODE] = "MF_MEDIA_ENGINE_BROWSER_COMPATIBILITY_MODE";
	map[MF_MEDIA_ENGINE_CALLBACK] = "MF_MEDIA_ENGINE_CALLBACK";
	map[MF_MEDIA_ENGINE_CONTENT_PROTECTION_FLAGS] = "MF_MEDIA_ENGINE_CONTENT_PROTECTION_FLAGS";
	map[MF_MEDIA_ENGINE_CONTENT_PROTECTION_MANAGER] = "MF_MEDIA_ENGINE_CONTENT_PROTECTION_MANAGER";
	map[MF_MEDIA_ENGINE_COREWINDOW] = "MF_MEDIA_ENGINE_COREWINDOW";
	map[MF_MEDIA_ENGINE_DXGI_MANAGER] = "MF_MEDIA_ENGINE_DXGI_MANAGER";
	map[MF_MEDIA_ENGINE_EXTENSION] = "MF_MEDIA_ENGINE_EXTENSION";
	map[MF_MEDIA_ENGINE_OPM_HWND] = "MF_MEDIA_ENGINE_OPM_HWND";
	map[MF_MEDIA_ENGINE_PLAYBACK_HWND] = "MF_MEDIA_ENGINE_PLAYBACK_HWND";
	map[MF_MEDIA_ENGINE_PLAYBACK_VISUAL] = "MF_MEDIA_ENGINE_PLAYBACK_VISUAL";
	map[MF_MEDIA_ENGINE_SOURCE_RESOLVER_CONFIG_STORE] = "MF_MEDIA_ENGINE_SOURCE_RESOLVER_CONFIG_STORE";
	map[MF_MEDIA_ENGINE_STREAM_CONTAINS_ALPHA_CHANNEL] = "MF_MEDIA_ENGINE_STREAM_CONTAINS_ALPHA_CHANNEL";
	map[MF_MEDIA_ENGINE_VIDEO_OUTPUT_FORMAT] = "MF_MEDIA_ENGINE_VIDEO_OUTPUT_FORMAT";

	map[MF_MPEG4SINK_MOOV_BEFORE_MDAT] = "MF_MPEG4SINK_MOOV_BEFORE_MDAT";
	map[MF_MPEG4SINK_SPSPPS_PASSTHROUGH] = "MF_MPEG4SINK_SPSPPS_PASSTHROUGH";

	map[MF_MT_H264_CAPABILITIES] = "MF_MT_H264_CAPABILITIES";
	map[MF_MT_H264_MAX_CODEC_CONFIG_DELAY] = "MF_MT_H264_MAX_CODEC_CONFIG_DELAY";
	map[MF_MT_H264_MAX_MB_PER_SEC] = "MF_MT_H264_MAX_MB_PER_SEC";
	map[MF_MT_H264_RATE_CONTROL_MODES] = "MF_MT_H264_RATE_CONTROL_MODES";
	map[MF_MT_H264_SIMULCAST_SUPPORT] = "MF_MT_H264_SIMULCAST_SUPPORT";
	map[MF_MT_H264_SUPPORTED_RATE_CONTROL_MODES] = "MF_MT_H264_SUPPORTED_RATE_CONTROL_MODES";
	map[MF_MT_H264_SUPPORTED_SLICE_MODES] = "MF_MT_H264_SUPPORTED_SLICE_MODES";
	map[MF_MT_H264_SUPPORTED_SYNC_FRAME_TYPES] = "MF_MT_H264_SUPPORTED_SYNC_FRAME_TYPES";
	map[MF_MT_H264_SUPPORTED_USAGES] = "MF_MT_H264_SUPPORTED_USAGES";
	map[MF_MT_H264_SVC_CAPABILITIES] = "MF_MT_H264_SVC_CAPABILITIES";
	map[MF_MT_H264_USAGE] = "MF_MT_H264_USAGE";

	map[MF_MT_MPEG2_CONTENT_PACKET] = "MF_MT_MPEG2_CONTENT_PACKET";

	map[MF_MT_MPEG2_STANDARD] = "MF_MT_MPEG2_STANDARD";
	map[MF_MT_MPEG2_TIMECODE] = "MF_MT_MPEG2_TIMECODE";

	map[MF_MT_TIMESTAMP_CAN_BE_DTS] = "MF_MT_TIMESTAMP_CAN_BE_DTS";

	map[MF_MT_VIDEO_3D] = "MF_MT_VIDEO_3D";
	map[MF_MT_VIDEO_3D_FIRST_IS_LEFT] = "MF_MT_VIDEO_3D_FIRST_IS_LEFT";
	map[MF_MT_VIDEO_3D_FORMAT] = "MF_MT_VIDEO_3D_FORMAT";
	map[MF_MT_VIDEO_3D_LEFT_IS_BASE] = "MF_MT_VIDEO_3D_LEFT_IS_BASE";
	map[MF_MT_VIDEO_3D_NUM_VIEWS] = "MF_MT_VIDEO_3D_NUM_VIEWS";

	map[MF_MT_VIDEO_ROTATION] = "MF_MT_VIDEO_ROTATION";

	map[MF_NALU_LENGTH_INFORMATION] = "MF_NALU_LENGTH_INFORMATION";
	map[MF_NALU_LENGTH_SET] = "MF_NALU_LENGTH_SET";

	map[MF_READWRITE_D3D_OPTIONAL] = "MF_READWRITE_D3D_OPTIONAL";

	map[MF_READWRITE_MMCSS_CLASS] = "MF_READWRITE_MMCSS_CLASS";
	map[MF_READWRITE_MMCSS_CLASS_AUDIO] = "MF_READWRITE_MMCSS_CLASS_AUDIO";
	map[MF_READWRITE_MMCSS_PRIORITY] = "MF_READWRITE_MMCSS_PRIORITY";
	map[MF_READWRITE_MMCSS_PRIORITY_AUDIO] = "MF_READWRITE_MMCSS_PRIORITY_AUDIO";
	map[MF_SA_BUFFERS_PER_SAMPLE] = "MF_SA_BUFFERS_PER_SAMPLE";

	map[MF_SA_D3D11_AWARE] = "MF_SA_D3D11_AWARE";
	map[MF_SA_D3D11_BINDFLAGS] = "MF_SA_D3D11_BINDFLAGS";
	map[MF_SA_D3D11_SHARED] = "MF_SA_D3D11_SHARED";
	map[MF_SA_D3D11_SHARED_WITHOUT_MUTEX] = "MF_SA_D3D11_SHARED_WITHOUT_MUTEX";
	map[MF_SA_D3D11_USAGE] = "MF_SA_D3D11_USAGE";
	map[MF_SA_MINIMUM_OUTPUT_SAMPLE_COUNT] = "MF_SA_MINIMUM_OUTPUT_SAMPLE_COUNT";
	map[MF_SA_MINIMUM_OUTPUT_SAMPLE_COUNT_PROGRESSIVE] = "MF_SA_MINIMUM_OUTPUT_SAMPLE_COUNT_PROGRESSIVE";

	map[MF_SA_REQUIRED_SAMPLE_COUNT_PROGRESSIVE] = "MF_SA_REQUIRED_SAMPLE_COUNT_PROGRESSIVE";

	map[MF_SINK_WRITER_D3D_MANAGER] = "MF_SINK_WRITER_D3D_MANAGER";

	map[MF_SINK_WRITER_ENCODER_CONFIG] = "MF_SINK_WRITER_ENCODER_CONFIG";

	map[MF_SOURCE_READER_DISABLE_CAMERA_PLUGINS] = "MF_SOURCE_READER_DISABLE_CAMERA_PLUGINS";

	map[MF_SOURCE_READER_ENABLE_ADVANCED_VIDEO_PROCESSING] = "MF_SOURCE_READER_ENABLE_ADVANCED_VIDEO_PROCESSING";
	map[MF_SOURCE_READER_ENABLE_TRANSCODE_ONLY_TRANSFORMS] = "MF_SOURCE_READER_ENABLE_TRANSCODE_ONLY_TRANSFORMS";

	map[MF_SOURCE_STREAM_SUPPORTS_HW_CONNECTION] = "MF_SOURCE_STREAM_SUPPORTS_HW_CONNECTION";
	map[MF_STREAM_SINK_SUPPORTS_HW_CONNECTION] = "MF_STREAM_SINK_SUPPORTS_HW_CONNECTION";
	map[MF_STREAM_SINK_SUPPORTS_ROTATION] = "MF_STREAM_SINK_SUPPORTS_ROTATION";

	map[MF_TOPONODE_WORKQUEUE_ITEM_PRIORITY] = "MF_TOPONODE_WORKQUEUE_ITEM_PRIORITY";

	map[MF_TOPONODE_WORKQUEUE_MMCSS_PRIORITY] = "MF_TOPONODE_WORKQUEUE_MMCSS_PRIORITY";

	map[MF_USER_DATA_PAYLOAD] = "MF_USER_DATA_PAYLOAD";
	map[MF_VIDEODSP_MODE] = "MF_VIDEODSP_MODE";
	map[MF_XVP_DISABLE_FRC] = "MF_XVP_DISABLE_FRC";

	map[MFPROTECTION_GRAPHICS_TRANSFER_AES_ENCRYPTION] = "MFPROTECTION_GRAPHICS_TRANSFER_AES_ENCRYPTION";
	map[MFPROTECTION_VIDEO_FRAMES] = "MFPROTECTION_VIDEO_FRAMES";
	map[MFPROTECTIONATTRIBUTE_BEST_EFFORT] = "MFPROTECTIONATTRIBUTE_BEST_EFFORT";
	map[MFPROTECTIONATTRIBUTE_FAIL_OVER] = "MFPROTECTIONATTRIBUTE_FAIL_OVER";
	map[MFSampleExtension_3DVideo] = "MFSampleExtension_3DVideo";
	map[MFSampleExtension_3DVideo_SampleFormat] = "MFSampleExtension_3DVideo_SampleFormat";

	map[MFSampleExtension_DecodeTimestamp] = "MFSampleExtension_DecodeTimestamp";

	map[MFSampleExtension_FrameCorruption] = "MFSampleExtension_FrameCorruption";

	map[MFSampleExtension_VideoDSPMode] = "MFSampleExtension_VideoDSPMode";
	map[MFSampleExtension_VideoEncodePictureType] = "MFSampleExtension_VideoEncodePictureType";
	map[MFSampleExtension_VideoEncodeQP] = "MFSampleExtension_VideoEncodeQP";

	map[MFT_DECODER_EXPOSE_OUTPUT_TYPES_IN_NATIVE_ORDER] = "MFT_DECODER_EXPOSE_OUTPUT_TYPES_IN_NATIVE_ORDER";
	map[MFT_DECODER_FINAL_VIDEO_RESOLUTION_HINT] = "MFT_DECODER_FINAL_VIDEO_RESOLUTION_HINT";

	map[MFT_ENUM_HARDWARE_VENDOR_ID_Attribute] = "MFT_ENUM_HARDWARE_VENDOR_ID_Attribute";

	map[MFT_SUPPORT_3DVIDEO] = "MFT_SUPPORT_3DVIDEO";

#endif // OCEAN_COMPILER_MSC

	return map;
}

Utilities::IdMap Utilities::majorMediaTypeMap()
{
	IdMap map;

	map[MFMediaType_Default] = "MFMediaType_Default";
	map[MFMediaType_Audio] = "MFMediaType_Audio";
	map[MFMediaType_Video] = "MFMediaType_Video";
	map[MFMediaType_Protected] = "MFMediaType_Protected";
	map[MFMediaType_SAMI] = "MFMediaType_SAMI";
	map[MFMediaType_Script] = "MFMediaType_Script";
	map[MFMediaType_Image] = "MFMediaType_Image";
	map[MFMediaType_HTML] = "MFMediaType_HTML";
	map[MFMediaType_Binary] = "MFMediaType_Binary";
	map[MFMediaType_FileTransfer] = "MFMediaType_FileTransfer";

#if defined(OCEAN_COMPILER_MSC) && OCEAN_MSC_VERSION >= 1700
	map[MFMediaType_Stream] = "MFMediaType_Stream";
#endif // OCEAN_COMPILER_MSC

	return map;
}

Utilities::IdMap Utilities::videoSubtypeMap()
{
	IdMap map;

	// uncompressed formats
	map[MFVideoFormat_RGB8] = "MFVideoFormat_RGB8";
	map[MFVideoFormat_RGB555] = "MFVideoFormat_RGB555";
	map[MFVideoFormat_RGB565] = "MFVideoFormat_RGB565";
	map[MFVideoFormat_RGB24] = "MFVideoFormat_RGB24";
	map[MFVideoFormat_RGB32] = "MFVideoFormat_RGB32";
	map[MFVideoFormat_ARGB32] = "MFVideoFormat_ARGB32";

	// yuv formats 8 bit
	map[MFVideoFormat_AI44] = "MFVideoFormat_AI44";
	map[MFVideoFormat_AYUV] = "MFVideoFormat_AYUV";
	map[MFVideoFormat_I420 ] = "MFVideoFormat_I420 ";
	map[MFVideoFormat_IYUV] = "MFVideoFormat_IYUV";
	map[MFVideoFormat_NV11] = "MFVideoFormat_NV11";
	map[MFVideoFormat_NV12] = "MFVideoFormat_NV12";
	map[MFVideoFormat_UYVY] = "MFVideoFormat_UYVY";
	map[MFVideoFormat_Y41P] = "MFVideoFormat_Y41P";
	map[MFVideoFormat_Y41T] = "MFVideoFormat_Y41T";
	map[MFVideoFormat_Y42T] = "MFVideoFormat_Y42T";
	map[MFVideoFormat_YUY2] = "MFVideoFormat_YUY2";
	map[MFVideoFormat_YV12] = "MFVideoFormat_YV12";

	// yuv formats 10-16 bit
	map[MFVideoFormat_P010] = "MFVideoFormat_P010";
	map[MFVideoFormat_P016] = "MFVideoFormat_P016";
	map[MFVideoFormat_P210] = "MFVideoFormat_P210";
	map[MFVideoFormat_P216] = "MFVideoFormat_P216";
	map[MFVideoFormat_v210] = "MFVideoFormat_v210";
	map[MFVideoFormat_v216] = "MFVideoFormat_v216";
	map[MFVideoFormat_v410] = "MFVideoFormat_v410";
	map[MFVideoFormat_Y210] = "MFVideoFormat_Y210";
	map[MFVideoFormat_Y216] = "MFVideoFormat_Y216";
	map[MFVideoFormat_Y410] = "MFVideoFormat_Y410";
	map[MFVideoFormat_Y416] = "MFVideoFormat_Y416";

	// encoded video types
	map[MFVideoFormat_DV25] = "MFVideoFormat_DV25";
	map[MFVideoFormat_DV50] = "MFVideoFormat_DV50";
	map[MFVideoFormat_DVC] = "MFVideoFormat_DVC";
	map[MFVideoFormat_DVH1] = "MFVideoFormat_DVH1";
	map[MFVideoFormat_DVHD] = "MFVideoFormat_DVHD";
	map[MFVideoFormat_DVSD] = "MFVideoFormat_DVSD";
	map[MFVideoFormat_DVSL] = "MFVideoFormat_DVSL";

	map[MFVideoFormat_H264] = "MFVideoFormat_H264";

	map[MFVideoFormat_M4S2] = "MFVideoFormat_M4S2";
	map[MFVideoFormat_MJPG] = "MFVideoFormat_MJPG";
	map[MFVideoFormat_MP43] = "MFVideoFormat_MP43";
	map[MFVideoFormat_MP4S] = "MFVideoFormat_MP4S";
	map[MFVideoFormat_MP4V] = "MFVideoFormat_MP4V";
	map[MFVideoFormat_MPEG2] = "MFVideoFormat_MPEG2";
	map[MFVideoFormat_MPG1] = "MFVideoFormat_MPG1";
	map[MFVideoFormat_MSS1] = "MFVideoFormat_MSS1";
	map[MFVideoFormat_MSS2] = "MFVideoFormat_MSS2";
	map[MFVideoFormat_WMV1] = "MFVideoFormat_WMV1";
	map[MFVideoFormat_WMV2] = "MFVideoFormat_WMV2";
	map[MFVideoFormat_WMV3	] = "MFVideoFormat_WMV3	";
	map[MFVideoFormat_WVC1] = "MFVideoFormat_WVC1";

#if defined(OCEAN_COMPILER_MSC) && OCEAN_MSC_VERSION >= 1700

	map[MFVideoFormat_H263] = "MFVideoFormat_H263";

	map[MFVideoFormat_H264_ES] = "MFVideoFormat_H264_ES";

#endif // OCEAN_COMPILER_MSC

	return map;
}

Utilities::IdMap Utilities::audioSubtypeMap()
{
	IdMap map;

	map[MEDIASUBTYPE_RAW_AAC1] = "MEDIASUBTYPE_RAW_AAC1";
	map[MFAudioFormat_AAC] = "MFAudioFormat_AAC";
	map[MFAudioFormat_ADTS] = "MFAudioFormat_ADTS";

	map[MFAudioFormat_Dolby_AC3_SPDIF] = "MFAudioFormat_Dolby_AC3_SPDIF";

	map[MFAudioFormat_DRM] = "MFAudioFormat_DRM";
	map[MFAudioFormat_DTS] = "MFAudioFormat_DTS";
	map[MFAudioFormat_Float] = "MFAudioFormat_Float";
	map[MFAudioFormat_MP3] = "MFAudioFormat_MP3";
	map[MFAudioFormat_MPEG] = "MFAudioFormat_MPEG";
	map[MFAudioFormat_MSP1] = "MFAudioFormat_MSP1";
	map[MFAudioFormat_PCM] = "MFAudioFormat_PCM";
	map[MFAudioFormat_WMASPDIF] = "MFAudioFormat_WMASPDIF";
	map[MFAudioFormat_WMAudio_Lossless] = "MFAudioFormat_WMAudio_Lossless";
	map[MFAudioFormat_WMAudioV8] = "MFAudioFormat_WMAudioV8";
	map[MFAudioFormat_WMAudioV9] = "MFAudioFormat_WMAudioV9";

#if defined(OCEAN_COMPILER_MSC) && OCEAN_MSC_VERSION >= 1700

	map[MFAudioFormat_Dolby_AC3] = "MFAudioFormat_Dolby_AC3";

	map[MFAudioFormat_Dolby_DDPlus] = "MFAudioFormat_Dolby_DDPlus";

#endif // OCEAN_COMPILER_MSC

	return map;
}

Utilities::IdMap Utilities::transformCategoryMap()
{
	IdMap map;

	map[MFT_CATEGORY_AUDIO_DECODER] = "MFT_CATEGORY_AUDIO_DECODER";
	map[MFT_CATEGORY_AUDIO_EFFECT] = "MFT_CATEGORY_AUDIO_EFFECT";
	map[MFT_CATEGORY_AUDIO_ENCODER] = "MFT_CATEGORY_AUDIO_ENCODER";
	map[MFT_CATEGORY_DEMULTIPLEXER] = "MFT_CATEGORY_DEMULTIPLEXER";
	map[MFT_CATEGORY_MULTIPLEXER] = "MFT_CATEGORY_MULTIPLEXER";
	map[MFT_CATEGORY_OTHER] = "MFT_CATEGORY_OTHER";
	map[MFT_CATEGORY_VIDEO_DECODER] = "MFT_CATEGORY_VIDEO_DECODER";
	map[MFT_CATEGORY_VIDEO_EFFECT] = "MFT_CATEGORY_VIDEO_EFFECT";
	map[MFT_CATEGORY_VIDEO_ENCODER] = "MFT_CATEGORY_VIDEO_ENCODER";
	map[MFT_CATEGORY_VIDEO_PROCESSOR] = "MFT_CATEGORY_VIDEO_PROCESSOR";

	return map;
}

}

}

}
