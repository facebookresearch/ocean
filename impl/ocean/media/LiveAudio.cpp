/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/LiveAudio.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace Media
{

LiveAudio::LiveAudio(const std::string& url) :
	Medium(url),
	ConfigMedium(url),
	LiveMedium(url),
	SoundMedium(url)
{
	type_ = Type(type_ | LIVE_AUDIO);
}

bool LiveAudio::addSamples(const SampleType /*sampleType*/, const void* /*data*/, const size_t /*size*/)
{
	return false;
}

bool LiveAudio::writeWavFile(const std::string& filename, const SampleType sampleType, const void* data, const size_t size)
{
	ocean_assert(!filename.empty());
	ocean_assert(data != nullptr && size != 0);

	if (size + 44 >= size_t(NumericT<unsigned int>::maxValue()))
	{
		return false;
	}

	const uint32_t size32 = uint32_t(size);
	const uint32_t fileSize = size32 + 44u - 8u;

	constexpr uint32_t lengthFormatData = 16u;

	constexpr uint16_t dataType = 1u; // PCM

	ocean_assert(sampleType == ST_INTEGER_16_MONO_48 || sampleType == ST_INTEGER_16_STEREO_48);
	const uint16_t numberChannels = sampleType == ST_INTEGER_16_MONO_48 ? 1u : 2u;
	constexpr uint16_t bitsPerSample = 16u;

	constexpr uint32_t sampleRate = 48000u;
	const uint32_t byteRate = (sampleRate * uint32_t(bitsPerSample) * numberChannels) / 8u;

	const uint16_t blockAlign = (bitsPerSample * numberChannels) / 8u;

	std::ofstream stream(filename, std::ios::binary);

	if (!stream.good())
	{
		return false;
	}

	stream << "RIFF";
	stream.write((const char*)(&fileSize), sizeof(fileSize));
	stream << "WAVE";

	stream << "fmt ";
	stream.write((const char*)(&lengthFormatData), sizeof(lengthFormatData));
	stream.write((const char*)(&dataType), sizeof(dataType));
	stream.write((const char*)(&numberChannels), sizeof(numberChannels));
	stream.write((const char*)(&sampleRate), sizeof(sampleRate));
	stream.write((const char*)(&byteRate), sizeof(byteRate));
	stream.write((const char*)(&blockAlign), sizeof(blockAlign));
	stream.write((const char*)(&bitsPerSample), sizeof(bitsPerSample));

	stream << "data";
	stream.write((const char*)(&size32), sizeof(size32));
	stream.write((const char*)(data), size);

	return stream.good();
}

}

}
