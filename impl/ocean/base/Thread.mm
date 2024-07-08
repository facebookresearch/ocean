/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/Thread.h"

#include <Foundation/Foundation.h>

namespace Ocean
{

Thread::ThreadPriority Thread::threadPriority()
{
	const double priority = [NSThread threadPriority];

	if (priority >= 0.95)
		return PRIORTY_REALTIME;

	if (priority >= 0.75)
		return PRIORTY_HIGH;

	if (priority >= 0.6)
		return PRIORTY_ABOVE_NORMAL;

	if (priority >= 0.5)
		return PRIORTY_NORMAL;

	if (priority >= 0.25)
		return PRIORTY_BELOW_NORMAL;

	return PRIORITY_IDLE;
}

bool Thread::setThreadPriority(const ThreadPriority priority)
{
	switch (priority)
	{
		case PRIORITY_IDLE:
			return [NSThread setThreadPriority:0.1];

		case PRIORTY_BELOW_NORMAL:
			return [NSThread setThreadPriority:0.25];

		case PRIORTY_NORMAL:
			return [NSThread setThreadPriority:0.5];

		case PRIORTY_ABOVE_NORMAL:
			return [NSThread setThreadPriority:0.6];

		case PRIORTY_HIGH:
			return [NSThread setThreadPriority:0.75];

		case PRIORTY_REALTIME:
			return [NSThread setThreadPriority:0.95];
	}

	ocean_assert(false && "Unknown priority value.");
	return false;
}

} // namespace Ocean
