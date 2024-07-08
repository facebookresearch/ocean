/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_MF_UTILITIES_H
#define META_OCEAN_MEDIA_MF_UTILITIES_H

#include "ocean/media/mediafoundation/MediaFoundation.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/base/Frame.h"

#include <map>

namespace Ocean
{

namespace Media
{

namespace MediaFoundation
{

/**
 * This class implements utilities functions for the Microsoft Media Foundation.
 * @ingroup mediamf
 */
class OCEAN_MEDIA_MF_EXPORT Utilities
{
	private:

		/**
		 * GUID helper struct.
		 */
		struct GUIDCompare
		{
			/**
			 * Compares two GUID objects.
			 * @param object0 First object
			 * @param object1 Second object
			 * @return True, if the first object is lesser than the second one
			 */
			inline bool operator()(const GUID& object0, const GUID& object1) const;
		};

		/**
		 * Definition of a map mapping GUID objects to strings.
		 */
		typedef std::map<GUID, std::string, GUIDCompare> IdMap;

	public:

		/**
		 * Translates a media foundation attribute to a readable string.
		 * @param attribute The attribute to be translated
		 * @return Translated attribute
		 */
		static std::string attribute2String(const GUID& attribute);

		/**
		 * Translates a major media type to a readable string.
		 * @param type The type to be translated
		 * @return Translated type
		 */
		static std::string majorMediaType2String(const GUID& type);

		/**
		 * Translates a video subtype to a readable string.
		 * @param type The type to be translated
		 * @return Translated type
		 */
		static std::string videoSubtype2String(const GUID& type);

		/**
		 * Translates an unregistered video subtype to a readable string.
		 * @param type The type to be translated
		 * @return Translated type
		 */
		static std::string unregisteredVideoSubtype2String(const GUID& type);

		/**
		 * Translates an audio subtype to a readable string.
		 * @param type The type to be translated
		 * @return Translated type
		 */
		static std::string audioSubtype2String(const GUID& type);

		/**
		 * Translates an transform category to a readable string.
		 * @param category The category to be translated
		 * @return Translated type
		 */
		static std::string transformCategory2String(const GUID& category);

		/**
		 * Translates the GUID key to a string.
		 * @param value The value to be translated
		 * @return Translated key
		 */
		static std::string guid2String(const GUID& value);

		/**
		 * Dumps a set of attributes to a string object.
		 * @param attributes The attributes to dump
		 * @param result Resulting dump
		 * @return True, if succeeded
		 */
		static bool dumpAttributes(IMFAttributes* attributes, std::string& result);

		/**
		 * Enumerates the registered media foundation transforms.
		 * @param result The resulting enumerated transforms
		 * @return True, if succeeded
		 */
		static bool enumerateTransforms(std::string& result);

		/**
		 * Converts a Media Foundation media subtype to a pixel format.
		 * @param mediaSubtype Media Foundation media type to convert
		 * @return Resulting pixel format
		 */
		static FrameType::PixelFormat convertMediaSubtype(const GUID& mediaSubtype);

		/**
		 * Returns the pixel origin of a Media Foundation media subtype.
		 * @param mediaSubtype Media Foundation media type to return the pixel origin for
		 * @return Pixel origin
		 */
		static FrameType::PixelOrigin extractPixelOrigin(const GUID& mediaSubtype);

		/**
		 * Converts a pixel format to a Media Foundation media subtype.
		 * @param pixelFormat Pixel format
		 * @return Media Foundation media type
		 */
		static GUID convertPixelFormat(const FrameType::PixelFormat pixelFormat);

		/**
		 * Creates the media source object for a given URL.
		 * The resulting object has to be released by the caller.<br>
		 * @param url URL for that the media source object will be returned
		 * @return Resulting media source object, invalid otherwise
		 */
		static ScopedIMFMediaSource createMediaSourceByUrl(const std::wstring& url);

		/**
		 * Adds a source node to a given topology.
		 * @param topology The topology to that the node will be added, must be valid
		 * @param mediaSource Media source object, must be valid
		 * @param presentationDescriptor Presentation descriptor, must be valid
		 * @param streamDescriptor Stream descriptor, must be valid
		 * @return Resulting topology node, invalid otherwise
		 */
		static ScopedIMFTopologyNode addSourceNodeToTopology(IMFTopology* topology, IMFMediaSource* mediaSource, IMFPresentationDescriptor* presentationDescriptor, IMFStreamDescriptor* streamDescriptor);

		/**
		 * Adds an output node to a given topology.
		 * @param topology The topology to that the node will be added, must be valid
		 * @param sinkActivate Sink activate object, must be valid
		 * @param streamIndex Stream index
		 * @return Resulting topology node, invalid otherwise
		 */
		static ScopedIMFTopologyNode addOutputNodeToTopology(IMFTopology* topology, IMFActivate* sinkActivate, const DWORD streamIndex = 0);

		/**
		 * Connects the selected media source with a given sink activate object.
		 * @param topology The topology that will receive the connection, must be valid
		 * @param mediaSource The media source object, must be valid
		 * @param sinkActivate The sink activate object, must be valid
		 * @param majorMediaType Major media type
		 * @return True, if succeeded
		 */
		static bool connectSelectedStream(IMFTopology* topology, IMFMediaSource* mediaSource, IMFActivate* sinkActivate, const GUID& majorMediaType);

		/**
		 * Returns an allocated string from an activate.
		 * @param activate The activate from which the string will be returned, must be valid
		 * @param key The key identifying which value to retrieve, must be valid
		 * @param value The resulting string value
		 * @reutrn True, if succeeded
		 */
		static bool getAllocatedString(IMFActivate* activate, const GUID& key, std::string& value);

		/**
		 * Extracts an object from a topology node.
		 * @param node The node from that the object will be extracted, must be valid
		 * @return Resulting object which must be released by the caller, otherwise nullptr
		 * @tparam T Data type of the desired object
		 */
		template <typename T>
		static ScopedMediaFoundationObject<T> topologyNodeObject(IMFTopologyNode* node);

		/**
		 * Extracts an object from an event.
		 * @param mediaEvent Media event from that the object will be extracted, must be valid
		 * @return Resulting object which must be released by the caller, otherwise nullptr
		 * @tparam T Data type of the desired object
		 */
		template <typename T>
		static ScopedMediaFoundationObject<T> eventObject(IMFMediaEvent *mediaEvent);

	private:

		/**
		 * Returns a map mapping all GUID ids to readable strings.
		 * @return GUID map
		 */
		static IdMap guidMap();

		/**
		 * Returns a map mapping attribute GUID ids to readable strings.
		 * @return GUID map
		 */
		static IdMap attributeMap();

		/**
		 * Returns a map mapping major media type GUID ids to readable strings.
		 * @return GUID map
		 */
		static IdMap majorMediaTypeMap();

		/**
		 * Returns a map mapping video subtype GUID ids to readable strings.
		 * @return GUID map
		 */
		static IdMap videoSubtypeMap();

		/**
		 * Returns a map mapping audio subtype GUID ids to readable strings.
		 * @return GUID map
		 */
		static IdMap audioSubtypeMap();

		/**
		 * Returns a map mapping transform category GUID ids to readable strings.
		 * @return GUID map
		 */
		static IdMap transformCategoryMap();
};

inline bool Utilities::GUIDCompare::operator()(const GUID& object0, const GUID& object1) const
{
	ocean_assert(sizeof(object0) == sizeof(uint64_t) * 2);

	const uint64_t* value0 = (const uint64_t*)(&object0);
	const uint64_t* value1 = (const uint64_t*)(&object1);

	return value0[0] < value1[0] || (value0[0] == value1[0] && value0[1] < value1[1]);
}

template <typename T>
ScopedMediaFoundationObject<T> Utilities::topologyNodeObject(IMFTopologyNode* node)
{
	ocean_assert(node);

	ScopedMediaFoundationObject<IUnknown> object;
	ScopedMediaFoundationObject<T> result;

	if (S_OK == node->GetObject(&object.resetObject()))
	{
		object->QueryInterface(IID_PPV_ARGS(&result.resetObject()));
	}

	return result;
}

template <typename T>
ScopedMediaFoundationObject<T> Utilities::eventObject(IMFMediaEvent *mediaEvent)
{
	ocean_assert(mediaEvent);

	PROPVARIANT variant;
	PropVariantInit(&variant);

	ScopedMediaFoundationObject<T> result;

	if (S_OK == mediaEvent->GetValue(&variant))
	{
		variant.punkVal->QueryInterface(__uuidof(T), (void**)(&result.resetObject()));
	}

	PropVariantClear(&variant);

	return result;
}

}

}

}

#endif // META_OCEAN_MEDIA_MF_UTILITIES_H
