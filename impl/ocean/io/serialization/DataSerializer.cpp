/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/io/serialization/DataSerializer.h"

namespace Ocean
{

namespace IO
{

namespace Serialization
{

bool DataSerializer::stopAndWait(const double timeout)
{
	if (!stop())
	{
		return false;
	}

	const Timestamp startTimestamp(true);

	while (!startTimestamp.hasTimePassed(timeout))
	{
		if (hasStopped())
		{
			return true;
		}

		sleep(1u);
	}

	stopThreadExplicitly();

	return false;
}

}

}

}
