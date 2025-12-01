/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/io/serialization/DataSample.h"

namespace Ocean
{

namespace IO
{

namespace Serialization
{

bool DataSample::readSample(InputBitstream& inputBitstream)
{
	return inputBitstream.read<double>(playbackTimestamp_) && DataTimestamp::read(inputBitstream, dataTimestamp_);
}

bool DataSample::writeSample(OutputBitstream& outputBitstream) const
{
	ocean_assert(playbackTimestamp_ >= 0.0);
	ocean_assert(dataTimestamp_.isValid());

	return outputBitstream.write<double>(playbackTimestamp_) && DataTimestamp::write(outputBitstream, dataTimestamp_);
}

}

}

}
