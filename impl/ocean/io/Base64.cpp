/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/io/Base64.h"

namespace Ocean
{

namespace IO
{

void Base64::encode(const uint8_t* buffer, const size_t bufferSize, Buffer& encodedText)
{
	ocean_assert(encodedText.empty());
	encodedText.clear();

	ocean_assert(bufferSize != 0);
	if (bufferSize == 0)
	{
		return;
	}

	uint8_t buffer3[3];

	const size_t encodedTextLength = 4 * ((bufferSize + 2) / 3);
	encodedText.resize(encodedTextLength);

	unsigned int i3 = 0u;
	size_t encodedPosition = 0;

	for (size_t n = 0; n < bufferSize; ++n)
	{
		buffer3[i3++] = buffer[n];

		if (i3 == 3u)
		{
			encode3(buffer3, encodedText.data() + encodedPosition);
			encodedPosition += 4;
			i3 = 0u;
		}
	}

	if (i3 != 0u)
	{
		ocean_assert(i3 == 1u || i3 == 2u);

		if (i3 == 1u)
		{
			buffer3[1] = 0;
		}
		buffer3[2] = 0;

		encode3(buffer3, encodedText.data() + encodedPosition);

		if (i3 == 1u)
		{
			encodedText[encodedTextLength - 2] = '=';
		}
		encodedText[encodedTextLength - 1] = '=';
	}
}

bool Base64::decode(const uint8_t* encodedText, const size_t encodedTextSize, Buffer& buffer)
{
	ocean_assert(buffer.empty());
	buffer.clear();

	if (encodedTextSize == 0)
	{
		return true;
	}

	if (encodedTextSize % 4 != 0)
	{
		return false;
	}

	uint8_t encoded4[4];

	ocean_assert(encodedTextSize >= 4);
	const size_t endingCharacters = (encodedText[encodedTextSize - 1] == '=' ? 1 : 0) + (encodedText[encodedTextSize - 2] == '=' ? 1 : 0);
	ocean_assert(endingCharacters <= 2);

	const size_t bufferLength = 3 * (encodedTextSize / 4) - endingCharacters;
	buffer.resize(bufferLength);

	unsigned int i4 = 0u;
	size_t bufferPosition = 0;

	for (size_t n = 0; n < encodedTextSize - endingCharacters; ++n)
	{
		encoded4[i4++] = encodedText[n];

		if (i4 == 4u)
		{
			if (!decode4(encoded4, buffer.data() + bufferPosition))
			{
				return false;
			}

			bufferPosition += 3;
			i4 = 0u;
		}
	}

	if (i4 != 0u)
	{
		ocean_assert(i4 == 1u || i4 == 2u || i4 == 3u);

		if (i4 <= 1u)
		{
			encoded4[1] = 0u;
		}

		if (i4 <= 2u)
		{
			encoded4[2] = 0u;
		}

		encoded4[3] = 0u;

		uint8_t buffer3[3];
		if (!decode4(encoded4, buffer3))
		{
			return false;
		}

		ocean_assert(bufferPosition < buffer.size());
		buffer.data()[bufferPosition] = buffer3[0];

		if (i4 >= 3u)
		{
			ocean_assert(bufferPosition + 1 < buffer.size());
			buffer.data()[bufferPosition + 1] = buffer3[1];
		}
	}

	return true;
}

}

}
