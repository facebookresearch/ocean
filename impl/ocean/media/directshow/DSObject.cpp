/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/directshow/DSObject.h"

DISABLE_WARNINGS_BEGIN
	#include <Streams.h>
DISABLE_WARNINGS_END

namespace Ocean
{

namespace Media
{

namespace DirectShow
{

ScopedIPin DSObject::firstPin(IBaseFilter* filter, PinType pinType, ConnectionType connectionType)
{
	if (filter == nullptr)
	{
		return ScopedIPin();
	}

	ScopedIEnumPins enumerator;

	if (S_OK != filter->EnumPins(&enumerator.resetObject()))
	{
		return ScopedIPin();
	}

	ScopedIPin pin;
	while (enumerator->Next(1, &pin.resetObject(), 0) == S_OK)
	{
		PIN_DIRECTION direction;
		if (pin->QueryDirection(&direction) == S_OK)
		{
			if (pinType == PTYPE_DONT_CARE || direction == PINDIR_OUTPUT && pinType == PTYPE_OUTPUT || direction == PINDIR_INPUT && pinType == PTYPE_INPUT)
			{
				if (connectionType == CTYPE_DONT_CARE)
				{
					break;
				}

				const ScopedIPin connected = connectedPin(*pin);

				if (connected.isValid())
				{
					if (connectionType == CTYPE_CONNECTED)
					{
						break;
					}
				}
				else if (connectionType == CTYPE_NOT_CONNECTED)
				{
					break;
				}
			}
		}

		pin.release();
	}

	return pin;
}

DSObject::Pins DSObject::pins(IBaseFilter* filter, PinType pinType, ConnectionType connectionType)
{
	Pins pins;

	if (filter == nullptr)
	{
		return pins;
	}

	ScopedIEnumPins enumerator;

	if (S_OK != filter->EnumPins(&enumerator.resetObject()))
	{
		return pins;
	}

	ScopedIPin pin;
	while (enumerator->Next(1, &pin.resetObject(), 0) == S_OK)
	{
		PIN_DIRECTION direction;
		if (pin->QueryDirection(&direction) == S_OK)
		{
			if (pinType == PTYPE_DONT_CARE || direction == PINDIR_OUTPUT && pinType == PTYPE_OUTPUT || direction == PINDIR_INPUT && pinType == PTYPE_INPUT)
			{
				if (connectionType == CTYPE_DONT_CARE)
				{
					pins.emplace_back(std::move(pin));
				}
				else
				{
					const ScopedIPin connected = connectedPin(*pin);

					if (connected.isValid())
					{
						if (connectionType == CTYPE_CONNECTED)
						{
							pins.emplace_back(std::move(pin));
						}
					}
					else if (connectionType == CTYPE_NOT_CONNECTED)
					{
						pins.emplace_back(std::move(pin));
					}
				}
			}
		}
	}

	return pins;
}

DSObject::PinPairs DSObject::connections(IBaseFilter* filter)
{
	PinPairs pairs;

	if (filter == nullptr)
	{
		return pairs;
	}

	Pins connectedPins = pins(filter, PTYPE_OUTPUT, CTYPE_CONNECTED);

	for (unsigned int n = 0; n < connectedPins.size(); n++)
	{
		ScopedIPin connected = connectedPin(*connectedPins[n]);
		ocean_assert(connected.isValid());

		ScopedIBaseFilter childFilter = pinOwner(*connected);
		ocean_assert(childFilter.isValid());

		pairs.emplace_back(std::move(connectedPins[n]), std::move(connected));

		PinPairs children(connections(*childFilter));

		pairs.insert(pairs.cend(), std::make_move_iterator(children.begin()), std::make_move_iterator(children.end()));
	}

	return pairs;
}

ScopedIBaseFilter DSObject::pinOwner(IPin* pin)
{
	if (pin == nullptr)
	{
		return ScopedIBaseFilter();
	}

	PIN_INFO info;
	if (S_OK != pin->QueryPinInfo(&info))
	{
		return ScopedIBaseFilter();
	}

	return ScopedIBaseFilter(info.pFilter);
}

ScopedIPin DSObject::connectedPin(IPin* pin)
{
	if (pin == nullptr)
	{
		return ScopedIPin();
	}

	ScopedIPin connected;
	if (VFW_E_NOT_CONNECTED == pin->ConnectedTo(&connected.resetObject()))
	{
		ocean_assert(!connected.isValid());
		return ScopedIPin();
	}

	return connected;
}

}

}

}
