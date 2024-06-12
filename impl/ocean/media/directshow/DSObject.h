/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_DS_OBJECT_H
#define META_OCEAN_MEDIA_DS_OBJECT_H

#include "ocean/media/directshow/DirectShow.h"

#include "ocean/base/ScopedObject.h"

DISABLE_WARNINGS_BEGIN
	#include <Dshow.h>
DISABLE_WARNINGS_END

namespace Ocean
{

namespace Media
{

namespace DirectShow
{

/**
 * This functions allows to release a DirectShow object if it does exist.
 * @param object DirectShow object to be released
 * @ingroup mediads
 */
template <class T> void release(T *object);

/**
 * Template specialization for ScopedObjectCompileTimeVoid using the release() function to release DirectShow objects.
 * @tparam T The data type of the wrapped object
 * @ingroup mediads
 */
template <typename T>
using ScopeDirectShowObject = ScopedObjectCompileTimeVoidT<T*, release>;

/**
 * Definition of a scoped object holding an IAMStreamConfig object.
 * The wrapped IAMStreamConfig object will be released automatically once the scoped object does not exist anymore.
 * @ingroup mediads
 */
typedef ScopeDirectShowObject<IAMStreamConfig> ScopedIAMStreamConfig;

/**
 * Definition of a scoped object holding an IAMVfwCompressDialogs object.
 * The wrapped IAMVfwCompressDialogs object will be released automatically once the scoped object does not exist anymore.
 * @ingroup mediads
 */
typedef ScopeDirectShowObject<IAMVfwCompressDialogs> ScopedIAMVfwCompressDialogs;

/**
 * Definition of a scoped object holding an IBaseFilter object.
 * The wrapped IBaseFilter object will be released automatically once the scoped object does not exist anymore.
 * @ingroup mediads
 */
typedef ScopeDirectShowObject<IBaseFilter> ScopedIBaseFilter;

/**
 * Definition of a scoped object holding an IBasicAudio object.
 * The wrapped IBasicAudio object will be released automatically once the scoped object does not exist anymore.
 * @ingroup mediads
 */
typedef ScopeDirectShowObject<IBasicAudio> ScopedIBasicAudio;

/**
 * Definition of a scoped object holding an ICreateDevEnum object.
 * The wrapped ICreateDevEnum object will be released automatically once the scoped object does not exist anymore.
 * @ingroup mediads
 */
typedef ScopeDirectShowObject<ICreateDevEnum> ScopedICreateDevEnum;

/**
 * Definition of a scoped object holding an IEnumFilters object.
 * The wrapped IEnumFilters object will be released automatically once the scoped object does not exist anymore.
 * @ingroup mediads
 */
typedef ScopeDirectShowObject<IEnumFilters> ScopedIEnumFilters;

/**
 * Definition of a scoped object holding an IEnumMoniker object.
 * The wrapped IEnumMoniker object will be released automatically once the scoped object does not exist anymore.
 * @ingroup mediads
 */
typedef ScopeDirectShowObject<IEnumMoniker> ScopedIEnumMoniker;

/**
 * Definition of a scoped object holding an IEnumPins object.
 * The wrapped IEnumPins object will be released automatically once the scoped object does not exist anymore.
 * @ingroup mediads
 */
typedef ScopeDirectShowObject<IEnumPins> ScopedIEnumPins;

/**
 * Definition of a scoped object holding an IFileSinkFilter object.
 * The wrapped IFileSinkFilter object will be released automatically once the scoped object does not exist anymore.
 * @ingroup mediads
 */
typedef ScopeDirectShowObject<IFileSinkFilter> ScopedIFileSinkFilter;

/**
 * Definition of a scoped object holding an IGraphBuilder object.
 * The wrapped IGraphBuilder object will be released automatically once the scoped object does not exist anymore.
 * @ingroup mediads
 */
typedef ScopeDirectShowObject<IGraphBuilder> ScopedIGraphBuilder;

/**
 * Definition of a scoped object holding an IMediaControl object.
 * The wrapped IMediaControl object will be released automatically once the scoped object does not exist anymore.
 * @ingroup mediads
 */
typedef ScopeDirectShowObject<IMediaControl> ScopedIMediaControl;

/**
 * Definition of a scoped object holding an IMediaEvent object.
 * The wrapped IMediaEvent object will be released automatically once the scoped object does not exist anymore.
 * @ingroup mediads
 */
typedef ScopeDirectShowObject<IMediaEvent> ScopedIMediaEvent;

/**
 * Definition of a scoped object holding an IMediaFilter object.
 * The wrapped IMediaFilter object will be released automatically once the scoped object does not exist anymore.
 * @ingroup mediads
 */
typedef ScopeDirectShowObject<IMediaFilter> ScopedIMediaFilter;

/**
 * Definition of a scoped object holding an IMediaSeeking object.
 * The wrapped IMediaSeeking object will be released automatically once the scoped object does not exist anymore.
 * @ingroup mediads
 */
typedef ScopeDirectShowObject<IMediaSeeking> ScopedIMediaSeeking;

/**
 * Definition of a scoped object holding an IMoniker object.
 * The wrapped IMoniker object will be released automatically once the scoped object does not exist anymore.
 * @ingroup mediads
 */
typedef ScopeDirectShowObject<IMoniker> ScopedIMoniker;

/**
 * Definition of a scoped object holding an IPin object.
 * The wrapped IPin object will be released automatically once the scoped object does not exist anymore.
 * @ingroup mediads
 */
typedef ScopeDirectShowObject<IPin> ScopedIPin;

/**
 * Definition of a scoped object holding an IPropertyBag object.
 * The wrapped IPropertyBag object will be released automatically once the scoped object does not exist anymore.
 * @ingroup mediads
 */
typedef ScopeDirectShowObject<IPropertyBag> ScopedIPropertyBag;

/**
 * Definition of a scoped object holding an IRunningObjectTable object.
 * The wrapped IRunningObjectTable object will be released automatically once the scoped object does not exist anymore.
 * @ingroup mediads
 */
typedef ScopeDirectShowObject<IRunningObjectTable> ScopedIRunningObjectTable;


/**
 * This class provides basic DirectShow functionalities.
 * @ingroup mediads
 */
class OCEAN_MEDIA_DS_EXPORT DSObject
{
	public:

		/**
		 * Definition of input and output pin.
		 */
		enum PinType
		{
			/// Input pin.
			PTYPE_INPUT,
			/// Output pin.
			PTYPE_OUTPUT,
			/// Don't care.
			PTYPE_DONT_CARE
		};

		/**
		 * Definition of pin connection types.
		 */
		enum ConnectionType
		{
			/// Connected.
			CTYPE_CONNECTED,
			/// Not connected.
			CTYPE_NOT_CONNECTED,
			/// Don't care.
			CTYPE_DONT_CARE
		};

		/**
		 * Definition of a pair of pins.
		 */
		typedef std::pair<ScopedIPin, ScopedIPin> PinPair;

		/**
		 * Definition of a vector holding pin pairs.
		 */
		typedef std::vector<PinPair> PinPairs;

		/**
		 * Definition of a vector holding pins.
		 */
		typedef std::vector<ScopedIPin> Pins;

	public:

		/**
		 * Returns the first pin.
		 * @param filter The filter to return the pin from
		 * @param pinType Input or output pin
		 * @param connectionType Connection type of the pin
		 * @return First free pin, if any
		 */
		static ScopedIPin firstPin(IBaseFilter* filter, PinType pinType, ConnectionType connectionType);

		/**
		 * Returns all pins with a specified property.
		 * The pins have to be released by the caller.
		 * @param filter The filter to return the pin from
		 * @param pinType Input or output pin
		 * @param connectionType Connection type of the pin
		 * @return Pins, if any
		 */
		static Pins pins(IBaseFilter* filter, PinType pinType, ConnectionType connectionType);

		/**
		 * Returns all connects in a branch.
		 * The pins have to be released by the caller.
		 * @param filter Start point of the branch
		 * @return Connections
		 */
		static PinPairs connections(IBaseFilter* filter);

		/**
		 * Returns the owner filter of a pin.
		 * The filter has to be released by the caller.
		 * @param pin The pin the get the owner from
		 * @return Filter, if any
		 */
		static ScopedIBaseFilter pinOwner(IPin* pin);

		/**
		 * Returns the connected pin of a given pin.
		 * The pin has to be released by the caller.
		 * @param pin The pin to get the connected pin from
		 * @return Connected pin, if any
		 */
		static ScopedIPin connectedPin(IPin* pin);
};

template <class T>
void release(T *object)
{
	if (object != nullptr)
	{
		object->Release();
	}
}

}

}

}

#endif // META_OCEAN_MEDIA_DS_OBJECT_H
