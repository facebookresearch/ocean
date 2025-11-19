/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/mediafoundation/MFLiveVideo.h"
#include "ocean/media/mediafoundation/MFLibrary.h"
#include "ocean/media/mediafoundation/Utilities.h"

#include <ks.h>
#include <ksmedia.h>
#include <ksproxy.h>

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

double MFLiveVideo::exposureDuration(double* minDuration, double* maxDuration, ControlMode* exposureMode) const
{
	const ScopedLock scopedLock(lock_);

	ScopedIKsControl iKsControl;
	if (S_OK != mediaSource_->QueryInterface(IID_IKsControl, (void**)(&iKsControl.resetObject())))
	{
		return false;
	}

	if (minDuration != nullptr || maxDuration != nullptr)
	{
		double minExposure = NumericD::minValue();
		double maxExposure = NumericD::minValue();

		const bool result = exposureRange(*iKsControl, minExposure, maxExposure);
		ocean_assert_and_suppress_unused(result, result);

		if (minDuration != nullptr)
		{
			*minDuration = minExposure;
		}

		if (maxDuration != nullptr)
		{
			*maxDuration = maxExposure;
		}
	}

	double duration = Numeric::minValue();

	ControlMode controlMode = CM_INVALID;
	if (!exposure(*iKsControl, duration, controlMode))
	{
		return -1.0;
	}

	if (exposureMode != nullptr)
	{
		*exposureMode = controlMode;
	}

	return duration;
}

bool MFLiveVideo::setExposureDuration(const double duration, const bool /*allowShorterExposure*/)
{
	if (duration < 0.0)
	{
		Log::warning() << "MFLiveVideo::setExposureDuration() does not support one-time auto exposure";

		return false;
	}

	const ScopedLock scopedLock(lock_);

	if (!sessionStarted_)
	{
		delayedExposureDuration_ = duration;

		return true;
	}

	ScopedIKsControl iKsControl;
	if (S_OK != mediaSource_->QueryInterface(IID_IKsControl, (void**)(&iKsControl.resetObject())))
	{
		return false;
	}

	return setExposure(*iKsControl, duration);
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

void MFLiveVideo::onSessionStarted()
{
	const ScopedLock scopedLock(lock_);

	MFFrameMedium::onSessionStarted();

	sessionStarted_ = true;

	if (delayedExposureDuration_ != NumericD::minValue())
	{
		setExposureDuration(delayedExposureDuration_);

		delayedExposureDuration_ = NumericD::minValue();
	}
}

void MFLiveVideo::onSessionStopped()
{
	const ScopedLock scopedLock(lock_);

	MFFrameMedium::onSessionStopped();

	sessionStarted_ = false;
}

bool MFLiveVideo::exposureRange(IKsControl* iKsControl, double& minExposure, double& maxExposure)
{
	// User-mode clients use the KSPROPERTY_CAMERACONTROL_EXPOSURE property to get or set a digital camera's exposure time. This property is optional.

	KSPROPERTY ksProperty = {};
	ksProperty.Set = PROPSETID_VIDCAP_CAMERACONTROL;
	ksProperty.Id = KSPROPERTY_CAMERACONTROL_EXPOSURE;
	ksProperty.Flags = KSPROPERTY_TYPE_BASICSUPPORT;

	KSPROPERTY_DESCRIPTION ksPropertyDescription = {};
	ULONG bytesReturned = 0;
	if (S_OK != iKsControl->KsProperty(&ksProperty, sizeof(ksProperty), &ksPropertyDescription, sizeof(ksPropertyDescription), &bytesReturned))
	{
		return false;
	}

	if (bytesReturned < sizeof(KSPROPERTY_DESCRIPTION))
	{
		return false;
	}

	constexpr size_t expectedSize = sizeof(KSPROPERTY_DESCRIPTION) + sizeof(KSPROPERTY_MEMBERSHEADER) + sizeof(KSPROPERTY_STEPPING_LONG);

	if (ksPropertyDescription.DescriptionSize < expectedSize)
	{
		return false;
	}

	std::vector<uint8_t> buffer(ksPropertyDescription.DescriptionSize);

	bytesReturned = 0;
	if (S_OK != iKsControl->KsProperty(&ksProperty, sizeof(ksProperty), buffer.data(), ULONG(buffer.size()), &bytesReturned))
	{
		return false;
	}

	const KSPROPERTY_MEMBERSHEADER* propertyMember = (const KSPROPERTY_MEMBERSHEADER*)(buffer.data() + sizeof(KSPROPERTY_DESCRIPTION));

	if (propertyMember->MembersFlags & KSPROPERTY_MEMBER_RANGES)
	{
		const KSPROPERTY_STEPPING_LONG* range = (const KSPROPERTY_STEPPING_LONG*)(buffer.data() + sizeof(KSPROPERTY_DESCRIPTION) + sizeof(KSPROPERTY_MEMBERSHEADER));

		const LONG minLogBase2 = range->Bounds.SignedMinimum;
		const LONG maxLogBase2 = range->Bounds.SignedMaximum;

		minExposure = translateExposure(minLogBase2);
		maxExposure = translateExposure(maxLogBase2);

		return true;
	}

	return false;
}

bool MFLiveVideo::setExposure(IKsControl* iKsControl, const double exposure)
{
	ocean_assert(iKsControl != nullptr);

	LONG exposureValue = 0;

	if (exposure > 0.0)
	{
		exposureValue = translateExposure(exposure);
	}

	KSPROPERTY_CAMERACONTROL_S cameraControl = {};

	cameraControl.Property.Set = PROPSETID_VIDCAP_CAMERACONTROL;
	cameraControl.Property.Id = KSPROPERTY_CAMERACONTROL_EXPOSURE;
	cameraControl.Property.Flags = KSPROPERTY_TYPE_SET;
	cameraControl.Value = exposureValue;
	cameraControl.Flags = exposure > 0.0 ? KSPROPERTY_CAMERACONTROL_FLAGS_MANUAL : KSPROPERTY_CAMERACONTROL_FLAGS_AUTO;
	cameraControl.Capabilities = KSPROPERTY_CAMERACONTROL_FLAGS_ABSOLUTE;

	ULONG bytesReturned = 0;

    const HRESULT result = iKsControl->KsProperty(&cameraControl.Property, sizeof(cameraControl), &cameraControl, sizeof(cameraControl), &bytesReturned);

	return result == S_OK;
}

bool MFLiveVideo::exposure(IKsControl* iKsControl, double& exposure, ControlMode& controlMode)
{
	KSPROPERTY_CAMERACONTROL_S cameraControl = {};

	cameraControl.Property.Set = PROPSETID_VIDCAP_CAMERACONTROL;
	cameraControl.Property.Id = KSPROPERTY_CAMERACONTROL_EXPOSURE;
	cameraControl.Property.Flags = KSPROPERTY_TYPE_GET;
	cameraControl.Value = 0;
	cameraControl.Flags = 0;
	cameraControl.Capabilities = 0;

	ULONG bytesReturned = 0;

    if (S_OK != iKsControl->KsProperty(&cameraControl.Property, sizeof(cameraControl), &cameraControl, sizeof(cameraControl), &bytesReturned))
	{
		return false;
	}

	exposure = translateExposure(cameraControl.Value);

	if (cameraControl.Flags & KSPROPERTY_CAMERACONTROL_FLAGS_AUTO)
	{
		controlMode = CM_DYNAMIC;
	}
	else if (cameraControl.Flags & KSPROPERTY_CAMERACONTROL_FLAGS_MANUAL)
	{
		controlMode = CM_FIXED;
	}
	else
	{
		controlMode = CM_INVALID;
	}

	return true;
}

double MFLiveVideo::translateExposure(const LONG logBase2)
{
	if (logBase2 >= 0)
	{
		return double(logBase2) * 2.0;
	}
	else
	{
		return 1.0 / NumericD::pow(2.0, double(-logBase2));
	}
}

LONG MFLiveVideo::translateExposure(const double exposure)
{
	ocean_assert(exposure > 0.0);

	if (exposure >= 1.0)
	{
		const double exposureValueD = exposure / 2.0;

		return NumericD::round32(exposureValueD);
	}
	else
	{
		const double exposureValueD = NumericD::log2(exposure);

		return NumericD::round32(exposureValueD);
	}
}

}

}

}
